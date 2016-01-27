//========================================================================\
// afcpmu00.h: header per l'omonima libreria di controlli e funzioni di   |
//             utilit… PM                                                 |
// API:
// BOOL APIENTRY EDElbReg(HAB hab);
// BOOL APIENTRY EDImgReg(HAB hab);
// BOOL APIENTRY EDBarReg(HAB hab);
// BOOL APIENTRY EDGrpReg(HAB hab);
// BOOL APIENTRY EDStbReg(HAB hab);
// BOOL APIENTRY EDClbReg(HAB hab);
// BOOL APIENTRY EDMoreReg(HAB hab);
// BOOL APIENTRY WprogressBar(HWND hParent, HWND hOwner, PPROGRESSDLG ppd);
// LONG APIENTRY WeditColorDlg(HWND hParent, HWND hOwner, PCLRWHEEL pcw);
// BOOL APIENTRY WfontDlg(HWND hwnd, PSZ pszTitle, PSZ pszSample, PSZ pszFont,
//                        ULONG cbFont, PULONG pfl);
// #define WlboxDeselectAll(hwnd)\
//    ((BOOL)WinSendMsg(hwnd, LM_SELECTITEM, (MPARAM)LIT_NONE, MPVOID))
// BOOL APIENTRY InitControls(HAB hab);
// BOOL APIENTRY WcenterInWindow(HWND hwnd, HWND hin);
// BOOL APIENTRY WcenterWindow(HWND hwnd);
// MRESULT APIENTRY WdlgMinMax(HWND hwnd, MPARAM mp1, PULONG pul);
// BOOL APIENTRY WdrawDisabledRect(HPS hps, PRECTL prcl, LONG color);
// LONG APIENTRY WdrawDisabledText(HPS hps, LONG cch, PSZ psz, PRECTL prcl,
//                            LONG clrNW, LONG clrSE, LONG clrbkgnd, ULONG fl);
// BOOL APIENTRY Wfil2mle(HWND hmle, PSZ pszfile);
// BOOL APIENTRY WfillListBox(HWND hwnd, ULONG ulId, PSZ pszList);
// HBITMAP APIENTRY WloadBitmap(HWND hwnd, HMODULE hmod, ULONG id);
// BOOL APIENTRY Wmle2fil(HWND hmle, PSZ pszfile);
// ULONG APIENTRY Wmle2str(HWND hmle, PSZ pszstr);
// VOID APIENTRY WmoveRect(PRECTL prcl, LONG x, LONG y);
// BOOL APIENTRY WopenObject(PSZ pszObj, ULONG flmode, ULONG flOpt);
// BOOL APIENTRY Wprint(HWND hwnd, PSZ pszMsg, ULONG fl);
// BOOL APIENTRY WqueryBitmapSize(HBITMAP hbmp, PSIZEL pszl);
// HWND APIENTRY WqueryHwndFrame(HWND hwnd);
// BOOL APIENTRY WsetButtonBmp(HWND hwnd, ULONG ulId, HBITMAP hbmp);
// BOOL APIENTRY WsetDefBtn(HWND hwnd, ULONG id);
// BOOL APIENTRY WsetDlgFonts(HWND hwnd, HWND htbar, PSZ pszFont);
// HPOINTER APIENTRY WsetDlgIcon(HWND hwnd, HMODULE hmod, ULONG id);
// BOOL APIENTRY WsetRelativePos(HWND hwnd, PSWP pswp);
// BOOL APIENTRY WsetSysMouPtr(LONG lid);
// BOOL APIENTRY WdrawBtnBkgnd(PUSERBUTTON pub, ULONG cx, ULONG cy, LONG color);
// LONG APIENTRY WgetCtlClr(HWND hwnd, HPS hps, ULONG ulid1, ULONG ulid2, LONG ldef);
// BOOL APIENTRY WprocSpin(ULONG ulEvent, HWND hspin, PLONG pl, LONG lcur);
// BOOL APIENTRY Wstr2mle(HWND hmle, PSZ pszstr, INT cb);
//========================================================================/

#ifndef AFCPMU00_H_
   #define AFCPMU00_H_

//========================================================================\
// controllo enhanced listbox
//========================================================================/

#define WC_EXTLISTBOX   "000afcccEnhListBox"

BOOL APIENTRY EDElbReg(HAB hab);

// flag notifica WM_CONTROL ------------------------------------------------
// indica che si Š premuto il tasto 1 del mouse su un item della listbox
// nel primo short di mp2 e' indicato l'indice dell'item, mentre nel secondo
// l'indice della colonna
#define ELBXN_SELECT            0x0001
// notifica l'owner che un item Š in procinto di essere mosso, se l'owner
// restituisce TRUE l'operazione di drag viene cancellata e viene emesso
// un segnale acustico
#define ELBXN_DRAGITEM          0x0002
// quando l'evento di drag avviene all'interno della listbox notifica
// l'owner. Se l'owner restituisce TRUE viene usato SPTR_ILLEGAL come
// puntatore del mouse
#define ELBXN_DRAGOVER          0x0003
// notifica l'avvenuto spostamento di un item. Nel primo short di mp2 Š
// indicata la precedente posizione dell'item, nel secondo short la nuova
// se l'owner restituisce 0 l'item viene spostato nella nuova posizione
// se l'owner restituisce -1 l'operazione di spostamento viene cancellata
// se l'owner restituisce 1 significa che esso si occupa dello spostamento
// dell'item e nella dll si ha il solo resettaggio delle flag e puntatori
#define ELBXN_DROPITEM          0x0004
// indica che il mouse Š stato premuto all'interno di un'area nella parte
// sinistra di una colonna larga quanto l'altezza dell'item stesso che
// si suppone sia usata per disegnare una checkbox. Il contenuto e
// significato dei parametri Š identico a ELBXN_SELECT
#define ELBXN_CHECKED           0x0005
// indica che il cursore Š cambiato perchŠ si Š premuto uno dei tasti
// freccia, pagina, home, end. In mp2 c'Š l'indice dell'item su cui Š
// il cursore
// Per spostare il cursore su un dato item usare il msg ELBXM_SETCURSOR
#define ELBXN_CURSORMOVE        0x0006


// messaggi controllo ------------------------------------------------------
#define ELBXM_QUERYCOLCOUNT     WM_USER + 0xff0 // richiesta numero colonne
#define ELBXM_QUERYCOLWIDTH     WM_USER + 0xff1 // mp1 contiene l'indice della colonna di
                                 // cui si richiede la larghezza in pixel
#define ELBXM_QUERYMULTICOLWIDTH WM_USER + 0xff2// mp1 contiene l'indirizzo dell'array di
                                 // strutture COLCOORD in cui copiare le
                                 // coordinate delle colonne, mp2 contiene
                                 // il numero di elementi dell'array
                                 // restituisce TRUE/FALSE in caso di
                                 // successo/errore

// setta contemporaneamente un item come top index e sposta il cursore su esso
// in mp1 deve essere l'indice dell'item che si vuole al top, mp2 = MPVOID
#define ELBXM_SETTOPINDEX          WM_USER + 0xff3
// come sopra sposta solo il cursore sull'item mp1
#define ELBXM_SETCURSOR          WM_USER + 0xff4
// forza il repaint di un particolare item passato in mp1. Se mp1 == 0xffff
// forza repaint intero componente listbox
#define ELBXM_REDRAWITEM         WM_USER + 0xff5


// stili complessivi controllo:
// indica che l'ordine degli item pu• essere cambiato tramite il drag & drop
#define ELBCS_ITEMDRAGGABLE    0x0001
// indica che il quadrato alla sinistra di una colonna deve essere considerato
// come una checkbox ed in quanto tale notifica l'owner con ELBXN_CHECKED
// piuttosto che con ELBXN_SELECT. Il contenuto dei parametri Š lo stesso
#define ELBCS_ITEMCHECKBOXED   0x0002

// id controlli interni (ridefinito successivamente)
// #define FID_LIST                     0xa001
// timer usato per l'autoscrolling della listbox
#define DRAG_TIMERID     33

#pragma pack(2)

// CONTROLDATA extended listbox (struttura dimensione variabile)

typedef struct {
   USHORT cb;          // dimensione totale struttura
   USHORT ccol;        // numero colonne
   USHORT crow;        // numero righe testo header
   USHORT acx[1];      // larghezze colonne
} ELBOXCDATA, * PELBOXCDATA;

// struttura coordinate colonne
typedef struct {
   USHORT x0;
   USHORT x1;
} COLCOORD, *PCOLCOORD;

#pragma pack()


//========================================================================\
// controllo immagine statica
//========================================================================/

#define WC_EDLGSIMG     "000afcccDlgStaticImage"

BOOL APIENTRY EDImgReg(HAB hab);

// il testo di un controllo statico dovrebbe essere definito come:
// "<nomemodulo>\t<idres>[-idres]"
// il testo di un bottone dovebbe essere:
// "<nomemodulo>\t<idres>[-idres][\tTestoBottone]"
// idres deve essere specificato come decimale!!!

#define ESI_BMP        0x0
#define ESI_PTR        0x00000100
#define ESI_ICON       0x00000200
#define ESI_MINI       0x00000400
//#define ESI_AUTOSIZE   SS_AUTOSIZE


//========================================================================\
// controlli statusbar, groupbox e barra orizzontale/verticale            |
//========================================================================/

#define WC_STATUSBAR    "000afcccDlgStatusBar"
#define WC_3DGROUPBOX   "000afcccGroupBox"
#define WC_BAR          "000afcccBar"

BOOL APIENTRY EDBarReg(HAB hab);
BOOL APIENTRY EDGrpReg(HAB hab);
BOOL APIENTRY EDStbReg(HAB hab);

// stili comuni a tutti i 3 controlli:

// 3D look style:----------------------------------------------------------
#define CCTL_INDENTED          0x00   // DEFAULT
#define CCTL_RAISED            0x40
// frame height/depth: (common flags) -------------------------------------
#define CCTL_NOBORDER           0x0   // flat (DEFAULT)
#define CCTL_BORDER1            0x1   // 1 pixel high/deep
#define CCTL_BORDER2            0x2   // 2 pixels high/deep
#define CCTL_BORDER3            0x3   // 3 pixels high/deep
#define CCTL_BORDER4            0x4   // 4 pixels high/deep
#define CCTL_BORDER5            0x5   // 5 pixels high/deep
#define CCTL_BORDER6            0x6   // 6 pixels high/deep
#define CCTL_BORDER7            0x7   // 7 pixels high/deep

// stili comuni a groupbox e statusbar

// bordered frame:---------------------------------------------------------
#define CCTL_NOFRAME           0x00   // no frame (DEFAULT)
#define CCTL_FRAME0            0x08   // no border
#define CCTL_FRAME1            0x10   // border width = border thickness
#define CCTL_FRAME2            0x18   // border width = 2 * border thickness
#define CCTL_FRAME3            0x20   // border width = 3 * border thickness
#define CCTL_FRAME4            0x28   // border width = 4 * border thickness
#define CCTL_FRAME5            0x30   // border width = 5 * border thickness
#define CCTL_FRAME6            0x38   // border width = 6 * border thickness

// text styles defined in PMWIN.H:-----------------------------------------
//   DT_LEFT                    0x00000000
//   DT_CENTER                  0x00000100
//   DT_RIGHT                   0x00000200
//   DT_TOP                     0x00000000
//   DT_VCENTER                 0x00000400  // invalid with STBAR_GROUP
//   DT_BOTTOM                  0x00000800  // invalid with STBAR_GROUP
//   DT_MNEMONIC                0x00002000
//   DT_WORDBREAK               0x00004000  // invalid with STBAR_GROUP

// sfondo opaco/trasparente
//#define CCTL_OPAQUEBKGND       0x00000000   // deafult: sfondo col background
//#define CCTL_TRANSPARENT       0x00008000   // sfondo trasparente


//==========================================================================\
// controllo compound listbox                                               |
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

#define WC_COMPLISTBOX  "000afcccCmpListBox"

// stili compound list:

#define CLBXS_CHECK      0x0001      // il componente 1 Š una checkbox
#define CLBXS_NOCAPT     0x0002      // non viene lasciato spazio per il
                                     // controllo 1 (no caption)
#define CLBXS_DROPDOWN   0x0004      // il controllo 3 Š una dropdown box
#define CLBXS_HSCROLL    0x0008      // il controllo 2 ha una scrollbar
                                     // orizzontale
#define CLBXS_ENTRREM    0x0010      // l'item viene rimosso quando si fa
                                     // doppio click o si preme enter
                                     // nella listbox
#define CLBXS_ENTREDIT   0x0020      // il testo dell'item viene replicato
                                     // nell'entryfield quando si fa doppio
                                     // click o si preme enter nella listbox
#define CLBXS_NODUP      0x0040      // prima dell'inserzione di un nuovo
                                     // item si controlla che esso non sia
                                     // gi… presente nella lista
#define CLBXS_MULTISEL   0x0080      // listbox 2 a selezione multipla
#define CLBXS_EXTENDSEL  0x0100      // listbox 2 a selezione estesa
#define CLBXS_OWNERDRAW  0x0200      // listbox ownerdraw
#define CLBXS_INSSTART   0x0000      // gli item inseriti quando si clicca
                                     // sul bottone add sono inseriti
                                     // all'inizio della lista (default)
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
// mp2, quando non altrimenti specificato, contiene l'handle del controllo
#define CLBXN_CHECK      0x0001      // cambiamento stato checkbox
                                     // mp2 contiene lo stato di check
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
                                     // mp2 contiene il testo che si sta per
                                     // aggiungere alla lista
#define CLBXN_REM        0x0019      // pressione bottone remove
                                     // mp2 contiene l'indice dell'item che
                                     // si sta per rimuovere dalla lista
#define CLBXN_FIND       0x0020      // pressione bottone find
#define CLBXN_ADDED      0x0021      // elaborazione evento add completata
                                     // mp2 Š l'indice dell'item aggiunto
//#define CLBXN_REMOVED    0x0022      // elaborazione evento remove completata

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


// API registrazione controllo
BOOL APIENTRY EDClbReg(HAB hab);


//==========================================================================\
// controllo More box                                                       |
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
//                                                                          |
//==========================================================================/

#define WC_MOREBOX      "000afcccMoreBox"

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

// API registrazione controllo
BOOL APIENTRY EDMoreReg(HAB hab);


//==========================================================================\
// progress dialog                                                          |
//==========================================================================/

// struttura dialogo progresso

typedef struct {
   HWND hNotify;      // handle finestra con cui vengono comunicati msg
   HBITMAP hbmp;      // handle bitmap usata nella barra di progresso
   LONG color;        // colore da usare nella barra di progresso
   PSZ pszTitle;      // titolo finestra di progresso
   PSZ pszPrgrss;     // caption % progresso
   PSZ pszTime;       // caption elapsed time
   PSZ pszStop;       // caption bottone stop
   PSZ FontNameSize;  // dialog font (something like "9.WarpSans")
   HWND hPos;         // ptl member or PRGSS_CENTER is relative to this window
   POINTL ptl;        // dialog position
   ULONG fl;          // stile dialogo secondo le seguenti flag:
} PROGRESSDLG, * PPROGRESSDLG;

// progress dialog style: (* = current styles)
#define PRGSS_SIMPLE    0x0000   // *only one progress bar
//#define PRGSS_DOUBLE    0x0001   // partial/total (double) progressbar
#define PRGSS_COLOR     0x0000   // *solid color progressbar
#define PRGSS_BITMAP    0x0002   // *bitmap progressbar
//#define PRGSS_BTNSTOP   0x0010   // *add a Stop button
//#define PRGSS_DETAIL    0x0020   // add a statusbar to show current operation
//#define PRGSS_PERCENT   0x0040   // *show progress (percent) in text
//#define PRGSS_TIME      0x0080   // *show elapsed time
#define PRGSS_DLG       0x0000   // *standalone dialog window
//#define PRGSS_CTL       0x1000   // control window
#define PRGSS_CENTER    0x2000   // centra il dialogo nella parent window

BOOL APIENTRY WprogressBar(HWND hParent, HWND hOwner, PPROGRESSDLG ppd);

// messaggi spediti dalla notify window
#define PRGSM_INCREMENT        WM_USER + 0x1000
#define PRGSM_MSG              PRGSM_INCREMENT + 1
#define PRGSM_END              PRGSM_INCREMENT + 3

// messaggi spediti dal dialogo di progresso
#define WM_PRGRSREADY          PRGSM_INCREMENT
#define WM_PRGRSQUIT           PRGSM_INCREMENT + 2


//========================================================================\
// colorwheel dialog                                                      |
//========================================================================/

// struttura dati controllo selezione colore

typedef struct {
   COLOR clr;         // initial color value
   PSZ pszTitle;      // dialog title
   PSZ pszOK;         // buttons caption
   PSZ pszUndo;
   PSZ pszCancel;
   PSZ pszRed;
   PSZ pszGreen;
   PSZ pszBlue;
   PSZ FontNameSize;  // dialog font (something like "9.WarpSans")
   HWND hPos;         // ptl member or CWDS_CENTER is relative to this window
   POINTL ptl;        // dialog position
   ULONG fl;
} CLRWHEEL, * PCLRWHEEL;

LONG APIENTRY WeditColorDlg(HWND hParent, HWND hOwner, PCLRWHEEL pcw);

// warp 4 messages
#define WM_COLORWHEELCHANGED        0x0601
#define CWM_COLORWHEELSETVAL        0x0602

// warp 3 messages
#define WM_COLORWHEELCHANGED3       0x130C
#define CWM_COLORWHEELSETVAL3       0x1384

//==========================================================================\
// font dialog:                                                             |
// alloca memoria per le strutture necessarie per il dialogo di selezione   |
// dei font, le inizializza, mostra il dialogo e copia in pszFont il nuovo  |
// font selezionato                                                         |
// parametri:                                                               |
// HWND hwnd: (in)handle della finestra di cui si vuole cambiare il font    |
// PSZ pszTitle: titolo della finestra di dialogo                           |
// PSZ pszSample: testo da mostrare come sample del font selezionato        |
// PSZ pszFont: (output)dimensione-nome-attributi nuovo font selezionato    |
// ULONG cbFont: dimensioni buffer pszFont                                  |
// PULONG pfl: in/out.                                                      |
//             in: se TRUE nasconde le checkbox display e printer           |
//             out: se TRUE indica che in pszFont c'Š un nuovo font         |
// valore restituito:                                                       |
// BOOL: TRUE = successo, FALSE = errore                                    |
//==========================================================================/

BOOL APIENTRY WfontDlg(HWND hwnd, PSZ pszTitle, PSZ pszSample, PSZ pszFont,
                       ULONG cbFont, PULONG pfl);


// various constants ------------------------------------------------------

#define CWDS_CENTER        0x1   // force the dialog to be centered inside
                                 // its parent window ignoring the ptl
                                 // CLRWHEEL structure member


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

//========================================================================\
// funzioni di utilit…                                                    |
//========================================================================/

// deseleziona tutti gli item in una listbox
#define WlboxDeselectAll(hwnd)\
   ((BOOL)WinSendMsg(hwnd, LM_SELECTITEM, (MPARAM)LIT_NONE, MPVOID))

// inizializza tutti i controlli
BOOL APIENTRY InitControls(HAB hab);

//==========================================================================\
// Centra la finestra di dialogo hwnd all'interno di hin mostrandola,
// attivandola e portandola al top dello Z-order.
// Se hin = NULLHANDLE centra la finestra all'interno della parent window
// parametri:                                                               |
// HWND hwnd = handle della finestra da centrare
// HWND hin = handle della finestra in cui viene effettuata la centratura
// valore restituito:                                                       |
// BOOL = TRUE/FALSE : successo/errore                                      |
//==========================================================================/

BOOL APIENTRY WcenterInWindow(HWND hwnd, HWND hin);

//==========================================================================\
// Centra la finestra di dialogo hwnd all'interno della sua parent window   |
// mostrandola, attivandola e portandola al top dello Z-order.              |
// parametri:                                                               |
// HWND hwnd = handle della dialog window                                   |
// valore restituito:                                                       |
// BOOL = TRUE/FALSE : successo/errore                                      |
//==========================================================================/

BOOL APIENTRY WcenterWindow(HWND hwnd);

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

MRESULT APIENTRY WdlgMinMax(HWND hwnd, MPARAM mp1, PULONG pul);

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

BOOL APIENTRY WdrawBtnBkgnd(PUSERBUTTON pub, ULONG cx, ULONG cy, LONG color);

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

BOOL APIENTRY WdrawDisabledRect(HPS hps, PRECTL prcl, LONG color);

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
                           LONG clrNW, LONG clrSE, LONG clrbkgnd, ULONG fl);

//==========================================================================\
// Legge il file pszfile e lo importa nella MLE hmle                        |
// parametri:                                                               |
// HWND hmle = handle MLE                                                   |
// PSZ pszfile = nome file                                                  |
// valore restituito:                                                       |
// BOOL = TRUE/FALSE : successo/errore                                      |
//==========================================================================/

BOOL APIENTRY Wfil2mle(HWND hmle, PSZ pszfile);

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

BOOL APIENTRY WfillListBox(HWND hwnd, ULONG ulId, PSZ pszList);

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

LONG APIENTRY WgetCtlClr(HWND hwnd, HPS hps, ULONG ulid1, ULONG ulid2, LONG ldef);

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

HBITMAP APIENTRY WloadBitmap(HWND hwnd, HMODULE hmod, ULONG id);

//==========================================================================\
// legge il contenuto della MLE hmle e lo scrive nel file pszfile           |
// parametri:                                                               |
// HWND hmle = handle finestra MLE                                          |
// PSZ pszfile = nome del file                                              |
// valore restituito:                                                       |
// BOOL = TRUE/FALSE : successo/errore                                      |
//==========================================================================/

BOOL APIENTRY Wmle2fil(HWND hmle, PSZ pszfile);

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

ULONG APIENTRY Wmle2str(HWND hmle, PSZ pszstr);

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

VOID APIENTRY WmoveRect(PRECTL prcl, LONG x, LONG y);

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

BOOL APIENTRY WopenObject(PSZ pszObj, ULONG flmode, ULONG flOpt);

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

BOOL APIENTRY Wprint(HWND hwnd, PSZ pszMsg, ULONG fl);

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

BOOL APIENTRY WprocSpin(ULONG ulEvent, HWND hspin, PLONG pl, LONG lcur);

//==========================================================================\
// restituisce le dimensioni di una bitmap dall'handle della bitmap stessa  |
// parametri:                                                               |
// HBITMAP hbmp = bitmap handle                                             |
// PSIZEL pszl = indirizzo in cui memorizzare dimensione bitmap             |
// valore restituito:                                                       |
// BOOL : TRUE = successo, FALSE = errore                                   |
//==========================================================================/

BOOL APIENTRY WqueryBitmapSize(HBITMAP hbmp, PSIZEL pszl);

//==========================================================================\
// ritorna l'handle della frame principale della finestra hwnd              |
// (per frame principale si intende il child di HWND_DESKTOP)               |
// parametri:                                                               |
// HWND hwnd = finestra di cui si vuole rintracciare la frame window        |
// valore restituito:                                                       |
// HWND handle frame window contenente la finestra o un suo parent          |
//==========================================================================/

HWND APIENTRY WqueryHwndFrame(HWND hwnd);

//==========================================================================\
// cambia la bitmap mostrata dal bottone                                    |
// parametri: --------------------------------------------------------------|
// HWND hwnd: handle dialogo                                                |
// ULONG ulId: ID bottone                                                   |
// HBITMAP hbmp: handle bitmap                                              |
// valore restituito:                                                       |
// BOOL : TRUE = successo, FALSE = errore                                   |
//==========================================================================/

BOOL APIENTRY WsetButtonBmp(HWND hwnd, ULONG ulId, HBITMAP hbmp);

//==========================================================================\
// cambia il bottone di default di un dialogo                               |
// parametri:                                                               |
// HWND hwnd = handle dialog window                                         |
// ULONG id = handle bottone                                                |
// valore restituito:                                                       |
// BOOL : TRUE = successo, FALSE = errore                                   |
//==========================================================================/

BOOL APIENTRY WsetDefBtn(HWND hwnd, ULONG id);

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

BOOL APIENTRY WsetDlgFonts(HWND hwnd, HWND htbar, PSZ pszFont);

//==========================================================================\
// Imposta l'icona di una finestra di dialogo caricando la risorsa id dal   |
// modulo hmod. Se id = 0 considera hmod come l'indirizzo della stringa     |
// del nome del file di icona                                               |
// parametri:                                                               |
// HWND hwnd    : handle della dialog window                                |
// PVOID hmod   : indirizzo handle modulo contente icona o nome file icona  |
// ULONG id     : id risorsa                                                |
// valore restituito:                                                       |
// HPOINTER = handle dell'icona usata dal dialogo                           |
//==========================================================================/

HPOINTER APIENTRY WsetDlgIcon(HWND hwnd, HMODULE hmod, ULONG id);

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

BOOL APIENTRY WsetRelativePos(HWND hwnd, PSWP pswp);

//==========================================================================\
// assegna al mouse un puntatore del set di puntatori corrente              |
// parametri:                                                               |
// LONG lid: id puntatore sistema                                           |
// valore restituito:                                                       |
// BOOL : TRUE/FALSE = successo/errore                                      |
//==========================================================================/

BOOL APIENTRY WsetSysMouPtr(LONG lid);

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

BOOL APIENTRY Wstr2mle(HWND hmle, PSZ pszstr, INT cb);

#endif
