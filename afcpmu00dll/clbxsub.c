//========================================================================\
// clbxsub.c                                                              |
// - procedura creazione e subclassing coonenti controllo                 |
// - procedure subclassate combobox e listbox per cambiamento focus tra   |
//   listbox e combobox alla pressione di Alt + freccia su/gi—            |
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
// procedura subclassata checkbox                                           |
//==========================================================================/

static MRESULT EXPENTRY newchkproc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   PCLBOX pclb = (PCLBOX)stGetData(WinQueryWindow(hwnd, QW_PARENT));
   if (msg == WM_PAINT && !pclb->is.enbl) {
      pclb->pfbtn(hwnd, msg, mp1, mp2);
      paintDisabledCheck(hwnd, &pclb->rclcpt, pclb->lbkgnd);
      return(MRESULT)FALSE;
   } /* endif */
   return pclb->pfbtn(hwnd, msg, mp1, mp2);
}


//==========================================================================\
// crea i vari componenti del controllo                                     |
//==========================================================================/

BOOL clbChilds(PCLBOX pclb, PCREATESTRUCT pc) {
   ULONG fl = 0;
   clbSize(pclb);
   if (pclb->fl & WS_DISABLED) fl = WS_DISABLED;
   if (pclb->fl & WS_VISIBLE) fl |= WS_VISIBLE;
   // se stile CLBXS_CHECK crea checkbox
   if (pclb->fl & CLBXS_CHECK) {
      LONG pp[] = {12, PP_BACKGROUNDCOLOR, 4, pclb->lbkgnd};
      pclb->hchk = WinCreateWindow(pclb->hwnd, WC_BUTTON, pc->pszText,
                                   BS_AUTOCHECKBOX | fl,
                                   0, pclb->rclcpt.yBottom, 
                                   pclb->szl.cx, pclb->rclcpt.yTop -
                                   pclb->rclcpt.yBottom, pclb->hwnd,
                                   HWND_TOP, FID_CAPTION, NULL, (PVOID)pp);
      if (*pc->pszText) *pc->pszText = 0;
      fl |= WS_DISABLED;
      if (!pclb->hchk) return FALSE;
      pclb->pfbtn = WinSubclassWindow(pclb->hchk, newchkproc);
   } /* endif */
   // entryfield/dropdownbox
   if (pclb->fl & CLBXS_DROPDOWN) {       // dropdown box
      pclb->hentry = WinCreateWindow(pclb->hwnd, WC_COMBOBOX, NULL,
                                     WS_VISIBLE | fl | CBS_DROPDOWN,
                                     0, pclb->rclef.yBottom,
                                     pclb->rclef.xRight, pclb->rclef.yTop -
                                     pclb->rclef.yBottom, pclb->hwnd,
                                     HWND_TOP, FID_ENTRY, NULL, NULL);
      pclb->pfef = WinSubclassWindow(pclb->hentry, newcomboproc);
   } else {                               // entryfield
      pclb->hentry = WinCreateWindow(pclb->hwnd, WC_ENTRYFIELD, NULL,
                                     WS_VISIBLE | ES_MARGIN | ES_AUTOSCROLL |
                                     fl, 0,
                                     pclb->rclef.yBottom,
                                     pclb->rclef.xRight, pclb->rclef.yTop -
                                     pclb->rclef.yBottom, pclb->hwnd,
                                     HWND_TOP, FID_ENTRY, NULL, NULL);
   } /* endif */
   // bottone add
   pclb->hadd = WinCreateWindow(pclb->hwnd, WC_BUTTON, NULL,
                                WS_VISIBLE | WS_DISABLED | fl | BS_PUSHBUTTON |
                                BS_USERBUTTON | BS_NOCURSORSELECT,
                                pclb->ptladd.x, pclb->ptladd.y, 
                                pclb->csdbtn, pclb->csdbtn, pclb->hwnd,
                                HWND_TOP, FID_ADD, NULL, NULL);
   pclb->hrem = WinCreateWindow(pclb->hwnd, WC_BUTTON, NULL,
                                WS_VISIBLE | WS_DISABLED | fl | BS_PUSHBUTTON |
                                BS_USERBUTTON | BS_NOCURSORSELECT,
                                pclb->ptlrem.x, pclb->ptlrem.y, 
                                pclb->csdbtn, pclb->csdbtn, pclb->hwnd,
                                HWND_TOP, FID_REMOVE, NULL, NULL);
   pclb->hfind = WinCreateWindow(pclb->hwnd, WC_BUTTON, NULL,
                                 WS_VISIBLE | fl | BS_PUSHBUTTON |
                                 BS_USERBUTTON | BS_NOCURSORSELECT,
                                 pclb->ptlfnd.x, pclb->ptlfnd.y, 
                                 pclb->csdbtn, pclb->csdbtn, pclb->hwnd,
                                 HWND_TOP, FID_FIND, NULL, NULL);
   // listbox
   if (pclb->fl & CLBXS_HSCROLL) fl |= LS_HORZSCROLL;
   if (pclb->fl & CLBXS_MULTISEL) fl |= LS_MULTIPLESEL;
   if (pclb->fl & CLBXS_EXTENDSEL) fl |= LS_EXTENDEDSEL;
   if (pclb->fl & CLBXS_OWNERDRAW) fl |= LS_OWNERDRAW;
   pclb->hlbx = WinCreateWindow(pclb->hwnd, WC_LISTBOX, NULL,
                                WS_VISIBLE | LS_NOADJUSTPOS | fl,
                                0, pclb->rcllbx.yBottom, 
                                pclb->szl.cx, pclb->rcllbx.yTop -
                                pclb->rcllbx.yBottom, pclb->hwnd,
                                HWND_TOP, FID_LIST, NULL, NULL);
   if (!pclb->pfbtn) // se non si Š ottenuto con il subclassing
      pclb->pfbtn = (PFNWP)WinQueryWindowPtr(pclb->hadd, QWP_PFNWP);                             
   pclb->pflbx = WinSubclassWindow(pclb->hlbx, clblboxproc);
   if (!pclb->pfef)
      pclb->pfef = (PFNWP)WinQueryWindowPtr(pclb->hentry, QWP_PFNWP);
   // setta massimo numero caratteri ammessi nell'entryfield
   if (pclb->cchef)
      pclb->pfef(pclb->hentry, EM_SETTEXTLIMIT, (MPARAM)pclb->cchef, MPVOID);
   else
      pclb->cchef = 32;
   return (BOOL)(pclb->hentry && pclb->hadd && pclb->hrem &&
                 pclb->hfind && pclb->hlbx && pclb->pfbtn &&
                 pclb->pflbx && pclb->pfef);
}


//==========================================================================\
// procedura subclassata listbox                                            |
//==========================================================================/

MRESULT EXPENTRY clblboxproc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   PCLBOX pclb = (PCLBOX)stGetData(WinQueryWindow(hwnd, QW_PARENT));
   if (msg == WM_CHAR &&
       (CHARMSG(&msg)->fs & KC_ALT) &&
       !(CHARMSG(&msg)->fs & KC_KEYUP) &&
       ((CHARMSG(&msg)->vkey == VK_UP) ||
        (CHARMSG(&msg)->vkey == VK_DOWN))) {
      WinSetFocus(HWND_DESKTOP, pclb->hentry);
      return (MRESULT)TRUE;
   } /* endif */
   return pclb->pflbx(hwnd, msg, mp1, mp2); // default listsbox controls proc
}


//==========================================================================\
// procedura subclassata combobox                                           |
//==========================================================================/

MRESULT EXPENTRY newcomboproc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   PCLBOX pclb = (PCLBOX)stGetData(WinQueryWindow(hwnd, QW_PARENT));
   if (msg == WM_CHAR &&
       (CHARMSG(&msg)->fs & KC_ALT) &&
       !(CHARMSG(&msg)->fs & KC_KEYUP) &&
       ((CHARMSG(&msg)->vkey == VK_UP))) {
      WinSetFocus(HWND_DESKTOP, pclb->hlbx);
      return (MRESULT)TRUE;
   } /* endif */
   return pclb->pfef(hwnd, msg, mp1, mp2); // default listsbox controls proc
}
