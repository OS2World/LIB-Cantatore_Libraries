//========================================================================\
//                                3Dctls.H                                |
//========================================================================/


//#define OS_VER_WARP_4

#define WC_STATUSBAR            "StatusBar"
#define WC_3DGROUPBOX           "3DGroupBox"

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
