# makefile
# Creato dal MakeMake del WorkFrame/2 IBM alle 15:33:46 del 23 Marzo 1999
#
# Le azioni incluse in questo file Make sono:
#  Compile::C++ Compiler
#  Link::Linker

.SUFFIXES: .c .obj 

.all: \
    .\afcbsu00.dll

.c.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Ss /Q /O /Rn /Ge- /G5 /Gu /C %s

{F:\dev\prj\UTIL_DLL\DLL1}.c.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Ss /Q /O /Rn /Ge- /G5 /Gu /C %s

.\afcbsu00.dll: \
    .\strutil.obj \
    .\util.obj \
    .\fileutil.obj \
    {$(LIB)}DLL.def
    @echo " Link::Linker "
    icc.exe @<<
     /B" /exepack:2 /pmtype:pm /packd /align:4 /optfunc /nologo"
     /Feafcbsu00.dll 
     DLL.def
     .\strutil.obj
     .\util.obj
     .\fileutil.obj
<<

.\strutil.obj: \
    F:\dev\prj\UTIL_DLL\DLL1\strutil.c \
    {F:\dev\prj\UTIL_DLL\DLL1;$(INCLUDE);}common.h

.\fileutil.obj: \
    F:\dev\prj\UTIL_DLL\DLL1\fileutil.c \
    {F:\dev\prj\UTIL_DLL\DLL1;$(INCLUDE);}common.h

.\util.obj: \
    F:\dev\prj\UTIL_DLL\DLL1\util.c \
    {F:\dev\prj\UTIL_DLL\DLL1;$(INCLUDE);}common.h
