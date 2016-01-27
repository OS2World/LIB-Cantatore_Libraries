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
#include "elbox.h"


// prototipi funzioni



//==========================================================================\
// registrazione classe                                                     |
//==========================================================================/

BOOL APIENTRY EDElbReg(HAB hab) {
   return WinRegisterClass(hab, WC_EXTLISTBOX, (PFNWP)EDElbWinProc,
                           flWCstc & ~CS_PUBLIC, cbWCstc + 4);
}


//==========================================================================\
// procedura controllo extended listbox                                     |
//==========================================================================/

MRESULT EXPENTRY EDElbWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   switch (msg) {
      // messaggi ordinari controllo ----------------------------------------
      case WM_CREATE:
         return (MRESULT)!(elbCreateProc(hwnd, (PELBOXCDATA)mp1,
                                        (PCREATESTRUCT)mp2, SS_TEXT));
//      case WM_ADJUSTWINDOWPOS:
      case WM_WINDOWPOSCHANGED:
         if (((PSWP)mp1)->fl & SWP_SIZE) elbUpdateCtlSize(hwnd, (PSWP)mp1);
         break;
      case WM_PAINT:
         return PaintELB(hwnd);
      // aggiorna testo finestra
      case WM_SETWINDOWPARAMS:
         return elbSetWparms((PELBOX)stGetData(hwnd), (PWNDPARAMS)mp1);
      // restituisce testo controllo (risposta a WinQueryWindowText())
      case WM_QUERYWINDOWPARAMS:
         return elbQueryWparms((PELBOX)stGetData(hwnd), (PWNDPARAMS)mp1);
      // memorizza nuovo colore/font e invalida controllo x aggiornarlo
      case WM_SYSCOLORCHANGE:
         return elbSCchange((PELBOX)stGetData(hwnd));
      case WM_PRESPARAMCHANGED:
         return elbPPchange((PELBOX)stGetData(hwnd), (ULONG)mp1); 
      case WM_DESTROY:
         elbDestroyProc((PELBOX)stGetData(hwnd));
         break;
      case WM_BUTTON1DOWN:   
      case WM_BUTTON2DOWN:
         return elbMouBtn((PELBOX)stGetData(hwnd));
      case WM_SETFOCUS:
         return elbSetFocus(hwnd, mp1, (BOOL)mp2);
      case WM_DELAYFOCUS:
         return elbMouBtn((PELBOX)stGetData(hwnd));
      // messaggi tipici controllo ------------------------------------------
      case ELBXM_QUERYCOLCOUNT:
         return colCount((PELBOX)stGetData(hwnd));
      case ELBXM_QUERYCOLWIDTH:
         return colWidth((PELBOX)stGetData(hwnd), (ULONG)mp1);
      case ELBXM_QUERYMULTICOLWIDTH:
         return multiColWidth((PELBOX)stGetData(hwnd),
                              (PCOLCOORD)mp1, (ULONG)mp2);
      case ELBXM_SETTOPINDEX: {
         PELBOX pelb = (PELBOX)stGetData(hwnd);
         // se item Š fuori dal range valido restituisce errore
         if ((SHORT)mp1 < 0 || (SHORT)mp1 >= pelb->citems)
            return (MRESULT)FALSE;
         // setta l'item come top index, lo seleziona e deseleziona per
         // assicurarsi che acquisti il cursore
         return (MRESULT)
                 (pelb->lbxwprc(pelb->hlbx, LM_SETTOPINDEX, mp1, mp2) &&
                  pelb->lbxwprc(pelb->hlbx, LM_SELECTITEM, mp1, (MPARAM)1) &&
                  pelb->lbxwprc(pelb->hlbx, LM_SELECTITEM, mp1, (MPARAM)0));
      } // end case ELBXM_SETCURSOR:
      case ELBXM_SETCURSOR: {
         PELBOX pelb = (PELBOX)stGetData(hwnd);
         // se item Š fuori dal range valido restituisce errore
         if ((SHORT)mp1 < 0 || (SHORT)mp1 >= pelb->citems)
            return (MRESULT)FALSE;
         // setta l'item come top index, lo seleziona e deseleziona per
         // assicurarsi che acquisti il cursore
         return (MRESULT)
                 (pelb->lbxwprc(pelb->hlbx, LM_SELECTITEM, mp1, (MPARAM)1) &&
                  pelb->lbxwprc(pelb->hlbx, LM_SELECTITEM, mp1, (MPARAM)0));
      } // end case ELBXM_SETCURSOR:
      case ELBXM_REDRAWITEM:
         return elbRedrawItem((PELBOX)stGetData(hwnd), (ULONG)mp1);
      // messaggi ridiretti alla child listbox ------------------------------
      case LM_DELETEALL: {
         PELBOX pelb = (PELBOX)stGetData(hwnd);
         pelb->citems = 0;
         return pelb->lbxwprc(pelb->hlbx, msg, mp1, mp2);
      } // end redirection listbox messages
      case LM_INSERTMULTITEMS:
      case LM_INSERTITEM:
      case LM_DELETEITEM: {
         PELBOX pelb = (PELBOX)stGetData(hwnd);
         MRESULT mr = pelb->lbxwprc(pelb->hlbx, msg, mp1, mp2);
         pelb->citems = (ULONG)pelb->lbxwprc(pelb->hlbx, LM_QUERYITEMCOUNT,
                                             MPVOID, MPVOID);
         return mr;
      } // end redirection listbox messages
      case LM_QUERYITEMCOUNT:
//      case LM_SETTOPINDEX: usare ELBXM_SETCURSOR
      case LM_QUERYSELECTION:
//      case LM_SELECTITEM: usare ELBXM_SETCURSOR
      case LM_SETITEMTEXT:
      case LM_QUERYITEMTEXTLENGTH:
      case LM_QUERYITEMTEXT:
      case LM_SETITEMHANDLE:
      case LM_QUERYITEMHANDLE:
      case LM_SEARCHSTRING:
      case LM_SETITEMHEIGHT:
      case LM_QUERYTOPINDEX:
      case LM_SETITEMWIDTH: {
         PELBOX pelb = (PELBOX)stGetData(hwnd);
         return pelb->lbxwprc(pelb->hlbx, msg, mp1, mp2);
      } // end redirection listbox messages
      case WM_QUERYDLGCODE:
         return (MRESULT)0;
      case WM_CONTROL:
         return (MRESULT)FALSE;
      // memorizza altezza items
      case WM_MEASUREITEM: {
         PELBOX pelb = (PELBOX)stGetData(hwnd);
         pelb->cyitem = (ULONG)WinSendMsg(WinQueryWindow(pelb->hwnd, QW_OWNER),
                                          msg, mp1, mp2);
         return (MRESULT)pelb->cyitem;
      } // end case WM_MEASUREITEM
      // memorizza coordinate rettangolo e indice top item
      case WM_DRAWITEM: {
         PELBOX pelb = (PELBOX)stGetData(hwnd);
         if (((POWNERITEM)mp2)->rclItem.yTop > (pelb->szllb.cy - pelb->cyitem)) {
            memcpy((PVOID)&pelb->rcltopitem,
                   (PVOID)&(((POWNERITEM)mp2)->rclItem), sizeof(RECTL));
            //pelb->topitem = ((POWNERITEM)mp2)->idItem;
         } /* endif */
         return WinSendMsg(WinQueryWindow(pelb->hwnd, QW_OWNER), msg, mp1, mp2);
      } // end case WM_DRAWITEM
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

BOOL elbCreateProc(HWND hwnd, PELBOXCDATA pelbcd,
                   PCREATESTRUCT pc, ULONG style) {
   PELBOX pelb;
   HPS hps;
   // allocate structure for caching some control data
   if (!(pelb = (PELBOX)malloc(sizeof(ELBOX)))) return FALSE;
   memset((PVOID)pelb, 0, sizeof(ELBOX));
   // copy some data
   pelb->hab = WinQueryAnchorBlock(hwnd);
   pelb->hwnd = hwnd;
   pelb->fl = pc->flStyle;
   pelb->szl.cx = pc->cx;
   pelb->szl.cy = pc->cy;
   pelb->id = pc->id;
   // imposta colori controllo
   if (!(hps = WinGetPS(hwnd))) goto error0;
   elbUpdColors(pelb, hps);
   pelb->cyFont = stSizeFont(hps);
   WinReleasePS(hps);
   // interpreta ELBOXCDATA
   if (!setELBctldata(pelb, pelbcd)) goto error1;
   if (elbSetText(pelb, pc->pszText, -1)) *pc->pszText = 0;
   // reset WC_STATIC style
   WinSetWindowBits(hwnd, QWL_STYLE, style, 0xffff);
   WinSetWindowPtr(hwnd, cbWCstc, (PVOID)pelb);
   if (pfnwpWCstc(hwnd, WM_CREATE, (MPARAM)pelbcd, (MPARAM)pc)) goto error2;
   if (!createChild(pelb, pelbcd)) goto error3;
   return TRUE;

   error3:
      WinDestroyWindow(hwnd);
   error2:
      if (pelb->psz) free(pelb->psz);
   error1:
      if (pelb->pcol) free(pelb->pcol);
   error0:
      free(pelb);
      return FALSE;
}


//==========================================================================\
// ridimensiona listbox e ricalcola larghezza colonne                       |
//==========================================================================/

VOID elbUpdateCtlSize(HWND hwnd, PSWP pswp) {
   PELBOX pelb = (PELBOX)stGetData(hwnd);
   if (pelb) {
      pelb->szl.cx = pswp->cx;
      pelb->szl.cy = pswp->cy;
   } // end if
   sizeLBox(pelb);
   WinSetWindowPos(pelb->hlbx, 0, 0, 0, pelb->szllb.cx, pelb->szllb.cy,
                   SWP_SIZE);
   sizeCols(pelb);
}


//==========================================================================\
// disegna le descrizioni delle colonne                                     |
//==========================================================================/

MRESULT PaintELB(HWND hwnd) {
   PELBOX pelb = (PELBOX)stGetData(hwnd);
   HPS hps;
   if (pelb && NULLHANDLE != (hps = WinBeginPaint(hwnd, NULLHANDLE, NULL))) {
      INT i;
      RECTL rcl = {0, pelb->szllb.cy, pelb->szl.cx - 1, pelb->szl.cy - 1};
      ULONG fl = (WinQueryWindowULong(hwnd, QWL_STYLE) & WS_DISABLED)?
                  DT_HALFTONE | DT_MNEMONIC | DT_WORDBREAK:
                  DT_MNEMONIC | DT_WORDBREAK;
      ULONG cchleft;
      ULONG cchdone;
      fl |= pelb->fl & (DT_LEFT | DT_CENTER | DT_RIGHT);
      GpiCreateLogColorTable(hps, 0, LCOLF_RGB, 0, 0, NULL);
      WinFillRect(hps, &rcl, pelb->lbkgnd);
      for (i = 0; i < pelb->ccol; ++i) {
         cchdone = 0;
         cchleft = pelb->pcol[i].cb;
         memcpy((PVOID)&rcl, (PVOID)&pelb->pcol[i].rcl, sizeof(RECTL));
         WinDrawBorder(hps, &rcl, 1, 1, 0, 0, 0x400);
         rcl.xLeft += 2;
         rcl.yBottom++;
         rcl.xRight--;
         rcl.yTop--;
         do {
            cchleft -= (cchdone += WinDrawText(hps, cchleft,
                                              pelb->pcol[i].psz + cchdone,
                                              &rcl,
                                              pelb->lfgnd, pelb->lbkgnd,
                                              fl));
            if ((rcl.yTop -= pelb->cyFont) < rcl.yBottom) break;
         } while (cchleft > 0); // end do
      } /* endfor */
      WinEndPaint(hps);
   } /* endif */
   return (MRESULT)FALSE;
}


//==========================================================================\
// la procedura risponde all'evento cambiando il testo e gli altri dati     |
// della finestra come necessario                                           |
// restituisce TRUE/FALSE = successo/errore                                 |
//==========================================================================/

MRESULT elbSetWparms(PELBOX pelb, PWNDPARAMS pwpm) {
   MRESULT mr = (MRESULT)TRUE;
   if (pwpm->fsStatus & WPM_TEXT) {
      elbSetText(pelb, pwpm->pszText, pwpm->cchText);
   } /* endif */
   if (pwpm->fsStatus & WPM_CTLDATA) {
      mr = (MRESULT)setELBctldata(pelb, pwpm->pCtlData);
   } /* endif */
   WinRefreshWin(pelb->hwnd);
   return mr;
}


//==========================================================================\
// Registra la classe del controllo                                         |
// parametri: ------------------------------------------------------------- |
// HAB hab                                                                  |
// valore restituito: ----------------------------------------------------- |
// BOOL : TRUE/FALSE (successo/errore)                                      |
//==========================================================================/

MRESULT elbQueryWparms(PELBOX pelb, PWNDPARAMS pwpm) {
   INT i;
   // parametri presentazione: usa procedura default
   if (isOrdWndParm(pwpm->fsStatus))
      return(WinDefWindowProc(pelb->hwnd, WM_QUERYWINDOWPARAMS,
                              (MPARAM)pwpm, MPVOID));
   // testo finestra
   if (pwpm->fsStatus & WPM_TEXT) {
      if (pelb->cbtxt) {
         PSZ psz;
         memcpy((PVOID)pwpm->pszText, (PVOID)pelb->psz,
                min(pwpm->cchText, pelb->cbtxt) + 1);
         // sostituisce terminazioni descrizioni singole colonne con '\t'
         for (psz = pwpm->pszText, i = 0; i < pelb->ccol - 1; ++i) {
            psz += pelb->pcol[i].cb;
            *psz = '\t';
         } /* endfor */
      } else {
         if (pwpm->cchText) *(pwpm->pszText) = '\0';
      } /* endif */
   } /* endif */
   // lunghezza testo finestra
   if (pwpm->fsStatus & WPM_CCHTEXT) {
      pwpm->cchText = pelb->cbtxt;
   } /* endif */
   // controldata
   if (pwpm->fsStatus & WPM_CTLDATA) {
      ULONG cb = sizeof(ELBOXCDATA) + (pelb->ccol - 1) * sizeof(USHORT);
      if (pwpm->cbCtlData < cb) return (MRESULT)FALSE;
      ((PELBOXCDATA)pwpm->pCtlData)->cb = cb;
      ((PELBOXCDATA)pwpm->pCtlData)->ccol = pelb->ccol;
      ((PELBOXCDATA)pwpm->pCtlData)->crow = pelb->crow;
      for (i = 0; i < pelb->ccol; ++i) {
         ((PELBOXCDATA)pwpm->pCtlData)->acx[i] = pelb->pcol[i].cx;
      } /* endfor */
   } /* endif */
   if (pwpm->fsStatus & WPM_CBCTLDATA) {
      pwpm->cbCtlData = sizeof(ELBOXCDATA) + (pelb->ccol - 1) * sizeof(USHORT);
   } /* endif */
   return (MRESULT)TRUE;
}


//==========================================================================\
// in seguito a WM_SYSCOLORCHANGE reimposta tutti i colori del controllo    |
// e lo ridisegna                                                           |
//==========================================================================/

MRESULT elbSCchange(PELBOX pelb) {
   HPS hps;
   if (pelb && NULLHANDLE != (hps = WinGetPS(pelb->hwnd))) {
      elbUpdColors(pelb, hps);
      WinReleasePS(hps);
      WinRefreshWin(pelb->hwnd);
   } /* endif */
   return (MRESULT)FALSE;
}


//==========================================================================\
// in seguito a WM_PRESPARAMCHANGED reimposta il colore cambiato e          |
// ridisegna il controllo                                                   |
//==========================================================================/

MRESULT elbPPchange(PELBOX pelb, ULONG fl) {
   HPS hps;
   if (pelb && NULLHANDLE != (hps = WinGetPS(pelb->hwnd))) {
      switch (fl) {
         case PP_FOREGROUNDCOLOR:
         case PP_FOREGROUNDCOLORINDEX:
            pelb->lfgnd = WgetCtlClr(pelb->hwnd, hps, PP_FOREGROUNDCOLOR,
                         PP_FOREGROUNDCOLORINDEX, SYSCLR_WINDOWSTATICTEXT);
            break;
         case PP_BACKGROUNDCOLOR:
         case PP_BACKGROUNDCOLORINDEX:
            pelb->lbkgnd = WgetCtlClr(pelb->hwnd, hps, PP_BACKGROUNDCOLOR,
                         PP_BACKGROUNDCOLORINDEX, SYSCLR_DIALOGBACKGROUND);
            break;
         case PP_FONTNAMESIZE:
            pelb->cyFont = stSizeFont(hps);
            sizeLBox(pelb);
            WinSetWindowPos(pelb->hlbx, 0, 0, 0,
                            pelb->szllb.cx, pelb->szllb.cy, SWP_SIZE);
            sizeCols(pelb);
            break;
      } /* endswitch */
      WinReleasePS(pelb->hwnd);
      WinRefreshWin(pelb->hwnd);
   }
   return (MRESULT)FALSE;
}


//==========================================================================\
// libera le risorse allocate per il controllo                              |
//==========================================================================/

VOID elbDestroyProc(PELBOX pelb) {
   if (pelb) {
      WinDestroyWindow(pelb->hlbx);
      if (pelb->psz) free(pelb->psz);
      if (pelb->pcol) free(pelb->pcol);
      free(pelb);
      _heapmin();
   } /* endif */
}


//==========================================================================\
// da focus alla listbox se il mouse viene premuto sull'intestazione        |
//==========================================================================/

MRESULT elbMouBtn(PELBOX pelb) {
   WinSetFocus(HWND_DESKTOP, pelb->hlbx);
   return (MRESULT)TRUE;
}


//==========================================================================\
// posta messaggio per dare focus alla listbox                              |
//==========================================================================/

MRESULT elbSetFocus(HWND hwnd, MPARAM mp1, BOOL fl) {
   if (fl) WinPostMsg(hwnd, WM_DELAYFOCUS, (MPARAM)hwnd, MPVOID);
   return WinDefWindowProc(hwnd, WM_SETFOCUS, mp1, (MPARAM)fl);
}


//==========================================================================\
// aggiorna i colori del controllo                                          |
//==========================================================================/

VOID elbUpdColors(PELBOX pelb, HPS hps) {
   pelb->lfgnd = WgetCtlClr(pelb->hwnd, hps, PP_FOREGROUNDCOLOR,
                         PP_FOREGROUNDCOLORINDEX, SYSCLR_WINDOWSTATICTEXT);
   pelb->lbkgnd = WgetCtlClr(pelb->hwnd, hps, PP_BACKGROUNDCOLOR,
                         PP_BACKGROUNDCOLORINDEX, SYSCLR_DIALOGBACKGROUND);
}


//==========================================================================\
// setta alcuni parametri del controllo ricavandoli dai CTLDATA             |
//==========================================================================/

BOOL setELBctldata(PELBOX pelb, PELBOXCDATA pelbcd) {
   INT i;
   ULONG cb;

   // CTLDATA Š essenziale per cui in caso di mancanza restituisce errore
   if (!pelbcd) return TRUE;
   // se la finestra Š stata precedentemente creata e la chiamata Š dovuta
   // all'aggiornamento dei controldata resetta precedenti dati
   if (pelb->pcol) free(pelb->pcol);
   pelb->ccol = pelbcd->ccol;           // nuovo numero colonne
   pelb->crow = pelbcd->crow;           // righe testo descriz colonne
   // alloca spazio per dati colonne
   cb = sizeof(COLDATA) * pelb->ccol;
   if (!(pelb->pcol = (PCOLDATA)malloc(cb))) return FALSE;
   memset(pelb->pcol, 0, cb);
   // calcola dimensioni listbox
   if (!pelb->hlbx) sizeLBox(pelb);
   // inizializza dati larghezze (in percentuale del totale) singole colonne
   for (i = 0; i < pelb->ccol; ++i) pelb->pcol[i].cx = pelbcd->acx[i];
   // inizializza coordinate rettangoli riservati a header colonne
   sizeCols(pelb);

   return TRUE;
}


//==========================================================================\
// memorizza il testo del controllo separandolo le descrizioni delle varie  |
// colonne e memorizzandone gli indirizzi                                   |
// parametri: ------------------------------------------------------------- |
// PELBOX pelb: struttura dati controllo                                    |
// PSZ psz: nuovo testo controllo                                           |
// LONG cb: lunghezza nuovo testo                                           |
// valore restituito: ----------------------------------------------------- |
// ULONG : lunghezza nuovo testo controllo                                  |
//==========================================================================/

ULONG elbSetText(PELBOX pelb, PSZ psz, LONG cb) {
   INT i;
   PSZ pend, pmnemo, ptxt;
   if (!pelb) return 0;
   if (pelb->psz) free(pelb->psz);
   if (!psz ||
       !(pelb->cbtxt = cb < 0? strlen(psz): cb) ||
       !(pelb->psz = (PSZ)malloc(pelb->cbtxt + 1))) {
      pelb->cbtxt = 0; 
      pelb->psz = NULL;
      return 0;
   } // end if
   memcpy(pelb->psz, psz, pelb->cbtxt + 1);
   for (ptxt = pelb->psz, i = 0; i < pelb->ccol; ++i) {
      pelb->pcol[i].psz = ptxt;
      if (NULL != (pmnemo = strchr(ptxt, '~'))) pmnemo++;
      if (NULL != (pend = strchr(ptxt, '\t'))) {
         if (pmnemo && pmnemo <= pend) pelb->pcol[i].mnemo = *pmnemo;
         pelb->pcol[i].cb = pend - ptxt;
         *pend = 0;
         ptxt = pend + 1;
      } else { // Š il testo dell'ultima colonna
         if (pmnemo) pelb->pcol[i].mnemo = *pmnemo;
         pelb->pcol[i].cb = strlen(ptxt);
         break;
      }
   } /* endfor */
   return pelb->cbtxt;
}


//==========================================================================\
// calcola dimensioni listbox in base all'altezza del font, del numero di   |
// righe di testo costituenti la descrizione delle colonne e della          |
// dimensione totale del controllo                                          |
//==========================================================================/

VOID sizeLBox(PELBOX pelb) { 
   pelb->szllb.cx = pelb->szl.cx;
   pelb->szllb.cy = pelb->szl.cy - 4 - pelb->crow * pelb->cyFont;
}


//==========================================================================\
// calcola i rettangoli riservati al testo di descrizione delle singole     |
// colonne                                                                  |
//==========================================================================/

VOID sizeCols(PELBOX pelb) {
   INT i;
   ULONG cx = 0;
   ULONG cxtot = pelb->szl.cx;// - WinQuerySysValue(HWND_DESKTOP, SV_CXVSCROLL);
   for (i = 0; i < pelb->ccol - 1; ++i) {
      pelb->pcol[i].rcl.xLeft = cx;
      pelb->pcol[i].rcl.yBottom = pelb->szllb.cy;
      pelb->pcol[i].rcl.xRight = cx + 
                                 (pelb->pcol[i].cx * cxtot) / 100;
      pelb->pcol[i].rcl.yTop = pelb->szl.cy;
      cx = pelb->pcol[i].rcl.xRight;
   } /* endfor */
   // ultima colonna (gli assegna larghezza restante)
   pelb->pcol[i].rcl.xLeft = cx;
   pelb->pcol[i].rcl.yBottom = pelb->szllb.cy;
   pelb->pcol[i].rcl.xRight = cxtot;
   pelb->pcol[i].rcl.yTop = pelb->szl.cy;
}

//==========================================================================\
// restituisce il numero di colonne presenti                                |
//==========================================================================/

MRESULT colCount(PELBOX pelb) {
   return (MRESULT)pelb->ccol;
}


//==========================================================================\
// restituisce la larghezza della colonna icol                              |
//==========================================================================/

MRESULT colWidth(PELBOX pelb, ULONG icol) {
   if (icol > pelb->ccol) return (MRESULT)0;
   return (MRESULT)(pelb->pcol[icol].rcl.xRight -
                    pelb->pcol[icol].rcl.xLeft + 1);
}


//==========================================================================\
// restituisce la larghezza di tutte le colonne                             |
//==========================================================================/

MRESULT multiColWidth(PELBOX pelb, PCOLCOORD pcx, ULONG ccx) {
   INT i;
   if (ccx < pelb->ccol) return(MRESULT)FALSE;
   for (i = 0; i < pelb->ccol; ++i) {
      pcx[i].x0 = pelb->pcol[i].rcl.xLeft;
      pcx[i].x1 = pelb->pcol[i].rcl.xRight;
   } // end for
   return (MRESULT)TRUE;
}


//==========================================================================\
// forza repaint rettangolo item o tutta listbox se ULONG item == 0xffff    |
//==========================================================================/

MRESULT elbRedrawItem(PELBOX pelb, ULONG item) {
   RECTL rcl;
   SHORT top = (SHORT)pelb->lbxwprc(pelb->hlbx, LM_QUERYTOPINDEX,
                                    MPVOID, MPVOID);
   if (item == 0xffff) return (MRESULT)WinRefreshWin(pelb->hlbx);
   if (item < top) return (MRESULT)FALSE;
   rcl.xLeft = 2;
   rcl.yTop = pelb->szllb.cy - pelb->cyitem * (item - top);
   rcl.xRight = pelb->szllb.cx - WgetSysValue(SV_CXVSCROLL);
   rcl.yBottom = rcl.yTop - pelb->cyitem - 2;
   return (MRESULT)WinInvalidateRect(pelb->hlbx, &rcl, FALSE);
}
