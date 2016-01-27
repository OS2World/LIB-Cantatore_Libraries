//========================================================================\
// statusbar.c                                                            |
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
#include "common.h"
#include "api.h"


// prototipi funzioni

MRESULT PaintStatusBar(HWND hwnd);
MRESULT EXPENTRY EDStbWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);


//==========================================================================\
// registrazione classe                                                     |
//==========================================================================/

BOOL APIENTRY EDStbReg(HAB hab) {
   return WinRegisterClass(hab, WC_STATUSBAR, (PFNWP)EDStbWinProc,
                           flWCstc & ~CS_PUBLIC, cbWCstc + 4);
}


//==========================================================================\
// procedura controllo statusbar                                            |
//==========================================================================/

MRESULT EXPENTRY EDStbWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   switch (msg) {
      // creazione: ridimensiona controllo per permettere disegno bordo
      case WM_CREATE:
         if (!stCreateProc(hwnd, (PCREATESTRUCT)mp2, SS_TEXT))
            return (MRESULT)TRUE;
         break;
      // se il controllo Š spostato o mosso aggiorna dati in SBAR e
      // ridimensiona opportunamente il controllo
      case WM_ADJUSTWINDOWPOS:     // default window processing
         if (((PSWP)mp1)->fl & SWP_SIZE) stUpdateCtlSize(hwnd, (PSWP)mp1);
         break;
      case WM_ENABLE:
         stEnable(stGetData(hwnd), (BOOL)mp1);
         break;
      case WM_PAINT:
         return PaintStatusBar(hwnd);
      // aggiorna testo finestra
      case WM_SETWINDOWPARAMS:
         if (((PWNDPARAMS)mp1)->fsStatus & WPM_TEXT) {
            stSetText(stGetData(hwnd), ((PWNDPARAMS)mp1)->pszText);
            WinRefreshWin(hwnd);
         } // end if
         return (MRESULT)TRUE;
      // restituisce testo controllo (risposta a WinQueryWindowText())
      case WM_QUERYWINDOWPARAMS:
         return stReturnText(stGetData(hwnd), (PWNDPARAMS)mp1);
      // memorizza nuovo colore/font e invalida controllo x aggiornarlo
      case WM_SYSCOLORCHANGE:
         return stSCchange(stGetData(hwnd));
      case WM_PRESPARAMCHANGED:
         return stPPchange(stGetData(hwnd), (ULONG)mp1); 
      case WM_DESTROY:
         stDestroyProc(hwnd);
         break;
   } /* endswitch */
   return pfnwpWCstc(hwnd, msg, mp1, mp2); // default static controls proc
}


MRESULT PaintStatusBar(HWND hwnd) {
   PCTL psb = stGetData(hwnd);
   HPS hps;
   if (psb && NULLHANDLE != (hps = WinBeginPaint(hwnd, NULLHANDLE, NULL))) {
      RECTL rcl = {0, 0, psb->wr.cx, psb->wr.cy};
      ULONG brd = psb->fl & 0x7;
      GpiCreateLogColorTable(hps, 0, LCOLF_RGB, 0, 0, NULL);
      WinFillRect(hps, &rcl, psb->lbkgnd);
      WinDrawBorder(hps, &rcl, brd, brd, 0, 0,
                    (psb->fl & CCTL_RAISED? 0x400: 0x800));
      // se necessario disegnare anche frame
      if (psb->fl & CCTL_FRAME6) {
         ULONG frm = brd * ((psb->fl & 0x38) >> 3);
         rcl.xLeft = frm;
         rcl.yBottom = frm;
         rcl.xRight -= frm; 
         rcl.yTop -= frm; 
         WinDrawBorder(hps, &rcl, brd, brd, 0, 0,
                       (psb->fl & CCTL_RAISED? 0x800: 0x400));
      } /* endif */
      // disegna testo
      rcl.xLeft += brd + 1;
      rcl.yBottom += brd;
      rcl.xRight -= brd + 2; 
      rcl.yTop -= brd; 
      if (psb->fl & DT_WORDBREAK) {
         ULONG fl = (WinQueryWindowULong(hwnd, QWL_STYLE) & WS_DISABLED)?
                    DT_HALFTONE: 0;
         ULONG cchleft = psb->cbtxt;
         ULONG cchdone = 0;
         // l'allineamento verticale Š ignorato
         fl |= psb->fl &
               (DT_LEFT | DT_CENTER | DT_RIGHT | DT_WORDBREAK | DT_MNEMONIC);
         cchdone = 0;
         cchleft = psb->cbtxt;
         do {
            cchleft -= (cchdone += WinDrawText(hps, cchleft,
                                              psb->psz + cchdone, &rcl,
                                              psb->lfgnd, psb->lbkgnd,
                                              fl));
            if ((rcl.yTop -= psb->cyFont) < rcl.yBottom) break;
         } while (cchleft > 0); // end do
      } else {
         ULONG fl = (psb->fl & WS_DISABLED)? DT_HALFTONE: 0;
         fl |= (psb->fl & 0x2f00) | DT_ERASERECT; // solo flag DT valide
         WinDrawText(hps, psb->cbtxt, psb->psz, &rcl,
                     psb->lfgnd, psb->lbkgnd, fl);
      } // end if (DT_WORDBREAK) else
      WinEndPaint(hps);
   } /* endif */
   return (MRESULT)FALSE;
}
