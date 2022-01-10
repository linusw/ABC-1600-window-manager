/* wsh_defs.h */

/*
 * This file contains constant definitions for the window shell - wsh.
 */

/* 1985-03-19, Peter Andersson, Luxor Datorer AB */



/*
 * Mnemonic constants.
 */

#define		NO		0
#define		YES		1
#define		ENGLISH		0
#define		SWEDISH		1
#define		OK		0
#define		ERROR		1
#define		NULL		0
#define		STDIN		0	/* standard input */
#define		STDERR		2	/* standard error output */
#define		SUPERCH		3	/* window "super" channel */
#define		MENU		0
#define		TERMINAL	1



/*
 * Other constants.
 */

#define		SET_EIGHT_BIT	0x80
#define		SEVEN_BITS	0x7f
#define		REDRAWSIG	SIGINT		/* signal used for redrawing */



/*
 * The default data input files.
 */

#define		DFLTFILE	".window"
#define		GLBLFILE	"/etc/.window"



/*
 * Codes (i.e. indices) of the different messages.
 */

#define		M_ACCESS	0
#define		M_FOIF		1
#define		M_NOMEM		2
#define		M_ILLFORM	3
#define		M_CANTINIT	4
