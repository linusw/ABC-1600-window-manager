/* uid.c */

/*
 * This command returns the user id as exit status.
 */

/* 1984-12-20, Peter Andersson, Luxor Datorer AB */


main()
{
	exit(getuid());
}
