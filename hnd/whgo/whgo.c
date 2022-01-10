/* whgo.c */

/*
 * This is the start up program for the window handler.
 * It is this program that waits for the window handler to be activated and
 * when this happens, executes the appropriate window handler.
 */

/* 1985-03-19, Peter Andersson, Luxor Datorer AB */

/*
 * Functions in this file:
 *	main()
 *	alrmtrap()
 *	panicerr()
 *	uwritbuf()
 */


#define		_NFILE		20	/* the preprocessor says to much
					   defining, so stdio.h cannot be
					   included */
#include	"../hsrc/defs.h"
#include	"../hsrc/graphconst.h"
#include	"../hsrc/structs.h"
#include	"../hsrc/macros.h"
#include	"../hsrc/dnixmacros.h"
#include	<signal.h>
#include	<sys/stat.h>
#include	<dnix/ireq.h>
/* #include	<dnix/errno.h> */
#include	"errno.h"		/* to avoid space problems in the
					   preprocessor                   */
#include	<dnix/fcodes.h>
#include	"../../gdrv/x35g.h"
#include	"../../gdrv/kybrdcodes.h"

#define		CONMOD1		020622		/* mode of console when not
						   activated              */
#define		CONMOD2		020600		/* mode of console when
						   activated              */
#define		NOSTARTUP	0		/* not starting up        */
#define		STARTUP		1		/* starting up            */
#ifndef	WH_NAME
#define		WH_NAME		"ABCwindow"	/* the name */
#endif
#ifndef	WH_VERSION
#define		WH_VERSION	"2.0"		/* version number */
#endif



/*
 * The name of this program.
 */

char	*name;



/*
 * The main routine.
 */

main(argc, argv)
int	argc;		/* number of arguments  */
char	**argv;		/* pointer to arguments */
{
	register		i;		/* loop index                  */
	register		rn;		/* request number              */
	register		tmp;		/* temporary                   */
	register struct	iogrst	*cmpgp;		/* pointer to shared mem page  */
	register	char	*cp;
	register	char	*cp1;
	struct		ireq	rqst;		/* structure receiving request */
	struct		mapseg	mapbuf;		/* to map in memory            */
	struct		stat	wstat;		/* mount directory status      */
	static		sint	locflg[MAXLOC];	/* to hold locatate opens      */
	static		char	console[] = "/dev/console";
	static		byte	lmsetup[] = {	/* data for mouse and keyboard */
					K_POSM,	/* landscape mode              */
					((MYPOS >> 6) & 0x3f) + ' ',
					(MYPOS & 0x3f) + ' ',
					((MXPOS >> 6) & 0x3f) + ' ',
					(MXPOS & 0x3f) + ' ',
					K_PREM,
					((MXSCALE >> 6) & 0x3f) + ' ',
					(MXSCALE & 0x3f) + ' ',
					((MYSCALE >> 6) & 0x3f) + ' ',
					(MYSCALE & 0x3f) + ' ',
					((MXINCR >> 6) & 0x3f) + ' ',
					(MXINCR & 0x3f) + ' ',
					((MYINCR >> 6) & 0x3f) + ' ',
					(MYINCR & 0x3f) + ' ',
					K_LIMM,
					((MYMINLIM >> 6) & 0x3f) + ' ',
					(MYMINLIM & 0x3f) + ' ',
					((MYMAXLIM >> 6) & 0x3f) + ' ',
					(MYMAXLIM & 0x3f) + ' ',
					((MXMINLIM >> 6) & 0x3f) + ' ',
					(MXMINLIM & 0x3f) + ' ',
					((MXMAXLIM >> 6) & 0x3f) + ' ',
					(MXMAXLIM & 0x3f) + ' ',
					K_OFLD1,
					K_OFLD2,
					K_OFLD3,
					K_OFLD4,
					K_OFLD5,
					K_OFLD6,
					K_OFLD7,
					K_OFLD8,
					K_OFINS,
					K_OFALT
	};
	static		byte	pmsetup[] = {	/* data for mouse and keyboard */
					K_POSM,	/* portrait mode               */
					((MXPOS >> 6) & 0x3f) + ' ',
					(MXPOS & 0x3f) + ' ',
					((MYPOS >> 6) & 0x3f) + ' ',
					(MYPOS & 0x3f) + ' ',
					K_PREM,
					((MXSCALE >> 6) & 0x3f) + ' ',
					(MXSCALE & 0x3f) + ' ',
					((MYSCALE >> 6) & 0x3f) + ' ',
					(MYSCALE & 0x3f) + ' ',
					((MXINCR >> 6) & 0x3f) + ' ',
					(MXINCR & 0x3f) + ' ',
					((MYINCR >> 6) & 0x3f) + ' ',
					(MYINCR & 0x3f) + ' ',
					K_LIMM,
					((MXMINLIM >> 6) & 0x3f) + ' ',
					(MXMINLIM & 0x3f) + ' ',
					((MXMAXLIM >> 6) & 0x3f) + ' ',
					(MXMAXLIM & 0x3f) + ' ',
					((MYMINLIM >> 6) & 0x3f) + ' ',
					(MYMINLIM & 0x3f) + ' ',
					((MYMAXLIM >> 6) & 0x3f) + ' ',
					(MYMAXLIM & 0x3f) + ' ',
					K_OFLD1,
					K_OFLD2,
					K_OFLD3,
					K_OFLD4,
					K_OFLD5,
					K_OFLD6,
					K_OFLD7,
					K_OFLD8,
					K_OFINS,
					K_OFALT
	};
	int			alrmtrap();

	/*
	 * Exit if any arguments has been given.
	 */
	if (argc != 1) {
		exit(1);
	}
	name = *argv;		/* point to handler name */

	/*
	 * Ignore some signals and make sure the rest are set to default.
	 */
	for (i = 1 ; i < NSIG ; i++) {
		signal(i, SIG_DFL);
	}
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGALRM, SIG_IGN);

	/*
	 * Close all channels except file descriptor 0. This file descriptor
	 * is the one used to read the request queue and it must only be
	 * closed if the name of this command is 'whgo'.
	 */
	for (i = 1 ; i < _NFILE ; i++) {
		close(i);
	}

	/*
	 * Map in the I/O pages and the 2K memory area shared with the tty
	 * device driver.
	 */
	if ((i = Segproc(&mapbuf, sizeof(struct mapseg), 1)) != sizeof(struct mapseg)) {
		panicerr(HE_MAPIO, i);
	}

	/*
	 * Compute the address to the shared memory page.
	 */
	cmpgp = (struct iogrst *)(mapbuf.lad + PAGEOFFS);

	/*
	 * Do some things if we are starting up, i.e. the name of this command
	 * is 'whgo'.
	 */
	if ((cp = name + strlen(name) - 4) >= name && strcmp(cp, "whgo") == 0) {

		/*
		 * First close channel 0.
		 */
		close(0);

		/*
		 * Get the status for the directory to mount on.
		 */
		cp = FMNTDIR;
		if (stat(cp, &wstat) < 0) {
			panicerr(HE_NODIR, 0);
		}

		/*
		 * Mount myself on the directory.
		 */
		if ((i = Mount(cp + 1, MNTDIRLEN)) != MNTDIRLU) {
			if (i == EBADFC) {

				/*
				 * We failed to mount, probably because another
				 * window handler is already running.
				 * Just exit.
				 */
				exit(1);
			}
			panicerr(HE_MOUNT, i);
		}

		/*
		 * Display the name and the version on the console.
		 */
		dnix(F_LOGC, "%s Ver. %s\n", WH_NAME, WH_VERSION);

		/*
		 * Make sure we are running in the background.
		 */
		if (fork() != 0) {
			exit(0);
		}

		/*
		 * Copy the stat structure to the shared memory page.
		 */
		cp = (char *)(&wstat);
		cp1 = (char *)(&cmpgp->userm[0]);

		/*
		 * We are starting up.
		 */
		i = STARTUP;
	}
	else {
		
		/*
		 * Copy the stat structure from the shared memory page
		 * to 'wstat'.
		 */
		cp = (char *)(&cmpgp->userm[0]);
		cp1 = (char *)(&wstat);

		/*
		 * We are not starting up.
		 */
		i = NOSTARTUP;
	}

	tmp = sizeof(struct stat);
	do {
		*cp1++ = *cp++;
	} while (--tmp != 0);

	/*
	 * If the name is 'whp' or 'whl', the screen shall just be turned.
	 */
	cp = name;
	if (strcmp(cp, "whp") == 0) {
		(byte *)cp1 = &pmsetup[0];
		tmp = HE_WHP;
	}
	else if (strcmp(cp, "whl") == 0) {
		(byte *)cp1 = &lmsetup[0];
		tmp = HE_WHL;
	}
	else {

		/*
		 * If we are not starting up, reset the mode of the console
		 * to its normal state.
		 */
		if (i == NOSTARTUP) {
			if (chmod(&console[0], CONMOD1) < 0) {
				panicerr(HE_NOCON, 0);
			}
		}

		/*
		 * The name is 'whgo' or 'whwait'. Wait for the window handler
		 * to be activated.
		 * Wait until an open request is received from a process. This
		 * channel will become the "super" channel.
		 * Also serve stat requests.
		 */
		do {
			if ((i = Waitreq(MNTDIRLU, &rqst, sizeof(struct ireq))) != sizeof(struct ireq)) {
				panicerr(HE_RQUE, i);
			}
			rn = rqst.ir_rn;
			tmp = NO;

			switch (rqst.ir_fc) {

			case F_CLOSE:		/* a close on a locate open */
				if (rqst.ir_opid > MAXWINS) {
					locflg[rqst.ir_opid-MAXWINS-1] = 0;
				}
				break;

			case F_OPEN:		/* activate the window handler */
				i = ENOENT;
				if (rqst.ir_bc == 9) {	/* length of nullterminated 'activate' */
					i = MAXWINS;
					tmp = YES;
				}
				Termreq(rn, i, OP_READ | OP_WRIT);
				break;

			case F_LOCATE:		/* a locate open */
				for (i = 0 ; i < MAXLOC && locflg[i] != 0 ; i++)
					;
				if (i >= MAXLOC) {
					i = EMFILE;	/* too many locate opens */
				}
				else {
					locflg[i] = 1;
					i += MAXWINS + 1;
				}
				Termreq(rn, i, OP_SDIR);
				break;

			case F_IOCR:		/* I/O control read */
			case F_IOCW:		/* I/O control write */
				if (rqst.ir_upar != SF_STAT) {
					i = EINVAL;	/* only stat allowed */
				}
				else {
					i = uwritbuf(rn, (char *)(&wstat), sizeof(struct stat));
				}
				Termreq(rn, i, 0);
				break;

			default:
				Termreq(rn, EBADFC, 0);
				break;
			}
		} while (tmp == NO);

		/*
		 * Decide which handler to execute.
		 */
		if (Inb(mapbuf.lad + MSTATUSOFFS) & POLAFLG) {
			cp = "whl";		/* landscape mode */
			tmp = HE_WHL;
			(byte *)cp1 = &lmsetup[0];
		}
		else {
			cp = "whp";		/* portrait mode */
			tmp = HE_WHP;
			(byte *)cp1 = &pmsetup[0];
		}

		/*
		 * Change the mode of the console so only the owner can
		 * write to it.
		 */
		if (chmod(&console[0], CONMOD2) < 0) {
			panicerr(HE_NOCON, 0);
		}
	}

	/*
	 * Open the console.
	 */
	if (open(&console[0], 2) != CON_LU) {
		panicerr(HE_CONOPN, 0);
	}

	/*
	 * Set up the mouse. A time out is used to prevent the computer to
	 * hang up if any error occurs.
	 */
	alarm(10);
	signal(SIGALRM, alrmtrap);
	for (i = 0 ; i < sizeof(pmsetup) ; i++) {
		while (ptokbrd(CON_LU, (byte *)cp1) < 0)
			;
		((byte *)cp1)++;
	}
	signal(SIGALRM, SIG_IGN);
	alarm(0);

	/*
	 * Ignore the terminate signal.
	 */
	signal(SIGTERM, SIG_IGN);

	/*
	 * Move to the '/usr/window' directory and execute the window handler.
	 */
	chdir("/usr/window");
	execl(cp, cp, NULL);

	/*
	 * Display an error on the console and then exit.
	 */
	panicerr(tmp, 0);
}	/* end of main() */



/*
 * Time out routine used when setting up the keyboard.
 */

alrmtrap()
{
	panicerr(HE_KYBRD, 0);
}	/* end of alrmtrap() */



/*
 * Routine to output a panic error code and status to the console and then
 * exit.
 */

panicerr(errcode, status)
int	errcode;		/* error code   */
int	status;			/* error status */
{
	dnix(F_LOGC, "%s: error code = %d, status = %d\n", name, errcode, status);
	exit(1);
}	/* end of panicerr() */



/*
 * Write data to another process. Returns EFAULT if it failed, otherwise the
 * size of the written data.
 * For an explanation, see the source file main2.c.
 */

int
uwritbuf(rqn, bp, bc)
int		rqn;		/* request number                    */
char		*bp;		/* pointer to area to copy data from */
register	bc;		/* size of the data                  */
{
	register	r;		/* temporary */

	if (Wubnwait(rqn, bp, bc, 0) != bc) {

		/*
		 * We failed, probably because the process is swapped. Use
		 * a type 1 request instead.
		 */
		while ((r = Writubuf(rqn, bp, bc, 0)) != bc) {
			if (r != EINTR) {
				bc = EFAULT;		/* we failed again */
				break;
			}
		}
	}

	return(bc);
}	/* end of uwritbuf() */
