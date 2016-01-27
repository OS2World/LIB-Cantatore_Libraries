-----------------------------------
Name:    afcbsu.dll Runtime/Library
License: BSD 3-Clause
Author:  Alessandro Cantatore
------------------------------------

ENGLISH
-----------------------

This library is used on Alessandro's "Batch Editor" program. 

TO DO:
add functions bsuballoc and bsubfree and change the header

Clear all the functions already present in the subsystem library ...
including memory allocation

Follow the instructions on page 213 of the Visual Age Programming Guide to
create the DLL that includes both the run-time functions and aditional features.

Use together with string functions:

WinCompareStrigs
WinCpTranlateChar
WinCpTranslateString
WinNextChar
WinPrevChar
WinQueryCp
WinSetCp
WinSetDlgItemShort
WinUpper
WinUpperChar

The functions of utility ... PM were transferred in Control DLL.

ITALIAN
-----------------------
il codice e' incompleto:
aggiungere le funzioni bsuballoc e bsubfree e modificare l'header

cancellare tutte le funzioni gi… presenti nella subsystem library
incluse quelle di allocazione memoria

seguire istruzioni a pag 213 del Visual Age Programming Guide per
creare DLL che includa sia le funzioni di runtime che mie funzioni
aggiuntive

usare insieme alle funzioni stringa:

WinCompareStrigs
WinCpTranlateChar
WinCpTranslateString
WinNextChar
WinPrevChar
WinQueryCp
WinSetCp
WinSetDlgItemShort
WinUpper
WinUpperChar


Le funzioni di utilit… PM sono state trasferite nella DLL dei controlli