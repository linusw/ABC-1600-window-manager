/* wopen.c */

/*
 * This command creates a new window with the status of a terminal and
 * executes the command given as argument in it (if no command is specified,
 * a shell is executed).
 * The syntax is:
 *
 *	wopen [-notbwz] [-c <n>] [-r <n>] [-h <n>] [-w <n>] [-x <n>] [-y <n>]
 *	      [-f <c>] [-s <n>] [-e <n>] [command]
 *
 * Explanation of the options:
 *   b - Black window.
 *   w - White window (this is the default).
 *   n - No window border.
 *   o - Single (one) line window border.
 *   t - Double (two) lines window border (this is the default).
 *   z - Zoom box shall be present in the border.
 *   c - Number of character columns in the window (default 80).
 *   r - Number of character rows in the window (default 24).
 *   h - height of window in pixels.
 *   w - width of window in pixels.
 *   x - x coordinate of the lower left corner of the window (default 24 in
 *       portrait mode and 152 in landscape mode).
 *   y - y coordinate of the lower left corner of the window (default 344 in
 *       portrait mode and 216 in landscape mode).
 *   f - The default font to be used (default 'A').
 *   s - Signal to be used to signal that the window has moved, etc.
 *       (default 0).
 *   e - Signal to be sent when the close box is used. If not zero, a close
 *       (exit) box will be present in the border (default 0).
 */

/* 1985-01-25, Peter Andersson, Luxor Datorer AB */



#include	<stdio.h>
#include	<signal.h>
#include	<fcntl.h>
#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>

#define		NO		0
#define		YES		1


/*
 * Structure used to create the window.
 */

struct	winstruc wd;



/*
 * The main function.
 */

main(argc, argv)
int		argc;		/* number of arguments */
register char	**argv;		/* pointer to the arguments */
{
	register		 px;	/* x position, portrait of window and various uses */
	register		 lx;	/* x position, landscape of window and various uses */
	register		 py;	/* y position, portrait of window and various uses */
	int			 ly;	/* y position, landscape of window and various uses */
	register		 xsize;	/* x size of window in pixels */
	register		 ysize;	/* y size of window in pixels */
	register		 font;	/* font */
	register	char	 *str;
	register	char	 *str2;
	register struct winstruc *wdp;	/* pointer to window data */
	int			 tmp;	/* temporary */
	static		char	 *dfltcmd[] = { "sh", NULL };

	/*
	 * Set up the default values.
	 */
	wdp = &wd;
	wdp->w_color = WHITE;
	xsize = 80;
	ysize = 24;
	wdp->w_boxes = BX_SIZE | BX_MOVE | BX_HSCR | BX_VSCR;
	wdp->w_flags = PMODE | LMODE | SAVETEXT | TXTSIZE;
	px = 24;
	lx = 152;
	py = 344;
	ly = 216;
	font = 'A';
	wdp->w_border = DLBORDER;
	/* wdp->w_rsig = 0; */
	/* wdp->w_csig = 0; */

	/*
	 * Decode all the options.
	 */
	while ((str = *++argv) != NULL && *str == '-') {
		while (*++str != '\0') {
			if (*str != 'b' && *str != 'w' && *str != 'n' &&
			    *str != 'o' && *str != 't' && *str != 'z' &&
			    *(str + 1) != '\0') {
				illsyntax();	/* illegal syntax */
			}

			switch (*str) {

			case 'n':	/* no border */
				wdp->w_border = NOBORDER;
				break;

			case 'o':	/* single line border */
				wdp->w_border = SLBORDER;
				break;

			case 't':	/* double line border */
				wdp->w_border = DLBORDER;
				break;

			case 'z':	/* zoom box shall be present */
				wdp->w_boxes |= BX_ZOOM;
				break;

			case 'b':	/* black background */
				wdp->w_color = BLACK;
				break;

			case 'w':	/* white background or window width */
				if (*(str + 1) != '\0' || (tmp = cnvarg(*(argv + 1))) < 0) {
					wdp->w_color = WHITE;
				}
				else {
					xsize = tmp;
					wdp->w_flags &= ~TXTSIZE;
					argv++;
				}
				break;

			case 'h':	/* window height */
				if ((ysize = cnvarg(*++argv)) <= 0) {
					illsyntax();
				}
				wdp->w_flags &= ~TXTSIZE;
				break;

			case 'c':	/* number of columns */
				if ((xsize = cnvarg(*++argv)) <= 0) {
					illsyntax();
				}
				wdp->w_flags |= TXTSIZE;
				break;

			case 'r':	/* number of rows */
				if ((ysize = cnvarg(*++argv)) <= 0) {
					illsyntax();
				}
				wdp->w_flags |= TXTSIZE;
				break;

			case 'x':	/* x position of window */
				tmp = NO;	/* sign flag */
				str2 = *++argv;
				if (str2 != NULL && *str2 == '-') {
					tmp = YES;
					str2++;
				}
				if ((lx = cnvarg(str2)) < 0) {
					illsyntax();
				}
				if (tmp != NO) {
					lx = -lx;
				}
				px = lx;
				break;

			case 'y':	/* y position of window */
				tmp = NO;	/* sign flag */
				str2 = *++argv;
				if (str2 != NULL && *str2 == '-') {
					tmp = YES;
					str2++;
				}
				if ((ly = cnvarg(str2)) < 0) {
					illsyntax();
				}
				if (tmp != NO) {
					ly = -ly;
				}
				py = ly;
				break;

			case 'f':	/* font */
				if ((font = **++argv) < 'A' || font > 'Z' || *(*argv + 1) != '\0') {
					illsyntax();
				}
				break;

			case 's':	/* signal for redrawing */
				if ((wdp->w_rsig = cnvarg(*++argv)) < 0) {
					illsyntax();
				}
				break;

			case 'e':	/* close box signal */
				if ((wdp->w_csig = cnvarg(*++argv)) < 0) {
					illsyntax();
				}
				wdp->w_boxes &= ~BX_CLOS;
				if (wdp->w_csig != 0) {
					wdp->w_boxes |= BX_CLOS;
				}
				break;

			default:
				illsyntax();
			}
		}
	}
	if (str == NULL) {
		argv = dfltcmd;		/* no arguments left for the command */
	}

	/*
	 * Now fill in the structure used to create the window. The fact that
	 * the structure is initialized to zeros is used.
	 */
	wdp->wp_xorig = px;
	wdp->wl_xorig = lx;
	wdp->wp_yorig = py;
	wdp->wl_yorig = ly;
	wdp->wp_xsize = wdp->wp_vxsize = xsize;
	wdp->wl_xsize = wdp->wl_vxsize = xsize;
	wdp->wp_ysize = wdp->wp_vysize = ysize;
	wdp->wl_ysize = wdp->wl_vysize = ysize;
	if (wdp->w_flags & TXTSIZE) {
		wdp->wp_vxorig = wdp->wl_vxorig = wdp->wp_vyorig = wdp->wl_vyorig = 1;
	}
	wdp->wp_font = wdp->wl_font = font;

	/*
	 * Let the program fly.
	 */
	if (fork() != 0) {
		exit(0);
	}

	/*
	 * Set all signals to default.
	 */
	for (px = 1 ; px < NSIG ; px++) {
		signal(px, SIG_DFL);
	}

	/*
	 * Close all open files.
	 */
	for (px = 0 ; px < _NFILE ; px++) {
		close(px);
	}

	/*
	 * Set up a new process group (equal to the process id).
	 */
	if (setpgrp() < 0 ||

	/*
	 * Open a channel for the new window.
	 */
	    open(WMNTDIR, 2) != 0 ||

	/*
	 * Set up a new controlling terminal.
	 */
	    fcntl(0, F_SETCT, 0) < 0 ||

	/*
	 * Do two dup's to create the standard output and error output.
	 */
	    dup(0) != 1 || dup(0) != 2 ||

	/*
	 * Create the window.
	 */
	    Wincreat(0, wdp) < 0 || wdp->w_rstat != W_OK) {
		exit(1);
	}

	/*
	 * Execute the desired command.
	 */
	execvp(*argv, argv);

	/*
	 * The execution failed.
	 */
	str = "wopen: cannot execute command\n";
	write(2, str, strlen(str));
	sleep(3);

	exit(1);
}	/* end of main() */



/*
 * Illegal syntax, display the correct one.
 */

illsyntax()
{
	error("usage: wopen [-notbwz] [-c <n>] [-r <n>] [-h <n>] [-w <n>] [-x <n>] [-y <n>]\n\
       [-f <c>] [-s <n>] [-e <n>] [command]\n");
}	/* end of illsyntax() */



/*
 * Display an error message on the standard output and the exit.
 */

error(s)
char	*s;
{
	write(2, s, strlen(s));
	exit(1);
}	/* error() */



/*
 * Convert a positive decimal ASCII string to binary.
 * Returns -1 if error or null pointer, otherwise the binary value.
 */

int
cnvarg(s)
register char	*s;
{
	register	val;		/* returned value */

	if (s == NULL || *s == '\0') {
		return(-1);
	}

	val = 0;
	while (*s != '\0') {
		if (*s < '0' || *s > '9') {
			return(-1);
		}
		val = 10 * val + *s++ - '0';
	}

	return(val);
}	/* end of cnvarg() */
