#
# Makefile for the window handler test programs.
#

FILES =	Makefile \
	bitmap.c \
	relative.c \
	scrback.c \
	visible.c \
	gettext.c \
	termio.c \
	grtest1.c \
	grtest2.c \
	grtest3.c \
	grtest4.c \
	paintspeed.c \
	arcspeed.c \
	stat.c \
	level.c \
	kysig.c \
	awopen.c \
	emovea.c \
	patterns.c \
	userbox.c \
	topgroup.c \
	spray.c

TARGETS =	bitmap \
		relative \
		scrback \
		visible \
		gettext \
		termio \
		grtest1 \
		grtest2 \
		grtest3 \
		grtest4 \
		paintspeed \
		arcspeed \
		stat \
		level \
		kysig \
		awopen \
		emovea \
		patterns \
		userbox \
		topgroup \
		spray

W = ../wincl/win

CFLAGS = -O -I../wincl

CC = cc

PRINT = print
TOUCH = touch

RM = -rm

all: $(TARGETS)

bitmap: bitmap.c
	$(CC) $(CFLAGS) bitmap.c -o $@

bitmap: $W/w_const.h
bitmap: $W/w_types.h
bitmap: $W/w_structs.h
bitmap: $W/w_macros.h

relative: relative.c
	$(CC) $(CFLAGS) relative.c -o $@

relative: $W/w_const.h
relative: $W/w_types.h
relative: $W/w_structs.h
relative: $W/w_macros.h

scrback: scrback.c
	$(CC) $(CFLAGS) scrback.c -o $@

visible: visible.c
	$(CC) $(CFLAGS) visible.c -o $@

visible: $W/w_const.h
visible: $W/w_types.h
visible: $W/w_structs.h
visible: $W/w_macros.h

gettext: gettext.c
	$(CC) $(CFLAGS) gettext.c -o $@

gettext: $W/w_const.h
gettext: $W/w_types.h
gettext: $W/w_structs.h
gettext: $W/w_macros.h


termio: termio.c
	$(CC) $(CFLAGS) termio.c -o $@


grtest1: grtest1.c
	$(CC) $(CFLAGS) grtest1.c -o $@


grtest2: grtest2.c
	$(CC) $(CFLAGS) grtest2.c -o $@


grtest3: grtest3.c
	$(CC) $(CFLAGS) grtest3.c -o $@


grtest4: grtest4.c
	$(CC) $(CFLAGS) grtest4.c -o $@


paintspeed: paintspeed.c
	$(CC) $(CFLAGS) paintspeed.c -o $@


arcspeed: arcspeed.c
	$(CC) $(CFLAGS) arcspeed.c -o $@


stat: stat.c
	$(CC) $(CFLAGS) stat.c -o $@


level: level.c
	$(CC) $(CFLAGS) level.c -o $@

level: $W/w_const.h
level: $W/w_types.h
level: $W/w_structs.h
level: $W/w_macros.h


kysig: kysig.c
	$(CC) $(CFLAGS) kysig.c -o $@

kysig: $W/w_const.h
kysig: $W/w_types.h
kysig: $W/w_structs.h
kysig: $W/w_macros.h


awopen: awopen.c
	$(CC) $(CFLAGS) awopen.c -o $@
awopen: $W/w_const.h
awopen: $W/w_types.h
awopen: $W/w_structs.h
awopen: $W/w_macros.h


emovea: emovea.c
	$(CC) $(CFLAGS) emovea.c -o $@


patterns: patterns.c
	$(CC) $(CFLAGS) patterns.c -o $@


userbox: userbox.c
	$(CC) $(CFLAGS) userbox.c -o $@
userbox: $W/w_const.h
userbox: $W/w_types.h
userbox: $W/w_structs.h
userbox: $W/w_macros.h


topgroup: topgroup.c
	$(CC) $(CFLAGS) topgroup.c -o $@
topgroup: $W/w_const.h
topgroup: $W/w_types.h
topgroup: $W/w_structs.h
topgroup: $W/w_macros.h


spray:spray.c
	$(CC) $(CFLAGS) spray.c -o $@



print: $(FILES)
	$(PRINT) $?
	$(TOUCH) $@


cleanup:
	$(RM) $(TARGETS)
