//==========================================================================\
// strutil.c  (c) 1998-1999 * Alessandro Cantatore * Team OS/2 Italy        |
// sorgente DLL racchiudente funzioni utilit… x manipolazione di stringhe   |
//  |
// funzioni: -------------------------------------------------------------- |
// LONG APIENTRY linedel(PSZ pszline);                                      |
// PSZ APIENTRY wlin1str(PSZ psz, PSZ substr);                              |
// PSZ APIENTRY lin1str(PSZ psz, PSZ substr);                               |
// PSZ APIENTRY pszlastchr(PSZ psz, CHAR ch);                               |
// BOOL APIENTRY wpszwldcmp(char* mask, char* cmp);                         |
// BOOL APIENTRY pszwldcmp(char* mask, char* cmp);                          |
// char* APIENTRY pszcpy(char* target, char* source);                       |
// ULONG APIENTRY pszncpy(PSZ target, PSZ source, ULONG cch);               |
// int APIENTRY wpszicmp(const char* s1, const char* s2);                   |
// int APIENTRY pszicmp(const char* s1, const char* s2);                    |
// int APIENTRY wpsznicmp(char* s1, const char* s2, unsigned int n);        |
// int APIENTRY psznicmp(char* s1, const char* s2, unsigned int n);         |
// ULONG APIENTRY wstrdel(PSZ pszbuf, PSZ pszstr, ULONG cbbuf);             |
// ULONG APIENTRY strdel(PSZ pszbuf, PSZ pszstr, ULONG cbbuf);              |
// PSZ APIENTRY wstristr(PSZ str, PSZ substr, ULONG cb);                    |
// PSZ APIENTRY stristr(PSZ str, PSZ substr, ULONG cb);                     |
// LONG APIENTRY strrepl(PSZ ins, PSZ psznew, PSZ endch);                   |
// ULONG APIENTRY strins(PSZ psz, PSZ substr, PSZ ins, ULONG cb);           |
//                                                                          |
// API: --------------------------------------------------------------------|
//  |
//==========================================================================/

#pragma strings(readonly)

#define INCL_WIN
#define INCL_DOSMISC
#define INCL_DOSNLS
#define INCL_DOSSEMAPHORES
#define INCL_DOSPROCESS
#define INCL_DOSMODULEMGR
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "common.h"


//==========================================================================\
// cancella dalla posizione corrente a fine riga includendo anche i         |
// caratteri di terminazione riga, restituisce il numero dei caratteri      |
// cancellati (in negativo)                                                 |
// parametri: ------------------------------------------------------------- |
// PSZ pszline: indirizzo linea da cancellare                               |
// valore restituito: ----------------------------------------------------- |
// LONG : quantit… caratteri da sottrarre a vecchia lunghezza buffer        |
//==========================================================================/

LONG APIENTRY linedel(PSZ pszline) {
   PSZ pend = strpbrk(pszline, "\r\n\x1a");
   // cerca la fine della linea (l'inizio della riga successiva)
   // se non trova CR-LF o EOF allora punta alla fine del file
   if (!pend) {
      LONG cb = strlen(pszline);
      *pszline = 0;
      return -cb;
   } // end if
   // se pend punta a return cancella eventuale newline successivo
   pend += (memcmp(pend, "\r\n", 2)? 1: 2);
   // il "+ 1" Š allo scopo di copiare il carattere \0 di terminazione
   memmove((PVOID)pszline, (PVOID)pend, strlen(pend) + 1);
   return - (pend - pszline);
}


//==========================================================================\
// restituisce un puntatore alla prima linea che inizia con "substr"        |
// o NULL se non viene trovata alcuna occorrenza                            |
// (versione internazionale)                                                |
// parametri:                                                               |
// PSZ psz: indirizzo buffer testo                                          |
// PSZ substr: stringa da ricercare                                         |
// valore restituito:                                                       |
// PSZ : indirizzo linea trovata, NULL se nessuna occorrenza                |
//==========================================================================/

PSZ APIENTRY wlin1str(PSZ psz, PSZ substr) {
   ULONG cb = strlen(substr);
   while (*psz) {
      if (!wpsznicmp(psz, substr, cb)) return psz;       // compara
      if (!(psz = strchr(psz, '\n'))) return NULL; // avanza riga seguente
      psz++;
   } /* endwhile */
   return NULL;
}


//==========================================================================\
// restituisce un puntatore alla prima linea che inizia con "substr"        |
// o NULL se non viene trovata alcuna occorrenza                            |
// parametri:                                                               |
// PSZ psz: indirizzo buffer testo                                          |
// PSZ substr: stringa da ricercare                                         |
// valore restituito:                                                       |
// PSZ : indirizzo linea trovata, NULL se nessuna occorrenza                |
//==========================================================================/

PSZ APIENTRY lin1str(PSZ psz, PSZ substr) {
   ULONG cb = strlen(substr);
   while (*psz) {
      if (!psznicmp(psz, substr, cb)) return psz;       // compara
      if (!(psz = strchr(psz, '\n'))) return NULL; // avanza riga seguente
      psz++;
   } /* endwhile */
   return NULL;
}


//==========================================================================\
// trova nella stringa l'indirizzo dell'ultima occorrenza del carattere ch  |
// + 1 (il carattere successivo a ch)                                       |
// parametri:                                                               |
// PSZ psz: stringa                                                         |
// CHAR ch: carattere da ricercare                                          |
// valore restituito:                                                       |
// PSZ pszCh: carattere successivo all'ultima occorrenza di ch in psz       |
//            se ch non Š presente ritorna psz                              |
//==========================================================================/

PSZ APIENTRY pszlastchr(PSZ psz, CHAR ch) {
   PSZ pszLst = strrchr(psz, ch);
   return (pszLst? ++pszLst: psz);
}


//==========================================================================\
// compara due stringhe di cui la prima contiene caratteri jolly            |
// parametri:                                                               |
// char* mask: maschera contenente wildchar                                 |
// char* cmp: stringa da comparare con "mask"                               |
// valore restituito:                                                       |
// BOOL: TRUE se cmp risponde al criterio specificato in mask               |
//==========================================================================/

BOOL APIENTRY wpszwldcmp(char* mask, char* cmp) {
   char buf[260];
   DosEditName(1, cmp, mask, buf, 260);
   return WinCompareStrings(0, 0, 0, cmp, buf, 0) == WCS_EQ;
}


//==========================================================================\
// compara due stringhe di cui la prima contiene caratteri jolly            |
// parametri:                                                               |
// char* mask: maschera contenente wildchar                                 |
// char* cmp: stringa da comparare con "mask"                               |
// valore restituito:                                                       |
// BOOL: TRUE se cmp risponde al criterio specificato in mask               |
//==========================================================================/

BOOL APIENTRY pszwldcmp(char* mask, char* cmp) {
   while (*cmp) {
      switch (*mask++) {
         case '*':
            if (!*mask) return TRUE;
            while (*cmp && !pszwldcmp(mask, cmp)) ++cmp;
            break;
         case '?':
            if (!*mask) return TRUE;
            if (*cmp == '.') break;
            if (*cmp == '\\') return FALSE;
            ++cmp;
            break;
         default:
            if (lower(*(mask - 1)) == lower(*cmp)) {
               ++cmp;
               break;
            } else {
               return FALSE;
            } // endif
      } /* endswitch */
   } /* endwhile */
   return *mask? FALSE: TRUE;
}


//==========================================================================\
// copia source su target, ritorna un puntatore alla fine di target         |
//==========================================================================/

char* APIENTRY pszcpy(char* target, char* source) {
   strcpy(target, source);
   return strchr(target, 0);
}


//==========================================================================\
// copia n caratteri di source su target terminando con un carattere NULL   |
// target in n Š compreso lo 0 di teminazione                               |
// restituisce il numero di caratteri copiati (lunghezza target: lo 0       |
// di terminazione non Š incluso                                            |
// esempio:                                                                 |
// pszncpy(buf, "0123456789", 5);                                           |
// copia in buf: "0123" e restituisce 4                                     |
//==========================================================================/

ULONG APIENTRY pszncpy(PSZ target, PSZ source, ULONG cch) {
   strncpy(target, source, cch);
   *(target + cch) = 0;
   return strlen(target);
}


//==========================================================================\
// compara due stringhe in modo case insensitivo                            |
//==========================================================================/

int APIENTRY wpszicmp(const char* s1, const char* s2) {
   int ai[4] = {-2, 0, -1, +1};
   ULONG ul = WinCompareStrings(0, 0, 0, (PSZ)s1, (PSZ)s2, 0);
   return ai[ul];
}


//==========================================================================\
// compara due stringhe in modo case insensitivo                            |
//==========================================================================/

int APIENTRY pszicmp(const char* s1, const char* s2) {
   for (; (*s1 == *s2 ||
           ((*s1 < *s2) && 
            ((*s1 > '@' && *s1 < '[') && (*s1 + 32) == *s2)) ||
           ((*s1 > '`' && *s1 < '{') && (*s1 - 32) == *s2)) &&
           *s1;
           s1++, s2++);
   return *s1 - *s2;
}


//==========================================================================\
// compara due stringhe in modo case insensitivo per n caratteri            |
//==========================================================================/

int APIENTRY wpsznicmp(const char* s1, const char* s2, unsigned int n) {
   PSZ ss1, ss2;
   ULONG ul = 0;
   COUNTRYCODE cc = {0, 0};
   int ret;
   if (!(ss1 = sts_malloc(2 * (n + 1)))) return -256;
   ss2 = ss1 + n + 1;
   memcpy(ss1, s1, n);
   memcpy(ss2, s2, n);
   *(ss1 + n) = *(ss2 + n) = 0;
   if (DosMapCase(n, &cc, ss1) || DosMapCase(n, &cc, ss2)) return -256;
   ret = memcmp(ss1, ss2, n);
   sts_free(ss1);
   return ret;
}


//==========================================================================\
// compara due stringhe in modo case insensitivo per n caratteri            |
//==========================================================================/

int APIENTRY psznicmp(char* s1, const char* s2, unsigned int n) {
  int c = 0;
  for (;(*s1 == *s2 ||
       !(c = ((*s1 > '@' && *s1 < '[')? *s1 + 32: *s1) -
        ((*s2 > '@' && *s2 < '[')? *s2 + 32: *s2))) && *s1 && --n;
        ++s1, ++s2);
  return c;
}


//==========================================================================\
// cancella una substringa (pszstr) contenuta in una stringa (pszbuf)       |
// restituisce la nuova lunghezza di pszbuf (cbbuf Š la lunghezza originaria)
//==========================================================================/

ULONG APIENTRY wstrdel(PSZ pszbuf, PSZ pszstr, ULONG cbbuf) {
   PSZ pcur;
   ULONG ul = strlen(pszstr);
   // se non trova la stringa restituisce dimensione immutata
   if (!(pcur = wstristr(pszbuf, pszstr, ul))) return cbbuf;
   memmove((PVOID)pcur, (PVOID)(pcur + ul), pszbuf + cbbuf + 1 - pcur - ul);
   return cbbuf - ul;
}


//==========================================================================\
// cancella una substringa (pszstr) contenuta in una stringa (pszbuf)       |
// restituisce la nuova lunghezza di pszbuf (cbbuf Š la lunghezza originaria)
//==========================================================================/

ULONG APIENTRY strdel(PSZ pszbuf, PSZ pszstr, ULONG cbbuf) {
   PSZ pcur;
   ULONG ul = strlen(pszstr);
   // se non trova la stringa restituisce dimensione immutata
   if (!(pcur = stristr(pszbuf, pszstr, ul))) return cbbuf;
   memmove((PVOID)pcur, (PVOID)(pcur + ul), pszbuf + cbbuf + 1 - pcur - ul);
   return cbbuf - ul;
}


//==========================================================================\
// trova la substringa substr in str in modo case insensitivo               |
// rtestituisce il puntatore alla substringa in str o NULL se la            |
// substringa non Š presente                                                |
//==========================================================================/


PSZ APIENTRY wstristr(PSZ str, PSZ substr, ULONG cb) {
   while (*str) {
      if (!wpsznicmp(str, substr, cb)) return str;
      ++str;
   } /* endwhile */
   return NULL;
}


//==========================================================================\
// trova la substringa substr in str in modo case insensitivo               |
// rtestituisce il puntatore alla substringa in str o NULL se la            |
// substringa non Š presente                                                |
//==========================================================================/


PSZ APIENTRY stristr(PSZ str, PSZ substr, ULONG cb) {
   while (*str) {
      if (!psznicmp(str, substr, cb)) return str;
      ++str;
   } /* endwhile */
   return NULL;
}


//==========================================================================\
// sostituisce la substringa "psznew" in "psz" alla precedente stringa      |
// delimitata da endch restituisce differenza lunghezza tra nuova stringa   |
// e vecchia stringa                                                        |
// parametri: ------------------------------------------------------------- |
// PSZ ins: punto inserzione nuova stringa (inizio vecchia stringa)         |
// PSZ psznew: nuova stringa da inserire. Se NULL la vecchia stringa viene  |
//             semplicemente cancellata                                     |
// PSZ endch: caratteri che delimitano il riconoscimento della vecchia      |
//            stringa (da ins alla prima occorrenza di un carattere incluso |
//            in endch                                                      |
// valore restituito: ----------------------------------------------------- |
// LONG : differenza lunghezza tra vecchia stringa e nuova                  |
//==========================================================================/

LONG APIENTRY strrepl(PSZ ins, PSZ psznew, PSZ endch) {
   PSZ pend = strpbrk(ins, endch);
   ULONG cbnew = (psznew? strlen(psznew): 0);
   // se carattere endch non presente considera \0 come delimitatore
   if (!pend) {                         // vecchia stringa
      if (cbnew) strcpy(ins, psznew);
      else *(ins + 1) = 0;
      return cbnew - strlen(ins);
   } else {
      if ((pend - ins) != cbnew)        // se le stringhe non sono lunghe =
      // sposta a fine inserzione nuova stringa quanto segue vecchia stringa
         memmove((PVOID)(ins + cbnew), (PVOID)pend, strlen(pend) + 1);
      if (cbnew) memmove((PVOID)ins, (PVOID)psznew, cbnew);
   } // end if
   return (ULONG)(cbnew - (pend - ins));
}


//==========================================================================\
// inserisce una stringa in un altra restituendo la nuova lunghezza della   |
// stringa                                                                  |
// parametri:                                                               |
// PSZ psz   : stringa in cui si deve effettuare inserzione                 |
// PSZ substr: substringa da inserire                                       |
// PSZ ins   : indirizzo punto di inserzione                                |
// ULONG cb  : dimensione corrente di psz                                   |
// valore restituito:                                                       |
// ULONG: nuova lunghezza di psz                                            |
//==========================================================================/

ULONG APIENTRY strins(PSZ psz, PSZ substr, PSZ ins, ULONG cb) {
   ULONG cbss = strlen(substr);
   memmove((PVOID)(ins + cbss), (PVOID)ins, psz + cb - ins);
   memmove((PVOID)ins, (PVOID)substr, cbss);
   return cb + cbss;
}


//==========================================================================\
//  |
// parametri:                                                               |
// HWND hwnd: handle dialogo
// PAPPDATA pad: indirizzo struttura dati applicazione
//  |
// valore restituito:                                                       |
// VOID:
// BOOL:
// ULONG:
//==========================================================================/

