// common.h

#define DOS_INFOSHIFT   0x100

#define ALLFILES                    FILE_ARCHIVED | FILE_DIRECTORY |\
                                    FILE_SYSTEM | FILE_HIDDEN |\
                                    FILE_READONLY 

#define lower(ch)     (((ch) > '@' && (ch) < '[')? ((ch) + 32) : (ch))

// struttura usata da ProcessTree

//==========================================================================\
// Processa (opzionalmente in modo ricorsivo) il contenuto di una directory.|
// La funzione usa la struttura PROCESSTREEDATA:                            |
// typedef struct _PROCESSTREEDATA {                                        |
//    FILEFINDBUF3 fBuf;       // dati file trovato                         |
//    CHAR achPath[260];       // file specifiche file da ricercare         |
//    CHAR achMask[80];        // maschera di ricerca                       |
//    ULONG flfind;            // attributi file da ricercare               |
//    PVOID pdata;             // user data                                 |
//    LONG (* pffile)(struct _PROCESSTREEDATA * pptd);    // funzione       |
//    LONG (* pfdirpre)(struct _PROCESSTREEDATA * pptd);  // funzione       |
//    LONG (* pfdirpost)(struct _PROCESSTREEDATA * pptd); // funzione       |
// } PROCESSTREEDATA, * PPROCESSTREEDATA;                                   |
// Quando la funzione viene chiamata in modo ricorsivo Š necessario         |
// specificare la maschera di ricerca in achMask, mentre achPath deve       |
// contenere il path completo di partenza seguito da "\\*".                 |
// In tal caso, per ogni directory trovata, viene modificato achPath        |
// aggiungendo il nome della directory seguito da "\\*", viene eseguita     |
// la funzione pfdirpre() (se non NULL), viene richiamata ricorsivamente    |
// ioProcessTree() e infine viene richiamata la funzione pfdirpost()        |
// (se non NULL), mentre per ogni file trovato, se questo soddisfa la       |
// maschera di ricerca achMask, viene richiamata la funzione pffile.        |
// Nel modo non ricorsivo per ogni file e directory soddisfacente la        |
// maschera achMask viene richiamato pffile()                               |
// Tutte le funzioni (pffile, pfdirpre, pfdirpost) devono restituire        |
// il numero di file processati (>= 0) oppure -1 in caso di errore.         |
// valore restituito:                                                       |
// LONG: >= 0 indica numero file trovati, < 0 indica errore                 |
//==========================================================================/

typedef struct _PROCESSTREEDATA {
   FILEFINDBUF3 fBuf;       // dati file trovato
   CHAR achPath[260];       // file specifiche file da ricercare
   CHAR achMask[80];        // maschera di ricerca
   ULONG flfind;            // flag ricerca
   PVOID pdata;             // user data
   LONG (* pffile)(struct _PROCESSTREEDATA * pptd);
   LONG (* pfdirpre)(struct _PROCESSTREEDATA * pptd);
   LONG (* pfdirpost)(struct _PROCESSTREEDATA * pptd);
} PROCESSTREEDATA, * PPROCESSTREEDATA;

extern HMTX hmtx;            // semaforo mutex
extern HMODULE hmod;         // module handle


// various:
PVOID APIENTRY sts_malloc(ULONG cb);
VOID APIENTRY sts_free(PVOID pv);
INT APIENTRY sts_heapmin(VOID);
BOOL APIENTRY uExeFullName(PSZ pszPath, ULONG cch);
BOOL APIENTRY uEditExeExt(PSZ pszPath, ULONG cch, PSZ pszExt);
BOOL APIENTRY uDateTime(PSZ pszdate, PSZ psztime);
LONG uGetSval(LONG lID);

// string utiliaties
LONG APIENTRY linedel(PSZ pszline);
PSZ APIENTRY wlin1str(PSZ psz, PSZ substr);
PSZ APIENTRY lin1str(PSZ psz, PSZ substr);
PSZ APIENTRY pszlastchr(PSZ psz, CHAR ch);
BOOL APIENTRY wpszwldcmp(char* mask, char* cmp);
BOOL APIENTRY pszwldcmp(char* mask, char* cmp);
char* APIENTRY pszcpy(char* target, char* source);
ULONG APIENTRY pszncpy(PSZ target, PSZ source, ULONG cch);
int APIENTRY wpszicmp(const char* s1, const char* s2);
int APIENTRY pszicmp(const char* s1, const char* s2);
int APIENTRY wpsznicmp(const char* s1, const char* s2, unsigned int n);
int APIENTRY psznicmp(char* s1, const char* s2, unsigned int n);
ULONG APIENTRY wstrdel(PSZ pszbuf, PSZ pszstr, ULONG cbbuf);
ULONG APIENTRY strdel(PSZ pszbuf, PSZ pszstr, ULONG cbbuf);
PSZ APIENTRY wstristr(PSZ str, PSZ substr, ULONG cb);
PSZ APIENTRY stristr(PSZ str, PSZ substr, ULONG cb);
LONG APIENTRY strrepl(PSZ ins, PSZ psznew, PSZ endch);
ULONG APIENTRY strins(PSZ psz, PSZ substr, PSZ ins, ULONG cb);

// io utilities
#define ioFNameFromPath(psz)            pszlastchr((psz), '\\')
BOOL APIENTRY ioBackup(PSZ pszFile, PSZ pszExt);
PSZ APIENTRY ioEditExt(PSZ pszFile, PSZ pszExt);
PSZ APIENTRY ioF2psz(PSZ pszFile, PULONG pcb);
BOOL APIENTRY ioFExists(PSZ pszFile, PULONG pul);
BOOL APIENTRY ioFilOlder(PFDATE pfds, PFTIME pfts, PFDATE pfdt, PFTIME pftt);
BOOL APIENTRY ioMakePath(PSZ pszPath);
LONG APIENTRY ioProcessTree(PPROCESSTREEDATA pptd);
ULONG APIENTRY ioPsz2f(PSZ pszFile, PSZ pdata, ULONG cb);
PSZ APIENTRY ioUniqFName(PSZ pszFile);

