The licence is set for each file separately.


1) How to compile the GUI:

1.1) You need:
- wxWidgets 2.9. (an earlier version won't work, cause I use wxString functions only available in 2.9., I use atm 2.9.2)

In the package a makefile for the gcc (makefile.gcc) and a makefile for the MS Visual C Compiler (makefile.vc) is included, but you can generate your own makefile for the compiler of your choice.

Compiling wxWidgets:
- Compile it with BUILD=release otherwise the executable will be very large.
- The vc compiler somehow generates a smaller executable.
- Set the linkage to static (in the config.gcc or config.vc) before you compile the wxWidgets, to ensure, that also on other PC's your executable will run.

If you use my makefiles, you have to change the WX_DIR Variable to your wxWidgets directory. This Variable is nearly at the beginning of the makefiles.
note: I have set the linkage to static in the corresponding config.* files, if you have not compiled you wxWidgets with static linkage the compiler will fail, when linking the GUI.

1.2) Using gcc:  <-- not provide any more
change in the uMod_GUI Directory and type:
mingw32-make -f makefile.gcc BUILD=release
(note: with the options -j 4 you compile simultaneously with 4 threads)

1.3) Using vc:
change in the uMod_GUI Directory and type:
nmake -f makefile.vc BUILD=release
(note: you need to use the special MS Visual prompt)


2) How to compile the dll:

2.1) You need:
- The DirectX SDK (I use June 2010)
- MS Visual C Compiler (I use the free Express 2010 version)
- maybe the Microsoft Windows SDK

There exist two makefiles, one for the mingw32-make.exe (makefile.gcc) which call the vc compiler and one for the nmake.exe (makefile.vc) which also calls the vc compiler. I have created the makefile.gcc only to use the -j 4 option of the gnu make.

You have to compile the 3 dll's separately (one for each injection method). The dll will be copied after successful compilation in the uMod_GUI/bin directory.

If you want to use the logging mode of the dll you have to parse LOG_MESSAGE=1


2.2) Using gcc (BUT you need the vc compiler!):
change in the uMod_DXMain Directory and type:
mingw32-make -f makefile.gcc HI=1
mingw32-make -f makefile.gcc DI=1
mingw32-make -f makefile.gcc NI=1
(note: you need to use the special MS Visual prompt)


2.3) Using vc:
change in the uMod_DXMain Directory and type:
nmake -f makefile.vc HI=1
nmake -f makefile.vc DI=1
nmake -f makefile.vc NI=1
(note: you need to use the special MS Visual prompt)