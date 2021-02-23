# A makefile for MSVC's nmake.

OBJ = main.obj world.obj console.obj tick.obj print.obj open-simplex-noise.obj

micraneft.exe: $(OBJ)
	$(CC) /Fe:$@ $**

# A weird workaround for a .c file in another directory.
{open-simplex-noise\}.c{$(ODIR)}.obj::
	$(CC) /c $<

open-simplex-noise.obj: open-simplex-noise\open-simplex-noise.h

print.obj: print.h world.h console.h
tick.obj: world.h
console.obj: console.h
world.obj: world.h console.h open-simplex-noise\open-simplex-noise.h
main.obj: print.h world.h console.h

clean:
	-rm micraneft.exe $(OBJ)
