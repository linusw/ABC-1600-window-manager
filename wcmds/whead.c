/* whead.c */

/*
 * This command inserts a header in a window.
 * The syntax is:
 *
 *	whead [ -i ] [ -t ] [ header ]
 *
 * Explanation of the options:
 *   i - Invert the header.
 *   t - Invert the top header.
 *
 * If no header is given, the present header will be removed.
 */

/* 1985-02-05, Peter Andersson, Luxor Datorer AB */


#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>

#define		STDIN		0
#define		STDERROUT	2
#define		NULL		0


/*
 * Structure used to insert the header. Note that the fact that this structure
 * is initialized to zeros is used.
 */

struct	headstruc hd;



main(argc, argv)
int	argc;		/* number of arguments */
char	**argv;		/* pointer to argument list */
{
	register char	*strs;
	register char	*strd;

	while ((strs = *++argv) != NULL && *strs == '-') {
		while (*++strs != '\0') {
			switch (*strs) {
			case 'i':		/* invert header */
				hd.h_flags |= H_INVHD;
				break;

			case 't':		/* invert top header */
				hd.h_flags |= H_INVTOP;
				break;

			default:		/* otherwise error */
				illsyntax();
			}
		}
		argc--;
	}

	if (argc > 1) {
		if (argc != 2) {
			illsyntax();
		}
		strd = &hd.h_hdr[0];
		while (*strs != '\0' && strd < &hd.h_hdr[HDRSIZE]) {
			*strd++ = *strs++;
		}
	}

	if (Winheader(STDIN, &hd) < 0) {
		error("whead: can't alter header\n");
	}

	exit(0);
}	/* end of main() */



/*
 * Illegal syntax, display the correct one.
 */

illsyntax()
{
	error("usage: whead [ -i ] [ -t ] [ header ]\n");
}



/*
 * Display a message on standard error output and then exit.
 */

error(s)
char	*s;
{
	write(STDERROUT, s, strlen(s));
	exit(1);
}	/* end of error() */
