//==========================================================================\
// util.c  (c) 1998-1999 * Alessandro Cantatore * Team OS/2 Italy           |
// - inizializzazione DLL
// - funzioni allocazione memoria
// - funzioni varie
//  |
// funzioni: -------------------------------------------------------------- |
//  |
// API: --------------------------------------------------------------------|
//  |
//==========================================================================/

#pragma strings(readonly)

#define INCL_WIN
#define INCL_DOSMISC
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES
#define INCL_DOSMODULEMGR
#define INCL_DOSNLS        /* National Language Support values */
#define INCL_DOSDATETIME   /* Date and time values */
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"


int _rmem_init(void);
int _rmem_term(void);

HMTX hmtx;            // semaforo mutex
HMODULE hmod;         // module handle


//==========================================================================\
// inizializzazione:                                                        |
// - memorizza handle modulo                                                |
// - inizializza funzioni allocazione memoria libreria subsistem            |
// - crea semaforo per accesso risorse condivise                            |
// terminazione:                                                            |
// - rilascia risorse funzioni allocazione memoria libreria subsistem       |
// - chiude semaforo                                                        |
//==========================================================================/

#pragma entry(AFCBSU00InitTerm)

ULONG _System AFCBSU00InitTerm(ULONG ul, ULONG flag) {
   switch (flag) {
      case 0:
         hmod = (HMODULE)ul;
         if (_rmem_init()) return 0L;
         if (DosCreateMutexSem(NULL, &hmtx, 0, 0)) return 0L;
         break;
      case 1:
         _rmem_term();
         DosCloseMutexSem(hmtx);
         break;
      default: return 0L;
   } /* endswitch */
   return 1L;
}


//==========================================================================\
// Subsystem Thread Safe malloc                                             |
//==========================================================================/

PVOID APIENTRY sts_malloc(ULONG cb) {
   PVOID pv;
   if (!cb) return NULL;
   if (WinRequestMutexSem(hmtx, SEM_INDEFINITE_WAIT)) return NULL;
   pv = malloc(cb);
   DosReleaseMutexSem(hmtx);
   return pv;
}


//==========================================================================\
// Subsystem Thread Safe free                                               |
//==========================================================================/

VOID APIENTRY sts_free(PVOID pv) {
   if (!pv) return;
   if (WinRequestMutexSem(hmtx, SEM_INDEFINITE_WAIT)) return;
   free(pv);
   DosReleaseMutexSem(hmtx);
}


//==========================================================================\
// Subsystem Thread Safe _heapmin                                           |
//==========================================================================/

INT APIENTRY sts_heapmin(VOID) {
   INT rc;
   if (WinRequestMutexSem(hmtx, SEM_INDEFINITE_WAIT)) return -1;
   rc = _heapmin();
   DosReleaseMutexSem(hmtx);
   return rc;
}


//-------------------------- utility varie ----------------------------------


//==========================================================================\
// ritorna il nome incluso il percorso dell'eseguibile del processo attivo  |
// parametri:                                                               |
// PSZ pszPath: buffer in cui sar… scritto il percorso dell'eseguibile      |
// ULONG cch: dimensione del buffer                                         |
// valore restituito:                                                       |
// BOOL: TRUE = successo, FALSE = errore                                    |
//==========================================================================/

BOOL APIENTRY uExeFullName(PSZ pszPath, ULONG cch) {
   PPIB ppib;
   PTIB ptib;
   if (DosGetInfoBlocks(&ptib, &ppib) ||
       DosQueryModuleName(ppib->pib_hmte, cch, pszPath))
      return FALSE;
   return TRUE;
}


//==========================================================================\
// ricava il nome del modulo che ha avviato il processo corrente e          |
// sostituisce l'estensione EXE con pszExt                                  |
// parametri:                                                               |
// PSZ pszPath: buffer in cui sar… scritto il percorso dell'eseguibile      |
// ULONG cch: dimensione del buffer                                         |
// PSZ pszExt: estensione da dare al file INI                               |
// valore restituito:                                                       |
// BOOL: TRUE = successo, FALSE = errore                                    |
//==========================================================================/

BOOL APIENTRY uEditExeExt(PSZ pszPath, ULONG cch, PSZ pszExt) {
   if (!uExeFullName(pszPath, cch)) return FALSE;
   strcpy(strrchr(pszPath, '.') + 1, pszExt);
   return TRUE;
}


//==========================================================================\
// scrive la data e l'ora correnti nel formato appropriato                  |
//==========================================================================/

BOOL APIENTRY uDateTime(PSZ pszdate, PSZ psztime) {
   COUNTRYCODE cc = {0};
   COUNTRYINFO ci = {0};
   DATETIME dt;
   ULONG ul = 0;
   // rileva informazioni relative formato dati secondo nazionalit…
   if (DosQueryCtryInfo(sizeof(COUNTRYINFO), &cc, &ci, &ul))
      return FALSE;
                    
   DosGetDateTime(&dt);
   pszdate[2] = pszdate[5] = *ci.szDateSeparator;
   psztime[2] = psztime[5] = *ci.szTimeSeparator;
   psztime[0] = '0' + dt.hours / 10;
   psztime[1] = '0' + dt.hours % 10;
   psztime[3] = '0' + dt.minutes / 10;
   psztime[4] = '0' + dt.minutes % 10;
   psztime[6] = '0' + dt.seconds / 10;
   psztime[7] = '0' + dt.seconds % 10;
   switch (ci.fsDateFmt) {
      case(1):                                         /* dd/mm/yy */
         pszdate[0] = '0' + dt.day / 10;
         pszdate[1] = '0' + dt.day % 10;
         pszdate[3] = '0' + dt.month;
         pszdate[4] = '0' + dt.month;
         pszdate[6] = '0' + (dt.year % 100) / 10;
         pszdate[7] = '0' + dt.year % 10;
         break;
      case(2):                                               /* yy/mm/dd */
         pszdate[0] = '0' + (dt.year % 100) / 10;
         pszdate[1] = '0' + dt.year % 10;
         pszdate[3] = '0' + dt.month / 10;
         pszdate[4] = '0' + dt.month % 10;
         pszdate[6] = '0' + dt.day / 10;
         pszdate[7] = '0' + dt.day % 10;
         break;
      default:                                               /* mm/dd/yy */
         pszdate[0] = '0' + dt.month / 10;
         pszdate[1] = '0' + dt.month % 10;
         pszdate[3] = '0' + dt.day / 10;
         pszdate[4] = '0' + dt.day % 10;
         pszdate[6] = '0' + (dt.year % 100) / 10;
         pszdate[7] = '0' + dt.year % 10;
         break;
   } /* endswitch */
   return TRUE;
}


//==========================================================================\
// restituisce il valore corrispondente ad un settaggio di sistema          |
// parametri:                                                               |
// LONG lID: id valore richiesto                                            |
// valore restituito:                                                       |
// LONG: valore restituito (-1 in caso di errore)                           |
//==========================================================================/

LONG uGetSval(LONG lID) {
   LONG l;
   if (lID > DOS_INFOSHIFT) {
      lID -= DOS_INFOSHIFT;
      if (DosQuerySysInfo(lID, lID, &l, 4)) return -1;
   } else {
      l = WinQuerySysValue(HWND_DESKTOP, lID);
   } /* endif */
   return l;
}
