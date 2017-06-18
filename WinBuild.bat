@echo off

set builddir=Build\x64\
::Deal with VC changing the working directory
set workdir=%cd%
call "C:\Program Files (x86)\Microsoft Visual C++ Build Tools\vcbuildtools.bat" x64
cd %workdir%

set compilerflags=/Gm /Zi /c /EHsc /ISDL2\include /Fo.\%builddir% /Fd./%builddir%
set linkflags=/SUBSYSTEM:CONSOLE /OUT:%builddir%gabeui.exe

::Make
cl.exe %compilerflags% src\*.cpp src\jsmn\jsmn.c
::Link
link %linkflags% %builddir%*.obj SDL2\lib\x64\*.lib

::cleanup
::del *.obj *.idb *.pdb