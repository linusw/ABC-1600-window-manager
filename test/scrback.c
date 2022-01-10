/* scrback.c */

main()
{
	register	i;
	register char	*str;

	for (i = 0 ; i < 1000 ; i++) {
		str = "abcdefghijklmnopqrstuvxyz1234567890abcdefghijklmnopqrstuvwxyz\n\33M\33M";
		write(1, str, strlen(str));
	}

	exit(0);
}
