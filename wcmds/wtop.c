/* wtop.c */

/*
 * This command moves a window to the top level.
 * The syntax is:
 *
 *	wtop
 */

/* 1985-04-13, Peter Andersson, Luxor Datorer AB */



#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>

#define		STDIN		0	/* standard input */
#define		STDERR		2	/* standard error output */



main()
{
	int		 estat;		/* exit status */
	struct	winlevel wl;		/* to change level */

	estat = 0;
	if (Winlevel(STDIN, &wl) < 0 || wl.l_rstat != W_OK) {
		write(STDERR, "wtop: failed to move window to top level\n", 41);
		estat = 1;
	}

	exit(estat);
}
