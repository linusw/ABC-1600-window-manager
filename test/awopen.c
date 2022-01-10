/* awopen.c */

/*
 * This command is similar to wopen but it also activates the window handler.
 * The syntax is:
 *
 *	awopen [-b] [-w] [-c <n>] [-r <n>] [-h <n>] [-w <n>] [-x <n>] [-y <n>]
 *	       [-f <c>] [-s <n>] [command]
 *
 * Explanation of the options:
 *   b - Black window.
 *   w - White window (this is the default).
 *   c - Number of character columns in the window (default 80).
 *   r - Number of character rows in the window (default 24).
 *   h - height of window in pixels.
 *   w - width of window in pixels.
 *   x - x coordinate of the lower left corner of the window (default 24 in
 *       portrait mode and 152 in landscape mode).
 *   y - y coordinate of the lower left corner of the window (default 344 in
 *       portrait mode and 216 in landscape mode).
 *   f - The standard font to be used (default 'A').
 *   s - Signal to be used to signal that the window has moved, etc.
 */

/* 1985-01-25, Peter Andersson, Luxor Datorer AB */



#include	<stdio.h>
#include	<signal.h>
#include	<fcntl.h>
#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>


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
	register		 color;	/* window background */
	register		 xsize;	/* x size of window in pixels */
	register		 ysize;	/* y size of window in pixels */
	int			 font;	/* font */
	register	char	 *str;
	register struct winstruc *wdp;	/* pointer to window data */
	int			 tmp;	/* temporary */
	static		char	 *dfltcmd[] = { "sh", NULL };

	/*
	 * Set up the default values.
	 */
	wdp = &wd;
	color = WHITE;
	xsize = 80;
	ysize = 24;
	wdp->w_boxes = BX_SIZE | BX_MOVE | BX_HSCR | BX_VSCR;
	wdp->w_flags = PMODE | LMODE | SAVETEXT | TXTSIZE;
	px = 24;
	lx = 152;
	py = 344;
	ly = 216;
	font = 'A';
	/* wdp->w_rsig = 0; */

	/*
	 * Decode all the options.
	 */
	while ((str = *++argv) != NULL && *str == '-') {
		if (*(++str + 1) != '\0') {
			illsyntax();	/* illegal syntax */
		}

		switch (*str) {
		case 'b':		/* black background */
			color = BLACK;
			break;

		case 'w':		/* white background or window width */
			if ((tmp = cnvarg(*(argv + 1))) <= 0) {
				color = WHITE;
			}
			else {
				xsize = tmp;
				wdp->w_flags &= ~TXTSIZE;
				argv++;
			}
			break;

		case 'h':		/* window height */
			if ((ysize = cnvarg(*++argv)) <= 0) {
				illsyntax();
			}
			wdp->w_flags &= ~TXTSIZE;
			break;

		case 'c':		/* number of columns */
			if ((xsize = cnvarg(*++argv)) <= 0) {
				illsyntax();
			}
			wdp->w_flags |= TXTSIZE;
			break;

		case 'r':		/* number of rows */
			if ((ysize = cnvarg(*++argv)) <= 0) {
				illsyntax();
			}
			wdp->w_flags |= TXTSIZE;
			break;

		case 'x':		/* x position of window */
			if ((px = lx = cnvarg(*++argv)) < 0) {
				illsyntax();
			}
			break;

		case 'y':		/* y position of window */
			if ((py = ly = cnvarg(*++argv)) < 0) {
				illsyntax();
			}
			break;

		case 'f':		/* font */
			if ((font = **++argv) < 'A' || font > 'Z' || *(*argv + 1) != '\0') {
				illsyntax();
			}
			break;

		case 's':		/* signal for redrawing */
			if ((wdp->w_rsig = cnvarg(*++argv)) < 0) {
				illsyntax();
			}
			break;

		default:
			illsyntax();
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
	wdp->w_border = DLBORDER;
	wdp->w_color = color;
	wdp->wp_font = wdp->wl_font = font;

	/*
	 * Activate the window handler.
	 */
	open(WACTIVATE, 2);

	/*
	 * Let the program fly.
	 */
	if ((px = fork()) != 0) {
		while ((lx = wait(&font)) != px && lx >= 0)
			;
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
	if (setpgrp() == -1 ||

	/*
	 * Open a channel for the new window.
	 */
	    open(WMNTDIR, 2) != 0 ||

	/*
	 * Set up a new controlling terminal.
	 */
	    fcntl(0, F_SETCT, 0) == -1 ||

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
	str = "awopen: can't execute command\n";
	write(2, str, strlen(str));
	sleep(3);

	exit(1);
}	/* end of main() */



/*
 * Illegal syntax, display the correct one.
 */

illsyntax()
{
	error("usage: awopen [-b] [-w] [-c <n>] [-r <n>] [-h <n>] [-w <n>] [-x <n>] [-y <n>]\n\
       [-f <c>] [-s <n>] [command]\n");
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

	if (s == NULL) {
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
