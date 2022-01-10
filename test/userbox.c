/* userbox.c */

/*
 * This is a test program intented to test the user defined box facility of
 * the window handler.
 * The syntax is:
 *
 *	userbox [<rows>]
 *
 * Where <rows> is the number of rows in the window (default 35).
 */

/* 1985-07-21, Peter Andersson, Luxor Datorer AB */


#include	<stdio.h>
#include	<signal.h>
#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>

#define		STDERR		2	/* standard error output */



/*
 * Global variables.
 */

int		 wfd;		/* file descriptor of the window */
struct	winstruc wd;		/* used to create a window */
struct	zoomlst	 zd;		/* used to add a zoom list */
struct	userbox	 ubd[] = {	/* user defined boxes */
	{
		{		/* the bit pattern for a 1 */
			0xffff,
			0x8001,
			0x8001,
			0x8081,

			0x8181,
			0x8281,
			0x8081,
			0x8081,

			0x8081,
			0x8081,
			0x8081,
			0x8081,

			0x83e1,
			0x8001,
			0x8001,
			0xffff
		},
		1,		/* signal number */
		0,
		0
	},
	{
		{		/* bit pattern for a 2 */
			0xffff,
			0x8001,
			0x8001,
			0x81c1,

			0x8221,
			0x8421,
			0x8021,
			0x8041,

			0x8081,
			0x8101,
			0x8201,
			0x8401,

			0x87e1,
			0x8001,
			0x8001,
			0xffff
		},
		2,		/* signal number */
		0,
		0
	},
	{
		{		/* bit pattern for a 3 */
			0xffff,
			0x8001,
			0x8001,
			0x83c1,

			0x8421,
			0x8021,
			0x8021,
			0x81c1,

			0x8021,
			0x8021,
			0x8021,
			0x8421,

			0x83c1,
			0x8001,
			0x8001,
			0xffff
		},
		3,		/* signal number */
		0,
		0
	},
	{
		{		/* bit pattern for a 4 */
			0xffff,
			0x8001,
			0x8001,
			0x8021,

			0x8061,
			0x80a1,
			0x8121,
			0x8221,

			0x8421,
			0x8ff1,
			0x8021,
			0x8021,

			0x8071,
			0x8001,
			0x8001,
			0xffff
		},
		4,		/* signal number */
		0,
		0
	},
	{
		{		/* bit pattern for a 5 */
			0xffff,
			0x8001,
			0x8001,
			0x87e1,

			0x8401,
			0x8401,
			0x8401,
			0x87c1,

			0x8021,
			0x8021,
			0x8021,
			0x8421,

			0x83c1,
			0x8001,
			0x8001,
			0xffff
		},
		5,		/* signal number */
		0,
		0
	},
	{
		{		/* bit pattern for a 6 */
			0xffff,
			0x8001,
			0x8001,
			0x83c1,

			0x8421,
			0x8401,
			0x8401,
			0x85c1,

			0x8621,
			0x8421,
			0x8421,
			0x8421,

			0x83c1,
			0x8001,
			0x8001,
			0xffff
		},
		6,		/* signal number */
		0,
		0
	}
};



/*
 * The main function.
 */

main(argc, argv)
int	argc;		/* argument count */
char	**argv;		/* pointer to arguments */
{
	int	i;		/* loop index */
	int	rows;		/* no. of rows in virtual screen */
	int	usersig();	/* signal handling routine */

	/*
	 * Decode possible argument.
	 */
	rows = 35;
	if (argc > 2 || (argc == 2 && (rows = cnvarg(*(argv + 1))) == 0)) {
		error("usage: userbox [<rows>]\n");
	}

	/*
	 * Change process group.
	 */
	if (setpgrp() < 0) {
		error("userbox: failed to change process group\n");
	}

	/*
	 * Open and create a window.
	 */
	wd.wp_xorig = 100;
	wd.wl_xorig = 100;
	wd.wp_yorig = 100;
	wd.wl_yorig = 100;
	wd.wp_xsize = 40;
	wd.wl_xsize = 40;
	wd.wp_ysize = rows;
	wd.wl_ysize = rows;
	wd.wp_vxorig = 1;
	wd.wl_vxorig = 1;
	wd.wp_vyorig = 1;
	wd.wl_vyorig = 1;
	wd.wp_vxsize = 40;
	wd.wl_vxsize = 40;
	wd.wp_vysize = rows;
	wd.wl_vysize = rows;
	wd.w_color = WHITE;
	wd.w_border = DLBORDER;
	wd.wp_font = 'A';
	wd.wl_font = 'A';
	wd.w_uboxes = 6;
	wd.w_tsig = 0;
	wd.w_ntsig = 0;
	wd.w_rsig = 0;
	wd.w_csig = SIGKILL;
	wd.w_boxes = BX_HSCR | BX_VSCR | BX_CLOS | BX_SIZE | BX_MOVE | BX_ZOOM | BX_BLOW | BX_USER;
	wd.w_flags = PMODE | LMODE | SAVETEXT | TXTSIZE;
	wd.w_pad.w_xxx = 0;
	if ((wfd = open(WMNTDIR, 2)) < 0 || Wincreat(wfd, &wd) < 0 || wd.w_rstat != W_OK) {
		error("userbox: failed to open window\n");
	}

	/*
	 * Set up a zoom list.
	 */
	zd.zp_list[0] = 'B';
	zd.zl_list[0] = 'B';
	zd.zp_list[1] = 'Q';
	zd.zl_list[1] = 'Q';
	zd.zp_list[2] = '\0';
	zd.zl_list[2] = '\0';
	zd.z_flags = Z_PMODE | Z_LMODE;
	zd.z_pad.z_xxx = 0;
	if (Winzoom(wfd, &zd) < 0 || zd.z_rstat != W_OK) {
		error("userbox: failed to set up zoom list\n");
	}

	/*
	 * Now set up all the user defined boxes.
	 */
	for (i = 0 ; i < 6 ; i++) {
		signal(i + 1, usersig);
		if (Winubox(wfd, &ubd[i]) < 0 || ubd[i].bx_rstat != W_OK) {
			error("userbox: failed to set up a user box\n");
		}
	}

	/*
	 * Fork.
	 */
	if (fork() != 0) {
		exit(0);
	}

	/*
	 * Close all files, except the window.
	 */
	for (i = 0 ; i < _NFILE ; i++) {
		if (i != wfd) {
			close(i);
		}
	}

	/*
	 * Now wait for signals to come in.
	 */
	for ( ; ; ) {
		pause();
	}
}	/* end of main() */



/*
 * Come here when any of the user defined boxes has been used.
 */

usersig(sig)
int	sig;		/* received signal */
{
	static	char	msg[] = "User box number 0 used.\n";

	signal(sig, SIG_IGN);
	msg[16] = sig + '0';
	write(wfd, &msg[0], 24);
	signal(sig, usersig);

	return;
}	/* end of usersig() */



/*
 * Write a error to the standard error output and then exit.
 */

error(s)
char	*s;
{
	write(STDERR, s, strlen(s));
	exit(1);
}	/* end of error() */



/*
 * Convert a string of decimal digits to binary. Returns 0 if error, otherwise
 * a positive number.
 */

int
cnvarg(cp)
char	*cp;
{
	int	val;

	val = 0;
	while (*cp != '\0') {
		if (*cp < '0' || *cp > '9') {
			return(0);
		}
		val = 10 * val + *cp++ - '0';
	}

	return(val);
}	/* end of cnvarg() */
