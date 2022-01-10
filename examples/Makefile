#
# Makefile for the window examples.
#

FILES =	Makefile \
	w_ex1.c

TARGETS = w_ex1

W = ../wincl/win

CFLAGS = -O -I../wincl
CC = cc

PRINT = print
TOUCH = touch

RM = -rm


all: $(TARGETS)

w_ex1: w_ex1.c
	$(CC) $(CFLAGS) w_ex1.c -o $@

w_ex1: $W/w_const.h
w_ex1: $W/w_types.h
w_ex1: $W/w_structs.h
w_ex1: $W/w_macros.h


print: $(FILES)
	$(PRINT) $?
	$(TOUCH) $@


cleanup:
	$(RM) $(TARGETS)
