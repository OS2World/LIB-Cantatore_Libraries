//==========================================================================\
//                              elbox.h                                     |
//==========================================================================/

// il controllo dovrebbe permettere di creare una listbox multicolonna
// con possibilit… di inserire controlli come checkbox all'interno delle
// colonne e di spostare gli item all'interno della listbox tramite il
// drag & drop

// le colonne possono contere i seguenti elementi:
// reciprocamente esclusivi:
#define ELBC_CHKBOX  0x01  // checkbox
#define ELBC_ICON    0x02  // icona
#define ELBC_MINI    0x04  // mini icona
#define ELBC_PTR     0x08  // pointer
// anche in combinazione con uno dei precedenti stili:
#define ELBC_TEXT    0x10  // testo             
// allineamento orizzontale all'interno della colonna
//   DT_LEFT                    0x0000
//   DT_CENTER                  0x0100
//   DT_RIGHT                   0x0200

// messaggi interni:
#define WM_DELAYFOCUS           WM_USER + 4561

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
#define ELBXN_CURSORMOVE        0x0006


// messaggi controllo
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
// in mp1 deve essere l'indice dell'item che si vuole al top
#define ELBXM_SETTOPINDEX          WM_USER + 0xff3
// come sopra sposta solo il cursore sull'item mp1
#define ELBXM_SETCURSOR          WM_USER + 0xff4
// forza il repaint di un particolare item passato in mp1. Se mp1 == 0xffff
// forza repaint intero componente listbox
#define ELBXM_REDRAWITEM         WM_USER + 0xff5

// id controlli interni
#define FID_LIST                     0xa001
// stili complessivi controllo:
// indica che l'ordine degli item pu• essere cambiato tramite il drag & drop
#define ELBCS_ITEMDRAGGABLE    0x0001
// indica che il quadrato alla sinistra di una colonna deve essere considerato
// come una checkbox ed in quanto tale notifica l'owner con ELBXN_CHECKED
// piuttosto che con ELBXN_SELECT. Il contenuto dei parametri Š lo stesso
#define ELBCS_ITEMCHECKBOXED   0x0002


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

typedef struct {
   USHORT x0;
   USHORT x1;
} COLCOORD, *PCOLCOORD;

#pragma pack()

typedef struct {
   ULONG cx;           // larghezza colonna intesa come percentuale
   RECTL rcl;          // rettangolo contenente testo header colonne
   ULONG cb;           // lunghezza testo
   PSZ psz;            // testo header;
   UCHAR mnemo;        // mnemonico descrizione colonna
} COLDATA, * PCOLDATA;

// flag posizione mouse durante drag:
#define ELBOX_DRAGIN    0x0
#define ELBOX_DRAGOUT   0x1
#define ELBOX_DRAGUP    0x2
#define ELBOX_DRAGDWN   0x3

typedef struct {
   UINT dragging  : 1; // operazione di trascinamento in corso
   UINT where     : 2; // posizione mouse durante operazione drag
   UINT           : 0;
} STATUS;

typedef struct {
   STATUS is;          // struttura stato controllo
   HAB hab;            // hanchor block handle
   HWND hwnd;          // handle controllo
   HWND hlbx;          // handle listbox child
   ULONG id;           // id controllo
   PFNWP lbxwprc;      // indirizzo procedura default listbox
   SIZEL szl;          // dimensione controllo
   SIZEL szllb;        // dimensioni listbox
   ULONG citems;       // totale items
   ULONG cyitem;       // altezza item listbox
   RECTL rcltopitem;   // rettangolo primo item listbox
   ULONG dragitem;     // indice item trascinato
   ULONG topitem;      // topitem
   ULONG cbtxt;        // lunghezza totale descrizioni colonne
   PSZ psz;            // descrizioni colonne separate da \t
   ULONG cyFont;       // altezza font
   ULONG crow;         // numero righe testo descrizione colonne
   ULONG fl;           // stile controllo (differente da quanto memorizzato
                       // nel QWL_STYLE del controllo originario)
   ULONG ccol;         // numero colonne
   PCOLDATA pcol;      // array di COLDATA
   LONG lfgnd;         // foreground color
   LONG lbkgnd;        // background color
} ELBOX, * PELBOX;

// funzioni controllo
MRESULT EXPENTRY EDElbWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
 BOOL elbCreateProc(HWND hwnd, PELBOXCDATA pelbcd,
                    PCREATESTRUCT pc, ULONG style);
VOID elbUpdateCtlSize(HWND hwnd, PSWP pswp);
MRESULT PaintELB(HWND hwnd);
MRESULT elbSetWparms(PELBOX pelb, PWNDPARAMS pwpm);
MRESULT elbQueryWparms(PELBOX pelb, PWNDPARAMS pwpm);
MRESULT elbSCchange(PELBOX pelb);
MRESULT elbPPchange(PELBOX pelb, ULONG fl);
VOID elbDestroyProc(PELBOX pelb);
MRESULT elbMouBtn(PELBOX pelb);
MRESULT elbSetFocus(HWND hwnd, MPARAM mp1, BOOL fl);
VOID elbUpdColors(PELBOX pelb, HPS hps);
BOOL setELBctldata(PELBOX pelb, PELBOXCDATA pelbcd);
ULONG elbSetText(PELBOX pelb, PSZ psz, LONG cb);
VOID sizeLBox(PELBOX pelb);
VOID sizeCols(PELBOX pelb);
MRESULT colCount(PELBOX pelb);
MRESULT colWidth(PELBOX pelb, ULONG icol);
MRESULT multiColWidth(PELBOX pelb, PCOLCOORD pcx, ULONG ccx);
MRESULT elbRedrawItem(PELBOX pelb, ULONG item);

// funzioni componente listbox subclassata
BOOL createChild(PELBOX pelb, PELBOXCDATA pelbcd);
MRESULT EXPENTRY newlboxproc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);


