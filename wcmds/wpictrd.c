/* wpictrd.c */

/*
 * This command reads a rectangle of the picture memory for a virtual screen
 * or the whole screen and writes a parameter header followed by the binary
 * data to the specified file. The parameter header is the wpictblk structure
 * (see the w_structs.h header file).
 * The syntax is:
 *
 *	wpictrd [-p] [-x <n>] [-y <n>] [-w <n>] [-h <n>] [-c <n>] [-o <file>]
 *
 * Explanation of the options:
 *   p - first output a parameter header.
 *   x - x pixel coordinate of the lower left corner of the rectangle to
 *	 read (default 0).
 *   y - y pixel coordinate of the lower left corner of the rectangle to
 *	 read (default 0).
 *   w - Width in pixels of the rectangle (default 100).
 *   h - Height in pixels of the rectangle (default 100).
 *   c - The file descriptor (channel) to read the data through (default 0,
 *	 i.e. standard input).
 *   o - The name of the output file. If not specified, the output is written
 *	 to the standard output.
 */

/* 1985-05-21, Peter Andersson, Luxor Datorer AB */



#include	<stdio.h>
#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>

#define		STDIN		0	/* standard input */
#define		STDOUT		1	/* standard output */
#define		STDERR		2	/* standard error output */
#define		NO		0
#define		YES		1
#define		PROT_MODE	0644	/* output file protection mode */



/*
 * The main function.
 */

main(argc, argv)
int	argc;		/* argument count */
char	**argv;		/* pointer to arguments */
{
	register	tmp;		/* temporary */
	register long	size;		/* size of buffer used by request */
	register char	*p;		/* pointer to various things */
	register char	*file;		/* pointer to output file name */
	register	xopt;		/* value of x option */
	register	yopt;		/* value of y option */
	register	wopt;		/* value of w option */
	register	hopt;		/* value of h option */
	int		copt;		/* value of c option */
	int		popt;		/* p option flag */
	int		fd;		/* file descriptor of output file */
	char		*sbrk();

	/*
	 * Set up the default values for the options.
	 */
	xopt = yopt = 0;
	wopt = hopt = 100;
	copt = STDIN;
	popt = NO;
	file = NULL;

	/*
	 * Decode the arguments.
	 */
	while ((p = *++argv) != NULL) {
		if (*p++ != '-' || *(p + 1) != '\0' ||
		    (*p != 'p' && *p != 'o' && (tmp = cnvarg(*++argv)) < 0)) {
			illsyntax();		/* illegal syntax */
		}

		switch (*p) {

		case 'x':
			xopt = tmp;
			break;

		case 'y':
			yopt = tmp;
			break;

		case 'w':
			if (tmp == 0) {
				illsyntax();
			}
			wopt = tmp;
			break;

		case 'h':
			if (tmp == 0) {
				illsyntax();
			}
			hopt = tmp;
			break;

		case 'c':
			if (tmp >= _NFILE) {
				illsyntax();
			}
			copt = tmp;
			break;

		case 'o':
			if ((file = *++argv) == NULL) {
				illsyntax();
			}
			break;

		case 'p':
			popt = YES;
			break;

		default:
			illsyntax();
		}
	}

	/*
	 * Allocate size for the buffer used to do the request.
	 */
	size = (wopt + 7) / 8;
	size = sizeof(struct wpictblk) + size * hopt;
	if ((int)(p = sbrk(size)) < 0) {
		error("wpictrd: cannot allocate memory\n");
	}

	/*
	 * Fill in the parameter structure.
	 */
	((struct wpictblk *)p)->p_xaddr = xopt;
	((struct wpictblk *)p)->p_yaddr = yopt;
	((struct wpictblk *)p)->p_width = wopt;
	((struct wpictblk *)p)->p_height = hopt;
	((struct wpictblk *)p)->p_pad.p_xxx = 0;

	/*
	 * Now we are ready to read the picture memory.
	 */
	if (Wpictrd(copt, p, size) < 0) {
		error("wpictrd: failed to read picture memory\n");
	}

	/*
	 * Create the output file.
	 */
	fd = STDOUT;
	if (file != NULL && (fd = creat(file, PROT_MODE)) < 0) {
		error("wpictrd: failed to create output file\n");
	}

	/*
	 * Now write all the data to the output file.
	 */
	if (popt == NO) {
		p += sizeof(struct wpictblk);
		size -= sizeof(struct wpictblk);
	}
	if (write(fd, p, size) != size) {
		if (file != NULL) {
			unlink(file);
		}
		error("wpictrd: failed to write output data\n");
	}

	exit(0);
}	/* end of main() */



/*
 * Illegal syntax, display the correct one.
 */

illsyntax()
{
	error("usage: wpictrd [-p] [-x <n>] [-y <n>] [-w <n>] [-h <n>] [-c <n>] [-o <file>]\n");
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
