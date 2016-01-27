//========================================================================\
// moreaux.c                                                              |
// funzioni ausiliarie controllo compound listbox                         |
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
#include <ctype.h>
#include "common.h"
#include "api.h"
#include "more.h"


//==========================================================================\
// procedura subclassata checkbox                                           |
//==========================================================================/

static MRESULT EXPENTRY newchkproc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   PMOREBOX pmore = (PMOREBOX)stGetData(WinQueryWindow(hwnd, QW_PARENT));
   if (msg == WM_PAINT && !pmore->is.enbl) {
      pmore->pfcpt(hwnd, msg, mp1, mp2);
      paintDisabledCheck(hwnd, &pmore->rclcpt, pmore->lbkgnd);
      return(MRESULT)FALSE;
   } else if (msg == WM_SETWINDOWPARAMS &&
              (((PWNDPARAMS)mp1)->fsStatus & WPM_TEXT) &&
              !pmore->is.enbl) {
      MRESULT mr = pmore->pfcpt(hwnd, msg, mp1, mp2);
      paintDisabledCheck(pmore->hcpt, &pmore->rclcpt, pmore->lbkgnd);
      return mr;
   } /* endif */
   return pmore->pfcpt(hwnd, msg, mp1, mp2);
}


//==========================================================================\
// crea i vari componenti del controllo                                     |
//==========================================================================/

BOOL moreChilds(PMOREBOX pmore, PCREATESTRUCT pc) {
   ULONG fl = 0;
   moreSize(pmore);
   if (pmore->fl & WS_DISABLED) fl = WS_DISABLED;
   if (pmore->fl & WS_VISIBLE) fl |= WS_VISIBLE;
   // se stile MORE_CHECK crea checkbox
   if (pmore->fl & MORE_CHECK) {
      LONG pp[] = {12, PP_BACKGROUNDCOLOR, 4, pmore->lbkgnd};
      pmore->hcpt = WinCreateWindow(pmore->hwnd, WC_BUTTON, pc->pszText,
                                    BS_AUTOCHECKBOX | fl,
                                    0, pmore->rclcpt.yBottom, 
                                    pmore->rclcpt.xRight - pmore->rclcpt.xLeft,
                                    pmore->rclcpt.yTop, pmore->hwnd,
                                    HWND_TOP, FID_CAPTION, NULL, (PVOID)pp);
      if (*pc->pszText) *pc->pszText = 0;
      if (!pmore->hcpt) return FALSE;
      pmore->pfcpt = WinSubclassWindow(pmore->hcpt, newchkproc);
   // invece se stile MORE_ENTRY crea entryfield
   } else if (pmore->fl & MORE_ENTRY) {
      pmore->hcpt = WinCreateWindow(pmore->hwnd, WC_ENTRYFIELD, pc->pszText,
                                    WS_VISIBLE | ES_MARGIN | ES_AUTOSCROLL |
                                    fl, pmore->rclcpt.xLeft,
                                    pmore->rclcpt.yBottom,
                                    pmore->rclcpt.xRight - pmore->rclcpt.xLeft,
                                    pmore->rclcpt.yTop - pmore->rclcpt.yBottom,
                                    pmore->hwnd,
                                    HWND_TOP, FID_CAPTION, NULL, NULL);
      if (*pc->pszText) *pc->pszText = 0;
      if (!pmore->hcpt) return FALSE;
   } /* endif */
   if (!pmore->is.chkon) fl |= WS_DISABLED;
   // se stile MORE_SWAP aggiunge bottone swap
   if (pmore->fl & MORE_SWAP) {       // dropdown box
      pmore->hswap = WinCreateWindow(pmore->hwnd, WC_BUTTON, NULL,
                                     WS_VISIBLE | fl | BS_PUSHBUTTON |
                                     BS_USERBUTTON | BS_NOCURSORSELECT,
                                     pmore->ptlswap.x, pmore->ptlswap.y, 
                                     pmore->csdbtn, pmore->csdbtn, pmore->hwnd,
                                     HWND_TOP, FID_SWAP, NULL, NULL);
      if (!pmore->hswap) return FALSE;
   } /* endif */
   // bottone find
   pmore->hfind = WinCreateWindow(pmore->hwnd, WC_BUTTON, NULL,
                                 WS_VISIBLE | fl | BS_PUSHBUTTON |
                                 BS_USERBUTTON | BS_NOCURSORSELECT,
                                 pmore->ptlfnd.x, pmore->ptlfnd.y, 
                                 pmore->csdbtn, pmore->csdbtn, pmore->hwnd,
                                 HWND_TOP, FID_FIND, NULL, NULL);
   // indirizzi procedure default
   pmore->pfbtn = (PFNWP)WinQueryWindowPtr(pmore->hfind, QWP_PFNWP);                             
   if (pmore->fl & MORE_ENTRY)
      if (!(pmore->pfcpt = (PFNWP)WinQueryWindowPtr(pmore->hcpt, QWP_PFNWP)))
         return FALSE;
   // setta massimo numero caratteri ammessi nell'entryfield
   if ((pmore->fl & MORE_ENTRY) && pmore->cchef)
      pmore->pfcpt(pmore->hcpt, EM_SETTEXTLIMIT, (MPARAM)pmore->cchef, MPVOID);
   else
      pmore->cchef = 32;
   return (BOOL)(pmore->hfind && pmore->pfbtn);
}


//==========================================================================\
// aggiorna i colori del controllo                                          |
//==========================================================================/

VOID moreUpdColors(PMOREBOX pmore, HPS hps) {
   pmore->lfgnd = WgetCtlClr(pmore->hwnd, hps, PP_FOREGROUNDCOLOR,
                         PP_FOREGROUNDCOLORINDEX, SYSCLR_WINDOWSTATICTEXT);
   pmore->lbkgnd = WgetCtlClr(pmore->hwnd, hps, PP_BACKGROUNDCOLOR,
                         PP_BACKGROUNDCOLORINDEX, SYSCLR_DIALOGBACKGROUND);
}


//==========================================================================\
// setta il massimo numero di caratteri consentito per il componente        |
// entryfield ricavandolo dal controldata                                   |
//==========================================================================/

BOOL moreSetCtlData(PMOREBOX pmore, PMORECDATA pmrcd) {
   if (!pmrcd) return TRUE;
   if (pmrcd->cb != sizeof(MORECDATA)) return FALSE;
   pmore->cchef = pmrcd->cch;
   return TRUE;
}


//==========================================================================\
// memorizza il testo del controllo separandolo le descrizioni delle varie  |
// colonne e memorizzandone gli indirizzi                                   |
// parametri: ------------------------------------------------------------- |
// PMOREBOX pmore: struttura dati controllo                                 |
// PSZ psz: nuovo testo controllo                                           |
// LONG cb: lunghezza nuovo testo                                           |
// valore restituito: ----------------------------------------------------- |
// ULONG : lunghezza nuovo testo controllo                                  |
//==========================================================================/

ULONG moreSetText(PMOREBOX pmore, PSZ psz, LONG cb) {
   if (!pmore) return 0;
   if (pmore->psz) free(pmore->psz);
   if (pmore->fl & (MORE_CHECK | MORE_ENTRY)) {
      if (pmore->hcpt) WinSetWindowText(pmore->hcpt, psz);
      return 0;
   } /* endif */
   if (!psz ||
       !(pmore->cbtxt = strlen(psz)) ||
       !(pmore->psz = (PSZ)malloc(pmore->cbtxt + 1))) {
      pmore->cbtxt = 0; 
      pmore->psz = NULL;
      return 0;
   } // end if
   memcpy(pmore->psz, psz, pmore->cbtxt + 1);
   return pmore->cbtxt;
}


//==========================================================================\
// calcola dimensioni componenti controllo                                  |
//==========================================================================/

VOID moreSize(PMOREBOX pmore) {
   pmore->csdbtn = pmore->szl.cy;                // lato bottoni
   if (pmore->csdbtn % 2) --pmore->csdbtn;
   if (pmore->fl & MORE_ENTRY) {                     // stile entryfield
      pmore->rclcpt.yTop = pmore->szl.cy - 3;
      pmore->rclcpt.yBottom = 3;
      pmore->rclcpt.xLeft = 3;
      pmore->rclcpt.xRight = pmore->szl.cx - 4 - pmore->csdbtn;
      pmore->ptlfnd.x = pmore->rclcpt.xRight + 4;    // bottone add
   } else { // se stile checkbox o default
      pmore->rclcpt.yTop = pmore->szl.cy;
      pmore->rclcpt.xRight = pmore->szl.cx - 1 - pmore->csdbtn;
      pmore->ptlfnd.x = pmore->rclcpt.xRight + 1;     // bottone add
   } /* endif */
   if (pmore->fl & MORE_SWAP) {                      // bottone swap
      pmore->rclcpt.xRight -= pmore->csdbtn;
      pmore->ptlswap.x = pmore->ptlfnd.x - pmore->csdbtn;
   } /* endif */
}


//==========================================================================\
// in caso di ridimensionamento del controllo o di cambiamento del font     |
// usato riposiziona opportunamente in componenti del controllo             |
//==========================================================================/

BOOL moreMove(PMOREBOX pmore) {
   ULONG cctls = 1;
   PSWP pswp = (PSWP)malloc(sizeof(SWP) * 3);
   INT i;
   if (!pswp) return FALSE;
   for (i = 0; i < 3; ++i) pswp[i].fl = SWP_SIZE | SWP_MOVE;
   // bottone find
   mkswp(pswp, pmore->hfind, pmore->ptlfnd.x, pmore->ptlfnd.y, 
         pmore->csdbtn, pmore->csdbtn);
   if (pmore->hswap) {  // bottone swap
      mkswp(pswp + 1, pmore->hswap, pmore->ptlswap.x, pmore->ptlswap.y, 
            pmore->csdbtn, pmore->csdbtn);
      ++cctls;
   } /* endif */
   if (pmore->hcpt) {   // checkbox o entryfield
      rcl2swp(pswp + 2, pmore->hcpt, pmore->rclcpt.xLeft,
              pmore->rclcpt.yBottom, pmore->rclcpt.xRight, pmore->rclcpt.yTop);
      ++cctls;
   } /* endif */
   i = (INT)WinSetMultWindowPos(pmore->hab, pswp, cctls);
   free(pswp);
   return (BOOL)i;
}
