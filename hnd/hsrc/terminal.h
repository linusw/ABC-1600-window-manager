/* terminal.h */

/*
 * This file is used to combine the old header file <sgtty.h> with the new
 * header files <sys/ioctl.h> and <sys/termio.h>.
 */

/* 1985-01-31, Peter Andersson, Luxor Datorer AB */


/*
 * Structure containing terminal parameters (v7).
 */
struct	sgttyb {
	char	sg_ispeed;	/* input speed     */
	char	sg_ospeed;	/* output speed    */
	char	sg_erase;	/* erase character */
	char	sg_kill;	/* kill character  */
	short	sg_flags;	/* mode flags      */
};

/*
 * Structure containing special characters (v7).
 */
struct	tchars {
	char	t_intrc;	/* interrupt                 */
	char	t_quitc;	/* quit                      */
	char	t_startc;	/* start output              */
	char	t_stopc;	/* stop output               */
	char	t_eofc;		/* end-of-file               */
	char	t_brkc;		/* input delimiter (like nl) */
};


#define		O_XTABS		06000
#define		O_TBDELAY	06000
#define		O_TAB1		02000
#define		O_LCASE		04
#define		O_ECHO		010
#define		O_CRMOD		020
#define		O_NL1		0400
#define		O_NL2		01000
#define		O_RAW		040
#define		O_CBREAK	02
#define		O_ODDP		0100
#define		O_EVENP		0200
#define		O_VTDELAY	040000
#define		O_BSDELAY	0100000
#define		O_CR1		010000
#define		O_CR2		020000
#define		O_TAB2		04000

#include	<sys/types.h>
#include	<sys/ioctl.h>
#include	<sys/termio.h>
