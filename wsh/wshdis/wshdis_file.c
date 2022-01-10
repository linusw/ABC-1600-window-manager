/* wshdis_file.c */

/*
 * This file contains routines to output text to a file in the wshdis command.
 */

/* 1985-02-27, Peter Andersson, Luxor Datorer AB */

/*
 * Functions in this file:
 *	charout()
 *	strout()
 *	emptybuf()
 */



#include	"wshdis_defs.h"
#include	"wshdis_glbl.h"

#define		FILBUFSIZE	512	/* size of file output buffer */


/*
 * Global variables.
 */
extern	struct	globals	glbl;

static	char	cbuf[FILBUFSIZE];	/* file output buffer */
static	char	*cbufp = &cbuf[0];	/* current position in cbuf[] */



/*
 * This function puts a character in the file output buffer.
 */

charout(c)
int	c;		/* character to output */
{
	*cbufp++ = c;
	if (cbufp >= &cbuf[FILBUFSIZE]) {
		emptybuf();
	}
	glbl.linlen++;
	if (c == '\n') {
		glbl.linlen = 0;
	}

	return;
}	/* end of charout() */



/*
 * This function puts a string of characters terminated by a null character in
 * the file output buffer.
 */

strout(s)
register char	*s;		/* string to put in buffer */
{
	while (*s != '\0') {
		*cbufp++ = *s;
		if (cbufp >= &cbuf[FILBUFSIZE]) {
			emptybuf();
		}
		glbl.linlen++;
		if (*s++ == '\n') {
			glbl.linlen = 0;
		}
	}

	return;
}	/* end of strout() */



/*
 * Flush the file output buffer.
 */

emptybuf()
{
	register	cnt;	/* # of characters to output to file */

	cnt = cbufp - &cbuf[0];
	if (cnt > 0) {
		if (write(glbl.outfd, &cbuf[0], cnt) != cnt) {
			if (glbl.outfile != NULL) {
				close(glbl.outfd);
				unlink(glbl.outfile);
			}
			errexit(M_FWOF);
		}
		cbufp = &cbuf[0];
	}

	return;
}	/* end of emptybuf() */
