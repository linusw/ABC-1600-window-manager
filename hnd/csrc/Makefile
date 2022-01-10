#
# Makefile for the ABC1600 window handler.
#

H = ../hsrc
G = ../../gdrv
P = o_portrait
L = o_landscape

SOURCES =	main1.c \
		main2.c \
		charhand.c \
		escgraph.c \
		graphhand.c \
		wincntrl.c \
		winfunc1.c \
		winfunc2.c \
		wingraph.c \
		winhand.c \
		setupgraph.c \
		savehand.c \
		mousehand1.c \
		mousehand2.c \
		paint.c \
		winsupport.c \
		$H/defs.h \
		$H/eglobals.h \
		$H/globals.h \
		$H/graphconst.h \
		$H/macros.h \
		$H/structs.h \
		$H/dnixmacros.h \
		$H/charcodes.h \
		$H/terminal.h \
		$H/gdriver.h

CSOURCES =	main1.c \
		main2.c \
		charhand.c \
		escgraph.c \
		graphhand.c \
		wincntrl.c \
		winfunc1.c \
		winfunc2.c \
		wingraph.c \
		winhand.c \
		setupgraph.c \
		savehand.c \
		mousehand1.c \
		mousehand2.c \
		paint.c \
		winsupport.c

POBJECTS =	$P/main1.o \
		$P/main2.o \
		$P/charhand.o \
		$P/escgraph.o \
		$P/graphhand.o \
		$P/savehand.o \
		$P/wincntrl.o \
		$P/winfunc1.o \
		$P/winfunc2.o \
		$P/wingraph.o \
		$P/winhand.o \
		$P/setupgraph.o \
		$P/mousehand1.o \
		$P/mousehand2.o \
		$P/paint.o \
		$P/winsupport.o

LOBJECTS =	$L/main1.o \
		$L/main2.o \
		$L/charhand.o \
		$L/escgraph.o \
		$L/graphhand.o \
		$L/savehand.o \
		$L/wincntrl.o \
		$L/winfunc1.o \
		$L/winfunc2.o \
		$L/wingraph.o \
		$L/winhand.o \
		$L/setupgraph.o \
		$L/mousehand1.o \
		$L/mousehand2.o \
		$L/paint.o \
		$L/winsupport.o

TARGETS =	whp \
		whl


#
# There are some constants which may be defined in order to control some
# things when compiling the window handler:
#
#   LANDSCAPE	-  A landscape mode version of the handler is generated.
#		   If not defined, a portrait mode version is generated.
#
#   NO_BUG_TRAP	-  The signals SIGILL, SIGBUS, and SIGSEGV are normally
#		   catched in order to try to clean up if an internal bug
#		   in the handler is encountered. This may not be desired
#		   when debugging the handler (no core dumps are generated).
#		   To not catch these signals, define this constant.
#
#   NS32	-  By default a version for an M68000 type of processor is
#		   generated. If this constant is defined, a version for an
#		   NS32000 type of processor is generated instead. In this
#		   case a couple of functions has to be written to read and
#		   write directly to the hardware.
#
# The macro USER can be used to define these constants in the command line,
# i.e.
#
#	make "USER = -DNS32"
#

LIBES = -lc

CFLAGS = -O $(USER)

LDFLAGS = -n -X

CC = cc
AS = as
LD = ld
MV = -mv
SIZE = -size
RM = -rm

PRINT = print
TOUCH = touch

LINT = lint
CAT = cat


#
# Compile both the portrait and landscape mode versions of the window handler.
#

all: $(TARGETS)


#
# Compile the portrait version of the window handler.
#

whp: $(POBJECTS)
	$(MV) whp whp.old
	$(LD)  $(LDFLAGS) $P/crt0.o  $(POBJECTS)  $(LIBES) -o $@
	$(SIZE) whp.old whp
	$(RM) whp.old

$P/main1.o: main1.c \
	    $H/defs.h \
	    $H/structs.h \
	    $H/globals.h \
	    $H/graphconst.h \
	    $H/macros.h \
	    $H/dnixmacros.h \
	    $H/terminal.h \
	    $H/gdriver.h \
	    $G/x35g.h \
	    $G/kybrdcodes.h \
	    $G/gpage.h
	$(CC) -c $(CFLAGS) main1.c
	$(MV) main1.o $P

$P/main2.o: main2.c \
	    $H/defs.h \
	    $H/structs.h \
	    $H/eglobals.h \
	    $H/macros.h \
	    $H/dnixmacros.h \
	    $H/terminal.h \
	    $H/gdriver.h \
	    $G/gpage.h
	$(CC) -c $(CFLAGS) main2.c
	$(MV) main2.o $P

$P/charhand.o: charhand.c \
	       $H/defs.h \
	       $H/structs.h \
	       $H/eglobals.h \
	       $H/graphconst.h \
	       $H/macros.h \
	       $H/charcodes.h \
	       $H/terminal.h \
	       $H/gdriver.h \
	       $G/kybrdcodes.h \
	       $G/gpage.h
	$(CC) -c $(CFLAGS) charhand.c
	$(MV) charhand.o $P

$P/escgraph.o: escgraph.c \
	       $H/defs.h \
	       $H/structs.h \
	       $H/eglobals.h \
	       $H/graphconst.h \
	       $H/macros.h \
	       $H/charcodes.h \
	       $H/terminal.h \
	       $H/gdriver.h \
	       $G/gpage.h
	$(CC) -c $(CFLAGS) escgraph.c 
	$(MV) escgraph.o $P

$P/graphhand.o: graphhand.c \
		$H/defs.h \
		$H/structs.h \
		$H/eglobals.h \
		$H/graphconst.h \
		$H/macros.h \
		$H/terminal.h \
		$H/gdriver.h \
		$G/gpage.h
	$(CC) -c $(CFLAGS) graphhand.c
	$(MV) graphhand.o $P

$P/savehand.o: savehand.c \
	       $H/defs.h \
	       $H/structs.h \
	       $H/eglobals.h \
	       $H/graphconst.h \
	       $H/macros.h \
	       $H/dnixmacros.h \
	       $H/terminal.h \
	       $H/gdriver.h \
	       $G/gpage.h
	$(CC) -c $(CFLAGS) savehand.c
	$(MV) savehand.o $P

$P/wincntrl.o: wincntrl.c \
	       $H/defs.h \
	       $H/structs.h \
	       $H/eglobals.h \
	       $H/graphconst.h \
	       $H/macros.h \
	       $H/terminal.h \
	       $H/gdriver.h \
	       $G/gpage.h \
	       $G/x35g.h
	$(CC) -c $(CFLAGS) wincntrl.c
	$(MV) wincntrl.o $P

$P/winfunc1.o: winfunc1.c \
	       $H/defs.h \
	       $H/structs.h \
	       $H/eglobals.h \
	       $H/graphconst.h \
	       $H/macros.h \
	       $H/dnixmacros.h \
	       $H/terminal.h \
	       $H/gdriver.h \
	       $G/gpage.h
	$(CC) -c $(CFLAGS) winfunc1.c
	$(MV) winfunc1.o $P

$P/winfunc2.o: winfunc2.c \
	       $H/defs.h \
	       $H/structs.h \
	       $H/eglobals.h \
	       $H/graphconst.h \
	       $H/macros.h \
	       $H/dnixmacros.h \
	       $H/terminal.h \
	       $H/gdriver.h \
	       $G/gpage.h
	$(CC) -c $(CFLAGS) winfunc2.c
	$(MV) winfunc2.o $P

$P/wingraph.o: wingraph.c \
	       $H/defs.h \
	       $H/structs.h \
	       $H/eglobals.h \
	       $H/graphconst.h \
	       $H/macros.h \
	       $H/terminal.h \
	       $H/gdriver.h \
	       $G/gpage.h
	$(CC) -c $(CFLAGS) wingraph.c
	$(MV) wingraph.o $P

$P/winhand.o: winhand.c \
	      $H/defs.h \
	      $H/structs.h \
	      $H/eglobals.h \
	      $H/graphconst.h \
	      $H/macros.h \
	      $H/terminal.h \
	      $H/gdriver.h \
	      $G/gpage.h
	$(CC) -c $(CFLAGS) winhand.c
	$(MV) winhand.o $P

$P/setupgraph.o: setupgraph.c \
		 $H/defs.h \
		 $H/structs.h \
		 $H/eglobals.h \
		 $H/graphconst.h \
		 $H/macros.h \
		 $H/terminal.h \
		 $H/gdriver.h \
		 $G/gpage.h
	$(CC) -c $(CFLAGS) setupgraph.c
	$(MV) setupgraph.o $P

$P/mousehand1.o: mousehand1.c \
		 $H/defs.h \
		 $H/structs.h \
		 $H/eglobals.h \
		 $H/graphconst.h \
		 $H/macros.h \
		 $H/dnixmacros.h \
		 $H/terminal.h \
		 $H/gdriver.h \
		 $G/gpage.h
	$(CC) -c $(CFLAGS) mousehand1.c
	$(MV) mousehand1.o $P

$P/mousehand2.o: mousehand2.c \
		 $H/defs.h \
		 $H/structs.h \
		 $H/eglobals.h \
		 $H/graphconst.h \
		 $H/macros.h \
		 $H/terminal.h \
		 $H/gdriver.h \
		 $G/gpage.h
	$(CC) -c $(CFLAGS) mousehand2.c
	$(MV) mousehand2.o $P

$P/paint.o: paint.c \
	    $H/defs.h \
	    $H/structs.h \
	    $H/eglobals.h \
	    $H/graphconst.h \
	    $H/macros.h \
	    $H/dnixmacros.h \
	    $H/terminal.h \
	    $H/gdriver.h \
	    $G/gpage.h
	$(CC) -c $(CFLAGS) paint.c
	$(MV) paint.o $P

$P/winsupport.o: winsupport.c \
		 $H/defs.h \
		 $H/structs.h \
		 $H/eglobals.h \
		 $H/graphconst.h \
		 $H/macros.h \
		 $H/dnixmacros.h \
		 $H/terminal.h \
		 $H/gdriver.h \
		 $G/gpage.h
	$(CC) -c $(CFLAGS) winsupport.c
	$(MV) winsupport.o $P


#
# Compile the landscape version of the window handler.
#

whl: $(LOBJECTS)
	$(MV) whl whl.old
	$(LD)  $(LDFLAGS) $L/crt0.o  $(LOBJECTS)  $(LIBES) -o $@
	$(SIZE) whl.old whl
	$(RM) whl.old

$L/main1.o: main1.c \
	    $H/defs.h \
	    $H/structs.h \
	    $H/globals.h \
	    $H/graphconst.h \
	    $H/macros.h \
	    $H/dnixmacros.h \
	    $H/terminal.h \
	    $H/gdriver.h \
	    $G/x35g.h \
	    $G/kybrdcodes.h \
	    $G/gpage.h
	$(CC) -DLANDSCAPE -c $(CFLAGS) main1.c
	$(MV) main1.o $L

$L/main2.o: main2.c \
	    $H/defs.h \
	    $H/structs.h \
	    $H/eglobals.h \
	    $H/macros.h \
	    $H/dnixmacros.h \
	    $H/terminal.h \
	    $H/gdriver.h \
	    $G/gpage.h
	$(CC) -DLANDSCAPE -c $(CFLAGS) main2.c
	$(MV) main2.o $L

$L/charhand.o: charhand.c \
	       $H/defs.h \
	       $H/structs.h \
	       $H/eglobals.h \
	       $H/graphconst.h \
	       $H/macros.h \
	       $H/charcodes.h \
	       $H/terminal.h \
	       $H/gdriver.h \
	       $G/kybrdcodes.h \
	       $G/gpage.h
	$(CC) -DLANDSCAPE -c $(CFLAGS) charhand.c
	$(MV) charhand.o $L

$L/escgraph.o: escgraph.c \
	       $H/defs.h \
	       $H/structs.h \
	       $H/eglobals.h \
	       $H/graphconst.h \
	       $H/macros.h \
	       $H/charcodes.h \
	       $H/terminal.h \
	       $H/gdriver.h \
	       $G/gpage.h
	$(CC) -DLANDSCAPE -c $(CFLAGS) escgraph.c
	$(MV) escgraph.o $L

$L/graphhand.o: graphhand.c \
		$H/defs.h \
		$H/structs.h \
		$H/eglobals.h \
		$H/graphconst.h \
		$H/macros.h \
		$H/terminal.h \
		$H/gdriver.h \
		$G/gpage.h
	$(CC) -DLANDSCAPE -c $(CFLAGS) graphhand.c
	$(MV) graphhand.o $L

$L/savehand.o: savehand.c \
	       $H/defs.h \
	       $H/structs.h \
	       $H/eglobals.h \
	       $H/graphconst.h \
	       $H/macros.h \
	       $H/dnixmacros.h \
	       $H/terminal.h \
	       $H/gdriver.h \
	       $G/gpage.h
	$(CC) -DLANDSCAPE -c $(CFLAGS) savehand.c
	$(MV) savehand.o $L

$L/wincntrl.o: wincntrl.c \
	       $H/defs.h \
	       $H/structs.h \
	       $H/eglobals.h \
	       $H/graphconst.h \
	       $H/macros.h \
	       $H/terminal.h \
	       $H/gdriver.h \
	       $G/gpage.h \
	       $G/x35g.h
	$(CC) -DLANDSCAPE -c $(CFLAGS) wincntrl.c
	$(MV) wincntrl.o $L

$L/winfunc1.o: winfunc1.c \
	       $H/defs.h \
	       $H/structs.h \
	       $H/eglobals.h \
	       $H/graphconst.h \
	       $H/macros.h \
	       $H/dnixmacros.h \
	       $H/terminal.h \
	       $H/gdriver.h \
	       $G/gpage.h
	$(CC) -DLANDSCAPE -c $(CFLAGS) winfunc1.c
	$(MV) winfunc1.o $L

$L/winfunc2.o: winfunc2.c \
	       $H/defs.h \
	       $H/structs.h \
	       $H/eglobals.h \
	       $H/graphconst.h \
	       $H/macros.h \
	       $H/dnixmacros.h \
	       $H/terminal.h \
	       $H/gdriver.h \
	       $G/gpage.h
	$(CC) -DLANDSCAPE -c $(CFLAGS) winfunc2.c
	$(MV) winfunc2.o $L

$L/wingraph.o: wingraph.c \
	       $H/defs.h \
	       $H/structs.h \
	       $H/eglobals.h \
	       $H/graphconst.h \
	       $H/macros.h \
	       $H/terminal.h \
	       $H/gdriver.h \
	       $G/gpage.h
	$(CC) -DLANDSCAPE -c $(CFLAGS) wingraph.c
	$(MV) wingraph.o $L

$L/winhand.o: winhand.c \
	      $H/defs.h \
	      $H/structs.h \
	      $H/eglobals.h \
	      $H/graphconst.h \
	      $H/macros.h \
	      $H/terminal.h \
	      $H/gdriver.h \
	      $G/gpage.h
	$(CC) -DLANDSCAPE -c $(CFLAGS) winhand.c
	$(MV) winhand.o $L

$L/setupgraph.o: setupgraph.c \
		 $H/defs.h \
		 $H/structs.h \
		 $H/eglobals.h \
		 $H/graphconst.h \
		 $H/macros.h \
		 $H/terminal.h \
		 $H/gdriver.h \
		 $G/gpage.h
	$(CC) -DLANDSCAPE -c $(CFLAGS) setupgraph.c
	$(MV) setupgraph.o $L

$L/mousehand1.o: mousehand1.c \
		 $H/defs.h \
		 $H/structs.h \
		 $H/eglobals.h \
		 $H/graphconst.h \
		 $H/macros.h \
		 $H/dnixmacros.h \
		 $H/terminal.h \
		 $H/gdriver.h \
		 $G/gpage.h
	$(CC) -DLANDSCAPE -c $(CFLAGS) mousehand1.c
	$(MV) mousehand1.o $L

$L/mousehand2.o: mousehand2.c \
		 $H/defs.h \
		 $H/structs.h \
		 $H/eglobals.h \
		 $H/graphconst.h \
		 $H/macros.h \
		 $H/terminal.h \
		 $H/gdriver.h \
		 $G/gpage.h
	$(CC) -DLANDSCAPE -c $(CFLAGS) mousehand2.c
	$(MV) mousehand2.o $L

$L/paint.o: paint.c \
	    $H/defs.h \
	    $H/structs.h \
	    $H/eglobals.h \
	    $H/graphconst.h \
	    $H/macros.h \
	    $H/dnixmacros.h \
	    $H/terminal.h \
	    $H/gdriver.h \
	    $G/gpage.h
	$(CC) -DLANDSCAPE -c $(CFLAGS) paint.c
	$(MV) paint.o $L

$L/winsupport.o: winsupport.c \
		 $H/defs.h \
		 $H/structs.h \
		 $H/eglobals.h \
		 $H/graphconst.h \
		 $H/macros.h \
		 $H/dnixmacros.h \
		 $H/terminal.h \
		 $H/gdriver.h \
		 $G/gpage.h
	$(CC) -DLANDSCAPE -c $(CFLAGS) winsupport.c
	$(MV) winsupport.o $L



#
# Print all source files.
#

print: Makefile $(SOURCES)
	$(PRINT) $?
	$(TOUCH) $@



#
# Check the source code using lint (portrait version).
#

lintp:
	$(LINT)  $(CSOURCES) >lintlog1 2>lintlog2
	$(CAT) lintlog1 lintlog2 >lintlog
	$(RM) lintlog1 lintlog2


#
# Check the source code using lint (landscape version).
#

lintl:
	$(LINT) -DLANDSCAPE $(CSOURCES) >lintlog1 2>lintlog2
	$(CAT) lintlog1 lintlog2 >lintlog
	$(RM) lintlog1 lintlog2


#
# Clean up.
#

cleanup:
	$(RM) $(TARGETS) $(POBJECTS) $(LOBJECTS)
