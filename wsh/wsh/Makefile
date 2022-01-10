#
# Makefile for the window shell - wsh.
#

SOURCES =	wsh.c \
		wsh_defs.h \
		wsh_glbl.h \
		wsh_msg.h \
		../h/w_language.h \
		../h/w_wsh.h


OBJECTS =	wsh.o

W = ../../wincl/win
H = ../h

CFLAGS = -O -n -I../../wincl
CC = cc

PRINT = print
TOUCH = touch

RM = -rm

#
# Compile the window shell.
#

wsh: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

wsh.o: wsh.c \
       $W/w_const.h \
       $W/w_types.h \
       $W/w_structs.h \
       $W/w_macros.h \
       $H/w_wsh.h \
       wsh_defs.h \
       wsh_glbl.h \
       wsh_msg.h
	$(CC) -c $(CFLAGS) wsh.c


#
# Print all recently changed files.
#

print: Makefile $(SOURCES)
	$(PRINT) $?
	$(TOUCH) $@


#
# Clean up.
#

cleanup:
	$(RM) $(OBJECTS) wsh
