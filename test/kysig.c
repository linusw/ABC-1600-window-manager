/* kysig.c */

/*
 * This program opens and creates two windows. It echoes whatever is entered
 * from the keyboard in the opposit window. To do this, the facility to
 * receive a signal when there is something to read from the keyboard buffer
 * is used.
 */

/* 1985-06-26, Peter Andersson, Luxor Datorer AB */



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
	struct	winstruc  wd;
	struct	sgttyb	  tty;
	struct	kysigst	  kd;
	struct	headstruc hd;
	int		  i;
	int		  winsig0();
	int		  winsig1();

	wd.wp_xorig = 100;
	wd.wl_xorig = 100;
	wd.wp_yorig = 300;
	wd.wl_yorig = 300;
	wd.wp_xsize = 40;
	wd.wl_xsize = 40;
	wd.wp_ysize = 10;
	wd.wl_ysize = 10;
	wd.wp_vxorig = 1;
	wd.wl_vxorig = 1;
	wd.wp_vyorig = 1;
	wd.wl_vyorig = 1;
	wd.wp_vxsize = 40;
	wd.wl_vxsize = 40;
	wd.wp_vysize = 10;
	wd.wl_vysize = 10;
	wd.w_color = WHITE;
	wd.w_border = SSDSBORD;
	wd.wp_font = 'A';
	wd.wl_font = 'A';
	wd.w_tsig = 0;
	wd.w_ntsig = 0;
	wd.w_rsig = 0;
	wd.w_csig = SIGTERM;
	wd.w_boxes = BX_CLOS | BX_MOVE;
	wd.w_flags = PMODE | LMODE | SAVETEXT | TXTSIZE;
	tty.sg_flags = RAW;

	if (fork() != 0) {
		exit(0);
	}

	/*
	 * Set up the signals.
	 */
	signal(SIGTERM, SIG_DFL);
	signal(SIGINT, winsig0);
	signal(SIGQUIT, winsig1);

	/*
	 * Close the standard input and output.
	 */
	close(0);
	close(1);

	/*
	 * Change process group.
	 */
	if (setpgrp() < 0) {
		error("kysig: cannot change process group\n");
	}

	/*
	 * Open channels and create the windows.
	 */
	if (open(WMNTDIR, 2) != 0 || Wincreat(0, &wd) < 0 || wd.w_rstat != W_OK) {
		error("kysig: cannot open window 0\n");
	}
	wd.wp_xorig = 100;
	wd.wl_xorig = 100;
	wd.wp_yorig = 100;
	wd.wl_yorig = 100;
	wd.wp_xsize = 40;
	wd.wl_xsize = 40;
	wd.wp_ysize = 10;
	wd.wl_ysize = 10;
	wd.wp_vxorig = 1;
	wd.wl_vxorig = 1;
	wd.wp_vyorig = 1;
	wd.wl_vyorig = 1;
	wd.wp_vxsize = 40;
	wd.wl_vxsize = 40;
	wd.wp_vysize = 10;
	wd.wl_vysize = 10;
	wd.w_color = WHITE;
	wd.w_border = SSDSBORD;
	wd.wp_font = 'A';
	wd.wl_font = 'A';
	wd.w_tsig = 0;
	wd.w_ntsig = 0;
	wd.w_rsig = 0;
	wd.w_csig = SIGTERM;
	wd.w_boxes = BX_CLOS | BX_MOVE;
	wd.w_flags = PMODE | LMODE | SAVETEXT | TXTSIZE;
	if (open(WMNTDIR, 2) != 1 || Wincreat(1, &wd) < 0 || wd.w_rstat != W_OK) {
		error("kysig: cannot open window 1\n");
	}

	/*
	 * Insert headers in the two windows.
	 */
	hd.h_flags = H_INVTOP;
	hd.h_pad.h_xxx = 0;
	strcpy(&hd.h_hdr[0], " Window 0 ");
	Winheader(0, &hd);
	strcpy(&hd.h_hdr[0], " Window 1 ");
	Winheader(1, &hd);

	/*
	 * Set up the keyboard signals.
	 */
	kd.ks_sig = SIGINT;
	kd.ks_flags = 0;
	kd.ks_pad.ks_xxx = 0;
	if (Winkysig(0, &kd) < 0 || kd.ks_rstat != W_OK) {
		error("kysig: cannot set up keyboard signal 0\n");
	}
	kd.ks_sig = SIGQUIT;
	if (Winkysig(1, &kd) < 0 || kd.ks_rstat != W_OK) {
		error("kysig: cannot set up keyboard signal 1\n");
	}

	/*
	 * Close all open files, except file descriptors 0 and 1.
	 */
	for (i = 2 ; i < _NFILE ; i++) {
		close(i);
	}

	/*
	 * Set the windows to raw mode.
	 */
	ioctl(0, TIOCSETP, &tty);
	ioctl(1, TIOCSETP, &tty);

	/*
	 * Pause until a signal is received .....
	 */
	for ( ; ; ) {
		pause();
	}
}	/* end of main() */



/*
 * Buffer used to read keyboard.
 */

static	char	cbuf[256];



/*
 * Come here if anything to read from window 0.
 */

winsig0()
{
	int	cnt;

	signal(SIGINT, SIG_IGN);
	if (rdchk(0) > 0) {
		cnt = read(0, &cbuf[0], 256);
		if (cnt > 0) {
			write(1, &cbuf[0], cnt);
		}
	}
	signal(SIGINT, winsig0);

	return;
}	/* end of winsig0() */



/*
 * Come here if anything to read from window 1.
 */

winsig1()
{
	int	cnt;

	signal(SIGQUIT, SIG_IGN);
	if (rdchk(1) > 0) {
		cnt = read(1, &cbuf[0], 256);
		if (cnt > 0) {
			write(0, &cbuf[0], cnt);
		}
	}
	signal(SIGQUIT, winsig1);

	return;
}	/* end of winsig1() */



/*
 * Write an error message to the standard error output and then exit.
 */

error(s)
char	*s;
{
	write(STDERR, s, strlen(s));
	exit(1);
}	/* end of error() */
