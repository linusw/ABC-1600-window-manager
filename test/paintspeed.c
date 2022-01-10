/* paintspeed.c */

/*
 * This program tests the speed of the paint escape sequence by filling a
 * rectangle 100 times.
 */

/* 1985-06-12, Peter Andersson, Luxor Datorer AB */



main()
{
	char		*cp;
	char		*buf;
	register char	*p1;
	register char	*p2;
	register	i;
	char		*sbrk();

	cp = "\033[2J\033:10;10m\033:710;10;;1d\033:710;310;;1d\033:10;310;;1d\033:10;10;;1d\033:360;160;;1F";
	buf = sbrk(100 * strlen(cp) + 1);

	p1 = buf;
	for (i = 0 ; i < 100 ; i++) {
		p2 = cp;
		while (*p2 != '\0') {
			*p1++ = *p2++;
		}
	}

	*p1 = '\0';
	write(1, buf, strlen(buf));

	exit(0);
}
