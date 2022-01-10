/* topgroup.c */

/*
 * This test command moves a window to the top level of a window group.
 * The syntax is:
 *
 *	topgroup
 */

/* 1985-07-24, Peter Andersson, Luxor Datorer AB */



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
	if (Win1lev(STDIN, &wl) < 0 || wl.l_rstat != W_OK) {
		write(STDERR, "topgroup: failed to move window to top level\n", 45);
		estat = 1;
	}

	exit(estat);
}
