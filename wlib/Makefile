#
# Makefile for the libwin.a library.
#

W = ../wincl/win

FILES =	Makefile \
	w_open.c \
	w_term.c

OBJECTS =	w_open.o \
		w_term.o

TARGET =	libwin.a

CFLAGS = -O -I../wincl

CC = cc
AR = ar
RANLIB = ranlib

PRINT = print
TOUCH = touch

CP = cp
CHOWN = chown
CHGRP = chgrp

RM = -rm

all: $(OBJECTS)


w_open.o: w_open.c
	$(CC) $(CFLAGS) -c w_open.c
	$(AR) uv $(TARGET) w_open.o
	$(RANLIB) $(TARGET)

w_open.o: $W/w_const.h \
	  $W/w_types.h \
	  $W/w_structs.h \
	  $W/w_macros.h \
	  $W/w_liberr.h



w_term.o: w_term.c
	$(CC) $(CFLAGS) -c w_term.c
	$(AR) uv $(TARGET) w_term.o
	$(RANLIB) $(TARGET)

w_term.o: $W/w_const.h \
	  $W/w_types.h \
	  $W/w_structs.h \
	  $W/w_macros.h


#
# Print all recently changed files.
#

print: $(FILES)
	$(PRINT) $?
	$(TOUCH) $@


#
# Install the library in the directory /usr/lib.
#

install: /usr/lib/libwin.a

/usr/lib/libwin.a: libwin.a
	$(CP) libwin.a /usr/lib
	cd /usr/lib ; $(CHOWN) bin libwin.a ; $(CHGRP) bin libwin.a


#
# Clean up.
#

cleanup:
	$(RM) $(OBJECTS) $(TARGET)
