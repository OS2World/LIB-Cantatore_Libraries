//========================================================================\
// image.c : implementazione controllo immagine statica                   |
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
#include "image.h"
#include "api.h"


//==========================================================================\
// registrazione classe                                                     |
//==========================================================================/

BOOL APIENTRY EDImgReg(HAB hab) {
   return WinRegisterClass(hab, WC_EDLGSIMG, (PFNWP)EDImgWinProc,
                           flWCstc & ~CS_PUBLIC, cbWCstc + 4);
}


//==========================================================================\
// nuova procedura finestre statiche                                        |
//==========================================================================/

MRESULT EXPENTRY EDImgWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   // alla creazione estrae dal testo del controllo il nome della libreria
   // e l'ID della risorsa da cui ricavare l'immagine
   switch (msg) {
      case WM_CREATE: {
         LHANDLE himg;        // handle immagine
         PSZ psz;             // per ricavare nome modulo e id risorsa
         ULONG idres;         // id risorsa
         HLIB hlib;
         HAB hab;
         SIZEL szl;
   
         hab = WinQueryAnchorBlock(hwnd);
         // cerca primo tab lo cambia in 0 e carica lib
         if (!(psz = memchr((PVOID)((PCREATESTRUCT)mp2)->pszText, '\t', 9)))
            goto error0;
         *psz = 0;                          
         if (!(hlib = WinLoadLibrary(hab, ((PCREATESTRUCT)mp2)->pszText)))
            goto error0;
         // ottiene handle e dimensione immagine
         if (!(idres = strtoul(++psz, NULL, 10))) goto error1;
         if (((PCREATESTRUCT)mp2)->flStyle & (ESI_PTR | ESI_ICON | ESI_MINI)) {
            if (!(himg = WinLoadPointer(HWND_DESKTOP, hlib, idres)))
               goto error1;
            if (((PCREATESTRUCT)mp2)->flStyle & ESI_PTR) {          // pointer
               szl.cx = WgetSysValue(SV_CXPOINTER);
               szl.cy = WgetSysValue(SV_CYPOINTER);
            } else if (((PCREATESTRUCT)mp2)->flStyle & ESI_ICON) {  // icona
               szl.cx = WgetSysValue(SV_CXICON);
               szl.cy = WgetSysValue(SV_CYICON);
            } else {                               // minicona
               szl.cx = WgetSysValue(SV_CXICON) / 2;
               szl.cy = WgetSysValue(SV_CYICON) / 2;
            } // end if
         } else {                                  // bitmap
            if (!(himg = WloadBitmap(hwnd, hlib, idres)))
               goto error1;                                   
            if (!WqueryBitmapSize(himg, &szl))
               goto error2;
         } // end if
         WinDeleteLibrary(hab, hlib);
         // memorizza handle immagine
         WinSetWindowULong(hwnd, cbWCstc, (ULONG)himg);
         *((PCREATESTRUCT)mp2)->pszText = 0;
         //break;
         WinSetWindowPos(hwnd, 0, 0, 0, szl.cx, szl.cy, SWP_SIZE|SWP_NOADJUST);
         break;
         
         error2: GpiDeleteBitmap(himg);
         error1: WinDeleteLibrary(hab, hlib);
         error0: return (MRESULT)TRUE;
      } // end case WM_CREATE
      case WM_ADJUSTWINDOWPOS:    // default window processing
         return WinDefWindowProc(hwnd, msg, mp1, mp2);
      case WM_PAINT: {
         LHANDLE himg = (LHANDLE)WinQueryWindowULong(hwnd, cbWCstc);
         ULONG flStyle = WinQueryWindowULong(hwnd, QWL_STYLE);
         HPS hps;
         hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);
         if (flStyle & (ESI_PTR | ESI_ICON | ESI_MINI)) {
            WinDrawPointer(hps, 0, 0, himg,
                           (flStyle & ESI_MINI? DP_MINI: DP_NORMAL));
         } else {
            POINTL ptl = {0,0};
            WinDrawBitmap(hps, himg, NULL, &ptl, 0, 0, DBM_NORMAL);
         } // end if
         WinEndPaint(hps);
         return (MRESULT)FALSE;
      }  // end case WM_PAINT
      // restituisce handle immagine corrente
      case SM_QUERYHANDLE:
         return (MPARAM)WinQueryWindowULong(hwnd, cbWCstc);
      // memorizza nuovo handle immagine, forza repaint e restituisce
      // handle precedente immagine
      case SM_SETHANDLE: {
         LHANDLE himg = (LHANDLE)WinQueryWindowULong(hwnd, cbWCstc);
         if (!WinSetWindowULong(hwnd, cbWCstc, (ULONG)mp1))
            return NULLHANDLE;
         WinRefreshWin(hwnd);
         return (MRESULT)himg;
      } // end case SM_SETHANDLE
      case WM_MATCHMNEMONIC:
         return (MRESULT)FALSE;
      case WM_DESTROY: {
         LHANDLE himg = (LHANDLE)WinQueryWindowULong(hwnd, cbWCstc);
         if (WinQueryWindowULong(hwnd, QWL_STYLE) &
                                (ESI_PTR | ESI_ICON | ESI_MINI)) {
            WinDestroyPointer(himg);
         } else {
            GpiDeleteBitmap(himg);
         } // end if
      }  break;
   } /* endswitch */
   return pfnwpWCstc(hwnd, msg, mp1, mp2); // default static controls processing
}
