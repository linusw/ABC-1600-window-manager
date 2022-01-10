#
# Makefile for the window handler startup program.
#

H = ../hsrc
G = ../../gdrv

CC = cc
CFLAGS = -O -n

PRINT = print
TOUCH = touch

RM = -rm


whgo: whgo.c \
      $H/defs.h \
      $H/graphconst.h \
      $H/structs.h \
      $H/macros.h \
      $H/dnixmacros.h \
      $G/x35g.h \
      $G/kybrdcodes.h
	$(CC) $(CFLAGS) whgo.c -o $@


print: Makefile whgo.c
	$(PRINT) $?
	$(TOUCH) $@


cleanup:
	$(RM) whgo
