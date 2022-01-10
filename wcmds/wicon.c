/* wicon.c */

/*
 * This command sets up an icon in a window.
 * The syntax is:
 *
 *	wicon [-prielmqszt] [-x <n>] [-y <n>] [-w <n>] [-h <n>] <sequence>
 *
 * Explanation of the options:
 *   b - Send icon sequence when left mouse button is pressed (default).
 *   r - Send icon sequence when left mouse button is released.
 *   i - Invert the icon when the mouse pointer points to it.
 *   e - Send the icon sequence when we enter the icon area.
 *   l - Send the icon sequence when we leave the icon area.
 *   m - Remove the icon after the icon sequence has been sent.
 *   q - Only send the icon sequence if there is a pending read request on
 *       the window.
 *   s - Check if option e or l is fulfilled upon set up.
 *   z - Only send the icon sequence if it is the level zero window.
 *   t - The coordinates and sizes are supposed to be given in character
 *       box units.
 *   x - The x coordinate of the lower left corner of the icon (default 0).
 *   y - The y coordinate of the lower left corner of the icon (default 0).
 *   w - The width of the icon (default 100).
 *   h - The height of the icon (default 100).
 *
 * <sequence> is the icon sequence to be sent when the icon is chosen.
 */

/* 1985-04-13, Peter Andersson, Luxor Datorer AB */



#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>

#define		STDIN		0	/* standard input */
#define		STDERR		2	/* standard error output */
#define		NULL		0	/* null pointer */


/*
 * Structure used to set up the icon.
 */

struct	winicon	id;



/*
 * The main function.
 */

main(argc, argv)
int	argc;		/* argument count */
char	**argv;		/* pointer to arguments */
{
	register	word	flags;	/* icon flags */
	register struct	winicon	*idp;	/* pointer to icon structure */
	register	char	*str;
	register	char	*strd;
	register		c;	/* character */
	register		tmp;

	/*
	 * Set up the default values.
	 */
	idp = &id;
	idp->ip_xsize = idp->il_xsize = idp->ip_ysize = idp->il_ysize = 100;
	flags = I_PMODE | I_LMODE;

	/*
	 * Decode all the arguments.
	 */
	while ((str = *++argv) != NULL) {
		if (*str != '-') {

			/*
			 * It is an icon sequence.
			 */
			strd = &idp->i_cmdseq[0];
			while (*str != '\0' && strd < &idp->i_cmdseq[ICONSEQLEN]) {
				*strd++ = *str++;
			}
		}
		else {

			/*
			 * It is an option.
			 */
			str++;
			while (*str != '\0') {
				c = *str++;
				switch (c) {

				case 'p':
					flags |= I_PRESS;
					break;

				case 'r':
					flags |= I_RELEASE;
					break;

				case 'i':
					flags |= I_INVERT;
					break;

				case 'e':
					flags |= I_ENTER;
					break;

				case 'l':
					flags |= I_LEAVE;
					break;

				case 'm':
					flags |= I_REMOVE;
					break;

				case 'q':
					flags |= I_RQST;
					break;

				case 's':
					flags |= I_SETCHK;
					break;

				case 'z':
					flags |= I_LZERO;
					break;

				case 't':
					flags |= I_TEXT;
					break;

				case 'x':
				case 'y':
				case 'w':
				case 'h':
					if (*str != '\0' || (tmp = cnvarg(*++argv)) < 0) {
						illsyntax();	/* illegal syntax */
					}
					switch (c) {

					case 'x':
						idp->ip_xorig = idp->il_xorig = tmp;
						break;

					case 'y':
						idp->ip_yorig = idp->il_yorig = tmp;
						break;

					case 'w':
						idp->ip_xsize = idp->il_xsize = tmp;
						break;

					case 'h':
						idp->ip_ysize = idp->il_ysize = tmp;
						break;
					}
					break;

				default:
					illsyntax();	/* illegal syntax */
				}
			}
		}
	}

	idp->i_flags = flags;

	/*
	 * Now we are ready to set up the icon.
	 */
	if (Winicon(STDIN, idp) < 0 || idp->i_rstat != W_OK) {
		error("wicon: failed to set up the icon\n");
	}

	exit(0);
}	/* end of main() */



/*
 * Illegal syntax, display the correct one.
 */

illsyntax()
{
	error("usage: wicon [-prielmqszt] [-x <n>] [-y <n>] [-w <n>] [-h <n>] <sequence>\n");
}	/* end of illsyntax() */



/*
 * Display a message on the standard error output and then exit with false
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
 * Returns -1 if error or NULL pointer, otherwise the binary value.
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
