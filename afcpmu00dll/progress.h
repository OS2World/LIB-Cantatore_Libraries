// ========================================================================\
// mostra progress bar usando una bitmap o un colore                       |
//=========================================================================/


// progress dialog style: (* = current styles)
#define PRGSS_SIMPLE    0x0000   // *only one progress bar
#define PRGSS_DOUBLE    0x0001   // partial/total (double) progressbar
#define PRGSS_COLOR     0x0000   // *solid color progressbar
#define PRGSS_BITMAP    0x0002   // *bitmap progressbar
#define PRGSS_BTNSTOP   0x0010   // *add a Stop button
#define PRGSS_DETAIL    0x0020   // add a statusbar to show current operation
#define PRGSS_PERCENT   0x0040   // *show progress (percent) in text
#define PRGSS_TIME      0x0080   // *show elapsed time
#define PRGSS_DLG       0x0000   // *standalone dialog window
#define PRGSS_CTL       0x1000   // control window
#define PRGSS_CENTER    0x2000   // centra il dialogo nella parent window


// various constants ------------------------------------------------------

// ====================================================================== \
// -------------------------- MESSAGGI -----------------------------------|
// la comunicazione tra il dialogo di porogresso e la notify window       |
// avviene attraverso i seguenti msg:                                     |
// - WM_PRGRSREADY   : dal progress dialog alla notify window             |
//                     segnala l'avvenuta inizializzazione del dialogo    |
//                     mp1 = handle progress dialog                       |
//                     mp2 = 0                                            |
// ---------------------------------------------------------------------- |
//   nella notify window viene eseguita l'elaborazione il cui progresso   |
//   si vuole monitorare. Ad ogni step di elaborazione viene spedito      |
//   il messaggio:                                                        |
// - PRGSM_INCREMENT : dalla notify window al progress dialog             |
//                     il msg viene spedito con WinSendMsg()              |
//                     mp1 = (numeroStepEseguiti * 1000) / numeroStepTotali
//                     mp2 = 0                                            |
//                     rc  = FALSE (normale) - TRUE (Stop)                |
//   alla ricezione del precedente messaggio la progress dialog reagisce  |
//   ridipingendo la barra di progresso restituendo FALSE in condizione   |
//   normale, TRUE se in precedenza era stato premuto il tasto di stop    |
// ---------------------------------------------------------------------- |
// when it is necessary to suspend or stop the elaboration (and so the
// progress) in order to ask some questions or to notify an error
// condition the following message must be sent to the progress dialog:
// - PRGSM_MSG       :  the message must be sent from the notify window
//                      the message is shown through WinMessageBox
//                      mp1 = (PSZ) the address of a string including:
//                            title\tmessage
//                            where title is the message box title, \t is
//                            the tab char and message is the message to
//                            be displayed in the message box. To use the
//                            default message box title the string must
//                            be just \tmessage.
//                      mp2 = (ULONG) WinMessageBox flags
//                      rc  = (ULONG) WinMessageBox return value
// ----------------------------------------------------------------------
// when system shutdown is detected or the window is closed through the
// window list, the progress dialog sends the following message to the
// notify window:
// - WM_PRGRSQUIT    :  mp1 = (PSZ) msg buffer address
//                      mp2 = VOID
//                      rc = (BOOL) TRUE/FALSE:
//                           close immediately / ask for confirmation
// example of the notify window procedure in case a confirmation is
// required:
//    case WM_PRGRSQUIT:
//       strcpy((PSZ)mp2, "Please confirm\tProcessing is not yet complete!"
//                        " Do you want to terminate it?");
//       return (MRESULT)TRUE;
// in case no confirmation is required:
//    case WM_PRGRSQUIT:
//       return (MRESULT)FALSE;
// =======================================================================/

// messaggi spediti dalla notify window
#define PRGSM_INCREMENT        WM_USER + 0x1000
#define PRGSM_MSG              PRGSM_INCREMENT + 1
#define PRGSM_END              PRGSM_INCREMENT + 3

// messaggi spediti dal dialogo di progresso
#define WM_PRGRSREADY          PRGSM_INCREMENT
#define WM_PRGRSQUIT           PRGSM_INCREMENT + 2


// application structures -------------------------------------------------

typedef struct {
   PPROGRESSDLG ppd;               // struttura dati creazione progress dialog
   HAB hab;
   HWND hOwner;                    // handle owner window
   HWND hProgress;                 // handle progress bar
   HWND hButton;                   // handle bottone stop
   ULONG cxbmp;                    // larghezza bitmap
   SWP swp;                        // swp bottone
   SLDCDATA sldd;                  // dati coltrollo slider
   ULONG cxPrgs;                   // larghezza progress bar
   ULONG cxCur;                    // larghezza corrente
   ULONG cyTitle;                  // altezza titlebar
   RECTL rcl;                      // dialog window rectangle
   ULONG incr;                     // valore incremento
   BOOL flStop;                    // flag bottone Stop premuto
   QMSG qQuit;                     // struttura messaggio WM_QUIT
   ULONG ulCurPrg;                 // progresso corrente (%) in cifre
   CHAR achCurPrg[32];             // progresso corrente (%) in lettere
   ULONG start;                    // secondi trascorsi dal 1/1/1970 (inizio)
   ULONG current;                  // secondi trascorsi dal 1/1/1970 (correnti)
   ULONG elapsed;                  // secondi trascorsi da inizio
   CHAR achElapsed[80];            // tempo trascorso da inizio progresso
} APPDATA, * PAPPDATA;

// functions --------------------------------------------------------------

MRESULT EXPENTRY ProgressDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
ULONG WMsg(HWND hwnd, PSZ psz, ULONG fl);
PAPPDATA GetData(HWND hwnd);

// strings ----------------------------------------------------------------
#define SZ_PROGRESS    "Progress"
#define SZ_ELAPSTIME   "Elapsed time"
#define SZ_STOP        "Stop"

// dlg controls -----------------------------------------------------------
#define DLG_PROGRESS                0x7580
#define SLDR_PROGRESS               0x7581
#define TXT_PERCENT                 0x7582
#define TXT_TIME                    0x7583
#define BTN_STOP                    0x7584
