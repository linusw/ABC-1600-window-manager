/* wsh_glbl.h */

/*
 * This file contains a structure which contains most of the global variables
 * used in the window shell - wsh.
 */

/* 1985-03-19, Peter Andersson, Luxor Datorer AB */



struct	globals {
	char		*cmdname;	/* name of this command */
	struct	wshhead	*fh;		/* pointer to file header */
	int		language;	/* language flag */
	short		sigflag;	/* signal received flag */
	sint		screenmode;	/* landscape or portrait mode */
	sint		n_opt;		/* '-n' option flag */
};
