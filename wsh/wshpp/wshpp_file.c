/* wshpp_file.c */

/*
 * This file contains routines to read the input text file for the window shell
 * preprocessor - wshpp.
 */

/* 1985-02-20, Peter Andersson, Luxor Datorer AB */

/*
 * Functions in this file:
 *	readchar()
 *	backchar()
 *	nreadchar()
 *	creadchar()
 *	skipeoln()
 */


#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	"../h/w_wsh.h"
#include	"wshpp_defs.h"
#include	"wshpp_glbl.h"

#define		PUTBACK		16	/* max number of characters possibe to put back */
#define		INBUFSIZE	512	/* size of input buffer */


/*
 * Global variables.
 */
extern	struct	globals	glbl;


/*
 * Global variables local to this file.
 */
static	char	inbuf[PUTBACK+INBUFSIZE];	/* input buffer */
static	char	*inbufp;			/* pointer to next char to read */
static	int	count = 0;			/* # of chars left in input buffer */



/*
 * Read the next character from the file. Returns EOF if end of file or error.
 */

int
readchar()
{
	int		c;		/* read character */
	static	int	eof = NO;	/* end of file flag */

	if (count <= 0) {
		if (eof != NO || (count = read(glbl.inputfd, &inbuf[PUTBACK], INBUFSIZE)) <= 0) {
			eof = YES;
			return(EOF);
		}
		inbufp = &inbuf[PUTBACK];
	}
	count--;
	glbl.charno++;
	if ((c = *inbufp++ & CMASK) == '\n') {
		glbl.lineno++;		/* next line */
	}

	return(c);
}	/* end of readchar() */



/*
 * This function puts a character back on the input buffer. Max. PUTBACK
 * characters can be put back.
 */

backchar(c)
int	c;		/* character to put back */
{
	if (c != EOF) {
		*--inbufp = c;
		count++;
		glbl.charno--;
		if (c == '\n') {
			glbl.lineno--;
		}
	}

	return;
}	/* end of backchar() */



/*
 * This function is like readchar() but it automatically discards escaped
 * newlines.
 */

int
nreadchar()
{
	register	c;	/* read character */

	while ((c = readchar()) == '\\') {
		if ((c = readchar()) != '\n') {
			backchar(c);
			c = '\\';
			break;
		}
	}

	return(c);
}	/* end of nreadchar() */



/*
 * This function is like readchar() but it automatically skips comments and
 * escaped newlines, i.e. if it founds a '#' at the start of a line it skips
 * that line.
 */

int
creadchar()
{
	register	c;	/* read character */

	/*
	 * The first test is necessary in order to allow the input file to
	 * start with a comment.
	 */
	if (glbl.charno == 0) {
		if ((c = readchar()) == COMMENT) {
			skipeoln();
		}
		else {
			backchar(c);
		}
	}

	while ((c = nreadchar()) == '\n') {
		if ((c = nreadchar()) != COMMENT) {
			backchar(c);
			c = '\n';
			break;
		}
		skipeoln();
	}

	return(c);
}	/* end of creadchar() */



/*
 * Skip until the end of the current line. It is arranged so that the next
 * time a read is done it returns a newline.
 */

skipeoln()
{
	int	c;

	while ((c = nreadchar()) != '\n' && c != EOF)
		;
	backchar(c);

	return;
}	/* end of skipeoln() */
