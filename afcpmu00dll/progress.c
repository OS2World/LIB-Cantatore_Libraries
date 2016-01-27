//==========================================================================\
// (c) 199 * Alessandro Cantatore * Team OS/2 Italy                         |
// progress.c : mostra una barra di progresso usando un colore o una bitmap |
// funzioni: -------------------------------------------------------------- |
//  |
// API: --------------------------------------------------------------------|
//  |
//==========================================================================/

#pragma strings(readonly)


#define INCL_WIN
#define INCL_DOSPROCESS
#define INCL_DOSMISC
#define INCL_DOSMODULEMGR
#define INCL_GPI
#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "progress.h"
#include "api.h"


//==========================================================================\
// alloca dati e apre dialogo                                               |
// se handle bitmap diverso da zero il valore color viene letto come la     |
// larghezza della bitmap                                                   |
// parametri:                                                               |
// HWND hParent: handle parent window                                       |
// HWND hOwner:  handle owner window                                        |
// HWND hNotify: handle notify window                                       |
// HBITMAP hbmp: handle bitmap                                              |
// LONG color  : indice colore                                              |
// PSZ pszTitle: titolo dialogo                                             |
// valore restituito:                                                       |
// BOOL: TRUE successo, FALSE errore                                        |
//==========================================================================/

BOOL APIENTRY WprogressBar(HWND hParent, HWND hOwner, PPROGRESSDLG ppd) {
   QMSG  qmsg;
   HWND hwnd;
   PAPPDATA pad;
   BOOL rc = FALSE;
   
   // allocate storage for application data
   if (!(pad = (PAPPDATA)malloc(sizeof(APPDATA)))) return FALSE;
   memset(pad, 0, sizeof(APPDATA));
   pad->ppd = ppd;
   // ottiene tempo di inizio
   pad->start = WinGetCurrentTime(pad->hab = WinQueryAnchorBlock(hOwner))
                / 1000;
   // window handles
   if (ppd->fl & PRGSS_BITMAP) {
      SIZEL szl;
      WqueryBitmapSize(pad->ppd->hbmp, &szl);
      pad->cxbmp = szl.cx;
   } /* endif */
   if (NULLHANDLE != (hwnd = WinLoadDlg(hParent, hOwner, ProgressDlgProc,
                                     hmod, DLG_PROGRESS, pad))) {
      if (!ppd->pszPrgrss) ppd->pszPrgrss = SZ_PROGRESS;
      if (!ppd->pszTime) ppd->pszTime = SZ_ELAPSTIME;
      if (ppd->pszStop) WinSetDlgItemText(hwnd, BTN_STOP, ppd->pszStop);
      pad->hOwner = WinQueryWindow(hwnd, QW_OWNER);
      WinEnableWindow(pad->hOwner, FALSE);
      for(;;) {
         if (WinGetMsg(pad->hab, &qmsg, NULLHANDLE, 0, 0)) {    // loop standard
            if (qmsg.msg == PRGSM_END && qmsg.hwnd == hwnd) {
               if (pad->qQuit.msg)
                  WinPostMsg(pad->qQuit.hwnd, pad->qQuit.msg,
                             pad->qQuit.mp1, pad->qQuit.mp2);
               break;
            } // end if   
            WinDispatchMsg(pad->hab, &qmsg);
         } else {                   // WM_QUIT
            pad->qQuit = qmsg;
            if(qmsg.hwnd == NULLHANDLE)                       // SHUTDOWN
               WinPostMsg(hwnd, WM_CLOSE, MPFROMLONG(TRUE), NULL);
            else if(qmsg.hwnd == HWNDFROMMP(qmsg.mp2))        // TASKLIST
               WinPostMsg(hwnd, WM_CLOSE, 0L, 0L);
            else                            // chiusura regolare: termina
               break;
         } // end if
      } // end forever
      WinSetFocus(HWND_DESKTOP, hOwner);
      WinDestroyWindow(hwnd);
      WinEnableWindow(pad->hOwner, TRUE);
      rc = TRUE;
   } // end if
   free(pad);
   return rc;
}


//==========================================================================\
// procedura dialogo progress bar                                           |
// messaggi:                                                                |
// WM_INITDLG                                                               |
// WM_DRAWITEM                                                              |
// PRGSM_INCREMENT                                                          |
// WM_COMMAND                                                               |
// WM_CLOSE                                                                 |
//==========================================================================/

MRESULT EXPENTRY ProgressDlgProc(HWND hwnd,
                                 ULONG msg,
                                 MPARAM mp1,
                                 MPARAM mp2) {
   switch (msg) {
   // init -----------------------------------------------------------------
      case WM_INITDLG: {
         PAPPDATA pad = (PAPPDATA)mp2;
         ULONG ul;
         POINTL aptl[] = {{1, 36}, {220, 24}}; 
         WinSetWindowPtr(hwnd, 0L, (PVOID)mp2);
         WinSetWindowText(hwnd, pad->ppd->pszTitle);
         // dimensioni-coordinate varie
         pad->cyTitle = WgetSysValue(SV_CYTITLEBAR);
         pad->hButton = WinWindowFromID(hwnd, BTN_STOP);
         WinQueryWindowPos(pad->hButton, &pad->swp);
         WinQueryWindowRect(hwnd, &pad->rcl);
         WinMapDlgPoints(hwnd, aptl, 2, TRUE);
         pad->sldd.cbSize = sizeof(SLDCDATA);
         pad->sldd.usScale1Increments = 11;
         pad->sldd.usScale1Spacing = (pad->rcl.xRight - 2 * pad->cyTitle) / 10 - 1;
         pad->hProgress = WinCreateWindow(hwnd, WC_SLIDER, (PSZ)NULL,
                                          SLS_BOTTOM | SLS_OWNERDRAW |
                                          SLS_READONLY | SLS_RIBBONSTRIP |
                                          WS_VISIBLE, 3, aptl[0].y,
                                          pad->rcl.xRight - 6, aptl[1].y,
                                          hwnd, HWND_TOP, SLDR_PROGRESS,
                                          (PVOID)&pad->sldd, NULL);
         // controlla versione S.O. e setta font slider e bottone
         DosQuerySysInfo(QSV_VERSION_MINOR, QSV_VERSION_MINOR,
                         &ul, sizeof(ULONG));
         if (ul < 40) {
            WinSetPresParam(pad->hProgress, PP_FONTNAMESIZE, 7, "8.Helv");
            WinSetPresParam(pad->hButton, PP_FONTNAMESIZE, 7, "8.Helv");
         } else {
            WinSetPresParam(pad->hProgress, PP_FONTNAMESIZE, 11, "9.WarpSans");
         } /* endif */
         // rileva larghezza progress bar
         pad->cxPrgs = LOUSHORT((ULONG)WinSendMsg(pad->hProgress,
                                                 SLM_QUERYSLIDERINFO,
                                         MPFROM2SHORT(SMA_SHAFTDIMENSIONS, 0),
                                         MPVOID)) - 6;
         // setta altezza barra pari a altezza barra titolo
         WinSendMsg(pad->hProgress, SLM_SETSLIDERINFO,
                    MPFROM2SHORT(SMA_SHAFTDIMENSIONS, 0), (MPARAM)pad->cyTitle);
         // setta testo e altezza barra graduata
         WinSendMsg(pad->hProgress, SLM_SETTICKSIZE,
                    MPFROM2SHORT(SMA_SETALLTICKS, pad->cyTitle / 3), MPVOID);
         WinSendMsg(pad->hProgress, SLM_SETSCALETEXT, (MPARAM)0, (MPARAM)"0%");
         WinSendMsg(pad->hProgress, SLM_SETSCALETEXT, (MPARAM)5, (MPARAM)"50%");
         WinSendMsg(pad->hProgress, SLM_SETSCALETEXT, (MPARAM)10, (MPARAM)"100%");
         // se specificato setta nuovo font dialogo
         if (pad->ppd->FontNameSize)
            WsetDlgFonts(hwnd, NULLHANDLE, pad->ppd->FontNameSize);
         // posiziona finestra al centro Parent window
         if (pad->ppd->fl & PRGSS_CENTER) {
            WcenterInWindow(hwnd, pad->ppd->hPos);
         } else {
            SWP swp;
            swp.hwnd = pad->ppd->hPos;
            swp.x = pad->ppd->ptl.x;
            swp.y = pad->ppd->ptl.y;
            swp.cx = swp.cy = 0;
            swp.fl = SWP_MOVE | SWP_SHOW | SWP_ACTIVATE | SWP_ZORDER;
            swp.hwndInsertBehind = HWND_TOP;
            WsetRelativePos(hwnd, &swp);
         } // end if
         // notifica creazione dialogo
         WinPostMsg(pad->ppd->hNotify, WM_PRGRSREADY, (MPARAM)hwnd, (MPARAM)0);
         break;
      } // end case WM_INITDLG
      case WM_DRAWITEM:
         if (((POWNERITEM)mp2)->idItem == SDA_RIBBONSTRIP) {
            PAPPDATA pad = GetData(hwnd);
            if (pad->ppd->fl & PRGSS_BITMAP) {   // se deve disegnare la bitmap
               RECTL rcl = {0, 0, ((POWNERITEM)mp2)->rclItem.xRight -
                            ((POWNERITEM)mp2)->rclItem.xLeft, pad->cyTitle};
               POINTL ptl = {((POWNERITEM)mp2)->rclItem.xLeft,
                             ((POWNERITEM)mp2)->rclItem.yBottom};
               while (ptl.x < ((POWNERITEM)mp2)->rclItem.xRight) {
                  WinDrawBitmap(((POWNERITEM)mp2)->hps, pad->ppd->hbmp, &rcl,
                                &ptl, 0L, 0L, DBM_NORMAL);
                  ptl.x += pad->cxbmp;
                  rcl.xRight = ((POWNERITEM)mp2)->rclItem.xRight - ptl.x;
               } /* endwhile */
            } else {                       // colore
               GpiCreateLogColorTable(((POWNERITEM)mp2)->hps,
                                      0, LCOLF_RGB, 0, 0, NULL);
               WinFillRect(((POWNERITEM)mp2)->hps,
                           &((POWNERITEM)mp2)->rclItem, pad->ppd->color);
            } /* endif */
            return (MRESULT)TRUE;
         } // end if
         break;
      case PRGSM_INCREMENT: { // notifica incremento
         PAPPDATA pad = GetData(hwnd);
         ULONG ul;
         pad->incr = (ULONG)mp1;
         WinSendMsg(pad->hProgress,
                    SLM_SETSLIDERINFO,
                    MPFROM2SHORT(SMA_SLIDERARMPOSITION, SMA_RANGEVALUE),
                    (MPARAM)((pad->incr * pad->cxPrgs) / 1000));
         ul = WinGetCurrentTime(pad->hab) / 1000;
         if (pad->current != ul) {
            pad->current = ul;
            pad->elapsed = pad->current - pad->start;
            sprintf(pad->achElapsed, "%s: %02d:%02d:%02d", pad->ppd->pszTime,
                    pad->elapsed / 3600, (pad->elapsed % 3600) / 60,
                    pad->elapsed % 60);
            WinSetDlgItemText(hwnd, TXT_TIME, pad->achElapsed);
         } // end if
         if (pad->ulCurPrg != (ul = (ULONG)mp1 / 10)) {
            pad->ulCurPrg = ul;
            sprintf(pad->achCurPrg, "%s: %3d %%",
                    pad->ppd->pszPrgrss, pad->ulCurPrg);
            WinSetDlgItemText(hwnd, TXT_PERCENT, pad->achCurPrg);
         } // end if
         return (MRESULT)pad->flStop;
      } // end case PRGSM_INCREMENT
      case PRGSM_MSG:
         return (MRESULT)WMsg(hwnd, (PSZ)mp1, (ULONG)mp2);
      case WM_COMMAND:
         if ((ULONG)mp1 == BTN_STOP) {
            PAPPDATA pad = GetData(hwnd);
            pad->flStop = TRUE;
            WinEnableWindow(pad->hButton, FALSE);
         } /* endif */
         break;
      case WM_CLOSE: {
         PAPPDATA pad = GetData(hwnd);
         // se elaborazione ancora in corso mostra messaggio
         if (pad->incr > 0 && pad->incr < 1000) {
            PSZ psz;
            if (!(psz = (PSZ)malloc(1024)) ||
            // se la notify window restituisce NULL
                (BOOL)WinSendMsg(pad->ppd->hNotify, WM_PRGRSQUIT,
                                 (MPARAM)psz, MPVOID) ||
            // o se la chiusura Š confermata
                MBID_YES == WMsg(hwnd, psz,
                                 MB_YESNO | MB_ICONQUESTION | MB_MOVEABLE)) {
               WinPostMsg(hwnd, PRGSM_END, MPVOID, MPVOID);
               WinPostMsg(pad->hOwner, WM_CLOSE, MPVOID, MPVOID);
            // se nel frattempo Š terminato
            } else {
               pad->qQuit.msg = 0;
               if (!pad->incr || pad->incr >= 1000) {
                  WinPostMsg(hwnd, PRGSM_END, MPVOID, MPVOID);
               } else if ((BOOL)mp1) {
                  WinCancelShutdown((HMQ)WinQueryWindowULong(hwnd, QWL_HMQ),
                                    FALSE);
               } // end if
            } // end if
            free (psz);
         } /* endif */
      }  break;
      default:
         return WinDefDlgProc(hwnd, msg, mp1, mp2);
   } /* endswitch */
   return (MRESULT)FALSE;
}


ULONG WMsg(HWND hwnd, PSZ psz, ULONG fl) {
   PSZ pszMsg;
   if (!psz) {
      pszMsg = "";
   } else if (!(pszMsg = strchr(psz, '\t'))) {
      pszMsg = psz;
      psz = NULL;
   } else if (pszMsg == psz) {
      psz = NULL;
      ++pszMsg;
   } else {
      *pszMsg++ = 0;
   } // end if
   return WinMessageBox(HWND_DESKTOP, hwnd, pszMsg, psz, 0, fl);
}


PAPPDATA GetData(HWND hwnd) {
   return (PAPPDATA)WinQueryWindowPtr(hwnd, 0);
}
