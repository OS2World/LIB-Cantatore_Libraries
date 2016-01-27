// 3DGroupBox.c

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include "clib.h"
#include "controls.h"


BOOL APIENTRY RegisterGroupBox3DClass(HAB hab) {
  return WinRegisterClass(hab, WC_3DGROUPBOX, (PFNWP)GroupBox3DWinProc,
                          CS_SYNCPAINT | CS_SIZEREDRAW | CS_HITTEST,
                          CTL_RESERVED);
}


MRESULT EXPENTRY GroupBox3DWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   PCTL3D_DATA pcd;
   switch (msg) {
      //----------------------- inizializzazione-----------------------------
      case WM_CREATE:
         if ((pcd = (PCTL3D_DATA)accmalloc(sizeof(CTL3D_DATA))) == NULL)
            return (MRESULT)TRUE;
         InitCtl(hwnd, pcd, (PCREATESTRUCT)mp2);
         pcd->ulType = CTL_GROUPBOX;
         SetControlColors(hwnd, pcd);
         return (MRESULT)FALSE;
      case WM_ADJUSTWINDOWPOS: {
         PSWP pswp = (PSWP)PVOIDFROMMP(mp1);
         pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);
         if (pswp->fl & (SWP_SIZE | SWP_MOVE)) {
            ULONG ulStyle = WinQueryWindowULong(hwnd,QWL_STYLE);
            ULONG ulThick;
            pcd->flPPaint = TRUE;
            pcd->ptOrig.x = pswp->x;
            pcd->ptOrig.y = pswp->y;
            if ((ulStyle & (GROUP_3DHBAR))) {
               ulThick = 2 * (ulStyle & 0x07);
               if (!ulThick) ulThick = 2;
               if (ulStyle & 0x100) pswp->cx = ulThick;
               else pswp->cy = ulThick;
            } else {
               pcd->hps = WinGetPS(hwnd);
               pcd->txt.size.cy = pcd->txt.cy;
               MeasureText(pcd, FALSE, TRUE);
               WinReleasePS(pcd->hps);
            } // end if
            pcd->rcl.xRight = pswp->cx;
            pcd->rcl.yTop = pswp->cy;
         } // end if
         return WinDefWindowProc(hwnd, msg, MPFROMP(pswp), mp2);
      } // end case WM_ADJUSTWINDOWPOS
      //--------------------- acquisizione focus ----------------------------
      case WM_SETFOCUS:
         return ProcStaticCtlFocus(hwnd, HWNDFROMMP(mp1), LONGFROMMP(mp2));
      //------------------------- movimento mouse ---------------------------
      case WM_MOUSEMOVE: {
         HPOINTER hptr = WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, FALSE);
         WinSetPointer(HWND_DESKTOP, hptr);  // resetta pointer arrow normale
         return (MRESULT)FALSE;
      } // end case WM_MOUSEMOVE
      //------------------------ HITTEST -----------------------------------
      case WM_HITTEST:
         return (MRESULT)HT_TRANSPARENT;
      //------------------------ ridisegna il controllo --------------------
      case WM_PAINT:
         pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);
         pcd->ulStyle = WinQueryWindowULong(hwnd,QWL_STYLE);
         if (pcd->ulStyle & GROUP_3DHBAR)   // bar
            DrawBar(hwnd, pcd);
         else
            DrawGroupBox(hwnd, pcd);
         return (MRESULT)FALSE;
      //------------------------ WM_QUERYCONVERTPOS ------------------------
      case WM_QUERYCONVERTPOS :
         return(MRFROMLONG(QCP_NOCONVERT));
      //------------------------ cambiamento colori sistema ----------------
      case WM_SYSCOLORCHANGE:
         pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);
         SetControlColors(hwnd, pcd);
         RepaintControl(hwnd, pcd, FALSE);
         return (MRESULT)FALSE;
      //--------------- abilitazione/disabilitazione controllo -------------
      case WM_ENABLE:
         pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);
         RepaintControl(hwnd, pcd, FALSE);
         return (MRESULT)FALSE;
      //---------- identificazione controllo dalla dialog procedure --------
      case WM_QUERYDLGCODE:
         return (MRESULT)(DLGC_STATIC | DLGC_TABONCLICK);
      //--------cambiamento colori controllo (solo warp 4) -----------------
#ifdef OS_VER_WARP_4
      case WM_CTLCOLORCHANGE: {
         CTLCOLOR cc[4];
         pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);
         cc[0].clrIndex = CCI_BACKGROUNDDIALOG;
         cc[1].clrIndex = CCI_FOREGROUND;
         cc[2].clrIndex = CCI_BORDERLIGHT;
         cc[3].clrIndex = CCI_BORDERDARK;
         WinQueryControlColors(hwnd, CCT_GROUPBOX, CCF_APPLICATION, 4, cc);
         if (cc[0].clrValue != CCV_NOTFOUND) pcd->lBack = cc[0].clrValue;
         if (cc[1].clrValue != CCV_NOTFOUND) pcd->lFore = cc[1].clrValue;
         if (cc[2].clrValue != CCV_NOTFOUND) pcd->lHilite = cc[2].clrValue;
         if (cc[3].clrValue != CCV_NOTFOUND) pcd->lShadow = cc[3].clrValue;
         cc[0].clrIndex = CCI_ACTIVEBACKGROUNDTEXT;
         cc[1].clrIndex = CCI_ACTIVEFOREGROUND;
         WinQueryControlColors(hwnd, CCT_TITLEBAR, CCF_APPLICATION, 2, cc);
         if (cc[0].clrValue != CCV_NOTFOUND) pcd->lTxtBack = cc[0].clrValue;
         if (cc[1].clrValue != CCV_NOTFOUND) pcd->lTxt = cc[1].clrValue;
         RepaintControl(hwnd, pcd, FALSE);
         return (MRESULT)FALSE;
      } // end WM_CTLCOLORCHANGE
#endif
      //---------------- cambiamento parametri presentazione ---------------
      case WM_PRESPARAMCHANGED:
         pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);
         if (GetNewPresParms(hwnd, pcd,LONGFROMMP(mp1))) {
            pcd->hps = WinGetPS(hwnd);
            UpdateFontData(pcd);
            pcd->txt.size.cy = pcd->txt.cy;
            MeasureText(pcd, FALSE, TRUE);
            WinReleasePS(pcd->hps);
         } // end if
         RepaintControl(hwnd, pcd, FALSE);
         return (MRESULT)FALSE;
      //----------------- cambiamento testo controllo ----------------------
      case WM_SETWINDOWPARAMS:
         pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);
         if (!SetControlText(hwnd, pcd, (PWNDPARAMS)mp1))
            return (MRESULT)FALSE;
         pcd->hps = WinGetPS(hwnd);
         MeasureText(pcd, FALSE, TRUE);
         WinReleasePS(pcd->hps);
         RepaintControl(hwnd, pcd, FALSE);
         return (MRESULT)TRUE;
      //------------------ lettura testo controllo -------------------------
      case WM_QUERYWINDOWPARAMS:
         if (!mp1) return (MRESULT)FALSE;
         pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);
         return ProcessQueryPresParms(hwnd, (PWNDPARAMS)mp1, pcd);
      //----------------- controllo carattere mnemonico --------------------
      case WM_MATCHMNEMONIC:
         pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);
         if ((pcd->ulStyle & DT_MNEMONIC) &&      // if DT_MNEMONIC style
             (lowchar(*pcd->mn.pch) == lowchar((INT)LONGFROMMP(mp1))))
            return (MRESULT)TRUE;
         return (MRESULT)FALSE;
      //------------------- terminazione controllo -------------------------
      case WM_DESTROY:
         pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);
         if (pcd->pdl) accfree(pcd->pdl);
         accfree(pcd->txt.psz);
         accfree(pcd);
         return (MRESULT)FALSE;
   } // end switch
   return WinDefWindowProc(hwnd, msg, mp1, mp2) ;
}


//========================================================================\
// disegna groupbox secondo lo stile prescelto                            |
//========================================================================/

VOID DrawGroupBox(HWND hwnd, PCTL3D_DATA pcd) {
   ULONG ulThick = pcd->ulStyle & 0x7;
   ULONG ulFrame = (pcd->ulStyle & CCTL_FRAME6) >> 3;
   BOOL raised = pcd->ulStyle & CCTL_RAISED? 1: 0;
   POINTL aptl[16];
   RECTL rclTxtInt;             // rettangolo interno testo
   RECTL rclTxtExt;             // rettangolo esterno testo
   ULONG ulTotThick;            // shift posizione testo
   ULONG ulShift;               // ulThick - 1;
   ULONG ulTotShift;            // ulTotThick - 1;
   RECTL rcl;                   // rettangolo testo usato da DrawText
   ULONG flStyle = DT_VCENTER;

   pcd->hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);
   GpiCreateLogColorTable(pcd->hps, 0, LCOLF_RGB, 0, 0, NULL);
   if (!*pcd->txt.psz) {//se non c'Š il testo trasforma opportunamente lo stile
      pcd->ulStyle = (pcd->ulStyle | GROUP_TXTENCLOSED) &
                     ~(GROUP_TXTAUTOSIZE | GROUP_TXTHILITE);
   } else if (pcd->flPPaint) {
      RECTL rcl;
      pcd->flPPaint = FALSE;
      rcl.xLeft = pcd->ptOrig.x;
      rcl.yBottom = pcd->ptOrig.y;
      rcl.xRight = rcl.xLeft + pcd->rcl.xRight;
      rcl.yTop = rcl.yBottom + pcd->rcl.yTop;
      WinInvalidateRect(pcd->hParent, &rcl, TRUE);
      WinEndPaint(pcd->hps);
      return;
   } /* endif */
   if (!ulThick) ulThick = 1;        // profondit… di default
   ulFrame *= ulThick;
   ulTotThick = ulThick + ulFrame;
   ulShift = ulThick - 1;
   ulTotShift = ulTotThick - 1;

//----------------------[ testo circondato da frame ]----------------------|
   if (pcd->ulStyle & GROUP_TXTFRAMED) {
      rclTxtInt.yBottom = pcd->rcl.yTop - pcd->txt.size.cy - 2 - ulTotThick;
      rclTxtInt.yTop = pcd->rcl.yTop - ulTotThick;
      rclTxtExt.yBottom = rclTxtInt.yBottom - ulTotThick;
      if (pcd->ulStyle & GROUP_TXTAUTOSIZE) {
         rclTxtInt.xLeft = (pcd->ulStyle & DT_CENTER)?
                           (pcd->rcl.xRight - pcd->txt.size.cx) / 2 - 3:
                           ((pcd->ulStyle & DT_RIGHT)?
                           pcd->rcl.xRight - pcd->txt.size.cx - ulTotThick - 6:
                           ulTotThick);
         rclTxtInt.xRight = rclTxtInt.xLeft + pcd->txt.size.cx + 6;
         rclTxtExt.xLeft = rclTxtInt.xLeft - ulTotThick;
         rclTxtExt.xRight = rclTxtInt.xRight + ulTotThick;
         flStyle |= (pcd->ulStyle & (DT_MNEMONIC)) | DT_CENTER;
      } else {
         rclTxtInt.xLeft = ulTotThick;
         rclTxtInt.xRight = pcd->rcl.xRight - ulTotThick;
         rclTxtExt.xLeft = 0;
         rclTxtExt.xRight = pcd->rcl.xRight;
         flStyle |= pcd->ulStyle & (DT_CENTER | DT_RIGHT | DT_MNEMONIC);
      } // end if
      // disegna il testo
      if (*pcd->txt.psz) DrawGroupBoxText(pcd, &rclTxtInt, flStyle);
      if (ulFrame) {
         // se opaco disegna sfondo (default grigio chiaro)
         if (!(pcd->ulStyle & CCTL_TRANSPARENT)) {
            aptl[0].x = aptl[0].y = 0;
            aptl[1].x = aptl[0].x; aptl[1].y = rclTxtInt.yBottom - 1;
            aptl[2].x = rclTxtExt.xLeft; aptl[2].y = aptl[1].y;
            aptl[3].x = aptl[2].x; aptl[3].y = pcd->rcl.yTop - 1;
            aptl[4].x = rclTxtExt.xRight - 1;
            aptl[4].y = aptl[3].y;
            aptl[5].x = aptl[4].x; aptl[5].y = aptl[1].y;
            aptl[6].x = pcd->rcl.xRight - 1;
            aptl[6].y = aptl[5].y;
            aptl[7].x = aptl[6].x; aptl[7].y = 0;
            aptl[8].x = aptl[8].y = ulTotThick - 1;
            aptl[9].x = aptl[8].x; aptl[9].y = rclTxtExt.yBottom;
            aptl[10].x = pcd->rcl.xRight - ulTotThick; aptl[10].y = aptl[9].y;
            aptl[11].x = aptl[10].x; aptl[11].y = aptl[8].y;
            aptl[12].x = rclTxtInt.xLeft - 1; aptl[12].y = rclTxtInt.yBottom - 1;
            aptl[13].x = aptl[12].x; aptl[13].y = rclTxtInt.yTop;
            aptl[14].x = rclTxtInt.xRight; aptl[14].y = aptl[13].y;
            aptl[15].x = aptl[14].x; aptl[15].y = aptl[12].y;
            GpiSetColor(pcd->hps, pcd->lBack);
            GpiBeginPath(pcd->hps, 1);
            GpiMove(pcd->hps, aptl);
            GpiPolyLine(pcd->hps, 7, aptl + 1);
            GpiCloseFigure(pcd->hps);
            GpiMove(pcd->hps, aptl + 8);
            GpiPolyLine(pcd->hps, 3, aptl + 9);
            GpiCloseFigure(pcd->hps);
            GpiMove(pcd->hps, aptl + 12);
            GpiPolyLine(pcd->hps, 3, aptl + 13);
            GpiCloseFigure(pcd->hps);
            GpiEndPath(pcd->hps);
            GpiFillPath(pcd->hps, 1, FPATH_ALTERNATE);
         } // end if (!(pcd->ulStyle & CCTL_TRANSPARENT))
         aptl[0].x = 0; aptl[0].y = 1;
         aptl[1].x = aptl[0].x; aptl[1].y = rclTxtInt.yBottom - 1;
         aptl[2].x = rclTxtExt.xLeft; aptl[2].y = aptl[1].y;
         aptl[3].x = aptl[2].x; aptl[3].y = pcd->rcl.yTop - 1;
         aptl[4].x = rclTxtExt.xRight - (ulThick == 1? 1: 2);
         aptl[4].y = aptl[3].y;
         aptl[5].x = aptl[4].x - ulShift; aptl[5].y = aptl[4].y - ulShift;
         aptl[6].x = rclTxtExt.xLeft + ulShift; aptl[6].y = aptl[5].y;
         aptl[7].x = aptl[6].x; aptl[7].y = aptl[2].y - ulShift;
         aptl[8].x = ulShift; aptl[8].y = aptl[7].y;
         aptl[9].x = ulShift; aptl[9].y = ulThick;
         GpiSetColor(pcd->hps, raised? pcd->lHilite: pcd->lShadow);
         GpiBeginArea(pcd->hps, 0);
         GpiMove(pcd->hps, aptl);
         GpiPolyLine(pcd->hps, 9, aptl + 1);
         if (pcd->rcl.xRight - rclTxtExt.xRight) {
            aptl[10].x = rclTxtExt.xRight; aptl[10].y = aptl[1].y;
            aptl[11].x = pcd->rcl.xRight - (ulThick == 1? 1: 2);
            aptl[11].y = aptl[1].y;
            aptl[12].x = aptl[11].x - ulShift; aptl[12].y = aptl[11].y - ulShift;
            aptl[13].x = aptl[10].x - ulShift; aptl[13].y = aptl[10].y - ulShift;
            GpiMove(pcd->hps, aptl + 10);
            GpiPolyLine(pcd->hps, 3, aptl + 11);
         } /* endif */
         GpiEndArea(pcd->hps);
         // hilite interno gruppo
         aptl[0].x = ulFrame + 1; aptl[0].y = ulFrame;
         aptl[1].x = pcd->rcl.xRight - ulFrame - 1; aptl[1].y = aptl[0].y;
         aptl[2].x = aptl[1].x; aptl[2].y = rclTxtExt.yBottom + ulShift - 1;
         aptl[3].x = aptl[2].x - ulShift; aptl[3].y = aptl[2].y - ulShift;
         aptl[4].x = aptl[3].x; aptl[4].y = ulTotShift;
         aptl[5].x = ulTotThick; aptl[5].y = aptl[4].y;
         // hilite interno testo
         aptl[6].x = rclTxtExt.xLeft + ulFrame + 1;
         aptl[6].y = rclTxtInt.yBottom - ulThick;
         aptl[7].x = rclTxtInt.xRight + ulShift; aptl[7].y = aptl[6].y;
         aptl[8].x = aptl[7].x; aptl[8].y = rclTxtInt.yTop + ulShift - 1;
         aptl[9].x = rclTxtInt.xRight; aptl[9].y = rclTxtInt.yTop - 1;
         aptl[10].x = aptl[9].x; aptl[10].y = rclTxtInt.yBottom - 1;
         aptl[11].x = rclTxtInt.xLeft; aptl[11].y = aptl[10].y;
         GpiBeginArea(pcd->hps, 0);
         GpiMove(pcd->hps, aptl);
         GpiPolyLine(pcd->hps, 5, aptl + 1);
         GpiMove(pcd->hps, aptl + 6);
         GpiPolyLine(pcd->hps, 5, aptl + 7);
         GpiEndArea(pcd->hps);
         GpiSetColor(pcd->hps, raised? pcd->lShadow: pcd->lHilite);
         aptl[0].x = 1; aptl[0].y = 0;
         aptl[1].x = pcd->rcl.xRight - 1; aptl[1].y = 0;
         GpiBeginArea(pcd->hps, 0);
         if (pcd->rcl.xRight - rclTxtExt.xRight) {
            aptl[2].x = aptl[1].x; aptl[2].y = rclTxtInt.yBottom - 2;
            aptl[6].x = rclTxtExt.xRight - 1; aptl[6].y = rclTxtInt.yBottom;
            aptl[7].x = aptl[6].x; aptl[7].y = pcd->rcl.yTop - 2;
            aptl[8].x = aptl[7].x - ulShift; aptl[8].y = aptl[7].y - ulShift;
            aptl[9].x = aptl[8].x; aptl[9].y = aptl[6].y - ulShift;
            GpiMove(pcd->hps, aptl + 6);
            GpiPolyLine(pcd->hps, 3, aptl + 7);
         } else {
            aptl[2].x = aptl[1].x; aptl[2].y = pcd->rcl.yTop - 2;
         } // end if (pcd->rcl.xRight - rclTxtExt.xRight)
         aptl[3].x = aptl[1].x - ulShift; aptl[3].y = aptl[2].y - ulShift;
         aptl[4].x = aptl[3].x; aptl[4].y = ulShift;
         aptl[5].x = ulThick; aptl[5].y = aptl[4].y;
         GpiMove(pcd->hps, aptl);
         GpiPolyLine(pcd->hps, 5, aptl + 1);
         GpiEndArea(pcd->hps);
         aptl[0].x = ulFrame; aptl[0].y = ulFrame + 1;
         aptl[1].x = ulFrame; aptl[1].y = rclTxtExt.yBottom + ulShift;
         aptl[2].x = pcd->rcl.xRight - ulFrame - (ulThick == 1? 1: 2);
         aptl[2].y = aptl[1].y;
         aptl[3].x = aptl[2].x - ulShift; aptl[3].y = aptl[2].y - ulShift;
         aptl[4].x = ulTotShift; aptl[4].y = aptl[3].y;
         aptl[5].x = aptl[4].x; aptl[5].y = ulTotThick;
         aptl[6].x = rclTxtInt.xLeft - ulThick;
         aptl[6].y = rclTxtInt.yBottom - ulShift;
         aptl[7].x = aptl[6].x; aptl[7].y = rclTxtInt.yTop + ulShift;
         aptl[8].x = rclTxtInt.xRight + ulShift - (ulThick == 1? 0: 1);
         aptl[8].y = aptl[7].y;
         aptl[9].x = aptl[8].x - ulShift; aptl[9].y = aptl[8].y - ulShift;
         aptl[10].x = rclTxtInt.xLeft - 1; aptl[10].y = aptl[9].y;
         aptl[11].x = aptl[10].x; aptl[11].y = aptl[6].y + ulShift;
         GpiBeginArea(pcd->hps, 0);
         GpiMove(pcd->hps, aptl);
         GpiPolyLine(pcd->hps, 5, aptl + 1);
         GpiMove(pcd->hps, aptl + 6);
         GpiPolyLine(pcd->hps, 5, aptl + 7);
         GpiEndArea(pcd->hps);
      } else {
         if (!(pcd->ulStyle & CCTL_TRANSPARENT)) {
            // reattangolo esterno gruppo
            aptl[0].x = aptl[0].y = 0;
            aptl[1].x = aptl[0].x; aptl[1].y = rclTxtExt.yBottom - 2;
            aptl[2].x = pcd->rcl.xRight - 1; aptl[2].y = aptl[1].y;
            aptl[3].x = aptl[2].x; aptl[3].y = 0;
            aptl[4].x = aptl[4].y = ulShift;
            aptl[5].x = aptl[4].x; aptl[5].y = aptl[1].y - ulShift;
            aptl[6].x = aptl[2].x - ulShift; aptl[6].y = aptl[5].y;
            aptl[7].x = aptl[6].x; aptl[7].y = ulShift;
            aptl[8].x = rclTxtExt.xLeft; aptl[8].y = rclTxtExt.yBottom;
            aptl[9].x = aptl[8].x; aptl[9].y = pcd->rcl.yTop - 1;
            aptl[10].x = rclTxtExt.xRight - 1; aptl[10].y = aptl[9].y;
            aptl[11].x = aptl[10].x; aptl[11].y = aptl[8].y;
            aptl[12].x = aptl[8].x + ulShift; aptl[12].y = aptl[8].y + ulShift;
            aptl[13].x = aptl[12].x; aptl[13].y = aptl[9].y - ulShift;
            aptl[14].x = aptl[10].x - ulShift; aptl[14].y = aptl[13].y;
            aptl[15].x = aptl[14].x; aptl[15].y = aptl[12].y;
            GpiSetColor(pcd->hps, pcd->lBack);
            GpiBeginPath(pcd->hps, 1);
            GpiMove(pcd->hps, aptl);
            GpiPolyLine(pcd->hps, 3, aptl + 1);
            GpiCloseFigure(pcd->hps);
            GpiMove(pcd->hps, aptl + 4);
            GpiPolyLine(pcd->hps, 3, aptl + 5);
            GpiCloseFigure(pcd->hps);
            GpiMove(pcd->hps, aptl + 8);
            GpiPolyLine(pcd->hps, 3, aptl + 9);
            GpiCloseFigure(pcd->hps);
            GpiMove(pcd->hps, aptl + 12);
            GpiPolyLine(pcd->hps, 3, aptl + 13);
            GpiCloseFigure(pcd->hps);
            GpiEndPath(pcd->hps);
            GpiFillPath(pcd->hps, 1, FPATH_ALTERNATE);
         } // end if (!(pcd->ulStyle & CCTL_TRANSPARENT))
         // prima porzione hilite (gruppo)
         aptl[0].x = 0; aptl[0].y = 1;
         aptl[1].x = aptl[0].x; aptl[1].y = rclTxtExt.yBottom - 2;
         aptl[2].x = pcd->rcl.xRight - (ulThick == 1? 1: 2);
         aptl[2].y = aptl[1].y;
         aptl[3].x = aptl[2].x - ulShift; aptl[3].y = aptl[2].y - ulShift;
         aptl[4].x = ulShift; aptl[4].y = aptl[3].y;
         aptl[5].x = aptl[4].x; aptl[5].y = ulThick;
         // porzione hilite testo
         aptl[6].x = rclTxtExt.xLeft; aptl[6].y = rclTxtExt.yBottom + 1;
         aptl[7].x = aptl[6].x; aptl[7].y = pcd->rcl.yTop - 1;
         aptl[8].x = rclTxtExt.xRight - (ulThick == 1? 1: 2);
         aptl[8].y = aptl[7].y;
         aptl[9].x = aptl[8].x - ulShift; aptl[9].y = aptl[8].y - ulShift;
         aptl[10].x = aptl[6].x + ulShift; aptl[10].y = aptl[9].y;
         aptl[11].x = aptl[10].x; aptl[11].y = aptl[6].y + ulShift;
         GpiSetColor(pcd->hps, raised? pcd->lHilite: pcd->lShadow);
         GpiBeginArea(pcd->hps, 0);
         GpiMove(pcd->hps, aptl);
         GpiPolyLine(pcd->hps, 5, aptl + 1);
         GpiMove(pcd->hps, aptl + 6);
         GpiPolyLine(pcd->hps, 5, aptl + 7);
         GpiEndArea(pcd->hps);
         // prima porzione shadow (gruppo)
         aptl[0].x = 1; aptl[0].y = 0;
         aptl[1].x = pcd->rcl.xRight - 1; aptl[1].y = 0;
         aptl[2].x = aptl[1].x; aptl[2].y = rclTxtExt.yBottom - 3;
         aptl[3].x = aptl[2].x - ulShift; aptl[3].y = aptl[2].y - ulShift;
         aptl[4].x = aptl[3].x; aptl[4].y = ulShift;
         aptl[5].x = ulThick; aptl[5].y = aptl[4].y;
         // seconda porzione shadow (testo)
         aptl[6].x = rclTxtExt.xLeft + 1; aptl[6].y = rclTxtExt.yBottom;
         aptl[7].x = rclTxtExt.xRight - 1; aptl[7].y = aptl[6].y;
         aptl[8].x = aptl[7].x; aptl[8].y = pcd->rcl.yTop  - 2;
         aptl[9].x = aptl[8].x - ulShift; aptl[9].y = aptl[8].y - ulShift;
         aptl[10].x = aptl[9].x; aptl[10].y = aptl[7].y + ulShift;
         aptl[11].x = aptl[6].x + ulShift; aptl[11].y = aptl[10].y;
         GpiSetColor(pcd->hps, raised? pcd->lShadow: pcd->lHilite);
         GpiBeginArea(pcd->hps, 0);
         GpiMove(pcd->hps, aptl);
         GpiPolyLine(pcd->hps, 5, aptl + 1);
         GpiMove(pcd->hps, aptl + 6);
         GpiPolyLine(pcd->hps, 5, aptl + 7);
         GpiEndArea(pcd->hps);
      } /* endif */
   } else if (pcd->ulStyle & GROUP_TXTENCLOSED) {    // testo incluso frame
      // disegna il testo
      rclTxtInt.yBottom = pcd->rcl.yTop - pcd->txt.size.cy - 2 - ulTotThick;
      rclTxtInt.yTop = pcd->rcl.yTop - ulTotThick;
      rclTxtExt.yBottom = rclTxtInt.yBottom;
      if (pcd->ulStyle & GROUP_TXTAUTOSIZE) {
         rclTxtInt.xLeft = (pcd->ulStyle & DT_CENTER)?
                           (pcd->rcl.xRight - pcd->txt.size.cx) / 2 - 3:
                           ((pcd->ulStyle & DT_RIGHT)?
                           pcd->rcl.xRight - pcd->txt.size.cx - ulTotThick - 6:
                           ulTotThick);
         rclTxtInt.xRight = rclTxtInt.xLeft + pcd->txt.size.cx + 6;
         rclTxtExt.xLeft = rclTxtInt.xLeft - ulTotThick;
         rclTxtExt.xRight = rclTxtInt.xRight + ulTotThick;
         flStyle |= (pcd->ulStyle & (DT_MNEMONIC)) | DT_CENTER;
      } else {
         rclTxtInt.xLeft = ulTotThick;
         rclTxtInt.xRight = pcd->rcl.xRight - ulTotThick;
         rclTxtExt.xLeft = 0;
         rclTxtExt.xRight = pcd->rcl.xRight;
         flStyle |= pcd->ulStyle & (DT_CENTER | DT_RIGHT | DT_MNEMONIC);
      } // end if
      if (*pcd->txt.psz) DrawGroupBoxText(pcd, &rclTxtInt, flStyle);
      if (!(pcd->ulStyle & CCTL_TRANSPARENT)) {
         aptl[0].x = aptl[0].y = 0;
         aptl[1].x = aptl[0].x; aptl[1].y = rclTxtInt.yBottom + ulTotShift;
         aptl[2].x = rclTxtExt.xLeft; aptl[2].y = aptl[1].y;
         aptl[3].x = aptl[2].x; aptl[3].y = pcd->rcl.yTop - 1;
         aptl[4].x = rclTxtExt.xRight - 1; aptl[4].y = aptl[3].y;
         aptl[5].x = aptl[4].x; aptl[5].y = aptl[1].y;
         aptl[6].x = pcd->rcl.xRight - 1; aptl[6].y = aptl[5].y;
         aptl[7].x = aptl[6].x; aptl[7].y = 0;
         aptl[8].x = aptl[8].y = ulTotShift;
         aptl[9].x = aptl[8].x; aptl[9].y = rclTxtExt.yBottom;
         aptl[10].x = rclTxtInt.xLeft - 1; aptl[10].y = aptl[9].y;
         aptl[11].x = aptl[10].x; aptl[11].y = rclTxtInt.yTop;
         aptl[12].x = rclTxtInt.xRight; aptl[12].y = aptl[11].y;
         aptl[13].x = aptl[12].x; aptl[13].y = aptl[10].y;
         aptl[14].x = pcd->rcl.xRight - ulTotThick; aptl[14].y = aptl[13].y;
         aptl[15].x = aptl[14].x; aptl[15].y = aptl[8].y;
         GpiSetColor(pcd->hps, pcd->lBack);
         GpiBeginPath(pcd->hps, 1);
         GpiMove(pcd->hps, aptl);
         GpiPolyLine(pcd->hps, 7, aptl + 1);
         GpiCloseFigure(pcd->hps);
         GpiMove(pcd->hps, &aptl[8]);
         GpiPolyLine(pcd->hps, 7, aptl + 9);
         GpiCloseFigure(pcd->hps);
         GpiEndPath(pcd->hps);
         GpiFillPath(pcd->hps, 1, FPATH_ALTERNATE);
      } // end if (!(pcd->ulStyle & CCTL_TRANSPARENT))
      aptl[0].x = 0; aptl[0].y = 1;
      aptl[1].x = aptl[0].x; aptl[1].y = rclTxtInt.yBottom  + ulTotShift;
      aptl[2].x = rclTxtExt.xLeft; aptl[2].y = aptl[1].y;
      aptl[3].x = aptl[2].x; aptl[3].y = pcd->rcl.yTop - 1;
      aptl[4].x = rclTxtExt.xRight - (ulThick == 1? 1: 2);
      aptl[4].y = aptl[3].y;
      aptl[5].x = aptl[4].x - ulShift; aptl[5].y = aptl[4].y - ulShift;
      aptl[6].x = rclTxtExt.xLeft + ulShift; aptl[6].y = aptl[5].y;
      aptl[7].x = aptl[6].x; aptl[7].y = aptl[2].y - ulShift;
      aptl[8].x = ulShift; aptl[8].y = aptl[7].y;
      aptl[9].x = ulShift; aptl[9].y = ulThick;
      GpiSetColor(pcd->hps, raised? pcd->lHilite: pcd->lShadow);
      GpiBeginArea(pcd->hps, 0);
      GpiMove(pcd->hps, aptl);
      GpiPolyLine(pcd->hps, 9, aptl + 1);
      if (pcd->rcl.xRight - rclTxtExt.xRight) {
         aptl[10].x = rclTxtExt.xRight; aptl[10].y = aptl[1].y;
         aptl[11].x = pcd->rcl.xRight - (ulThick == 1? 1: 2);
         aptl[11].y = aptl[1].y;
         aptl[12].x = aptl[11].x - ulShift; aptl[12].y = aptl[11].y - ulShift;
         aptl[13].x = aptl[10].x - ulShift; aptl[13].y = aptl[10].y - ulShift;
         GpiMove(pcd->hps, aptl + 10);
         GpiPolyLine(pcd->hps, 3, aptl + 11);
      } /* endif (pcd->rcl.xRight - rclTxtExt.xRight) */
      GpiEndArea(pcd->hps);
      if (ulFrame) {
         GpiBeginArea(pcd->hps, 0);
         aptl[0].x = ulFrame + 1; aptl[0].y = ulFrame;
         aptl[1].x = pcd->rcl.xRight - ulFrame - 1; aptl[1].y = aptl[0].y;
         if (pcd->rcl.xRight - rclTxtExt.xRight) { // se non allineato a dx
            aptl[2].x = aptl[1].x; aptl[2].y = rclTxtInt.yBottom + ulShift - 1;
            aptl[6].x = rclTxtInt.xRight + ulShift;
            aptl[6].y = rclTxtInt.yBottom + ulThick;
            aptl[7].x = aptl[6].x; aptl[7].y = rclTxtInt.yTop + ulShift - 1;
            aptl[8].x = aptl[7].x - ulShift; aptl[8].y = aptl[7].y - ulShift;
            aptl[9].x = aptl[8].x; aptl[9].y = rclTxtInt.yBottom + 1;
            GpiMove(pcd->hps, aptl + 6);
            GpiPolyLine(pcd->hps, 3, aptl + 7);
         } else {
            aptl[2].x = aptl[1].x; aptl[2].y = pcd->rcl.yTop - ulFrame - 2;
         } /* endif (pcd->rcl.xRight - rclTxtExt.xRight) */
         aptl[3].x = aptl[2].x - ulShift; aptl[3].y = aptl[2].y - ulShift;
         aptl[4].x = aptl[3].x; aptl[4].y = ulTotShift;
         aptl[5].x = ulTotThick; aptl[5].y = aptl[4].y;
         GpiMove(pcd->hps, aptl);
         GpiPolyLine(pcd->hps, 5, aptl + 1);
         GpiEndArea(pcd->hps);
      } /* endif */
      GpiSetColor(pcd->hps, raised? pcd->lShadow: pcd->lHilite);
      aptl[0].x = 1; aptl[0].y = 0;
      aptl[1].x = pcd->rcl.xRight - 1; aptl[1].y = 0;
      aptl[2].x = aptl[1].x;
      GpiBeginArea(pcd->hps, 0);
      if (pcd->rcl.xRight - rclTxtExt.xRight) { // se non allineato a destra
         aptl[2].y = rclTxtInt.yBottom + ulTotThick - 2;
         aptl[6].x = rclTxtExt.xRight - 1;
         aptl[6].y = rclTxtInt.yBottom + ulTotThick;
         aptl[7].x = aptl[6].x; aptl[7].y = pcd->rcl.yTop - 2;
         aptl[8].x = aptl[7].x - ulShift; aptl[8].y = aptl[7].y - ulShift;
         aptl[9].x = aptl[8].x; aptl[9].y = aptl[6].y - ulShift;
         GpiMove(pcd->hps, aptl + 6);
         GpiPolyLine(pcd->hps, 3, aptl + 7);
      } else {
         aptl[2].y = pcd->rcl.yTop - 2;
      } // end if (pcd->rcl.xRight - rclTxtExt.xRight)
      aptl[3].x = aptl[1].x - ulShift; aptl[3].y = aptl[2].y - ulShift;
      aptl[4].x = aptl[3].x; aptl[4].y = ulShift;
      aptl[5].x = ulThick; aptl[5].y = aptl[4].y;
      GpiMove(pcd->hps, aptl);
      GpiPolyLine(pcd->hps, 5, aptl + 1);
      GpiEndArea(pcd->hps);
      if (ulFrame) {
         GpiBeginArea(pcd->hps, 0);
         aptl[0].x = ulFrame; aptl[0].y = ulFrame + 1;
         aptl[1].x = ulFrame; aptl[1].y = rclTxtExt.yBottom + ulShift;
         aptl[2].x = rclTxtInt.xLeft - ulThick; aptl[2].y = aptl[1].y;
         aptl[3].x = aptl[2].x; aptl[3].y = rclTxtInt.yTop + ulShift;
         aptl[4].x = rclTxtInt.xRight + ulShift - (ulThick == 1? 0: 1);
         aptl[4].y = aptl[3].y;
         aptl[5].x = aptl[4].x - ulShift; aptl[5].y = aptl[4].y - ulShift;
         aptl[6].x = rclTxtInt.xLeft - 1; aptl[6].y = aptl[5].y;
         aptl[7].x = aptl[6].x; aptl[7].y = rclTxtInt.yBottom;
         aptl[8].x = ulTotShift; aptl[8].y = aptl[7].y;
         aptl[9].x = aptl[8].x; aptl[9].y = ulTotThick;
         if (pcd->rcl.xRight - rclTxtExt.xRight) { // se non allineato a dx
            aptl[10].x = rclTxtInt.xRight + 1; aptl[10].y = rclTxtInt.yBottom;
            aptl[11].x = pcd->rcl.xRight - ulTotThick - (ulThick == 1? 0: 1);
            aptl[11].y = aptl[10].y;
            aptl[12].x = aptl[11].x + ulShift;
            aptl[12].y = aptl[11].y + ulShift;
            aptl[13].x = aptl[10].x + ulShift; aptl[13].y = aptl[12].y;
            GpiMove(pcd->hps, aptl + 10);
            GpiPolyLine(pcd->hps, 3, aptl + 11);
         } // end if (pcd->rcl.xRight - rclTxtExt.xRight)
         GpiMove(pcd->hps, aptl);
         GpiPolyLine(pcd->hps, 9, aptl + 1);
         GpiEndArea(pcd->hps);
      } /* endif (ulFrame) */
   } else {                   // testo non circondato da frame
      // disegna il testo
      rclTxtInt.yBottom = pcd->rcl.yTop - pcd->txt.size.cy - 2;
      rclTxtInt.yTop = pcd->rcl.yTop;
      rclTxtExt.yBottom = rclTxtInt.yBottom - ulTotThick;
      if (pcd->ulStyle & GROUP_TXTAUTOSIZE) {
         rclTxtInt.xLeft = (pcd->ulStyle & DT_CENTER)?
                           (pcd->rcl.xRight - pcd->txt.size.cx) / 2 - 3:
                           ((pcd->ulStyle & DT_RIGHT)?
                           pcd->rcl.xRight - pcd->txt.size.cx - 6: 0);
         rclTxtInt.xRight = rclTxtInt.xLeft + pcd->txt.size.cx + 6;
         rclTxtExt.xLeft = rclTxtInt.xLeft? rclTxtInt.xLeft - ulTotThick: 0;
         rclTxtExt.xRight = (pcd->ulStyle & DT_RIGHT)?
                            pcd->rcl.xRight: rclTxtInt.xRight + ulTotThick;
         flStyle |= (pcd->ulStyle & (DT_MNEMONIC)) | DT_CENTER;
      } else {
         rclTxtInt.xLeft = 0;
         rclTxtInt.xRight = pcd->rcl.xRight;
         rclTxtExt.xLeft = 0;
         rclTxtExt.xRight = pcd->rcl.xRight;
         flStyle |= pcd->ulStyle & (DT_CENTER | DT_RIGHT | DT_MNEMONIC);
      } // end if
      if (*pcd->txt.psz) DrawGroupBoxText(pcd, &rclTxtInt, flStyle);
      if (!(pcd->ulStyle & CCTL_TRANSPARENT)) {
         aptl[0].x = aptl[0].y = 0;
         aptl[1].x = aptl[0].x;
         aptl[1].y = (rclTxtExt.xLeft)?
                     pcd->rcl.yTop - 1: rclTxtInt.yBottom - 1; 
         aptl[2].x = rclTxtInt.xLeft - 1; aptl[2].y = aptl[1].y;
         aptl[3].x = aptl[2].x; aptl[3].y = rclTxtInt.yBottom - 1;
         aptl[4].x = rclTxtInt.xRight; aptl[4].y = aptl[3].y;
         aptl[5].x = aptl[4].x;
         aptl[5].y = (pcd->rcl.xRight - rclTxtExt.xRight)?
                     pcd->rcl.yTop - 1: aptl[4].y;
         aptl[6].x = pcd->rcl.xRight - 1; aptl[6].y = aptl[5].y;
         aptl[7].x = aptl[6].x; aptl[7].y = 0;
         aptl[8].x = aptl[8].y = ulTotShift;
         aptl[9].x = aptl[8].x; aptl[9].y = aptl[1].y - ulTotShift;
         aptl[10].x = rclTxtExt.xLeft; aptl[10].y = aptl[9].y;
         aptl[11].x = aptl[10].x; aptl[11].y = aptl[3].y - ulTotShift;
         aptl[12].x = rclTxtExt.xRight - 1;
         aptl[12].y = aptl[11].y;
         aptl[13].x = aptl[12].x; aptl[13].y = aptl[5].y - ulTotShift;
         aptl[14].x = aptl[6].x - ulTotShift; aptl[14].y = aptl[13].y;
         aptl[15].x = aptl[14].x; aptl[15].y = ulTotShift;
         GpiSetColor(pcd->hps, pcd->lBack);
         GpiBeginPath(pcd->hps, 1);
         GpiMove(pcd->hps, aptl);
         GpiPolyLine(pcd->hps, 7, aptl + 1);
         GpiCloseFigure(pcd->hps);
         GpiMove(pcd->hps, aptl + 8);
         GpiPolyLine(pcd->hps, 7, aptl + 9);
         GpiCloseFigure(pcd->hps);
         GpiEndPath(pcd->hps);
         GpiFillPath(pcd->hps, 1, FPATH_ALTERNATE);
      } // end if (!(pcd->ulStyle & CCTL_TRANSPARENT))
      aptl[0].x = 0; aptl[0].y = 1;
      aptl[1].x = aptl[0].x;
      if (rclTxtExt.xLeft) {
         aptl[1].y = pcd->rcl.yTop - 1;
         aptl[2].x = rclTxtExt.xLeft + ulTotThick - (ulThick == 1? 1: 2);
         aptl[6].x = rclTxtInt.xLeft - ulShift;
      } else {
         aptl[1].y = rclTxtInt.yBottom - 1;
         aptl[2].x = ulTotThick;
         aptl[6].x = aptl[2].x - ulShift;
      } // end if
      aptl[2].y = aptl[1].y;
      aptl[3].x = aptl[2].x - ulShift; aptl[3].y = aptl[2].y - ulShift;
      aptl[4].x = ulShift; aptl[4].y = aptl[3].y;
      aptl[5].x = aptl[4].x; aptl[5].y = ulThick;
      // hilite esterno seconda parte
      aptl[6].y = rclTxtInt.yBottom - ulThick;
      if (pcd->rcl.xRight - rclTxtExt.xRight) {
         aptl[7].x = rclTxtInt.xRight + ulShift;
         aptl[8].y = pcd->rcl.yTop - ulThick;
      } else {
         aptl[7].x = aptl[6].x + ulThick + ulThick;
         aptl[8].y = aptl[6].y;
      } /* endif */
      aptl[7].y = aptl[6].y;
      aptl[8].x = aptl[7].x;
      aptl[9].x = pcd->rcl.xRight - ulShift - (ulThick == 1? 1: 2);
      aptl[9].y = aptl[8].y;
      aptl[10].x = aptl[9].x + ulShift; aptl[10].y = aptl[9].y + ulShift;
      aptl[11].x = aptl[8].x - ulShift; aptl[11].y = aptl[10].y;
      aptl[12].x = aptl[11].x; aptl[12].y = aptl[7].y + ulShift;
      aptl[13].x = aptl[6].x + ulShift; aptl[13].y = aptl[12].y;
      GpiSetColor(pcd->hps, raised? pcd->lHilite: pcd->lShadow);
      GpiBeginArea(pcd->hps, 0);
      GpiMove(pcd->hps, aptl);
      GpiPolyLine(pcd->hps, 5, aptl + 1);
      GpiMove(pcd->hps, aptl + 6);
      GpiPolyLine(pcd->hps, 7, aptl + 7);
      GpiEndArea(pcd->hps);
      if (ulFrame) {
         GpiBeginArea(pcd->hps, 0);
         aptl[0].x = ulFrame + 1; aptl[0].y = ulFrame;
         aptl[1].x = pcd->rcl.xRight - ulFrame - 1; aptl[1].y = aptl[0].y;
         aptl[2].x = aptl[1].x; aptl[2].y = aptl[10].y - ulFrame - 1;
         aptl[3].x = aptl[2].x - ulShift; aptl[3].y = aptl[2].y - ulShift;
         aptl[4].x = aptl[3].x; aptl[4].y = ulTotShift;
         aptl[5].x = ulTotThick; aptl[5].y = aptl[4].y;
         if ((rclTxtExt.xLeft)) { // se non allineato sx
            aptl[6].x = rclTxtExt.xLeft;  aptl[6].y = rclTxtExt.yBottom + 1;
            aptl[7].x = aptl[6].x + ulShift; aptl[7].y = aptl[6].y + ulShift;
            aptl[8].x = aptl[7].x; aptl[8].y = pcd->rcl.yTop - ulFrame - 2;
            aptl[9].x = aptl[6].x; aptl[9].y = aptl[8].y - ulShift;
            GpiMove(pcd->hps, aptl + 6);
            GpiPolyLine(pcd->hps, 3, aptl + 7);
         } /* endif */
         GpiMove(pcd->hps, aptl);
         GpiPolyLine(pcd->hps, 5, aptl + 1);
         GpiEndArea(pcd->hps);
      } /* endif */
      GpiSetColor(pcd->hps, raised? pcd->lShadow: pcd->lHilite);
      GpiBeginArea(pcd->hps, 0);
      aptl[0].x = 1; aptl[0].y = 0;
      aptl[1].x = pcd->rcl.xRight - 1; aptl[1].y = 0;
      aptl[2].x = aptl[1].x;
      aptl[2].y = (pcd->rcl.xRight - rclTxtExt.xRight)?  // se non allineato dx
                  pcd->rcl.yTop - 2: rclTxtInt.yBottom - 2;
      aptl[3].x = aptl[2].x - ulShift; aptl[3].y = aptl[2].y - ulShift;
      aptl[4].x = aptl[3].x; aptl[4].y = ulShift;
      aptl[5].x = ulThick; aptl[5].y = aptl[4].y;
      if (rclTxtExt.xLeft) { // se non allineato sx
         aptl[6].x = rclTxtInt.xLeft - 1;  aptl[6].y = rclTxtInt.yBottom;
         aptl[7].x = aptl[6].x; aptl[7].y = pcd->rcl.yTop - 2;
         aptl[8].x = aptl[7].x - ulShift; aptl[8].y = aptl[7].y - ulShift;
         aptl[9].x = aptl[8].x; aptl[9].y = aptl[6].y - ulShift;
         GpiMove(pcd->hps, aptl + 6);
         GpiPolyLine(pcd->hps, 3, aptl + 7);
      } /* endif */
      GpiMove(pcd->hps, aptl);
      GpiPolyLine(pcd->hps, 5, aptl + 1);
      GpiEndArea(pcd->hps);
      if (ulFrame) {
         // shadow interno groupbox
         GpiBeginArea(pcd->hps, 0);
         aptl[0].x = ulFrame; aptl[0].y = ulFrame + 1;
         aptl[1].x = ulFrame;
         if (rclTxtExt.xLeft) {    // se non allineato sx
            aptl[1].y = pcd->rcl.yTop - ulFrame - 1;
            aptl[2].x = rclTxtExt.xLeft + ulShift - (ulThick == 1? 0: 1);
            aptl[6].x = rclTxtExt.xLeft + ulThick;
         } else {
            aptl[1].y = rclTxtExt.yBottom + ulShift;
            aptl[2].x = ulTotThick + ulShift;
            aptl[6].x = aptl[2].x;
         } /* endif */
         aptl[2].y = aptl[1].y;
         aptl[3].x = aptl[2].x - ulShift; aptl[3].y = aptl[2].y - ulShift;
         aptl[4].x = ulTotShift; aptl[4].y = aptl[3].y;
         aptl[5].x = aptl[4].x; aptl[5].y = aptl[0].y + ulShift;
         // seconda porzione shadow interna
         aptl[6].y = rclTxtExt.yBottom + ulShift;
         if (pcd->rcl.xRight - rclTxtExt.xRight) { // se non allineato dx
            aptl[7].x = rclTxtExt.xRight - ulThick;
            aptl[8].y = pcd->rcl.yTop - ulFrame - 1;
         } else {
            aptl[7].x = aptl[6].x;
            aptl[8].y = aptl[6].y;
         } /* endif */
         aptl[7].y = aptl[6].y;
         aptl[8].x = aptl[7].x;
         aptl[9].x = pcd->rcl.xRight - ulFrame - (ulThick == 1? 1: 2);
         aptl[9].y = aptl[8].y;
         aptl[10].x = aptl[9].x - ulShift; aptl[10].y = aptl[9].y - ulShift;
         aptl[11].x = aptl[8].x + ulShift; aptl[11].y = aptl[10].y;
         aptl[12].x = aptl[11].x; aptl[12].y = aptl[7].y - ulShift;
         aptl[13].x = aptl[6].x - ulShift; aptl[13].y = aptl[12].y;
         GpiMove(pcd->hps, aptl);
         GpiPolyLine(pcd->hps, 5, aptl + 1);
         GpiMove(pcd->hps, aptl + 6);
         GpiPolyLine(pcd->hps, 7, aptl + 7);
         GpiEndArea(pcd->hps);
      } /* endif (ulFrame) */
   } // end if
   WinEndPaint(pcd->hps);
}

BOOL DrawBar(HWND hwnd, PCTL3D_DATA pcd) {
   ULONG ulShift = pcd->ulStyle & 0x7;
   BOOL raised = pcd->ulStyle & CCTL_RAISED? 1: 0;
   POINTL aptl[6];
   pcd->hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);
   GpiCreateLogColorTable(pcd->hps, 0, LCOLF_RGB, 0, 0, NULL);
   if (!(pcd->ulStyle & CCTL_TRANSPARENT)) {
      GpiSetColor(pcd->hps, pcd->lBack);
      GpiBox(pcd->hps, DRO_FILL, ((PPOINTL)&pcd->rcl) + 1, 0L, 0L);
   } // end if
   if (ulShift) --ulShift;  // spessore default = 1
   aptl[0].x = 0; aptl[0].y = 1;
   aptl[1].x = aptl[0].x; aptl[1].y = pcd->rcl.yTop - 1;
   aptl[2].x = pcd->rcl.xRight - 2; aptl[2].y = aptl[1].y;
   aptl[3].x = aptl[2].x - ulShift; aptl[3].y = aptl[2].y - ulShift;
   aptl[4].x = ulShift; aptl[4].y = aptl[3].y;
   aptl[5].x = aptl[4].x; aptl[5].y = ulShift + 1;
   GpiSetColor(pcd->hps, raised? pcd->lHilite: pcd->lShadow);
   GpiBeginArea(pcd->hps, 0);
   GpiMove(pcd->hps, aptl);
   GpiPolyLine(pcd->hps, 5, aptl + 1);
   GpiEndArea(pcd->hps);
   aptl[0].x = 1; aptl[0].y = 0;
   aptl[1].x = pcd->rcl.xRight - 1; aptl[1].y = aptl[0].y;
   aptl[2].x = aptl[1].x; aptl[2].y = pcd->rcl.yTop - 2;
   aptl[3].x = aptl[2].x - ulShift; aptl[3].y = aptl[2].y - ulShift;
   aptl[4].x = aptl[3].x; aptl[4].y = ulShift;
   aptl[5].x = ulShift + 1; aptl[5].y = aptl[4].y;
   GpiSetColor(pcd->hps, raised? pcd->lShadow: pcd->lHilite);
   GpiBeginArea(pcd->hps, 0);
   GpiMove(pcd->hps, aptl);
   GpiPolyLine(pcd->hps, 5, aptl + 1);
   GpiEndArea(pcd->hps);
   WinEndPaint(pcd->hps);
   return 0;
}


// disegna il testo del groupbox

VOID DrawGroupBoxText(PCTL3D_DATA pcd, PRECTL prcl, ULONG flStyle) {
   RECTL rcl = *prcl;
   --rcl.xRight;
   if (pcd->ulStyle & GROUP_TXTHILITE) {     // colora sfondo testo
      GpiSetColor(pcd->hps, pcd->lTxtBack);
      GpiMove(pcd->hps, (PPOINTL)&rcl);
      GpiBox(pcd->hps, DRO_FILL, ((PPOINTL)&rcl) + 1, 0L, 0L);
      GpiSetColor(pcd->hps, pcd->lTxt);
   } else {
      GpiSetColor(pcd->hps, pcd->lFore);
   } /* endif */
   --rcl.yTop;
   if (pcd->ulStyle & GROUP_TXTAUTOSIZE) {
      ++rcl.xRight;
   } else {
      rcl.xLeft += 3;
      rcl.xRight -= 4;
   } // end if
   DrawText(pcd, flStyle, &rcl);
}
