//========================================================================\
// more.c                                                                 |
// FUNZIONI: ------------------------------------------------------------ |
// MESSAGGI: ------------------------------------------------------------ |
// WM_CREATE                                                              |
// WM_ADJUSTWINDOWPOS                                                     |
// WM_PAINT                                                               |
// WM_DESTROY                                                             |
//========================================================================/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "common.h"
#include "api.h"
#include "more.h"


//==========================================================================\
// registrazione classe                                                     |
//==========================================================================/

BOOL APIENTRY EDMoreReg(HAB hab) {
   return WinRegisterClass(hab, WC_MOREBOX, (PFNWP)EDMoreWinProc,
                           flWCstc & ~CS_PUBLIC, cbWCstc + 4);
}


//==========================================================================\
// procedura controllo extended listbox                                     |
//==========================================================================/

MRESULT EXPENTRY EDMoreWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   switch (msg) {
      // messaggi ordinari controllo ----------------------------------------
      case WM_CREATE:
         return (MRESULT)!(moreCreateProc(hwnd, (PMORECDATA)mp1,
                                         (PCREATESTRUCT)mp2, SS_TEXT));
      case WM_WINDOWPOSCHANGED:
         if (((PSWP)mp1)->fl & SWP_SIZE) moreUpdateCtlSize(hwnd, (PSWP)mp1);
         break;
      case WM_ENABLE:
         return moreEnable((PMOREBOX)stGetData(hwnd), (BOOL)mp1);
      case WM_CONTROL:
         return moreControl((PMOREBOX)stGetData(hwnd), mp1, (PUSERBUTTON)mp2);
      case WM_COMMAND:
         return moreCommand((PMOREBOX)stGetData(hwnd), (ULONG)mp1);
      case WM_PAINT:
         return morePaint((PMOREBOX)stGetData(hwnd));
      case WM_SETWINDOWPARAMS:
         return moreSetWparms((PMOREBOX)stGetData(hwnd), (PWNDPARAMS)mp1);
      case WM_QUERYWINDOWPARAMS:
         return moreQueryWparms((PMOREBOX)stGetData(hwnd), (PWNDPARAMS)mp1);
      case WM_SYSCOLORCHANGE:
         return moreSCchange((PMOREBOX)stGetData(hwnd));
      case WM_PRESPARAMCHANGED:
         return morePPchange((PMOREBOX)stGetData(hwnd), (ULONG)mp1); 
      case WM_DESTROY:
         moreDestroyProc((PMOREBOX)stGetData(hwnd));
         break;
      case WM_QUERYDLGCODE:
         return (MRESULT)0;
      case WM_MATCHMNEMONIC:
         return moreMnemonic((PMOREBOX)stGetData(hwnd), (ULONG)mp1);
      case WM_BUTTON1DOWN:   
      case WM_BUTTON2DOWN:
         return moreMouBtn((PMOREBOX)stGetData(hwnd));
      case WM_SETFOCUS:
         return moreSetFocus(hwnd, mp1, (BOOL)mp2);
      case WM_DELAYFOCUS:
         return moreDelayFocus((PMOREBOX)stGetData(hwnd));
      // messaggi rediretti alla checkbox ---------------------------------
      case BM_SETCHECK:
         return moreSetCheck((PMOREBOX)stGetData(hwnd), (ULONG)mp1);
      case BM_QUERYCHECK:
         return moreQueryCheck((PMOREBOX)stGetData(hwnd));
      // messaggi rediretti all'entryfield/combobox -----------------------
      case EM_SETTEXTLIMIT:
         return moreEFtxtlimit((PMOREBOX)stGetData(hwnd), (ULONG)mp1);
   } /* endswitch */
   return pfnwpWCstc(hwnd, msg, mp1, mp2); // default static controls proc
}


//==========================================================================\
// procedura creazione controllo extended listbox
// - viene allocata memoria per i dati del controllo                        |
// - vengono impostati i colori                                             |
// - viene misurata l'altezza del font usato                                |
// - viene resettato lo stile del controllo di default                      |
// - viene resettato il testo del controllo di default                      |
// - l'indirizzo della struttura Š memorizzato nelle window words           |
//==========================================================================/

BOOL moreCreateProc(HWND hwnd, PMORECDATA pmrcd, PCREATESTRUCT pc, ULONG style) {
   PMOREBOX pmore;
   HPS hps;
   // allocate structure for caching some control data
   if (!(pmore = (PMOREBOX)malloc(sizeof(MOREBOX)))) return FALSE;
   memset((PVOID)pmore, 0, sizeof(MOREBOX));
   // copy some data
   pmore->hab = WinQueryAnchorBlock(hwnd);
   pmore->hwnd = hwnd;
   pmore->fl = pc->flStyle;
   pmore->szl.cx = pc->cx;
   pmore->szl.cy = pc->cy;
   pmore->id = pc->id;
   // imposta colori controllo
   if (!(hps = WinGetPS(hwnd))) goto error0;
   moreUpdColors(pmore, hps);
   WinReleasePS(hps);
   // interpreta controldata
   if (!moreSetCtlData(pmore, pmrcd)) goto error0;
   if (moreSetText(pmore, pc->pszText, -1)) *pc->pszText = 0;
   // reset WC_STATIC style
   WinSetWindowBits(hwnd, QWL_STYLE, style, 0xffff);
   WinSetWindowPtr(hwnd, cbWCstc, (PVOID)pmore);
   if (pfnwpWCstc(hwnd, WM_CREATE, (MPARAM)pmrcd, (MPARAM)pc)) goto error1;
   // se non Š stile MORE_CHECK setta flag checkon sempre a 1
   if (!(pmore->fl & MORE_CHECK)) pmore->is.chkon = 1;
   if (!moreChilds(pmore, pc)) goto error2;
   if (!(pmore->fl & WS_DISABLED)) pmore->is.enbl = 1;
   return TRUE;

   error2:
      WinDestroyWindow(hwnd);
   error1:
      if (pmore->psz) free(pmore->psz);
   error0:
      free(pmore);
      return FALSE;
}


//==========================================================================\
// ridimensiona listbox e ricalcola larghezza colonne                       |
//==========================================================================/

VOID moreUpdateCtlSize(HWND hwnd, PSWP pswp) {
   PMOREBOX pmore = (PMOREBOX)stGetData(hwnd);
   if (pmore) {
      pmore->szl.cx = pswp->cx;
      pmore->szl.cy = pswp->cy;
   } // end if
   moreSize(pmore);
   moreMove(pmore);
}


//==========================================================================\
// quando cambia lo stato di abilitazione del controllo abilita/disabilita  |
// componenti controllo                                                     |
//==========================================================================/

MRESULT moreEnable(PMOREBOX pmore, BOOL fl) {
   if (pmore) {
      pmore->is.enbl = fl? 1: 0;
      WinEnableWindow(pmore->hswap, fl & pmore->is.chkon);
      WinEnableWindow(pmore->hfind, fl & pmore->is.chkon);
      WinRefreshWin(pmore->hwnd);
      if (pmore->hcpt) WinRefreshWin(pmore->hcpt);
   } /* endif */
   return (MRESULT)FALSE;
}


//==========================================================================\
// processa messaggi notifica:                                              |
// - se stile MORE_CHECK click su checkbox                                  |
// - selezione item listbox per abilitazione bottone remove                 |
// - LN_ENTER in listbox per rimozione o editazione item                    |
// - cambiamento testo nell'entry per abilitazione/disabilitazione btn add  |
// - BN_PAINT per disegnare bottoni                                         |
//==========================================================================/

MRESULT moreControl(PMOREBOX pmore, MPARAM mp1, PUSERBUTTON pub) {
   SHORT item;
   if (SHORT1FROMMP(mp1) == FID_CAPTION) {
      // click sulla checkbox cambia stato abilitazione altri componenti
      if ((pmore->fl & MORE_CHECK) &&
          (SHORT2FROMMP(mp1) == BN_CLICKED ||
           SHORT2FROMMP(mp1) == BN_DBLCLICKED)) {
         pmore->is.chkon = ((BOOL)pmore->pfbtn(pmore->hcpt, BM_QUERYCHECK,
                                               MPVOID, MPVOID))? 1: 0;
         if (pmore->hswap)
            WinEnableWindow(pmore->hswap, pmore->is.chkon);
         WinEnableWindow(pmore->hfind, pmore->is.chkon);
         WinSendMsg(WinQueryWindow(pmore->hwnd, QW_OWNER), WM_CONTROL,
                    MPFROM2SHORT(pmore->id, MOREN_CHECK),
                    (MPARAM)pmore->is.chkon);
      // cambiamento testo entryfield
      } else if (pmore->fl & MORE_ENTRY) {
         USHORT usn = 0;
         switch (SHORT2FROMMP(mp1)) {
            case EN_SETFOCUS:         usn = MOREN_EFSETFCS;  break;
            case EN_KILLFOCUS:        usn = MOREN_EFLOSEFCS; break;
            case EN_CHANGE:           usn = MOREN_EFCHANGE;  break;
            case EN_SCROLL:           usn = MOREN_EFSCROLL;  break;
            case EN_MEMERROR:         usn = MOREN_EFMEMERR;  break;
            case EN_OVERFLOW:         usn = MOREN_EFOVFLW;   break;
            case EN_INSERTMODETOGGLE: usn = MOREN_EFINSTGL;  break;
         } // end switch
         if (usn)
            WinSendMsg(WinQueryWindow(pmore->hwnd, QW_OWNER), WM_CONTROL,
                       MPFROM2SHORT(pmore->id, usn), (MPARAM)pmore->hwnd);
      } /* endif */
   // disegna bottone swap
   } else if (SHORT1FROMMP(mp1) == FID_SWAP &&
              SHORT2FROMMP(mp1) == BN_PAINT) {
      WdrawBtnBkgnd(pub, pmore->csdbtn, pmore->csdbtn, pmore->lbkgnd);
      drawSwap(pub, pmore->csdbtn);
      if (pub->fsState & BDS_DISABLED)
         drawDisBtn(pub, pmore->csdbtn, pmore->lbkgnd);
   // disegna bottone find
   } else if (SHORT1FROMMP(mp1) == FID_FIND &&
              SHORT2FROMMP(mp1) == BN_PAINT) {
      WdrawBtnBkgnd(pub, pmore->csdbtn, pmore->csdbtn, pmore->lbkgnd);
      drawFind(pub, pmore->csdbtn);
      if (pub->fsState & BDS_DISABLED)
         drawDisBtn(pub, pmore->csdbtn, pmore->lbkgnd);
   } /* endswitch */
   return (MRESULT)FALSE;
}


//==========================================================================\
// quando riceve un WM_COMMAND da uno dei bottoni spedisce un WM_CONTROL    |
// con gli opportuni parametri alla owner window                            |
// Nel caso di button add o remove si controlla il valore restituito da     |
// WinSendMsg() per decidere se eseguire o meno la procedura di default     |
// del relativo bottone                                                     |
//==========================================================================/

MRESULT moreCommand(PMOREBOX pmore, ULONG id) {
   WinSendMsg(WinQueryWindow(pmore->hwnd, QW_OWNER), WM_CONTROL,
            MPFROM2SHORT(pmore->id, (id == FID_SWAP? MOREN_SWAP: MOREN_FIND)),
            (MPARAM)pmore->hwnd);
   return (MRESULT)FALSE;
}


//==========================================================================\
// disegna le descrizioni delle colonne                                     |
//==========================================================================/

MRESULT morePaint(PMOREBOX pmore) {
   HPS hps;
   if (pmore &&
       NULLHANDLE != (hps = WinBeginPaint(pmore->hwnd, NULLHANDLE, NULL))) {
      if (!(pmore->fl & (MORE_CHECK | MORE_ENTRY))) {
         ULONG fl = DT_VCENTER | DT_ERASERECT | DT_MNEMONIC;
         GpiCreateLogColorTable(hps, 0, LCOLF_RGB, 0, 0, NULL);
         WinDrawText(hps, -1, pmore->psz, &pmore->rclcpt, pmore->lfgnd,
                     pmore->lbkgnd, (pmore->is.enbl? fl: fl | DT_HALFTONE));
      } /* endif */
      WinEndPaint(hps);
   } /* endif */
   return (MRESULT)FALSE;
}


//==========================================================================\
// la procedura risponde all'evento cambiando il testo e gli altri dati     |
// della finestra come necessario                                           |
// restituisce TRUE/FALSE = successo/errore                                 |
//==========================================================================/

MRESULT moreSetWparms(PMOREBOX pmore, PWNDPARAMS pwpm) {
   MRESULT mr = (MRESULT)TRUE;
   if (pwpm->fsStatus & WPM_TEXT) {
      moreSetText(pmore, pwpm->pszText, pwpm->cchText);
   } /* endif */
   if (pwpm->fsStatus & WPM_CTLDATA) {
      mr = (MRESULT)moreSetCtlData(pmore, pwpm->pCtlData);
      if (pmore->fl & MORE_ENTRY)
         pmore->pfcpt(pmore->hcpt, EM_SETTEXTLIMIT,
                      (MPARAM)pmore->cchef, MPVOID);
   } /* endif */
   WinRefreshWin(pmore->hwnd);
   return mr;
}


//==========================================================================\
//==========================================================================/

MRESULT moreQueryWparms(PMOREBOX pmore, PWNDPARAMS pwpm) {
   // parametri presentazione: usa procedura default
   if (isOrdWndParm(pwpm->fsStatus))
      return(WinDefWindowProc(pmore->hwnd, WM_QUERYWINDOWPARAMS,
                              (MPARAM)pwpm, MPVOID));
   // testo finestra
   if (pwpm->fsStatus & WPM_TEXT) {
      if (pmore->hcpt) {
         WinQueryWindowText(pmore->hcpt, pwpm->cchText, pwpm->pszText);
      } else if (pmore->cbtxt) {
         memcpy((PVOID)pwpm->pszText, (PVOID)pmore->psz,
                min(pwpm->cchText, pmore->cbtxt) + 1);
      } else {
         if (pwpm->cchText) *(pwpm->pszText) = '\0';
      } /* endif */
   } /* endif */
   // lunghezza testo finestra
   if (pwpm->fsStatus & WPM_CCHTEXT) {
      pwpm->cchText = (pmore->hcpt)?
                       WinQueryWindowTextLength(pmore->hcpt): pmore->cbtxt;
   } /* endif */
   if (pwpm->fsStatus & WPM_CTLDATA) {
      if (pwpm->cbCtlData < sizeof(MORECDATA)) return (MRESULT)FALSE;
      ((PMORECDATA)pwpm->pCtlData)->cb =  sizeof(MORECDATA);
      ((PMORECDATA)pwpm->pCtlData)->cch = pmore->cchef;
   } /* endif */
   if (pwpm->fsStatus & WPM_CBCTLDATA) {
      pwpm->cbCtlData = sizeof(MORECDATA);
   } /* endif */
   return (MRESULT)TRUE;
}


//==========================================================================\
// in seguito a WM_SYSCOLORCHANGE reimposta tutti i colori del controllo    |
// e lo ridisegna                                                           |
//==========================================================================/

MRESULT moreSCchange(PMOREBOX pmore) {
   HPS hps;
   if (pmore && NULLHANDLE != (hps = WinGetPS(pmore->hwnd))) {
      moreUpdColors(pmore, hps);
      WinReleasePS(hps);
      WinRefreshWin(pmore->hwnd);
   } /* endif */
   return (MRESULT)FALSE;
}


//==========================================================================\
// in seguito a WM_PRESPARAMCHANGED reimposta il colore cambiato e          |
// ridisegna il controllo                                                   |
//==========================================================================/

MRESULT morePPchange(PMOREBOX pmore, ULONG fl) {
   HPS hps;
   if (pmore && NULLHANDLE != (hps = WinGetPS(pmore->hwnd))) {
      switch (fl) {
         case PP_FOREGROUNDCOLOR:
         case PP_FOREGROUNDCOLORINDEX:
            pmore->lfgnd = WgetCtlClr(pmore->hwnd, hps, PP_FOREGROUNDCOLOR,
                         PP_FOREGROUNDCOLORINDEX, SYSCLR_WINDOWSTATICTEXT);
            break;
         case PP_BACKGROUNDCOLOR:
         case PP_BACKGROUNDCOLORINDEX:
            pmore->lbkgnd = WgetCtlClr(pmore->hwnd, hps, PP_BACKGROUNDCOLOR,
                         PP_BACKGROUNDCOLORINDEX, SYSCLR_DIALOGBACKGROUND);
            break;
         case PP_FONTNAMESIZE:
            break;
      } /* endswitch */
      WinReleasePS(pmore->hwnd);
      WinRefreshWin(pmore->hwnd);
   }
   return (MRESULT)FALSE;
}


//==========================================================================\
// libera le risorse allocate per il controllo                              |
//==========================================================================/

VOID moreDestroyProc(PMOREBOX pmore) {
   if (pmore) {
      if (pmore->hcpt) WinDestroyWindow(pmore->hcpt);
      if (pmore->hswap) WinDestroyWindow(pmore->hswap);
      WinDestroyWindow(pmore->hfind);
      free(pmore->psz);
      free(pmore);
      _heapmin();
   } /* endif */
}


//==========================================================================\
// restituisce TRUE se il carattere corrisponde al mnemonico del controllo  |
//==========================================================================/

MRESULT moreMnemonic(PMOREBOX pmore, ULONG ch) {
   HWND hfocus;
   if (ch > 0xff) {
      if (WinIsChild((hfocus = WinQueryFocus(HWND_DESKTOP)), pmore->hwnd)) {
         if (ch == 0x9d00 &&        // -> simula comando bottone FIND
             pmore->is.chkon) {
            WinSendMsg(WinQueryWindow(pmore->hwnd, QW_OWNER), WM_CONTROL,
                    MPFROM2SHORT(pmore->id, MOREN_FIND), (MPARAM)pmore->hwnd);
         } else if (ch == 0xa000 && // -> simula comando bottone SWAP
                    pmore->hswap &&
                    pmore->is.chkon) {
            WinSendMsg(WinQueryWindow(pmore->hwnd, QW_OWNER), WM_CONTROL,
                    MPFROM2SHORT(pmore->id, MOREN_SWAP), (MPARAM)pmore->hwnd);
         } /* endif */
      } /* endif */
      return (MRESULT)FALSE;
   } /* endif */
   if (pmore->fl & MORE_ENTRY) return (MRESULT)FALSE;
   if (pmore->fl & MORE_CHECK) {
      if (pmore->pfbtn(pmore->hcpt, WM_MATCHMNEMONIC, (MPARAM)ch, MPVOID))
         WinSetFocus(HWND_DESKTOP, pmore->hcpt);
      return (MRESULT)FALSE;
   } else if (pmore->psz) {
      PSZ pmnemo = strchr(pmore->psz, '~');
      if (!pmnemo) return (MRESULT)FALSE;
      ch = WinUpperChar(pmore->hab, 0, 0, ch);
      return (MRESULT)(ch == WinUpperChar(pmore->hab, 0, 0, *++pmnemo) && ch);
   } else {
      return (MRESULT)FALSE;
   } /* endif */
}


//==========================================================================\
// da focus al prossimo bottone se il mouse viene premuto sulla caption     |
//==========================================================================/

MRESULT moreMouBtn(PMOREBOX pmore) {
   if (pmore->hcpt) return (MRESULT)FALSE;
   WinSetFocus(HWND_DESKTOP, (pmore->hswap? pmore->hswap: pmore->hfind));
   return (MRESULT)TRUE;
}


//==========================================================================\
// posta messaggio per dare focus al prossimo bottone                       |
//==========================================================================/

MRESULT moreSetFocus(HWND hwnd, MPARAM mp1, BOOL fl) {
   if (fl) WinPostMsg(hwnd, WM_DELAYFOCUS, (MPARAM)hwnd, MPVOID);
   return WinDefWindowProc(hwnd, WM_SETFOCUS, mp1, (MPARAM)fl);
}


//==========================================================================\
// da il focus al prossimo bottone                                          |
//==========================================================================/

MRESULT moreDelayFocus(PMOREBOX pmore) {
   WinSetFocus(HWND_DESKTOP, (pmore->hcpt? pmore->hcpt: pmore->hfind));
   return (MRESULT)FALSE;
}


//==========================================================================\
// cambia stato checkbox                                                    |
//==========================================================================/

MRESULT moreSetCheck(PMOREBOX pmore, BOOL fl) {
   MRESULT mr;
   if (!(pmore->fl & MORE_CHECK)) return (MRESULT)0;
   pmore->is.chkon = (fl != 0);
   if (pmore->hswap)
      WinEnableWindow(pmore->hswap, pmore->is.chkon);
   WinEnableWindow(pmore->hfind, pmore->is.chkon);
   mr = pmore->pfbtn(pmore->hcpt, BM_SETCHECK, (MPARAM)pmore->is.chkon, MPVOID);
   // corregge problema ridisegno checkbox
   if (!pmore->is.enbl)
      paintDisabledCheck(pmore->hcpt, &pmore->rclcpt, pmore->lbkgnd); 
   return mr;
}


//==========================================================================\
// riporta stato checkbox                                                   |
//==========================================================================/

MRESULT moreQueryCheck(PMOREBOX pmore) {
   return (MRESULT)pmore->is.chkon;
}


//==========================================================================\
// aggiorna massima quantit… caratteri ammessa nel componente entryfield    |
//==========================================================================/

MRESULT moreEFtxtlimit(PMOREBOX pmore, ULONG cb) {
   if (cb > 0x7fff || !(pmore->fl & MORE_ENTRY)) return (MRESULT)FALSE;
   pmore->cchef = cb;
   return pmore->pfcpt(pmore->hcpt, EM_SETTEXTLIMIT, (MPARAM)cb, MPVOID);
}
