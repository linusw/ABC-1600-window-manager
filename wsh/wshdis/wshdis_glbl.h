/* wshdis_glbl.h */

/*
 * This file contains a structure containing most of the global variables in
 * the wsh disassembler - wshdis.
 */

/* 1985-02-26, Peter Andersson, Luxor Datorer AB */



struct	globals {
	char	*cmdname;	/* name of this command */
	int	language;	/* language flag */
	char	*infile;	/* name of input file */
	char	*outfile;	/* name of output file */
	int	outfd;		/* file descriptor for output file */
	long	offset;		/* value to add to all pointers */
	int	linlen;		/* length of the current output line */
};
