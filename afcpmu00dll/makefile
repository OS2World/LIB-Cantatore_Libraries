# makefile
# Creato dal MakeMake del WorkFrame/2 IBM alle 16:54:52 del 18 Marzo 1999
#
# Le azioni incluse in questo file Make sono:
#  Compile::Resource Compiler
#  Compile::C++ Compiler
#  Link::Linker
#  Bind::Resource Bind

.SUFFIXES: .c .obj .rc .res 

.all: \
    .\AFCPMU00.dll

.rc.res:
    @echo " Compile::Resource Compiler "
    rc.exe -r %s %|dpfF.RES

{F:\dev\prj\CCtrl\3DCTLS\all2}.rc.res:
    @echo " Compile::Resource Compiler "
    rc.exe -r %s %|dpfF.RES

.c.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Ss /Q /O /Rn /Ge- /G5 /Gu /C %s

{F:\dev\prj\CCtrl\3DCTLS\all2}.c.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Ss /Q /O /Rn /Ge- /G5 /Gu /C %s

.\AFCPMU00.dll: \
    .\progress.obj \
    .\statusbar.obj \
    .\api.obj \
    .\bar.obj \
    .\common.obj \
    .\pmapi.obj \
    .\editcol.obj \
    .\elbx.obj \
    .\elbxlbox.obj \
    .\fontdlg.obj \
    .\groupbox.obj \
    .\image.obj \
    .\clbx.obj \
    .\clbxaux.obj \
    .\clbxsub.obj \
    .\more.obj \
    .\moreaux.obj \
    .\common.res \
    {$(LIB)}dll.def
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /exepack:2 /pmtype:pm /packd /align:4 /optfunc /nologo"
     /FeAFCPMU00.dll 
     dll.def
     .\progress.obj
     .\statusbar.obj
     .\api.obj
     .\bar.obj
     .\common.obj
     .\pmapi.obj
     .\editcol.obj
     .\elbx.obj
     .\elbxlbox.obj
     .\fontdlg.obj
     .\groupbox.obj
     .\image.obj
     .\clbx.obj
     .\clbxaux.obj
     .\clbxsub.obj
     .\more.obj
     .\moreaux.obj
<<
    rc.exe -x2 .\common.res AFCPMU00.dll

.\common.res: \
    F:\dev\prj\CCtrl\3DCTLS\all2\common.rc \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE)}progress.dlg \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE)}progress.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE)}editcol.dlg \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE)}editcol.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE)}common.h

.\progress.obj: \
    F:\dev\prj\CCtrl\3DCTLS\all2\progress.c \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}common.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}api.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}progress.h

.\image.obj: \
    F:\dev\prj\CCtrl\3DCTLS\all2\image.C \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}common.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}api.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}image.h

.\fontdlg.obj: \
    F:\dev\prj\CCtrl\3DCTLS\all2\fontdlg.c \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}common.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}api.h

.\groupbox.obj: \
    F:\dev\prj\CCtrl\3DCTLS\all2\groupbox.C \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}common.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}api.h

.\elbxlbox.obj: \
    F:\dev\prj\CCtrl\3DCTLS\all2\elbxlbox.c \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}common.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}api.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}elbox.h

.\elbx.obj: \
    F:\dev\prj\CCtrl\3DCTLS\all2\elbx.c \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}common.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}api.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}elbox.h

.\clbx.obj: \
    F:\dev\prj\CCtrl\3DCTLS\all2\clbx.c \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}common.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}api.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}clbox.h

.\clbxaux.obj: \
    F:\dev\prj\CCtrl\3DCTLS\all2\clbxaux.c \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}common.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}api.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}clbox.h

.\clbxsub.obj: \
    F:\dev\prj\CCtrl\3DCTLS\all2\clbxsub.c \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}common.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}api.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}clbox.h

.\more.obj: \
    F:\dev\prj\CCtrl\3DCTLS\all2\more.c \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}common.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}api.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}more.h

.\moreaux.obj: \
    F:\dev\prj\CCtrl\3DCTLS\all2\moreaux.c \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}common.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}api.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}more.h

.\editcol.obj: \
    F:\dev\prj\CCtrl\3DCTLS\all2\editcol.c \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}common.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}api.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}editcol.h

.\common.obj: \
    F:\dev\prj\CCtrl\3DCTLS\all2\common.C \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}common.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}api.h

.\pmapi.obj: \
    F:\dev\prj\CCtrl\3DCTLS\all2\pmapi.C \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}common.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}api.h

.\bar.obj: \
    F:\dev\prj\CCtrl\3DCTLS\all2\bar.C \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}common.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}api.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}bar.h

.\api.obj: \
    F:\dev\prj\CCtrl\3DCTLS\all2\api.c

.\statusbar.obj: \
    F:\dev\prj\CCtrl\3DCTLS\all2\statusbar.C \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}common.h \
    {F:\dev\prj\CCtrl\3DCTLS\all2;$(INCLUDE);}api.h
