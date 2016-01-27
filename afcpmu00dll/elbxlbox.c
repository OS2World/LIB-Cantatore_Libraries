//=========================================================================\
// elbxlbox.c : procedura subclassata componente listbox del controllo     |
//              enhanced listbox                                           |
//                                                                         |
// FUNZIONI: ------------------------------------------------------------  |
// BOOL createChild(PELBOX pelb, PELBOXCDATA pelbcd)                       |
// MRESULT EXPENTRY newlboxproc(HWND hwnd, ULONG msg,                      |
//                              MPARAM mp1, MPARAM mp2)                    |
// VOID lboxMouBtn1dwn(PELBOX pelb, PMSEMSG pmmsg);                        |
// BOOL lboxChar(PELBOX pelb, PCHRMSG pchmsg);                             |
// BOOL lboxMouseMove(PELBOX pelb, PMSEMSG pmmsg);                         |
// BOOL lboxEndDrag(pelb, PMSEMSG pmmsg);                                  |
// BOOL lboxTimer(PELBOX pelb, ULONG id);                                  |
//                                                                         |
// MESSAGGI: ------------------------------------------------------------  |
// WM_BUTTON1DOWN:                                                         |
// WM_CHAR:                                                                |
// WM_BEGINDRAG:                                                           |
// WM_MOUSEMOVE:                                                           |
// WM_ENDDRAG:                                                             |
// WM_TIMER:                                                               |
//=========================================================================/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "api.h"
#include "elbox.h"


VOID lboxMouBtn1dwn(PELBOX pelb, PMSEMSG pmmsg);
BOOL lboxBeginDrag(PELBOX pelb, PMSEMSG pmmsg);
BOOL lboxChar(PELBOX pelb, PCHRMSG pchmsg);
BOOL lboxMouseMove(PELBOX pelb, PMSEMSG pmmsg);
BOOL lboxEndDrag(PELBOX pelb, PMSEMSG pmmsg);
BOOL lboxTimer(PELBOX pelb, ULONG id);

//==========================================================================\
// crea la child listbox e la subclassa                                     |
//==========================================================================/

BOOL createChild(PELBOX pelb, PELBOXCDATA pelbcd) {
   LONG pp[] = {12, PP_BACKGROUNDCOLOR, 4, pelb->lbkgnd};
   if (!(pelb->hlbx = WinCreateWindow(pelb->hwnd, WC_LISTBOX, NULL,
                                  LS_OWNERDRAW | LS_MULTIPLESEL | 
                                  LS_NOADJUSTPOS | WS_VISIBLE,
                                  0, 0, pelb->szllb.cx, pelb->szllb.cy,
                                  pelb->hwnd, HWND_TOP, FID_LIST,
                                  NULL, (PVOID)pp)) ||
       !(pelb->lbxwprc = WinSubclassWindow(pelb->hlbx, newlboxproc)))
      return FALSE;
   return TRUE;
}


//==========================================================================\
// procedura subclassata listbox:                                           |
// - permette di determinare in quale colonna avviene il mousebutton1click  |
// - permette di spostare gli item al suo interno tramite drag&drop         |
//==========================================================================/

MRESULT EXPENTRY newlboxproc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   PELBOX pelb = (PELBOX)stGetData(WinQueryWindow(hwnd, QW_PARENT));
   switch (msg) {
      case WM_BUTTON1DOWN:
         lboxMouBtn1dwn(pelb, MOUSEMSG(&msg));
         WinPostMsg(hwnd, WM_BUTTON1UP, mp1, mp2);
         break;
      case WM_CHAR:
         if (lboxChar(pelb, CHARMSG(&msg))) return (MRESULT)TRUE;
         break;
      case WM_BEGINDRAG:
         if (lboxBeginDrag(pelb, MOUSEMSG(&msg))) return (MRESULT)TRUE;
         break;
      case WM_MOUSEMOVE:
         if (lboxMouseMove(pelb, MOUSEMSG(&msg))) return (MRESULT)TRUE;
         break;
      case WM_ENDDRAG:
         if (lboxEndDrag(pelb, MOUSEMSG(&msg))) return (MRESULT)TRUE;
         break;
      // implementa autoscrolling
      case WM_TIMER:
         if (lboxTimer(pelb, (ULONG)mp1)) return (MRESULT)FALSE;
         break;
   } /* endswitch */
   return pelb->lbxwprc(hwnd, msg, mp1, mp2);
}


//==========================================================================\
// se si preme il tasto 1 del mouse su un item della listbox notifica       |
// all'owner del controllo l'indice dell'item e della colonna su cui si Š   |
// verificato l'evento                                                      |
//==========================================================================/

VOID lboxMouBtn1dwn(PELBOX pelb, PMSEMSG pmmsg) {
   INT i, idx;
   // trova colonna
   for (i = 0; i < pelb->ccol; ++i) {
      if (pmmsg->x >= pelb->pcol[i].rcl.xLeft &&
          pmmsg->x < pelb->pcol[i].rcl.xRight) {
         // trova indice item
         idx = (SHORT)pelb->lbxwprc(pelb->hlbx, LM_QUERYTOPINDEX,
                                    MPVOID, MPVOID) +
               (pelb->rcltopitem.yTop - pmmsg->y) / pelb->cyitem;
         // notifica owner controllo (se non si clicca su un'area vuota)
         if (idx < pelb->citems) {
            if ((pelb->fl & ELBCS_ITEMCHECKBOXED) &&
                ((pmmsg->x - pelb->pcol[i].rcl.xLeft) < pelb->cyitem))
               WinSendMsg(WinQueryWindow(pelb->hwnd, QW_OWNER), WM_CONTROL,
                          MPFROM2SHORT((USHORT)pelb->id, ELBXN_CHECKED), 
                          MPFROM2SHORT(idx, i));
            else
               WinSendMsg(WinQueryWindow(pelb->hwnd, QW_OWNER), WM_CONTROL,
                          MPFROM2SHORT((USHORT)pelb->id, ELBXN_SELECT), 
                          MPFROM2SHORT(idx, i));
         } else {
            WinAlarm(HWND_DESKTOP, WA_ERROR);
         } // end if
         break;
      } // end if
   } /* endfor */
}


//==========================================================================\
// se il controllo ha il focus e si preme un tasto corrispondente ad un     |
// carattere mnemonico delle descrizioni delle colonne notifica all'owner   |
// l'indice dell'item su cui Š il cursore e l'indice della colonna che      |
// corrisponde al carattere mnemonico                                       |
// restituisce TRUE se processa il carattere, FALSE in caso contrario       |
//==========================================================================/

BOOL lboxChar(PELBOX pelb, PCHRMSG pchmsg) {
   ULONG fs = pchmsg->fs & 0xfff;
   // se cambia cursore notifica owner indicando in mp1 l'ID dell'item con
   // il cursore
   if (!((pchmsg->fs & KC_ALT) || (pchmsg->fs & KC_KEYUP)) &&
       (pchmsg->fs & KC_VIRTUALKEY) &&
       ((pchmsg->vkey == VK_PAGEUP) ||
        (pchmsg->vkey == VK_PAGEDOWN) ||
        (pchmsg->vkey == VK_END) ||
        (pchmsg->vkey == VK_HOME) ||
        (pchmsg->vkey == VK_UP) ||
        (pchmsg->vkey == VK_DOWN))) {
      pelb->lbxwprc(pelb->hlbx, WM_CHAR,
                    MPFROMSH2CH(pchmsg->fs, pchmsg->cRepeat, pchmsg->scancode),
                    MPFROM2SHORT(pchmsg->chr, pchmsg->vkey));
      WinSendMsg(WinQueryWindow(pelb->hwnd, QW_OWNER), WM_CONTROL,
                       MPFROM2SHORT((USHORT)pelb->id, ELBXN_CURSORMOVE),
                       (MPARAM)(pelb->lbxwprc(pelb->hlbx, LM_QUERYSELECTION,
                                              (MPARAM)LIT_CURSOR, MPVOID)));
      return TRUE;
   } /* endif */
   // ignora caratteri selezione item
   if (pchmsg->vkey == VK_SPACE ||
       pchmsg->vkey == VK_NEWLINE ||
       pchmsg->vkey == VK_ENTER ||
       pchmsg->fs & KC_CTRL)
      return TRUE;
   // considera solo caratteri:
   if (fs == 5 || fs == 0xd) {
      INT i;
      SHORT idx;
      for (i = 0; i < pelb->ccol; ++i) {
         if (pchmsg->chr == pelb->pcol[i].mnemo &&
            (idx = (SHORT)pelb->lbxwprc(pelb->hlbx, LM_QUERYSELECTION,
                                 (MPARAM)LIT_CURSOR, MPVOID)) >= 0) {
            WinSendMsg(WinQueryWindow(pelb->hwnd, QW_OWNER), WM_CONTROL,
                       MPFROM2SHORT((USHORT)pelb->id,
                                          (pelb->fl & ELBCS_ITEMCHECKBOXED?
                                                ELBXN_CHECKED: ELBXN_SELECT)), 
                       MPFROM2SHORT(idx, i));
            // cambia stato selezione item per forzarne aggiornamento
            i = (SHORT)pelb->lbxwprc(pelb->hlbx, LM_QUERYSELECTION,
                                     (MPARAM)(idx - 1), MPVOID);
            pelb->lbxwprc(pelb->hlbx, LM_SELECTITEM, (MPARAM)idx,
                          (MPARAM)(i != idx || i < 0));
            break;
         } /* endif */
      } /* endfor */
      return TRUE;
   } /* endif */
   // gli altri caratteri di scrolling sono regolarmente processati
   return FALSE;
}


//==========================================================================\
// inizio procedura drag:                                                   |
// se il controllo non ha lo stile ELBCS_ITEMDRAGGABLE l'evento viene       |
// ignorato, altrimenti:                                                    |
// - si ricava l'indice dell'item draggato                                  |
// - viene catturato il mouse                                               |
// - viene avviato il timer per l'autoscrolling della listbox               |
// la funzione restituisce TRUE se il messaggio viene processato, altrimenti|
// restituisce FALSE                                                        |
//==========================================================================/

BOOL lboxBeginDrag(PELBOX pelb, PMSEMSG pmmsg) {
   if (pelb->fl & ELBCS_ITEMDRAGGABLE) {
      // in caso di errore cancella operazione
      if (pelb->is.dragging) {
         pelb->is.dragging = 0;
         WinSetCapture(HWND_DESKTOP, NULLHANDLE);
         WinStopTimer(pelb->hab, pelb->hlbx, DRAG_TIMERID);
         return FALSE;
      } // end if
      // trova indice item trascinato
      pelb->dragitem = (SHORT)pelb->lbxwprc(pelb->hlbx, LM_QUERYTOPINDEX,
                                            MPVOID, MPVOID) +
                       (pelb->rcltopitem.yTop - pmmsg->y) / pelb->cyitem;
      // se il mouse Š sopra (all'esterno) al primo item o se Š sotto
      // l'ultimo suona allarme e restituisce true
      if (pmmsg->y >= pelb->rcltopitem.yTop || pelb->dragitem >= pelb->citems)
         goto cancel;
      // se tutto Š a posto sposta il cursore sull'item draggato:
      pelb->lbxwprc(pelb->hlbx, WM_BUTTON1DOWN,
                    MPFROM2SHORT(pmmsg->x, pmmsg->y), MPVOID);
      pelb->lbxwprc(pelb->hlbx, WM_BUTTON1UP,
                    MPFROM2SHORT(pmmsg->x, pmmsg->y), MPVOID);
      // notifica owner window evento inizio drag. Se l'owner restituisce
      // TRUE l'operazione di drag viene cancellata
      if (WinSendMsg(WinQueryWindow(pelb->hwnd, QW_OWNER), WM_CONTROL,
                     MPFROM2SHORT((USHORT)pelb->id, ELBXN_DRAGITEM),
                     MPFROM2SHORT(pelb->dragitem, pelb->citems)))
         goto cancel;
      pelb->is.dragging = 1;
      pelb->is.where = ELBOX_DRAGIN;
      WsetSysMouPtr(SPTR_MOVE);
      WinSetCapture(HWND_DESKTOP, pelb->hlbx);
      WinStartTimer(pelb->hab, pelb->hlbx, DRAG_TIMERID, 70);
      return TRUE;
   } // end if
   return FALSE;

cancel:
   pelb->is.dragging = 0;
   WinAlarm(HWND_DESKTOP, WA_ERROR);
   return TRUE;
}


//==========================================================================\
// movimenti mouse:                                                         |
// se il controllo non ha lo stile ELBCS_ITEMDRAGGABLE o non Š in corso     |
// un'operazione di drag l'evento viene ignorato, altrimenti:               |
// viene controllata posizione mouse relativamente al controllo e se questa |
// Š cambiata viene opportunamente cambiata anche la forma del puntatore    |
//==========================================================================/

BOOL lboxMouseMove(PELBOX pelb, PMSEMSG pmmsg) {
   ULONG where;
   SHORT dropitem = (SHORT)pelb->lbxwprc(pelb->hlbx, LM_QUERYTOPINDEX,
                                         MPVOID, MPVOID) +
                    (pelb->rcltopitem.yTop - pmmsg->y + pelb->cyitem / 2) /
                    pelb->cyitem;
   if (!(pelb->fl & ELBCS_ITEMDRAGGABLE) || !pelb->is.dragging)
      return FALSE;
   if (pmmsg->x < 0 || pmmsg->x > pelb->rcltopitem.xRight) {
      where = ELBOX_DRAGOUT; 
   } else if (pmmsg->y < 2) {
      where = ELBOX_DRAGDWN;
   } else if (pmmsg->y >= pelb->rcltopitem.yTop) {
      where = ELBOX_DRAGUP;
   } else {
      where = ELBOX_DRAGIN;
   } /* endif */
   // se il mouse Š all'interno della listbox, notifica ELBXN_DRAGOVER
   // all'owner. Se questo restituisce TRUE viene usato SPTR_ILLEGAL
   // come puntatore del mouse
   if (where == ELBOX_DRAGIN &&
       WinSendMsg(WinQueryWindow(pelb->hwnd, QW_OWNER), WM_CONTROL,
                   MPFROM2SHORT((USHORT)pelb->id, ELBXN_DRAGOVER), 
                   MPFROM2SHORT(dropitem, 0)))
      where = ELBOX_DRAGOUT;
   if (where != pelb->is.where) {
      switch (where) {
         case ELBOX_DRAGOUT:
            WsetSysMouPtr(SPTR_ILLEGAL);
            break;
         case ELBOX_DRAGIN:
            WsetSysMouPtr(SPTR_MOVE);
            break;
         case ELBOX_DRAGUP:
         case ELBOX_DRAGDWN:
            WsetSysMouPtr(SPTR_SIZENS);
            break;
      } /* endswitch */
      pelb->is.where = where;
   } /* endif */
   return TRUE;
}


//==========================================================================\
// fine drag:                                                               |
// se il controllo non ha lo stile ELBCS_ITEMDRAGGABLE o non Š in corso     |
// un'operazione di drag l'evento viene ignorato, altrimenti:               |
// - libera il mouse                                                        |
// - blocca il timer di autoscrolling                                       |
// - se l'operazione termina con il mouse all'interno della listbox e su un |
//   item differente da quello su cui era iniziato il drag, legge dati item |
//   draggato, li replica inserendo un nuovo item con gli stessi dati nella |
//   posizione di drop e infine cancella l'item draggato dalla precedente   |
//   posizione                                                              |
// la funzione restituisce TRUE se l'evento viene processato altrimenti     |
// restituisce FALSE                                                        |
//==========================================================================/

BOOL lboxEndDrag(PELBOX pelb, PMSEMSG pmmsg) {
   LONG rc;
   if (!(pelb->fl & ELBCS_ITEMDRAGGABLE) || !pelb->is.dragging)
      return FALSE;
   WinSetCapture(HWND_DESKTOP, NULLHANDLE);
   WinStopTimer(pelb->hab, pelb->hlbx, DRAG_TIMERID);
   pelb->is.dragging = 0;
   if (pelb->is.where == ELBOX_DRAGIN) {
      PVOID pitemdata;
      PSZ psz;
      ULONG cb;
      SHORT dropitem = (SHORT)pelb->lbxwprc(pelb->hlbx, LM_QUERYTOPINDEX,
                                            MPVOID, MPVOID) +
                       (pelb->rcltopitem.yTop - pmmsg->y + pelb->cyitem / 2) /
                       pelb->cyitem;
      if (dropitem != pelb->dragitem && dropitem != (pelb->dragitem + 1)) {
         rc = (LONG)WinSendMsg(WinQueryWindow(pelb->hwnd, QW_OWNER),
                               WM_CONTROL,
                               MPFROM2SHORT((USHORT)pelb->id, ELBXN_DROPITEM), 
                               MPFROM2SHORT(pelb->dragitem, dropitem));
         if (rc < 0) goto cancel;
         if (rc > 0) return TRUE;
         // ricava dati item draggato
         cb = (ULONG)pelb->lbxwprc(pelb->hlbx, LM_QUERYITEMTEXTLENGTH,
                                   (MPARAM)pelb->dragitem, MPVOID) + 1;
         if (NULL != (psz = (PSZ)malloc(cb))) {
            pelb->lbxwprc(pelb->hlbx, LM_QUERYITEMTEXT,
                          MPFROM2SHORT(pelb->dragitem, cb), (MPARAM)psz);
            pitemdata = pelb->lbxwprc(pelb->hlbx, LM_QUERYITEMHANDLE,
                                      (MPARAM)pelb->dragitem, MPVOID);
            WinEnableWindowUpdate(pelb->hlbx, FALSE);
            // lo copia nella nuova posizione
            if (dropitem >= pelb->citems) dropitem = LIT_END;
            dropitem = (SHORT)pelb->lbxwprc(pelb->hlbx, LM_INSERTITEM,
                                            (MPARAM)dropitem, (MPARAM)psz);
            pelb->lbxwprc(pelb->hlbx, LM_SETITEMHANDLE, (MPARAM)dropitem,
                          (MPARAM)pitemdata);
            free(psz);
            // se inserzione nuovo item precede vecchio aumenta indice
            if (dropitem < pelb->dragitem) ++pelb->dragitem;
            else --dropitem;
            // cancella item da vecchia posizione
            pelb->lbxwprc(pelb->hlbx, LM_DELETEITEM,
                          (MPARAM)pelb->dragitem, MPVOID);
            // seleziona item riposizionato
            pelb->lbxwprc(pelb->hlbx, LM_SELECTITEM,
                          (MPARAM)dropitem, (MPARAM)TRUE);
            WinEnableWindowUpdate(pelb->hlbx, TRUE);
            return TRUE;
         } /* endif */
      } /* endif */
   } /* endif */
cancel:
   WinAlarm(HWND_DESKTOP, WA_ERROR);
   return TRUE;
} /* endif */


//==========================================================================\
// messaggio di autoscroll:                                                 |
// se il controllo non ha lo stile ELBCS_ITEMDRAGGABLE o non Š in corso     |
// un'operazione di drag o l'ID del timer non Š giusta o il mouse non Š al  |
// di sopra o al di sotto della listbox non processa il messaggio,          |
// altrimenti setta il top index della listbox secondo la posizione del     |
// mouse                                                                    |
//==========================================================================/

BOOL lboxTimer(PELBOX pelb, ULONG id) {
   SHORT topitem;
   if (!(pelb->fl & ELBCS_ITEMDRAGGABLE) ||
       !pelb->is.dragging ||
       id != DRAG_TIMERID ||
       pelb->is.where == ELBOX_DRAGIN ||
       pelb->is.where == ELBOX_DRAGOUT)
      return FALSE;
   topitem = (SHORT)pelb->lbxwprc(pelb->hlbx, LM_QUERYTOPINDEX,
                                  MPVOID, MPVOID);
   if (pelb->is.where == ELBOX_DRAGUP) {
      if (topitem)      // se il topitem non Š gi… 0
         pelb->lbxwprc(pelb->hlbx, LM_SETTOPINDEX,
                       (MPARAM)(pelb->topitem = --topitem), MPVOID);
      else
         WsetSysMouPtr(SPTR_ILLEGAL);
   } else {             // se non Š gi… il topmost item
      if (topitem >= pelb->topitem)
         pelb->lbxwprc(pelb->hlbx, LM_SETTOPINDEX,
                       (MPARAM)(pelb->topitem = ++topitem), MPVOID);
      else
         WsetSysMouPtr(SPTR_ILLEGAL);
   } /* endif */
   return TRUE;
}
