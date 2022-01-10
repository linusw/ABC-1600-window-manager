#
# Makefile to update the most things concerning the window handler.
#

PRINT = print
TOUCH = touch


compile:
	cd hnd/whgo ; make
	cd hnd/csrc ; make
	cd wsh/wsh ; make
	cd wsh/wshpp ; make
	cd wsh/wshdis ; make
	cd demo ; make
	cd test ; make
	cd wlib ; make
	cd wcmds ; make
	cd examples ; make


ccheck:
	cd hnd/whgo ; make -n
	cd hnd/csrc ; make -n
	cd wsh/wsh ; make -n
	cd wsh/wshpp ; make -n
	cd wsh/wshdis ; make -n
	cd demo ; make -n
	cd test ; make -n
	cd wlib ; make -n
	cd wcmds ; make -n
	cd examples ; make -n


head:
	@echo "You must be super user do do this!"
	cd wincl/win ; make all


hcheck:
	cd wincl/win ; make -n


lib:
	@-echo "You must be super user to do this!"
	cd wlib ; make install


lcheck:
	cd wlib ; make -n install


#
# Print all recently changed files.
#

printall:
	make print
	cd hnd/csrc ; make print
	cd demo ; make print
	cd install ; make print
	cd doc ; make print
	cd test ; make print
	cd hnd/whgo ; make print
	cd wincl/win ; make print
	cd wlib ; make print
	cd wsh/wsh ; make print
	cd wsh/wshpp ; make print
	cd wsh/wshdis ; make print
	cd wsh/wd ; make print
	cd wsh/st ; make print
	cd wcmds ; make print
	cd examples ; make print


pcheck:
	make -n print
	cd hnd/csrc ; make -n print
	cd demo ; make -n print
	cd install ; make -n print
	cd doc ; make -n print
	cd test ; make -n print
	cd hnd/whgo ; make -n print
	cd wincl/win ; make -n print
	cd wlib ; make -n print
	cd wsh/wsh ; make -n print
	cd wsh/wshpp ; make -n print
	cd wsh/wshdis ; make -n print
	cd wsh/wd ; make -n print
	cd wsh/st ; make -n print
	cd wcmds ; make -n print
	cd examples ; make -n print


cleanup:
	cd hnd/whgo ; make cleanup
	cd hnd/csrc ; make cleanup
	cd wsh/wsh ; make cleanup
	cd wsh/wshpp ; make cleanup
	cd wsh/wshdis ; make cleanup
	cd demo ; make cleanup
	cd test ; make cleanup
	cd wlib ; make cleanup
	cd wcmds ; make cleanup
	cd examples ; make cleanup


cleanchk:
	cd hnd/whgo ; make -n cleanup
	cd hnd/csrc ; make -n cleanup
	cd wsh/wsh ; make -n cleanup
	cd wsh/wshpp ; make -n cleanup
	cd wsh/wshdis ; make -n cleanup
	cd demo ; make -n cleanup
	cd test ; make -n cleanup
	cd wlib ; make -n cleanup
	cd wcmds ; make -n cleanup
	cd examples ; make -n cleanup


print: Makefile READ_ME Wishlist Mods_since_1.0
	$(PRINT) $?
	$(TOUCH) $@
