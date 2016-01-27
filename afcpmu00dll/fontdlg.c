//==========================================================================\
// fontdlg.c: mostra il dialogo di selezione font                           |
// funzioni:                                                                |
// BOOL SetFont(PFONTDLGDATA pfdd);                                         |
// BOOL SetFontTerminate(PFONTDLG pfd, BOOL rc);                            |
// MRESULT EXPENTRY NewFontDlgProc(HWND hwnd, ULONG msg,                    |
//                                 MPARAM mp1, MPARAM mp2);                 |
//==========================================================================/

#pragma strings(readonly)

#define INCL_DOSPROCESS
#define  INCL_WIN
#define  INCL_STDDLG
#define  INCL_GPI
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <winutil.h>
#include "common.h"
#include "api.h"


MRESULT EXPENTRY NewFontDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);


//==========================================================================\
// alloca memoria per le strutture necessarie per il dialogo di selezione   |
// dei font, le inizializza, mostra il dialogo e copia in pszFont il nuovo  |
// font selezionato                                                         |
// parametri:                                                               |
// HWND hwnd: (in)handle della finestra di cui si vuole cambiare il font    |
// PSZ pszTitle: titolo della finestra di dialogo                           |
// PSZ pszSample: testo da mostrare come sample del font selezionato        |
// PSZ pszFont: (output)dimensione-nome-attributi nuovo font selezionato    |
// ULONG cbFont: dimensioni buffer pszFont                                  |
// PULONG pfl: in/out.                                                      |
//             in: se TRUE nasconde le checkbox display e printer           |
//             out: se TRUE indica che in pszFont c'Š un nuovo font         |
// valore restituito:                                                       |
// BOOL: TRUE = successo, FALSE = errore                                    |
//==========================================================================/

BOOL APIENTRY WfontDlg(HWND hwnd, PSZ pszTitle, PSZ pszSample, PSZ pszFont,
                       ULONG cbFont, PULONG pfl) {
   PFONTDLG pfd = NULL;
   PFONTMETRICS pfm = NULL;
   PSZ psz;
   BOOL rc = FALSE;

   if (!(pfd = (PFONTDLG)malloc(sizeof(FONTDLG)))) goto end;
   memset((PVOID)pfd, 0, sizeof(FONTDLG));
   if (!(pfm = (PFONTMETRICS)malloc(sizeof (FONTMETRICS)))) goto end;
   memset((PVOID)pfm, 0, sizeof(FONTMETRICS));
   pfd->cbSize = sizeof(FONTDLG);
   pfd->pszFamilyname = psz = pszFont;
   pfd->usFamilyBufLen = cbFont;
   pfd->pszTitle = pszTitle;
   pfd->pszPreview = pszSample;
   pfd->clrFore = CLR_NEUTRAL;
   pfd->clrBack = SYSCLR_WINDOW;
   pfd->hpsScreen = WinGetPS(hwnd);
   // subclassa la procedura del dialogo solo se *pfl = TRUE
   pfd->pfnDlgProc = *pfl? NewFontDlgProc: NULL;

   if (!GpiQueryFontMetrics(pfd->hpsScreen, sizeof(FONTMETRICS), pfm)) goto end;

   // inizializza la struttura FATTRS di FONTDLG
   pfd->fAttrs.usRecordLength = sizeof(FATTRS);
   // inizializza gli attributi del font
   while (*psz != '.') pfd->ulUser = pfd->ulUser * 10 + *psz++ - '0';
   pfd->fxPointSize = pfd->ulUser << 16;
   while (*psz) {
      if (!strncmp(psz, ".Italic", 7)) {
         pfd->fAttrs.fsSelection |= FATTR_SEL_ITALIC; psz += 7;
      } else if (!strncmp(psz, ".Bold", 5)) {
         pfd->fAttrs.fsSelection |= FATTR_SEL_BOLD; psz += 5;
      } else if (!strncmp(psz, ".Underscore", 11)) {
         pfd->fAttrs.fsSelection |= FATTR_SEL_UNDERSCORE; psz += 11;
      } else if (!strncmp(psz, ".Outline", 8)) {
         pfd->fAttrs.fsSelection |= FATTR_SEL_OUTLINE; psz += 8;
      } else if (!strncmp(psz, ".Strikeout", 10)) {
         pfd->fAttrs.fsSelection |= FATTR_SEL_STRIKEOUT; psz += 10;
      } else {
         psz++;
      } /* endif */
   } /* endwhile */

   pfd->fAttrs.lMatch = pfm->lMatch;
   pfd->fAttrs.idRegistry = pfm->idRegistry;
   pfd->fAttrs.usCodePage = pfm->usCodePage;

   if (!(pfm->fsDefn & FM_DEFN_OUTLINE)) {
      pfd->fAttrs.lMaxBaselineExt = pfm->lMaxBaselineExt;
      pfd->fAttrs.lAveCharWidth = pfm->lAveCharWidth;
   } // end if
   // tipo font
   pfd->fAttrs.fsType |= pfm->fsType & FM_TYPE_KERNING? FATTR_TYPE_KERNING: 0;
   pfd->fAttrs.fsType |= pfm->fsType & FM_TYPE_DBCS? FATTR_TYPE_DBCS: 0;
   pfd->fAttrs.fsType |= pfm->fsType & FM_TYPE_MBCS? FATTR_TYPE_MBCS: 0;
   strcpy(pfd->fAttrs.szFacename, pfm->szFacename);

   // the fontdlg style flags
   pfd->fl = FNTS_CENTER | FNTS_INITFROMFATTRS | FNTS_RESETBUTTON;
   pfd->usWeight = pfm->usWeightClass;
   pfd->usWidth = pfm->usWidthClass;

   if (!WinFontDlg(HWND_DESKTOP, hwnd, pfd)) goto end;
   if (pfd->lReturn == DID_OK) {
      *pfl = TRUE;
      sprintf(pszFont, "%d.%s", FIXEDINT(pfd->fxPointSize),
              pfd->fAttrs.szFacename);
      if (pfd->fAttrs.fsSelection & FATTR_SEL_ITALIC)
         strcat(pszFont, ".Italic");
      if (pfd->fAttrs.fsSelection & FATTR_SEL_UNDERSCORE)
         strcat(pszFont, ".Underscore");
      if (pfd->fAttrs.fsSelection & FATTR_SEL_STRIKEOUT)
         strcat(pszFont, ".Strikeout");
      if (pfd->fAttrs.fsSelection&FATTR_SEL_BOLD)
         strcat(pszFont, ".Bold");
      if (pfd->fAttrs.fsSelection&FATTR_SEL_OUTLINE)
         strcat(pszFont, ".Outline");
   } else {
      *pfl = FALSE;
   } // end if
   rc = TRUE;

   end:
   if (pfd) {
      if (pfd->hpsScreen) WinReleasePS(pfd->hpsScreen);
      free(pfd);
      free(pfm);
   } // end if
   return rc;
}


//==========================================================================\
// procedura di subclassing del dialogo di selezione font, si limita a      |
// nascondere le checkbox di selezione font stampante/schermo               |
// parametri:                                                               |
// HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2                             |
// valore restituito:                                                       |
// MRESULT                                                                  |
//==========================================================================/

MRESULT EXPENTRY NewFontDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   if (msg == WM_INITDLG) {
      DlgShowCtl(hwnd, DID_DISPLAY_FILTER, FALSE);
      DlgShowCtl(hwnd, DID_PRINTER_FILTER, FALSE);
   } // end if
   return WinDefFontDlgProc(hwnd, msg, mp1, mp2);
}
