#
# Makefile for the window utility commands.
#

H = ../wincl/win

FILES =	Makefile \
	wopen.c \
	whead.c \
	wicon.c \
	rmicons.c \
	wtop.c \
	wzoom.c \
	wfont.c \
	wbg.c \
	wmsk.c \
	wmp.c \
	widtp.c \
	wpictrd.c \
	wdsize.c \
	whelp.c

OBJECTS =	wopen.o \
		whead.o \
		wicon.o \
		rmicons.o \
		wtop.o \
		wzoom.o \
		wfont.o \
		wbg.o \
		wmsk.o \
		wmp.o \
		widtp.o \
		wpictrd.o \
		wdsize.o \
		whelp.o

TARGETS =	wopen \
		whead \
		wicon \
		rmicons \
		wtop \
		wzoom \
		wfont \
		wbg \
		wmsk \
		wmp \
		widtp \
		wpictrd \
		wdsize \
		whelp


CFLAGS = -O -I../wincl

CC = cc

PRINT = print
TOUCH = touch

RM = -rm


all: $(TARGETS)


wopen: wopen.o
	$(CC) -n wopen.o -o $@

wopen.o: $H/w_const.h \
	 $H/w_types.h \
	 $H/w_structs.h \
	 $H/w_macros.h


whead: whead.o
	$(CC) -n whead.o -o $@

whead.o: $H/w_const.h \
	 $H/w_types.h \
	 $H/w_structs.h \
	 $H/w_macros.h


wicon: wicon.o
	$(CC) -n wicon.o -o $@

wicon.o: $H/w_const.h \
	 $H/w_types.h \
	 $H/w_structs.h \
	 $H/w_macros.h


rmicons: rmicons.o
	$(CC) -n rmicons.o -o $@

rmicons.o: $H/w_const.h \
	   $H/w_types.h \
	   $H/w_structs.h \
	   $H/w_macros.h


wtop: wtop.o
	$(CC) -n wtop.o -o $@

wtop.o: $H/w_const.h \
	$H/w_types.h \
	$H/w_structs.h \
	$H/w_macros.h


wzoom: wzoom.o
	$(CC) -n wzoom.o -o $@

wzoom.o: $H/w_const.h \
	 $H/w_types.h \
	 $H/w_structs.h \
	 $H/w_macros.h


wfont: wfont.o
	$(CC) -n wfont.o -o $@

wfont.o: $H/w_const.h \
	 $H/w_types.h \
	 $H/w_structs.h \
	 $H/w_macros.h


wbg: wbg.o
	$(CC) -n wbg.o -o $@

wbg.o: $H/w_const.h \
       $H/w_types.h \
       $H/w_structs.h \
       $H/w_macros.h


wmsk: wmsk.o
	$(CC) -n wmsk.o -o $@

wmsk.o: $H/w_const.h \
        $H/w_types.h \
        $H/w_structs.h \
        $H/w_macros.h


wmp: wmp.o
	$(CC) -n wmp.o -o $@

wmp.o: $H/w_const.h \
       $H/w_types.h \
       $H/w_structs.h \
       $H/w_macros.h


widtp: widtp.o
	$(CC) -n widtp.o -o $@

widtp.o: $H/w_const.h \
         $H/w_types.h \
         $H/w_structs.h \
         $H/w_macros.h


wpictrd: wpictrd.o
	$(CC) -n wpictrd.o -o $@

wpictrd.o: $H/w_const.h \
           $H/w_types.h \
           $H/w_structs.h \
           $H/w_macros.h


wdsize: wdsize.o
	$(CC) -n wdsize.o -o $@

wdsize.o: $H/w_const.h \
          $H/w_types.h \
          $H/w_structs.h \
          $H/w_macros.h


whelp: whelp.o
	$(CC) -n whelp.o -o $@

whelp.o: $H/w_const.h \
         $H/w_types.h \
         $H/w_structs.h \
         $H/w_macros.h



print: $(FILES)
	$(PRINT) $?
	$(TOUCH) $@


cleanup:
	$(RM) $(OBJECTS) $(TARGETS)
