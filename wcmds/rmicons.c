/* rmicons.c */

/*
 * This command removes all icons in a window.
 * The syntax is:
 *
 *	rmicons
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
	int	estat;		/* exit status */

	estat = 0;
	if (Rmicons(STDIN) < 0) {
		write(STDERR, "rmicons: failed to remove icons\n", 32);
		estat = 1;
	}

	exit(estat);
}
