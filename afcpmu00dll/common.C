//========================================================================\
// common.c                                                               |
// FUNZIONI: ------------------------------------------------------------ |
// APIRET APIENTRY InitControls(HAB hab);                                 |
//========================================================================/


#pragma strings(readonly)

#define INCL_WIN
#define INCL_GPI
#define INCL_DOSMISC
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES
#define INCL_DOSMODULEMGR

#include <os2.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "common.h"
#include "api.h"

int _rmem_init(void);
int _rmem_term(void);

HMTX hmtx;            // semaforo mutex
HMODULE hmod;         // module handle
ULONG flWCstc;        // stile default classe static
PFNWP pfnwpWCstc;     // procedura originale classe WC_STATIC
ULONG cbWCstc;        // offset alla winword aggiuntiva


//==========================================================================\
// inizializzazione: memorizza handle modulo e dati classe WC_STATIC        |
//==========================================================================/

#pragma entry(AFCPMU00InitTerm)

ULONG _System AFCPMU00InitTerm(ULONG ul, ULONG flag) {
   switch (flag) {
      case 0: {
         CLASSINFO ci;
         hmod = (HMODULE)ul;
         if (_rmem_init() ||
             !WinQueryClassInfo(NULLHANDLE, WC_STATIC, &ci))
            return 0L;
         flWCstc = ci.flClassStyle;
         pfnwpWCstc = ci.pfnWindowProc;
         cbWCstc = ci.cbWindowData;
         if (DosCreateMutexSem(NULL, &hmtx, 0, 0)) return 0L;
      }  break;
      case 1:
         _rmem_term();
         DosCloseMutexSem(hmtx);
         break;
      default: return 0L;
   } /* endswitch */
   return 1L;
}


//==========================================================================\
// inizializza tutti i controlli                                            |
//==========================================================================/

BOOL APIENTRY InitControls(HAB hab) {
   return (EDImgReg(hab) &&
           EDStbReg(hab) &&
           EDGrpReg(hab) &&
           EDBarReg(hab) &&
           EDElbReg(hab) &&
           EDClbReg(hab) &&
           EDMoreReg(hab));
}


//==========================================================================\
// wrapper per l'API WinQuerySysValue                                       |
//==========================================================================/

LONG APIENTRY WgetSysValue(LONG id) {
   return WinQuerySysValue(HWND_DESKTOP, id);
}


//==========================================================================\
// restituisce la struttura dati del controllo WC_STATUSBAR o WC_3DGROUPBOX |
// dalle window words del controllo stesso                                  |
//==========================================================================/

PCTL stGetData(HWND hwnd) {
   return (PCTL)WinQueryWindowPtr(hwnd, cbWCstc);
}


//==========================================================================\
// procedura creazione comune ai controlli WC_STATUSBAR e WC_3DGROUPBOX     |
// - viene allocata memoria per i dati del controllo                        |
// - vengono impostati i colori                                             |
// - viene misurata l'altezza del font usato                                |
// - viene resettato lo stile del controllo di default                      |
// - viene resettato il testo del controllo di default                      |
// - l'indirizzo della struttura Š memorizzato nelle window words           |
//==========================================================================/

PCTL stCreateProc(HWND hwnd, PCREATESTRUCT pc, ULONG style) {
   PCTL pct;
   HPS hps;
   // allocate structure for caching some control data
   if (!(pct = (PCTL)malloc(sizeof(CTL)))) return NULL;
   memset((PVOID)pct, 0, sizeof(CTL));
   // copy some data
   pct->hwnd = hwnd;
   pct->fl = pc->flStyle;
   // se bordo non specificato usa il valore 1
   if (!(pct->fl & CCTL_BORDER7)) pct->fl |= CCTL_BORDER1;
   pct->wr.cx = pc->cx;
   pct->wr.cy = pc->cy;
   // imposta colori controllo
   if (!(hps = WinGetPS(hwnd))) {free(pct); return NULL;}
   stUpdColors(pct, hps);
   pct->cyFont = stSizeFont(hps);
   WinReleasePS(hps);
   // reset WC_STATIC style
   WinSetWindowBits(hwnd, QWL_STYLE, style, 0xffff);
   if (stSetText(pct, pc->pszText)) *pc->pszText = 0;
   WinSetWindowPtr(hwnd, cbWCstc, (PVOID)pct); 
   return pct;
}


//==========================================================================\
// aggiorna testo controllo                                                 |
// restituisce la lunghezza del testo                                       |
//==========================================================================/

ULONG stSetText(PCTL pct, PSZ psz) {
   if (!pct) return 0;
   if (pct->psz) free(pct->psz);
   if (!psz ||
       !(pct->cbtxt = strlen(psz)) ||
       !(pct->psz = (PSZ)malloc(pct->cbtxt + 1))) {
      pct->cbtxt = 0; 
      pct->psz = NULL;
      return 0;
   } // end if
   memcpy(pct->psz, psz, pct->cbtxt + 1);
   return pct->cbtxt;
}


//==========================================================================\
// restituisce il testo del controllo in risposta di WM_QUERYWINDOWPARAMS   |
//==========================================================================/

MRESULT stReturnText(PCTL pct, PWNDPARAMS pwp) {
   if (pct) {
      if (pwp->fsStatus & WPM_TEXT) {
         // se pcs->cbTxt = 0 allora pcs->pszTxt Š NULL e non si pu•
         // copiare in pwp->pszText
         if (pct->cbtxt) {
            memcpy((PVOID)(pwp->pszText),
                   (PVOID)pct->psz,
                   min(pct->cbtxt, pwp->cchText) + 1);
         } else {
            if (pwp->cchText) *(pwp->pszText) = '\0';
         } // end if
      } // end if
      // se richiede lunghezza stringa:
      if (pwp->fsStatus & WPM_CCHTEXT) {
         pwp->cchText = pct->cbtxt;
      } // end if
   } // end if
   return (MRESULT)TRUE;
}


//==========================================================================\
// memorizza stato abilitazione finestra                                    |
//==========================================================================/

VOID stEnable(PCTL pct, BOOL fl) {
   if (pct) pct->fl = fl? pct->fl & ~WS_DISABLED: pct->fl | WS_DISABLED;
}


//==========================================================================\
// memorizza nuova dimensione controllo in risposta di WM_ADJUSTWINDOWPOS   |
// con flag SWP_SIZE                                                        |
//==========================================================================/

VOID stUpdateCtlSize(HWND hwnd, PSWP pswp) {
   PCTL pct = stGetData(hwnd);
   if (pct) {
      pct->wr.cx = pswp->cx;
      pct->wr.cy = pswp->cy;
   } // end if
}


//==========================================================================\
// libera le risorse allocate per il controllo                              |
//==========================================================================/

VOID stDestroyProc(HWND hwnd) {
   PCTL pct = stGetData(hwnd);
   if (pct) {
      if (pct->psz) free(pct->psz);
      free(pct);
   } // end if
}


//==========================================================================\
// aggiorna i colori dei controlli con stile WC_STATUSBAR e WC_3DGROUPBOX   |
//==========================================================================/

VOID stUpdColors(PCTL pct, HPS hps) {
   pct->lfgnd = WgetCtlClr(pct->hwnd, hps, PP_FOREGROUNDCOLOR,
                         PP_FOREGROUNDCOLORINDEX, SYSCLR_WINDOWSTATICTEXT);
   pct->lbkgnd = WgetCtlClr(pct->hwnd, hps, PP_BACKGROUNDCOLOR,
                         PP_BACKGROUNDCOLORINDEX, SYSCLR_DIALOGBACKGROUND);
   pct->lhilite = WgetCtlClr(pct->hwnd, hps, PP_BORDERLIGHTCOLOR,
                         0, SYSCLR_BUTTONLIGHT);
   pct->lshadow = WgetCtlClr(pct->hwnd, hps, PP_BORDERDARKCOLOR,
                         0, SYSCLR_BUTTONDARK);
}


//==========================================================================\
// restituisce la massima estensione in altezza del font corrente           |
//==========================================================================/

ULONG stSizeFont(HPS hps) {
   PFONTMETRICS pfm;
   ULONG cy;
   if (!(pfm = (PFONTMETRICS)malloc(sizeof(FONTMETRICS)))) return 0;
   if (!GpiQueryFontMetrics(hps, sizeof(FONTMETRICS), pfm)) cy = 0;
   else cy = pfm->lMaxBaselineExt;
   free(pfm);
   return cy;
}


//==========================================================================\
// in seguito a WM_SYSCOLORCHANGE reimposta tutti i colori del controllo    |
// e lo ridisegna                                                           |
//==========================================================================/

MRESULT stSCchange(PCTL pct) {
   HPS hps;
   if (pct && NULLHANDLE != (hps = WinGetPS(pct->hwnd))) {
      stUpdColors(pct, hps);
      WinReleasePS(hps);
      WinRefreshWin(pct->hwnd);
   } /* endif */
   return (MRESULT)FALSE;
}


//==========================================================================\
// in seguito a WM_PRESPARAMCHANGED reimposta il colore cambiato e          |
// ridisegna il controllo                                                   |
//==========================================================================/

MRESULT stPPchange(PCTL pct, ULONG fl) {
   HPS hps;
   if (pct && NULLHANDLE != (hps = WinGetPS(pct->hwnd))) {
      switch (fl) {
         case PP_FOREGROUNDCOLOR:
         case PP_FOREGROUNDCOLORINDEX:
            pct->lfgnd = WgetCtlClr(pct->hwnd, hps, PP_FOREGROUNDCOLOR,
                         PP_FOREGROUNDCOLORINDEX, SYSCLR_WINDOWSTATICTEXT);
            break;
         case PP_BACKGROUNDCOLOR:
         case PP_BACKGROUNDCOLORINDEX:
            pct->lbkgnd = WgetCtlClr(pct->hwnd, hps, PP_BACKGROUNDCOLOR,
                         PP_BACKGROUNDCOLORINDEX, SYSCLR_DIALOGBACKGROUND);
            break;
         case PP_BORDERLIGHTCOLOR:
            pct->lhilite = WgetCtlClr(pct->hwnd, hps, PP_BORDERLIGHTCOLOR,
                         0, SYSCLR_BUTTONLIGHT);
            break;
         case PP_BORDERDARKCOLOR:
            pct->lshadow = WgetCtlClr(pct->hwnd, hps, PP_BORDERDARKCOLOR,
                         0, SYSCLR_BUTTONDARK);
            break;
         case PP_FONTNAMESIZE:
            pct->cyFont = stSizeFont(hps);
            break;
      } /* endswitch */
      WinReleasePS(pct->hwnd);
      WinRefreshWin(pct->hwnd);
   }
   return (MRESULT)FALSE;
}


//==========================================================================\
// forza repaint del controllo                                              |
//==========================================================================/

BOOL WinRefreshWin(HWND hwnd) {
   return WinInvalidateRect((hwnd), NULL, FALSE);
}


//==========================================================================\
// inizializza membri struttura swp                                         |
//==========================================================================/

VOID mkswp(PSWP pswp, HWND hwnd, LONG x, LONG y, LONG cx, LONG cy) {
   pswp->hwnd = hwnd;
   pswp->x = x;
   pswp->y = y;
   pswp->cx = cx;
   pswp->cy = cy;
}


//==========================================================================\
// trasforma coordinate rettangolo nelle corrispondenti coordinate struttura|
// SWP                                                                      |
//==========================================================================/

VOID rcl2swp(PSWP pswp, HWND hwnd, LONG x0, LONG y0, LONG x1, LONG y1) {
   pswp->hwnd = hwnd;
   pswp->x = x0;
   pswp->y = y0;
   pswp->cx = x1 - x0;
   pswp->cy = y1 - y0;
}


//==========================================================================\
// sovraimpone pattern disabilitazione al bottone                           |
//==========================================================================/

BOOL drawDisBtn(PUSERBUTTON pub, ULONG cx, LONG color) {
   RECTL rcl = {0, 0, cx, cx};
   return WdrawDisabledRect(pub->hps, &rcl, color);
}


//==========================================================================\
// disegna segno "+" contenuto nel bottone add                              |
//==========================================================================/

VOID drawAdd(PUSERBUTTON pub, ULONG cx) {
   POINTL aptl[8];
   // disegna parte orizzontale segno +
   aptl[0].x = 6;
   aptl[0].y = cx / 2 - 1;
   aptl[1].x = cx - 7;
   aptl[1].y = aptl[0].y;
   aptl[2].x = aptl[1].x;
   aptl[2].y = aptl[0].y + 1;
   aptl[3].x = aptl[0].x;
   aptl[3].y = aptl[2].y;
   aptl[4].x = aptl[0].y;
   aptl[4].y = 6;
   aptl[5].x = aptl[1].y;
   aptl[5].y = aptl[1].x;
   aptl[6].x = aptl[2].y;
   aptl[6].y = aptl[2].x;
   aptl[7].x = aptl[3].y;
   aptl[7].y = aptl[3].x;
   if (pub->fsState & BDS_HILITED) {
      ULONG i;
      for (i = 0; i < 8; ++i) {
         aptl[i].x++;
         aptl[i].y--;
      } /* endfor */
   } /* endif */
   GpiSetColor(pub->hps, 0);
   GpiMove(pub->hps, aptl);
   GpiPolyLine(pub->hps, 3, aptl + 1);
   GpiMove(pub->hps, aptl + 4);
   GpiPolyLine(pub->hps, 3, aptl + 5);
}


//==========================================================================\
// disegna segno "-" contenuto nel bottone remove                           |
//==========================================================================/

VOID drawRem(PUSERBUTTON pub, ULONG cx) {
   POINTL aptl[4];
   // disegna parte orizzontale segno +
   aptl[0].x = 6;
   aptl[0].y = cx / 2 - 1;
   aptl[1].x = cx - 7;
   aptl[1].y = aptl[0].y;
   aptl[2].x = aptl[1].x;
   aptl[2].y = aptl[0].y + 1;
   aptl[3].x = aptl[0].x;
   aptl[3].y = aptl[2].y;
   if (pub->fsState & BDS_HILITED) {
      ULONG i;
      for (i = 0; i < 4; ++i) {
         aptl[i].x++;
         aptl[i].y--;
      } /* endfor */
   } /* endif */
   GpiSetColor(pub->hps, 0);
   GpiMove(pub->hps, aptl);
   GpiPolyLine(pub->hps, 3, aptl + 1);
}


//==========================================================================\
// disegna triangolo contenuto nel bottone find                             |
//==========================================================================/

VOID drawFind(PUSERBUTTON pub, ULONG cx) {
   POINTL aptl[6];
   aptl[0].x = 5 + ((cx - 10) / 4);
   aptl[0].y = 6;
   aptl[1].x = aptl[0].x + 1;
   aptl[1].y = aptl[0].y;
   aptl[2].x = aptl[0].x + (cx - 12) / 2;
   aptl[2].y = aptl[0].y + (cx - 12) / 2 - 1;
   aptl[3].x = aptl[2].x;
   aptl[3].y = aptl[2].y + 1;
   aptl[4].x = aptl[1].x;
   aptl[4].y = aptl[0].y + cx - 13;
   aptl[5].x = aptl[0].x;
   aptl[5].y = aptl[4].y;
   if (pub->fsState & BDS_HILITED) {
      ULONG i;
      for (i = 0; i < 6; ++i) {
         aptl[i].x++;
         aptl[i].y--;
      } /* endfor */
   } /* endif */
   GpiSetColor(pub->hps, 0);
   GpiBeginArea(pub->hps, 0);
   GpiMove(pub->hps, aptl);
   GpiPolyLine(pub->hps, 5, aptl + 1);
   GpiEndArea(pub->hps);
}


//==========================================================================\
// disegna triangolo orientato verso il basso per bottone swap              |
//==========================================================================/

VOID drawSwap(PUSERBUTTON pub, ULONG cx) {
   POINTL aptl[6];
   aptl[0].x = 6;
   aptl[0].y = cx - 7 - ((cx - 10) / 4);
   aptl[1].x = aptl[0].x;
   aptl[1].y = aptl[0].y - 1;
   aptl[2].x = aptl[0].x + (cx - 12) / 2 - 1;
   aptl[2].y = aptl[0].y - (cx - 12) / 2;
   aptl[3].x = aptl[2].x + 1;
   aptl[3].y = aptl[2].y;
   aptl[4].x = cx - 7;
   aptl[4].y = aptl[1].y;
   aptl[5].x = aptl[4].x;
   aptl[5].y = aptl[0].y;
   if (pub->fsState & BDS_HILITED) {
      ULONG i;
      for (i = 0; i < 6; ++i) {
         aptl[i].x++;
         aptl[i].y--;
      } /* endfor */
   } /* endif */
   GpiSetColor(pub->hps, 0);
   GpiBeginArea(pub->hps, 0);
   GpiMove(pub->hps, aptl);
   GpiPolyLine(pub->hps, 5, aptl + 1);
   GpiEndArea(pub->hps);
}


//==========================================================================\
// disegna pattern disabilitato sopra le checkbox                           |
//==========================================================================/

VOID paintDisabledCheck(HWND hwnd, PRECTL prcl, LONG lclr) {
   RECTL rcl = {0, 0, prcl->xRight - prcl->xLeft, prcl->yTop - prcl->yBottom};
   HPS hps = WinGetPS(hwnd);
   GpiCreateLogColorTable(hps, 0, LCOLF_RGB, 0, 0, NULL);
   WdrawDisabledRect(hps, prcl, lclr);
   WinReleasePS(hps);
}
