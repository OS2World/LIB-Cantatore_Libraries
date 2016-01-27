//==========================================================================\
// (c) 199 * Alessandro Cantatore * Team OS/2 Italy                         |
// API wrapper
//==========================================================================/

#pragma strings(readonly)


#define INCL_WIN
#define INCL_DOSSEMAPHORES
#define INCL_DOSPROCESS
#define INCL_DOSMODULEMGR
#define INCL_GPI
#include <os2.h>
#include <stdlib.h>

extern HMTX hmtx;

PVOID myalloc(ULONG cb) {
   PVOID pv;
   if (WinRequestMutexSem(hmtx, SEM_INDEFINITE_WAIT)) return NULL;
   pv = malloc(cb);
   DosReleaseMutexSem(hmtx);
   return pv;
}


VOID myfree(PVOID pv) {
   if (WinRequestMutexSem(hmtx, SEM_INDEFINITE_WAIT)) return;
   free(pv);
   DosReleaseMutexSem(hmtx);
}


INT myheapmin(VOID) {
   INT rc;
   if (WinRequestMutexSem(hmtx, SEM_INDEFINITE_WAIT)) return -1;
   rc = _heapmin();
   DosReleaseMutexSem(hmtx);
   return rc;
}


BOOL SetItemText(HWND hwnd, ULONG id, PSZ psz) {
   return WinSetDlgItemText(hwnd, id, psz);
}


MRESULT DlgSend(HWND hwnd, ULONG id, ULONG msg, MPARAM mp1, MPARAM mp2) {
   return WinSendDlgItemMsg(hwnd, id, msg, mp1, mp2);
}


MRESULT Send(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   return WinSendMsg(hwnd, msg, mp1, mp2);
}


BOOL Post(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   return WinPostMsg(hwnd, msg, mp1, mp2);
}


BOOL Enable(HWND hwnd, BOOL fl) {
   return WinEnableWindow(hwnd, fl);
}
