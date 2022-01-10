/*
 *	File : tty.c
 *	============
 *
 *	General tty subroutines
 */


#ifdef X35
#define GRAPH_X35
#endif
#ifdef DS60NS32
#define GRAPH_X35
#endif
#include "../h/types.h"
#include "../h/param.h"
#include "../h/syspar.h"
#include "../h/nodestr.h"
#include "../h/user.h"
#include "../h/extvars.h"
#include "../h/sysrut.h"
#include "../h/ioconf.h"
#include "../h/tty.h"
#include "../h/ioctl.h"
#include "../h/ports.h"
#include "../h/errno.h"
#include "../h/sig.h"
#include "../h/fcodes.h"
#ifdef	GRAPH_X35
#undef TRUE	/* collision */
#undef FALSE
#undef min
#include "../h/wind.h"
#include "../h/gpage.h"

extern UCHAR grfstat;

#endif

/*
 *	Flush function codes.
 */
#define	FOPEN (-1)
#define	FREAD	1
#define	FWRITE	2


char	partab[];

#define MAXTRSIZE	32	/* max transfer count with masked interrupt */
#define	CANBSIZ		256
char	canonb[CANBSIZ];	/* Buffer for erase and kill */


/*
 *	Input mapping for quoted characters, quoted character are replaced
 *	by corresponding non-zero value found in table.
 */

char	maptab[] ={
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
	'X','Y','Z',000,000,000,000,000,
};
/*eject*/
ttyopen(tp)
register struct tty *tp;
/*
 *	Open tty
 */
{
	tp->t_state &= ~WOPEN;
	tp->t_state |= ISOPEN;
	return(0);
}

ttypgrp(dev,tp)
dev_t	dev;
register struct tty *tp;
/*
 *	Assign process group to tty
 */
{
	if (tp->t_pgrp==0){
		tp->t_pgrp = runtcb->tc_pgrp;
	}
}
/*eject*/
ttychars(tp)
register struct tty *tp;
{
	/*
	 *	Set control characters in default mode
	 */
	static uchar_t ttcchar[NCC+2] = {
		CINTR,	CQUIT,	CERASE,	CKILL,	CEOF,
		0,	0,	0,	CEOF,	0
	};

	register int i;

	for( i=0; i< sizeof(ttcchar); i++) {
		tp->t_cc[i] = ttcchar[i];
	}
}

ttyclose(tp)
register struct tty *tp;
/*
 *	Reset tty on last close
 */
{
	tp->t_pgrp = 0;
	waitflushtty(tp, FREAD);
	tp->t_state &= ~(ISOPEN|WOPEN);
}
/*eject*/
ttioccomm(com, tp, addr, dev)
register struct tty *tp;
laddr_t addr;
/*
 *	General ioctl handler
 */
{
	unsigned t1;
	register unsigned t;
	struct sgttyb tb;
	struct termio cb;
	struct tc tun;
	register int rpar;

	rpar = 0;
	if( com == SF_CTER  ){
		ttypgrp(dev,tp);
		return(1);
	}
	if(com == SF_RDCHK){
		com = FIORDCHK;
	}
	switch(com) {

	case IOCTYPE:
		return(TIOC);

	case TCSETAW:
	case TCSETAF:
		if (com == TCSETAF) waitflushtty( tp, FREAD|FWRITE);
		else waitflushtty(tp, 0);
	case TCSETA:
		rpar = sizeof(cb);
		if( acopy(addr,ccontext,(laddr_t)&cb,0,sizeof(cb))) {
			rpar = EFAULT;
			break;
		}
		tp->t_iflag = cb.c_iflag;
		tp->t_oflag = cb.c_oflag;
		tp->t_cflag = cb.c_cflag;
		tp->t_lflag = cb.c_lflag;
		if(tp->t_ospeed != (tp->t_cflag & CBAUD)){
			tp->t_ispeed = tp->t_ospeed = tp->t_cflag & CBAUD;
		}
		copybytes(cb.c_ccs, tp->t_cc, NCC);
		return(rpar);

	case TCGETA:
		cb.c_iflag = tp->t_iflag;
		cb.c_oflag = tp->t_oflag;
		cb.c_cflag = tp->t_cflag;
		cb.c_lflag = tp->t_lflag;
		cb.c_line  = 0;
		copybytes(tp->t_cc, cb.c_ccs, NCC);
		rpar = sizeof(cb);
		if( acopy((laddr_t)&cb,0,addr,ccontext,sizeof(cb))) {
			rpar = EFAULT;
		}
		break;

	case TCSBRK:
		waitflushtty(tp, 0);
		/* send brake ..... */
		rpar = 1;
		break;

	case TCXONC:
		rpar = EINVAL;
		break;

	case TCFLSH:
		t1 = fuword(addr);
		if(t1 < 0 || t1 > 2){
			rpar = EINVAL;
		} else {
			switch(t1){
			case 0:
				t = FREAD;
				break;
			case 1:
				t = FWRITE;
				break;
			case 2:
				t = FREAD | FWRITE;
				break;
			}
			flushtty(tp,t);
			rpar = 1;
		}
		break;

		/*
			 * get discipline number, only allowed value = 0
			 */
	case TIOCGETD:
		t1 = 0;
		if(acopy((laddr_t)&t1,0,addr,ccontext,sizeof(t)))
			rpar = EFAULT;
		break;

		/*
			 * set line discipline (only allowed value = 0!)
			 */
	case TIOCSETD:
		if (acopy(addr,ccontext, (laddr_t)&t1,0, sizeof(t))) {
			rpar = EFAULT;
			break;
		}
		rpar = t1 ? ENXIO : 0 ;
		break;

		/*
			 * make tty exclusive
			 */
	case TIOCEXCL:
		tp->t_lflag |= XCLUDE;
		break;
	case TIOCNXCL:
		tp->t_lflag &= ~XCLUDE;
		break;

		/* Parameter setting (speed, echo etc. */
	case TIOCSETP:
		waitflushtty(tp,FREAD|FWRITE);
	case TIOCSETN:
		rpar = sizeof(tb);
		if (acopy(addr,ccontext, (laddr_t)&tb,0, sizeof(tb))) {
			return(EFAULT);
		}
		spl5();
		while (canon(tp) >= 0) ;
		spl0();
		tp->t_ispeed = tb.sg_ispeed;
		tp->t_ospeed = tb.sg_ospeed;
		tp->t_cc[VERASE] = tb.sg_erase;
		tp->t_cc[VKILL] = tb.sg_kill;
		tp->t_oflag = 0;
		tp->t_lflag = 0;
		tp->t_iflag = 0;
		tp->t_cflag = (tb.sg_ospeed & CBAUD) | CREAD |
			(tp->t_cflag & ( CLOCAL | HUPCL));
		if((tb.sg_ospeed&CBAUD)==B110) tp->t_cflag |= CSTOPB;
		t = tb.sg_flags;
		if(t & O_XTABS)
			tp->t_oflag |= TAB3;
		else if(t&O_TBDELAY) {
			if(t&O_TAB1)
				tp->t_oflag |= TAB1;
			else tp->t_oflag |= TAB2;
		}
		if(t&O_LCASE) {
			tp->t_iflag |= IUCLC;
			tp->t_oflag |= OLCUC;
			tp->t_lflag |= XCASE;
		}
		if (t&O_ECHO) tp->t_lflag |= ECHO|ECHOE;
		tp->t_lflag |= ECHOK;
		if(t&O_CRMOD) {
			tp->t_iflag |= ICRNL;
			tp->t_oflag |= ONLCR;
			if(t&O_CR1) tp->t_oflag |= CR1;
			if(t&O_CR2) tp->t_oflag |= ONOCR|CR2;
		} 
		else {
			tp->t_oflag |= ONLRET;
			if(t&O_NL1) tp->t_oflag |= CR1;
			if(t&O_NL2) tp->t_oflag |= CR2;
		}
		if (t&O_RAW) {
			tp->t_cc[VTIME] = 0;
			tp->t_cc[VMIN]  = 1;
			tp->t_iflag &= ~(ICRNL|IUCLC);
			tp->t_oflag &= ~ONLCR;
			tp->t_cflag |= CS8;
		} 
		else if (t&O_CBREAK) {
			tp->t_cc[VTIME] = 0;
			tp->t_cc[VMIN]  = 1;
			tp->t_iflag |= BRKINT|IGNPAR|ISTRIP|IXON|IXANY;
			tp->t_oflag |= OPOST;
			tp->t_cflag |= CS7|PARENB;
			tp->t_lflag |= ISIG;
		} 
		else {
			tp->t_cc[VEOF] = tp->t_cc[VCEOF];
			tp->t_cc[VEOL] = tp->t_cc[VCEOL];
			tp->t_iflag |= BRKINT|IGNPAR|ISTRIP|IXON|IXANY;
			tp->t_oflag |= OPOST;
			tp->t_cflag |= CS7|PARENB;
			tp->t_lflag |= ICANON|ISIG;
		}
		tp->t_iflag |= INPCK;
		if(t&O_ODDP)
			if(t&O_EVENP)
				tp->t_iflag &= ~INPCK;
			else
				tp->t_cflag |= PARODD;
		if(t&O_VTDELAY)
			tp->t_oflag |= FFDLY;
		if(t&O_BSDELAY)
			tp->t_oflag |= BSDLY;
		if(t & O_TANDEM){
			tp->t_iflag |= IXOFF;
		}
#ifdef	GRAPH_X35
		if((t & (O_CBREAK|O_RAW)) == (O_CBREAK|O_RAW)){
			tp->t_tcb = runtcb;
		} 
		else {
			tp->t_tcb = NULL;
		}
#endif
		break;

		/*
			 * Get current parameter settings
			 */
	case TIOCGETP:
		tb.sg_ispeed = tp->t_ispeed;
		tb.sg_ospeed = tp->t_ospeed;
		tb.sg_erase = tp->t_cc[VERASE];
		tb.sg_kill = tp->t_cc[VKILL];
		t = 0;
		if(!(tp->t_lflag&ICANON))
			if((tp->t_oflag&OPOST)==0)
				t |= O_RAW;
			else t |= O_CBREAK;
		if(tp->t_lflag & XCASE)
			t |= O_LCASE;
		if(tp->t_lflag & ECHO)
			t |= O_ECHO;
		if(tp->t_cflag & PARODD)
			t |= O_ODDP;
		else if(tp->t_iflag & INPCK)
			t |= O_EVENP;
		else t |= O_ODDP|O_EVENP;
		if(tp->t_oflag&ONLCR) {
			t |= O_CRMOD;
			if(tp->t_oflag&CR1)
				t |= O_CR1;
			if(tp->t_oflag&CR2)
				t |= O_CR2;
		} 
		else {
			if(tp->t_oflag&CR1)
				t |= O_NL1;
			if(tp->t_oflag&CR2)
				t |= O_NL2;
		}
		if ((tp->t_oflag &TABDLY)==TAB3)
			t |= O_XTABS;
		else if(tp->t_oflag&TAB1)
			t |= O_TAB1;
		else if(tp->t_oflag&TAB2)
			t |= O_TAB2;
		if(tp->t_oflag&FFDLY)
			t |= O_VTDELAY;
		if(tp->t_oflag&BSDLY)
			t |= O_BSDELAY;
		if(tp->t_iflag & IXOFF){
			t |= O_TANDEM;
		}
		tb.sg_flags = t;
		rpar = sizeof(tb);
		if (acopy((laddr_t)&tb, 0,addr,ccontext, sizeof(tb)))
			rpar = EFAULT;
		break;

		/*
			 * Hang up on close
			 */

	case TIOCHPCL:
		tp->t_cflag |= HUPCL;
		break;

	case TIOCFLUSH:
		flushtty(tp,FREAD|FWRITE);
		break;

		/*
			 * line discipline, ioctl - not implemented, always 0
			 */
	case DIOCSETP:
	case DIOCGETP:
		/* ioctl = nodev */
		break;

		/*
			 * Set control characters (break,quit etc.)
			 */
	case TIOCSETC:
		rpar = sizeof(tun);
		if (acopy(addr,ccontext,(laddr_t)&tun,0,sizeof(tun))) {
			rpar = EFAULT;
		} 
		else {
			tp->t_cc[VINTR] = tun.t_intrc;
			tp->t_cc[VQUIT] = tun.t_quitc;
			tp->t_cc[VCEOF] = tun.t_eofc;
			tp->t_cc[VCEOL] = tun.t_brkc;
			/*
					if(tp->t_lflag & ICANON) {
							tp->t_cc[VEOF] = tun.t_eofc;
							tp->t_cc[VEOL] = tun.t_brkc;
						}
					*/
		}
		break;

		/*
		         * Get control characters
		  	 */
	case TIOCGETC:
		tun.t_intrc=tp->t_cc[VINTR];
		tun.t_quitc=tp->t_cc[VQUIT];
		tun.t_eofc =tp->t_cc[VCEOF];
		tun.t_brkc = tp->t_cc[VCEOL];
		tun.t_startc=CSTART;
		tun.t_stopc =CSTOP;
		rpar = sizeof(tun);
		if(acopy((laddr_t)&tun,0,addr,ccontext,sizeof(tun)))
			rpar = EFAULT;
		break;

		/*
			 * Check if character on input
			 */
	case FIORDCHK:
		if ((tp->t_state&CARR_ON)==0)
			break;
		if(tp->t_canq.c_cc != 0)
			rpar = 1;
		else if((tp->t_lflag&(ICANON))==0 && tp->t_rawq.c_cc > 0)
			rpar = 1;
		else if((tp->t_lflag&(ICANON))!=0 && tp->t_delct >0)
			rpar = 1;

		break;
#ifdef GRAPH_X35
/* this is removed now, font are loaded automaticly 
	case PFONTLD:
		rpar=fontld(addr);
		break;
*/

	case PPICTLD:
		rpar=pictld(addr);
		break;

	case PPICTRD:
		rpar=pictrd(addr);
		break;

	case PFNKLD:
		rpar=fnkld(addr);
		break;

	case PFNKRD:
		rpar=fnkrd(addr);
		break;

	case PTOKBD:
		rpar=tokbd(addr);
		break;

	case PFLOAD:
		rpar=ttyfld(addr);
		break;

	case PFREMOVE:
		rpar=ttyfrm(addr);
		break;
#endif
	default:
		return(EINVAL);
	}
	return(rpar);
}
/*eject*/
waitflushtty(tp, flushmode)
register struct tty *tp;
/*
 *		Wait for all caharacters in output queue to go.
 */
{

	spl5();
	while (tp->t_outq.c_cc && tp->t_state&CARR_ON) {
		siostart(tp);
		tp->t_state |= OSLEEP;
		sleep(&tp->t_slpq,TTOPRI);
	}
	if(flushmode)
		flushtty(tp, flushmode);
	spl0();
}

flushtty(tp,cmd)
register struct tty *tp;
{
	/*
	 *	Clear tty queues selected from cmd. (READ AND WRITE).
	 */
	register s;

	if(cmd&FREAD)
		flushc(&tp->t_canq);
	wakeup(&tp->t_slpq);
	s = spl6();
	tp->t_state &= ~TTSTOP;
	(*devtab[major(tp->t_dev)].d_stop)(tp);
	if(cmd&FWRITE)
		flushc(&tp->t_outq);
	if(cmd&FREAD) {
		flushc(&tp->t_rawq);
		tp->t_delct = 0;
	}
	splx(s);
}
/*eject*/
canon(tp)
register struct tty *tp;
/*
 *	Copy input to clist, fix erase,kill and escape handling
 */
{
	register char *bp;
	char *bp1;
	register int c;
	int mc;

	spl5();
	if ((tp->t_lflag&ICANON)!=0 && tp->t_delct==0
	    || (tp->t_lflag&ICANON)==0 && tp->t_rawq.c_cc==0) {
		return(-1);
	}
	spl0();
loop:
	bp = &canonb[2];
	while ((c=getc(&tp->t_rawq)) >= 0) {
		if ((tp->t_lflag & ICANON) != 0) {
			if (c==0377) {
				tp->t_delct--;
				break;
			}
			if((c < ' ') || (bp[-1]!='\\')){
				if (c==tp->t_cc[VERASE]) {
					if (bp > &canonb[2])
						bp--;
					continue;
				}
				if (c==tp->t_cc[VKILL])
					goto loop;
				if (c==tp->t_cc[VEOF])
					continue;
			} 
			else {
				mc = maptab[c];
				if (c==tp->t_cc[VERASE] || c==tp->t_cc[VKILL])
					mc = c;
				if (mc && (mc==c || (tp->t_iflag&IUCLC))) {
					if (bp[-2] != '\\')
						c = mc;
					bp--;
				}
			}
		}
		*bp++ = c;
		if (bp>=canonb+CANBSIZ)
			break;
	}
	bp1 = &canonb[2];
	/* To avoid losing characters during big transfer */
	/* do it in chunks                                */
	while ((bp-bp1)>MAXTRSIZE){
		b_to_q(bp1,MAXTRSIZE,&tp->t_canq);
		bp1 += MAXTRSIZE;
	}
	b_to_q(bp1, bp-bp1, &tp->t_canq);

	if (tp->t_state&TBLOCK && tp->t_rawq.c_cc < TTYHOG/5) {
		if (putc(CSTART, &tp->t_outq)==0) {
			tp->t_state &= ~TBLOCK;
			ttstart(tp);
		}
	}

	return(bp-bp1);
}

/*eject*/
ttyinput(c, tp)
register c;
register struct tty *tp;
/*
 * Put character on raw input q, do echoing etc.
 */
{
	register iflag;
	register struct chan   *cp;


	iflag = tp->t_iflag;
#ifdef GRAPH_X35
	if ((minor (tp -> t_dev) & SIOCHMSK) == 1 && graphp) {
		if (iogrp.mouse.remkey) {
			/* mouse key not reported, do it now */
			if (!iogrp.mflag) {
				iogrp.mflag = iogrp.mouse.remkey;
				iogrp.mouse.remkey = 0;
				if ((tp -> t_oflag & OPOST)==0){
					if (tp -> t_tcb) {
						psignal (tp -> t_tcb, SIGCHAR);
					}
				}

			}
		}
		if ((c & 0x80) || iogrp.kbdstate) {
			/* key needing transformation or mouse sequence */
			replace (c, tp);
			return;
		}
		/* reset cursor counters */
		if (iogrp.blnkcnt==0) {
		    /* screen has been dimmed, skip first char */
		    outb(MFLAG,grfstat);
		    iogrp.blnkcnt=iogrp.blnkstr;
		    return;
		}
		
		iogrp.blnkcnt=iogrp.blnkstr;
		if (c != tp->t_cc[VQUIT] && c != tp->t_cc[VINTR] &&
		    tekrep(c,tp)) return; /* if tektronix GIN mode */
	}
#endif	GRAPH_X35
	if(iflag & ISTRIP)
		c &= 0177;
	else 
		c &= 0377;
	if(iflag & IXON) {
		if (tp -> t_state & TTSTOP) {
			if(c == CSTART || (c != CSTOP && iflag & IXANY)){
				tp -> t_state &= ~TTSTOP;
#ifdef GRAPH_X35
				if (((minor (tp -> t_dev) & SIOCHMSK) == 1) &&
				    graphp && tp->t_state&OSLEEP) {
					tp->t_state &= ~OSLEEP;
					wakeup(&tp->t_slpq);
				}
				else
#endif	GRAPH_X35
					ttstart (tp);
			}
		} 
		else {
			if (c == CSTOP) {
				tp -> t_state |= TTSTOP;
				(*devtab[major (tp -> t_dev)].d_stop) (tp);
			}
		}
		if( c==CSTART || c==CSTOP)
			return;
	}
	if (tp->t_lflag & ISIG) {
		if (c == tp->t_cc[VQUIT] || c == tp->t_cc[VINTR]) {
			flushtty(tp,FREAD|FWRITE);
			c = (c == tp->t_cc[VINTR]) ? SIGINT : SIGQUIT;
			signal (tp -> t_pgrp, c);
			return;
		}
	}
	if ( iflag & ICRNL && c == '\r')
		c = '\n';

	if (tp -> t_rawq.c_cc > TTYHOG) {
		flushc(&tp->t_rawq);
		tp->t_delct=0;
		tp->t_state &= ~TBLOCK;
	}
	if(iflag & IXOFF){
		if(tp->t_rawq.c_cc + tp->t_canq.c_cc > TTYHOG/2 &&
		   ((tp->t_state&TBLOCK)==0) ){
#ifdef GRAPH_X35
		    if (!((minor(tp->t_dev)&SIOCHMSK) ==1 && graphp)){
			/* don't want x-off to console */
#endif
			/* Send x-off character */
			putc(CSTOP, &tp->t_outq);
			tp->t_state |= TBLOCK;
			ttstart(tp);
#ifdef GRAPH_X35
		    }
#endif
		}
	}

	if (iflag & IUCLC && c >= 'A' && c <= 'Z')
		c += 'a' - 'A';
	putc (c, &tp -> t_rawq);
#ifdef	GRAPH_X35
	if( (tp->t_oflag & OPOST)==0 ) {
		if (tp -> t_tcb) {
			psignal (tp -> t_tcb, SIGCHAR);
		}
	}
#endif	GRAPH_X35
	if ( ! (tp->t_lflag & ICANON) || (c == '\n' || c == tp->t_cc[VEOF])) {
		if (tp->t_lflag & ICANON && putc (0377, &tp -> t_rawq) == 0)
			tp -> t_delct++;
		wakeup (&tp -> t_slpq);
	}
	if (tp->t_lflag & ECHO) {
		if (c == tp->t_cc[VERASE] && tp->t_lflag&ECHOE) {
			ttyoutput('\b',tp);
			ttyoutput(' ',tp);
			c = '\b';
		}
		ttyoutput (c, tp);

		if (c == tp -> t_cc[VKILL] && (tp->t_lflag & ICANON)) {
			ttyoutput ('\n', tp);
		}
#ifdef GRAPH_X35
		if (!(((minor (tp -> t_dev) & SIOCHMSK) == 1) && graphp)) {
			ttstart (tp);
		}
#else
		ttstart (tp);
#endif
	}
}
/*eject*/
ttyoutput(c, tp)
register c;
register struct tty *tp;
/*
 *	Put character on output q, add delays, expand tabs etc.
 */
{
	register char *colp;
	register ctype;

#ifdef GRAPH_X35
	if (((minor (tp -> t_dev) & SIOCHMSK) == 1) && graphp) {
		putc(c,&tp->t_outq);
		tty_ctr ();
		return;
	}
#endif
	/* Changed t_lflag&ICANON to t_oflag&OPOST,  LA */
	if (tp->t_oflag&OPOST) {
		/* Normal or cbreak mode */
		c &= 0177;
		if (c==CEOF)
			/* Skip EOF if not in RAW mode (avoid hanging up) */
			return;
	} 
	else {
		/* Raw mode */
		putc(c, &tp->t_outq);
		return;
	}
	if (c=='\t' && (tp->t_oflag&TABDLY)==TAB3) {
		/* Expand tabs to spaces */
		c = 8;
		do
		    ttyoutput(' ', tp);
		while (--c >= 0 && tp->t_col&07);
		return;
	}
	if (tp->t_oflag&OLCUC) {
		colp = "({)}!|^~'`";
		/* If upper-case-only terminals, output escapes */
		while(*colp++)
			if(c == *colp++) {
				ttyoutput('\\', tp);
				c = colp[-2];
				break;
			}
		if ('a'<=c && c<='z')
			c += 'A' - 'a';
	}
	if (c=='\n' && tp->t_oflag&ONLCR)
		/* Convert <nl> to <cr><lf> */
		ttyoutput('\r', tp);
	putc(c, &tp->t_outq);
	/*
		 * Calculate delays.
		 * The numbers here represent clock ticks
		 * and are not necessarily optimal for all terminals.
		 * The delays are indicated by characters above 0200.
		 * In raw mode there are no delays and the
		 * transmission path is 8 bits wide.
		 */
	colp = &tp->t_col;
	ctype = partab[c];
	c = 0;
	switch (ctype&077) {

		/* ordinary */
	case 0:
		(*colp)++;

		/* non-printing */
	case 1:
		break;

		/* backspace */
	case 2:
		if (*colp)
			(*colp)--;
		break;

		/* newline */
	case 3:
		ctype = (tp->t_oflag >> 8) & 03;
		if(ctype == 1) { /* tty 37 */
			if (*colp)
				c = ((unsigned)*colp>>4) + 3;
			if( c < (unsigned)6){
				c = (unsigned) 6;
			}
		} 
		else
			if(ctype == 2) { /* vt05 */
				c = 6;
			}
		*colp = 0;
		break;

		/* tab */
	case 4:
		ctype = (tp->t_oflag >> 11) & 03;
		if(ctype == 1) { /* tty 37 */
			c = 1 - (*colp | ~07);
			if(c < 5)
				c = 0;
		}
		*colp |= 07;
		(*colp)++;
		break;

		/* vertical motion */
	case 5:
		if(tp->t_oflag & VTDLY) /* tty 37 */
			c = 0177;
		break;

		/* carriage return */
	case 6:
		ctype = (tp->t_oflag >> 9) & 03;
		if(ctype == 1) { /* tn 300 */
			c = 5;
		} 
		else if(ctype == 2) { /* ti 700 */
			c = 10;
		}
		*colp = 0;
	}
	if(c)
		putc(c|0200, &tp->t_outq);
}
/*eject*/
ttrstrt(tp)
register struct tty *tp;
/*
 *	Restart tty
 */
{

	tp->t_state &= ~TIMEOUT;
	ttstart(tp);
}

ttstart(tp)
register struct tty *tp;
/*
 *	Start output on tty
 */
{
	register s;

	s = spl5();
	if((tp->t_state&(TIMEOUT|TTSTOP|BUSY)) == 0)
		siostart(tp);
	splx(s);
}
/*eject*/
ttread(tp,rn)
register NODE *rn;
register struct tty *tp;
/*
 *	Read characters from tty
 */
{
	register s;

	if ((tp->t_state&CARR_ON)==0)
		return(0);
	s = spl5();
	if (tp->t_canq.c_cc==0)
		while ((canon(tp)<0) && (tp->t_state&CARR_ON))
			/* There is a gap here as canon changes priority */
			sleep(&tp->t_slpq,TTIPRI);
	splx(s);
	while (tp->t_canq.c_cc && passc(getc(&tp->t_canq),rn)>=0)
		;
	return(tp->t_rawq.c_cc + tp->t_canq.c_cc);
}
/*eject*/
laddr_t
ttwrite(tp,rn)
register NODE *rn;
register struct tty *tp;
/*
 * Write characters to tty
 */
{
	register c;

	if ((tp->t_state&CARR_ON)==0)
		return(0);
	while (rn->n.rn.r_bcnt) {
		spl5();
		while (tp->t_outq.c_cc > TTHIWAT) {
			ttstart(tp);
			tp->t_state |= OSLEEP;
			sleep(&tp->t_slpq,TTOPRI);
		}
		spl0();
		if ((c = cpass(rn)) < 0)
			break;
		ttyoutput(c, tp);
	}
	ttstart(tp);
	return(0);
}
