//========================================================================\
// clbxaux.c                                                              |
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
#include "clbox.h"


//==========================================================================\
// aggiorna i colori del controllo                                          |
//==========================================================================/

VOID clbUpdColors(PCLBOX pclb, HPS hps) {
   pclb->lfgnd = WgetCtlClr(pclb->hwnd, hps, PP_FOREGROUNDCOLOR,
                         PP_FOREGROUNDCOLORINDEX, SYSCLR_WINDOWSTATICTEXT);
   pclb->lbkgnd = WgetCtlClr(pclb->hwnd, hps, PP_BACKGROUNDCOLOR,
                         PP_BACKGROUNDCOLORINDEX, SYSCLR_DIALOGBACKGROUND);
}


//==========================================================================\
// setta il massimo numero di caratteri consentito per il componente        |
// entryfield ricavandolo dal controldata                                   |
//==========================================================================/

BOOL clbSetCtlData(PCLBOX pclb, PCLBOXCDATA pclcd) {
   if (!pclcd) return TRUE;
   if (pclcd->cb != sizeof(CLBOXCDATA)) return FALSE;
   pclb->cchef = pclcd->cch;
   return TRUE;
}


//==========================================================================\
// memorizza il testo del controllo separandolo le descrizioni delle varie  |
// colonne e memorizzandone gli indirizzi                                   |
// parametri: ------------------------------------------------------------- |
// PCLBOX pclb: struttura dati controllo                                    |
// PSZ psz: nuovo testo controllo                                           |
// LONG cb: lunghezza nuovo testo                                           |
// valore restituito: ----------------------------------------------------- |
// ULONG : lunghezza nuovo testo controllo                                  |
//==========================================================================/

ULONG clbSetText(PCLBOX pclb, PSZ psz, LONG cb) {
   if (!pclb) return 0;
   free(pclb->psz);
   if (pclb->fl & CLBXS_NOCAPT) return 0;
   if (pclb->fl & CLBXS_CHECK) {
      if (pclb->hchk) WinSetWindowText(pclb->hchk, psz);
      return 0;
   } /* endif */
   if (!psz ||
       !(pclb->cbtxt = strlen(psz)) ||
       !(pclb->psz = (PSZ)malloc(pclb->cbtxt + 1))) {
      pclb->cbtxt = 0; 
      pclb->psz = NULL;
      return 0;
   } // end if
   memcpy(pclb->psz, psz, pclb->cbtxt + 1);
   return pclb->cbtxt;
}


//==========================================================================\
// calcola dimensioni componenti controllo                                  |
//==========================================================================/

VOID clbSize(PCLBOX pclb) {
   pclb->csdbtn = pclb->cyFont + 6;              // lato bottoni
   if (pclb->fl & CLBXS_DROPDOWN) {              // se stile dropdown
      pclb->rclef.yBottom = -100;
      pclb->rclef.yTop = pclb->cyFont + 6;
      pclb->rclef.xRight = pclb->szl.cx - 1 - 3 * pclb->csdbtn;
      pclb->rcllbx.yBottom = pclb->rclef.yTop + 1;       // listbox
      pclb->ptladd.x = pclb->rclef.xRight + 1;      // bottone add
   } else {                                      // stile entryfield
      pclb->rclef.yTop = pclb->cyFont + 3;
      pclb->rclef.yBottom = 3;
      pclb->rclef.xLeft = 3;
      pclb->rclef.xRight = pclb->szl.cx - 4 - 3 * pclb->csdbtn;
      pclb->rcllbx.yBottom = pclb->rclef.yTop + 4;       // listbox
      pclb->ptladd.x = pclb->rclef.xRight + 4;      // bottone add
   } /* endif */
   pclb->ptlrem.x = pclb->ptladd.x + pclb->csdbtn; // bottone remove
   pclb->ptlfnd.x = pclb->ptlrem.x + pclb->csdbtn; // bottone find
   if (pclb->fl & CLBXS_NOCAPT) {                // caption assente
      pclb->rcllbx.yTop = pclb->szl.cy;
   } else {
      POINTL ptl = {0, 10};
      WinMapDlgPoints(pclb->hwnd, &ptl, 1, TRUE);
      pclb->rclcpt.yTop = pclb->szl.cy;
      pclb->rclcpt.yBottom = pclb->rclcpt.yTop - ptl.y;
      pclb->rclcpt.xRight = pclb->szl.cx;
      pclb->rcllbx.yTop = pclb->rclcpt.yBottom - 1;
   } /* endif */
   pclb->rcllbx.xRight = pclb->szl.cx;            // listbox
}


//==========================================================================\
// in caso di ridimensionamento del controllo o di cambiamento del font     |
// usato riposiziona opportunamente in componenti del controllo             |
//==========================================================================/

BOOL clbMove(PCLBOX pclb) {
   ULONG cctls = (pclb->fl & CLBXS_CHECK)? 6: 5;
   PSWP pswp = (PSWP)malloc(sizeof(SWP) * cctls);
   INT i;
   if (!pswp) return FALSE;
   for (i = 0; i < cctls; ++i) pswp[i].fl = SWP_SIZE | SWP_MOVE;
   if (cctls == 6)
      mkswp(pswp + 5, pclb->hchk, 0, pclb->rclcpt.yBottom, pclb->szl.cx,
            pclb->rclcpt.yTop - pclb->rclcpt.yBottom);
   // entry
   rcl2swp(pswp, pclb->hentry, pclb->rclef.xLeft, pclb->rclef.yBottom,
           pclb->rclef.xRight, pclb->rclef.yTop);
   // bottoni
   mkswp(pswp + 1, pclb->hadd, pclb->ptladd.x, pclb->ptladd.y, 
         pclb->csdbtn, pclb->csdbtn);
   mkswp(pswp + 2, pclb->hrem, pclb->ptlrem.x, pclb->ptlrem.y, 
         pclb->csdbtn, pclb->csdbtn);
   mkswp(pswp + 3, pclb->hfind, pclb->ptlfnd.x, pclb->ptlfnd.y, 
         pclb->csdbtn, pclb->csdbtn);
   // listbox
   rcl2swp(pswp + 4, pclb->hlbx, pclb->rcllbx.xLeft, pclb->rcllbx.yBottom,
           pclb->rcllbx.xRight, pclb->rcllbx.yTop);
   i = (INT)WinSetMultWindowPos(pclb->hab, pswp, cctls);
   free(pswp);
   return (BOOL)i;
}


//==========================================================================\
// processa i messaggi di notifica di FID_LIST                              |
//==========================================================================/

VOID clbControlLboxMsg(PCLBOX pclb, ULONG nmsg) {
   SHORT item;
   switch (nmsg) {
      // doppio click su item listbox
      case LN_ENTER:
         nmsg = CLBXN_LNENTER;
         if (pclb->fl & CLBXS_ENTRREM) {          // rimuove item
            item = (SHORT)pclb->pflbx(pclb->hlbx, LM_QUERYSELECTION,
                                             (MPARAM)LIT_FIRST, MPVOID);
            pclb->pflbx(pclb->hlbx, LM_DELETEITEM, (MPARAM)item, MPVOID);
            pclb->is.remon = 0;
            WinEnableWindow(pclb->hrem, FALSE);
         } else if (pclb->fl & CLBXS_ENTREDIT) {  // edita item
            SHORT cb;
            PSZ psz = NULL;
            item = (SHORT)pclb->pflbx(pclb->hlbx, LM_QUERYSELECTION,
                                             (MPARAM)LIT_FIRST, MPVOID);
            cb = (SHORT)pclb->pflbx(pclb->hlbx, LM_QUERYITEMTEXTLENGTH,
                                    (MPARAM)item, MPVOID);
            if (cb != LIT_ERROR &&
                NULL != (psz = (PSZ)malloc(++cb)) &&
                0 != (cb = (SHORT)pclb->pflbx(pclb->hlbx, LM_QUERYITEMTEXT,
                               MPFROM2SHORT(item, cb), (MPARAM)psz)))
               WinSetWindowText(pclb->hentry, psz);
            free(psz);
         } /* endif */
         break;
      // selezione item listbox
      case LN_SELECT:
         nmsg = CLBXN_LNSELECT;
         pclb->is.remon = (LIT_NONE == (item =
                         (SHORT)pclb->pflbx(pclb->hlbx, LM_QUERYSELECTION,
                                     (MPARAM)LIT_FIRST, MPVOID)))? 0: 1;
         WinEnableWindow(pclb->hrem, pclb->is.remon);
         break;
      // case LN_SETFOCUS: nmsg = CLBXN_LNSETFCS; break;
      // case LN_KILLFOCUS: nmsg = CLBXN_LNLOSEFCS; break;
      // case LN_SCROLL: nmsg = CLBXN_LNSCROLL; break;
      default: nmsg = 0;
   } /* endswitch */
   if (nmsg)
      WinSendMsg(WinQueryWindow(pclb->hwnd, QW_OWNER), WM_CONTROL,
                 MPFROM2SHORT(pclb->id, nmsg), (MPARAM)pclb->hwnd);
}


//==========================================================================\
// processa i messaggi di notifica di FID_ENTRY in modo diverso se il       |
// controllo Š un entryfield o una dropdownbox                              |
//==========================================================================/

VOID clbControlEntryMsg(PCLBOX pclb, ULONG nmsg) {
   MPARAM mp = (MPARAM)pclb->hwnd;
   if (pclb->fl & CLBXS_DROPDOWN) {     // dropdown box
      switch (nmsg) {
         case CBN_EFCHANGE: nmsg = clbEnableAdd(pclb); break;
         // case CBN_EFSCROLL: nmsg = CLBXN_ENEFSCROLL; break;
         case CBN_MEMERROR: nmsg = CLBXN_ENMEMERR; break;
         // case CBN_LBSELECT: nmsg = CLBXN_ENSELECT; break;
         // case CBN_LBSCROLL: nmsg = CLBXN_ENSCROLL; break;
         // case CBN_SHOWLIST: nmsg = CLBXN_ENSHOW; break;
         case CBN_ENTER:
            nmsg = CLBXN_ENENTER;
            mp = (MPARAM)WinSendMsg(pclb->hentry, LM_QUERYSELECTION,
                                    (MPARAM)LIT_CURSOR, MPVOID);
            break;
         default: nmsg = 0;
      } /* endswitch */
   } else {                             // entryfield
      switch (nmsg) {
         case EN_CHANGE: nmsg = clbEnableAdd(pclb); break;
         // case EN_SETFOCUS: nmsg = CLBXN_ENSETFCS; break;
         // case EN_KILLFOCUS: nmsg = CLBXN_ENLOSEFCS; break;
         // case EN_SCROLL: nmsg = CLBXN_ENEFSCROLL; break;
         case EN_MEMERROR: nmsg = CLBXN_ENMEMERR; break;
         // case EN_OVERFLOW: nmsg = CLBXN_ENOVERFLW; break;
         // case EN_INSERTMODETOGGLE: nmsg = CLBXN_ENINSTGL; break;
         default: nmsg = 0;
      } /* endswitch */
   } /* endif */
   if (nmsg)
      WinSendMsg(WinQueryWindow(pclb->hwnd, QW_OWNER), WM_CONTROL,
                 MPFROM2SHORT(pclb->id, nmsg), mp);
}


//==========================================================================\
// abilita/disabilita bottone "add" in base alla presenza di testo          |
// nell'entryfield                                                          |
//==========================================================================/

ULONG clbEnableAdd(PCLBOX pclb) {
   BOOL notempty = WinQueryWindowTextLength(pclb->hentry)? 1: 0;
   if (notempty != pclb->is.addon) {
      pclb->is.addon = notempty;
      WinEnableWindow(pclb->hadd, pclb->is.addon);
   } // end if
   return CLBXN_ENCHANGE;
}
