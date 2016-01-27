//========================================================================\
// statusbar.c                                                            |
// FUNZIONI: ------------------------------------------------------------ |
// BOOL APIENTRY RegisterStatusBarClass(HAB hab)                          |
// MRESULT EXPENTRY StatusBarWinProc(HWND hwnd, ULONG msg,                |
//                                   MPARAM mp1, MPARAM mp2)              |
// VOID AdjustSBarWinPos(HWND hwnd, PCTL3D_DATA pcd, PSWP pswp)           |
// VOID DrawStatusBar(HWND hwnd, PCTL3D_DATA pcd)                         |
// VOID DrawStaticTxt(HWND hwnd, PCTL3D_DATA pcd)                         |
// VOID ProcessTextChanged(PCTL3D_DATA pcd, HWND hwnd)                    |
// MESSAGGI: ------------------------------------------------------------ |
// WM_CREATE                                                              |
// WM_ACTIVATE                                                            |
// WM_ADJUSTWINDOWPOS                                                     |
// WM_SETFOCUS                                                            |
// WM_BUTTON1DOWN                                                         |
// WM_MOUSEMOVE                                                           |
// WM_BUTTON1UP                                                           |
// WM_PAINT                                                               |
// WM_QUERYCONVERTPOS                                                     |
// WM_SYSCOLORCHANGE                                                      |
// WM_ENABLE                                                              |
// WM_QUERYDLGCODE                                                        |
// WM_CTLCOLORCHANGE                                                      |
// WM_PRESPARAMCHANGED                                                    |
// WM_SETWINDOWPARAMS                                                     |
// WM_QUERYWINDOWPARAMS                                                   |
// WM_MATCHMNEMONIC                                                       |
// WM_DESTROY                                                             |
//========================================================================/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include "clib.h"
#include "controls.h"


BOOL APIENTRY RegisterStatusBarClass(HAB hab) {
  return WinRegisterClass(hab, WC_STATUSBAR, (PFNWP)StatusBarWinProc,
                          CS_SYNCPAINT | CS_SIZEREDRAW, CTL_RESERVED);
}


MRESULT EXPENTRY StatusBarWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   PCTL3D_DATA pcd;
   switch (msg) {
      //----------------------- inizializzazione-----------------------------
      case WM_CREATE:
         if ((pcd = (PCTL3D_DATA)accmalloc(sizeof(CTL3D_DATA))) == NULL)
            return (MRESULT)TRUE;
         pcd->ulType = CTL_STATUSBAR;
         InitCtl(hwnd, pcd, (PCREATESTRUCT)mp2);
         SetControlColors(hwnd, pcd);
         return (MRESULT)FALSE;
      //----------------------- help contestuale ----------------------------
      case WM_ACTIVATE:
         return ActivateControl(hwnd, SHORT1FROMMP(mp1));
      //---------------------- rilevazione dimensioni -----------------------
      case WM_ADJUSTWINDOWPOS: {
         PSWP pswp = (PSWP)PVOIDFROMMP(mp1);
         pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);
         if (pswp->fl & (SWP_SIZE | SWP_MOVE)) {
            pcd->flPPaint = TRUE;
            pcd->hps = WinGetPS(hwnd);
            AdjustSBarWinPos(hwnd, pcd, pswp);
            WinReleasePS(pcd->hps);
         } // end if
         return WinDefWindowProc(hwnd, msg, MPFROMP(pswp), mp2);
      } // end case WM_ADJUSTWINDOWPOS
      //--------------------- acquisizione focus ----------------------------
      case WM_SETFOCUS:
         return ProcStaticCtlFocus(hwnd, HWNDFROMMP(mp1), LONGFROMMP(mp2));
      //------------------- pressione tasto 1 mouse -------------------------
      case WM_BUTTON1DOWN:
         pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);
         if (!(pcd->ulStyle & STBAR_CLICKABLE))
            return WinDefWindowProc(hwnd, msg, mp1, mp2);
         pcd->fStatus |= ISCLICKED;
         InvalidateControl(hwnd, pcd);
         return (MRESULT)FALSE;
      //------------------------- movimento mouse ---------------------------
      case WM_MOUSEMOVE: {
         HPOINTER hptr = WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, FALSE);
         pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);      
         WinSetPointer(HWND_DESKTOP, hptr);  // resetta pointer arrow normale
         // se non Š status bar cliccabile non processa il messaggio
         if (!(pcd->ulStyle & STBAR_CLICKABLE)) return (MRESULT)FALSE;
         if (!WinIsChild(hwnd, WinQueryActiveWindow(HWND_DESKTOP))) {
            pcd->fStatus = 0;
            RepaintControl(hwnd, pcd, FALSE);
            return (MRESULT)FALSE;
         } // end if
         return ControlMouseMove(hwnd, pcd, SHORT1FROMMP(mp1), SHORT2FROMMP(mp1));
      } // end case WM_MOUSEMOVE
      //-------------------------- rilascio mouse ---------------------------
      case WM_BUTTON1UP:
         pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);
         if (!(pcd->ulStyle & STBAR_CLICKABLE) || !(pcd->fStatus & ISCLICKED))
            return (MRESULT)FALSE;
         return ControlButton1Up(hwnd, pcd, SHORT1FROMMP(mp1), SHORT2FROMMP(mp1));
      //------------------------ ridisegna il controllo --------------------
      case WM_PAINT:
         pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);
         if (pcd->ulStyle & STBAR_STATICTXT)
            DrawStaticTxt(hwnd, pcd);
         else
            DrawStatusBar(hwnd, pcd);
         return (MRESULT)FALSE;
      //------------------------ WM_QUERYCONVERTPOS ------------------------
      case WM_QUERYCONVERTPOS:
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
         pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);      
         if (!(pcd->ulStyle & STBAR_CLICKABLE))
            return (MRESULT)(DLGC_STATIC | DLGC_TABONCLICK);
         return (MRESULT)FALSE;
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
            WinReleasePS(pcd->hps);
            ProcessTextChanged(pcd, hwnd);
//            WinReleasePS(pcd->hps);
         } // end if
         RepaintControl(hwnd, pcd, FALSE);
         return (MRESULT)FALSE;
      //----------------- cambiamento testo controllo ----------------------
      case WM_SETWINDOWPARAMS:
         pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);
         if (!SetControlText(hwnd, pcd, (PWNDPARAMS)mp1))
            return (MRESULT)FALSE;
         ProcessTextChanged(pcd, hwnd);
         // invalida solo background testo
         RepaintControl(hwnd, pcd, TRUE);
         return (MRESULT)TRUE;
      //------------------ lettura testo controllo -------------------------
      case WM_QUERYWINDOWPARAMS:
         if (!mp1) return (MRESULT)FALSE;
         pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);
         return ProcessQueryPresParms(hwnd, (PWNDPARAMS)mp1, pcd);
      //----------------- controllo carattere mnemonico --------------------
      case WM_MATCHMNEMONIC:
         pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);
         if (pcd->ulStyle & DT_MNEMONIC) {     // if DT_MNEMONIC style
            if (lowchar(*pcd->mn.pch) == lowchar((INT)LONGFROMMP(mp1))) {
               if (pcd->ulStyle & STBAR_CLICKABLE) // if STBAR_CLICKABLE style
                  WinPostMsg(pcd->hOwner, WM_COMMAND, MPFROMSHORT((SHORT)pcd->id),
                             MPFROM2SHORT(CMDSRC_OTHER, FALSE)); //WM_COMMAND
               return (MRESULT)TRUE;
            } // end if
         } /* endif */
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
// memorizza posizione e dimensioni finestra, se lo stile Š autosize      |
// calcola le dimensioni                                                  |
//========================================================================/

VOID AdjustSBarWinPos(HWND hwnd, PCTL3D_DATA pcd, PSWP pswp) {
   ULONG ulFrame = (pcd->ulStyle & 0x7)? pcd->ulStyle & 0x7: 1;
   ulFrame *= 2 * (1 + ((pcd->ulStyle & CCTL_FRAME6) >> 3));
   pcd->ptOrig.x = pswp->x;
   pcd->ptOrig.y = pswp->y;
   pcd->hps = WinGetPS(hwnd);
   if (pcd->ulStyle & STBAR_STATICTXT) { // se Š testo statico
      if (pcd->ulStyle & STBAR_AUTOSIZE) { // con stile autosize
         RECTL rcl;
         WinQueryWindowRect(pcd->hParent, &rcl);
         pcd->txt.size.cx = rcl.xRight - pswp->x;
         pcd->txt.size.cy = rcl.yTop - pswp->y;
         MeasureText(pcd, pcd->ulStyle & DT_WORDBREAK, TRUE);
         pcd->rcl.xRight = pswp->cx = pcd->txt.size.cx; // larghezza controllo
         pcd->rcl.yTop = pswp->cy = pcd->txt.size.cy;   // altezza controllo
      } else {
         pcd->txt.size.cx = pcd->rcl.xRight = pswp->cx;
         pcd->txt.size.cy = pcd->rcl.yTop = pswp->cy;
         MeasureText(pcd, pcd->ulStyle & DT_WORDBREAK, FALSE);
      } // end if
   } else {
      pcd->rcl.xRight = pswp->cx;
      pcd->rcl.yTop = pswp->cy;
      pcd->txt.size.cx = pswp->cx - 6 - ulFrame;
      pcd->txt.size.cy = pswp->cy - 2 - ulFrame;
      MeasureText(pcd, pcd->ulStyle & DT_WORDBREAK, FALSE);
   } /* endif */
   WinReleasePS(pcd->hps);
}


//========================================================================\
// disegna sullo schermo la status bar                                    |
//========================================================================/

VOID DrawStatusBar(HWND hwnd, PCTL3D_DATA pcd) {
   ULONG ulStyle = WinQueryWindowULong(hwnd,QWL_STYLE);
   ULONG ulThick = ulStyle & 0x7;
   ULONG ulFrame = (ulStyle & CCTL_FRAME6) >> 3;
   BOOL raised = ulStyle & CCTL_RAISED? 1: 0;
   LONG fShift = 0;             // shift posizione testo
   POINTL aptl[6];

   pcd->hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);
   if ((ulStyle & (CCTL_BORDER7 | CCTL_FRAME6)) ^ // se Š mutato lo stile
       (pcd->ulStyle & (CCTL_BORDER7 | CCTL_FRAME6))) {  // della cornice
      pcd->ulStyle = ulStyle;
      ulStyle = 2 * ulThick * (1 + ulFrame);
      pcd->txt.size.cx = pcd->rcl.xRight - 6 - ulStyle;
      pcd->txt.size.cy = pcd->rcl.yTop - 2 - ulStyle;
      MeasureText(pcd, pcd->ulStyle & DT_WORDBREAK, FALSE);
   } /* endif */
   // modifica valori ulThick e raised secondo stile attuale e posiz mouse
   if ((pcd->fStatus & ISONWINDOW) &&
       (pcd->fStatus & ISCLICKED)) {  // se il mouse Š sul controllo dopo click
      if (!ulThick) {        // se manca bordo, disegna bordo 1 depresso
         ulThick = 1;
         raised = FALSE;
      } /* endif */
   } else if ((pcd->fStatus & ISONWINDOW) ^
              ((pcd->fStatus & ISCLICKED) >> 1)) {
      if (!ulThick) {
         ulThick = 1;
         raised = TRUE;
         fShift = -2;
      } else {
         fShift = ((raised || ulFrame) && !(raised && ulFrame))? +1: -1;
         raised = !(pcd->ulStyle & CCTL_RAISED);
      } /* endif */
   } // endif           --- commuta in modalit… RGB
   GpiCreateLogColorTable(pcd->hps, 0, LCOLF_RGB, 0, 0, NULL);
   if (!(pcd->ulStyle & CCTL_TRANSPARENT)) { // se non stile trasp col sfondo
      GpiSetColor(pcd->hps, pcd->lBack);
      aptl[0].x = pcd->rcl.xRight; aptl[0].y = pcd->rcl.yTop;
      GpiBox(pcd->hps, DRO_FILL, aptl, 0L, 0L);
   // deve ridisegnare la parent window
   } else if (pcd->flPPaint) { // && (pcd->ulStyle & STBAR_CLICKABLE)) {
      RECTL rcl;
      pcd->flPPaint = FALSE;
      rcl.xLeft = pcd->ptOrig.x;
      rcl.yBottom = pcd->ptOrig.y;
      rcl.xRight = rcl.xLeft + pcd->rcl.xRight;
      rcl.yTop = rcl.yBottom + pcd->rcl.yTop;
      WinInvalidateRect(pcd->hParent, &rcl, TRUE);
      WinEndPaint(pcd->hps);
      return;
//   } else {                    // la finestra Š gi… stata disegnata in
//      pcd->flPPaint = TRUE;    // precedenza, resetta la flag e ritorna
   } /* endif */
//========================================================================\
//                                                                        |
//          aptl[1] +------------------------------+ aptl[2]              |
//                  | +---------------------------+ aptl[3]               |
//                  | | aptl[4]                                           |
//                  | + aptl[5]                                           |
//          aptl[0] +                                                     |
//                                                                        |
//========================================================================/
   if (ulThick) {
      aptl[0].x = 0; aptl[0].y = 1;
      aptl[1].x = 0; aptl[1].y = pcd->rcl.yTop - 1;
      aptl[2].x = pcd->rcl.xRight - (ulThick == 1? 1: 2);
      aptl[2].y = aptl[1].y;
      aptl[3].x = aptl[2].x - ulThick + 1; aptl[3].y = aptl[2].y - ulThick + 1;
      aptl[4].x = ulThick - 1; aptl[4].y = aptl[3].y;
      aptl[5].x = aptl[4].x; aptl[5].y = ulThick;
      GpiSetColor(pcd->hps, raised? pcd->lHilite: pcd->lShadow);
      GpiBeginArea(pcd->hps, 0); //BA_NOBOUNDARY | BA_ALTERNATE);
      GpiMove(pcd->hps, aptl);
      GpiPolyLine(pcd->hps, 5, aptl + 1);
      GpiEndArea(pcd->hps);
      if (ulFrame) {  // inizializza valori x disegnare interno frame
         ulFrame *= ulThick;      // distanza bordo interno
         aptl[0].x = ulFrame; aptl[0].y = ulFrame + 1;
         aptl[1].x = aptl[0].x; aptl[1].y = pcd->rcl.yTop - ulFrame - 1;
         aptl[2].x = pcd->rcl.xRight - ulFrame - (ulThick == 1? 1: 2);
         aptl[2].y = aptl[1].y;
         aptl[3].x = aptl[2].x - ulThick + 1; aptl[3].y = aptl[2].y - ulThick + 1;
         aptl[4].x = ulFrame + ulThick - 1; aptl[4].y = aptl[3].y;
         aptl[5].x = aptl[4].x; aptl[5].y = ulFrame + ulThick;
         GpiSetColor(pcd->hps, raised? pcd->lShadow: pcd->lHilite);
         GpiBeginArea(pcd->hps, 0); //BA_NOBOUNDARY | BA_ALTERNATE);
         GpiMove(pcd->hps, aptl);
         GpiPolyLine(pcd->hps, 5, aptl + 1);
         GpiEndArea(pcd->hps);
         aptl[0].x = ulFrame + 1; aptl[0].y = ulFrame;
         aptl[1].x = pcd->rcl.xRight - ulFrame - 1; aptl[1].y = aptl[0].y;
         aptl[2].x = aptl[1].x; aptl[2].y = pcd->rcl.yTop - ulFrame - 2;
         aptl[3].x = aptl[2].x - ulThick + 1; aptl[3].y = aptl[2].y - ulThick + 1;
         aptl[4].x = aptl[3].x; aptl[4].y = ulFrame + ulThick - 1;
         aptl[5].x = ulFrame + ulThick; aptl[5].y = aptl[4].y;
         GpiSetColor(pcd->hps, raised? pcd->lHilite: pcd->lShadow);
         GpiBeginArea(pcd->hps, 0);//BA_NOBOUNDARY | BA_ALTERNATE);
         GpiMove(pcd->hps, aptl);
         GpiPolyLine(pcd->hps, 5, aptl + 1);
         GpiEndArea(pcd->hps);
      } /* endif */
      aptl[0].x = 1; aptl[0].y = 0;
      aptl[1].x = pcd->rcl.xRight - 1; aptl[1].y = 0;
      aptl[2].x = aptl[1].x; aptl[2].y = pcd->rcl.yTop - 2;
      aptl[3].x = aptl[2].x - ulThick + 1; aptl[3].y = aptl[2].y - ulThick + 1;
      aptl[4].x = aptl[3].x; aptl[4].y = ulThick - 1;
      aptl[5].x = ulThick; aptl[5].y = aptl[4].y;
      GpiSetColor(pcd->hps, raised? pcd->lShadow: pcd->lHilite);
      GpiBeginArea(pcd->hps, 0);//BA_NOBOUNDARY | BA_ALTERNATE);
      GpiMove(pcd->hps, aptl);
      GpiPolyLine(pcd->hps, 5, aptl + 1);
      GpiEndArea(pcd->hps);
      aptl[0].x = aptl[0].y = 0;
      GpiMove(pcd->hps, aptl);
   } // end if
   if (*pcd->txt.psz) {     // scrive il testo
      RECTL rcl;
      GpiSetColor(pcd->hps, pcd->lFore);
      ulFrame += ulThick;
      rcl.xLeft = 3 + ulFrame + fShift;
      rcl.xRight = pcd->rcl.xRight - 3 - ulFrame + fShift;
      rcl.yBottom = ulFrame + 1 - fShift;
      rcl.yTop = pcd->rcl.yTop - ulFrame - 1 - fShift;
      DrawText(pcd, pcd->ulStyle & STBAR_VALIDTEXTFLAGS, &rcl);
   } /* endif */
   if (pcd->ulStyle & WS_DISABLED)
      PaintDisabled(pcd);
   WinEndPaint(pcd->hps);
}


//========================================================================\
// disegna testo statico                                                  |
//========================================================================/

VOID DrawStaticTxt(HWND hwnd, PCTL3D_DATA pcd) {
   POINTL ptl;
   pcd->ulStyle = WinQueryWindowULong(hwnd,QWL_STYLE);
   pcd->hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);
   GpiCreateLogColorTable(pcd->hps, 0, LCOLF_RGB, 0, 0, NULL);
   if (!(pcd->ulStyle & CCTL_TRANSPARENT)) { // se non stile trasp col sfondo
      GpiSetColor(pcd->hps, pcd->lBack);
      ptl.x = pcd->rcl.xRight; ptl.y = pcd->rcl.yTop;
      GpiBox(pcd->hps, DRO_FILL, &ptl, 0L, 0L);
   } // end if
   if (*pcd->txt.psz) {   // scrive il testo
      RECTL rcl;
      GpiSetColor(pcd->hps, pcd->lFore);
      DrawText(pcd, pcd->ulStyle & STBAR_VALIDTEXTFLAGS, &pcd->rcl);
   } /* endif */
   if (pcd->ulStyle & WS_DISABLED)
      PaintDisabled(pcd);
   WinEndPaint(pcd->hps);
}


//========================================================================\
// se Š static text autosize forza il ricalcolo delle dimensioni del      |
// controllo. Calcola le dimensioni del testo, aggiorna dati carattere    |
// mnemonico e ridisegna il controllo                                     |
//========================================================================/

VOID ProcessTextChanged(PCTL3D_DATA pcd, HWND hwnd) {
   if ((pcd->ulStyle & STBAR_STATICTXT) && // forza ricalcolo dimensione
       (pcd->ulStyle & STBAR_AUTOSIZE)) {  // controllo in base al testo
      WinSetWindowPos(hwnd, 0, 0, 0, 1, 1, SWP_SIZE);
   } else {
      pcd->hps = WinGetPS(hwnd);
      MeasureText(pcd, pcd->ulStyle & DT_WORDBREAK, FALSE);
      WinReleasePS(pcd->hps);
   } // end if
}  
