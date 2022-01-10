#
# Makefile for the window shell preprocessor - wshpp.
#

SOURCES =	wshpp1.c \
		wshpp2.c \
		wshpp3.c \
		wshpp_file.c \
		wshpp_glbl.h \
		wshpp_defs.h \
		wshpp_msg.h


OBJECTS =	wshpp1.o \
		wshpp2.o \
		wshpp3.o \
		wshpp_file.o


W = ../../wincl/win
H = ../h

CFLAGS = -O -n -I../../wincl
CC = cc

PRINT = print
TOUCH = touch

RM = -rm


#
# Compile the window shell preprocessor.
#

wshpp: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

wshpp1.o: wshpp1.c \
	  $W/w_const.h \
	  $W/w_types.h \
	  $W/w_structs.h \
	  $H/w_language.h \
	  $H/w_wsh.h \
	  wshpp_defs.h \
	  wshpp_glbl.h \
	  wshpp_msg.h
	$(CC) -c $(CFLAGS) wshpp1.c

wshpp2.o: wshpp2.c \
	  $W/w_const.h \
	  $W/w_types.h \
	  $W/w_structs.h \
	  $H/w_language.h \
	  $H/w_wsh.h \
	  wshpp_defs.h \
	  wshpp_glbl.h
	$(CC) -c $(CFLAGS) wshpp2.c

wshpp3.o: wshpp3.c \
	  $W/w_const.h \
	  $W/w_types.h \
	  $W/w_structs.h \
	  $H/w_language.h \
	  $H/w_wsh.h \
	  wshpp_defs.h \
	  wshpp_glbl.h
	$(CC) -c $(CFLAGS) wshpp3.c

wshpp_file.o: wshpp_file.c \
	      $W/w_const.h \
	      $W/w_types.h \
	      $W/w_structs.h \
	      $H/w_wsh.h \
	      wshpp_defs.h \
	      wshpp_glbl.h
	$(CC) -c $(CFLAGS) wshpp_file.c



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
	$(RM) $(OBJECTS) wshpp
