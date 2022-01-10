/* spray.c */

/*
 * This program randomly sends spray escape sequences to the standard output.
 */

/* 1985-07-24, Peter Andersson, Luxor Datorer AB */



#define		STDOUT		1	/* standard output */
#define		STDERR		2	/* standard error output */



main(argc, argv)
int	argc;
char	**argv;
{
	char		buf[100];
	register char	*cp;
	register	lop;
	char		*itoa();

	lop = '0';
	if (argc > 1) {
		cp = *(argv + 1);
		if (argc != 2 || *cp != '-') {
			illsyntax();
		}
		cp++;
		if (*cp == 's') {
			lop = '1';
		}
		else if (*cp == 'c') {
			lop = '2';
		}
		else if (*cp == 'i') {
			lop = '3';
		}
		else if (*cp != 'r') {
			illsyntax();
		}
		if (*(cp + 1) != '\0') {
			illsyntax();
		}
	}

	for ( ; ; ) {
		cp = &buf[0];
		*cp++ = '\033';
		*cp++ = ':';
		cp = itoa(random(800), cp);
		*cp++ = ';';
		cp = itoa(random(800), cp);
		*cp++ = ';';
		*cp++ = '2';
		*cp++ = ';';
		*cp++ = lop;
		*cp++ = 's';
		write(STDOUT, &buf[0], cp - &buf[0]);
	}
}


int
random(max)
int	max;
{
	int	r;

	r = rand();
	while (r > max) {
		r -= max;
	}
	return(r);
}


char *
itoa(val, cp)
register	val;
register char	*cp;
{
	register char	*tmpcp;
	char		tmpbuf[32];

	tmpcp = &tmpbuf[32];
	do {
		*--tmpcp = val % 10 + '0';
	} while ((val /= 10) > 0);
	while (tmpcp < &tmpbuf[32]) {
		*cp++ = *tmpcp++;
	}
	return(cp);
}


illsyntax()
{
	write(STDERR, "usage: spray [-r] | [-s] | [-c] | [-i]\n", 39);
	exit(1);
}
