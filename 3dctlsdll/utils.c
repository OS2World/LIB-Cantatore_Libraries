//========================================================================\
// utils.c                                                                |
// VOID InitCtl(HWND hwnd, PCTL3D_DATA pcd, PCREATESTRUCT pcrst);         |
// MRESULT ActivateControl(HWND hwnd, SHORT flag);                        |
// MRESULT ProcStaticCtlFocus(HWND hwnd, HWND hFocus, BOOL isGetting);    |
// MRESULT ControlMouseMove(HWND hwnd, PCTL3D_DATA pcd, SHORT sx, SHORT sy);
// MRESULT ControlButton1Up(HWND hwnd, PCTL3D_DATA pcd, SHORT sx, SHORT sy);
// LONG GetCtlColor(HWND hwnd, ULONG ulID1, ULONG ulID2, LONG lSysDef,    |
//                  LONG lDef);                                           |
// VOID SetControlColors(HWND hwnd, PCTL3D_DATA pcd);                     |
// BOOL GetNewPresParms(HWND hwnd, PCTL3D_DATA pcd, LONG lIdx);           |
// BOOL SetControlText(HWND hwnd, PCTL3D_DATA pcd, PWNDPARAMS pwprm);     |
// MRESULT ProcessQueryPresParms(HWND hwnd, PWNDPARAMS pwprm,             |
//                               PCTL3D_DATA pcd);                        |
// VOID UpdateFontData(PCTL3D_DATA pcd);                                  |
// VOID RepaintControl(HWND hwnd, PCTL3D_DATA pcd);                       |
// VOID InvalidateControl(HWND hwnd, PCTL3D_DATA pcd);                    |
// VOID MeasureText(PCTL3D_DATA pcd, BOOL flBreak, BOOL flAsz);           |
// VOID DrawText(PCTL3D_DATA pcd, ULONG flStyle, PRECTL prcl);            |
// VOID PaintDisabled(PCTL3D_DATA pcd);                                   |
//========================================================================/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include "clib.h"
#include "controls.h"


//========================================================================\
// funzione di inizializzazione per statusbar, groupbox 3D                |
//========================================================================/

VOID InitCtl(HWND hwnd, PCTL3D_DATA pcd, PCREATESTRUCT pcrst) {

   WinSetWindowPtr(hwnd, QWL_CTL, (PVOID)pcd);
   pcd->id = pcrst->id;
   pcd->hOwner = pcrst->hwndOwner;
   pcd->hParent = pcrst->hwndParent;
   pcd->txt.psz = (PSZ)accmalloc(1 + accstrlen(pcrst->pszText));
   accstrcpy(pcd->txt.psz, pcrst->pszText);
   pcd->ulStyle = pcrst->flStyle;
   pcd->isDlg = WinQueryWindowUShort(pcd->hParent, QWS_FLAGS) & FF_DIALOGBOX;
//   pcd->flPPaint = TRUE;//flag usata x ridisegnare sfondo se stile trasparente
   pcd->hps = WinGetPS(hwnd);
   UpdateFontData(pcd);
   WinReleasePS(pcd->hps);
}


//========================================================================\
// risponde a WM_ACTIVATE attivando l'help in linea                       |
//========================================================================/

MRESULT ActivateControl(HWND hwnd, SHORT flag) {
   HWND hwndHelp;
   PCTL3D_DATA pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);

   if ((hwndHelp = WinQueryHelpInstance(hwnd)) != NULLHANDLE) {
      if (flag)
         WinSendMsg(hwndHelp, HM_SET_ACTIVE_WINDOW,
                    MPFROMHWND(pcd->hParent), MPFROMHWND(pcd->hParent));
      else
         WinSendMsg(hwndHelp, HM_SET_ACTIVE_WINDOW, 0L, 0L);
   } // end if
   return (MRESULT)FALSE;
}


//========================================================================\
// da il focus al controllo successivo se lo stile e controllo statico    |
//========================================================================/

MRESULT ProcStaticCtlFocus(HWND hwnd, HWND hFocus, BOOL isGetting) {
   PCTL3D_DATA pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);
   if (isGetting) {
      HWND hNext = WinQueryWindow(hwnd, QW_NEXT);
      WinSetFocus(HWND_DESKTOP, (hNext? hNext: pcd->hParent));
   } else if (pcd->fStatus) {
     pcd->fStatus = 0;
     RepaintControl(hwnd, pcd, FALSE);
   } // end if
   return (MRESULT)FALSE;
}


//========================================================================\
// processa messaggi relativi al movimento del mouse se lo stile Š        |
// clickable bar o bottone                                                |
//========================================================================/

MRESULT ControlMouseMove(HWND hwnd, PCTL3D_DATA pcd, SHORT sx, SHORT sy) {
   if (!(pcd->fStatus & ISCAPTURED)) {    // se il mouse non Š captured
      WinSetCapture(HWND_DESKTOP, hwnd);  // cattura mouse
      pcd->fStatus |= ISCAPTURED | ISONWINDOW;// setta flag stato
      InvalidateControl(hwnd, pcd);
      return (MRESULT)FALSE;
   } // end if
   if ((sx >= 0) && (sx <= pcd->rcl.xRight) &&
       (sy >= 0) && (sy <= pcd->rcl.yTop)) {
      if (pcd->fStatus & ISONWINDOW)           // se lo stato Š immutato
         return (MRESULT)FALSE;                // non lo ridisegna
      else
         pcd->fStatus |= ISONWINDOW;
   } else {                                    // se Š all'esterno
      if (!(pcd->fStatus & ISCLICKED)) {       // e non Š stato cliccato
         WinSetCapture(HWND_DESKTOP, NULLHANDLE); // libera il mouse
         pcd->fStatus &= ~ISCAPTURED & ~ISONWINDOW; // resetta flag status
      } else {
         if (!(pcd->fStatus & ISONWINDOW))     // stato immutato
            return (MRESULT)FALSE;             // non lo ridisegna
         else
            pcd->fStatus &= ~ISONWINDOW;       // resetta flag status
      } // end if
   } // end if
   InvalidateControl(hwnd, pcd);
   return (MRESULT)FALSE;
}


//========================================================================\
// processa messaggio relativo al rilascio del mouse                      |
//========================================================================/

MRESULT ControlButton1Up(HWND hwnd, PCTL3D_DATA pcd, SHORT sx, SHORT sy) {
   WinSetCapture(HWND_DESKTOP, NULLHANDLE);   // libera il mouse
   pcd->fStatus &= ~ISCLICKED & ~ISCAPTURED;  // resetta flags
   InvalidateControl(hwnd, pcd);
   // Post WM_COMMAND if mouse pointer is in window
   if ((sx >= 0) && (sx <= pcd->rcl.xRight) &&
       (sy >= 0) && (sy <= pcd->rcl.yTop))
      WinSendMsg(pcd->hOwner, WM_COMMAND, MPFROMSHORT((SHORT)pcd->id),
                 MPFROM2SHORT(CMDSRC_OTHER, TRUE));
   return (MRESULT)FALSE;
}


//========================================================================\
// la funzione ricerca il parametro di presentazione ulID1 o ulID2 (index)|
// se non lo trova prova ad impostare un colore di sistema, se fallisce   |
// imposta il colore di default                                           |
// parametri:                                                             |
// ULONG ulID1 parametro di presentazione colore RGB                      |
// ULONG ulID2 parametro di presentazione indice colore corrisp al preced |
// LONG lSysDef colore di sistema                                         |
// LONG lDef   colore default                                             |
//========================================================================/

LONG GetCtlColor(HWND hwnd, ULONG ulID1, ULONG ulID2, LONG lSysDef, LONG lDef) {
   HPS hps;
   LONG lClr;
   ULONG ulID;

   if (WinQueryPresParam(hwnd, ulID1, ulID2, &ulID, sizeof(LONG),	(PVOID)&lClr,
                         QPF_NOINHERIT | QPF_ID2COLORINDEX | QPF_PURERGBCOLOR)) {
      return(lClr);            // rileva eventuale parametro presentazione
   } else if ((lSysDef >= SYSCLR_SHADOWHILITEBGND) &&       // altrimenti
            (lSysDef <= SYSCLR_HELPHILITE)) {
      return(WinQuerySysColor(HWND_DESKTOP, lSysDef, 0L));  // val default
   } else if ((hps = WinGetPS(hwnd)) != (HPS)NULL) {
      if ((lClr =	GpiQueryRGBColor(hps, LCOLOPT_REALIZED, lSysDef)) ==
          GPI_ALTERROR ) {
         WinReleasePS(hps);
         return(lDef);
	   } else {
         WinReleasePS(hps);
         return(lClr);
      } // end if
   } else {
      return(lDef);
   } // end if
}


//========================================================================\
// Imposta i colori del controllo                                         |
//========================================================================/

VOID SetControlColors(HWND hwnd, PCTL3D_DATA pcd) {
#ifdef OS_VER_WARP_4 // warp 4 e superiori
   pcd->lHilite = GetCtlColor(hwnd, PP_BORDERLIGHTCOLOR, 0,
                               SYSCLR_BUTTONLIGHT, RGB_WHITE);
   pcd->lShadow = GetCtlColor(hwnd, PP_BORDERDARKCOLOR, 0,
                               SYSCLR_BUTTONDARK, RGB_DARKGRAY);
#else                // warp 3
   pcd->lHilite = GetCtlColor(hwnd, PP_HILITEFOREGROUNDCOLOR,
                               PP_HILITEFOREGROUNDCOLORINDEX,
                               SYSCLR_BUTTONLIGHT, RGB_WHITE);
   pcd->lShadow = GetCtlColor(hwnd, PP_BORDERCOLOR,
                               PP_BORDERCOLORINDEX,
                               SYSCLR_BUTTONDARK, RGB_DARKGRAY);
#endif
   pcd->lBack = GetCtlColor(hwnd, PP_BACKGROUNDCOLOR,
                             PP_BACKGROUNDCOLORINDEX,
                             (pcd->isDlg? SYSCLR_DIALOGBACKGROUND:
                             SYSCLR_WINDOW), RGB_GRAY);
   if (pcd->ulType != CTL_BUTTON) {
      pcd->lFore = GetCtlColor(hwnd, PP_FOREGROUNDCOLOR,
                                PP_FOREGROUNDCOLORINDEX,
                                SYSCLR_WINDOWSTATICTEXT, RGB_DARKBLUE);

   } else {
      pcd->lFore = GetCtlColor(hwnd, PP_FOREGROUNDCOLOR,
                                PP_FOREGROUNDCOLORINDEX,
                                SYSCLR_MENUTEXT, RGB_BLACK);

   } /* endif */
   // stili propri groupbox 3D
   if (pcd->ulType == CTL_GROUPBOX && !(pcd->ulStyle & GROUP_3DHBAR)) {
      pcd->lTxtBack = GetCtlColor(hwnd, PP_ACTIVETEXTBGNDCOLOR,
                                    PP_ACTIVETEXTBGNDCOLORINDEX,
                                    SYSCLR_ACTIVETITLETEXTBGND, RGB_WHITE);
      pcd->lTxt = GetCtlColor(hwnd, PP_ACTIVETEXTFGNDCOLOR,
                               PP_ACTIVETEXTFGNDCOLORINDEX,
                               SYSCLR_ACTIVETITLETEXT, 0x400080L);
   } /* endif */
}


//========================================================================\
// imposta nuovi parametri presentazione. Ritorna TRUE se si Š cambiato   |
// il font ed Š necessario un riclcolo delle dimensioni del rettangolo    |
// di testo                                                               |
//========================================================================/

BOOL GetNewPresParms(HWND hwnd, PCTL3D_DATA pcd, LONG lIdx) {
   switch (lIdx) {

#ifdef OS_VER_WARP_4 // warp 4 e superiori

      case PP_BORDERLIGHTCOLOR:
         pcd->lHilite = GetCtlColor(hwnd, PP_BORDERLIGHTCOLOR, 0,
                                     SYSCLR_BUTTONLIGHT, RGB_WHITE);
         break;
      case PP_BORDERDARKCOLOR:
         pcd->lShadow = GetCtlColor(hwnd, PP_BORDERDARKCOLOR, 0,
                                     SYSCLR_BUTTONDARK, RGB_DARKGRAY);

#else                // warp 3

      case PP_HILITEFOREGROUNDCOLOR:
      case PP_HILITEFOREGROUNDCOLORINDEX:
         pcd->lHilite = GetCtlColor(hwnd, PP_HILITEFOREGROUNDCOLOR,
                                     PP_HILITEFOREGROUNDCOLORINDEX,
                                     SYSCLR_BUTTONLIGHT, RGB_WHITE);
         break;
      case PP_BORDERCOLOR:
      case PP_BORDERCOLORINDEX:
         pcd->lShadow = GetCtlColor(hwnd, PP_BORDERCOLOR,
                                     PP_BORDERCOLORINDEX,
                                     SYSCLR_BUTTONDARK, RGB_DARKGRAY);
         break;

#endif

      case PP_BACKGROUNDCOLOR:
      case PP_BACKGROUNDCOLORINDEX:
         pcd->lBack = GetCtlColor(hwnd, PP_BACKGROUNDCOLOR,
                                   PP_BACKGROUNDCOLORINDEX,
                                   SYSCLR_BUTTONMIDDLE, RGB_GRAY);
         break;
      case PP_FOREGROUNDCOLOR:
      case PP_FOREGROUNDCOLORINDEX:
         if (pcd->ulType != CTL_BUTTON)
            pcd->lFore = GetCtlColor(hwnd, PP_FOREGROUNDCOLOR,
                                     PP_FOREGROUNDCOLORINDEX,
                                     SYSCLR_WINDOWSTATICTEXT, RGB_DARKBLUE);
         else
            pcd->lFore = GetCtlColor(hwnd, PP_FOREGROUNDCOLOR,
                                      PP_FOREGROUNDCOLORINDEX,
                                      SYSCLR_MENUTEXT, RGB_BLACK);
         break;
      case PP_ACTIVETEXTBGNDCOLOR:
      case PP_ACTIVETEXTBGNDCOLORINDEX:
         pcd->lTxtBack = GetCtlColor(hwnd, PP_ACTIVETEXTBGNDCOLOR,
                                       PP_ACTIVETEXTBGNDCOLORINDEX,
                                       SYSCLR_ACTIVETITLETEXTBGND, RGB_WHITE);
         break;
      case PP_ACTIVETEXTFGNDCOLOR:
      case PP_ACTIVETEXTFGNDCOLORINDEX:
         pcd->lTxt = GetCtlColor(hwnd, PP_ACTIVETEXTFGNDCOLOR,
                                  PP_ACTIVETEXTFGNDCOLORINDEX,
                                  SYSCLR_ACTIVETITLETEXT, 0x400080L);
         break;
      case PP_FONTNAMESIZE:
         return TRUE;
      default:
         break;
   } /* endswitch */
   return FALSE;
}


//========================================================================\
// imposta il testo del controllo                                         |
//========================================================================/

BOOL SetControlText(HWND hwnd, PCTL3D_DATA pcd, PWNDPARAMS pwprm) {
   if (pwprm && pwprm->fsStatus & WPM_TEXT) {
      PCTL3D_DATA pcd = (PCTL3D_DATA)WinQueryWindowPtr(hwnd, QWL_CTL);
      accfree(pcd->txt.psz);
      if ((pcd->txt.psz = accmalloc(1 + pwprm->cchText)) == NULL)
         return FALSE;
      accstrcpy(pcd->txt.psz, pwprm->pszText);
   } // end if
   return TRUE;
}


//========================================================================\
// restituisce il testo del controllo                                     |
//========================================================================/

MRESULT ProcessQueryPresParms(HWND hwnd, PWNDPARAMS pwprm, PCTL3D_DATA pcd) {
   // Set window parameter structure fields
   if (pwprm->fsStatus & WPM_CCHTEXT)
      pwprm->cchText = accstrlen(pcd->txt.psz);
   if (pwprm->fsStatus & WPM_TEXT)
      accstrcpy(pwprm->pszText, pcd->txt.psz);
   if (pwprm->fsStatus & WPM_CBPRESPARAMS)
      pwprm->cbPresParams = 0;
   if (pwprm->fsStatus & WPM_PRESPARAMS)
       pwprm->pPresParams = NULL;
   if (pwprm->fsStatus & WPM_CBCTLDATA)
       pwprm->cbCtlData = 0;
   if (pwprm->fsStatus & WPM_CTLDATA)
       pwprm->pCtlData = NULL;
   return (MRESULT)TRUE;
}


//========================================================================\
// get font average width and max descender height                        |
//========================================================================/

VOID UpdateFontData(PCTL3D_DATA pcd) {
   FONTMETRICS fm;

   GpiQueryFontMetrics(pcd->hps, sizeof(FONTMETRICS), &fm);
   pcd->txt.cy = fm.lMaxBaselineExt;
   pcd->txt.cyDesc = fm.lMaxDescender;
}


//========================================================================\
// forza l'aggiornamento del controllo                                    |
//========================================================================/

VOID RepaintControl(HWND hwnd, PCTL3D_DATA pcd, BOOL flag) {
   if (WinIsWindowShowing(hwnd)) {
      pcd->flPPaint = TRUE;
      if (flag) {
         ULONG ulFrame = (pcd->ulStyle & 0x7)? pcd->ulStyle & 0x7: 1;
         RECTL rcl = {ulFrame, ulFrame, pcd->rcl.xRight - ulFrame,
                      pcd->rcl.yTop - ulFrame};
         WinInvalidateRect(hwnd, &rcl, FALSE);
      } else {
         WinInvalidateRect(hwnd, NULL, FALSE);
      } // end if
   } // end if
}

VOID InvalidateControl(HWND hwnd, PCTL3D_DATA pcd) {
   pcd->flPPaint = TRUE;
   WinInvalidateRect(hwnd, NULL, FALSE);
}


//========================================================================\
// calcola i dati relativi ad ogni riga di testo, larghezza e altezza     |
// massima testo                                                          |
// argomenti:                                                             |
// PCTL3D_DATA pcd : puntatore alla struttura dati del controllo          |
// BOOL flBreak : TRUE -> stile word break                                |
// BOOL flAsz : TRUE -> stile autosize                                    |
//========================================================================/

VOID MeasureText(PCTL3D_DATA pcd, BOOL flBreak, BOOL flAsz) {

   PPOINTL pptl;                // array posizioni caratteri
   SPACE space = {NULL, 0, 0};
   LONG cxLine;         // larghezza disponibile per il testo
   LONG cchTot;         // lunghezza totale testo
   LONG cchPos;         // caratteri di cui si Š calcolata la posizione
   INT ich;             // indice carattere corrente nell'array di pointl
   PCHAR pch;           // puntatore carattere corrente
   LONG cxShift;        // correzione larghezza riga
   LONG cxMax;          // massima larghezza rilevata
   INT i;
   pch = pcd->txt.psz;      // posiz corrente nella stringa di caratteri
   pcd->mn.pch = NULL;
   pcd->mn.line = -1;
   pcd->mn.cch = 0;
   cxMax = 0;
   // calcola massimo numero linee che possono essere contenute nel rettangolo
   pcd->txt.cLines = (flBreak)? 1 + pcd->txt.size.cy / pcd->txt.cy: 1;
   if (pcd->pdl) accfree(pcd->pdl);
   if ((pcd->pdl = (PDRAWLINE)accmalloc(pcd->txt.cLines * sizeof(DRAWLINE)))
                    == NULL)
      return;
   if ((pptl = (PPOINTL)accmalloc(512 * sizeof(POINTL))) == NULL)
      return;
   cchPos = ((cchTot = accstrlen(pch) + 1)) > 511? 511: cchTot;
   GpiQueryCharStringPos(pcd->hps, 0L, cchPos--, pch, NULL, pptl);
   if (flBreak) {                  // testo su + righe
      for (ich = i = 0; *pch && i < pcd->txt.cLines; i++) {
         cxShift = ich > 0? -pptl[ich].x: space.cxPost;
         cxLine = pcd->txt.size.cx - cxShift;
         pcd->mn.cx = 0;
         space.pch = NULL;
         pcd->pdl[i].pchLine = pch;
         forever {
            if (ich < 0) {
               ++ich;
               ++pch;
               continue;
            } /* endif */
            if (ich > cchPos) {
               cchTot -= 511;
               cchPos = cchTot > 511? 511: cchTot;
               if (space.pch) space.cxPost +=pptl[ich].x;
               cxShift += pptl[ich].x;
               cxLine -= pptl[ich].x;
               ich = 0;
               GpiQueryCharStringPos(pcd->hps, 0L, cchPos--, pch, NULL, pptl);
            } /* endif */
            if (pptl[++ich].x - pcd->mn.cx > cxLine &&
                !(*pch == '\r' || *pch == '\n')) {
               if (space.pch) {
                  pcd->pdl[i].cch = space.pch - pcd->pdl[i].pchLine;
                  ich -= pch - space.pch;
                  pch = ++space.pch;
                  if (pcd->mn.pch > space.pch) pcd->mn.pch = NULL;
                  pcd->pdl[i].cxLine = space.cxPre;
               } else {
                  pcd->pdl[i].cch = pch - pcd->pdl[i].pchLine;
                  pcd->pdl[i].cxLine = pptl[--ich].x - pcd->mn.cx + cxShift;
               } /* endif */
               break;
            } /* endif */
            if (*pch == '~' && (pcd->ulStyle & DT_MNEMONIC) && !pcd->mn.pch) {
                  pcd->mn.pch = pch + 1;
                  pcd->mn.line = i;
                  pcd->mn.cch = pch - pcd->pdl[i].pchLine;
                  pcd->mn.cx = pptl[ich].x - pptl[ich - 1].x;
            } else if (*pch == ' ') {
               space.pch = pch;
               space.cxPre = pptl[ich - 1].x - pcd->mn.cx + cxShift;
               space.cxPost = -pptl[ich].x;
            } else if (*pch == '\n') {
               pcd->pdl[i].cch = pch - pcd->pdl[i].pchLine;
               pcd->pdl[i].cxLine = pptl[ich - 1].x - pcd->mn.cx + cxShift;
               ++pch;
               break;
            } else if (*pch == '\r') {
               pcd->pdl[i].cch = pch - pcd->pdl[i].pchLine;
               pcd->pdl[i].cxLine = pptl[ich - 1].x - pcd->mn.cx + cxShift;
               if (*++pch == '\n') {++pch; ++ich;}
               break;
            } else if (!*pch) {
               pcd->pdl[i].cch = pch - pcd->pdl[i].pchLine;
               pcd->pdl[i].cxLine = pptl[ich - 1].x - pcd->mn.cx + cxShift;
               break;
            } /* endif */
            ++pch;
         } // end forever
         if (pcd->pdl[i].cxLine > cxMax)
            cxMax = pcd->pdl[i].cxLine;
      } /* endfor */
      pcd->txt.cLines = i;      // numero linee effettivamente memorizzate
   } else {                        // una sola riga di testo
      ich = 0;
      pcd->pdl->pchLine = pch;
      forever {
         ++ich;
         if (*pch == '~' && (pcd->ulStyle & DT_MNEMONIC) && !pcd->mn.pch) {
            pcd->mn.pch = pch + 1;
            pcd->mn.line = 0;
            pcd->mn.cch = pch - pcd->pdl->pchLine;
            pcd->mn.cx = pptl[ich].x - pptl[ich - 1].x;
         } /* endif */
         if (!*pch || ich > 511) {
            pcd->pdl->cch = pch - pcd->pdl->pchLine;
            pcd->pdl->cxLine = pptl[ich - 1].x - pcd->mn.cx;
            break;
         } // end if
         ++pch;
      } // end do forever
      cxMax = pcd->pdl->cxLine;
      pcd->txt.cLines = 1;
   } /* endif */
   if (flAsz) { // se autosize
      if (pcd->txt.size.cy > pcd->txt.cLines * pcd->txt.cy)
         pcd->txt.size.cy = pcd->txt.cLines * pcd->txt.cy;
      pcd->txt.size.cx = cxMax;
   } /* endif */
   accfree(pptl);
}


//========================================================================\
// disegna il testo allineandolo secondo flStyle nel rettangolo puntato   |
// da prcl                                                                |
//========================================================================/

VOID DrawText(PCTL3D_DATA pcd, ULONG flStyle, PRECTL prcl) {

   POINTL ptl;
   INT i;

   // trova coordinata y prima stringa secondo lo stile
   if (flStyle & DT_VCENTER)
      ptl.y = prcl->yTop + pcd->txt.cyDesc -
               (pcd->txt.size.cy - pcd->txt.cLines * pcd->txt.cy) / 2;
   else if (flStyle & DT_BOTTOM)
      ptl.y = prcl->yBottom + pcd->txt.cLines * pcd->txt.cy +
              pcd->txt.cyDesc;
   else
      ptl.y = prcl->yTop + pcd->txt.cyDesc;
   for (i = 0; i < pcd->txt.cLines; i++) {
      ptl.y -= pcd->txt.cy;
      if (flStyle & DT_CENTER)    // allineamento orizzontale
         ptl.x = (prcl->xRight - prcl->xLeft - pcd->pdl[i].cxLine) / 2 +
                 prcl->xLeft;
      else if (flStyle & DT_RIGHT)
         ptl.x = prcl->xRight - prcl->xLeft - pcd->pdl[i].cxLine + prcl->xLeft;
      else
         ptl.x = prcl->xLeft;
      if (i == pcd->mn.line) {
         GpiMove(pcd->hps, &ptl);
         if (pcd->mn.cch) // se ci sono caratteri prima del menmonico
            GpiCharStringPos(pcd->hps, prcl, CHS_CLIP, pcd->mn.cch,
                             pcd->pdl[i].pchLine, NULL);
         GpiCharStringPos(pcd->hps, prcl, CHS_CLIP | CHS_UNDERSCORE, 1,
                          pcd->mn.pch, NULL);
         GpiCharStringPos(pcd->hps, prcl, CHS_CLIP,
                          pcd->pdl[i].cch - pcd->mn.cch - 2,
                          pcd->mn.pch + 1, NULL);
      } else {
         GpiCharStringPosAt(pcd->hps, &ptl, prcl, CHS_CLIP, pcd->pdl[i].cch,
                            pcd->pdl[i].pchLine, NULL);
      } /* endif */
   } /* endfor */
}


//========================================================================\
// da l'aspetto "disabled" al controllo                                   |
//========================================================================/

VOID PaintDisabled(PCTL3D_DATA pcd) {
   GpiMove(pcd->hps, (PPOINTL)&pcd->rcl);
   GpiSetPattern(pcd->hps, PATSYM_HALFTONE);
   GpiSetColor(pcd->hps, pcd->lBack);
   GpiBox(pcd->hps, DRO_FILL, ((PPOINTL)&pcd->rcl) + 1, 0L, 0L);
}


//========================================================================\
// forza il repaint dello sfondo della parent sotto il controllo e del    |
// controllo stesso                                                       |
//========================================================================/

VOID RepaintCtlBkGnd(PCTL3D_DATA pcd) {
   RECTL rcl;
   rcl.xLeft = pcd->ptOrig.x;
   rcl.yBottom = pcd->ptOrig.y;
   rcl.xRight = rcl.xLeft + pcd->rcl.xRight;
   rcl.yTop = rcl.yBottom + pcd->rcl.yTop;
   WinInvalidateRect(pcd->hParent, &rcl, TRUE);
}
