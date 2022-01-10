/* draw.c */

/*
 * Program used to test the mouse read facilities of the window handler.
 */

/* 1985-01-24, Peter Andersson, Luxor Datorer AB */


#include	<signal.h>
#include	<sgtty.h>
#undef		FIOCLEX
#undef		FIONCLEX
#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>

#define		STDIN		0
#define		STDOUT		1
#define		STDERROUT	2

struct	sgttyb	tty;		/* to set raw mode, no echo */

/*
 * Data for a hair cross mouse pointer.
 */
struct	npstruc	mpdata = {
	31, 31,
	15, 15,
	{
		0xfffeffff, 0xfffeffff, 0xfffeffff, 0xfffeffff,
		0xfffeffff, 0xfffeffff, 0xfffeffff, 0xfffeffff,
		0xfffeffff, 0xfffeffff, 0xfffeffff, 0xfffeffff,
		0xfffeffff, 0xfffeffff, 0xfffeffff, 0x00000001,
		0xfffeffff, 0xfffeffff, 0xfffeffff, 0xfffeffff,
		0xfffeffff, 0xfffeffff, 0xfffeffff, 0xfffeffff,
		0xfffeffff, 0xfffeffff, 0xfffeffff, 0xfffeffff,
		0xfffeffff, 0xfffeffff, 0xfffeffff, 0xffffffff
	} ,
	{
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
	} ,
	0
};


int	rsignal;	/* signal used to signal that the window has moved */
char	*bufbeg;	/* start of buffer holding the graphic contents */
char	*bufend;	/* end of the buffer */
char	*bufcur;	/* current position in the buffer */
char	buf[100];	/* temporary buffer */
char	sbuf[100];	/* save buffer */


main()
{
	register char	*str;
	register char	*str2;
	register	button;		/* state of the left mouse button */
	register	last;

	/*
	 * Set up so the SIGTERM signal will terminate this program.
	 */
	signal(SIGTERM, SIG_DFL);

	/*
	 * Set raw mode, no echo.
	 */
	tty.sg_flags = RAW;
	ioctl(STDIN, TIOCSETP, &tty);

	/*
	 * Initialize the automatic redrawing of the window.
	 */
	if (w_reini() != 0) {
		str = "Kan ej initialisera\n\r";
		write(STDERROUT, str, strlen(str));
		sleep(5);
		exit(1);
	}

	/*
	 * Alter the layout of the mouse pointer.
	 */
	if (Winchmpnt(STDIN, &mpdata) < 0 || mpdata.np_rstat != W_OK) {
		str = "Kan ej {ndra muspekare\n\r";
		write(STDERROUT, str, strlen(str));
		sleep(5);
		exit(1);
	}

	str = &buf[0];
	str2 = &sbuf[0];
	button = rdmous(str);
	strcat(str, "m");
	last = 0;

	for ( ; ; ) {
		strcpy(str2, str);
		button = rdmous(str);
		if (last == 0 && button != 0) {
			w_grout(str2);
		}
		if (button == 0) {
			strcat(str, "m");
		}
		else {
			strcat(str, ";;1d");
		}
		if (button != 0) {
			w_grout(str);
		}
		last = button;
	}
}	/* end of main() */



/*
 * Function used to read the mouse.
 */

int
rdmous(str)
char	*str;	/* position to save the first part of the answer string */
{
	register	c;		/* read character */
	register	button;		/* state of left button of mouse */
	register char	*s;

	write(STDOUT, "\033:7M", 4);	/* send the request string */

	for ( ; ; ) {
		s = str;
		c = onechar();
		if (c != '\033') {
			continue;
		}
		*s++ = c;
		c = onechar();
		if (c != ':') {
			continue;
		}
		*s++ = c;
		do {
			c = onechar();
			*s++ = c;
		} while (c >= '0' && c <= '9');
		if (c != ';') {
			continue;
		}
		do {
			c = onechar();
			*s++ = c;
		} while (c >= '0' && c <= '9');
		*--s = '\0';
		if (c != ';') {
			continue;
		}
		c = onechar();
		if (c == '0') {
			button = 0;
		}
		else if (c == '1') {
			button = 1;
		}
		else {
			continue;
		}
		c = onechar();
		if (c == 'P') {
			break;		/* it was the report sequence */
		}
	}	/* for ( ; ; ) */

	return(button);
}	/* end of rdmous() */



/*
 * Get one character from the standard input.
 */

int
onechar()
{
	static	char	inbuf[256];	/* tty read buffer */
	static	char	*inbufp;	/* pointer into inbuf[] */
	static	int	cnt = 0;	/* # of characters left in inbuf[] */

	if (cnt == 0) {
		while ((cnt = read(STDIN, &inbuf[0], 256)) <= 0)
			;
		inbufp = &inbuf[0];
	}
	cnt--;
	return(*inbufp++ & 255);
}	/* end of onechar() */



/*
 * Initialize things to take care of redrawing the graphics when the window
 * has changed in some way.
 * Returns a non-zero value if the initialization fails.
 */

int
w_reini()
{
	struct	winstruc wd;
	struct	winstruc *wdp;
	int		 w_rstrp();
	char		 *sbrk();

	wdp = &wd;
	if (Winstat(STDIN, wdp) < 0 || wdp->w_rstat != W_OK) {
		return(1);
	}
	if ((rsignal = wdp->w_rsig) == 0) {
		return(1);
	}
	if ((int)(bufbeg = bufcur = sbrk(2048)) == -1) {
		return(1);
	}
	bufend = bufbeg + 2048;
	signal(rsignal, w_rstrp);

	return(0);
}	/* end of w_reini() */



/*
 * This routine takes care of the redrawing of the window when it has changed
 * in some way.
 */

w_rstrp()
{
	signal(rsignal, SIG_IGN);
	write(STDOUT, bufbeg, bufcur - bufbeg);
	signal(rsignal, w_rstrp);

	return;
}	/* end of w_rstrp() */



/*
 * Routine to output graphics to a window and save it internally so the
 * window can redraw itself if necessary.
 */

w_grout(str)
char	*str;		/* graphics to output */
{
	register	len;
	register char	*str1;
	register char	*str2;
	char		*sbrk();

	str2 = str;
	len = strlen(str2);
	if (bufcur + len >= bufend) {
		if ((int)(sbrk(2048)) == -1) {
			bufcur = bufbeg;
		}
		else {
			bufend += 2048;
		}
	}
	str1 = bufcur;
	while (*str2 != '\0') {
		*str1++ = *str2++;
	}
	write(STDOUT, str, len);
	bufcur = str1;

	return;
}	/* end of w_grout() */
