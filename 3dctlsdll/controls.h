//========================================================================\
//                              CONTROLS.H                                |
//========================================================================/


//#define OS_VER_WARP_4

#define CTL_STATUSBAR           0
#define CTL_GROUPBOX            1
#define CTL_BUTTON              2

#define WC_STATUSBAR            "StatusBar"
#define WC_3DGROUPBOX           "3DGroupBox"

// 0x00008000 Š riservato per indicare lo sfondo trasparente

// strutture comune a statusbar, 3Dgroupbox e button

// struttura dati carattere mnemonico
typedef struct {             // struttura dati carattere mnemonico
   PCHAR pch;                // puntatore al carattere mnemonico
   INT line;                 // numero linea in cui Š presente il carattere
   INT cch;                  // quantit… caratteri precedenti il mnemonico
   ULONG cx;                 // larghezza mnemonico
} MNEMO, * PMNEMO;

// struttura dati testo controllo
typedef struct {
   SIZEL size;             // coordinate rettangolo testo;
   PSZ psz;                // testo controllo
   LONG cLines;            // massimo numero righe contenute nel rettangolo
   ULONG cy;               // font height
   ULONG cyDesc;           // maximum baseline descender
} TXTDATA, * PTXTDATA;

// struttura dati linea di testo
typedef struct {             // used to draw formatted multiple line text
   ULONG cch;                // numero caratteri linea corrente
   ULONG cxLine;             // larghezza linea corrente
   PCHAR pchLine;            // puntatore primo carattere linea
} DRAWLINE, * PDRAWLINE;


// struttura controllo
typedef struct {
   TXTDATA txt;                     // struttura dati testo
   MNEMO mn;                        // struttura dati carattere mnemonico
   POINTL ptOrig;                   // posizione controllo
   RECTL rcl;                       // control size
   PDRAWLINE pdl;                   // puntatore struttura dati linea testo
   ULONG id;                        // control id
   ULONG ulType;                    // control type
   ULONG ulStyle;                   // control style
   HWND hOwner;                     // owner window handle
   HWND hParent;                    // parent window handle
   ULONG fStatus;                   // status flag
   BOOL isDlg;                      // TRUE se l'owner del control Š dialog
   HPS hps;                         // presentations sapce handle
   BOOL flPPaint;                   // flag parent painting
   LONG lHilite;                    // hilite colour
   LONG lShadow;                    // shadow colour
   LONG lBack;                      // background colour
   LONG lFore;                      // foregorund colour
   LONG lTxtBack;                   // text background (GROUP_TXTHILITE)
   LONG lTxt;                       // text color (GROUP_TXTHILITE)
} CTL3D_DATA, *PCTL3D_DATA;


//========================================================================\
//                              WC_STATUSBAR                              |
//========================================================================/


// stili:
#define STBAR_STATUSBAR         0x00000000   // DEFAULT style: status bar
#define STBAR_CLICKABLE         0x00000080   // clickable status bar
#define STBAR_STATICTXT         0x00001000   // testo statico
// nel caso dello stile testo statico non hanno senso gli stili relativi
// all'effetto 3D x cui si pu• usare uno di essi
#define STBAR_AUTOSIZE          0x00000001

// mask of flags for DrawText():-------------------------------------------
#define STBAR_VALIDTEXTFLAGS    0x00006f00

// funzioni:
BOOL APIENTRY RegisterStatusBarClass(HAB hab);
MRESULT EXPENTRY StatusBarWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
VOID AdjustSBarWinPos(HWND hwnd, PCTL3D_DATA pcd, PSWP pswp);
VOID DrawStatusBar(HWND hwnd, PCTL3D_DATA pcd);
VOID DrawStaticTxt(HWND hwnd, PCTL3D_DATA pcd);
VOID ProcessTextChanged(PCTL3D_DATA pcd, HWND hwnd);

//========================================================================\
//                              WC_3DGROUPBOX                             |
//========================================================================/

// mutually exclusive
#define GROUP_TXTEXTERN    0x00000000 // text outside groupbox (default)
#define GROUP_TXTFRAMED    0x00000080 // text in a separate frame
#define GROUP_TXTENCLOSED  0x00000400 // text enclosed in the group frame
#define GROUP_3DHBAR       0x00001000 // horizontal bar
#define GROUP_3DVBAR       0x00001100 // vertical bar

#define GROUP_TXTAUTOSIZE  0x00000800 // text space autosized
#define GROUP_TXTHILITE    0x00004000 // text background = hilite titlebar


// funzioni:
BOOL APIENTRY RegisterGroupBox3DClass(HAB hab);
MRESULT EXPENTRY GroupBox3DWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
VOID DrawGroupBox(HWND hwnd, PCTL3D_DATA pcd);
BOOL DrawBar(HWND hwnd, PCTL3D_DATA pcd);
VOID DrawGroupBoxText(PCTL3D_DATA pcd, PRECTL prcl, ULONG flStyle);

//========================================================================\
//                               WC_3DBUTTONS                             |
//========================================================================/

#define WC_3DBUTTONS            "3DButtons"

// control structure:

typedef struct {
   ULONG id;                        // control id
   HWND hOwner;                     // owner window handle
   HWND hParent;                    // parent window handle
   HPS hps;                         // presentations sapce handle
   PSZ pszText;                     // control text
   ULONG cyFont;                    // font height
   ULONG cyFntDesc;                 // maximum baseline descender
   RECTL rcl;                       // control size
   LONG lHilite;                    // hilite colour
   LONG lShadow;                    // shadow colour
   LONG lBack;                      // background colour
   LONG lFore;                      // foregorund colour
   ULONG ulStyle;                   // control style
   ULONG fStatus;                   // status flag
   INT mnemo;                       // mnemonic character
} BUTTON3D, *PBUTTON3D;

// funzioni:
BOOL APIENTRY RegisterButtons3DClass (HAB hab);
MRESULT EXPENTRY Buttons3DWndProc (HWND, ULONG, MPARAM, MPARAM);


//========================================================================\
//                                 WC_3DLOGO                              |
//========================================================================/

#define WC_3DLOGO               "3DLogo"

// control structure:

typedef struct {
   ULONG id;                        // control id
   HWND hOwner;                     // owner window handle
   HWND hParent;                    // parent window handle
   HPS hps;                         // presentations sapce handle
   PSZ pszText;                     // control text
   ULONG cyFont;                    // font height
   ULONG cyFntDesc;                 // maximum baseline descender
   RECTL rcl;                       // control size
   LONG lHilite;                    // hilite colour
   LONG lShadow;                    // shadow colour
   LONG lBack;                      // background colour
   LONG lFore;                      // foregorund colour
   ULONG ulStyle;                   // control style
   ULONG fStatus;                   // status flag
   INT mnemo;                       // mnemonic character
} CTL3DLOGO, *PCTL3DLOGO;

// funzioni:
BOOL APIENTRY RegisterLogo3DClass (HAB hab);
MRESULT EXPENTRY Logo3DWndProc (HWND, ULONG, MPARAM, MPARAM);


//========================================================================\
//                                 WC_RGBCOLOR                            |
//========================================================================/

#define WC_RGBCOLOR             "RGB_ColorDlg"


//========================================================================\
//                               define comuni                            |
//========================================================================/


// frame height/depth: (common flags) -------------------------------------
#define CCTL_NOBORDER           0x0   // flat (DEFAULT)
#define CCTL_BORDER1            0x1   // 1 pixel high/deep
#define CCTL_BORDER2            0x2   // 2 pixels high/deep
#define CCTL_BORDER3            0x3   // 3 pixels high/deep
#define CCTL_BORDER4            0x4   // 4 pixels high/deep
#define CCTL_BORDER5            0x5   // 5 pixels high/deep
#define CCTL_BORDER6            0x6   // 6 pixels high/deep
#define CCTL_BORDER7            0x7   // 7 pixels high/deep

// bordered frame:---------------------------------------------------------
#define CCTL_NOFRAME           0x00   // no frame (DEFAULT)
#define CCTL_FRAME0            0x08   // no border
#define CCTL_FRAME1            0x10   // border width = border thickness
#define CCTL_FRAME2            0x18   // border width = 2 * border thickness
#define CCTL_FRAME3            0x20   // border width = 3 * border thickness
#define CCTL_FRAME4            0x28   // border width = 4 * border thickness
#define CCTL_FRAME5            0x30   // border width = 5 * border thickness
#define CCTL_FRAME6            0x38   // border width = 6 * border thickness

// 3D look style:----------------------------------------------------------
#define CCTL_INDENTED          0x00   // DEFAULT
#define CCTL_RAISED            0x40

// sfondo opaco/trasparente
#define CCTL_OPAQUEBKGND       0x00000000   // deafult: sfondo col background
#define CCTL_TRANSPARENT       0x00008000   // sfondo trasparente


// text styles defined in PMWIN.H:-----------------------------------------
//   DT_LEFT                    0x00000000
//   DT_CENTER                  0x00000100
//   DT_RIGHT                   0x00000200
//   DT_TOP                     0x00000000
//   DT_VCENTER                 0x00000400  // invalid with STBAR_GROUP
//   DT_BOTTOM                  0x00000800  // invalid with STBAR_GROUP
//   DT_MNEMONIC                0x00002000
//   DT_WORDBREAK               0x00004000  // invalid with STBAR_GROUP


//========================================================================\
// status flags:                                                          |
//========================================================================/

#define ISONWINDOW              0x00000001
#define ISCLICKED               0x00000002
#define ISCAPTURED              0x00000004


// default colours:--------------------------------------------------------
#define RGB_GRAY                0x00cccccc
#define RGB_DARKGRAY            0x00808080
#define RGB_DARKBLUE            0x00000080

// defines for class registration:-----------------------------------------
#define QWL_CTL            QWL_USER + 4
#define CTL_RESERVED       8           // dimensione memoria riservata al ctl

typedef struct {
   PCHAR pch;                // puntatore all'ultimo spazio
   LONG cxPre;               // larghezza testo precedente spazio
   LONG cxPost;              // larghezza testo dopo spazio fino ricalcolo pos
} SPACE, * PSPACE;


//========================================================================\
// macros:                                                                |
//========================================================================/

#define forever for(;;)
#define lowchar(ch)      ((((ch) < 'A') || ((ch) > 'Z'))?\
                         (ch):((ch) + 'a' - 'A'))


//========================================================================\
// internal functions:                                                    |
//========================================================================/

VOID InitCtl(HWND hwnd, PCTL3D_DATA pcd, PCREATESTRUCT pcrst);
MRESULT ActivateControl(HWND hwnd, SHORT flag);
MRESULT ProcStaticCtlFocus(HWND hwnd, HWND hFocus, BOOL isGetting);
MRESULT ControlMouseMove(HWND hwnd, PCTL3D_DATA pcd, SHORT sx, SHORT sy);
MRESULT ControlButton1Up(HWND hwnd, PCTL3D_DATA pcd, SHORT sx, SHORT sy);
LONG GetCtlColor(HWND hwnd, ULONG ulID1, ULONG ulID2, LONG lSysDef, LONG lDef);
VOID SetControlColors(HWND hwnd, PCTL3D_DATA pcd);
BOOL GetNewPresParms(HWND hwnd, PCTL3D_DATA pcd, LONG lIdx);
BOOL SetControlText(HWND hwnd, PCTL3D_DATA pcd, PWNDPARAMS pwprm);
MRESULT ProcessQueryPresParms(HWND hwnd, PWNDPARAMS pwprm, PCTL3D_DATA pcd);
VOID UpdateFontData(PCTL3D_DATA pcd);
VOID RepaintControl(HWND hwnd, PCTL3D_DATA pcd, BOOL flag);
VOID InvalidateControl(HWND hwnd, PCTL3D_DATA pcd);
VOID MeasureText(PCTL3D_DATA pcd, BOOL flBreak, BOOL flAsz);
VOID DrawText(PCTL3D_DATA pcd, ULONG flStyle, PRECTL prcl);
VOID PaintDisabled(PCTL3D_DATA pcd);
//VOID RepaintCtlBkGnd(PCTL3D_DATA pcd);
