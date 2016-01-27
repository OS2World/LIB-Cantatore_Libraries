//========================================================================\
// common.c                                                               |
// FUNZIONI: ------------------------------------------------------------ |
// APIRET APIENTRY InitControls(HAB hab);                                 |
//========================================================================/


#pragma strings(readonly)

#define INCL_WIN
#define INCL_GPI

#include <os2.h>
//#include <winutil.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "common.h"
#include "api.h"
#include "pmapi.h"


//==========================================================================\
// Centra la finestra di dialogo hwnd all'interno di hin mostrandola,       |
// attivandola e portandola al top dello Z-order.                           |
// Se hin = NULLHANDLE centra la finestra all'interno della parent window   |
// parametri:                                                               |
// HWND hwnd = handle della finestra da centrare                            |
// HWND hin = handle della finestra in cui viene effettuata la centratura   |
// valore restituito:                                                       |
// BOOL = TRUE/FALSE : successo/errore                                      |
//==========================================================================/

BOOL APIENTRY WcenterInWindow(HWND hwnd, HWND hin) {
   WRECTL wrp, wrw;
   HWND hParent = WinQueryWindow(hwnd, QW_PARENT);
   if (!hin) hin = hParent;
   if ((hin == hParent || WinIsWindow(WinQueryAnchorBlock(hwnd), hin)) &&
       WinQueryWindowRect(hin, (PRECTL)&wrp) &&
       WinQueryWindowRect(hwnd, (PRECTL)&wrw)) {
      wrp.x = (wrp.cx - wrw.cx) / 2;
      wrp.y = (wrp.cy - wrw.cy) / 2;
   } else {
      hin = NULLHANDLE;
   } // end if
   if (!(hin && WinMapWindowPoints(hin, hParent, (PPOINTL)&wrp, 1)))
      wrp.x = wrp.y = 0;
   return WinSetWindowPos(hwnd, HWND_TOP, wrp.x, wrp.y, 0, 0,
                          SWP_MOVE | SWP_ACTIVATE | SWP_SHOW | SWP_ZORDER);  
}


//==========================================================================\
// Centra la finestra di dialogo hwnd all'interno della sua parent window   |
// mostrandola, attivandola e portandola al top dello Z-order.              |
// parametri:                                                               |
// HWND hwnd = handle della dialog window                                   |
// valore restituito:                                                       |
// BOOL = TRUE/FALSE : successo/errore                                      |
//==========================================================================/

BOOL APIENTRY WcenterWindow(HWND hwnd) {
   WRECTL wrp, wrw;
   return (BOOL)
       WinQueryWindowRect(WinQueryWindow(hwnd, QW_PARENT), (PRECTL)&wrp) &&
       WinQueryWindowRect(hwnd, (PRECTL)&wrw) &&
       WinSetWindowPos(hwnd, HWND_TOP,
                       (wrp.cx - wrw.cx) / 2, (wrp.cy - wrw.cy) / 2, 0, 0,
                       SWP_MOVE | SWP_ACTIVATE | SWP_SHOW | SWP_ZORDER);  
}


//==========================================================================\
// processa messaggio WM_MINMAXFRAME nei dialoghi nascondendo/mostrando     |
// i controlli i cui ID sono nell'array passato come parametro allo scopo   |
// di ottenere una corretta minimizzazione del dialogo sulla scrivania.     |
// pul Š l'indirizzo di un array di ID di controlli. Il primo membro        |
// contiene il numero di elementi dell'array.                               |
// parametri:                                                               |
// HWND hwnd = handle dialogo                                               |
// MPARAM mp1 = parametro 1 del messaggio WM_MINMAXFRAME                    |
// PULONG pul = indirizzo array ID controlli da nascondere/mostrare. Il     |
//              primo membro contiene il numero di elementi dell'array      |
//              (ovvero il numero dei controlli + 1).                       |
// valore restituito:                                                       |
// il valore restituito dalla procedura di default dei dialoghi             |
//==========================================================================/

MRESULT APIENTRY WdlgMinMax(HWND hwnd, MPARAM mp1, PULONG pul) {
   ULONG fl = !(((PSWP)mp1)->fl & SWP_MINIMIZE);
   INT i;
   for (i = 1; i < *pul; ++i) DlgShowCtl(hwnd, pul[i], fl);
   return WinDefDlgProc(hwnd, WM_MINMAXFRAME, mp1, MPVOID);
}


//==========================================================================\
// Disegna sfondo e bordo bottone nello stile di warp 4                     |
// parametri:                                                               |
// PUSERBUTTON pub = indirizzo della struttura usata per i bottone          |
//                   BS_OWNERDRAW                                           |
// ULONG cx = larghezza bottone                                             |
// ULONG cy = altezza bottone                                               |
// LONG color = colore background. Per gli altri colori vengono usati i     |
//              valori dei colori di sistema per i bordi.                   |
// valore restituito:                                                       |
// BOOL = TRUE/FALSE : successo/errore                                      |
//==========================================================================/

BOOL APIENTRY WdrawBtnBkgnd(PUSERBUTTON pub, ULONG cx, ULONG cy, LONG color) {
   RECTL rcl;
   GpiCreateLogColorTable(pub->hps, 0, LCOLF_RGB, 0, 0, NULL);
   // cancella background
   rcl.xLeft = 0;
   rcl.yBottom = 0;
   rcl.xRight = cx;
   rcl.yTop = cy;
   GpiMove(pub->hps, (PPOINTL)&rcl);
   GpiSetColor(pub->hps, color);
   GpiBox(pub->hps, DRO_FILL, (PPOINTL)&rcl + 1, 0L, 0L);
   // disegna primo bordo
   rcl.xLeft = 1;
   rcl.yBottom = 1;
   rcl.xRight = cx - 1;
   rcl.yTop = cy - 1;
   if (!WinDrawBorder(pub->hps, &rcl, 1, 1, 0, 0, 0x800)) return FALSE;
   // disegna secondo bordo
   rcl.xLeft = 2;
   rcl.yBottom = 2;
   rcl.xRight = cx - 2;
   rcl.yTop = cy - 2;
   if (!WinDrawBorder(pub->hps, &rcl, 2, 2, 0, 0,
                      (pub->fsState & BDS_HILITED? 0x800: 0x400)))
      return FALSE;
   return TRUE;
}


//==========================================================================\
// disegna il rettangolo prcl halftoned con il colore color usando un       |
// pattern differente secondo la posizione in modo da ottenere sempre lo    |
// stesso effetto di halftoning                                             |
// parametri:                                                               |
// HPS hps = handle spazio presentazione                                    |
// PRECTL prcl = coordinate rettangolo                                      |
// LONG color = colore pattern                                              |
// valore restituito:                                                       |
// BOOL = TRUE/FALSE : successo/errore                                      |
//==========================================================================/

BOOL APIENTRY WdrawDisabledRect(HPS hps, PRECTL prcl, LONG color) {
   RECTL rcl = *prcl;
   rcl.xRight--;
   rcl.yTop--;
   return GpiMove(hps, (PPOINTL)&rcl) &&
          GpiSetPattern(hps, (((prcl->xLeft % 2) != (prcl->yBottom % 2))?
                                          PATSYM_DENSE5: PATSYM_HALFTONE)) &&
          GpiSetColor(hps, color) &&
          GpiBox(hps, DRO_FILL, (PPOINTL)&rcl + 1, 0L, 0L);
}


//==========================================================================\
// disegna il testo disabilitato con effetto 3D di incisione                |
// parametri:                                                               |
// HPS hps = handle spazio presentazione                                    |
// LONG cch = lunghezza stringa testo                                       |
// PSZ psz = testo da scrivere                                              |
// PRECTL prcl = rectangle enclosing the text                               |
// LONG clrNW = foreground north-west color                                 |
// LONG clrSE = foreground south-est color                                  |
// LONG clrbkgnd = background color                                         |
// ULONG fl = WinDrawText() flags                                           |
// valore restituito:                                                       |
// LONG number of characters drawn (0 in case of error)                     |
//==========================================================================/

LONG APIENTRY WdrawDisabledText(HPS hps, LONG cch, PSZ psz, PRECTL prcl,
                           LONG clrNW, LONG clrSE, LONG clrbkgnd, ULONG fl) {
   LONG cchdrawn;
   if (fl & DT_ERASERECT) {
      if (!WinFillRect(hps, prcl, clrbkgnd)) return 0;
      fl &= ~DT_ERASERECT;
   } /* endif */
   WmoveRect(prcl, 1, -1);
   cchdrawn = WinDrawText(hps, cch, psz, prcl, clrSE, 0, fl);
   WmoveRect(prcl, -1, 1);
   if (WinDrawText(hps, cch, psz, prcl, clrNW, 0, fl) != cchdrawn) return 0;
   return cchdrawn;
}


//==========================================================================\
// Legge il file pszfile e lo importa nella MLE hmle                        |
// parametri:                                                               |
// HWND hmle = handle MLE                                                   |
// PSZ pszfile = nome file                                                  |
// valore restituito:                                                       |
// BOOL = TRUE/FALSE : successo/errore                                      |
//==========================================================================/

BOOL APIENTRY Wfil2mle(HWND hmle, PSZ pszfile) {
   PSZ psz = NULL;
   HFILE hf = 0;
   ULONG ul;
   IPT ipt = -1;
   BOOL rc = FALSE;

   // alloca buffer
   if (DosAllocMem((PPVOID)&psz, 0x9000,
                   PAG_READ | PAG_WRITE | PAG_COMMIT | OBJ_TILE))
      return FALSE;
   if (DosOpen (pszfile, &hf, &ul, 0, FILE_NORMAL, FILE_OPEN,
                OPEN_FLAGS_FAIL_ON_ERROR | OPEN_FLAGS_SEQUENTIAL |
                OPEN_SHARE_DENYWRITE | OPEN_ACCESS_READONLY, NULL))
      goto end0;

   if (!WinSendMsg(hmle, MLM_SETIMPORTEXPORT, (MPARAM)psz, (MPARAM) 0x9000))
      goto end1;

   do {
      if (DosRead(hf, psz, 0x8000, &ul)) goto end1;
      WinSendMsg(hmle, MLM_IMPORT, (MPARAM)&ipt, (MPARAM)ul);
   } while (ul == 0x8000); /* enddo */
   rc = TRUE;
end1:
   DosClose(hf);
end0:
   DosFreeMem(psz);
   return rc;
}


//==========================================================================\
// sostituire con LM_INSERTMULTIITEMS                                       |
// riempie una listbox con gli item contenuti in una stringa consistente    |
// in una sequenza di stringhe terminate da un carattere NULL. Un doppio    |
// carattere NULL indica il termine della sequenza                          |
// parametri:                                                               |
// HWND hwnd = handle della dialog window                                   |
// ULONG ulId = id combo-listbox                                            |
// PSZ pszList = array di stringhe di testo da inserire nella lista         |
// valore restituito:                                                       |
// BOOL = TRUE/FALSE : successo/errore                                      |
//==========================================================================/

BOOL APIENTRY WfillListBox(HWND hwnd, ULONG ulId, PSZ pszList) {
   ULONG ulLength;          
   while (0 != (ulLength = strlen(pszList))) {
      if (0 > (SHORT)WinSendDlgItemMsg(hwnd, ulId, LM_INSERTITEM, 
                                       MPFROMLONG(LIT_END), MPFROMP(pszList)))
         return FALSE;
      pszList += ++ulLength;
   } /* endwhile */
   return TRUE;
}


//==========================================================================\
// ottiene il colore con cui disegnare una parte del controllo da un        |
// presentation parameter se presente, altrimenti converte ldef in colore   |
// RGB.                                                                     |
// parametri: ------------------------------------------------------------- |
// HWND hwnd   : handle controllo                                           |
// ULONG ulid1 : id presentation parameter                                  |
// ULONG ulid2 : id presentation parameter index                            |
// LONG ldef   : colore default espresso come SYSCLR_ o CLR_                |
// valore restituito: ----------------------------------------------------- |
// LONG : RGB color - 0 (nero) in caso di errore                            |
//==========================================================================/

LONG APIENTRY WgetCtlClr(HWND hwnd, HPS hps, ULONG ulid1, ULONG ulid2, LONG ldef) {
   LONG lclr;

   // se trova colore definito come Presentation Parameter lo restituisce
   if (WinQueryPresParam(hwnd, ulid1, ulid2, NULL, 4UL, (PVOID)&lclr,
                         QPF_NOINHERIT | QPF_PURERGBCOLOR |
                         (ulid2? QPF_ID2COLORINDEX: 0))) {
      return lclr;
   // se ldef Š indice colore sistema valido restituisce valore corrispondente
   } // end if
   if ((ldef >= SYSCLR_SHADOWHILITEBGND) && (ldef <= SYSCLR_HELPHILITE)) {
      return WinQuerySysColor(HWND_DESKTOP, ldef, 0L);
   // se Š indice colore CLR_ lo converte nel corrispondente valore RGB
   } // end if
   if ((lclr = GpiQueryRGBColor(hps, LCOLOPT_REALIZED, ldef)) < 0)  lclr = 0;
   return(lclr);
}


//==========================================================================\
// Carica la bitmap id dal modulo hmod. Da utilizzare al posto di           |
// GpiLoadBitmap quando si deve caricare una sola bitmap e non si ha gi…    |
// un handle del presentation space                                         |
// parametri:                                                               |
// HWND hwnd = handle finestra                                              |
// HMODULE hmod = handle modulo contente bitmap                             |
// ULONG id = id bitmap da caricare                                         |
// valore restituito: =                                                     |
// HBITMAP handle della bitmap caricata o NULLHANDLE in caso di errore      |
//==========================================================================/

HBITMAP APIENTRY WloadBitmap(HWND hwnd, HMODULE hmod, ULONG id) {
   HPS hps = WinGetPS(hwnd);
   HBITMAP hbmp = GpiLoadBitmap(hps, hmod, id, 0, 0);
   WinReleasePS(hps);
   return hbmp; 
}

      
//==========================================================================\
// legge il contenuto della MLE hmle e lo scrive nel file pszfile           |
// parametri:                                                               |
// HWND hmle = handle finestra MLE                                          |
// PSZ pszfile = nome del file                                              |
// valore restituito:                                                       |
// BOOL = TRUE/FALSE : successo/errore                                      |
//==========================================================================/

BOOL APIENTRY Wmle2fil(HWND hmle, PSZ pszfile) {
   PVOID psz;
   HFILE hf;
   ULONG ul, cbexp;
   IPT ipt = 0;
   LONG cb;
   BOOL rc = FALSE;

   if (DosOpen(pszfile, &hf, &ul, 0, FILE_NORMAL, OPEN_ACTION_CREATE_IF_NEW |
               OPEN_ACTION_REPLACE_IF_EXISTS, OPEN_FLAGS_FAIL_ON_ERROR |
               OPEN_SHARE_DENYREADWRITE| OPEN_ACCESS_WRITEONLY, NULL))
      return FALSE;
   if ((cb = (LONG)WinSendMsg(hmle, MLM_QUERYFORMATTEXTLENGTH, MPVOID,
                             (MPARAM)0xffffffff)) != 0) {
      if (DosAllocMem(&psz, 0x10000,
                      PAG_READ | PAG_WRITE | PAG_COMMIT | OBJ_TILE))
         goto end0;
      if (!WinSendMsg(hmle, MLM_SETIMPORTEXPORT, (MPARAM)psz, (MPARAM)0x10000))
         goto end1;
      do {
         cbexp = (ULONG)WinSendMsg(hmle, MLM_EXPORT, (MPARAM)&ipt, (MPARAM)&cb);
         if (DosWrite(hf, psz, cbexp, &ul)) goto end1;
      } while (cb); /* enddo */
   } /* endif */
   rc = TRUE;

end1:
   DosFreeMem(psz);
end0:
   DosClose(hf);
   return rc;
}


//==========================================================================\
// legge il contenuto di una MLE e lo copia in memoria che deve essere      |
// precedentemente allocata. Se pszstr Š NULL si limita a restituire la     |
// quantit… di memoria da allocare (contenuto della MLE + 1 per lo zero di  |
// terminazione)                                                            |
// (modificare aggiungendo parametri start e cbcopy)                        |
// HWND hmle = handle finestra MLE                                          |
// PSZ pszstr = indirizzo in cui copiare contenuto MLE                      |
// parametri:                                                               |
// valore restituito:                                                       |
// ULONG quantit… caratteri copiati                                         |
//==========================================================================/

ULONG APIENTRY Wmle2str(HWND hmle, PSZ pszstr) {
   PVOID psz = NULL;
   HFILE hf;
   ULONG rc, cbret, cbexp;
   IPT ipt = 0;
   LONG cb;

   rc = 0;
   cb = (LONG)WinSendMsg(hmle, MLM_QUERYFORMATTEXTLENGTH,
                         MPVOID, (MPARAM)0xffffffff);
   if (!pszstr) return cb + 1;

   if (cb) {
      cbret = cb;
      if (DosAllocMem(&psz, cb,
                      PAG_READ | PAG_WRITE | PAG_COMMIT | OBJ_TILE))
         return rc;
      if (!WinSendMsg(hmle, MLM_SETIMPORTEXPORT, (MPARAM)psz, (MPARAM)0x10000))
         goto end;
      do {
         cbexp = (ULONG)WinSendMsg(hmle, MLM_EXPORT, (MPARAM)&ipt, (MPARAM)&cb);
         memcpy(pszstr, psz, cbexp);
         pszstr += cbexp;
      } while (cb > 0); /* enddo */
   } /* endif */
   rc = cbret;

end:
   DosFreeMem(psz);
   return rc;
}


//==========================================================================\
// sposta le coordinate del rettangolo secondo i parametri x e y            |
// sostituire con una macro                                                 |
// parametri:                                                               |
// PRECTL prcl: indirizzo struttura dati rettangolo                         |
// LONG x: spostamento lungo l'asse x                                       |
// LONG y: spostamento lungo l'asse y                                       |
// valore restituito:                                                       |
// VOID:                                                                    |
//==========================================================================/

VOID APIENTRY WmoveRect(PRECTL prcl, LONG x, LONG y) {
   prcl->xLeft += x;
   prcl->yBottom += y;
   prcl->xRight += x;
   prcl->yTop += y;
}


//==========================================================================\
// apre un oggetto usando gli stessi parametri della funzione REXX          |
// SysOpenObject()                                                          |
// parametri:                                                               |
// PSZ pszObj : nome simbolico o path completo oggetto                      |
// ULONG flmode : (OPEN_SETTINGS, OPEN_TREE, OPEN_DEFAULT, OPEN_CONTENTS,   |
///                OPEN_DETAILS)                                            |
// ULONG flOpt : FALSE = eventualmente apre nuova occorrenza oggetto        |
// valore restituito:                                                       |
// BOOL : TRUE = successo, FALSE = errore                                   |
//==========================================================================/

BOOL APIENTRY WopenObject(PSZ pszObj, ULONG flmode, ULONG flOpt) {
   HOBJECT hobj;
   if (NULLHANDLE == (hobj = WinQueryObject(pszObj)) ||
       !WinOpenObject(hobj, flmode, flOpt))
      return FALSE;
   return TRUE;
}


//==========================================================================\
// mostra una message box stile warning, inizializzando l'ambiente PM se    |
// necessario                                                               |
// parametri:                                                               |
// HWND hwnd: handle owner window                                           |
// PSZ pszMsg: testo messaggio da visualizzare                              |
//             se il testo contiene un carattere di tabulazione, la parte   |
//             che precede il carattere di tabulazione stesso viene         |
//             interpretata come titolo della message box, mentre il resto  |
//             viene interpretato come il messaggio vero e proprio          |
// BOOL fl: flag:  PMPRNT_WININIT = richiede inizializzazione               |
//                 PMPRNT_ERROR (default) = errore                          |
//                 PMPRNT_WARN = stile Warning                              |
//                 PMPRNT_INFO = stile info                                 |
//                 PMPRNT_QUERY = richiesta conferma                        |
//          in modalit… PMPRNT_QUERY si possono usare le seguenti flag per  |
//          mostrare un'icona differente da quella di default (?) :         |
//          PMPRNT_QWARNICO = icona warning (!)                             |
//          PMPRNT_QINFOICO = icona informazioni (i)                        |
//          PMPRNT_QERRORICO = icona errore                                 |
//          PMPRNT_QQUERYICO = icona query (?) (default)                    |
// valore restituito:                                                       |
// BOOL: TRUE se si Š premuto YES o OK                                      |
// aggiungere flag x permettere concatenamento stringhe titolo e pszMsg     |
//==========================================================================/

BOOL APIENTRY Wprint(HWND hwnd, PSZ pszMsg, ULONG fl) {
   HAB hab = NULLHANDLE;
   HMQ hmq = NULLHANDLE;
   PSZ pszTitle;
   PSZ psz;
   ULONG fStyle = MB_MOVEABLE;
   ULONG res;
   if (fl & PMPRNT_WARN) {
      pszTitle = SZ_WARNING;
      fStyle |= MB_OK | MB_WARNING;
   } else if (fl & PMPRNT_INFO) {
      pszTitle = SZ_NOTE;
      fStyle |= MB_OK | MB_INFORMATION;
   } else if (fl & PMPRNT_QUERY) {
      pszTitle = SZ_QUERY;
      fStyle |= MB_YESNO;
      if (fl & PMPRNT_QWARNICO) {
         fStyle |= MB_WARNING;
      } else if (fl & PMPRNT_QINFOICO) {
         fStyle |= MB_INFORMATION;
      } else if (fl & PMPRNT_QERRORICO) {
         fStyle |= MB_ERROR;
      } else {
         fStyle |= MB_QUERY;
      } /* endif */
   } else {       
      pszTitle = SZ_ERROR;
      fStyle |= MB_OK | MB_ERROR;
   } /* endif */
   if (fl & PMPRNT_SYSMOD) {
      fStyle |= MB_SYSTEMMODAL;
   } /* endif */
   if (fl & 0x80000000)    // il valore 0x80000000 Š riservato per indicare
      fl &= ~0x80000000;  // avvenuta allocazione
   // controlla che in pszMsg non ci sia anche il titolo
   if (NULL != (psz = strchr(pszMsg, '\t')) &&
       NULL != (pszTitle = malloc(psz - pszMsg))) {
      fl |= 0x80000000;
      memcpy(pszTitle, pszMsg, psz - pszMsg);
      *(pszTitle + (ULONG)(psz - pszMsg)) = 0;
      pszMsg = psz + 1;
   } /* endif */
   if (fl & PMPRNT_WININIT) {
      hab = WinInitialize(0);
      hmq = WinCreateMsgQueue(hab, 0);
   } /* endif */
   res = WinMessageBox(HWND_DESKTOP, hwnd, pszMsg, pszTitle, 0, fStyle);
   if (fl & PMPRNT_WININIT) {
      WinDestroyMsgQueue(hmq);
      WinTerminate(hab);
   } /* endif */
   if (fl & 0x80000000) free(pszTitle);
   return (res == MBID_YES || res == MBID_OK);
}


//==========================================================================\
// elabora il messaggio WM_CONTROL del controllo spinbutton                 |
// restituisce TRUE alla fine dell'operazione di cambiamento valore del     |
// controllo, altrimenti restituisce FALSE                                  |
// parametri:                                                               |
// ULONG ulEvent: evento SPBN_                                              |
// HWND hspin: handle spinbutton                                            |
// PLONG pl: indirizzo in cui memorizzare valore corrente controllo         |
// LONG lcur: valore precedente variabile in corso di modificazione tramite |
//            lo spinbutton                                                 |
// valore restituito:                                                       |
// BOOL : TRUE = il valore mostrato nella spinbox Š cambiato.               |
//        FALSE = errore oppure valore immutato                             |
// esempio:                                                                 |
//         if (WprocSpin(ulEvent, hctl, &lnewvalue, lcurrvalue)) {          |
//            lcurrvalue = lnewvalue;                                       |
//            EnableToolbarUndoBtn(1);                                      |
//         }                                                                |
//==========================================================================/

BOOL APIENTRY WprocSpin(ULONG ulEvent, HWND hspin, PLONG pl, LONG lcur) {
   static BOOL spin;
   switch (ulEvent) {
      // solo per sicurezza nel caso il sistema non invii SPBN_ENDSPIN
      case SPBN_SETFOCUS: spin = 0; return FALSE;
      // eventi realmente utili:
      case SPBN_UPARROW:
      case SPBN_DOWNARROW: spin = 1; return FALSE;
      case SPBN_ENDSPIN: spin = 0;
      case SPBN_CHANGE:
         return (!spin &&
                 (BOOL)WinSendMsg(hspin, SPBM_QUERYVALUE, (MPARAM)pl,
                                  MPFROM2SHORT(0, SPBQ_ALWAYSUPDATE)) &&
                 *pl != lcur);
   } // end switch
   return FALSE;
}



//==========================================================================\
// restituisce le dimensioni di una bitmap dall'handle della bitmap stessa  |
// parametri:                                                               |
// HBITMAP hbmp = bitmap handle                                             |
// PSIZEL pszl = indirizzo in cui memorizzare dimensione bitmap             |
// valore restituito:                                                       |
// BOOL : TRUE = successo, FALSE = errore                                   |
//==========================================================================/

BOOL APIENTRY WqueryBitmapSize(HBITMAP hbmp, PSIZEL pszl) {
   BITMAPINFOHEADER bih;
   bih.cbFix = sizeof(BITMAPINFOHEADER);
   if (!GpiQueryBitmapParameters(hbmp, &bih)) return FALSE;
   pszl->cx = bih.cx;
   pszl->cy = bih.cy;
   return TRUE;
}


//==========================================================================\
// ritorna l'handle della frame principale della finestra hwnd              |
// (per frame principale si intende il child di HWND_DESKTOP)               |
// parametri:                                                               |
// HWND hwnd = finestra di cui si vuole rintracciare la frame window        |
// valore restituito:                                                       |
// HWND handle frame window contenente la finestra o un suo parent          |
//==========================================================================/

HWND APIENTRY WqueryHwndFrame(HWND hwnd) {
   HWND hPar, hDesk;
   for (hDesk = WinQueryDesktopWindow(WinQueryAnchorBlock(hwnd), NULLHANDLE);
        (hPar = WinQueryWindow(hwnd, QW_PARENT)) != hDesk;
        hwnd = hPar) {
   } // end for
   return hwnd;
}


//==========================================================================\
// cambia la bitmap mostrata dal bottone                                    |
// parametri: --------------------------------------------------------------|
// HWND hwnd: handle dialogo                                                |
// ULONG ulId: ID bottone                                                   |
// HBITMAP hbmp: handle bitmap                                              |
// valore restituito:                                                       |
// BOOL : TRUE = successo, FALSE = errore                                   |
//==========================================================================/

BOOL APIENTRY WsetButtonBmp(HWND hwnd, ULONG ulId, HBITMAP hbmp) {
   WNDPARAMS wp;
   BTNCDATA bcd;
   HBITMAP hbmpold;
   wp.fsStatus = WPM_CTLDATA;
   wp.cbCtlData = sizeof(BTNCDATA);
   wp.pCtlData = &bcd;
   if (!WinSendDlgItemMsg(hwnd, ulId, WM_QUERYWINDOWPARAMS,
                          MPFROMP((PVOID)&wp), MPVOID)) return FALSE;
   hbmpold = bcd.hImage;
   bcd.hImage = hbmp;
   if (!WinSendDlgItemMsg(hwnd, ulId, WM_SETWINDOWPARAMS,
                          MPFROMP((PVOID)&wp), MPVOID)) return FALSE;
   return GpiDeleteBitmap(hbmpold);
}


//==========================================================================\
// cambia il bottone di default di un dialogo                               |
// parametri:                                                               |
// HWND hwnd = handle dialog window                                         |
// ULONG id = handle bottone                                                |
// valore restituito:                                                       |
// BOOL : TRUE = successo, FALSE = errore                                   |
//==========================================================================/

BOOL APIENTRY WsetDefBtn(HWND hwnd, ULONG id) {
   HWND hnew = WinWindowFromID(hwnd, id);
   HWND hcur = WinQueryWindowULong(hwnd, QWL_DEFBUTTON);
   if (!hnew) return FALSE;
   if (hnew != hcur) {
      if (hcur)
         if (!WinSendMsg(hcur, BM_SETDEFAULT, MPVOID, MPVOID)) return FALSE;
      return (BOOL)WinSendMsg(hnew, BM_SETDEFAULT, (MPARAM)TRUE, MPVOID);
   } /* endif */
   return TRUE;
}


//==========================================================================\
// imposta come font del dialogo hwnd pszFont e resetta il font della       |
// titlebar al default di sistema. Se hTitle == 0 ricava l'handle della     |
// titlebar. Se hTitle == -1 ignora il font della titlebar                  |
// parametri:                                                               |
// HWND hwnd   : handle del dialogo                                         |
// HWND htbar  : handle barra titolo                                        |
// PSZ pszFont : nuovo font dialogo                                         |
// valore restituito:                                                       |
// BOOL : TRUE/FALSE = successo/errore                                      |
//==========================================================================/

BOOL APIENTRY WsetDlgFonts(HWND hwnd, HWND htbar, PSZ pszFont) {
   CHAR buf[128];
   ULONG ul;
   if (!WinSetPresParam(hwnd, PP_FONTNAMESIZE, strlen(pszFont) + 1,
                        (PVOID)pszFont)) return FALSE;
   // imposta il font della barra del titolo:
   if (htbar != (HWND)-1 &&
       (htbar ||                                                        
        (NULLHANDLE != (htbar = WinWindowFromID(hwnd, FID_TITLEBAR))))) {
      if (!WinQueryPresParam(htbar, PP_FONTNAMESIZE, 0, NULL, 128,
                              (PVOID)buf, QPF_NOINHERIT))
         if (!PrfQueryProfileString(HINI_USERPROFILE, "PM_SystemFonts",
                                    "WindowTitles", NULL, buf, 128))
            return FALSE;     
      return WinSetPresParam(htbar, PP_FONTNAMESIZE, strlen(buf) + 1,
                             (PVOID)buf);
   } // end if
   return TRUE;
}


//==========================================================================\
// Imposta l'icona di una finestra di dialogo caricando la risorsa id dal   |
// modulo hmod. Se id = 0 considera hmod come l'indirizzo della stringa     |
// del nome del file di icona                                               |
// parametri:                                                               |
// HWND hwnd    : handle della dialog window                                |
// PVOID hmod   : indirizzo handle modulo contente icona o nome file icona  |
// ULONG id     : id risorsa                                                |
// valore restituito:                                                       |
// BOOL: TRUE/FALSE = successo/errore                                       |
//==========================================================================/

BOOL APIENTRY WsetDlgIcon(HWND hwnd, HMODULE hmod, ULONG id) {
   HPOINTER hptr = WinLoadPointer(HWND_DESKTOP, hmod, id);
   BOOL rc = (BOOL)WinDefDlgProc(hwnd, WM_SETICON, (MPARAM)hptr, MPVOID);
   if (!rc) {
      if (hptr) WinDestroyPointer(hptr);
      return NULLHANDLE;
   } /* endif */
   return hptr;
}


//==========================================================================\
// Setta la posizione di hwnd relativamente alla finestra il cui handle Š   |
// passato nella struttura pswp                                             |
// parametri:                                                               |
// HWND hwnd = handle window da riposizionare                               |
// HWND hwndRelative = finestra cui sono relazionate le coordinate          |
// PSWP pswp = struttura coordinate e flag finestra                         |
// valore restituito:                                                       |
// BOOL: TRUE/FALSE = successo/errore                                       |
//==========================================================================/

BOOL APIENTRY WsetRelativePos(HWND hwnd, PSWP pswp) {
   POINTL ptl = {pswp->x, pswp->y};
   HWND hParent = WinQueryWindow(hwnd, QW_PARENT);
   if (!pswp->hwnd) {
      pswp->hwnd = hParent;
   } else if (WinIsWindow(WinQueryAnchorBlock(hwnd), pswp->hwnd) &&
              !WinMapWindowPoints(pswp->hwnd, hParent, (PPOINTL)&ptl, 1)) {
      ptl.x = ptl.y = 0;
   } // end if
   return WinSetWindowPos(hwnd, pswp->hwndInsertBehind, ptl.x, ptl.y,
                          pswp->cx, pswp->cy, pswp->fl);  
}


//==========================================================================\
// assegna al mouse un puntatore del set di puntatori corrente              |
// parametri:                                                               |
// LONG lid: id puntatore sistema                                           |
// valore restituito:                                                       |
// BOOL : TRUE/FALSE = successo/errore                                      |
//==========================================================================/

BOOL APIENTRY WsetSysMouPtr(LONG lid) {
   return WinSetPointer(HWND_DESKTOP,
                        WinQuerySysPointer(HWND_DESKTOP, lid, FALSE));
}


//==========================================================================\
// Importa nella MLE hmle il contenuto della stringa pszstr                 |
// (aggiungere parametri per punto inserzione)                              |
// parametri:                                                               |
// HWND hmle = handle finestra MLE                                          |
// PSZ pszstr = stringa da importare                                        |
// INT cb = quantit… caratteri da importare. Se 0 calcola la lunghezza della|
//          stringa ed importa l'intera stringa                             |
// valore restituito:                                                       |
// LONG numero caratteri importati nella MLE                                |
//==========================================================================/

BOOL APIENTRY Wstr2mle(HWND hmle, PSZ pszstr, INT cb) {
   PSZ psz = NULL;
   ULONG ul;
   IPT ipt = -1;
   BOOL rc = FALSE;

   if (!cb) cb = strlen(pszstr);
   // alloca buffer
   if (DosAllocMem((PPVOID)&psz, 0x9000,
                   PAG_READ | PAG_WRITE | PAG_COMMIT | OBJ_TILE))
      return FALSE;

   if (!WinSendMsg(hmle, MLM_SETIMPORTEXPORT, (MPARAM)psz, (MPARAM) 0x9000))
      goto end0;

   do {
      ul = cb > 0x8000? 0x8000: cb;
      memcpy(psz, pszstr, ul);
      WinSendMsg(hmle, MLM_IMPORT, (MPARAM)&ipt, (MPARAM)ul);
      cb -= 0x8000;
   } while (cb > 0); /* enddo */
   rc = TRUE;

end0:
   DosFreeMem(psz);
   return rc;
}


//==========================================================================-
//  PER FUNZIONARE CI VUOLE UNA CORRISPONDENTE API PER ALLOCARE GLI HANDLE! |
// se gli handle della listbox sono usati per memorizzare indirizzi di      |
// memoria allocata con malloc, libera tutta la memoria e vuota la lista    |
//==========================================================================-
//
//VOID APIENTRY WLboxFreeHandles(HWND hwnd) {
//   INT i;
//   PVOID pv;
//   for (i = 0;
//   0 != (pv = (PVOID)DlgLboxQueryItemHandle(hwnd, CLBX_EXCPROGS,i);
//              ++i)
//            free(pwe);
//      } /* endif */
//
//   for (; ; ) {
//   } /* endfor */
//
//
//
