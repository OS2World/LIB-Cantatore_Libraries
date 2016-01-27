//==========================================================================\
//                              image.h                                     |
//==========================================================================/

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

MRESULT EXPENTRY EDImgWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
