/* grtest3.c */

/*
 * This command was written to make it possible to test the speed when using
 * graphic escape sequences in the window handler.
 * A line is drawn back and forth between coordinates 0,0 and 0,100.
 */

/* 1985-02-04, Peter Andersson, Luxor Datorer AB */


main(argc, argv)
int	argc;
char	**argv;
{
	register char	*buf1;
	register char	*buf2;
	register char	*str1;
	register char	*str2;
	register	count;
	register long	len;
	char		*sbrk();

	if (argc <= 1) {
		count = 1000;
	}
	else if (argc > 2 || (count = cnvarg(*(argv + 1))) <= 0) {
		error("usage: grtest3 [<count>]\n");
	}
	str1 = "\033:0;0m";
	str2 = "\033:0;100;;1d\033:0;0d";
	len = strlen(str1) + count * strlen(str2);
	if ((int)(buf1 = buf2 = sbrk(len)) == -1) {
		error("grtest3: can't allocate memory\n");
	}
	while (*str1 != '\0') {
		*buf1++ = *str1++;
	}
	while (count-- > 0) {
		str1 = str2;
		while (*str1 != '\0') {
			*buf1++ = *str1++;
		}
	}
	write(1, buf2, len);

	exit(0);
}



/*
 * Display an error message on the standard error output and the exit.
 */

error(s)
char	*s;
{
	write(2, s, strlen(s));
	exit(1);
}	/* error() */



/*
 * Convert a positive decimal ASCII string to binary.
 * Returns -1 if error or null pointer, otherwise the binary value.
 */

int
cnvarg(s)
register char	*s;
{
	register	val;		/* returned value */

	val = 0;
	while (*s != '\0') {
		if (*s < '0' || *s > '9') {
			return(-1);
		}
		val = 10 * val + *s++ - '0';
	}

	return(val);
}	/* end of cnvarg() */
