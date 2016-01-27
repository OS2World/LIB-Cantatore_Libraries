// bar.h : bar control header file
//         the control is displayed as a 3D horizontal or vertical bar

// bar styles
#define BARS_HORIZONTAL        0x00
#define BARS_VERTICAL          0x80

// also valid:
// 3D look style:----------------------------------------------------------
// #define CCTL_INDENTED          0x00   // DEFAULT
// #define CCTL_RAISED            0x40


typedef struct {
   ULONG fl;                    // bar style
   SIZEL szl;                   // bar size
} BARDATA, * PBARDATA;
