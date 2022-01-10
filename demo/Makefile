#
# Makefile for the window demo programs.
#

FILES =	Makefile \
	chbg.c \
	draw.c \
	stars.c

TARGETS = chbg \
	  draw \
	  stars


W = ../wincl/win

CFLAGS = -O -I../wincl
CC = cc

PRINT = print
TOUCH = touch

RM = -rm


all: $(TARGETS)

chbg: chbg.c
	$(CC) $(CFLAGS) chbg.c -o $@

chbg: $W/w_const.h
chbg: $W/w_types.h
chbg: $W/w_structs.h
chbg: $W/w_macros.h

draw: draw.c
	$(CC) $(CFLAGS) draw.c -o $@

draw: $W/w_const.h
draw: $W/w_types.h
draw: $W/w_structs.h
draw: $W/w_macros.h

stars: stars.c
	$(CC) $(CFLAGS) stars.c -o $@


print: $(FILES)
	$(PRINT) $?
	$(TOUCH) $@


cleanup:
	$(RM) $(TARGETS)
