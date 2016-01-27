//==========================================================================\
//                               more.h                                     |
//                                                                          |
// il controllo Š strutturato come segue:                                   |
//                                                                          |
// ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿ÚÄÄ¿ÚÄÄ¿                       |
// ³ 1                                      ³³2 ³³3 ³                       |
// ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙÀÄÄÙÀÄÄÙ                       |
//                                                                          |
// - il componente 1 puo' essere:                                           |
//   a. una semplice caption (testo statico)                                |
//   b. un entryfield                                                       |
//   c. una combobox                                                        |
//   d. una chechbox                                                        |
// - il componente 2 Š facoltativo ed Š costituito da un bottone che serve  |
//   per cambiare il testo e il valore di [1] tra 2 o piu' valori.          |
//   Il bottone Š rappresentato da una freccia (un triangolo) volto verso   |
//   il basso.                                                              |
//   Il bottone non pu• essere usato quando 1 Š un entryfield o una         |
//   combobox.                                                              |
// - il componente 3 Š sempre presente ed Š un bottone con una freccia      |
//   (un triangolo) volta a sinistra                                        |
//==========================================================================/

// stili compound list:
#define MORE_CAPTION     0x0000      // default: il componente 1 Š una
                                     // semplice caption
#define MORE_CHECK       0x0001      // il componente 1 Š una checkbox
#define MORE_ENTRY       0x0002      // il componente 1 Š un entryfield
#define MORE_SWAP        0x0100      // Š presente il bottone Swap


// flag notifica WM_CONTROL ------------------------------------------------
#define MOREN_CHECK      0x0001      // cambiamento stato bottone check
#define MOREN_EFCHANGE   0x0002      // cambiamento testo entry field
#define MOREN_EFSETFCS   0x0003      // entryfield: acquisizione focus
#define MOREN_EFLOSEFCS  0x0004      // entryfield: perdita focus
#define MOREN_EFSCROLL   0x0005      // entryfield: scroll
#define MOREN_EFMEMERR   0x0006      // entryfield: memory error
#define MOREN_EFOVFLW    0x0007      // entryfield: overflow
#define MOREN_EFINSTGL   0x0008      // entryfield: insert/overwrite
#define MOREN_SWAP       0x1000      // pressione bottone swap
#define MOREN_FIND       0x2000      // pressione bottone find
        
// id controlli interni
#define FID_CAPTION                  0xa000
#define FID_SWAP                     0xa001
#define FID_FIND                     0xa005

#pragma pack(2)
// ControlData compound listbox (valido solo con stile MORE_ENTRY)
typedef struct {
   USHORT cb;          // dimensione struttura (4 byte)
   USHORT cch;         // numero caratteri ammessi nell'entryfield
} MORECDATA, *PMORECDATA;
#pragma pack()


// flags stato controllo

typedef struct {
   UINT enbl   : 1;    // stato abilitazione controllo
   UINT chkon  : 1;    // stato checkbox (se presente)
   UINT        : 0;
} STATUS, * PSTATUS;


// dati usati internamente dal controllo

typedef struct {
   STATUS is;          // stato controllo
   HAB hab;            // hanchor block
   HWND hwnd;          // handle controllo
   HWND hcpt;          // handle controllo 1
   HWND hswap;         // handle bottone swapp se presente
   HWND hfind;         // handle bottone find
   PFNWP pfbtn;        // procedura componente bottone
   PFNWP pfcpt;        // procedura componente 1
   ULONG id;           // id controllo
   ULONG cchef;        // massimo numero caratteri entryfield
   SIZEL szl;          // dimensione controllo
   RECTL rclcpt;       // coordinate controll1
   ULONG csdbtn;       // dimensione lato bottoni
   POINTL ptlswap;     // coordinate bottone swap
   POINTL ptlfnd;      // coordintate bottone find
   ULONG cbtxt;        // lunghezza totale testo
   PSZ psz;            // testo controllo 1 (ignorato se stile
   ULONG fl;           // stile controllo (differente da quanto memorizzato
                       // nel QWL_STYLE del controllo originario)
   LONG lfgnd;         // foreground color
   LONG lbkgnd;        // background color
} MOREBOX, * PMOREBOX;

// funzioni controllo
// more.c
MRESULT EXPENTRY EDMoreWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
BOOL moreCreateProc(HWND hwnd, PMORECDATA pmrcd, PCREATESTRUCT pc, ULONG style);
VOID moreUpdateCtlSize(HWND hwnd, PSWP pswp);
MRESULT moreEnable(PMOREBOX pmore, BOOL fl);
MRESULT moreControl(PMOREBOX pmore, MPARAM mp1, PUSERBUTTON pub);
MRESULT moreCommand(PMOREBOX pmore, ULONG id);
MRESULT morePaint(PMOREBOX pmore);
MRESULT moreSetWparms(PMOREBOX pmore, PWNDPARAMS pwpm);
MRESULT moreQueryWparms(PMOREBOX pmore, PWNDPARAMS pwpm);
MRESULT moreSCchange(PMOREBOX pmore);
MRESULT morePPchange(PMOREBOX pmore, ULONG fl);
VOID moreDestroyProc(PMOREBOX pmore);
MRESULT moreMnemonic(PMOREBOX pmore, ULONG ch);
MRESULT moreMouBtn(PMOREBOX pmore);
MRESULT moreSetFocus(HWND hwnd, MPARAM mp1, BOOL fl);
MRESULT moreDelayFocus(PMOREBOX pmore);
MRESULT moreSetCheck(PMOREBOX pmore, BOOL fl);
MRESULT moreQueryCheck(PMOREBOX pmore);
MRESULT moreEFtxtlimit(PMOREBOX pmore, ULONG cb);
// moreaux.c
BOOL moreChilds(PMOREBOX pmore, PCREATESTRUCT pc);
VOID moreUpdColors(PMOREBOX pmore, HPS hps);
BOOL moreSetCtlData(PMOREBOX pmore, PMORECDATA pmrcd);
ULONG moreSetText(PMOREBOX pmore, PSZ psz, LONG cb);
VOID moreSize(PMOREBOX pmore);
BOOL moreMove(PMOREBOX pmore);
