//========================================================================\
// groupbox.c                                                             |
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


// prototipi funzione

VOID SizeGroupText(PCTL pgb);
MRESULT PaintGroupBox(HWND hwnd);
MRESULT EXPENTRY EDGrpWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);


//==========================================================================\
// registrazione classe                                                     |
//==========================================================================/

BOOL APIENTRY EDGrpReg(HAB hab) {
   return WinRegisterClass(hab, WC_3DGROUPBOX, (PFNWP)EDGrpWinProc,
                           flWCstc & ~CS_PUBLIC, cbWCstc + 4);
}


//==========================================================================\
// procedura controllo groupbox                                             |
// per ora ignora tutti gli stili tranne raised o depressed e disegna il    |
// testo (se presente) in una cornice invertita rispetto alla cornice del   |
// gruppo                                                                   |
//==========================================================================/

MRESULT EXPENTRY EDGrpWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   switch (msg) {
      // creazione: memorizza testo controllo
      case WM_CREATE: {
         PCTL pgb;
         if (!(pgb = stCreateProc(hwnd, (PCREATESTRUCT)mp2, SS_GROUPBOX)))
            return (MRESULT)TRUE;
         // store control text and get text rectangle size
         if (pgb->cbtxt) SizeGroupText(pgb);
         break;
      }
      // se il controllo Š ridimensionato memorizza nuove dimensioni
      case WM_ADJUSTWINDOWPOS:
         if (((PSWP)mp1)->fl & SWP_SIZE) stUpdateCtlSize(hwnd, (PSWP)mp1);
         break;
      case WM_ENABLE:
         stEnable(stGetData(hwnd), (BOOL)mp1);
         break;
      // sostituisce totalmente procedura controllo statico
      case WM_PAINT:
         return PaintGroupBox(hwnd);
      // aggiorna testo finestra
      case WM_SETWINDOWPARAMS:
         if (((PWNDPARAMS)mp1)->fsStatus & WPM_TEXT) {
            PCTL pgb = stGetData(hwnd);
            // se nuovo testo presente (lunghezza > 0) ricalcola dimensioni
            if (stSetText(pgb, ((PWNDPARAMS)mp1)->pszText))
               SizeGroupText(pgb);
            WinRefreshWin(hwnd);
         } // end if
         return (MRESULT)TRUE;
      // restituisce testo controllo (risposta a WinQueryWindowText())
      case WM_QUERYWINDOWPARAMS:
         return stReturnText(stGetData(hwnd), (PWNDPARAMS)mp1);
      // memorizza nuovo colore/font e invalida controllo x aggiornarlo
      case WM_SYSCOLORCHANGE:
         return stSCchange(stGetData(hwnd));
      case WM_PRESPARAMCHANGED: {
         PCTL pgb = stGetData(hwnd);
         MRESULT mr = stPPchange(pgb, (ULONG)mp1);
         if ((ULONG)mp1 == PP_FONTNAMESIZE) SizeGroupText(pgb);
         return mr;
      } // end case WM_PRESPARAMCHANGED
      case WM_DESTROY:
         stDestroyProc(hwnd);
         break;
   } /* endswitch */
   return pfnwpWCstc(hwnd, msg, mp1, mp2); // default static controls proc
}


// calcola dimensioni rettangolo testo groupbox
VOID SizeGroupText(PCTL pgb) {
   HPS hps = WinGetPS(pgb->hwnd);
   POINTL aptl[3];
   PSZ psz;
   pgb->wrtxt.cx = 0;
   // calculate text rectangle size
   if ((pgb->fl & DT_MNEMONIC) &&
       NULL != (psz = strchr(pgb->psz, '~'))) {
      if (psz - pgb->psz) {  // se la tilde non Š primo char
         GpiQueryTextBox(hps, psz - pgb->psz, pgb->psz, 3, aptl);
         pgb->wrtxt.cx = aptl[2].x - aptl[0].x;
      } // end if              se tilde non Š ultimo char
      if (pgb->cbtxt - (psz - pgb->psz) - 1) {
         GpiQueryTextBox(hps, pgb->cbtxt - (psz - pgb->psz) - 1,
                         psz + 1, 3, aptl);
         pgb->wrtxt.cx += aptl[2].x - aptl[0].x + 2;
      } /* endif */
      //pgb->wrtxt.cy = aptl[0].y - aptl[1].y;
   } else {
      GpiQueryTextBox(hps, pgb->cbtxt, pgb->psz, 3, aptl);
      //pgb->wrtxt.cy = aptl[0].y - aptl[1].y;
      pgb->wrtxt.cx = aptl[2].x - aptl[0].x + 2;
   } /* endif */
   WinReleasePS(hps);
} /* endif */


// disegna il controllo groupbox
MRESULT PaintGroupBox(HWND hwnd) {
   PCTL pgb = stGetData(hwnd);
   if (pgb) {
      RECTL rcl = {0, 0, pgb->wr.cx, pgb->wr.cy};
      HPS hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);
      ULONG brd = pgb->fl & 0x7;
      ULONG frm = brd;
      GpiCreateLogColorTable(hps, 0, LCOLF_RGB, 0, 0, NULL);
      // disegna bordo groupbox
      WinDrawBorder(hps, &rcl, brd, brd, 0, 0,
                    (pgb->fl & CCTL_RAISED? 0x400: 0x800));
      // se necessario disegnare anche frame
      if (pgb->fl & CCTL_FRAME6) {
         frm = brd * ((pgb->fl & 0x38) >> 3);
         rcl.xLeft = frm;
         rcl.yBottom = frm;
         rcl.xRight -= frm; 
         rcl.yTop -= frm; 
         WinDrawBorder(hps, &rcl, brd, brd, 0, 0,
                       (pgb->fl & CCTL_RAISED? 0x800: 0x400));
      } /* endif */
      // se il testo Š presente
      if (pgb->psz) {
         // cancella background
         rcl.xLeft = frm;
         rcl.yBottom = pgb->wr.cy - pgb->cyFont - 2 * frm - 2;
         rcl.xRight = pgb->wrtxt.cx + 2 * frm + 3;
         rcl.yTop = pgb->wr.cy - frm;
         WinFillRect(hps, &rcl, pgb->lbkgnd);
         // disegna testo
         rcl.yBottom += 2;
         WinDrawText(hps, -1, pgb->psz, &rcl, pgb->lfgnd, pgb->lbkgnd,
                     DT_CENTER | DT_TOP | DT_MNEMONIC | DT_EXTERNALLEADING |
                     ((pgb->fl & WS_DISABLED)? DT_HALFTONE: 0));
         // disegna bordo inferiore e a destra del testo
         rcl.yBottom -= 2;
         GpiSetColor(hps, (pgb->fl & CCTL_RAISED? pgb->lshadow: pgb->lhilite));
         GpiMove(hps, (PPOINTL)&rcl);
         rcl.xLeft = rcl.xRight;
         rcl.yTop -= 1;
         GpiPolyLine(hps, 2, (PPOINTL)&rcl);
      } /* endif */
      WinEndPaint(hps);
   } /* endif */
   return (MRESULT)FALSE;
}
