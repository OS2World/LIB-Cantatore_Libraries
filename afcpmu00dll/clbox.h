//==========================================================================\
//                              clbox.h                                     |
//                                                                          |
// il controllo Š strutturato come segue:                                   |
//                                                                          |
// ÚÄ¿                                                                      |
// ÀÄÙ 1                                                                    |
// ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿                         |
// ³                                              ³                         |
// ³                                              ³                         |
// ³ 2                                            ³                         |
// ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ                         |
// ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿ÚÄÄ¿ÚÄÄ¿ÚÄÄ¿                         |
// ³ 3                                ³³4 ³³5 ³³6 ³                         |
// ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙÀÄÄÙÀÄÄÙÀÄÄÙ                         |
//                                                                          |
// il controllo Š composto da una listbox e dai controlli necessari per     |
// aggiungere, editare o rimuovere item alla listbox stessa                 |
// - il componente 1 puo' essere semplice testo di descrizione delle        |
//   funzioni della listbox, una checkbox che abiliti o disabiliti gli altri|
//   componenti, o pu• essere del tutto assente                             |
// - il componente 2 Š la listbox stessa. Questa puo' rispondere all'evento |
//   LN_ENTER rimuovendo l'item selezionato o riportandolo nell'entry 3 per |
//   l'editazione                                                           |
// - il componente 3 puo' essere un entryfield o una dropdown box           |
// - il componente 4 Š il bottone ADD                                       |
// - il componente 5 Š il bottone Remove                                    |
// - il componente 6 Š il bottone Find                                      |
//                                                                          |
//==========================================================================/

// stili compound list:

#define CLBXS_CHECK      0x0001      // il componente 1 Š una checkbox
#define CLBXS_NOCAPT     0x0002      // non viene lasciato spazio per il
                                     // controllo 1
#define CLBXS_DROPDOWN   0x0004      // il controllo 3 Š una dropdown box
#define CLBXS_HSCROLL    0x0008      // il controllo 2 ha una scrollbar
                                     // orizzontale
#define CLBXS_ENTRREM    0x0010      // l'item viene rimosso quando si fa
                                     // doppio click o si preme enter
#define CLBXS_ENTREDIT   0x0020      // il testo dell'item viene replicato
                                     // nell'entryfield quando si fa doppio
                                     // click o si preme enter
#define CLBXS_NODUP      0x0040      // prima dell'inserzione di un nuovo
                                     // item si controlla che esso non sia
                                     // gi… presente nella lista
#define CLBXS_MULTISEL   0x0080      // listbox 2 a selezione multipla
#define CLBXS_EXTENDSEL  0x0100      // listbox 2 a selezione estesa
#define CLBXS_OWNERDRAW  0x0200      // listbox ownerdraw
#define CLBXS_INSSTART   0x0000      // gli item inseriti quando si clicca
                                     // sul bottone add sono inseriti
                                     // all'inizio della lista
#define CLBXS_INSSORTUP  0x0400      // gli item inseriti quando si clicca
                                     // sul bottone add sono inseriti in
                                     // ordine alfabetico ascendente
#define CLBXS_INSSORTDN  0x0800      // gli item inseriti quando si clicca
                                     // sul bottone add sono inseriti in
                                     // ordine alfabetico discendente
#define CLBXS_INSEND     0x1000      // gli item inseriti quando si clicca
                                     // sul bottone add sono inseriti alla
                                     // fine
#define CLBXS_INSUPPER   0x2000      // converte in maiuscolo le stringhe
                                     // inserite 

// flag notifica WM_CONTROL ------------------------------------------------
#define CLBXN_CHECK      0x0001      // cambiamento stato checkbox
#define CLBXN_LNSELECT   0x0002      // listbox: selezione
// #define CLBXN_LNSETFCS   0x0003      // listbox: acquisizione focus
// #define CLBXN_LNLOSEFCS  0x0004      // listbox: perdita focus
// #define CLBXN_LNSCROLL   0x0005      // listbox: scroll
#define CLBXN_LNENTER    0x0006      // listbox: enter
// #define CLBXN_ENSETFCS   0x0007      // entryfield: acquisizione focus
// #define CLBXN_ENLOSEFCS  0x0008      // entryfield: perdita focus
#define CLBXN_ENCHANGE   0x0009      // entryfield: cambiamento testo
// #define CLBXN_ENEFSCROLL 0x0010      // entryfield: scroll
// #define CLBXN_ENOVERFLW  0x0011      // entryfield: overflow
// #define CLBXN_ENINSTGL   0x0012      // entryfield: tasto insert/overwrite
// #define CLBXN_ENSELECT   0x0013      // dropdownbox: selezione
// #define CLBXN_ENSCROLL   0x0014      // dropdownbox: enter
// #define CLBXN_ENSHOW     0x0015      // dropdownbox: show
#define CLBXN_ENENTER    0x0016      // dropdownbox: enter (in mp2 l'indice
                                     // dell'item selezionato)
#define CLBXN_ENMEMERR   0x0017      // dropdownbox/entryfield: memory error
#define CLBXN_ADD        0x0018      // pressione bottone add
#define CLBXN_REM        0x0019      // pressione bottone remove
#define CLBXN_FIND       0x0020      // pressione bottone find
#define CLBXN_ADDED      0x0021      // elaborazione evento add completata
#define CLBXN_REMOVED    0x0022      // elaborazione evento remove completata

// per la listbox valgono i messaggi LM_*
// #define LM_QUERYITEMCOUNT          0x0160
// #define LM_INSERTITEM              0x0161
// #define LM_SETTOPINDEX             0x0162
// #define LM_DELETEITEM              0x0163
// #define LM_SELECTITEM              0x0164
// #define LM_QUERYSELECTION          0x0165
// #define LM_SETITEMTEXT             0x0166
// #define LM_QUERYITEMTEXTLENGTH     0x0167
// #define LM_QUERYITEMTEXT           0x0168
// #define LM_SETITEMHANDLE           0x0169
// #define LM_QUERYITEMHANDLE         0x016a
// #define LM_SEARCHSTRING            0x016b
// #define LM_SETITEMHEIGHT           0x016c
// #define LM_QUERYTOPINDEX           0x016d
// #define LM_DELETEALL               0x016e
// #define LM_INSERTMULTITEMS         0x016f
// #define LM_SETITEMWIDTH            0x0660


// messaggi per interagire con l'eventuale componente listbox quando
// l'entryfield del controllo Š sostituito da una dropdown list

#define CCLM_QUERYITEMCOUNT          0x2160
#define CCLM_INSERTITEM              0x2161
#define CCLM_SETTOPINDEX             0x2162
#define CCLM_DELETEITEM              0x2163
#define CCLM_SELECTITEM              0x2164
#define CCLM_QUERYSELECTION          0x2165
#define CCLM_SETITEMTEXT             0x2166
#define CCLM_QUERYITEMTEXTLENGTH     0x2167
#define CCLM_QUERYITEMTEXT           0x2168
#define CCLM_SETITEMHANDLE           0x2169
#define CCLM_QUERYITEMHANDLE         0x216a
#define CCLM_SEARCHSTRING            0x216b
#define CCLM_SETITEMHEIGHT           0x216c
#define CCLM_QUERYTOPINDEX           0x216d
#define CCLM_DELETEALL               0x216e
#define CCLM_INSERTMULTITEMS         0x216f
#define CCLM_SETITEMWIDTH            0x2660
// setta il testo dell'entryfield con la stringa passata in mp1
#define CCLM_SETENTRYTEXT            0x2700

// id controlli interni
#define FID_CAPTION                  0xa000
#define FID_LIST                     0xa001
#define FID_ENTRY                    0xa002
#define FID_ADD                      0xa003
#define FID_REMOVE                   0xa004
#define FID_FIND                     0xa005

#pragma pack(2)
// ControlData compound listbox
typedef struct {
   USHORT cb;
   USHORT cch;
} CLBOXCDATA, *PCLBOXCDATA;
#pragma pack()


// flags stato controllo

typedef struct {
   INT item    : 16;
   UINT enbl   : 1;    // stato abilitazione controllo
   UINT chkon  : 1;    // stato checkbox (se presente)
   UINT remon  : 1;    // selezione item listbox per abilitazione btn remove
   UINT addon  : 1;    // presenza testo entry per abilitazione btn add
   UINT        : 0;
} STATUS, * PSTATUS;


// dati usati internamente dal controllo

typedef struct {
   STATUS is;          // stato controllo
   HAB hab;            // hanchor block handle
   HWND hwnd;          // handle controllo
   HWND hOwner;        // handle owner window
   HWND hchk;          // handle checkbox
   HWND hlbx;          // handle listbox child
   HWND hentry;        // handle entryfield/dropdown
   HWND hadd;          // handle bottone add
   HWND hrem;          // handle bottone remove
   HWND hfind;         // handle bottone find
   PFNWP pfbtn;        // procedura componente bottone
   PFNWP pflbx;        // procedura componente entry
   PFNWP pfef;         // procedura componente entry (entryfield/combobox)
   ULONG id;           // id controllo
   ULONG cchef;        // massimo numero caratteri entryfield
   SIZEL szl;          // dimensione controllo
   RECTL rclcpt;       // coordinate caption/checkbox
   RECTL rcllbx;       // coordinate listbox
   RECTL rclef;        // coordinate entryfield
   ULONG csdbtn;       // dimensione lato bottoni
   POINTL ptladd;      // coordinate bottone add
   POINTL ptlrem;      // coordinate bottone remove
   POINTL ptlfnd;      // coordintate bottone find
   ULONG cbtxt;        // lunghezza totale testo
   PSZ psz;            // testo controllo 1 (ignorato se stile
   ULONG cyFont;       // altezza font
   ULONG fl;           // stile controllo (differente da quanto memorizzato
                       // nel QWL_STYLE del controllo originario)
   LONG lfgnd;         // foreground color
   LONG lbkgnd;        // background color
} CLBOX, * PCLBOX;

// funzioni controllo
// clbx.c
MRESULT EXPENTRY EDClbWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
BOOL clbCreateProc(HWND hwnd, PCLBOXCDATA pclcd, PCREATESTRUCT pc, ULONG style);
VOID clbUpdateCtlSize(HWND hwnd, PSWP pswp);
MRESULT clbEnable(PCLBOX pclb, BOOL fl);
MRESULT clbControl(PCLBOX pclb, MPARAM mp1, PUSERBUTTON pub);
MRESULT clbCommand(PCLBOX pclb, ULONG id);
MRESULT clbPaint(PCLBOX pclb);
MRESULT clbSetWparms(PCLBOX pclb, PWNDPARAMS pwpm);
MRESULT clbQueryWparms(PCLBOX pclb, PWNDPARAMS pwpm);
MRESULT clbSCchange(PCLBOX pclb);
MRESULT clbPPchange(PCLBOX pclb, ULONG fl);
VOID clbDestroyProc(PCLBOX pclb);
MRESULT clbMnemonic(PCLBOX pclb, ULONG ch);
MRESULT clbMouBtn(PCLBOX pclb, PMSEMSG pmmsg);
MRESULT clbSetFocus(HWND hwnd, MPARAM mp1, BOOL fl);
MRESULT clbDelayFocus(PCLBOX pclb);
MRESULT clbSetCheck(PCLBOX pclb, BOOL fl);
MRESULT clbQueryCheck(PCLBOX pclb);
MRESULT clbSetEntryText(PCLBOX pclb, PSZ psz);
MRESULT clbEFtxtlimit(PCLBOX pclb, ULONG cb);
// clbxaux.c
VOID clbUpdColors(PCLBOX pclb, HPS hps);
BOOL clbSetCtlData(PCLBOX pclb, PCLBOXCDATA pclcd);
ULONG clbSetText(PCLBOX pclb, PSZ psz, LONG cb);
VOID clbSize(PCLBOX pclb);
BOOL clbMove(PCLBOX pclb);
VOID clbControlLboxMsg(PCLBOX pclb, ULONG nmsg);
VOID clbControlEntryMsg(PCLBOX pclb, ULONG nmsg);
ULONG clbEnableAdd(PCLBOX pclb);
// clbxsub.c
BOOL clbChilds(PCLBOX pclb, PCREATESTRUCT pc);
MRESULT EXPENTRY clblboxproc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY newcomboproc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
