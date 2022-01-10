/* main1.c */

/*
 * This file contains the main routine and some of the routines to make the
 * ABC1600 window handler behave as a handler. The other part is in the file
 * main2.c.
 * Also the routines to simulate the tty device driver is included.
 *
 * The handler should not be started directly from shell, use the whgo program
 * instead.
 */

/* 1984-08-09, Peter Andersson, Luxor Datorer AB (at DIAB) */

/*
 * Functions in this file:
 *	main()
 *	map_in_mem()
 *	consig()
 *	bugtrap()
 *	termtrap()
 *	terminate()
 *	openwin()
 *	closewin()
 *	termbad()
 *	term1que()
 *	iocntrl()
 *	cpcntlchr()
 *	sendkybrd()
 */



#include	"../hsrc/defs.h"
#include	"../hsrc/graphconst.h"
#include	"../hsrc/structs.h"
#include	"../hsrc/globals.h"
#include	"../hsrc/macros.h"
#include	"../hsrc/dnixmacros.h"
#include	<signal.h>
#include	<sys/stat.h>
#include	<dnix/ireq.h>
#include	<dnix/errno.h>
#include	<dnix/fcodes.h>
#include	"../../gdrv/x35g.h"



/*
 * Structure to hold the status structure for the directory which the handler
 * is mounted on. This is returned when a status request is received.
 */

struct	stat	wstat;

/*
 * Structure to hold the status structure for the console. This is returned
 * when a stat request is received in a window.
 */

struct	stat	cstat;

/*
 * Structure to receive a request.
 */

struct	ireq	rqst;



/*
 * The main routine.
 */

main(argc, argv)
int	argc;		/* number of arguments   */
char	**argv;		/* pointers to arguments */
{
	register		 lu;		/* logical unit number            */
	register		 i;		/* loop index                     */
	register	dword	 addr;		/* address                        */
	register struct	hwastruc *hwap;		/* pointer to hardware addresses  */
	register struct	ireq	 *rqstp;	/* pointer to received request    */
	register	char	 *cp1;		/* various uses                   */
	register	char	 *cp2;		/* various uses                   */
	int			 bugtrap();	/* takes care of fatal bugs       */
	int			 termtrap();	/* takes care of terminate signal */
	int			 consig();	/* keyboard interrupt routine     */
	dword			 map_in_mem();	/* map in memory routine          */

	hndname = *argv;		/* point to handler name */
	hwap = &hwa;

	/*
	 * Get the status for the device used to serve stat requests to a
	 * window. The device should be the same as /dev/null.
	 */
	if (stat("/dev/windev", &cstat) < 0) {
		panicerr(HE_NOCON, 0);
	}

	/*
	 * Map in the 2K memory area shared with the tty device driver and the
	 * I/O pages and compute addresses to the shared memory and to the
	 * ports on the ABC1600 graphic card.
	 */
	addr = map_in_mem(1, HE_MAPIO);
	cmpgp = (struct iogrst *)(addr + PAGEOFFS);
	hwap->y_sizeport = (hwap->x_sizeport = addr + SIZEOFFS) + 2;
	hwap->y_toport = (hwap->x_toport = addr + TOOFFS) + 2;
	hwap->y_fromport = (hwap->x_fromport = addr + FROMOFFS) + 2;
	hwap->mstatusport = addr + MSTATUSOFFS;
	hwap->mflagport = addr + MFLAGOFFS;

	/*
	 * Map in the graphic memory and compute some addresses.
	 */
	hwap->pictmemst = map_in_mem(3, HE_MAPGR);
	hwap->grphaddr = hwap->pictmemst + (GRPHYCOORD * PICTSIZE + GRPHXCOORD) / 8;

	/*
	 * Copy the status structure from the shared memory page.
	 */
	cp1 = (char *)(&wstat);
	cp2 = &cmpgp->userm[0];
	i = sizeof(struct stat);
	do {
		*cp1++ = *cp2++;
	} while (--i != 0);

	/*
	 * Set up the console in an appropriate way.
	 */
#ifdef LANDSCAPE
	conwrite("\033[?5l\033:1h\033[1v\033[?35h\033:1M", 23);
#else  PORTRAIT
	conwrite("\033[?5l\033:1l\033[1v\033[?35h\033:1M", 23);
#endif LANDSCAPE / PORTRAIT

	/*
	 * Set up things to use for graphics.
	 */
	setupgr();

	/*
	 * Initialize some things. The statements inside comments are included
	 * so it is possible to see which things must be initialized to zero.
	 */
	/* bg_cont = BLACK; */	/* pattern to use when drawing contours */
	/* inversevideo = NO; */	/* not inverse video            */
	/* keyin = NO; */	/* no key pressed yet                   */

	/* for (i = MAXWINS - 1 ; i >= 0 ; i--) { */
	/*	   wstatp[i] = NULL; */
	/* } */

	/* n_icons = 0; */		/* no icons set up */
	/* tot_icons = 0; */
	(char *)iconsave = mem_high = (char *)Sbrk(0);	/* init pointers */

	nwpart[MAXWINS] = 1;		/* background is one rectangle       */
	ltow[0] = MAXWINS;		/* the window on the highest level is
					   the background                    */
	/* wtol[MAXWINS] = 0; */	/* the level of the background is zero
					   i.e. the highest level            */

	(struct wpstruc *)cp1 = &wp[0];
	/* ((struct wpstruc *)cp1)->xl_p = 0; */
					/* the "window" on the highest level */
	((struct wpstruc *)cp1)->xr_p = SCXSIZE - 1;
					/* is the background, set the rec-   */
	/* ((struct wpstruc *)cp1)->yu_p = 0; */
					/* tangle for the background to be   */
	((struct wpstruc *)cp1)->yl_p = SCYSIZE - 1;
					/* the whole screen (in portrait
					   mode coordinates)                 */
	wpp[MAXWINS] = (struct wpstruc *)cp1;
					/* point to rectangle for background */
	rtotal = 1;			/* total number of rectangles        */
	/* nwin = 0; */			/* no windows open yet               */
	/* nnswin = 0; */		/* no non-special windows open yet   */

	/* invdata.inv_flg = OFF; */	/* no icon is inverted */

	/* curdata.cs_curvis = NO; */	/* text cursor is not visible */
	/* curdata.cs_bcnt = 0; */	/* counter must be 0          */
	/* curdata.cs_ccnt = 0; *	/* counter must be 0          */

	/*
	 * Get the tty parameters to use as initial values in windows.
	 */
	(void)ioctl(CON_LU, TCGETA, &ttydflt.td_driver);

	/*
	 * Catch some signals so we can clean up if a fatal error occurs in
	 * the window handler.
	 */
#ifndef NO_BUG_TRAP
	(void)signal(SIGILL, bugtrap);	/* illegal instruction */
	(void)signal(SIGBUS, bugtrap);	/* bus error           */
	(void)signal(SIGSEGV, bugtrap);	/* segment violation   */
#endif

	/*
	 * Put the console in raw mode, no echo.
	 */
	(struct sgttyb *)cp1 = &ttysave;
	(void)ioctl(CON_LU, TIOCGETP, (struct sgttyb *)cp1);
	((struct sgttyb *)cp1)->sg_flags |= O_RAW | O_CBREAK;	/* to enable SIGCHAR signal */
	((struct sgttyb *)cp1)->sg_flags &= ~O_ECHO;
	ttysflg = YES;
	(void)ioctl(CON_LU, TIOCSETP, (struct sgttyb *)cp1);

	(void)signal(SIGCHAR, consig);	/* this is a preliminary signal */
	cmpgp->mtbcnt = 0;		/* no substitute keys */
	cmpgp->mactive = 1;		/* tell the device driver to
					   handle the mouse in a special
					   way */

	/*
	 * Initialize flags for cursor blink routine in the terminal driver.
	 */
	cmpgp->whc.flgs = WHCURON | WHNOACT;

	/*
	 * Initialize the screen, i.e. clear the background with the desired
	 * pattern.
	 */
	scr_restore();

	/*
	 * Catch the terminate signal.
	 */
	(void)signal(SIGTERM, termtrap);

	/*
	 * Set up the font used for the window headers. If font A is not in the
	 * graphic memory, it is loaded, but this should not happen.
	 */
	hdf_data.fi_flag = NOFONT;
	(void)adjfont('A' - 'A', &hdf_data);

	/*
	 * The main loop. Read requests and serve them while taking care of
	 * characters entered from the keyboard, using a special purpose
	 * signal.
	 */
	rqstp = &rqst;
	for ( ; ; ) {
		if (keyin == MOUSE) {

			/*
			 * A change has occured to the mouse.
			 */
			mousehand();
		}
		else if (keyin == KEY) {

			/*
			 * There is something to read from the keyboard.
			 */
			ttyinput();
		}
		else {

			/*
			 * Wait for a request to come in.
			 */
			i = Waitreq(MNTDIRLU, rqstp, sizeof(struct ireq));
			if (i == EINTR) {

				/*
				 * We were interrupted by a signal, continue and read
				 * the console.
				 */
				continue;
			}

			else if (i != sizeof(struct ireq)) {
				panicerr(HE_RQUE, i);
			}

			/*
			 * Check if the window is just waiting to be closed and if so
			 * return bad status.
			 */
			i = rqstp->ir_fc;
			if (i == F_WRITE || i == F_READ || i == F_IOCR || i == F_IOCW) {
				lu = rqstp->ir_opid;
				if (lu < MAXWINS) {
					if (((struct winint *)cp1 = wstatp[lu]) == NULL ||
					    (((struct winint *)cp1)->status & WAITCLOS)) {
						Termreq(rqstp->ir_rn, EINVAL, 0);
						continue;
					}
				}
			}

			/*
			 * Serve the received request.
			 */
			switch (i) {

			case F_OPEN:		/* open a window */

				openwin();
				break;

			case F_CLOSE:		/* close */

				closewin();
				break;

			case F_IOCR:		/* i/o control read  */
			case F_IOCW:		/* i/o control write */

				iocntrl();
				break;

			case F_WRITE:		/* write to window */

				writewin();
				break;

			case F_READ:		/* read from window */

				readwin();
				break;

			case F_CANCEL:		/* cancel a request */

				cancelreq();
				break;

			case F_LOCATE:		/* locate request */

				lu = EMFILE;
				(sint *)cp1 = &locflg[0];
				for (i = 0 ; i < MAXLOC ; i++, ((sint *)cp1)++) {
					if (*((sint *)cp1) == 0) {
						*((sint *)cp1) = 1;
						lu = i + MAXWINS + 1;
						break;
					}
				}
				Termreq(rqstp->ir_rn, lu, OP_SDIR);
				break;

			default:		/* illegal function code */

				Termreq(rqstp->ir_rn, EBADFC, 0);
				break;
			}	/* switch (i) */
		}	/* else */
	}	/* for ( ; ; ) */
}	/* end of main() */



/*
 * Routine used to map in memory.
 * sel == 1: Map in shared memory and I/O ports.
 * sel == 3: Map in graphic memory.
 */

dword
map_in_mem(sel, errcode)
int	sel;		/* memory to map in selector        */
int	errcode;	/* error code to display if we fail */
{
	struct	mapseg	mapbuf;		/* structure used to map in memory */

	if ((sel = Segproc(&mapbuf, sizeof(struct mapseg), sel)) != sizeof(struct mapseg)) {
		panicerr(errcode, sel);
	}

	return((dword)mapbuf.lad);
}	/* end of map_in_mem() */



/*
 * Come here if a signal which indicates that there is something to read from
 * the console or that the mouse has changed in some way.
 */

void
consig()
{
	if (cmpgp->mflag != REPCHANGE) {
		keyin = MOUSE;
	}
	else {
		keyin = KEY;
	}
	(void)signal(SIGCHAR, consig);	/* set up preliminary SIGCHAR signal again */

	return;
}	/* end of consig() */



/*
 * Come here if a fatal bug occured.
 */

void
bugtrap(sig)
int	sig;		/* generated signal */
{
	(void)signal(SIGILL, SIG_DFL);	/* this signal is not reset */
	terminate();
	panicerr(HE_FATAL, sig);
}	/* end of bugtrap() */



/*
 * Come here if the catchable terminate signal is sent to the window handler.
 */

void
termtrap()
{
	terminate();
	Exit(0);
}	/* end of termtrap() */



/*
 * Routine used to clean up when a terminate signal is received or the
 * "super channel" is closed.
 */

void
terminate()
{
	register		i;		/* loop index                  */
	register struct	sgttyb	*ttysp;		/* pointer to saved tty status */
	register struct	winint	*wsp;		/* pointer to window status    */

	(void)signal(SIGTERM, SIG_IGN);		/* make sure ignored */

	/*
	 * Tell the tty device driver to stop handle the mouse in a different
	 * way.
	 */
	cmpgp->mactive = 0;

	/*
	 * Tell the tty device driver to stop blinking text cursor for the
	 * window handler.
	 */
	cmpgp->whc.flgs = 0;

	/*
	 * Terminate all pending read and write requests with bad status and
	 * send hang up signals to all processes running in the windows.
	 */
	i = MAXWINS - 1;
	do {
		wsp = wstatp[i];
		if (wsp != NULL) {
			termbad(i);
			Sigpgrp(wsp->pgid, SIGHUP);
		}
	} while (--i >= 0);

	/*
	 * Restore the console.
	 */
	ttysp = &ttysave;
	ttysp->sg_flags &= ~(O_RAW | O_CBREAK);
	ttysp->sg_flags |= O_ECHO;
	(void)ioctl(CON_LU, TIOCSETP, ttysp);

	/*
	 * Reset the screen.
	 */
#ifdef LANDSCAPE
	conwrite("\033:1h\033:0M\033[v\033[?35l", 17);
#else  PORTRAIT
	conwrite("\033:1l\033:0M\033[v\033[?35l", 17);
#endif LANDSCAPE / PORTRAIT

	return;
}	/* end of terminate() */



/*
 * Open a window, i.e. allocate space for one.
 */

void
openwin()
{
	register		lu;		/* lu number                */
	register		i;		/* loop index               */
	register struct	winint	*wsp;		/* pointer to window status */
	register struct	winint	**wspp;		/* to find free lu          */
	register struct	ireq	*rqstp;		/* pointer to request       */
	char			*allocmem();	/* memory allocator         */

	rqstp = &rqst;
	if (rqstp->ir_bc > 1) {

		/*
		 * File name given, this is not allowed.
		 */
		lu = ENOENT;
	}
	else {

		/*
		 * First find a free lu number.
		 */
		lu = EMFILE;		/* too many open files (windows) */
		wspp = &wstatp[0];
		for (i = 0 ; i < MAXWINS ; i++) {
			if (*wspp == NULL) {
				lu = i;

				/*
				 * Allocate memory for the window status
				 * structure.
				 */
				if ((wsp = (struct winint *)allocmem(sizeof(struct winint))) == NULL) {
					lu = ENOMEM;		/* no memory */
				}
				else {
					wsp->nsrque.q_cnt = 0;	/* no waiting read requests  */
					wsp->nswque.q_cnt = 0;	/* no waiting write requests */
					wsp->ksig = 0;		/* no keyboard signal        */
					wsp->status = 0;	/* not created yet           */
					wsp->hdrlen = 0;	/* no header yet             */
					wsp->zoomindex = 0;	/* no zoom list added yet    */
					wsp->zoomlist[1] = '\0';
					wsp->helplen = 1;	/* init length of help seq.  */
					wsp->help[0] = '?';	/* init help sequence        */
					wsp->alloccnt = 0;	/* no memory allocated yet   */
					wsp->pgid = -Getpgid(rqstp->ir_rn);  /* get process group id */

					/*
					 * Initialize the spray mask for the
					 * window.
					 */
					for (i = 0 ; i < 8 * sizeof(dword) ; i++) {
						wsp->spraym.sp_mask[i] = spraym.sp_mask[i];
					}
					*wspp = wsp;
				}
				break;
			}
			wspp++;
		}
	}

	/*
	 * Terminate the request.
	 */
	Termreq(rqstp->ir_rn, lu, OP_READ | OP_WRIT | OP_STRM | OP_NRAD);

	return;
}	/* end of openwin() */



/*
 * Close a window, disactivate the handler, or close a channel obtained from
 * a locate open.
 */

void
closewin()
{
	register	lu;	/* lu number to close */
	static	char	*exearg[] = { "whwait", NULL };

	lu = rqst.ir_opid;
	if (lu >= MAXWINS + 1) {

		/*
		 * It is a close of an lu returned to a locate open request.
		 */
		locflg[lu-MAXWINS-1] = 0;
	}
	else if (lu == MAXWINS) {

		/*
		 * In this case the handler shall be disactivated. This is done
		 * by executing the startup program again with "whwait" as
		 * argument 0.
		 */
		terminate();		/* restore */
		Exec("usr/window/whgo", 15, exearg);
		panicerr(HE_WHGO, 0);
	}

	else {

		/*
		 * Make sure that the screen is not dimmed.
		 */
		Screen_on(cmpgp);

		/*
		 * Terminate pending read and write requests with bad status.
		 */
		termbad(lu);

		/*
		 * Remove all set up icons for the window.
		 */
		rmicons(lu);

		/*
		 * Remove the window.
		 */
		removwin(lu);
	}

	return;
}	/* end of closewin() */



/*
 * Terminate all waiting read and write requests for a window with bad status.
 * Also remove the text copy buffer file for the window if present.
 */

void
termbad(lu)
int	lu;	/* window to terminate requests for */
{
	register struct	winint	*wsp;	/* pointer to window status */

	wsp = wstatp[lu];

	/*
	 * Terminate read requests.
	 */
	term1que(&wsp->nsrque);

	/*
	 * Terminate write requests.
	 */
	term1que(&wsp->nswque);

	/*
	 * Remove the text copy buffer file if present.
	 */
	if (wsp->status & CPTMPFILE) {
		cptmpname[CPTMPLEN-1] = 'A' + lu;
		(void)unlink(&cptmpname[0]);
	}

	return;
}	/* end of termbad() */



/*
 * Terminate all requests in one queue.
 */

void
term1que(rqp)
register struct	rqque	*rqp;	/* pointer to data describing queue */
{
	register		s;	/* status                    */
	struct		hireq	hi;	/* used to read handler info */

	while (--(rqp->q_cnt) >= 0) {
		s = Readhinfo(rqp->q_next, &hi, sizeof(struct hireq));
		if (s != sizeof(struct hireq)) {
			panicerr(HE_RHINFO, s);
		}
		Termreq(rqp->q_next, EINVAL, 0);
		rqp->q_next = hi.hir_1;
	}

	return;
}	/* end of term1que() */



/*
 * I/O control request.
 */

void
iocntrl()
{
	register		rqn;		/* request number            */
	register		lu;		/* logical unit number       */
	register		cmd;		/* i/o control function code */
	register		rstat;		/* terminate status          */
	register		tmp;		/* temporary                 */
	register struct	winint	*wsp;		/* pointer to window status  */
	register unsigned char	*str;
	register unsigned char	*subkp;		/* mouse substitute keys     */
	register union	buffer	*ubp;		/* pointer to copy buffer    */
	byte			kd;		/* 1 byte keyboard data      */
	int			intbuf;		/* 1 integer buffer          */
	struct		dfltchr	dchrbuf;	/* to set up new dflt font   */
#ifdef LANDSCAPE
	static		char	*exearg[] = { "whp", NULL };
#else  PORTRAIT
	static		char	*exearg[] = { "whl", NULL };
#endif LANDSCAPE / PORTRAIT

	ubp = &ubuf;
	rqn = rqst.ir_rn;
	lu = rqst.ir_opid;
	cmd = rqst.ir_upar;

	/*
	 * Make sure that the screen is not dimmed.
	 */
	if (cmd != SF_SYNC) {
		Screen_on(cmpgp);
	}

	if (lu > MAXWINS) {
		rstat = EINVAL;
		if (cmd == SF_STAT) {

			/*
			 * It is a stat request on the directory we are
			 * mounted on.
			 */
			rstat = uwritbuf(rqn, (char *)(&wstat), sizeof(struct stat), 0);
		}
	}
	else {
		rstat = 0;

		/*
		 * First check if it is a function code which can be issued
		 * both from a window and through the "super channel".
		 */
		switch (cmd) {

		case WINTEST:		/* test if the window handler is present */

			break;

		case WINMODE:		/* get current screen mode */

#ifdef LANDSCAPE
			ubp->modbuf.m_mode = M_LAND;
#else  PORTRAIT
			ubp->modbuf.m_mode = M_PORT;
#endif LANDSCAPE / PORTRAIT
			ubp->modbuf.m_rstat = W_OK;
			rstat = uwritbuf(rqn, (char *)(&ubp->modbuf), sizeof(struct modstruc), 0);
			break;

		case WINGETVIS:		/* get visible parts */

			rstat = getvisible();
			break;

		case WINCNT:		/* get number of open windows */

			ubp->nwbuf.nw_open = 0;
			tmp = MAXWINS - 1;
			do {
				if (wstatp[tmp] != NULL) {
					ubp->nwbuf.nw_open++;
				}
			} while (--tmp >= 0);
			ubp->nwbuf.nw_created = nwin;
			rstat = uwritbuf(rqn, (char *)(&ubp->nwbuf), sizeof(struct nwstruc), 0);
			break;

		case WPICTRD:		/* read the picture memory */

			rstat = pictrd();
			break;

		case PFNKLD:		/* load function keys */
		case PFNKRD:		/* read function keys */

			if ((rstat = ureadbuf(rqn, (char *)(&ubp->fnkbuf), sizeof(struct fnkst), 0)) >= 0) {
				if ((rstat = Ioctlr(CON_LU, cmd, &ubp->fnkbuf, sizeof(struct fnkst))) >= 0) {
					rstat = uwritbuf(rqn, (char *)(&ubp->fnkbuf),
					        sizeof(struct fnkst), 0);
				}
			}
			break;

		default:

			if (lu == MAXWINS) {

				/*
				 * It is a request through the "super channel".
				 */
				switch (cmd) {

				case WINTURN:	/* turn the screen, i.e. change handler */

					ubp->modbuf.m_rstat = W_OK;
					tmp = MAXWINS - 1;
					do {
						if (wstatp[tmp] != NULL) {
							ubp->modbuf.m_rstat = WE_OPEN;
						}
					} while (--tmp >= 0);
#ifdef LANDSCAPE
					ubp->modbuf.m_mode = M_PORT;
#else  PORTRAIT
					ubp->modbuf.m_mode = M_LAND;
#endif LANDSCAPE / PORTRAIT
					rstat = uwritbuf(rqn, (char *)(&ubp->modbuf),
						         sizeof(struct modstruc), 0);
					Termreq(rqn, rstat, 0);
					if (rstat >= 0 && ubp->modbuf.m_rstat == W_OK) {
						terminate();
						Exec("usr/window/whgo", 15, exearg);
						panicerr(HE_WHGO, 0);
					}
					return;

				case WINMSUB:	/* set mouse substitute keys */

					cmpgp->mtbcnt = 0;
					if ((rstat = ureadbuf(rqn, (char *)(&subkys),
					    sizeof(struct substit), 0)) >= 0) {

						/*
						 * Write the keys to the memory
						 * page shared with the tty
						 * device driver.
						 */
						str = &cmpgp->mtbstrt[0];
						subkp = &subkys.c_keys[0];
						tmp = SUBSTKEYS;
						do {
							*str++ = *subkp++;
						} while (--tmp != 0);
						if (subkys.c_initflg == ON) {
							cmpgp->mtbcnt = SUBSTKEYS;
						}
						else {
							cmpgp->mtbcnt = 1;
						}
					}
					break;

				case WINCHBG:	/* change background pattern */

					if ((rstat = ureadbuf(rqn, (char *)(&ubp->cbbuf),
					    sizeof(struct chbgstruc), 0)) >= 0) {
						chbgpatt();	/* change background */
					}
					break;

				case WINIVIDEO:		/* inverse video */

					invvideo(YES);
					break;

				case WINNVIDEO:		/* non-inverse video */

					invvideo(NO);
					break;

				case WINCHMPNT:		/* alter mouse pointer */

					if ((rstat = ureadbuf(rqn, (char *)(&ubp->npbuf),
					    sizeof(struct npstruc), 0)) >= 0) {
						ubp->npbuf.np_rstat = chmpnt(&mpdat);
						rstat = uwritbuf(rqn, (char *)(&ubp->npbuf),
						                 sizeof(struct npstruc), 0);
					}
					break;

				case WINRESTOR:		/* restore the whole screen */

					scr_restore();
					break;

				case WINSINIT:	/* set initial driver and terminal parameters */

					rstat = ureadbuf(rqn, (char *)(&ttydflt), sizeof(struct wininit), 0);
					break;

				case WINGINIT:	/* get initial driver and terminal parameters */

					rstat = uwritbuf(rqn, (char *)(&ttydflt), sizeof(struct wininit), 0);
					break;

				case PTOKBD:	/* write data to keyboard */

					if ((rstat = ureadbuf(rqn, (char *)(&kd), sizeof(byte), 0)) >= 0) {
						rstat = sendkybrd(kd);
					}
					break;

				default:

					rstat = EINVAL;
					break;

				}	/* switch (cmd) */
			}	/* if (lu == MAXWINS) */
			else {

				/*
				 * It is a request from a window.
				 */
				wsp = wstatp[lu];

				switch (cmd) {

				case WINCREAT:		/* create a window */
				case WINALTER:		/* alter a window group */
				case WIN1ALTER:		/* alter one window */
				case WINDFLSZ:		/* set default window size */

					/*
					 * Read the structure describing the
					 * window to be created or altered
					 * from the user's buffer.
					 */
					if ((rstat = ureadbuf(rqn, (char *)(&ubp->wbuf),
					    sizeof(struct winstruc), 0)) >= 0) {

						/*
						 * Now create or alter the
						 * window.
						 */
						if (cmd == WINCREAT) {
							ubp->wbuf.w_rstat = addwin(lu, &ubp->wbuf);
						}
						else {

							/*
							 * Convert from font
							 * box sizes to pixel
							 * sizes.
							 */
							if (ubp->wbuf.w_flags & TXTSIZE) {

								/*
								 * txttopix() needs the height
								 * of the window.
								 */
#ifdef LANDSCAPE
								ubp->wbuf.wl_ysize = wsp->x_size;
#else  PORTRAIT
								ubp->wbuf.wp_ysize = wsp->y_size;
#endif LANDSCAPE / PORTRAIT
								txttopix(&ubp->wbuf, wsp);
							}

							/*
							 * Convert the coordinates to internal
							 * coordinates.
							 */
							cnvcoord(&ubp->wbuf);
							if (cmd == WINALTER) {
								ubp->wbuf.w_rstat = alterwin(lu, &ubp->wbuf);
							}
							else if (cmd == WIN1ALTER) {
								ubp->wbuf.w_rstat =
								     alter1win(lu, &ubp->wbuf, NO);
							}
							else {
								ubp->wbuf.w_rstat =
								     setdflsz(&ubp->wbuf, wsp);
							}
						}

						/*
						 * Load the structure with the
						 * current values.
						 */
						winstat(lu, &ubp->wbuf);

						/*
						 * Write the updated structure
						 * back to the user's buffer.
						 */
						rstat = uwritbuf(rqn, (char *)(&ubp->wbuf),
						                 sizeof(struct winstruc), 0);

					}
					break;

				case WINLEVEL:		/* move window to top level */

					ubp->levbuf.l_rstat = chwglevel(lu, NO);

					/*
					 * Write the structure to the user's
					 * buffer.
					 */
					rstat = uwritbuf(rqn, (char *)(&ubp->levbuf),
							 sizeof(struct winlevel), 0);
					break;

				case WIN1LEV:	/* move window to top level in group */

					ubp->levbuf.l_rstat = chwtlig(lu);

					/*
					 * Write the structure to the user's
					 * buffer.
					 */
					rstat = uwritbuf(rqn, (char *)(&ubp->levbuf),
							 sizeof(struct winlevel), 0);
					break;

				case WINSTAT:

					if ((wsp->status & CREATED) == 0) {
						ubp->wbuf.w_rstat = WE_NOTCR;
					}
					else {
						winstat(lu, &ubp->wbuf);
						ubp->wbuf.w_rstat = W_OK;
					}

					/*
					 * Write the structure to the user's
					 * buffer.
					 */
					rstat = uwritbuf(rqn, (char *)(&ubp->wbuf),
							 sizeof(struct winstruc), 0);
					break;

				case WINHEADER:	/* insert a header in a window border */

					/*
					 * Read the structure describing the
					 * window header from the user's
					 * buffer.
					 */
					if ((rstat = ureadbuf(rqn, (char *)(&ubp->hbuf),
					     sizeof(struct headstruc), 0)) >= 0) {

						/*
						 * Now we are ready to insert
						 * the header.
						 */
						winheader(lu, &ubp->hbuf);
					}
					break;

				case WINICON:	/* add data for a new icon */

					/*
					 * Read the structure describing the
					 * icon from the user's buffer.
					 */
					if ((rstat = ureadbuf(rqn, (char *)(&ubp->icbuf),
					    sizeof(struct winicon), 0)) >= 0) {
						ubp->icbuf.i_rstat = addicon(lu, &ubp->icbuf);

						/*
						 * Write the structure back to
						 * the user's buffer.
						 */
						rstat = uwritbuf(rqn, (char *)(&ubp->icbuf),
								 sizeof(struct winicon), 0);
					}
					break;

				case RMICONS:	/* remove all icons for a window */

					rmicons(lu);
					break;

				case WINFLAGS:	/* alter the flags for a window */

					/*
					 * Read the structure with the new
					 * flags from the user's buffer.
					 */
					if ((rstat = ureadbuf(rqn, (char *)(&ubp->fbuf),
					    sizeof(struct flgstruc), 0)) >= 0) {
						ubp->fbuf.f_rstat = newflags(lu, &ubp->fbuf);

						/*
						 * Write the structure back to
						 * the user's buffer.
						 */
						rstat = uwritbuf(rqn, (char *)(&ubp->fbuf),
								 sizeof(struct flgstruc), 0);
					}
					break;

				case WINCURVIS:	/* make cursor visible */

					if (wsp->status & CREATED) {
						chkscroll(lu);
					}
					break;

				case WINCHMPNT:	/* alter mouse pointer */

					if ((rstat = ureadbuf(rqn, (char *)(&ubp->npbuf),
					    sizeof(struct npstruc), 0)) >= 0) {
						if ((wsp->status & CREATED) == 0) {
							ubp->npbuf.np_rstat = WE_NOTCR;
						}
						else if ((wsp->estatus & ALTMPNT) == 0) {
							ubp->npbuf.np_rstat = WE_NOMP;
						}
						else {
							ubp->npbuf.np_rstat = chmpnt(wsp->mppnt);
						}
						rstat = uwritbuf(rqn, (char *)(&ubp->npbuf),
								 sizeof(struct npstruc), 0);
					}
					break;

				case WINGETTXT:	/* get text contents of a virtual screen */

					rstat = gettext();
					break;

				case WINZOOM:	/* set up a new zoom list */

					rstat = wzoom();
					break;

				case WINNDCHR:	/* change to a new default font */

					if ((rstat = ureadbuf(rqn, (char *)(&dchrbuf),
					    sizeof(struct dfltchr), 0)) >= 0) {
						dchrbuf.dc_rstat = W_OK;
						if ((wsp->status & CREATED) == 0) {
							dchrbuf.dc_rstat = WE_NOTCR;
						}
#ifdef LANDSCAPE
						else if ((dchrbuf.dc_flags & Z_LMODE) == 0) {
							dchrbuf.dc_rstat = WE_ILMOD;
						}
#else  PORTRAIT
						else if ((dchrbuf.dc_flags & Z_PMODE) == 0) {
							dchrbuf.dc_rstat = WE_ILMOD;
						}
#endif LANDSCAPE / PORTRAIT
						else {
#ifdef LANDSCAPE
							tmp = dchrbuf.dcl_font;
							cmd = dchrbuf.dcl_x;
							rstat = dchrbuf.dcl_y;
#else  PORTRAIT
							tmp = dchrbuf.dcp_font;
							cmd = dchrbuf.dcp_x;
							rstat = dchrbuf.dcp_y;
#endif LANDSCAPE / PORTRAIT
							if ((tmp != 0 && (tmp < 'A' || tmp > 'Z')) ||
							    cmd <= 0 || (cur_d)cmd > wsp->rm_cur ||
							    rstat <= 0 || (cur_d)rstat > wsp->bm_cur) {
								dchrbuf.dc_rstat = WE_ILPARA;
							}
							else {
							    if (tmp == 0) {

								    /*
								     * Use next font in
								     * zoom list.
								     */
								    if ((tmp =
									wsp->zoomlist[++(wsp->zoomindex)])
									== '\0') {
									    wsp->zoomindex = 0;
									    tmp = wsp->zoomlist[0];
								    }
							    }

							    /*
							     * Change font.
							     */
							    dchrbuf.dc_rstat =
								 fontrplce(lu, tmp, cmd, rstat);
							}
						}

						/*
						 * Now write the structure
						 * back to the users buffer.
						 */
						rstat = uwritbuf(rqn, (char *)(&dchrbuf),
								 sizeof(struct dfltchr), 0);
					}
					break;

				case WINKYSIG:	/* set up keyboard signal */

					if ((rstat = ureadbuf(rqn, (char *)(&ubp->ksbuf),
					    sizeof(struct kysigst), 0)) >= 0) {
						if (ubp->ksbuf.ks_sig < 0 || ubp->ksbuf.ks_sig >= NSIG) {
							ubp->ksbuf.ks_rstat = WE_ILPARA;
						}
						else {
							ubp->ksbuf.ks_rstat = W_OK;
							wsp->ksig = ubp->ksbuf.ks_sig;
						}
						rstat = uwritbuf(rqn, (char *)(&ubp->ksbuf),
								 sizeof(struct kysigst), 0);
					}
					break;

				case WINUBOX:	/* set up a user defined box */

					if ((rstat = ureadbuf(rqn, (char *)(&ubp->ubbuf),
					    sizeof(struct userbox), 0)) >= 0) {
						ubp->ubbuf.bx_rstat = usrdbox(lu, &ubp->ubbuf);
						rstat = uwritbuf(rqn, (char *)(&ubp->ubbuf),
								 sizeof(struct userbox), 0);
					}
					break;

				case WINHELP:	/* set up a new help sequence */

					if ((rstat = ureadbuf(rqn, (char *)(&ubp->hlpbuf),
					    sizeof(struct helpst), 0)) >= 0) {

						/*
						 * Copy the sequence to the
						 * status structure for the
						 * window.
						 */
						(char *)str = &wsp->help[0];
						(char *)subkp = &ubp->hlpbuf.hlp_seq[0];
						tmp = 0;
						while (tmp < HLPSIZE && *((char *)subkp) != '\0') {
							*((char *)str)++ = *((char *)subkp)++;
							tmp++;
						}
						wsp->helplen = tmp;	/* save length of sequence */
					}
					break;

				case PSPRAY:	/* get a new spray mask */

#ifdef LANDSCAPE
					if ((rstat = ureadbuf(rqn, (char *)(&ubp->sprbuf),
					     sizeof(struct sprayst), 0)) >= 0) {

						/*
						 * Turn the bit 90 degrees
						 * clockwise.
						 */
						turn32x32(&ubp->sprbuf.sp_mask[0], &wsp->spraym.sp_mask[0]);
					}
#else  PORTRAIT
					rstat = ureadbuf(rqn, (char *)(&wsp->spraym),
							 sizeof(struct sprayst), 0);
#endif LANDSCAPE / PORTRAIT
					break;

				case IOCTYPE:

					rstat = TIOC;
					break;

				case TCSETAF:	/* first flush input and output queues */

					flushtty(wsp);

				case TCSETAW:		/* first flush output queue */

					flushout(lu);

				case TCSETA:	/* set parameters for the terminal */

					if ((rstat = ureadbuf(rqn, (char *)(&ubp->tmiobuf),
					    sizeof(struct termio), 0)) >= 0) {
						wsp->ttyd.t_iflag = ubp->tmiobuf.c_iflag;
						wsp->ttyd.t_oflag = ubp->tmiobuf.c_oflag;
						wsp->ttyd.t_cflag = ubp->tmiobuf.c_cflag;
						wsp->ttyd.t_lflag = ubp->tmiobuf.c_lflag;
						wsp->ttyd.t_ispeed = wsp->ttyd.t_ospeed =
							 wsp->ttyd.t_cflag & CBAUD;
						cpcntlchr(&ubp->tmiobuf.c_ccs[0], &wsp->ttyd.t_ccs[0]);
					}
					break;

				case TCGETA:	/* get terminal parameters */

					ubp->tmiobuf.c_iflag = wsp->ttyd.t_iflag;
					ubp->tmiobuf.c_oflag = wsp->ttyd.t_oflag;
					ubp->tmiobuf.c_cflag = wsp->ttyd.t_cflag;
					ubp->tmiobuf.c_lflag = wsp->ttyd.t_lflag;
					ubp->tmiobuf.c_line = 0;
					cpcntlchr(&wsp->ttyd.t_ccs[0],  &ubp->tmiobuf.c_ccs[0]);
					rstat = uwritbuf(rqn, (char *)(&ubp->tmiobuf),
							 sizeof(struct termio), 0);
					break;

				/*
				 * TCSBRK not supported.
				 */
				/*
				case TCSBRK:
					break;
				 */

				case TCFLSH:	/* flush input and/or output queues */

					if ((rstat = ureadbuf(rqn, (char *)(&intbuf),
					    sizeof(int), 0)) >= 0) {
						if (intbuf < 0 || intbuf > 2) {
							rstat = EINVAL;
						}
						else {
							if (intbuf != 1) {
								flushtty(wsp);
							}
							if (intbuf >= 1) {
								flushout(lu);
							}
						}
					}
					break;

				case TIOCEXCL:	/* set "exclusive-use" mode */

					wsp->ttyd.t_lflag |= XCLUDE;
					break;

				case TIOCNXCL:	/* turn off "exclusive-use" mode */

					wsp->ttyd.t_lflag &= ~XCLUDE;
					break;

				case TIOCSETP:	/* set the basic parameters, flush */

					flushout(lu);
					flushtty(wsp);

				case TIOCSETN:	/* as TIOCSETP, but no flush */

					if ((rstat = ureadbuf(rqn, (char *)(&ubp->pbuf),
					     sizeof(struct sgttyb), 0)) >= 0) {
						wsp->ttyd.t_ispeed = ubp->pbuf.sg_ispeed;
						wsp->ttyd.t_ospeed = ubp->pbuf.sg_ospeed;
						wsp->ttyd.t_ccs[VERASE] = ubp->pbuf.sg_erase;
						wsp->ttyd.t_ccs[VKILL] = ubp->pbuf.sg_kill;
						wsp->ttyd.t_oflag = 0;
						wsp->ttyd.t_iflag = 0;
						wsp->ttyd.t_lflag = 0;
						wsp->ttyd.t_cflag = (ubp->pbuf.sg_ispeed & CBAUD) | CREAD
								    | (wsp->ttyd.t_cflag & (CLOCAL | HUPCL));
						if ((ubp->pbuf.sg_ispeed & CBAUD) == B110) {
							wsp->ttyd.t_cflag |= CSTOPB;
						}
						tmp = ubp->pbuf.sg_flags;
						if (tmp & O_XTABS) {
							wsp->ttyd.t_oflag |= TAB3;
						}
						else if (tmp & O_TBDELAY) {
							if (tmp & O_TAB1) {
								wsp->ttyd.t_oflag |= TAB1;
							}
							else {
								wsp->ttyd.t_oflag |= TAB2;
							}
						}
						if (tmp & O_LCASE) {
							wsp->ttyd.t_iflag |= IUCLC;
							wsp->ttyd.t_oflag |= OLCUC;
							wsp->ttyd.t_lflag |= XCASE;
						}
						if (tmp & O_ECHO) {
							wsp->ttyd.t_lflag |= ECHO | ECHOE;
						}
						wsp->ttyd.t_lflag |= ECHOK;
						if (tmp & O_CRMOD) {
							wsp->ttyd.t_iflag |= ICRNL;
							wsp->ttyd.t_oflag |= ONLCR;
							if (tmp & O_CR1) {
								wsp->ttyd.t_oflag |= CR1;
							}
							if (tmp & O_CR2) {
								wsp->ttyd.t_oflag |= ONOCR | CR2;
							}
						}
						else {
							wsp->ttyd.t_oflag |= ONLRET;
							if (tmp & O_NL1) {
								wsp->ttyd.t_oflag |= CR1;
							}
							if (tmp & O_NL2) {
								wsp->ttyd.t_oflag |= CR2;
							}
						}
						if (tmp & O_RAW) {
							wsp->ttyd.t_ccs[VTIME] = 0;
							wsp->ttyd.t_ccs[VMIN] = 1;
							wsp->ttyd.t_iflag &= ~(ICRNL | IUCLC);
							wsp->ttyd.t_oflag &= ~OCRNL;
							wsp->ttyd.t_cflag |= CS8;
						}
						else if (tmp & O_CBREAK) {
							wsp->ttyd.t_ccs[VTIME] = 0;
							wsp->ttyd.t_ccs[VMIN] = 1;
							wsp->ttyd.t_iflag |=
							     BRKINT | IGNPAR | ISTRIP | IXON | IXANY;
							wsp->ttyd.t_oflag |= OPOST;
							wsp->ttyd.t_cflag |= CS7 | PARENB;
							wsp->ttyd.t_lflag |= ISIG;
						}
						else {
							wsp->ttyd.t_ccs[VEOF] = wsp->ttyd.t_ccs[VCEOF];
							wsp->ttyd.t_ccs[VEOL] = wsp->ttyd.t_ccs[VCEOL];
							wsp->ttyd.t_iflag |=
							     BRKINT | IGNPAR | ISTRIP | IXON | IXANY;
							wsp->ttyd.t_oflag |= OPOST;
							wsp->ttyd.t_cflag |= CS7 | PARENB;
							wsp->ttyd.t_lflag |= ICANON | ISIG;
						}
						wsp->ttyd.t_iflag |= INPCK;
						if (tmp & O_ODDP) {
							if (tmp & O_EVENP) {
								wsp->ttyd.t_iflag &= ~INPCK;
							}
							else {
								wsp->ttyd.t_cflag |= PARODD;
							}
						}
						if (tmp & O_VTDELAY) {
							wsp->ttyd.t_oflag |= FFDLY;
						}
						if (tmp & O_BSDELAY) {
							wsp->ttyd.t_oflag |= BSDLY;
						}
					}
					break;

				case TIOCGETP:	/* fetch the basic parameters for the terminal */

					ubp->pbuf.sg_ispeed = wsp->ttyd.t_ispeed;
					ubp->pbuf.sg_ospeed = wsp->ttyd.t_ospeed;
					ubp->pbuf.sg_erase = wsp->ttyd.t_ccs[VERASE];
					ubp->pbuf.sg_kill = wsp->ttyd.t_ccs[VKILL];
					tmp = 0;
					if ((wsp->ttyd.t_lflag & ICANON) == 0) {
						if ((wsp->ttyd.t_oflag & OPOST) == 0) {
							tmp |= O_RAW;
						}
						else {
							tmp |= O_CBREAK;
						}
					}
					if (wsp->ttyd.t_lflag & XCASE) {
						tmp |= O_LCASE;
					}
					if (wsp->ttyd.t_lflag & ECHO) {
						tmp |= O_ECHO;
					}
					if (wsp->ttyd.t_cflag & PARODD) {
						tmp |= O_ODDP;
					}
					else if (wsp->ttyd.t_iflag & INPCK) {
						tmp |= O_EVENP;
					}
					else {
						tmp |= O_ODDP | O_EVENP;
					}
					if (wsp->ttyd.t_oflag & ONLCR) {
						tmp |= O_CRMOD;
						if (wsp->ttyd.t_oflag & CR1) {
							tmp |= O_CR1;
						}
						if (wsp->ttyd.t_oflag & CR2) {
							tmp |= O_CR2;
						}
					}
					else {
						if (wsp->ttyd.t_oflag & CR1) {
							tmp |= O_NL1;
						}
						if (wsp->ttyd.t_oflag & CR2) {
							tmp |= O_NL2;
						}
					}
					if ((wsp->ttyd.t_oflag & TABDLY) == TAB3) {
						tmp |= O_XTABS;
					}
					else if (wsp->ttyd.t_oflag & TAB1) {
						tmp |= O_TAB1;
					}
					else if (wsp->ttyd.t_oflag & TAB2) {
						tmp |= O_TAB2;
					}
					if (wsp->ttyd.t_oflag & FFDLY) {
						tmp |= O_VTDELAY;
					}
					if (wsp->ttyd.t_oflag & BSDLY) {
						tmp |= O_BSDELAY;
					}
					ubp->pbuf.sg_flags = tmp;
					rstat = uwritbuf(rqn, (char *)(&ubp->pbuf),
							 sizeof(struct sgttyb), 0);
					break;

				case TIOCHPCL:	/* hangup on disconnect */

					wsp->ttyd.t_cflag |= HUPCL;
					break;

				case TIOCFLUSH:	/* flush input and output queues */

					flushout(lu);
					flushtty(wsp);
					break;

				case TIOCSETC:	/* set special characters */

					if ((rstat = ureadbuf(rqn, (char *)(&ubp->scbuf),
					    sizeof(struct tchars), 0)) >= 0) {
						wsp->ttyd.t_ccs[VINTR] = ubp->scbuf.t_intrc;
						wsp->ttyd.t_ccs[VQUIT] = ubp->scbuf.t_quitc;
						wsp->ttyd.t_ccs[VCEOF] = ubp->scbuf.t_eofc;
						wsp->ttyd.t_ccs[VCEOL] = ubp->scbuf.t_brkc;
					}
					break;

				case TIOCGETC:	/* get special characters */

					ubp->scbuf.t_intrc = wsp->ttyd.t_ccs[VINTR];
					ubp->scbuf.t_quitc = wsp->ttyd.t_ccs[VQUIT];
					ubp->scbuf.t_eofc = wsp->ttyd.t_ccs[VCEOF];
					ubp->scbuf.t_brkc = wsp->ttyd.t_ccs[VCEOL];
					ubp->scbuf.t_startc = CSTART;
					ubp->scbuf.t_stopc = CSTOP;
					rstat = uwritbuf(rqn, (char *)(&ubp->scbuf),
							 sizeof(struct tchars), 0);
					break;

				case SF_RDCHK:
				case FIORDCHK:	/* check if character on input */

					if (wsp->ttyd.t_outq.c_cc > 0 ||
					    (((wsp->ttyd.t_lflag & ICANON) == 0 &&
					    wsp->ttyd.t_inq.c_cc > 0) ||
					     ((wsp->ttyd.t_lflag & ICANON) && wsp->ttyd.t_delct > 0))) {
						rstat = 1;	/* character to read */
					}
					break;

				case SF_STAT:	/* it is a stat request on the window */

					rstat = uwritbuf(rqn, (char *)(&cstat), sizeof(struct stat), 0);
					break;

				default:

					rstat = EINVAL;
					break;

				}	/* switch (cmd) */
			}	/* else */
		}	/* switch (cmd) */
	}	/* else */

	/*
	 * Terminate the request.
	 */
	Termreq(rqn, rstat, 0);

	return;
}	/* end of iocntrl() */



/*
 * Copy the control character string in the termio structure from one area
 * to another.
 */

void
cpcntlchr(from, to)
unsigned char	*from;		/* area to copy from */
unsigned char	*to;		/* area to copy to   */
{
	register	i;		/* counter */

	i = NCC;
	do {
		*to++ = *from++;
	} while (--i != 0);

	return;
}	/* end of cpcntlchr() */



/*
 * Send the data specified as argument to the keyboard. This functions only
 * purpose is to save space.
 * This function returns the value returned by ptokbrd().
 */

int
sendkybrd(data)
char	data;
{
	return(ptokbrd(CON_LU, &data));
}	/* end of sendkybrd() */
