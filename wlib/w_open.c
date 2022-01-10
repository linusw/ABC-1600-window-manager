/* w_open.c */

/*
 * This function opens and creates a window. The window will not have the
 * status of a terminal, instead it becomes a child to another window.
 * If no error, the file descriptor for the window is returned, otherwise
 * an error code.
 */

/* 1985-01-23, Peter Andersson, Luxor Datorer AB */


#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>
#include	<win/w_liberr.h>


int
w_open(wdp)
struct	winstruc *wdp;		/* pointer to window data */
{
	register	fd;		/* file descriptor */

	fd = open(WMNTDIR, 2);
	if (fd == -1) {
		return(EWL_OPN);
	}
	if (Wincreat(fd, wdp) < 0) {
		close(fd);
		return(EWL_CRE);
	}
	return(fd);
}
