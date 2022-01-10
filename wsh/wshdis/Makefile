#
# Makefile for the wsh disassembler - wshdis.
#

SOURCES =	wshdis.c \
		wshdis_file.c \
		wshdis_defs.h \
		wshdis_glbl.h \
		wshdis_msg.h


OBJECTS =	wshdis.o \
		wshdis_file.o


W = ../../wincl/win
H = ../h

CFLAGS = -O -n -I../../wincl
CC = cc

PRINT = print
TOUCH = touch

RM = -rm


#
# Compile the wsh disassembler.
#

wshdis: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

wshdis.o: wshdis.c \
	  $W/w_const.h \
	  $W/w_types.h \
	  $W/w_structs.h \
	  $H/w_language.h \
	  $H/w_wsh.h \
	  wshdis_defs.h \
	  wshdis_glbl.h \
	  wshdis_msg.h
	$(CC) -c $(CFLAGS) wshdis.c

wshdis_file.o: wshdis_file.c \
	       wshdis_defs.h \
	       wshdis_glbl.h
	$(CC) -c $(CFLAGS) wshdis_file.c


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
	$(RM) $(OBJECTS) wshdis
