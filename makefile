CLINK=wsock32.lib kernel32.lib user32.lib advapi32.lib
SWITCHES=/EHsc /Z7

all: puzzle.exe

puzzle.exe: puzzle.cpp puzzle.h
	cl $(SWITCHES) puzzle.cpp /link $(CLINK)

clean: 
	del *.obj
	del *.exe
	del *.pdb
	del *.ilk
