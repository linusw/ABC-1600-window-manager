/* wfont.c */

/*
 * This command changes the default font for a window.
 * The syntax is:
 *
 *	wfont [-x <n>] [-y <n>] [<font>]
 *
 * Explanation of the options:
 *   x - The x coordinate for the middle visible character (default 1).
 *   y - The y coordinate for the middle visible character (default 1).
 * <font> is a single capital letter specifying the new font.
 * If no <font> is specified, the next font in the zoom list for the window
 * is used instead.
 */

/* 1985-05-05, Peter Andersson, Luxor Datorer AB */



#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>

#define		STDIN		0	/* standard input */
#define		STDERR		2	/* standard error output */
#define		NULL		0	/* null pointer */



/*
 * Structure used to change font.
 */

struct	dfltchr	dfd;



/*
 * The main function.
 */

main(argc, argv)
int	argc;		/* argument count */
char	**argv;		/* pointer to arguments */
{
	register		c;	/* character save */
	register		tmp;	/* temporary */
	register struct	dfltchr	*dfdp;	/* pointer to new font data */
	register	char	*str;	/* pointer into cmd argument */

	/*
	 * Set up the default values.
	 */
	dfdp = &dfd;
	dfdp->dcp_x = dfdp->dcl_x = dfdp->dcp_y = dfdp->dcl_y = 1;
	dfdp->dc_flags = Z_PMODE | Z_LMODE;

	/*
	 * Decode all the arguments.
	 */
	while ((str = *++argv) != NULL) {
		if (*str != '-') {

			/*
			 * It is a font.
			 */
			if (*str < 'A' || *str > 'Z' || *(str + 1) != '\0') {
				illsyntax();	/* illegal syntax */
			}
			dfdp->dcp_font = dfdp->dcl_font = *str;
		}
		else {
			c = *++str;
			if ((c != 'x' && c != 'y') || *(str + 1) != '\0' ||
			    (str = *++argv) == NULL || (tmp = cnvarg(str)) <= 0) {
				illsyntax();	/* illegal syntax */
			}
			if (c == 'x') {
				dfdp->dcp_x = dfdp->dcl_x = tmp;
			}
			else {
				dfdp->dcp_y = dfdp->dcl_y = tmp;
			}
		}
	}

	/*
	 * Now we are ready to change the default font.
	 */
	if (Winndchr(STDIN, dfdp) < 0 || dfdp->dc_rstat != W_OK) {
		error("wfont: failed to change font\n");
	}

	exit(0);
}	/* end of main() */



/*
 * Illegal syntax, display the correct one.
 */

illsyntax()
{
	error("usage: wfont [-x <n>] [-y <n>] [<font>]\n");
}	/* end of illsyntax() */



/*
 * Write a message to the standard error output and then exit with false
 * status.
 */

error(s)
char	*s;
{
	write(STDERR, s, strlen(s));

	exit(1);
}	/* end of error() */



/*
 * Convert a positive decimal ASCII string to binary.
 * Returns -1 if error, otherwise the binary value.
 */

int
cnvarg(s)
register char	*s;
{
	register	val;	/* returned value */

	val = 0;
	while (*s != '\0') {
		if (*s < '0' || *s > '9') {
			return(-1);
		}
		val = 10 * val + *s++ - '0';
	}

	return(val);
}	/* end of cnvarg() */
