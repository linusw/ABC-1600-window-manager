/* emovea.c */

/*
 * This program randomly sends mover escape sequences to the standard output.
 */

/* 1985-05-08, Peter Andersson, Luxor Datorer AB */



#define		STDOUT		1	/* standard output */



main()
{
	char		buf[100];
	register char	*cp;
	char		*itoa();

	for ( ; ; ) {
		cp = &buf[0];
		*cp++ = '\033';
		*cp++ = ':';
		cp = itoa(random(800), cp);
		*cp++ = ';';
		cp = itoa(random(800), cp);
		*cp++ = ';';
		cp = itoa(random(800), cp);
		*cp++ = ';';
		cp = itoa(random(800), cp);
		*cp++ = ';';
		cp = itoa(random(200), cp);
		*cp++ = ';';
		cp = itoa(random(200), cp);
		if (random(20000) > 10000) {
			*cp++ = ';';
			*cp++ = '1';
		}
		*cp++ = 'r';
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
