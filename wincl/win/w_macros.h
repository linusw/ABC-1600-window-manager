/* w_macros.h */

/*
 * This file contains macros used to access the ABC1600 window handler.
 */

/* 1985-07-23, Luxor Datorer AB */


#define		W_MACROS

#ifndef		F_IOCR
#include	<dnix/fcodes.h>
#endif
#ifndef		W_CONST
#include	<win/w_const.h>
#endif
#ifndef		W_TYPES
#include	<win/w_types.h>
#endif
#ifndef		W_STRUCTS
#include	<win/w_structs.h>
#endif



/*
 * Create a window.
 */

#define	Wincreat(fd, bp)	dnix(F_IOCR, fd, bp, sizeof(struct winstruc), WINCREAT, 0, 0, 0)



/*
 * Move window to top level.
 */

#define	Winlevel(fd, bp)	dnix(F_IOCR, fd, bp, sizeof(struct winlevel), WINLEVEL, 0, 0, 0)



/*
 * Move window to top level inside group.
 */

#define	Win1lev(fd, bp)		dnix(F_IOCR, fd, bp, sizeof(struct winlevel), WIN1LEV, 0, 0, 0)



/*
 * Alter a window.
 */

#define	Winalter(fd, bp)	dnix(F_IOCR, fd, bp, sizeof(struct winstruc), WINALTER, 0, 0, 0)



/*
 * Alter a father window without affecting child windows.
 */

#define	Win1alter(fd, bp)	dnix(F_IOCR, fd, bp, sizeof(struct winstruc), WIN1ALTER, 0, 0, 0)



/*
 * Set up a new default size and location for a window.
 */

#define	Windflsz(fd, bp)	dnix(F_IOCR, fd, bp, sizeof(struct winstruc), WINDFLSZ, 0, 0, 0)



/*
 * Get the status of a window.
 */

#define	Winstat(fd, bp)		dnix(F_IOCR, fd, bp, sizeof(struct winstruc), WINSTAT, 0, 0, 0)



/*
 * Insert a header in a window.
 */

#define	Winheader(fd, bp)	dnix(F_IOCR, fd, bp, sizeof(struct headstruc), WINHEADER, 0, 0, 0)



/*
 * Add an icon.
 */

#define	Winicon(fd, bp)		dnix(F_IOCR, fd, bp, sizeof(struct winicon), WINICON, 0, 0, 0)



/*
 * Remove all icons in a window.
 */

#define	Rmicons(fd)		dnix(F_IOCR, fd, 0, 0, RMICONS, 0, 0, 0)



/*
 * Alter the flags for a window.
 */

#define	Winflags(fd, bp)	dnix(F_IOCR, fd, bp, sizeof(struct flgstruc), WINFLAGS, 0, 0, 0)



/*
 * Set up substitute keys to simulate the mouse.
 */

#define	Winmsub(fd, bp)		dnix(F_IOCR, fd, bp, sizeof(struct substit), WINMSUB, 0, 0, 0)



/*
 * Alter the background pattern.
 */

#define	Winchbg(fd, bp)		dnix(F_IOCR, fd, bp, sizeof(struct chbgstruc), WINCHBG, 0, 0, 0)



/*
 * Get the visible parts of a window.
 */

#define	Wingetvis(fd, bp, bc)	dnix(F_IOCR, fd, bp, bc, WINGETVIS, 0, 0, 0)



/*
 * Set to inverse video.
 */

#define	Winivideo(fd)		dnix(F_IOCR, fd, 0, 0, WINIVIDEO, 0, 0, 0)



/*
 * Set to non-inverse video.
 */

#define	Winnvideo(fd)		dnix(F_IOCR, fd, 0, 0, WINNVIDEO, 0, 0, 0)



/*
 * Make cursor visible.
 */

#define	Wincurvis(fd)		dnix(F_IOCR, fd, 0, 0, WINCURVIS, 0, 0, 0)



/*
 * Change mouse pointer.
 */

#define	Winchmpnt(fd, bp)	dnix(F_IOCR, fd, bp, sizeof(struct npstruc), WINCHMPNT, 0, 0, 0)



/*
 * Get the number of open and created windows.
 */

#define	Wincnt(fd, bp)		dnix(F_IOCR, fd, bp, sizeof(struct nwstruc), WINCNT, 0, 0, 0)



/*
 * Restore the screen.
 */

#define	Winrestor(fd)	dnix(F_IOCR, fd, 0, 0, WINRESTOR, 0, 0, 0)



/*
 * Read the text contents of a virtual screen.
 */

#define	Wingettxt(fd, bp, bc)	dnix(F_IOCR, fd, bp, bc, WINGETTXT, 0, 0, 0)



/*
 * Test if the window handler is present.
 */

#define	Wintest(fd)	dnix(F_IOCR, fd, 0, 0, WINTEST, 0, 0, 0)



/*
 * Set the initial terminal and tty driver parameters for windows.
 */

#define	Winsinit(fd, bp)	dnix(F_IOCR, fd, bp, sizeof(struct wininit), WINSINIT, 0, 0, 0)



/*
 * Get the initial terminal and tty driver parameters for windows.
 */

#define	Winginit(fd, bp)	dnix(F_IOCR, fd, bp, sizeof(struct wininit), WINGINIT, 0, 0, 0)



/*
 * Set up a new zoom list.
 */

#define	Winzoom(fd, bp)		dnix(F_IOCR, fd, bp, sizeof(struct zoomlst), WINZOOM, 0, 0, 0)



/*
 * Change to a new default font.
 */

#define	Winndchr(fd, bp)	dnix(F_IOCR, fd, bp, sizeof(struct dfltchr), WINNDCHR, 0, 0, 0)



/*
 * Turn the screen.
 */

#define	Winturn(fd, bp)		dnix(F_IOCR, fd, bp, sizeof(struct modstruc), WINTURN, 0, 0, 0)



/*
 * Get the current screen mode.
 */

#define	Winmode(fd, bp)		dnix(F_IOCR, fd, bp, sizeof(struct modstruc), WINMODE, 0, 0, 0)



/*
 * Read the picture memory for a window or the whole screen.
 */

#define	Wpictrd(fd, bp, bc)	dnix(F_IOCR, fd, bp, bc, WPICTRD, 0, 0, 0)



/*
 * Set up a signal to be sent when there is something to read from the keyboard
 * buffer.
 */

#define	Winkysig(fd, bp)	dnix(F_IOCR, fd, bp, sizeof(struct kysigst), WINKYSIG, 0, 0, 0)



/*
 * Add a user defined box.
 */

#define	Winubox(fd, bp)		dnix(F_IOCR, fd, bp, sizeof(struct userbox), WINUBOX, 0, 0, 0)



/*
 * Alter the help report sequence.
 */

#define	Winhelp(fd, bp)		dnix(F_IOCR, fd, bp, sizeof(struct helpst), WINHELP, 0, 0, 0)



/*
 * Set up a new spray mask.
 */

#define	Spraymask(fd, bp)	dnix(F_IOCR, fd, bp, sizeof(struct sprayst), PSPRAY, 0, 0, 0)
