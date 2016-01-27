# debug.mak
# Creato dal MakeMake del WorkFrame/2 IBM alle 18:35:14 del 23 Marzo 1998
#
# Le azioni incluse in questo file Make sono:
#  Compile::C++ Compiler
#  Link::Linker

.SUFFIXES: .C .obj 

.all: \
    .\3Dctls.dll

.C.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Ss /Q /V"3D controls * (c) 1997 Alessandro Cantatore * Team OS/2 Italy" /Ti /Rn /Ge- /G5 /Gu /C %s

{F:\dev\prj\CCtrl\3DCTLS\dll8}.C.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Ss /Q /V"3D controls * (c) 1997 Alessandro Cantatore * Team OS/2 Italy" /Ti /Rn /Ge- /G5 /Gu /C %s

.\3Dctls.dll: \
    .\statusbar.obj \
    .\utils.obj \
    .\3DGroupBox.obj \
    .\clib.obj \
    {$(LIB)}3Dctls.def
    @echo " Link::Linker "
    icc.exe @<<
     /B" /de /exepack:2 /pmtype:pm /packd /optfunc /nologo"
     /Fe3Dctls.dll 
     3Dctls.def
     .\statusbar.obj
     .\utils.obj
     .\3DGroupBox.obj
     .\clib.obj
<<

.\statusbar.obj: \
    F:\dev\prj\CCtrl\3DCTLS\dll8\statusbar.C \
    {F:\dev\prj\CCtrl\3DCTLS\dll8;$(INCLUDE);}clib.h \
    {F:\dev\prj\CCtrl\3DCTLS\dll8;$(INCLUDE);}controls.h

.\clib.obj: \
    F:\dev\prj\CCtrl\3DCTLS\dll8\clib.c \
    {F:\dev\prj\CCtrl\3DCTLS\dll8;$(INCLUDE);}clib.h

.\3DGroupBox.obj: \
    F:\dev\prj\CCtrl\3DCTLS\dll8\3DGroupBox.C \
    {F:\dev\prj\CCtrl\3DCTLS\dll8;$(INCLUDE);}clib.h \
    {F:\dev\prj\CCtrl\3DCTLS\dll8;$(INCLUDE);}controls.h

.\utils.obj: \
    F:\dev\prj\CCtrl\3DCTLS\dll8\utils.c \
    {F:\dev\prj\CCtrl\3DCTLS\dll8;$(INCLUDE);}clib.h \
    {F:\dev\prj\CCtrl\3DCTLS\dll8;$(INCLUDE);}controls.h
