/* patterns.c */

/*
 * This program asks for a horizontal and a vertical mask (entered in hex) and
 * displays all possible patterns using these patterns.
 */

/* 1985-05-29, Peter Andersson, Luxor Datorer AB */



main()
{
	int	hpatt;		/* horizontal pattern */
	int	vmask;		/* vertical mask */
	int	i;		/* loop index */
	int	j;		/* loop index */
	char	uuuuh;

	for ( ; ; ) {
		printf("\033[2J\033[H");
		printf("Horizontal pattern: ");
		scanf("%x", &hpatt);
		printf("Vertical mask: ");
		scanf("%x", &vmask);
		getchar();

		for (i = 0 ; i < 16 ; i++) {
			printf("\033[2J\033[Hhorizontal pattern = %x\n", hpatt);
			printf("Vertical mask = %x\n", vmask);
			printf("Shift = %d", i);
			for (j = 0 ; j < 8 ; j++) {
				printf("\033:15;%d;%d;%d;%dR", hpatt, vmask, i, j);
				printf("\033:%d;0m\033:%d;89;15;1f", 90 * j, 90 * j + 10 * 9 - 1);
			}
			printf("\033[17;1H  op = 0    op = 1    op = 2    op = 3    \
op = 4    op = 5    op = 6    op = 7");
			printf("\033[5;1HPress a key to continue ... ");
			scanf("%c", &uuuuh);
		}
	}
}
