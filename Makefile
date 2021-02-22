# A makefile for MSVC's nmake.

OBJ = main.obj world.obj console.obj tick.obj print.obj

micraneft.exe : $(OBJ)
	$(CC) /Fe:$@ $**

print.obj : world.h console.h
tick.obj : world.h
console.obj : console.h
world.obj : world.h console.h
main.obj : world.h console.h

clean :
	-rm micraneft.exe $(OBJ)
