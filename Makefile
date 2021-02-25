# A makefile for MSVC's nmake.

OBJ = main.obj world.obj open-simplex-noise.obj

micraneft.exe: $(OBJ)
	$(CC) /Fe:$@ $**

# A weird workaround for a .c file in another directory.
{open-simplex-noise\}.c{$(ODIR)}.obj::
	$(CC) /c $<

open-simplex-noise.obj: open-simplex-noise\open-simplex-noise.h

world.obj: world.h open-simplex-noise\open-simplex-noise.h
main.obj: world.h ..\conge\conge.h

clean:
	-rm micraneft.exe $(OBJ)
