/* visible.c */

/*
 * This program displays the visible parts of a window.
 */


#include	<stdio.h>
#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>

#define		STDIN		0

struct {
	struct	visdes	vd;
	struct	rectdes	rd[4*MAXWINS];
} buf;


main(argc, argv)
int	argc;
char	**argv;
{
	register	i;

	buf.vd.v_nrect = 4*MAXWINS;
	if (Wingetvis(STDIN, &buf, sizeof(buf)) < 0 || buf.vd.v_rstat != W_OK) {
		fprintf(stderr, "%s: can't get visible parts\n", *argv);
		exit(1);
	}

	printf("Rectangle\tOrigin (x,y)\tSize (x,y)\n");

	for (i = 0 ; i < buf.vd.v_nrect ; i++) {
		printf("%d\t\t%d, %d    \t%d, %d\n", i + 1,
						     buf.rd[i].r_xorig,
						     buf.rd[i].r_yorig,
						     buf.rd[i].r_xsize,
						     buf.rd[i].r_ysize);
	}

	exit(0);
}
