/* whelp.c */

/*
 * This command changes the sequence sent when the help box is used.
 * The syntax is:
 *
 *	whelp [<sequence>]
 *
 * No sequence will be sent if <sequence> is not given.
 */

/* 1985-07-24, Peter Andersson, Luxor Datorer AB */


#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>

#define		STDIN		0
#define		STDERR		2


/*
 * Structure used to change the help sequence. Note that the fact that this
 * structure is initialized to zeros is used.
 */

struct	helpst	hd;



main(argc, argv)
int	argc;		/* number of arguments */
char	**argv;		/* pointer to argument list */
{
	register char	*strs;
	register char	*strd;

	if (argc > 1) {
		if (argc != 2) {
			error("usage: whelp [sequence]\n");
		}
		strd = &hd.hlp_seq[0];
		strs = *(argv + 1);
		while (*strs != '\0' && strd < &hd.hlp_seq[HLPSIZE]) {
			*strd++ = *strs++;
		}
	}

	if (Winhelp(STDIN, &hd) < 0) {
		error("whelp: failed to change sequence\n");
	}

	exit(0);
}	/* end of main() */



/*
 * Display a message on standard error output and then exit.
 */

error(s)
char	*s;
{
	write(STDERR, s, strlen(s));
	exit(1);
}	/* end of error() */
