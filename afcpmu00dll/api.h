// API wrapper header

BOOL SetItemText(HWND hwnd, ULONG id, PSZ psz);
MRESULT DlgSend(HWND hwnd, ULONG id, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT Send(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
BOOL Post(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
BOOL Enable(HWND hwnd, BOOL fl);
PVOID myalloc(ULONG cb);
VOID myfree(PVOID pv);
INT myheapmin(VOID);


#undef WinSetDlgItemText
#undef WinSendDlgItemMsg
#undef WinSendMsg
#undef WinPostMsg
#undef WinEnableWindow

#undef malloc
#undef free
#undef _heapmin

#define WinSetDlgItemText SetItemText
#define WinSendDlgItemMsg DlgSend
#define WinSendMsg Send
#define WinPostMsg Post
#define WinEnableWindow Enable

#define malloc myalloc
#define free myfree
#define _heapmin myheapmin
