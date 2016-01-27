//========================================================================\
// clbx.c                                                                 |
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
#include "clbox.h"


//==========================================================================\
// registrazione classe                                                     |
//==========================================================================/

BOOL APIENTRY EDClbReg(HAB hab) {
   return WinRegisterClass(hab, WC_COMPLISTBOX, (PFNWP)EDClbWinProc,
                           flWCstc & ~CS_PUBLIC, cbWCstc + 4);
}


//==========================================================================\
// procedura controllo extended listbox                                     |
//==========================================================================/

MRESULT EXPENTRY EDClbWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   switch (msg) {
      // messaggi ordinari controllo ----------------------------------------
      case WM_CREATE:
         return (MRESULT)!(clbCreateProc(hwnd, (PCLBOXCDATA)mp1,
                                         (PCREATESTRUCT)mp2, SS_TEXT));
      case WM_WINDOWPOSCHANGED:
         if (((PSWP)mp1)->fl & SWP_SIZE) clbUpdateCtlSize(hwnd, (PSWP)mp1);
         break;
      case WM_ENABLE:
         return clbEnable((PCLBOX)stGetData(hwnd), (BOOL)mp1);
      case WM_CONTROL:
         return clbControl((PCLBOX)stGetData(hwnd), mp1, (PUSERBUTTON)mp2);
      case WM_COMMAND:
         return clbCommand((PCLBOX)stGetData(hwnd), (ULONG)mp1);
      case WM_PAINT:
         return clbPaint((PCLBOX)stGetData(hwnd));
      case WM_SETWINDOWPARAMS:
         return clbSetWparms((PCLBOX)stGetData(hwnd), (PWNDPARAMS)mp1);
      case WM_QUERYWINDOWPARAMS:
         return clbQueryWparms((PCLBOX)stGetData(hwnd), (PWNDPARAMS)mp1);
      case WM_SYSCOLORCHANGE:
         return clbSCchange((PCLBOX)stGetData(hwnd));
      case WM_PRESPARAMCHANGED:
         return clbPPchange((PCLBOX)stGetData(hwnd), (ULONG)mp1); 
      case WM_DESTROY:
         clbDestroyProc((PCLBOX)stGetData(hwnd));
         break;
      case WM_QUERYDLGCODE:
         return (MRESULT)0;
      case WM_MATCHMNEMONIC:
         return clbMnemonic((PCLBOX)stGetData(hwnd), (ULONG)mp1);
      case WM_BUTTON1DOWN:   
      case WM_BUTTON2DOWN:
         return clbMouBtn((PCLBOX)stGetData(hwnd), MOUSEMSG(&msg));
      case WM_SETFOCUS:
         return clbSetFocus(hwnd, mp1, (BOOL)mp2);
      case WM_DELAYFOCUS:
         return clbDelayFocus((PCLBOX)stGetData(hwnd));
      case WM_MEASUREITEM:
      case WM_DRAWITEM:
         return WinSendMsg(WinQueryWindow(hwnd, QW_OWNER), msg, mp1, mp2);
      // messaggi rediretti alla checkbox ---------------------------------
      case BM_SETCHECK:
         return clbSetCheck((PCLBOX)stGetData(hwnd), (ULONG)mp1);
      case BM_QUERYCHECK:
         return clbQueryCheck((PCLBOX)stGetData(hwnd));
      case CCLM_SETENTRYTEXT:
         return clbSetEntryText((PCLBOX)stGetData(hwnd), (PSZ)mp1);
      // messaggi ridiretti alla listbox ----------------------------------
      case LM_DELETEALL:
      case LM_INSERTMULTITEMS:
      case LM_INSERTITEM:
      case LM_DELETEITEM:
      case LM_QUERYITEMCOUNT:
      case LM_SETTOPINDEX:
      case LM_SELECTITEM:
      case LM_QUERYSELECTION:
      case LM_SETITEMTEXT:
      case LM_QUERYITEMTEXTLENGTH:
      case LM_QUERYITEMTEXT:
      case LM_SETITEMHANDLE:
      case LM_QUERYITEMHANDLE:
      case LM_SEARCHSTRING:
      case LM_SETITEMHEIGHT:
      case LM_QUERYTOPINDEX:
      case LM_SETITEMWIDTH: {
         PCLBOX pclb = (PCLBOX)stGetData(hwnd);
         return pclb->pflbx(pclb->hlbx, msg, mp1, mp2);
      } // end redirection listbox messages
      // messaggi rediretti all'entryfield/combobox -----------------------
      case EM_SETTEXTLIMIT:
         return clbEFtxtlimit((PCLBOX)stGetData(hwnd), (ULONG)mp1);
      case CCLM_QUERYITEMCOUNT:
      case CCLM_INSERTITEM:
      case CCLM_SETTOPINDEX:
      case CCLM_DELETEITEM:
      case CCLM_SELECTITEM:
      case CCLM_QUERYSELECTION:
      case CCLM_SETITEMTEXT:
      case CCLM_QUERYITEMTEXTLENGTH:
      case CCLM_QUERYITEMTEXT:
      case CCLM_SETITEMHANDLE:
      case CCLM_QUERYITEMHANDLE:
      case CCLM_SEARCHSTRING:
      case CCLM_SETITEMHEIGHT:
      case CCLM_QUERYTOPINDEX:
      case CCLM_DELETEALL:
      case CCLM_INSERTMULTITEMS:
      case CCLM_SETITEMWIDTH: {
         PCLBOX pclb = (PCLBOX)stGetData(hwnd);
         return pclb->pfef(pclb->hentry, msg -  0x2000, mp1, mp2);
      } // end redirection combobox messages
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

BOOL clbCreateProc(HWND hwnd, PCLBOXCDATA pclcd, PCREATESTRUCT pc, ULONG style) {
   PCLBOX pclb;
   HPS hps;
   // allocate structure for caching some control data
   if (!(pclb = (PCLBOX)malloc(sizeof(CLBOX)))) return FALSE;
   memset((PVOID)pclb, 0, sizeof(CLBOX));
   // copy some data
   pclb->hab = WinQueryAnchorBlock(hwnd);
   pclb->hwnd = hwnd;
   pclb->fl = pc->flStyle;
   pclb->szl.cx = pc->cx;
   pclb->szl.cy = pc->cy;
   pclb->id = pc->id;
   pclb->hOwner = pc->hwndOwner;
   // imposta colori controllo
   if (!(hps = WinGetPS(hwnd))) goto error0;
   clbUpdColors(pclb, hps);
   if ((pclb->cyFont = stSizeFont(hps)) % 2) ++pclb->cyFont;
   WinReleasePS(hps);
   // interpreta controldata
   if (!clbSetCtlData(pclb, pclcd)) goto error0;
   if (clbSetText(pclb, pc->pszText, -1)) *pc->pszText = 0;
   // reset WC_STATIC style
   WinSetWindowBits(hwnd, QWL_STYLE, style, 0xffff);
   WinSetWindowPtr(hwnd, cbWCstc, (PVOID)pclb);
   if (pfnwpWCstc(hwnd, WM_CREATE, (MPARAM)pclcd, (MPARAM)pc)) goto error1;
   // se non Š stile CLBXS_CHECK setta flag checkon sempre a 1
   if (!(pclb->fl & CLBXS_CHECK)) pclb->is.chkon = 1;
   if (!clbChilds(pclb, pc)) goto error2;
   if (!(pclb->fl & WS_DISABLED)) pclb->is.enbl = 1;
   return TRUE;

   error2:
      WinDestroyWindow(hwnd);
   error1:
      if (pclb->psz) free(pclb->psz);
   error0:
      free(pclb);
      return FALSE;
}


//==========================================================================\
// ridimensiona listbox e ricalcola larghezza colonne                       |
//==========================================================================/

VOID clbUpdateCtlSize(HWND hwnd, PSWP pswp) {
   PCLBOX pclb = (PCLBOX)stGetData(hwnd);
   if (pclb) {
      pclb->szl.cx = pswp->cx;
      pclb->szl.cy = pswp->cy;
   } // end if
   clbSize(pclb);
   clbMove(pclb);
}


//==========================================================================\
// quando cambia lo stato di abilitazione del controllo abilita/disabilita  |
// componenti controllo                                                     |
//==========================================================================/

MRESULT clbEnable(PCLBOX pclb, BOOL fl) {
   if (pclb) {
      pclb->is.enbl = fl? 1: 0;
      WinEnableWindow(pclb->hlbx, fl & pclb->is.chkon);
      WinEnableWindow(pclb->hentry, fl & pclb->is.chkon);
      WinEnableWindow(pclb->hadd, fl & pclb->is.chkon & pclb->is.addon);
      WinEnableWindow(pclb->hrem, fl & pclb->is.chkon & pclb->is.remon);
      WinEnableWindow(pclb->hfind, fl & pclb->is.chkon);
      WinRefreshWin(pclb->hwnd);
      if (pclb->hchk) WinRefreshWin(pclb->hchk);
   } /* endif */
   return (MRESULT)FALSE;
}


//==========================================================================\
// processa messaggi notifica:                                              |
// - se stile CLBXS_CHECK click su checkbox                                 |
// - selezione item listbox per abilitazione bottone remove                 |
// - LN_ENTER in listbox per rimozione o editazione item                    |
// - cambiamento testo nell'entry per abilitazione/disabilitazione btn add  |
// - BN_PAINT per disegnare bottoni                                         |
//==========================================================================/

MRESULT clbControl(PCLBOX pclb, MPARAM mp1, PUSERBUTTON pub) {
   switch (SHORT1FROMMP(mp1)) {
      case FID_CAPTION:
         // click sulla checkbox cambia stato abilitazione altri componenti
         if ((pclb->fl & CLBXS_CHECK) &&
             (SHORT2FROMMP(mp1) == BN_CLICKED ||
              SHORT2FROMMP(mp1) == BN_DBLCLICKED)) {
            pclb->is.chkon = ((BOOL)pclb->pfbtn(pclb->hchk, BM_QUERYCHECK,
                                                MPVOID, MPVOID))? 1: 0;
            WinEnableWindow(pclb->hlbx, pclb->is.chkon);
            WinEnableWindow(pclb->hentry, pclb->is.chkon);
            WinEnableWindow(pclb->hadd, pclb->is.chkon & pclb->is.addon);
            WinEnableWindow(pclb->hrem, pclb->is.chkon & pclb->is.remon);
            WinEnableWindow(pclb->hfind, pclb->is.chkon);
            WinSendMsg(WinQueryWindow(pclb->hwnd, QW_OWNER), WM_CONTROL,
                 MPFROM2SHORT(pclb->id, CLBXN_CHECK), (MPARAM)pclb->is.chkon);
         } /* endif */
         break;
      case FID_LIST:
         clbControlLboxMsg(pclb, SHORT2FROMMP(mp1));
         break;
      case FID_ENTRY:
         clbControlEntryMsg(pclb, SHORT2FROMMP(mp1)); 
         break;
      case FID_ADD:
         if (SHORT2FROMMP(mp1) == BN_PAINT) {
            WdrawBtnBkgnd(pub, pclb->csdbtn, pclb->csdbtn, pclb->lbkgnd);
            drawAdd(pub, pclb->csdbtn);
            if (pub->fsState & BDS_DISABLED)
               drawDisBtn(pub, pclb->csdbtn, pclb->lbkgnd);
         } /* endif */
         break;
      case FID_REMOVE:
         if (SHORT2FROMMP(mp1) == BN_PAINT) {
            WdrawBtnBkgnd(pub, pclb->csdbtn, pclb->csdbtn, pclb->lbkgnd);
            drawRem(pub, pclb->csdbtn);
            if (pub->fsState & BDS_DISABLED)
               drawDisBtn(pub, pclb->csdbtn, pclb->lbkgnd);
         } /* endif */
         break;
      case FID_FIND:
         if (SHORT2FROMMP(mp1) == BN_PAINT) {
            WdrawBtnBkgnd(pub, pclb->csdbtn, pclb->csdbtn, pclb->lbkgnd);
            drawFind(pub, pclb->csdbtn);
            if (pub->fsState & BDS_DISABLED)
               drawDisBtn(pub, pclb->csdbtn, pclb->lbkgnd);
         } /* endif */
         break;
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

MRESULT clbCommand(PCLBOX pclb, ULONG id) {
   HWND hOwner = WinQueryWindow(pclb->hwnd, QW_OWNER);
   SHORT i = 0;
   PSZ psz;
   switch (id) {
      case FID_ADD:
         // alloca buffer per testo entryfield
         if (!(psz = (PSZ)malloc(pclb->cchef + 1))) break;
         WinQueryWindowText(pclb->hentry, pclb->cchef + 1, psz);
         if (!WinSendMsg(hOwner, WM_CONTROL,
                         MPFROM2SHORT(pclb->id, CLBXN_ADD), (MPARAM)psz)) {
            SHORT ins;
            // controlla che la stringa non sia gi… presente
            if (pclb->fl & CLBXS_NODUP &&
                LIT_NONE != (SHORT)pclb->pflbx(pclb->hlbx, LM_SEARCHSTRING,
                                               MPFROM2SHORT(0, LIT_FIRST),
                                               (MPARAM)psz)) {
               free(psz);                         
               break;
            } // end if
            // se stile lo richiede converte stringa in uppercase
            if (pclb->fl & CLBXS_INSUPPER) WinUpper(pclb->hab, 0, 0, psz);
            // setta flag inserzione secondo stile
            if (pclb->fl & CLBXS_INSSORTUP) i = LIT_SORTASCENDING;
            else if (pclb->fl & CLBXS_INSSORTDN) i = LIT_SORTDESCENDING;
            else if (pclb->fl & CLBXS_INSEND) i = LIT_END;
            ins = (SHORT)pclb->pflbx(pclb->hlbx, LM_INSERTITEM,
                                     (MPARAM)i, (MPARAM)psz);
            WinSetWindowText(pclb->hentry, "");
            WinSendMsg(hOwner, WM_CONTROL,
                       MPFROM2SHORT(pclb->id, CLBXN_ADDED), (MPARAM)ins);
         } // end if
         free(psz);
         break;
      case FID_REMOVE:
         i = (SHORT)pclb->pflbx(pclb->hlbx, LM_QUERYSELECTION,
                                (MPARAM)LIT_FIRST, MPVOID);
         if (!WinSendMsg(hOwner, WM_CONTROL, MPFROM2SHORT(pclb->id, CLBXN_REM),
                         (MPARAM)i)) {
            while (i >= 0) {
               pclb->pflbx(pclb->hlbx, LM_DELETEITEM, (MPARAM)i, MPVOID);
               i = (SHORT)pclb->pflbx(pclb->hlbx, LM_QUERYSELECTION,
                                      (MPARAM)LIT_FIRST, MPVOID);
            } /* end while */
         } /* end if */
         pclb->is.remon = 0;
         WinEnableWindow(pclb->hrem, FALSE);
         break;
      case FID_FIND:
         WinSendMsg(hOwner, WM_CONTROL, MPFROM2SHORT(pclb->id, CLBXN_FIND),
                    (MPARAM)pclb->hwnd);
         break;
   } /* endswitch */
   return (MRESULT)FALSE;
}


//==========================================================================\
// disegna le descrizioni delle colonne                                     |
//==========================================================================/

MRESULT clbPaint(PCLBOX pclb) {
   HPS hps;
   if (pclb &&
       NULLHANDLE != (hps = WinBeginPaint(pclb->hwnd, NULLHANDLE, NULL))) {
      if (!(pclb->fl & (CLBXS_CHECK | CLBXS_NOCAPT))) {
         ULONG fl = DT_VCENTER | DT_ERASERECT | DT_MNEMONIC;
         GpiCreateLogColorTable(hps, 0, LCOLF_RGB, 0, 0, NULL);
         WinDrawText(hps, -1, pclb->psz, &pclb->rclcpt, pclb->lfgnd,
                     pclb->lbkgnd, (pclb->is.enbl? fl: fl | DT_HALFTONE));
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

MRESULT clbSetWparms(PCLBOX pclb, PWNDPARAMS pwpm) {
   MRESULT mr = (MRESULT)TRUE;
   if (pwpm->fsStatus & WPM_TEXT) {
      clbSetText(pclb, pwpm->pszText, pwpm->cchText);
   } /* endif */
   if (pwpm->fsStatus & WPM_CTLDATA) {
      mr = (MRESULT)clbSetCtlData(pclb, pwpm->pCtlData);
      pclb->pfef(pclb->hentry, EM_SETTEXTLIMIT, (MPARAM)pclb->cchef, MPVOID);
   } /* endif */
   WinRefreshWin(pclb->hwnd);
   return mr;
}


//==========================================================================\
// Registra la classe del controllo                                         |
// parametri: ------------------------------------------------------------- |
// HAB hab                                                                  |
// valore restituito: ----------------------------------------------------- |
// BOOL : TRUE/FALSE (successo/errore)                                      |
//==========================================================================/

MRESULT clbQueryWparms(PCLBOX pclb, PWNDPARAMS pwpm) {
   // parametri presentazione: usa procedura default
   if (isOrdWndParm(pwpm->fsStatus))
      return(WinDefWindowProc(pclb->hwnd, WM_QUERYWINDOWPARAMS,
                              (MPARAM)pwpm, MPVOID));
   // testo finestra
   if (pwpm->fsStatus & WPM_TEXT) {
      if (pclb->fl & CLBXS_CHECK && pclb->hchk) {
         WinQueryWindowText(pclb->hchk, pwpm->cchText, pwpm->pszText);
      } else if (pclb->cbtxt) {
         memcpy((PVOID)pwpm->pszText, (PVOID)pclb->psz,
                min(pwpm->cchText, pclb->cbtxt) + 1);
      } else {
         if (pwpm->cchText) *(pwpm->pszText) = '\0';
      } /* endif */
   } /* endif */
   // lunghezza testo finestra
   if (pwpm->fsStatus & WPM_CCHTEXT) {
      if (pclb->fl & CLBXS_CHECK && pclb->hchk)
         pwpm->cchText = (pclb->fl & CLBXS_CHECK && pclb->hchk)?
                         WinQueryWindowTextLength(pclb->hchk): pclb->cbtxt;
   } /* endif */
   if (pwpm->fsStatus & WPM_CTLDATA) {
      if (pwpm->cbCtlData < sizeof(CLBOXCDATA)) return (MRESULT)FALSE;
      ((PCLBOXCDATA)pwpm->pCtlData)->cb =  sizeof(CLBOXCDATA);
      ((PCLBOXCDATA)pwpm->pCtlData)->cch = pclb->cchef;
   } /* endif */
   if (pwpm->fsStatus & WPM_CBCTLDATA) {
      pwpm->cbCtlData = sizeof(CLBOXCDATA);
   } /* endif */
   return (MRESULT)TRUE;
}


//==========================================================================\
// in seguito a WM_SYSCOLORCHANGE reimposta tutti i colori del controllo    |
// e lo ridisegna                                                           |
//==========================================================================/

MRESULT clbSCchange(PCLBOX pclb) {
   HPS hps;
   if (pclb && NULLHANDLE != (hps = WinGetPS(pclb->hwnd))) {
      clbUpdColors(pclb, hps);
      WinReleasePS(hps);
      WinRefreshWin(pclb->hwnd);
   } /* endif */
   return (MRESULT)FALSE;
}


//==========================================================================\
// in seguito a WM_PRESPARAMCHANGED reimposta il colore cambiato e          |
// ridisegna il controllo                                                   |
//==========================================================================/

MRESULT clbPPchange(PCLBOX pclb, ULONG fl) {
   HPS hps;
   if (pclb && NULLHANDLE != (hps = WinGetPS(pclb->hwnd))) {
      switch (fl) {
         case PP_FOREGROUNDCOLOR:
         case PP_FOREGROUNDCOLORINDEX:
            pclb->lfgnd = WgetCtlClr(pclb->hwnd, hps, PP_FOREGROUNDCOLOR,
                         PP_FOREGROUNDCOLORINDEX, SYSCLR_WINDOWSTATICTEXT);
            break;
         case PP_BACKGROUNDCOLOR:
         case PP_BACKGROUNDCOLORINDEX:
            pclb->lbkgnd = WgetCtlClr(pclb->hwnd, hps, PP_BACKGROUNDCOLOR,
                         PP_BACKGROUNDCOLORINDEX, SYSCLR_DIALOGBACKGROUND);
            break;
         case PP_FONTNAMESIZE:
            if ((pclb->cyFont = stSizeFont(hps)) % 2) ++pclb->cyFont;
            clbSize(pclb);
            clbMove(pclb);
            break;
      } /* endswitch */
      WinReleasePS(pclb->hwnd);
      WinRefreshWin(pclb->hwnd);
   }
   return (MRESULT)FALSE;
}


//==========================================================================\
// libera le risorse allocate per il controllo                              |
//==========================================================================/

VOID clbDestroyProc(PCLBOX pclb) {
   if (pclb) {
      if (pclb->hchk) WinDestroyWindow(pclb->hchk);
      WinDestroyWindow(pclb->hlbx);
      WinDestroyWindow(pclb->hentry);
      WinDestroyWindow(pclb->hadd);
      WinDestroyWindow(pclb->hrem);
      WinDestroyWindow(pclb->hfind);
      free(pclb->psz);
      free(pclb);
      _heapmin();
   } /* endif */
}


//==========================================================================\
// restituisce TRUE se il carattere corrisponde al mnemonico del controllo  |
//==========================================================================/

MRESULT clbMnemonic(PCLBOX pclb, ULONG ch) {
   HWND hfocus;
   if ((ch > 0xff || ch == '+' || ch == '-')) {
      if (WinIsChild((hfocus = WinQueryFocus(HWND_DESKTOP)), pclb->hwnd)) {
         switch (ch) {
            case 0x9d00:           // -> simula comando bottone FIND
               EDClbWinProc(pclb->hwnd, WM_COMMAND, (MPARAM)FID_FIND,
                            MPFROM2SHORT(CMDSRC_OTHER, 0));
               break;
            case '+':
               if (pclb->is.addon)
                  EDClbWinProc(pclb->hwnd, WM_COMMAND, (MPARAM)FID_ADD,
                               MPFROM2SHORT(CMDSRC_OTHER, 0));
               break;
            case '-':
               if (pclb->is.remon)
                  EDClbWinProc(pclb->hwnd, WM_COMMAND, (MPARAM)FID_REMOVE,
                               MPFROM2SHORT(CMDSRC_OTHER, 0));
               break;
         } /* endswitch */
      } /* endif */
      return (MRESULT)FALSE;
   } /* endif */
   if (pclb->fl & CLBXS_NOCAPT) return (MRESULT)FALSE;
   if (pclb->fl & CLBXS_CHECK) {
      if (pclb->pfbtn(pclb->hchk, WM_MATCHMNEMONIC, (MPARAM)ch, MPVOID))
         WinSetFocus(HWND_DESKTOP, pclb->hchk);
      return (MRESULT)FALSE;
   } else if (pclb->psz) {
      PSZ pmnemo = strchr(pclb->psz, '~');
      if (!pmnemo) return (MRESULT)FALSE;
      ch = WinUpperChar(pclb->hab, 0, 0, ch);
      return (MRESULT)(ch == WinUpperChar(pclb->hab, 0, 0, *++pmnemo) && ch);
   } else {
      return (MRESULT)FALSE;
   } /* endif */
}


//==========================================================================\
// da focus alla listbox se il mouse viene premuto sulla caption            |
//==========================================================================/

MRESULT clbMouBtn(PCLBOX pclb, PMSEMSG pmmsg) {
   if (pmmsg->x >= pclb->rclcpt.xLeft &&
       pmmsg->x < pclb->rclcpt.xRight &&
       pmmsg->y >= pclb->rclcpt.yBottom &&
       pmmsg->y < pclb->rclcpt.yTop) {
       WinSetFocus(HWND_DESKTOP, pclb->hlbx);
       return (MRESULT)TRUE;
   } /* endif */
   return (MRESULT)FALSE;
}


//==========================================================================\
// posta messaggio per dare focus alla listbox                              |
//==========================================================================/

MRESULT clbSetFocus(HWND hwnd, MPARAM mp1, BOOL fl) {
   if (fl) WinPostMsg(hwnd, WM_DELAYFOCUS, (MPARAM)hwnd, MPVOID);
   return WinDefWindowProc(hwnd, WM_SETFOCUS, mp1, (MPARAM)fl);
}


//==========================================================================\
// da il focus alla listbox                                                 |
//==========================================================================/

MRESULT clbDelayFocus(PCLBOX pclb) {
   WinSetFocus(HWND_DESKTOP, pclb->hlbx);
   return (MRESULT)FALSE;
}


//==========================================================================\
// cambia stato checkbox                                                    |
//==========================================================================/

MRESULT clbSetCheck(PCLBOX pclb, BOOL fl) {
   if (!(pclb->fl & CLBXS_CHECK)) return (MRESULT)0;
   pclb->is.chkon = (fl != 0);
   WinEnableWindow(pclb->hlbx, pclb->is.chkon);
   WinEnableWindow(pclb->hentry, pclb->is.chkon);
   WinEnableWindow(pclb->hadd, pclb->is.chkon & pclb->is.addon);
   WinEnableWindow(pclb->hrem, pclb->is.chkon & pclb->is.remon);
   WinEnableWindow(pclb->hfind, pclb->is.chkon);
   return pclb->pfbtn(pclb->hchk, BM_SETCHECK, (MPARAM)pclb->is.chkon, MPVOID);
}


//==========================================================================\
// riporta stato checkbox                                                   |
//==========================================================================/

MRESULT clbQueryCheck(PCLBOX pclb) {
   return (MRESULT)pclb->is.chkon;
}


//==========================================================================\
// setta il testo dell'entryfield (componente 3)                            |
//==========================================================================/

MRESULT clbSetEntryText(PCLBOX pclb, PSZ psz) {
   return (MRESULT)WinSetWindowText(pclb->hentry, psz);
}


//==========================================================================\
// aggiorna massima quantit… caratteri ammessa nel componente entryfield    |
//==========================================================================/

MRESULT clbEFtxtlimit(PCLBOX pclb, ULONG cb) {
   if (cb > 0x7fff) return (MRESULT)FALSE;
   pclb->cchef = cb;
   return pclb->pfef(pclb->hentry, EM_SETTEXTLIMIT, (MPARAM)cb, MPVOID);
}    
