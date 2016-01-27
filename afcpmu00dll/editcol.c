//=========================================================================\
// editcol.c : color wheel dlg procedure                                   |
// version 1.1                                                             |
//=========================================================================/

#pragma strings(readonly)

#define INCL_WIN
//#define INCL_GPI
#define INCL_DOSMODULEMGR
#define INCL_DOSMISC
#include <os2.h>
#include <stdlib.h>
#include "common.h"
#include "editcol.h"
#include "api.h"


//=========================================================================\
// WeditColorDlg():                                                      |
// parameters: ----------------------------------------------------------- |
// HWND hParent  : parent handle                                           |
// HWND hOwner   : owner handle                                            |
// PCLRWHEEL pcw : control data structure                                  |
// returned value: ------------------------------------------------------- |
// LONG lColor      : RGB color (success), CLR_ERROR (failure)             |
//=========================================================================/


LONG APIENTRY WeditColorDlg(HWND hParent, HWND hOwner, PCLRWHEEL pcw) {
   PAPPDATA pad;
   HAB hab;
   HLIB hlib;
   ULONG ulVer;
   LONG lcolor;

   // allocate storage for application data
   if (!(pad = (PAPPDATA)malloc(sizeof(APPDATA)))) return CLR_ERROR;
   // initialize APPDATA structure
   pad->pcw = pcw;
   hab = WinQueryAnchorBlock(hOwner);
   // window handles
   pad->hOwner = hOwner;
   // query the OS version
   DosQuerySysInfo(QSV_VERSION_MINOR, QSV_VERSION_MINOR, &ulVer, 4);
   // sets msg value according to OS version
   if (ulVer > 30) {                  // warp 4
      pad->cwChangedMsg = WM_COLORWHEELCHANGED;
      pad->cwSetMsg = CWM_COLORWHEELSETVAL;
   } else {                                // warp 3 (warp 2?)
      pad->cwChangedMsg = WM_COLORWHEELCHANGED3;
      pad->cwSetMsg = CWM_COLORWHEELSETVAL3;
   } /* endif */
   // copy the start color
   pad->clrCurr.l = pad->clrUndo.l = pcw->clr;
   // register the color wheel control and open the dialog
   if (!(hlib = WinLoadLibrary(hab, "WPCONFIG.DLL"))) {
      lcolor = CLR_ERROR;
   } else {   
      lcolor = (LONG)WinDlgBox(hParent, hOwner, EditColorDlgProc, hmod,
                               DLG_CWHEEL, pad);
      WinDeleteLibrary(hab, hlib);
   } // end if
   free(pad);
   return lcolor;
}


//=========================================================================\
// EditColorDlgProc(): color wheel dlg procedure                           |
// processed messages: - WM_INITDLG                                        |
//                     - WM_COLORWHEELCHANGED                              |
//                     - WM_CONTROL                                        |
//                     - WM_COMMAND                                        |
//                     - WM_CLOSE                                          |
//                     - WM_DESTROY                                        |
//=========================================================================/

MRESULT EXPENTRY EditColorDlgProc(HWND hwnd, ULONG msg,
                                  MPARAM mp1, MPARAM mp2) {
   switch (msg) {
   // init -----------------------------------------------------------------
      case WM_INITDLG: {
         PAPPDATA pad = (PAPPDATA)mp2;
         WinSetWindowPtr(hwnd, 0L, (PVOID)mp2);
         // ove necessario cambia caption comonenti dialogo
         if (pad->pcw->pszTitle) WinSetWindowText(hwnd, pad->pcw->pszTitle);
         if (pad->pcw->pszOK)
            WinSetDlgItemText(hwnd, BTN_OK, pad->pcw->pszOK);
         if (pad->pcw->pszUndo)
            WinSetDlgItemText(hwnd, BTN_UNDO, pad->pcw->pszUndo);
         if (pad->pcw->pszCancel)
            WinSetDlgItemText(hwnd, BTN_CANCEL, pad->pcw->pszCancel);
         if (pad->pcw->pszRed)
            WinSetDlgItemText(hwnd, TXT_RED, pad->pcw->pszRed);
         if (pad->pcw->pszGreen)
            WinSetDlgItemText(hwnd, TXT_GREEN, pad->pcw->pszGreen);
         if (pad->pcw->pszBlue)
            WinSetDlgItemText(hwnd, TXT_BLUE, pad->pcw->pszBlue);
         if (pad->pcw->FontNameSize)
            WsetDlgFonts(hwnd, NULLHANDLE, pad->pcw->FontNameSize);
         SetSpins(hwnd, pad->clrUndo); // initialize spinbutton
         // initialize the color wheel control
         WinSendDlgItemMsg(hwnd, CWHEEL, pad->cwSetMsg,
                           (MPARAM)pad->clrUndo.l, MPVOID);
         pad->is.upd = 1;            // sets the update flag
         // show the dialog
         if (pad->pcw->fl & CWDS_CENTER) {
            WcenterInWindow(hwnd, pad->pcw->hPos);
         } else {
            SWP swp;
            swp.hwnd = pad->pcw->hPos;
            swp.x = pad->pcw->ptl.x;
            swp.y = pad->pcw->ptl.y;
            swp.cx = swp.cy = 0;
            swp.fl = SWP_MOVE | SWP_SHOW | SWP_ACTIVATE | SWP_ZORDER;
            swp.hwndInsertBehind = HWND_TOP;
            WsetRelativePos(hwnd, &swp);
         } // end if
         break;
      } // end case WM_INITDLG
   // a new color has been set through the color wheel ---------------------
      case WM_COLORWHEELCHANGED3:
      case WM_COLORWHEELCHANGED: {
         PAPPDATA pad = WinQueryWindowPtr(hwnd, 0L);
         pad->clrCurr.l = (LONG)mp1;
         pad->is.upd = 0;           // reset update flag
         SetSpins(hwnd, pad->clrCurr);
         pad->is.upd = 1;            // set update flag
         // notify new color to owner
         WinSendMsg(pad->hOwner, msg, mp1, mp2);
         break;
      } // end case WM_COLORWHEELCHANGED
   // spinbutton value has changed -----------------------------------------
      case WM_CONTROL:
         if ((SHORT1FROMMP(mp1) == SPN_RED ||      // if a spin button
              SHORT1FROMMP(mp1) == SPN_GREEN ||    // originated SPBN_CHANGE
              SHORT1FROMMP(mp1) == SPN_BLUE)) {
            PAPPDATA pad = WinQueryWindowPtr(hwnd, 0L);
            ULONG ul;
            switch (SHORT2FROMMP(mp1)) {
               case SPBN_UPARROW:
               case SPBN_DOWNARROW: pad->is.spin = 1; break;
               case SPBN_ENDSPIN: pad->is.spin = 0;
               case SPBN_CHANGE:
                  if (pad->is.spin || !pad->is.upd) break;
                  WinSendMsg(HWNDFROMMP(mp2), SPBM_QUERYVALUE,
                             (MPARAM)&ul, MPFROM2SHORT(0, SPBQ_ALWAYSUPDATE));
                  switch (SHORT1FROMMP(mp1)) {
                     case SPN_RED:   pad->clrCurr.rgb.red = (BYTE)ul; break;
                     case SPN_GREEN: pad->clrCurr.rgb.grn = (BYTE)ul; break;
                     case SPN_BLUE:  pad->clrCurr.rgb.blu = (BYTE)ul; break;
                  } /* endswitch */
                  // update colorwheel
                  WinSendDlgItemMsg(hwnd, CWHEEL, pad->cwSetMsg,
                                    (MPARAM)pad->clrCurr.l, MPVOID);
                  // notify owner
                  WinSendMsg(pad->hOwner, pad->cwChangedMsg,
                             (MPARAM)pad->clrCurr.l, MPVOID);
                  break;
            } // end switch
         } // end if
         break;
   // user clicked on a PUSHBUTTON -----------------------------------------
      case WM_COMMAND: {
         PAPPDATA pad = WinQueryWindowPtr(hwnd, 0L);
         switch ((USHORT)mp1) {
            case BTN_OK:            // OK: return current color
               WinDismissDlg(hwnd, pad->clrCurr.l);
               break;
            case BTN_UNDO:          // UNDO: restore start color
               SetSpins(hwnd, pad->clrUndo);
               WinSendDlgItemMsg(hwnd, CWHEEL, pad->cwSetMsg,
                                 (MPARAM)pad->clrUndo.l, MPVOID);
               break;
            default:                // CANCEL: return start color
               WinSendMsg(pad->hOwner, pad->cwChangedMsg,
                          (MPARAM)pad->clrUndo.l, MPVOID);
               WinDismissDlg(hwnd, pad->clrUndo.l);
               break;
         } /* endswitch */
         break;
      } // end case WM_COMMAND
   // user closed the "edit color" dialog window ---------------------------
      case WM_CLOSE: {              // return start color
         PAPPDATA pad = WinQueryWindowPtr(hwnd, 0L);
         WinSendMsg(pad->hOwner, pad->cwChangedMsg,
                    (MPARAM)pad->clrUndo.l, MPVOID);
         WinDismissDlg(hwnd, pad->clrUndo.l);
         break;
      } // end case WM_CLOSE
      default:
         return WinDefDlgProc(hwnd, msg, mp1, mp2);
   } /* endswitch */
   return (MRESULT)FALSE;
}


//=========================================================================\
// SetSpins() : sets the spinbutton values on initialization and when a    |
//              new color is set through the Color Wheel control           |
// parameters: ----------------------------------------------------------- |
// HWND hwnd  : window handle                                              |
// CLR clr    : RGB color structure containing new RGB values              |
// returned value: ------------------------------------------------------- |
// VOID                                                                    |
//=========================================================================/


VOID SetSpins(HWND hwnd, CLR clr) {
   WinSendDlgItemMsg(hwnd, SPN_RED, SPBM_SETCURRENTVALUE,
                     (MPARAM)clr.rgb.red, MPVOID);
   WinSendDlgItemMsg(hwnd, SPN_GREEN, SPBM_SETCURRENTVALUE,
                     (MPARAM)clr.rgb.grn, MPVOID);
   WinSendDlgItemMsg(hwnd, SPN_BLUE, SPBM_SETCURRENTVALUE,
                     (MPARAM)clr.rgb.blu, MPVOID);
}
