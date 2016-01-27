BOOL WprocSpin(ULONG ulEvent, HWND hspin, PLONG pl, LONG lcur) {
   static BOOL spin;
   switch (ulEvent) {
      // solo per sicurezza nel caso il sistema non invii SPBN_ENDSPIN
      case SPBN_SETFOCUS: spin = 0; return FALSE;
      // eventi realmente utili:
      case SPBN_UPARROW:
      case SPBN_DOWNARROW: spin = 1; return FALSE;
      case SPBN_ENDSPIN: spin = 0;
      case SPBN_CHANGE:
         return (!spin &&
                 (BOOL)WinSendMsg(hspin, SPBM_QUERYVALUE, (MPARAM)pl,
                                  MPFROM2SHORT(0, SPBQ_ALWAYSUPDATE)) &&
                 *pl != lcur);
   }
}


#define WprocSpin(ulEvent, hspin, pl)                                  \
   if ((ulEvent) == SPBN_UPARROW || (ulEvent) == SPBN_DOWNARROW) {     \
      pad->is.spin = 1;                                                \
      break;                                                           \
   }                                                                   \
   if ((ulEvent) == SPBN_ENDSPIN) pad->is.spin = 0;                    \
   if ((ulEvent) == SPBN_ENDSPIN || (ulEvent) == SPBN_CHANGE) {        \
      if (pad->is.spin ||                                              \
          !(BOOL)WinSendMsg((hspin), SPBM_QUERYVALUE, (MPARAM)pl,      \
                            MPFROM2SHORT(0, SPBQ_ALWAYSUPDATE)))       \
         break;                                                        \
   }

