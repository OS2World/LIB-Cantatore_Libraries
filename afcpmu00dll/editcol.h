//=========================================================================\
// edit color dialog based on the undocumented color wheel OS/2 control    |
// from an idea of Paul Ratcliffe (E-mail: paulr@orac.clara.co.uk or       |
// paul.ratcliffe@bbc.co.uk)                                               |
// coded by Alessandro Cantatore (alexcant@tin.it                          |
// version 1.1                                                             |
//=========================================================================/

// color wheel class ------------------------------------------------------

#define WC_COLORWHEEL               "ColorSelectClass"


//=========================================================================\
// messages:                                                               |
// WM_COLORWHEELCHANGED : mp1 = LONG RGB color                             |
//                        mp2 = MPVOID                                     |
//                        is sent by the color wheel control to its owner  |
//                        when the color wheel color changes               |
//                        Is is also sent from the "edit color" dialog to  |
//                        its owner window                                 |
// CWM_COLORWHEELSETVAL : mp1 = LONG RGB color                             |
//                        mp2 = MPVOID                                     |
//                        is sent to the color wheel to set the current    |
//                        color                                            |
//=========================================================================/


// warp 4 messages
#define WM_COLORWHEELCHANGED        0x0601
#define CWM_COLORWHEELSETVAL        0x0602

// warp 3 messages
#define WM_COLORWHEELCHANGED3       0x130C
#define CWM_COLORWHEELSETVAL3       0x1384


// various constants ------------------------------------------------------

#define CWDS_CENTER        0x1   // force the dialog to be centered inside
                                 // its parent window ignoring the ptl
                                 // CLRWHEEL structure member

// application structures -------------------------------------------------

typedef struct {
   UINT upd   : 1;
   UINT spin  : 1;
} STATUS;

typedef struct {
   PCLRWHEEL pcw;
   HWND hOwner;                    // handle owner window
   STATUS is;                      // stato finestra
   ULONG cwChangedMsg;             // WM_COLORWHEELCHANGED(-3)
   ULONG cwSetMsg;                 // CWM_COLORWHEELSETVAL(-3)
   CLR clrUndo;                    // start color
   CLR clrCurr;                    // current color
} APPDATA, * PAPPDATA;


// functions --------------------------------------------------------------

MRESULT EXPENTRY EditColorDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
VOID SetSpins(HWND hwnd, CLR clr);


// strings ----------------------------------------------------------------
#define SZ_DEFTITLE                 "Edit Color"
#define SZ_RED                      "~Red:"
#define SZ_GREEN                    "~Green:"
#define SZ_BLUE                     "~Blue:"
#define SZ_OK                       "~OK"
#define SZ_UNDO                     "~Undo"
#define SZ_CANCEL                   "~Cancel"


// dlg controls -----------------------------------------------------------
#define DLG_CWHEEL                  0x7100
#define CWHEEL                      0x7101
#define TXT_RED                     0x7102
#define SPN_RED                     0x7103
#define TXT_GREEN                   0x7104
#define SPN_GREEN                   0x7105
#define TXT_BLUE                    0x7106
#define SPN_BLUE                    0x7107
#define BTN_OK                      0x7108
#define BTN_UNDO                    0x7109
#define BTN_CANCEL                  0x7110
