/* wbg.c */

/*
 * This command reads the file specified as argument and uses the data to
 * set up a new background pattern for the window handler. It supposes file
 * descriptor 3 to be the window handler "super" channel.
 * The syntax is:
 *
 *	wbg [-n] [file]
 *
 * where the '-n' option shall be used if no error messages shall be displayed.
 * If 'file' is not specified, the standard input is read instead.
 */

/* 1985-04-17, Peter Andersson, Luxor Datorer AB */



#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>

#define		NULL		0		/* null pointer */
#define		STDIN		0		/* standard input */
#define		STDERR		2		/* standard error output */
#define		SUPERCH		3		/* "super" channel */
#define		NO		0
#define		YES		1



int	nopt = NO;		/* -n option flag */



/*
 * The main function.
 */

main(argc, argv)
int	argc;		/* argument count */
char	**argv;		/* pointer to arguments */
{
	register char	  *file;	/* pointer to file name */
	register char	  *str;
	register	  fd;		/* file descriptor */
	register	  cnt1;		/* used to read data */
	register	  cnt2;		/* used to read data */
	struct	chbgstruc bpd;		/* to alter background */

	file = NULL;

	/*
	 * Decode the arguments.
	 */
	while ((str = *++argv) != NULL) {
		if (strcmp(str, "-n") == 0) {
			nopt = YES;
		}
		else if (*str != '-') {
			file = str;
		}
		else {
			error("usage: wbg [-n] [file]\n");
		}
	}

	/*
	 * Open the data file.
	 */
	fd = STDIN;
	if (file != NULL && (fd = open(file, 0)) < 0) {
		error("wbg: cannot open data file\n");
	}
	cnt2 = 0;
	while ((cnt1 = read(fd, (char *)(&bpd), sizeof(struct chbgstruc) - cnt2)) > 0 &&
	       (cnt2 += cnt1) != sizeof(struct chbgstruc))
		;
	if (cnt2 != sizeof(struct chbgstruc)) {
		error("wbg: cannot read data\n");
	}

	/*
	 * Now we are ready to set up a new background pattern.
	 */
	if (Winchbg(SUPERCH, &bpd) < 0) {
		error("wbg: failed to set up pattern\n");
	}

	exit(0);
}	/* end of main() */



/*
 * Write a message to the standard error output if the '-n' option was not
 * specified and then exit with false status.
 */

error(s)
char	*s;
{
	if (nopt == NO) {
		write(STDERR, s, strlen(s));
	}

	exit(1);
}	/* end of error() */
