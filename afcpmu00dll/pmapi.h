// pmapi.h

// strings

#define SZ_WARNING         "Warning!"
#define SZ_ERROR           "Error!"
#define SZ_NOTE            "Note!"
#define SZ_QUERY           "Please confirm!"


// flags usate da Wprint

#define PMPRNT_WININIT     0x0001    // richiede inizializzazione
#define PMPRNT_ERROR       0x0000    // Errore
#define PMPRNT_WARN        0x0002    // Attenzione
#define PMPRNT_INFO        0x0004    // Nota
#define PMPRNT_QUERY       0x0008    // richiesta conferma
#define PMPRNT_SYSMOD      0x0010    // system modal
// flags valide in modalit… PMPRNT_QUERY
#define PMPRNT_QWARNICO    0x0020
#define PMPRNT_QINFOICO    0x0040
#define PMPRNT_QERRORICO   0x0080
#define PMPRNT_QQUERYICO   0x0000
