//========================================================================\
// common.h: funzioni e define usati da tutti i controlli                 |
//========================================================================/

// define nuove classi
#ifndef _WINUTIL_H
   #include <winutil.h>
#endif

#define WC_EDLGSIMG     "000afcccDlgStaticImage"
#define WC_STATUSBAR    "000afcccDlgStatusBar"
#define WC_3DGROUPBOX   "000afcccGroupBox"
#define WC_BAR          "000afcccBar"
#define WC_EXTLISTBOX   "000afcccEnhListBox"
#define WC_COMPLISTBOX  "000afcccCmpListBox"
#define WC_MOREBOX      "000afcccMoreBox"

// messaggi interni:
#define WM_DELAYFOCUS           WM_USER + 4561

// vecchi stili comuni:

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


// struttura dati controlli comune

typedef struct {
   HWND hwnd;          // handle controllo
   ULONG cbtxt;        // lunghezza testo controllo
   PSZ psz;            // testo del controllo
   SIZEL wrtxt;        // rettangolo testo
   ULONG fl;           // stile controllo (differente da quanto memorizzato
                       // nel QWL_STYLE del controllo originario)
   SIZEL wr;           // dimensione controllo
   LONG lfgnd;         // foreground color
   LONG lbkgnd;        // background color
   LONG lhilite;       // hilite color
   LONG lshadow;       // shadow color
   ULONG cyFont;       // altezza font
} CTL, * PCTL;


// struttura dialogo progresso

typedef struct {
   HWND hNotify;
   HBITMAP hbmp;
   LONG color;
   PSZ pszTitle;
   PSZ pszPrgrss;
   PSZ pszTime;
   PSZ pszStop;
   PSZ FontNameSize;  // dialog font (something like "9.WarpSans")
   HWND hPos;         // ptl member or PRGSS_CENTER is relative to this window
   POINTL ptl;        // dialog position
   ULONG fl;
} PROGRESSDLG, * PPROGRESSDLG;


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


// variabili globali
extern HMTX hmtx;            // semaforo accesso a risorse condivise
extern HMODULE hmod;         // module handle
extern ULONG flWCstc;        // stile default classe static
extern PFNWP pfnwpWCstc;     // procedura originale classe WC_STATIC
extern ULONG cbWCstc;        // offset alla winword aggiuntiva

// macro usata con WM_QUERYWINDOWPARAMS restituisce TRUE se il parametro
// richiesto non Š WPM_TEXT | WPM_CTLDATA | WPM_CCHTEXT | WPM_CBCTLDATA

#define isOrdWndParm(x)   (!(x & \
                          (WPM_TEXT|WPM_CTLDATA|WPM_CCHTEXT|WPM_CBCTLDATA)))

// prototipi funzioni :
// funzione inizializzazione DLL
//ULONG _System _3DCtlsInitTerm(ULONG ul, ULONG flag);

// pmapi.c (funzioni esportate)
BOOL APIENTRY WcenterInWindow(HWND hwnd, HWND hin);
BOOL APIENTRY WcenterWindow(HWND hwnd);
MRESULT APIENTRY WdlgMinMax(HWND hwnd, MPARAM mp1, PULONG pul);
BOOL APIENTRY WdrawDisabledRect(HPS hps, PRECTL prcl, LONG color);
LONG APIENTRY WdrawDisabledText(HPS hps, LONG cch, PSZ psz, PRECTL prcl,
                           LONG clrNW, LONG clrSE, LONG clrbkgnd, ULONG fl);
BOOL APIENTRY Wfil2mle(HWND hmle, PSZ pszfile);
BOOL APIENTRY WfillListBox(HWND hwnd, ULONG ulId, PSZ pszList);
HBITMAP APIENTRY WloadBitmap(HWND hwnd, HMODULE hmod, ULONG id);
BOOL APIENTRY Wmle2fil(HWND hmle, PSZ pszfile);
ULONG APIENTRY Wmle2str(HWND hmle, PSZ pszstr);
BOOL APIENTRY Wstr2mle(HWND hmle, PSZ pszstr, INT cb);
VOID APIENTRY WmoveRect(PRECTL prcl, LONG x, LONG y);
BOOL APIENTRY WopenObject(PSZ pszObj, ULONG flmode, ULONG flOpt);
BOOL APIENTRY Wprint(HWND hwnd, PSZ pszMsg, ULONG fl);
BOOL APIENTRY WqueryBitmapSize(HBITMAP hbmp, PSIZEL pszl);
HWND APIENTRY WqueryHwndFrame(HWND hwnd);
BOOL APIENTRY WsetButtonBmp(HWND hwnd, ULONG ulId, HBITMAP hbmp);
BOOL APIENTRY WsetDefBtn(HWND hwnd, ULONG id);
BOOL APIENTRY WsetDlgFonts(HWND hwnd, HWND htbar, PSZ pszFont);
HPOINTER APIENTRY WsetDlgIcon(HWND hwnd, HMODULE hmod, ULONG id);
BOOL APIENTRY WsetRelativePos(HWND hwnd, PSWP pswp);
BOOL APIENTRY WsetSysMouPtr(LONG lid);
BOOL APIENTRY WdrawBtnBkgnd(PUSERBUTTON pub, ULONG cx, ULONG cy, LONG color);
LONG APIENTRY WgetCtlClr(HWND hwnd, HPS hps, ULONG ulid1, ULONG ulid2, LONG ldef);
BOOL APIENTRY WprocSpin(ULONG ulEvent, HWND hspin, PLONG pl, LONG lcur);

// common.c
BOOL APIENTRY InitControls(HAB hab);
LONG APIENTRY WgetSysValue(LONG id);
BOOL WinRefreshWin(HWND hwnd);
// funzioni usate da WC_STATUSBAR e WC_3DGROUPBOX
PCTL stGetData(HWND hwnd);
PCTL stCreateProc(HWND hwnd, PCREATESTRUCT pc, ULONG style);
ULONG stSetText(PCTL pct, PSZ psz);
MRESULT stReturnText(PCTL pct, PWNDPARAMS pwp);
VOID stEnable(PCTL pct, BOOL fl);
VOID stUpdateCtlSize(HWND hwnd, PSWP pswp);
VOID stDestroyProc(HWND hwnd);
VOID stUpdColors(PCTL pct, HPS hps);
ULONG stSizeFont(HPS hps);
MRESULT stSCchange(PCTL pct);
MRESULT stPPchange(PCTL pct, ULONG fl);
// funzioni usate per disegnare bottoni in WC_COMPLISTBOX e WC_MOREBOX
VOID mkswp(PSWP pswp, HWND hwnd, LONG x, LONG y, LONG cx, LONG cy);
VOID rcl2swp(PSWP pswp, HWND hwnd, LONG x0, LONG y0, LONG x1, LONG y1);
BOOL drawDisBtn(PUSERBUTTON pub, ULONG cx, LONG color);
VOID drawAdd(PUSERBUTTON pub, ULONG cx);
VOID drawRem(PUSERBUTTON pub, ULONG cx);
VOID drawFind(PUSERBUTTON pub, ULONG cx);
VOID drawSwap(PUSERBUTTON pub, ULONG cx);
// funzione per disegnare nel giusto colore le checkbox disabilitate
VOID paintDisabledCheck(HWND hwnd, PRECTL prcl, LONG lclr);
// funzioni esportate presenti negli altri moduli
BOOL APIENTRY EDBarReg(HAB hab);
BOOL APIENTRY EDGrpReg(HAB hab);
BOOL APIENTRY EDImgReg(HAB hab);
BOOL APIENTRY EDStbReg(HAB hab);
BOOL APIENTRY EDElbReg(HAB hab);
BOOL APIENTRY EDClbReg(HAB hab);
BOOL APIENTRY EDMoreReg(HAB hab);

LONG APIENTRY WeditColorDlg(HWND hParent, HWND hOwner, PCLRWHEEL pcw);
BOOL APIENTRY WprogressBar(HWND hParent, HWND hOwner, PPROGRESSDLG ppd);
BOOL APIENTRY WfontDlg(HWND hwnd, PSZ pszTitle, PSZ pszSample, PSZ pszFont,
                       ULONG cbFont, PULONG pfl);

