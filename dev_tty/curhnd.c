/*-------------------------------------------------------------------
 *
 *	Extract from driver showing what is done at 
 *	cursor blink interrupts (timeouts).
 *
 *	Jan Tjernberg, DIAB 1985-02-28
 *-------------------------------------------------------------------
 */

mouseint()
{

    char c;
    register int s;
    register struct clist *cp;

    extern window *curw;
    extern UCHAR grfstat;
    extern struct tty siotty[];

    cp = &(siotty[1].t_outq);
    iogrp.curstat |=COFFTO;


    if ((iogrp.curstat&(COFFWR+COFFCH))==0 && cp->c_cc>0) {
        /* write request is not beeing handled and */
	/* there are characters to echo */


	/* turn off cursor */
	if (curw->status&CURSORON) {
	    cursoff();
	}

	while (cp->c_cc>0) {
	    c = getc(cp);
	    s=spl1();
	    tty_out(&c,1);
	    splx(s);
	}
	if ((iogrp.curstat&CSTEADY) && (iogrp.curstat&COFF)==0) {
	    curson();
	}
	iogrp.curcnt=0;
    }
    
    if ((iogrp.curstat&(CSTEADY|COFF|COFFWR))==0) {
        if (iogrp.curcnt<=0) {
	    if (iogrp.blnkcnt) {
		if (curw->status&CURSORON) {
		    cursoff();
		}
		else {
		    curson();
		}
	    }
	    iogrp.curcnt=4;
	}
	iogrp.curcnt--;
    }
    else if (iogrp.whc.flgs&WHCURON &&
	     ((iogrp.whc.flgs&WHNOACT)==0)){
        WAITS;
	outb(MFLAG,grfstat|COMPMFL);
	outw(iogrp.whc.xsize,SIZEH);
	outw(iogrp.whc.ysize,SIZEL);
	outw(iogrp.whc.xpos,TOADH);
	outw(iogrp.whc.ypos,TOADL);
	outw(iogrp.whc.xpos,FROMH);
	outw(iogrp.whc.ypos,FROML);
	iogrp.whc.flgs ^= WHCPRES;
	WAITS;
	
    }
    if (iogrp.blnkcnt>=1) {
	if (iogrp.blnkcnt==1) {
            outb(MFLAG,grfstat|BLANKFL);
	}
	if (!(curw->status&NODIM)) {
	    iogrp.blnkcnt--;
	}
    }
    
    iogrp.curstat &= ~COFFTO;

    timeout(mouseint,0,MOUSET);
#ifdef MOUSE
    if (iogrp.led.cnt ) {
        /* send char to keyboard on cursor interupt  */
        CON_OUTDATA(fromleb());
    }
    else if (iogrp.curstat&MSON) {
        CON_OUTDATA(0x3c);	  /* start mouse request mode */
    }
#endif
}

