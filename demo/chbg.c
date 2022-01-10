/* chbg.c */

/*
 * This file contains the source code for a command to change the pattern
 * of the background in the window handler.
 * NOTE: It is assumed that the file descriptor for the "super" channel is
 * 3 on entry to this command.
 */

/* 1984-12-19, Peter Andersson, Luxor Datorer AB */


#include	<stdio.h>
#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>
#undef		FIOCLEX
#undef		FIONCLEX
#include	<sgtty.h>

#define		STDIN		0
#define		SUPERCHANNEL	3


struct	chbgstruc	cb;	/* to change background     */
struct	sgttyb		tty;	/* to set raw mode, no echo */



main()
{
	register	x;		/* x coordinate         */
	register	y;		/* y coordinate         */
	register	i, j;		/* loop indices         */
	register word	mask;		/* mask                 */
	register	c;		/* one character string */
	char		str[sizeof(word)+3];

	/*
	 * Make sure unbuffered output.
	 */
	setbuf(stdout, NULL);

	/*
	 * Put in raw mode, no echo.
	 */
	ioctl(STDIN, TIOCGETP, &tty);
	tty.sg_flags |= RAW;
	tty.sg_flags &= ~ECHO;
	ioctl(STDIN, TIOCSETP, &tty);

	/*
	 * Draw the grid and some information.
	 */
	printf("\033[2J\033[;H");
	for (i = 0 ; i < BGPSIZE ; i++) {
		cb.cb_bitmap[i] = 0;
		printf("0000000000000000\n\r");
	}
	printf("\033[4;20Hl = left");
	printf("\033[5;20Hr = right");
	printf("\033[6;20Hu = up");
	printf("\033[7;20Hd = down");
	printf("\033[8;20Ht = toggle");
	printf("\033[9;20Hi = invert");
	printf("\033[10;20Hc = clear");
	printf("\033[11;20He = end");
	printf("\033[12;20Hq = quit");
	printf("\033[;H");

	x = y = 1;		/* initial coordinates */

	/*
	 * The main loop.
	 */
	do {
		if ((c = getchar()) == EOF) {
			restore(0);
		}

		switch (c) {
		case 'l':		/* left */
		case 'L':
			if (x > 1) {
				x--;
			}
			break;

		case 'r':		/* right */
		case 'R':
			if (x < 8 * sizeof(word)) {
				x++;
			}
			break;

		case 'u':		/* up */
		case 'U':
			if (y > 1) {
				y--;
			}
			break;

		case 'd':		/* down */
		case 'D':
			if (y < BGPSIZE) {
				y++;
			}
			break;

		case 't':		/* toggle */
		case 'T':
			mask = 1 << (8 * sizeof(word) - x);
			cb.cb_bitmap[y-1] ^= mask;
			if (cb.cb_bitmap[y-1] & mask) {
				printf("1");
			}
			else {
				printf("0");
			}
			break;

		case 'i':		/* invert */
		case 'I':
			printf("\033[;H");
			for (i = 0 ; i < BGPSIZE ; i++) {
				cb.cb_bitmap[i] = ~cb.cb_bitmap[i];
				mask = 1 << 15;
				for (j = 0 ; j < 8 * sizeof(word) ; j++) {
					if (cb.cb_bitmap[i] & mask) {
						str[j] = '1';
					}
					else {
						str[j] = '0';
					}
					mask >>= 1;
				}
				str[j] = '\n';
				str[j+1] = '\r';
				str[j+2] = '\0';
				printf(str);
			}
			break;

		case 'c':		/* clear */
		case 'C':
			printf("\033[;H");
			for (i = 0 ; i < BGPSIZE ; i++) {
				cb.cb_bitmap[i] = 0;
				printf("0000000000000000\n\r");
			}
			break;

		}

		printf("\033[%d;%dH", y, x);

	} while (c != 'q' && c != 'Q' && c != 'e' && c != 'E');

	if (c == 'e' || c == 'E') {

		/*
		 * Change the background pattern.
		 */
		if (Winchbg(SUPERCHANNEL, &cb) < 0) {
			printf("\033[2J\033[;HCan't change background\n\r");
			sleep(5);
			restore(1);
		}
	}

	/*
	 * Everything is ok, exit.
	 */
	restore(0);
}



/*
 * Restore the screen and then exit.
 */

restore(status)
int	status;		/* exit status */
{
	printf("\033[2J\033[;H");
	tty.sg_flags &= ~RAW;
	tty.sg_flags |= ECHO;
	ioctl(STDIN, TIOCSETP, &tty);

	exit(status);
}
