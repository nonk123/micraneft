# A makefile for MSVC nmake.

OBJ = main.obj

micraneft.exe : $(OBJ)
	$(CC) /Fe:$@ $**

clean :
	-rm micraneft.exe $(OBJ)
