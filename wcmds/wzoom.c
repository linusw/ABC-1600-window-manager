/* wzoom.c */

/*
 * This command sets up a zoom list for a window.
 * The syntax is:
 *
 *	wzoom [<zoomlist>]
 *
 * <zoomlist> is a string of capital letters indicating the fonts which the
 * zoom list shall consist of. If no <zoomlist> is specified, any existing
 * zoomlist is removed.
 */

/* 1985-04-13, Peter Andersson, Luxor Datorer AB */



#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>

#define		STDIN		0	/* standard input */
#define		STDERR		2	/* standard error output */


/*
 * Structure used to set up the zoom list.
 */

struct	zoomlst	zd;



/*
 * The main function.
 */

main(argc, argv)
int	argc;		/* argument count */
char	**argv;		/* pointer to arguments */
{
	register	char	*cpp;	/* pointer to portrait list */
	register	char	*clp;	/* pointer to landscape list */
	register	char	*str;	/* pointer to argument */
	register struct	zoomlst	*zdp;	/* pointer to zoom structure */

	if (argc > 2) {
		error("usage: wzoom [<zoomlist>]\n");
	}
	zdp = &zd;
	zdp->z_flags = Z_PMODE | Z_LMODE;
	if (argc == 2) {

		/*
		 * A zoom list is given.
		 */
		cpp = &zdp->zp_list[0];
		clp = &zdp->zl_list[0];
		str = *(argv + 1);
		while (*str != '\0' && cpp < &zdp->zp_list[ZOOMSIZE]) {
			*cpp++ = *clp++ = *str++;
		}
	}

	/*
	 * Now set up the zoom list.
	 */
	if (Winzoom(STDIN, zdp) < 0 || zdp->z_rstat != W_OK) {
		error("wzoom: failed to set up zoom list\n");
	}

	exit(0);
}	/* end of main() */



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
