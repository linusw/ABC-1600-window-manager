#
# Makefile to print all files containing data for single structures.
# These files must first be processed by the window shell preprocessor using
# the '-x' option.
#

FILES = Makefile \
	bg_abc1600.st \
	bg_dots.st \
	bg_net.st \
	mp_simple.st \
	mp_gun.st \
	mp_darrow.st \
	mp_small.st


PRINT = print
TOUCH = touch


print: $(FILES)
	$(PRINT) $?
	$(TOUCH) $@
