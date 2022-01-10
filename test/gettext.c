/* gettext.c */

/*
 * This file contains the source code for a command to test the facility to
 * read the text contents of a virtual screen.
 */

/* 1985-01-29, Peter Andersson, Luxor Datorer AB */


#include	<stdio.h>
#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>


struct	t {
	struct	txtstruc	a;
	char			b[10000];
} s;



main(argc, argv)
int	argc;
char	**argv;
{
	int	i;
	char	*str;

	if (argc != 5) {
		fprintf(stderr, "usage: gettext row col rows cols\n");
		exit(1);
	}

	sscanf(*++argv, "%d", &i);
	s.a.tx_row = i;
	sscanf(*++argv, "%d", &i);
	s.a.tx_col = i;
	sscanf(*++argv, "%d", &i);
	s.a.tx_rcnt = i;
	sscanf(*++argv, "%d", &i);
	s.a.tx_ccnt = i;
#ifdef	DEBUG
	fprintf(stderr, "%d %d %d %d %d\n", s.a.tx_row, s.a.tx_col, s.a.tx_rcnt, s.a.tx_ccnt, s.a.tx_rstat);
#endif

	if (Wingettxt(0, &s, sizeof(struct t)) < 0) {
		fprintf(stderr, "gettext: error status returned\n");
		exit(1);
	}
#ifdef	DEBUG
	fprintf(stderr, "%d %d %d %d %d\n", s.a.tx_row, s.a.tx_col, s.a.tx_rcnt, s.a.tx_ccnt, s.a.tx_rstat);
#endif
	if (s.a.tx_rstat != W_OK) {
		switch (s.a.tx_rstat) {
		case WE_TSAVE:
			str = "gettext: no text contents saved\n";
			break;

		case WE_ILPARA:
			str = "gettext: illegal parameters\n";
			break;

		default:
			str = "gettext: impossible error\n";
			break;
		}
		fprintf(stderr, str);
		exit(1);
	}

	str = &s.b[0];
	for (i = 0 ; i < s.a.tx_rcnt ; i++) {
		write(1, str, s.a.tx_ccnt);
		write(1, "\n", 1);
		str += s.a.tx_ccnt;
	}

	exit(0);
}
