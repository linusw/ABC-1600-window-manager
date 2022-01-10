/* stat.c */

/*
 * This small program makes a stat system call on the standard input and
 * displayes the result on the standard output.
 */

/* 1985-03-14, Peter Andersson, Luxor Datorer AB */



#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>

#define		STDIN		0	/* standard input */



/*
 * Structure to receive the stat of the standard input.
 */

struct	stat	buf;



main(argc, argv)
int	argc;
char	**argv;
{
	if (fstat(STDIN, &buf) != 0) {
		fprintf(stderr, "stat: can't stat\n");
		exit(1);
	}

	/*
	 * Display the result.
	 */
	printf("st_dev (d) = %d\n", buf.st_dev);
	printf("st_ino (d) = %d\n", buf.st_ino);
	printf("st_mode (o) = %o\n", buf.st_mode);
	printf("st_nlink (d) = %d\n", buf.st_nlink);
	printf("st_uid (d) = %d\n", buf.st_uid);
	printf("st_gid (d) = %d\n", buf.st_gid);
	printf("st_rdev (d) = %d\n", buf.st_rdev);
	printf("st_size (d) = %d\n", buf.st_size);
	printf("st_atime (d) = %d\n", buf.st_atime);
	printf("st_mtime (d) = %d\n", buf.st_mtime);
	printf("st_ctime (d) = %d\n", buf.st_ctime);

	exit(0);
}
