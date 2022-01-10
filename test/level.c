/* level.c */

/*
 * This program open and creates a window. Each time the window becomes the
 * top level window or leaves the top level, an appropriate message is
 * displayed.
 */

/* 1985-04-12, Peter Andersson, Luxor Datorer AB */



#include	<stdio.h>
#undef		NULL
#include	<signal.h>
#include	<sgtty.h>
#undef		FIOCLEX
#undef		FIONCLEX
#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>

#define		STDERR		2		/* standard error output */



/*
 * The main function.
 */

main()
{
	struct	winstruc wd;
	struct	sgttyb	 tty;
	int		 i;
	int		 topsig();
	int		 ntopsig();

	wd.wp_xorig = 100;
	wd.wl_xorig = 100;
	wd.wp_yorig = 100;
	wd.wl_yorig = 100;
	wd.wp_xsize = 500;
	wd.wl_xsize = 500;
	wd.wp_ysize = 100;
	wd.wl_ysize = 100;
	wd.wp_vxorig = 0;
	wd.wl_vxorig = 0;
	wd.wp_vyorig = 0;
	wd.wl_vyorig = 0;
	wd.wp_vxsize = 500;
	wd.wl_vxsize = 500;
	wd.wp_vysize = 100;
	wd.wl_vysize = 100;
	wd.w_color = WHITE;
	wd.w_border = DLBORDER;
	wd.wp_font = 'A';
	wd.wl_font = 'A';
	wd.w_tsig = SIGINT;
	wd.w_ntsig = SIGQUIT;
	wd.w_rsig = 0;
	wd.w_csig = SIGTERM;
	wd.w_boxes = BX_CLOS | BX_SIZE | BX_MOVE | BX_HSCR | BX_VSCR;
	wd.w_flags = PMODE | LMODE | SAVETEXT;
	tty.sg_flags = RAW;

	if (fork() != 0) {
		exit(0);
	}

	/*
	 * Set up the signals.
	 */
	signal(SIGINT, topsig);
	signal(SIGQUIT, ntopsig);

	/*
	 * Close the standard input.
	 */
	close(0);

	/*
	 * Change process group, open a channel and create the window.
	 */
	if (setpgrp() < 0 || open(WMNTDIR, 2) != 0 ||
	    Wincreat(0, &wd) < 0 || wd.w_rstat != W_OK) {
		write(STDERR, "level: cannot open window\n", 26);
		exit(1);
	}

	/*
	 * Close all open files, except file descriptor 0.
	 */
	for (i = 1 ; i < _NFILE ; i++) {
		close(i);
	}

	/*
	 * Set the window to raw mode.
	 */
	ioctl(0, TIOCSETP, &tty);

	/*
	 * Pause until a signal is received .....
	 */
	for ( ; ; ) {
		pause();
	}
}	/* end of main() */



/*
 * Come here if we become the top level window.
 */

topsig()
{
	signal(SIGINT, SIG_IGN);
	write(0, "Top level window\n\r", 18);
	signal(SIGINT, topsig);

	return;
}	/* end of topsig() */



/*
 * Come here if we move from the top level.
 */

ntopsig()
{
	signal(SIGQUIT, SIG_IGN);
	write(0, "Moved from top level\n\r", 22);
	signal(SIGQUIT, ntopsig);

	return;
}	/* end of ntopsig() */
