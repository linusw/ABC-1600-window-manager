/* main2.c */

/*
 * This file contains the second part of the routines to make the ABC1600
 * window handler behave as a handler. Also the routines to simulate the
 * tty device driver is included.
 */

/* 1984-11-01, Peter Andersson, Luxor Datorer AB */

/*
 * Functions in this file:
 *	writewin()
 *	readwin()
 *	cancelreq()
 *	cancque()
 *	ttyinput()
 *	putonque()
 *	win_1char()
 *	panicerr()
 *	flushtty()
 *	flushout()
 *	serveread()
 *	servewrit()
 *	cptext()
 *	redrawsig()
 *	ureadbuf()
 *	uwritbuf()
 *	conwrite()
 */



#include	"../hsrc/defs.h"
#include	"../hsrc/structs.h"
#include	"../hsrc/eglobals.h"
#include	"../hsrc/macros.h"
#include	"../hsrc/dnixmacros.h"
#include	<signal.h>
#include	<dnix/ireq.h>
#include	<dnix/errno.h>
#include	<dnix/fcodes.h>



/*
 * Structure to receive a request.
 */

extern	struct	ireq	rqst;



/*
 * Serve a write request to a window.
 */

void
writewin()
{
	register		lu;	/* logical unit number      */
	register		tmp;	/* temporary                */
	register struct	winint	*wsp;	/* pointer to window status */
	register struct	ireq	*rqstp;	/* pointer to request       */
	struct		hireq	hi;	/* to write handler info    */

	rqstp = &rqst;
	lu = rqstp->ir_opid;
	tmp = EWAIT;		/* impossible value */
	if (lu >= MAXWINS || ((wsp = wstatp[lu])->status & CREATED) == 0) {
		tmp = EBADLU;
	}
	else if (rqstp->ir_bc < 0) {
		tmp = EINVAL;
	}
	else if (rqstp->ir_bc == 0) {
		tmp = 0;
	}

	if (tmp != EWAIT) {
		Termreq(rqstp->ir_rn, tmp, 0);
	}
	else if ((wsp->status & TTSTOP) == 0) {

		/*
		 * The output is not stopped by CTRL-S, so we can serve immediately.
		 */
		servewrit(lu, NEWREQ);
	}
	else {

		/*
		 * The output is stopped so we have to put the request on the
		 * write queue.
		 */
		if (wsp->nswque.q_cnt <= 0) {
			wsp->nswque.q_cnt = 1;
			wsp->nswque.q_next = rqstp->ir_rn;
		}
		else {
			wsp->nswque.q_cnt++;
			hi.hir_1 = rqstp->ir_rn;
			tmp = Writhinfo(wsp->nswque.q_last, &hi, sizeof(struct hireq));
			if (tmp != sizeof(struct hireq)) {
				panicerr(HE_WHINFO, tmp);
			}
		}
		wsp->nswque.q_last = rqstp->ir_rn;
	}

	return;
}	/* end of writewin() */



/*
 * Serve a read request to a window (if possible).
 */

void
readwin()
{
	register struct	winint	*wsp;	/* pointer to window status */
	register		lu;	/* lu # = window #          */
	register		tmp;	/* temporary                */
	register struct	ireq	*rqstp;	/* pointer to request       */
	struct		hireq	hi;	/* to write handler info    */

	rqstp = &rqst;
	lu = rqstp->ir_opid;
	tmp = EWAIT;		/* impossible value */

	if (lu >= MAXWINS || ((wsp = wstatp[lu])->status & CREATED) == 0) {
		tmp = EBADLU;
	}
	else if (rqstp->ir_bc < 0) {
		tmp = EINVAL;
	}
	else if (rqstp->ir_bc == 0) {
		tmp = 0;
	}

	if (tmp != EWAIT) {
		Termreq(rqstp->ir_rn, tmp, 0);
	}
	else if (wsp->nsrque.q_cnt > 0) {

		/*
		 * We have already read requests waiting. Put the request on
		 * the queue.
		 */
		wsp->nsrque.q_cnt++;
		hi.hir_1 = rqstp->ir_rn;
		tmp = Writhinfo(wsp->nsrque.q_last, &hi, sizeof(struct hireq));
		if (tmp != sizeof(struct hireq)) {
			panicerr(HE_WHINFO, tmp);	/* can't write handler info */
		}
		wsp->nsrque.q_last = rqstp->ir_rn;
	}
	else if (wsp->ttyd.t_outq.c_cc > 0 || ((wsp->ttyd.t_lflag & ICANON) == 0 &&
		 wsp->ttyd.t_inq.c_cc > 0) || ((wsp->ttyd.t_lflag & ICANON) &&
		 wsp->ttyd.t_delct > 0)) {

		/*
		 * We can serve the request immediately.
		 */
		serveread(lu, NEWREQ);
	}
	else {

		/*
		 * We have to put the request on the queue.
		 */
		wsp->nsrque.q_cnt = 1;
		wsp->nsrque.q_next = wsp->nsrque.q_last = rqstp->ir_rn;

		/*
		 * Check if there is anything to copy from an other window.
		 */
		cptext(lu);
	}

	return;
}	/* end of readwin() */



/*
 * Cancel a request.
 */

void
cancelreq()
{
	register struct winint	*wsp;	/* pointer to window status */
	register		i;	/* loop index               */

	i = MAXWINS - 1;
	do {
		wsp = wstatp[i];
		if (wsp != NULL) {
			if (cancque(&wsp->nsrque) != NO || cancque(&wsp->nswque) != NO) {
				break;
			}
		}
	} while (--i >= 0);

	return;
}	/* end of cancelreq() */



/*
 * This function is a support function to cancelreq().
 * It checks one read or write request queue for a request to cancel and if
 * found terminates the request with cancel status.
 * YES is returned if the request was in the queue, otherwise NO.
 */

int
cancque(rq)
register struct	rqque	*rq;	/* queue to check */
{
	register		rn;	/* request to cancel               */
	register		cnt;	/* number of requests in the queue */
	register		s;	/* status                          */
	struct		hireq	hi;	/* to read and write handler info  */
	register struct	hireq	*hip;	/* pointer to 'hi'                 */
	register		rn1;	/* previous request in queue       */
	register		rn2;	/* current request in queue        */
	register		flag;	/* return value                    */

	hip = &hi;
	flag = NO;
	cnt = rq->q_cnt;
	if (cnt > 0) {

		/*
		 * There are requests on this queue.
		 */
		rn = rqst.ir_rn;	/* request to cancel */
		s = Readhinfo(rq->q_next, hip, sizeof(struct hireq));
		if (s != sizeof(struct hireq)) {
			panicerr(HE_RHINFO, s);
		}
		if (rq->q_next == rn) {

			/*
			 * The first request in the queue shall be canceled.
			 */
			rq->q_next = hip->hir_1;
			flag = YES;
		}
		else {
			rn1 = rq->q_next;
			while (--cnt > 0) {
				rn2 = hip->hir_1;	/* save request number */
				s = Readhinfo(rn2, hip, sizeof(struct hireq));
				if (s != sizeof(struct hireq)) {
					panicerr(HE_RHINFO, s);
				}
				if (rn2 == rn) {

					/*
					 * This request shall be canceled.
					 */
					s = Writhinfo(rn1, hip, sizeof(struct hireq));
					if (s != sizeof(struct hireq)) {
						panicerr(HE_WHINFO, s);
					}

					/*
					 * If it is the last request in the
					 * queue, update the last pointer.
					 */
					if (rn == rq->q_last) {
						rq->q_last = rn1;
					}
					flag = YES;
					break;
				}
				rn1 = rn2;
			}
		}
		if (flag != NO) {
			rq->q_cnt--;
			Termreq(rn, ECANCL, 0);
		}
	}	/* if (cnt > 0) */

	return(flag);
}	/* end of cancque() */



/*
 * This routine is called when one or more characters are received from the
 * console.
 * The characters are put on the input queue for the top window (if there are
 * any windows) and echoed if appropriate.
 * If possible, pending read requests for the top window are served.
 * If the output previously was stopped and now will be started, all pending
 * write requests for the top window are served.
 */

void
ttyinput()
{
	register	cnt;		/* # of chars in string */
	register	wn;		/* number of top window */

	keyin = NO;

	/*
	 * Make sure that there is anything to read from the console.
	 */
	if (rdchk(CON_LU) > 0) {

		/*
		 * Read the keyboard.
		 */
		while ((cnt = read(CON_LU, &inbuf[0], IBSIZE)) <= 0)
			;

		/*
		 * Now put the string on the input buffer for the level zero
		 * window.
		 */
		if (nwin > 0) {
			wn = ltow[0];
			if (wstatp[wn]->estatus & KEYSCROLL) {
				chkscroll(wn);		/* scroll if necessary */
			}
			putonque(wn, &inbuf[0], cnt, NO);
		}
	}

	return;
}	/* end of ttyinput() */



/*
 * Put the characters in the string 'str' on the input buffer for window 'wn'.
 * If possible, pending read requests for the window are served.
 * If the output previously was stopped and now will be started, all pending
 * write requests for the window are served.
 */

/*
 * Macro to put a character on the raw character input queue.
 * 'ch' is the character to insert and 'wsp' a pointer to the window status
 * structure.
 */
#define	Putc(ch,wsp)	{ *((wsp)->ttyd.t_inq.c_cl)++ = (ch); \
			  (wsp)->ttyd.t_inq.c_cc++; \
			  if ((wsp)->ttyd.t_inq.c_cl >= (wsp)->ttyd.t_inq.c_cb + TTYHOG) \
				  (wsp)->ttyd.t_inq.c_cl = (wsp)->ttyd.t_inq.c_cb; }

void
putonque(wn, str, cnt, repflag)
register	wn;		/* window number        */
unsigned char	*str;		/* input string         */
int		cnt;		/* # of chars in string */
register	repflag;	/* escape report flag   */
{
	register struct	winint	*wsp;		/* pointer to window status */
	register unsigned short	iflag;		/* input mode flags         */
	register unsigned short	lflag;		/* line discipline flags    */
	register		c;		/* character                */
	register		serveflg;	/* anything to serve flag   */

	wsp = wstatp[wn];
	iflag = wsp->ttyd.t_iflag;
	lflag = wsp->ttyd.t_lflag;
	serveflg = NO;

	/*
	 * Now process the characters one by one.
	 */
	while (--cnt >= 0) {
		c = *str++ & 0377;
		if (iflag & ISTRIP) {
			c &= 0177;
		}

		if (iflag & IXON) {
			if (c == CSTOP) {

				/*
				 * The output shall be stopped.
				 */
				wsp->status |= TTSTOP;
				continue;
			}
			else if (wsp->status & TTSTOP) {

				/*
				 * The output has previously been stopped by
				 * CTRL-S.
				 * NOTE: Here the IXANY flag is always supposed
				 * to be set:
				 * The output shall be started again. Serve all
				 * requests waiting to write in this window.
				 */
				wsp->status &= ~TTSTOP;
				servewrit(wn, OLDREQ);
			}
			if (c == CSTART) {
				continue;	/* skip */
			}
		}	/* if (iflag & IXON) */

		if (lflag & ISIG) {
			if ((unsigned char)c == wsp->ttyd.t_ccs[VQUIT] ||
			    (unsigned char)c == wsp->ttyd.t_ccs[VINTR]) {

				/*
				 * An INTERRUPT or QUIT signal shall be
				 * generated, but first flush the input and
				 * output queues for this window.
				 */
				if ((lflag & NOFLSH) == 0) {
					flushout(wn);
					flushtty(wsp);
					serveflg = NO;
				}
				if ((unsigned char)c == wsp->ttyd.t_ccs[VINTR]) {
					c = SIGINT;
				}
				else {
					c = SIGQUIT;
				}
				Sigpgrp(wsp->pgid, c);	/* send the signal */
				continue;		/* next character */
			}
		}

		if (c == '\n' && (iflag & INLCR)) {
			c = '\r';
		}
		else if (c == '\r') {
			if (iflag & IGNCR) {
				continue;	/* skip CR */
			}
			if (iflag & ICRNL) {
				c = '\n';
			}
		}

		if (wsp->ttyd.t_inq.c_cc >= TTYHOG - 1) {
			flushtty(wsp);		/* flush buffer if full */
			serveflg = NO;
		}

		if ((iflag & IUCLC) && c >= 'A' && c <= 'Z') {
			c += 'a' - 'A';		/* convert to lower case */
		}

		/*
		 * Now we are ready to put the character on the input queue.
		 */
		Putc(c, wsp);

		/*
		 * If it isn't canonical input or it is a character which
		 * completes an input line, we have one more item to serve
		 * read requests.
		 */
		if ((lflag & ICANON) == 0 || c == '\n' || (unsigned char)c == wsp->ttyd.t_ccs[VEOF]) {
			if (lflag & ICANON) {
				Putc(0377, wsp);	/* put in delimiter */
				wsp->ttyd.t_delct++;	/* one more item */
			}
			serveflg = YES;			/* we have something to serve with */
		}

		/*
		 * Finish by echo the character (if echo on).
		 */
		if ((unsigned char)c == wsp->ttyd.t_ccs[VERASE] && (lflag & (ICANON | ECHOE)
		    == (ICANON | ECHOE))) {
			c == '\b';
		}
		if (lflag & ECHO) {
			if (repflag != NO) {

				/*
				 * We are echoing a escape report sequence.
				 * In order not to destroy the parameters of
				 * a possible escape sequence which generated
				 * this report, this fix has been added.
				 * An report escape sequence is not echoed
				 * until an alphabetical letter has been
				 * found. This is OK since the report escape
				 * sequences are just ignored by eschand().
				 */
				if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
					repflag = NO;	/* start echo again */

					/*
					 * Avoid infinite recursion.
					 */
					wsp->status &= ~ESCMODE;
				}
			}
			else {

				/*
				 * Echo the character.
				 */
				win_1char(wn, c);
			}
		}
		if (lflag & ICANON) {
			if ((c == '\n' && (lflag & (ECHO | ECHONL)) == ECHONL) ||
			    ((unsigned char)c == wsp->ttyd.t_ccs[VKILL] && (lflag & ECHOK))) {
				win_1char(wn, '\n');
			}
			if ((unsigned char)c == wsp->ttyd.t_ccs[VERASE] && (lflag & ECHOE)) {
				win_1char(wn, ' ');
				win_1char(wn, '\b');
			}
		}
	}	/* while (--cnt >= 0) */

	/*
	 * Serve old requests if we have anything to serve with.
	 */
	if (serveflg != NO) {
		if (wsp->ksig != 0 && wsp->nsrque.q_cnt <= 0) {

			/*
			 * Send a signal indicating that there is something
			 * to read from the keyboard buffer.
			 */
			Sigpgrp(wsp->pgid, wsp->ksig);
		}
		serveread(wn, OLDREQ);
	}

	return;
}	/* end of putonque() */

#undef	Putc



/*
 * Output one character to the window 'wn'.
 */

void
win_1char(wn, c)
register	wn;
unsigned char	c;
{
	register struct	pntstruc *pdp;	 /* pointer to mouse pointer data */
	register struct	invsave	 *invp;	 /* pointer to inverted icon data */
	register	short	 mfsav;	 /* mouse pointer flag save       */
	register	sint	 invsav; /* save of icon invert flag      */
	register		 lev;	 /* window level                  */

	pdp = &pntdata;
	invp = &invdata;

	/*
	 * Stop text cursor blink.
	 */
	offblnk();

	/*
	 * Remove the mouse pointer.
	 */
	mfsav = pdp->pd_onoff;
	if (mfsav != OFF) {
		if ((pdp->pd_flags & PD_OLAP) == 0) {
			mpover();	/* find overlapped windows */
		}
		if (wstatp[wn]->status & MPOVER) {
			mp_off();	/* remove mouse pointer */
		}
	}

	/*
	 * Restore possible inverted icon.
	 */
	invsav = invp->inv_flg;
	if (invp->inv_wn == wn && invsav != OFF) {
		invicon();
		invp->inv_flg = OFF;
	}

	/*
	 * Turn off text cursor if necessary.
	 */
	lev = wtol[wn];
	if (lev == 0) {
		cursoff();
	}

	/*
	 * Now output the character.
	 */
	win_out(wn, &c, 1);
	if (lev == 0) {
		curson();	/* turn on cursor */
	}
	if (invsav != OFF && invp->inv_flg == OFF) {
		invicon();
		invp->inv_flg = ON;
	}
	if (mfsav != OFF && pdp->pd_onoff == OFF) {
		mp_on();	/* restore mouse pointer */
	}

	/*
	 * Turn on text cursor blink again.
	 */
	onblnk();

	return;
}	/* end of win_1char() */



/*
 * Routine to output a panic error code and status to the console and then
 * exit.
 */

void
panicerr(errcode, status)
int	errcode;	/* error code   */
int	status;		/* error status */
{

	/*
	 * Restore the console to normal mode.
	 */
	if (ttysflg != NO) {
		ttysave.sg_flags &= ~(O_RAW | O_CBREAK);
		ttysave.sg_flags |= O_ECHO;
		(void)ioctl(CON_LU, TIOCSETP, &ttysave);
	}

	(void)dnix(F_LOGC, "\033c%s: error code = %d, status = %d\n", hndname, errcode, status);

	Exit(1);
}	/* end of panicerr() */



/*
 * Flush the raw character input and output queues for window 'wn'.
 */

void
flushtty(wsp)
register struct	winint	*wsp;	/* pointer to window status */
{
	wsp->ttyd.t_inq.c_cc = 0;
	wsp->ttyd.t_inq.c_cf = wsp->ttyd.t_inq.c_cl = wsp->ttyd.t_inq.c_cb;
	wsp->ttyd.t_outq.c_cc = 0;
	wsp->ttyd.t_delct = 0;

	return;
}	/* end of flushtty() */



/*
 * Flush the output queue for a window.
 */

void
flushout(wn)
int	wn;		/* window to flush queue for */
{
	register struct	winint	*wsp;	/* pointer to window status */

	wsp = wstatp[wn];
	while (wsp->nswque.q_cnt > 0) {
		wsp->status &= ~TTSTOP;
		servewrit(wn, OLDREQ);
	}

	return;
}	/* end of flushout() */



/*
 * Serve a possible read request from window 'wn'.
 * On entry it is assumed that it exist an item to read in the raw input
 * character buffer. However, any pending read requests does not have to
 * exist if 'flag' is OLDREQ. If 'flag' is NEWREQ the request is assumed
 * to not be in the queue, instead the nessecary data are taken from the
 * request buffer.
 */

void
serveread(wn, flag)
int	wn;		/* window to serve read request for */
int	flag;		/* new or old request flag          */
{
	register unsigned char	*bp;	/* pointer used when converting */
	register unsigned char	*bp1;	/* pointer used when converting */
	register struct	winint	*wsp;	/* pointer to window status     */
	register unsigned char	c;	/* character                    */
	register unsigned char	mc;	/* map character                */
	register		rn;	/* request number               */
	register		bc;	/* byte count                   */
	register		cnt;	/* temporary                    */
	struct	ireq_hireq 	irhi;	/* to reread request            */

	static	unsigned char	maptab[] = {
		000,000,000,000,000,000,000,000,
		000,000,000,000,000,000,000,000,
		000,000,000,000,000,000,000,000,
		000,000,000,000,000,000,000,000,
		000,'|',000,000,000,000,000,'`',
		'{','}',000,000,000,000,000,000,
		000,000,000,000,000,000,000,000,
		000,000,000,000,000,000,000,000,
		000,000,000,000,000,000,000,000,
		000,000,000,000,000,000,000,000,
		000,000,000,000,000,000,000,000,
		000,000,000,000,000,000,'~',000,
		000,'A','B','C','D','E','F','G',
		'H','I','J','K','L','M','N','O',
		'P','Q','R','S','T','U','V','W',
		'X','Y','Z',000,000,000,000,000
	};

	wsp = wstatp[wn];
	if (flag == NEWREQ) {

		/*
		 * It is a new request coming in.
		 */
		rn = rqst.ir_rn;
		bc = rqst.ir_bc;
	}
	else if (wsp->nsrque.q_cnt <= 0) {
		return;		/* no read requests to serve */
	}
	else {

		/*
		 * We have a request on the queue to serve.
		 */
		wsp->nsrque.q_cnt--;	/* one request less on the queue */
		rn = wsp->nsrque.q_next;
		cnt = Rereadrh(rn, &irhi, sizeof(struct ireq), sizeof(struct hireq));
		if (cnt != sizeof(struct ireq_hireq)) {
			panicerr(HE_RRRQ, cnt);
		}
		wsp->nsrque.q_next = irhi.hirpa.hir_1;	/* next request in the queue */
		bc = irhi.irpa.ir_bc;
	}

	/*
	 * If there aren't anything on the output queue we have to collect a
	 * new item from the raw input queue.
	 */
	if (wsp->ttyd.t_outq.c_cc <= 0) {

restart:		/* jump back here if kill line */
		bp = wsp->ttyd.t_outq.c_cb;
		*bp++ = '\0';
		*bp++ = '\0';
		bp1 = bp;

		while (wsp->ttyd.t_inq.c_cc > 0) {	/* while anything left */
			c = *(wsp->ttyd.t_inq.c_cf)++;
			wsp->ttyd.t_inq.c_cc--;
			if (wsp->ttyd.t_inq.c_cf >= wsp->ttyd.t_inq.c_cb + TTYHOG) {
				wsp->ttyd.t_inq.c_cf = wsp->ttyd.t_inq.c_cb;
			}

			/*
			 * If canonical mode we have to do some conversions.
			 */
			if (wsp->ttyd.t_lflag & ICANON) {
				if (c == 0377) {	/* if delimiter */
					wsp->ttyd.t_delct--;
					break;
				}
				if (*(bp - 1) != '\\') {
					if (c == wsp->ttyd.t_ccs[VERASE]) {
						if (bp > bp1) {
							bp--;
						}
						continue;
					}
					if (c == wsp->ttyd.t_ccs[VKILL]) {
						goto restart;
					}
					if (c == wsp->ttyd.t_ccs[VEOF]) {
						continue;	/* skip EOF character */
					}
				}	/* if (*(bp - 1) != '\\') */

				else {
					mc = maptab[c];
					if (c == wsp->ttyd.t_ccs[VERASE] || c == wsp->ttyd.t_ccs[VKILL]) {
						mc = c;
					}
					if (mc != 0 && (mc == c || (wsp->ttyd.t_iflag & IUCLC))) {
						if (*(bp - 2) != '\\') {
							c = mc;
						}
						bp--;
					}
				}	/* else */

			}

			*bp++ = c;	/* insert next character in output queue */

		}	/* while (wsp->ttyd.t_inq.c_cc > 0) */

		wsp->ttyd.t_outq.c_cc = bp - bp1;
		wsp->ttyd.t_outq.c_cn = bp1;

	}	/* if (wsp->ttyd.t_outq.c_cc <= 0) */

	/* cnt = Min(wsp->ttyd.t_outq.c_cc, bc); */
	cnt = wsp->ttyd.t_outq.c_cc;
	if (bc < cnt) {
		cnt = bc;
	}
	bc = uwritbuf(rn, (char *)wsp->ttyd.t_outq.c_cn, cnt, 0);
	wsp->ttyd.t_outq.c_cc -= cnt;
	wsp->ttyd.t_outq.c_cn += cnt;

	/*
	 * Terminate the request.
	 */
	Termreq(rn, bc, 0);

	return;
}	/* end of serveread() */



/*
 * Serve write requests. If 'flag' is NEWREQ the request is supposed to
 * has been read from the request queue, if it is OLDREQ all write requests
 * on the queue for the window is served.
 */

void
servewrit(wn, flag)
register	wn;		/* window to serve write request for */
int		flag;		/* new or old request flag           */
{
	register struct	winint	 *wsp;	 /* pointer to window status      */
	register struct	invsave	 *invp;	 /* pointer to inverted icon data */
	register		 cnt;	 /* number of characters to send  */
	register		 offset; /* offset into user buffer       */
	register		 size;	 /* size of next chunk to write   */
	register		 lev;	 /* level of window to write in   */
	register		 rn;	 /* request number                */
	sint			 invsav; /* save of icon invert flag      */
	struct	ireq_hireq	 irhi;	 /* to reread requests            */

	wsp = wstatp[wn];
	invp = &invdata;
	lev = wtol[wn];

	/*
	 * Make sure that the screen is not dimmed.
	 */
	Screen_on(cmpgp);

	/*
	 * If it is requests on the queue that shall be served, serve until
	 * no more left or the output is stopped by CTRL-S.
	 */
	do {
		if (flag == NEWREQ) {

			/*
			 * It is a new request coming in.
			 */
			rn = rqst.ir_rn;
			cnt = rqst.ir_bc;
		}
		else if (wsp->nswque.q_cnt <= 0) {
			break;		/* nothing more to serve */
		}
		else {

			/*
			 * We have requests on the queue to serve.
			 */
			wsp->nswque.q_cnt--;
			rn = wsp->nswque.q_next;
			cnt = Rereadrh(rn, &irhi, sizeof(struct ireq), sizeof(struct hireq));
			if (cnt != sizeof(struct ireq_hireq)) {
				panicerr(HE_RRRQ, cnt);
			}
			wsp->nswque.q_next = irhi.hirpa.hir_1;
			cnt = irhi.irpa.ir_bc;
		}

		/*
		 * Stop text cursor blink.
		 */
		offblnk();

		/*
		 * Remove mouse pointer if necessary.
		 */
		if ((pntdata.pd_flags & PD_OLAP) == 0) {
			mpover();	/* find overlapped windows */
		}
		if (wsp->status & MPOVER) {
			mp_off();	/* remove mouse pointer */
		}

		/*
		 * Restore possible inverted icon if necessary.
		 */
		if ((invsav = invp->inv_flg) != OFF && invp->inv_wn == wn) {
			invicon();
			invp->inv_flg = OFF;
		}

		/*
		 * Remove text cursor if level zero window.
		 */
		if (lev == 0) {
			cursoff();
		}
		offset = 0;
		while (cnt > 0) {
			/* size = Min(cnt, CBUFSIZE); */
			size = CBUFSIZE;
			if (cnt < size) {
				size = cnt;
			}
			cnt -= size;
			if (ureadbuf(rn, &ubuf.cbuf[0], size, offset) >= 0) {
				win_out(wn, (unsigned char *)(&ubuf.cbuf[0]), size);
							/* output the characters */
			}
			offset += size;
		}
		if (lev == 0) {
			curson();
		}
		if (invsav != OFF && invp->inv_flg == OFF) {
			invicon();
			invp->inv_flg = ON;
		}
		if (pntdata.pd_onoff == OFF) {
			mp_on();	/* restore mouse pointer */
		}

		/*
		 * Enable text cursor blink again.
		 */
		onblnk();

		/*
		 * Terminate the write request.
		 */
		Termreq(rn, offset, 0);

	} while (flag != NEWREQ && (wsp->status & TTSTOP) == 0);

	if (wsp->estatus & WRITSCROLL) {
		chkscroll(wn);		/* scroll if necessary */
	}

	return;
}	/* end of servewrit() */



/*
 * If there are any buffered copied text on a temporary file for the window
 * 'wn', put text on the raw input queue and serve read requests until the
 * buffer gets full or all the text in the file has been copied.
 */

void
cptext(wn)
register	wn;		/* window to copy text to */
{
	register struct	winint	*wsp;	/* pointer to window status    */
	register		fd;	/* file descriptor             */
	register		chunk;	/* chunk to put on input queue */
	register		tmp;	/* temporary                   */

	wsp = wstatp[wn];
	if (wsp->status & CPTMPFILE) {

		/*
		 * There is a file with buffered text.
		 */
		cptmpname[CPTMPLEN-1] = 'A' + wn;
		if ((fd = open(&cptmpname[0], AM_READ)) < 0 || lseek(fd, wsp->cpseek, 0) < 0) {
			wsp->cplen = 0;
		}
		else {
			do {
				chunk = Div2(TTYHOG - 2 - wsp->ttyd.t_inq.c_cc);
				tmp = wsp->cplen - wsp->cpseek;
				if (tmp < chunk) {
					chunk = tmp;
				}
				if (chunk > IBSIZE) {
					chunk = IBSIZE;
				}
				if (chunk > 0) {
					if (read(fd, &inbuf[0], chunk) != chunk) {
						wsp->cplen = 0;
						break;
					}
					wsp->cpseek += chunk;
					putonque(wn, &inbuf[0], chunk, NO);
				}
			} while (chunk > 0);
		}
		if (fd >= 0) {
			(void)close(fd);
		}
		if (wsp->cpseek >= wsp->cplen) {
			(void)unlink(&cptmpname[0]);
			wsp->status &= ~CPTMPFILE;
		}
	}

	return;
}	/* end of cptext() */



/*
 * Send a signal to indicate that graphics in the window have to be redrawn.
 */

void
redrawsig(wsp)
struct	winint	*wsp;		/* pointer to window status */
{
	if (wsp->rsig != 0) {
		Sigpgrp(wsp->pgid, wsp->rsig);
	}

	return;
}	/* end of redrawsig() */



/*
 * Read data from another process. Returns EFAULT if it failed, otherwise the
 * size of the data.
 * The reason for doing it in this way is as follows: The type 1 request used
 * to read another process's data will lead to wait if the process is swapped.
 * Hence it can be interrupted by a signal (i.e. SIGCHAR). Therefore it is
 * better to use a type 2 request and retry with a type 1 request if we fail.
 * Generally the type 2 requests are 5-6 times faster than type 1.
 */

int
ureadbuf(rqn, bp, bc, offs)
int		rqn;		/* request number                          */
char		*bp;		/* pointer to area to receive the data     */
register	bc;		/* size of the data                        */
int		offs;		/* offset from start of the process buffer */
{
	register	r;		/* temporary */

	if (Rubnwait(rqn, bp, bc, offs) != bc) {

		/*
		 * We failed, probably because the process is swapped. Use
		 * a type 1 request instead.
		 */
		while ((r = Readubuf(rqn, bp, bc, offs)) != bc) {
			if (r != EINTR) {
				bc = EFAULT;		/* we failed again */
				break;
			}
		}
	}

	return(bc);
}	/* end of ureadbuf() */



/*
 * Write data to another process. Returns EFAULT if it failed, otherwise the
 * size of the written data. Se also ureadbuf().
 */

int
uwritbuf(rqn, bp, bc, offs)
int		rqn;		/* request number                          */
char		*bp;		/* pointer to area to copy data from       */
register	bc;		/* size of the data                        */
int		offs;		/* offset from start of the process buffer */
{
	register	r;		/* temporary */

	if (Wubnwait(rqn, bp, bc, offs) != bc) {

		/*
		 * We failed, probably because the process is swapped. Use
		 * a type 1 request instead.
		 */
		while ((r = Writubuf(rqn, bp, bc, offs)) != bc) {
			if (r != EINTR) {
				bc = EFAULT;		/* we failed again */
				break;
			}
		}
	}

	return(bc);
}	/* end of uwritbuf() */



/*
 * Write to the console.
 */

void
conwrite(s, len)
char	*s;		/* pointer to string to write */
int	len;		/* length of the string       */
{
	(void)write(CON_LU, s, len);

	return;
}	/* end of conwrite() */
