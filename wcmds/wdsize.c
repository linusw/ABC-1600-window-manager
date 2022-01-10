/* wdsize.c */

/*
 * This command sets up a new default size and location for a window. If no
 * arguments are specified, the current size and location of the window will
 * become the default one.
 * The syntax is:
 *
 *	wdsize [-t] [-x <n>] [-y <n>] [-u <n>] [-v <n>] [-w <n>] [-h <n>]
 *
 * Explanation of the options:
 *   t - The parameters are given in units of font boxes.
 *   x - The lower left corner of the virtual screen (x coordinate).
 *   y - The lower left corner of the virtual screen (y coordinate).
 *   u - The lower left corner of the window (x coordinate).
 *   v - The lower left corner of the window (y coordinate).
 *   w - Width of the window.
 *   h - Height of the window. 
 */

/* 1985-07-18, Peter Andersson, Luxor Datorer AB */



#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>

#define		NO		0
#define		YES		1
#define		NULL		0	/* null pointer */
#define		STDIN		0	/* standard input */
#define		STDERR		2	/* standard error output */


/*
 * Structure used to set up new default size and location.
 */

struct	winstruc wd;



/*
 * The main function.
 */

main(argc, argv)
int		argc;		/* number of arguments */
register char	**argv;		/* pointer to the arguments */
{
	register	char	 *str;
	register	char	 *str2;
	register struct winstruc *wdp;	/* pointer to window data */
	register		 val;
	register		 tmp;	/* temporary */

	/*
	 * Set up the default values.
	 */
	wdp = &wd;
	if (Winstat(STDIN, wdp) < 0 || wdp->w_rstat != W_OK) {
		error("wdsize: cannot get status of window\n");
	}
	wdp->w_flags = PMODE | LMODE;

	/*
	 * Decode all the options.
	 */
	while ((str = *++argv) != NULL && *str == '-') {
		while (*++str != '\0') {
			if (*str != 't' && *(str + 1) != '\0') {
				illsyntax();	/* illegal syntax */
			}

			switch (*str) {

			case 'x':	/* x position of virtual screen */
				tmp = NO;	/* sign flag */
				str2 = *++argv;
				if (str2 != NULL && *str2 == '-') {
					tmp = YES;
					str2++;
				}
				if ((val = cnvarg(str2)) < 0) {
					illsyntax();
				}
				if (tmp != NO) {
					val = -val;
				}
				wdp->wp_xorig = wdp->wl_xorig = val;
				break;

			case 'y':	/* y position of virtual screen */
				tmp = NO;	/* sign flag */
				str2 = *++argv;
				if (str2 != NULL && *str2 == '-') {
					tmp = YES;
					str2++;
				}
				if ((val = cnvarg(str2)) < 0) {
					illsyntax();
				}
				if (tmp != NO) {
					val = -val;
				}
				wdp->wp_yorig = wdp->wl_yorig = val;
				break;

			case 'u':	/* x position of window */
				if ((wdp->wp_vxorig = wdp->wl_vxorig = cnvarg(*++argv)) < 0) {
					illsyntax();
				}
				break;

			case 'v':	/* y position of window */
				if ((wdp->wp_vyorig = wdp->wl_vyorig = cnvarg(*++argv)) < 0) {
					illsyntax();
				}
				break;

			case 'w':	/* width of window */
				if ((wdp->wp_vxsize = wdp->wl_vxsize = cnvarg(*++argv)) < 0) {
					illsyntax();
				}
				break;

			case 'h':	/* height of window */
				if ((wdp->wp_vysize = wdp->wl_vysize = cnvarg(*++argv)) < 0) {
					illsyntax();
				}
				break;

			case 't':	/* size in text box units */
				wdp->w_flags |= TXTSIZE;
				break;

			default:
				illsyntax();
			}
		}
	}

	/*
	 * Now we are ready to set up a new default size and location of the
	 * window.
	 */
	if (Windflsz(STDIN, wdp) < 0 || wdp->w_rstat != W_OK) {
		error("wdsize: failed to set up default size\n");
	}

	exit(0);
}	/* end of main() */



/*
 * Illegal syntax, display the correct one.
 */

illsyntax()
{
	error("usage: wdsize [-t] [-x <n>] [-y <n>] [-u <n>] [-v <n>] [-w <n>] [-h <n>]\n");
}	/* end of illsyntax() */



/*
 * Display an error message on the standard output and then exit.
 */

error(s)
char	*s;
{
	write(STDERR, s, strlen(s));
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
