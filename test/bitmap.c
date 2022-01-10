/* bitmap.c */

/*
 * This function reads the bitmap contents of the whole or a part of a window
 * and converts it to graphic escape sequences, which is sent to standard
 * output.
 */


#include	<stdio.h>
#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>

#define		STDIN		0


struct {
	struct	wpictblk d;
	char		 b[10000];
} s;



main(argc, argv)
int	argc;
int	*argv[];
{
	register	x;
	register	y;
	register	val;
	register	bit;
	register	bytes;
	int		tmp;

	s.d.p_xaddr = 0;
	s.d.p_yaddr = 0;
	s.d.p_width = 100;
	s.d.p_height = 100;
	if (argc > 1) {
		sscanf(argv[1], "%d", &tmp);
		s.d.p_xaddr = tmp;
	}
	if (argc > 2) {
		sscanf(argv[2], "%d", &tmp);
		s.d.p_yaddr = tmp;
	}
	if (argc > 3) {
		sscanf(argv[3], "%d", &tmp);
		s.d.p_width = tmp;
	}
	if (argc > 4) {
		sscanf(argv[4], "%d", &tmp);
		s.d.p_height = tmp;
	}

	if ((x = Wpictrd(STDIN, &s, sizeof(s))) < 0) {
		fprintf(stderr, "%s: can't read picture memory\n", argv[0]);
		exit(1);
	}

	bytes = (s.d.p_width + 7) / 8;
	for (y = 0 ; y < s.d.p_height ; y++) {
		for (x = 0 ; x < s.d.p_width ; x++) {
			val = s.b[y*bytes+x/8];
			bit = 1 << (7 - (x & 7));
			if (val & bit) {
				printf("\033:%d;%dp", x, y);
			}
			else {
				printf("\033:%d;%d;1p", x, y);
			}
		}
	}

	exit(0);
}
