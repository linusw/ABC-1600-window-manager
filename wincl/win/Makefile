#
# Makefile to install the header files in the directory /usr/include/win.
# Note: Only the super user can make the installation.
#

H = /usr/include/win

OWNER = bin
GROUP = bin
MODE = 0644

FILES =	Makefile \
	w_const.h \
	w_types.h \
	w_structs.h \
	w_macros.h \
	w_liberr.h

CP = cp
CHOWN = chown
CHGRP = chgrp
CHMOD = chmod

PRINT = print
TOUCH = touch


all:    $H/w_const.h \
	$H/w_types.h \
	$H/w_structs.h \
	$H/w_macros.h

$H/w_const.h: w_const.h
	$(CP) w_const.h $H
	$(CHOWN) $(OWNER) $H/w_const.h
	$(CHGRP) $(GROUP) $H/w_const.h
	$(CHMOD) $(MODE) $H/w_const.h

$H/w_types.h: w_types.h
	$(CP) w_types.h $H
	$(CHOWN) $(OWNER) $H/w_types.h
	$(CHGRP) $(GROUP) $H/w_types.h
	$(CHMOD) $(MODE) $H/w_types.h

$H/w_structs.h: w_structs.h
	$(CP) w_structs.h $H
	$(CHOWN) $(OWNER) $H/w_structs.h
	$(CHGRP) $(GROUP) $H/w_structs.h
	$(CHMOD) $(MODE) $H/w_structs.h

$H/w_macros.h: w_macros.h
	$(CP) w_macros.h $H
	$(CHOWN) $(OWNER) $H/w_macros.h
	$(CHGRP) $(GROUP) $H/w_macros.h
	$(CHMOD) $(MODE) $H/w_macros.h

$H/w_liberr.h: w_liberr.h
	$(CP) w_liberr.h $H
	$(CHOWN) $(OWNER) $H/w_liberr.h
	$(CHGRP) $(GROUP) $H/w_liberr.h
	$(CHMOD) $(MODE) $H/w_liberr.h


print: $(FILES)
	$(PRINT) $?
	$(TOUCH) $@
