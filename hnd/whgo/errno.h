/*
 *	Error codes
 *	===========
 *
 *	This is a stripped down version of <dnix/errno.h>, where all codes not
 *	used in whgo.c has been removed (PAn).
 *
 *	UNIX Compatible
 */
#define	ENOENT	-2		/* No such file or directory */
#define	EINTR	-4		/* Interrupted system call */
#define	EFAULT	-14		/* Bad address */
#define	EINVAL	-22		/* Invalid argument */
#define	EMFILE	-24		/* Too many open files */
/*
 *	DNIX Specials
 */
#define	EBADFC	-52		/* Illegal function code */
