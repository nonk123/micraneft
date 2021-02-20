# A makefile for MSVC nmake.

OBJ = main.obj world.obj

micraneft.exe : $(OBJ)
	$(CC) /Fe:$@ $**

world.obj: world.h

clean :
	-rm micraneft.exe $(OBJ)
