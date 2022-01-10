/* mousehand1.c */

/*
 * This file contains routines to handle the mouse (part 1).
 */

/* 1984-08-29, Peter Andersson, Luxor Datorer AB (at DIAB) */

/*
 * Functions in this file:
 *	mousehand()
 *	cnvsubst()
 *	mchng()
 *	chkmp_on()
 *	mp_on()
 *	mp_off()
 *	leftmkey()
 *	midmkey()
 *	mblowup()
 *	mzoomwin()
 */


#include	"../hsrc/defs.h"
#include	"../hsrc/structs.h"
#include	"../hsrc/eglobals.h"
#include	"../hsrc/graphconst.h"
#include	"../hsrc/macros.h"
#include	"../hsrc/dnixmacros.h"
#include	<dnix/fcodes.h>



/*
 * Come here when a change to the mouse or one of the mouse substitute keys
 * has been pressed.
 */

void
mousehand()
{
	register		 wn;		/* window number         */
	register struct	icondat	 *iconp;	/* pointer to icon data  */
	register struct	pntstruc *pdp;		/* pointer to mouse data */
	register struct	iogrst	 *mpp;		/* pointer to shared mem */
	struct		icondat  *pointicon();
	int			 consig();

	pdp = &pntdata;
	mpp = cmpgp;

	/*
	 * Make sure the screen is not dimmed.
	 */
	Screen_on(mpp);

	if (mpp->mflag >= M_SKEY) {

		/*
		 * A mouse substitute key has been pressed.
		 */
		cnvsubst(RELEASED);
		pdp->pd_flags |= PD_KYBRD;
	}
	else {
		pdp->pd_flags &= ~PD_KYBRD;
		pdp->pd_xkpos = mpp->mlocx;
		pdp->pd_ykpos = mpp->mlocy;
	}

	/*
	 * Turn of text cursor blink.
	 */
	offblnk();

	switch (mpp->mflag) {

	case MOUSEMVD:				/* the mouse has moved */

		updmpos();			/* update mouse pointer position */
		mp_on();			/* display mouse pointer at new position */
		mchng(MOUSCHNG, MOUSREP);	/* serve mouse read requests if any */
		break;

	case LEMKEYPR:		/* the left key has been pressed */

		leftmkey();
		break;

	case LEMKEYRE:		/* the left key has been released */

		pdp->pd_flags &= ~PD_LPRESS;
		if ((wn = pwinno()) != MAXWINS && (iconp = pointicon(wn, I_RELEASE)) != NULL) {
			(void)sendicon(iconp);
		}
		else if ((pdp->pd_flags & PD_LSKIP) == 0) {
			mchng(MOUSCHNG | MOUSBCHNG, MOUSREP | MOUSBREP);
						/* serve mouse requests if any */
		}
		pdp->pd_flags &= ~PD_LSKIP;
		break;

	case MIMKEYPR:		/* the middle key has been released */

		midmkey();
		break;

	case MIMKEYRE:		/* the middle key has been pressed */

		pdp->pd_flags &= ~PD_MPRESS;
		if ((pdp->pd_flags & PD_MSKIP) == 0 && nwin > 0 && (wstatp[ltow[0]]->estatus & NOCPOUT)) {
			mchng(MOUSCHNG | MOUSBCHNG, MOUSREP | MOUSBREP);
		}
		pdp->pd_flags &= ~PD_MSKIP;
		break;

	case RIMKEYPR:		/* the right key has been pressed */

		/*
		 * Move a window (or window group) to the top level or move
		 * the top window (or window group) to the bottom.
		 */
		if (nnswin > 1) {

			/*
			 * First find out which window we are pointing into,
			 * if any.
			 * The window is the highest level window on which the
			 * mouse pointer is pointing.
			 */
			wn = pwinno();
			if (wtol[wn] >= nnswin) {

				/*
				 * We are not pointing to any non-special
				 * window, so move the top window to the
				 * bottom.
				 */
				wn = ltow[0];
			}

			(void)chwglevel(wn, YES);	/* change level */
		}	/* if (nnswin > 1) */
		break;

	}

	/*
	 * Enable text cursor blink again.
	 */
	onblnk();

	/*
	 * Restore to normal signal routine and reset the mouse communication
	 * flag.
	 */
	if (keyin != KEY) {
		keyin = NO;
	}
	mpp->mflag = REPCHANGE;
	(void)signal(SIGCHAR, consig);

	return;
};	/* end of mousehand() */



/*
 * Convert a mouse substitute key to the corresponding mouse communication
 * flags (i.e. the flags used by the communication between the window handler
 * and the tty driver to tell the window handler what type of change has
 * occured to the mouse).
 * If 'state' is PRESSED we are supposed to be in a state where a key on the
 * mouse is pressed (or the corresponding keyboard key has been pressed once).
 */

void
cnvsubst(state)
int	state;		/* which state we are in */
{
	register unsigned char	 flag;		/* pressed substitute key  */
	register unsigned char	 flag2;		/* replacement code        */
	register		 x;		/* mouse position - x      */
	register		 y;		/* mouse position - y      */
	register		 step;		/* normal mouse step       */
	register		 lstep;		/* long mouse step         */
	register struct	substit	 *subp;		/* mouse substitute keys   */
	register struct	iogrst	 *cmpp;		/* pointer to shared mem.  */
	register struct	pntstruc *pdp;		/* pointer to mouse status */

	subp = &subkys;
	cmpp = cmpgp;
	pdp = &pntdata;
	flag = cmpp->mflag;
	if (flag == subp->c_keys[S_ONOFF]) {
		if (cmpp->mtbcnt != 1) {
			cmpp->mtbcnt = 1;
		}
		else {
			cmpp->mtbcnt = SUBSTKEYS;
		}
		flag2 = M_ILLEGAL;	/* don't do anything more */
	}
	else if (flag == subp->c_keys[S_PCMD]) {
		if (state != RELEASED) {
			flag2 = LEMKEYRE;
		}
		else {
			flag2 = LEMKEYPR;
		}
	}
	else if (flag == subp->c_keys[S_CHWIN]) {
		if (state != RELEASED) {
			flag2 = RIMKEYRE;
		}
		else {
			flag2 = RIMKEYPR;
		}
	}
	else if (flag == subp->c_keys[S_MCA]) {
		if (state != RELEASED) {
			flag2 = MIMKEYRE;
		}
		else {
			flag2 = MIMKEYPR;
		}
	}
	else {

		/*
		 * It is a mouse movement key.
		 */
		flag2 = MOUSEMVD;
#ifdef LANDSCAPE
		x = pdp->pd_ympos;
		y = -pdp->pd_xmpos;
#else  PORTRAIT
		x = pdp->pd_xmpos;
		y = pdp->pd_ympos;
#endif LANDSCAPE / PORTRAIT
		step = subp->c_step;
		lstep = subp->c_lstep;
		if (flag == subp->c_keys[S_MPU]) {
			y -= step;
		}
		else if (flag == subp->c_keys[S_MPD]) {
			y += step;
		}
		else if (flag == subp->c_keys[S_MPL]) {
			x -= step;
		}
		else if (flag == subp->c_keys[S_MPR]) {
			x += step;
		}
		else if (flag == subp->c_keys[S_MPUL]) {
			y -= step;
			x -= step;
		}
		else if (flag == subp->c_keys[S_MPUR]) {
			y -= step;
			x += step;
		}
		else if (flag == subp->c_keys[S_MPDL]) {
			y += step;
			x -= step;
		}
		else if (flag == subp->c_keys[S_MPDR]) {
			y += step;
			x += step;
		}
		else if (flag == subp->c_keys[S_LMPU]) {
			y -= lstep;
		}
		else if (flag == subp->c_keys[S_LMPD]) {
			y += lstep;
		}
		else if (flag == subp->c_keys[S_LMPL]) {
			x -= lstep;
		}
		else if (flag == subp->c_keys[S_LMPR]) {
			x += lstep;
		}
		else if (flag == subp->c_keys[S_LMPUL]) {
			y -= lstep;
			x -= lstep;
		}
		else if (flag == subp->c_keys[S_LMPUR]) {
			y -= lstep;
			x += lstep;
		}
		else if (flag == subp->c_keys[S_LMPDL]) {
			y += lstep;
			x -= lstep;
		}
		else if (flag == subp->c_keys[S_LMPDR]) {
			y += lstep;
			x += lstep;
		}
		else {
			flag2 = M_ILLEGAL;	/* illegal key, shouldn't happen */
		}

		/*
		 * Set the new mouse position. It is not necessary to check
		 * that the coordinates are legal here.
		 */
#ifdef LANDSCAPE
		pdp->pd_xkpos = x;
		pdp->pd_ykpos = -y;
#else  PORTRAIT
		pdp->pd_xkpos = x;
		pdp->pd_ykpos = (SCYSIZE - 1) - y;
#endif LANDSCAPE / PORTRAIT
	}

	/*
	 * Set the type of occurence.
	 */
	cmpp->mflag = flag2;

	return;
}	/* end of cnvsubst() */



/*
 * The mouse has changed in some way.
 * Set mouse change flags for the top level window, and check if there is any
 * request to serve.
 */

void
mchng(ctyp, rtyp)
dword	ctyp;		/* type of change */
dword	rtyp;		/* type of report */
{
	register		wn;		/* window number            */
	register struct	winint	*wsp;		/* pointer to window status */

	if (nwin > 0) {
		wn = ltow[0];
		wsp = wstatp[wn];
		wsp->status |= ctyp;
		if (wsp->status & rtyp) {
			repmous(wn);
			wsp->status &= ~(MOUSREP | MOUSBREP | MOUSCHNG | MOUSBCHNG);
		}
	}

	return;
}	/* end of mchng() */



/*
 * First find out which mouse pointer we shall use and then turn the mouse
 * pointer on.
 */

void
chkmp_on()
{
	register		 wn;	/* window number            */
	register struct	winint	 *wsp;	/* pointer to window status */
	register struct	pntstruc *pdp;	/* pointer to mouse status  */

	pdp = &pntdata;
	if ((wn = pwinno()) == MAXWINS || ((wsp = wstatp[wn])->estatus & ALTMPNT) == 0 ||
	    pdp->pd_xmpos < wsp->avx_orig || pdp->pd_xmpos > wsp->avx_lrc ||
	    pdp->pd_ympos < wsp->avy_orig || pdp->pd_ympos > wsp->avy_lrc) {
		pdp->pd_mpp = &mpdat;
	}
	else {
		pdp->pd_mpp = wsp->mppnt;
	}

	/*
	 * Finish by turning the mouse pointer on.
	 */
	mp_on();

	return;
}	/* end of chkmp_on() */



/*
 * Turn the mouse pointer on.
 */

void
mp_on()
{
	register		 i;	/* loop index                      */
	register		 x;	/* x coordinate                    */
	register		 y;	/* y coordinate                    */
	register		 xsize;	/* x size                          */
	register		 ysize;	/* y size                          */
	register		 xoffs;	/* x offset                        */
	register		 yoffs;	/* y offset                        */
	register	dword	 *x1;	/* used to construct mouse pointer */
	register	dword	 *x2;
	register struct	mpstruc	 *mpp;	/* pointer to mouse pointer data   */
	register struct	hwastruc *hwap;	/* pointer to hardware addresses   */

	hwap = &hwa;
	mpp = pntdata.pd_mpp;
	x = pntdata.pd_xmpos - mpp->mp_xpnt;
	y = pntdata.pd_ympos - mpp->mp_ypnt;
	xsize = mpp->mp_xsize;
	ysize = mpp->mp_ysize;
	xoffs = 0;
	yoffs = 0;
	if (x < 0) {
		xsize += x;
		xoffs = -x;
		x = 0;
	}
	else if (x + xsize >= SCXSIZE) {
		xsize = (SCXSIZE - 1) - x;
	}
	if (y < 0) {
		ysize += y;
		yoffs = -y;
		y = 0;
	}
	else if (y + ysize >= SCYSIZE) {
		ysize = (SCYSIZE - 1) - y;
	}

	/*
	 * Move the contents of the screen to a save area.
	 */
	Waitsp();
	Outb(GSTATFLG, hwap->mflagport);	/* don't invert */
	Move(x, y, MOUSXCOORD + xoffs, MOUSYCOORD + yoffs, xsize, ysize, UX_UY);

	/*
	 * Construct the mouse pointer.
	 */
	x1 = (dword *)(hwap->pictmemst + (MOUSYCOORD * PICTSIZE + MOUSXCOORD) / 8);
	x2 = (dword *)(hwap->pictmemst + (SETUPYCOORD * PICTSIZE + SETUPXCOORD) / 8);
	Waitsp();
	for (i = 0 ; i <= mpp->mp_ysize ; i++) {
		Outl((Inl(x1) & mpp->mp_and[i]) | mpp->mp_or[i], x2);
		x1 += PICTSIZE / (8 * sizeof(dword));
		x2 += PICTSIZE / (8 * sizeof(dword));
	}

	/*
	 * Move out the mouse pointer on the screen (we do not need to set up
	 * the mover size again).
	 */
	Outw(x, hwap->x_toport);
	Outw(y, hwap->y_toport);
	Outw(SETUPXCOORD + xoffs, hwap->x_fromport);
	Outw(SETUPYCOORD + yoffs, hwap->y_fromport);

	/*
	 * Save data about the mouse pointer.
	 */
	(struct pntstruc *)x1 = &pntdata;
	((struct pntstruc *)x1)->pd_x = x;
	((struct pntstruc *)x1)->pd_y = y;
	((struct pntstruc *)x1)->pd_xsize = xsize;
	((struct pntstruc *)x1)->pd_ysize = ysize;
	((struct pntstruc *)x1)->pd_xoffs = xoffs;
	((struct pntstruc *)x1)->pd_yoffs = yoffs;
	((struct pntstruc *)x1)->pd_onoff = ON;

	return;
}	/* end of mp_on() */



/*
 * Turn the mouse pointer off.
 */

void
mp_off()
{
	register struct	hwastruc *hwap;	/* pointer to hardware addresses */
	register struct pntstruc *pdp;	/* pointer to mouse pointer data */

	pdp = &pntdata;
	if (pdp->pd_onoff == ON) {
		hwap = &hwa;
		pdp->pd_onoff = OFF;
		Waitsp();
		Outb(GSTATFLG, hwap->mflagport);
		Move(MOUSXCOORD + pdp->pd_xoffs, MOUSYCOORD + pdp->pd_yoffs,
		     pdp->pd_x, pdp->pd_y, pdp->pd_xsize, pdp->pd_ysize, UX_UY);
	}

	return;
}	/* end of mp_off() */



/*
 * The left key on the mouse has been pressed, check if something shall be
 * done and if so, do it.
 */

void
leftmkey()
{
	register		 x;	/* x coordinate of mouse pointer */
	register		 y;	/* y coordinate of mouse pointer */
	register		 left;	/* left side of area to check    */
	register		 up;	/* upper side of area to check   */
	register		 wn;	/* window we are pointing to     */
	register		 n;	/* counter                       */
	register struct	winint	 *wsp;	/* pointer to window status      */
	register struct	icondat	 *iconp;/* pointer to icon area data     */
	register struct	pntstruc *pdp;	/* pointer to mouse pointer data */
	register struct	boxactd	 *bxp;	/* pointer to box data           */
	struct		icondat	 *pointicon();

	pdp = &pntdata;

	/*
	 * First find out which window we are pointing into.
	 * The window is the highest level window  on which the mouse pointer
	 * is pointing.
	 */
	if ((wn = pwinno()) != MAXWINS) {

		/*
		 * We are pointing into a window.
		 */
		x = pdp->pd_xmpos;
		y = pdp->pd_ympos;
		pdp->pd_flags |= PD_LSKIP;	/* skip next time left button released */
		wsp = wstatp[wn];
		n = BOXCOUNT;
		bxp = &boxdata[0];

		do {
			if (wsp->bpresent & bxp->ba_flags) {

				/*
				 * This box is present in the window border.
				 */
				left = bxp->ba_xoffs;
				if (left >= 0) {
					left += wsp->abx_orig;
				}
				else {
					left += wsp->abx_lrc;
				}
				up = bxp->ba_yoffs;
				if (up >= 0) {
					up += wsp->aby_orig;
				}
				else {
					up += wsp->aby_lrc;
				}
				if (x >= left && x <= left + bxp->ba_xsiz &&
				    y >= up && y <= up + bxp->ba_ysiz) {

					/*
					 * Found pointed to box.
					 */
					switch (bxp->ba_act) {

					case BA_CLOS:	/* close box */

						/*
						 * If no signal is given, mark the window as
						 * just waiting to be closed and terminate all
						 * pending read and write requests with bad
						 * status. Otherwise send the signal to the
						 * process group.
						 */
						if (wsp->csig != 0) {
							Sigpgrp(wsp->pgid, wsp->csig);
						}
						else {
							wsp->status |= WAITCLOS;
							termbad(wn);
						}
						return;

					case BA_MOVE:	/* move box */
						mmovwin(wn);
						return;

					case BA_SIZE:	/* size box */
						msizwin(wn);
						return;

					case BA_ZOOM:	/* zoom box */
						mzoomwin(wn);
						return;

					case BA_BLOW:	/* blow up box */
						mblowup(wn);
						return;

					case BA_HELP:	/* help box */
						putonque(wn, &wsp->help[0], wsp->helplen, NO);
						return;

					case BA_USCR:	/* scroll up box */
						mscrollwin(wn, 2);
						return;

					case BA_DSCR:	/* scroll down box */
						mscrollwin(wn, 3);
						return;

					case BA_LSCR:	/* scroll left box */
						mscrollwin(wn, 0);
						return;

					case BA_RSCR:	/* scroll right box */
						mscrollwin(wn, 1);
						return;
					}
				}
			}

			/*
			 * Try next box.
			 */
			bxp++;
		} while (--n != 0);
#ifdef LANDSCAPE

		/*
		 * Check if we are pointing into a user box.
		 */
		if (y >= wsp->aby_orig + BOXOFFS && y <= wsp->aby_orig + (BOXOFFS + UBOXSIZE - 1)) {
			n = (wsp->abx_lrc - DLBWIDTH - x) / UBOXTSIZ;
			if (n >= 0 && n < wsp->ubxcrrnt) {
				up = wsp->abx_lrc - Muls(n, UBOXTSIZ) - DLBWIDTH;
				if (x <= up && x >= up - (UBOXSIZE - 1)) {

					/*
					 * We are pointing into a user box.
					 * Send a signal.
					 */
					Sigpgrp(wsp->pgid, (wsp->uboxp + n)->ub_sig);
					return;
				}
			}
		}

		/*
		 * Check if we are pointing to the vertical indicator.
		 */
		if (wsp->bpresent & BXI_VIND) {
			left = vindloc(wsp);
			up = wsp->aby_lrc - INDOFFS - PINDSIZE + 1;
			if (x >= left && x <= left + (WINDSIZE - 1) &&
			    y >= up && y <= up + (PINDSIZE - 1)) {
				vindscroll(wn, left);
				return;
			}
		}

		/*
		 * Check if we are pointing to the horizontal indicator.
		 */
		if (wsp->bpresent & BXI_HIND) {
			up = hindloc(wsp);
			left = wsp->abx_orig + INDOFFS;
			if (x >= left && x <= left + (PINDSIZE - 1) &&
			    y >= up && y <= up + (WINDSIZE - 1)) {
				hindscroll(wn, up);
				return;
			}
		}
#else  PORTRAIT

		/*
		 * Check if we are pointing into a user box.
		 */
		if (x >= wsp->abx_orig + BOXOFFS && x <= wsp->abx_orig + (BOXOFFS + UBOXSIZE - 1)) {
			n = (y - wsp->aby_orig - DLBWIDTH) / UBOXTSIZ;
			if (n >= 0 && n < wsp->ubxcrrnt) {
				up = wsp->aby_orig + Muls(n, UBOXTSIZ) + DLBWIDTH;
				if (y >= up && y <= up + (UBOXSIZE - 1)) {

					/*
					 * We are pointing into a user box.
					 * Send a signal.
					 */
					Sigpgrp(wsp->pgid, (wsp->uboxp + n)->ub_sig);
					return;
				}
			}
		}

		/*
		 * Check if we are pointing to the vertical indicator.
		 */
		if (wsp->bpresent & BXI_VIND) {
			up = vindloc(wsp);
			left = wsp->abx_lrc - INDOFFS - PINDSIZE + 1;
			if (x >= left && x <= left + (PINDSIZE - 1) &&
			    y >= up && y <= up + (WINDSIZE - 1)) {
				vindscroll(wn, up);
				return;
			}
		}

		/*
		 * Check if we are pointing to the horizontal indicator.
		 */
		if (wsp->bpresent & BXI_HIND) {
			left = hindloc(wsp);
			up = wsp->aby_lrc - INDOFFS - PINDSIZE + 1;
			if (x >= left && x <= left + (WINDSIZE - 1) &&
			    y >= up && y <= up + (PINDSIZE - 1)) {
				hindscroll(wn, left);
				return;
			}
		}
#endif LANDSCAPE / PORTRAIT

		/*
		 * Check if we are pointing into an "icon-command" area.
		 */
		if ((iconp = pointicon(wn, I_PRESS)) != NULL) {
			(void)sendicon(iconp);
			return;
		}
	}

	/*
	 * Otherwise other processes can read the change.
	 */
	if (pdp->pd_flags & PD_KYBRD) {
		pdp->pd_flags ^= PD_LPRESS;
	}
	else {
		pdp->pd_flags |= PD_LPRESS;
	}
	pdp->pd_flags &= ~PD_LSKIP;

	mchng(MOUSCHNG | MOUSBCHNG, MOUSREP | MOUSBREP);
					/* serve possible mouse read request */

	return;
}	/* end of leftmkey() */



/*
 * The middle key on the mouse has been pressed, copy text.
 */

void
midmkey()
{
	register struct	winint	*wsp;	/* pointer to window status        */
	register		x;	/* upper left corner of contour    */
	register		y;
	register		xsize;	/* size of contour                 */
	register		ysize;
	register		i;	/* loop index and temporary        */
	register		tmp;	/* temporary                       */
	int			upc;	/* first row to copy               */
	int			rows;	/* number of rows to copy          */
	int			leftc;	/* first column to copy            */
	int			cols;	/* number of columns to copy       */
	int			wns;	/* source window                   */
	int			wnd;	/* destination window              */
	register unsigned char	*strs;	/* text to copy, source            */
	register unsigned char	*strd;	/* destination buffer              */
	register unsigned char	*strt;	/* temporary pointer while copying */

	(struct pntstruc *)strs = &pntdata;
	x = ((struct pntstruc *)strs)->pd_xmpos;
	y = ((struct pntstruc *)strs)->pd_ympos;

	/*
	 * First find out which window we are pointing into.
	 * The window is the highest level window on which the mouse pointer
	 * is pointing.
	 */
	if ((wns = pwinno()) == MAXWINS || ((wsp = wstatp[wns])->estatus & SAVETEXT) == 0
	    || (wsp->status & GRAPHIC) || (wsp->estatus & NOCPOUT)
#ifdef LANDSCAPE
	    || (pix_d)x > wsp->avx_lrc || (pix_d)y < wsp->avy_orig) {
#else  PORTRAIT
	    || (pix_d)x < wsp->avx_orig || (pix_d)y < wsp->avy_orig) {
#endif LANDSCAPE / PORTRAIT

		/*
		 * Processes can read the change.
		 */
		if (((struct pntstruc *)strs)->pd_flags & PD_KYBRD) {
			((struct pntstruc *)strs)->pd_flags ^= PD_MPRESS;
		}
		else {
			((struct pntstruc *)strs)->pd_flags |= PD_MPRESS;
		}
		((struct pntstruc *)strs)->pd_flags &= ~PD_MSKIP;
		if (nwin > 0 && (wstatp[ltow[0]]->estatus & NOCPOUT)) {
			mchng(MOUSCHNG | MOUSBCHNG, MOUSREP | MOUSBREP);
		}
		return;
	}

	/*
	 * Skip the next time the middle key is released.
	 */
	((struct pntstruc *)strs)->pd_flags |= PD_MSKIP;

	/*
	 * Compute coordinates for the lines to surround the pointed to
	 * character.
	 */
#ifdef LANDSCAPE
	xsize = wsp->fdata.fi_ysize;
	ysize = wsp->fdata.fi_xsize;
	leftc = (y - wsp->y_orig) / ysize;
	y = Muls(leftc, ysize) + wsp->y_orig;
	upc = (wsp->x_orig + wsp->x_size - 1 - x) / xsize;
	x = wsp->x_orig + wsp->x_size - Muls(upc, xsize) - xsize;
#else  PORTRAIT
	xsize = wsp->fdata.fi_xsize;
	ysize = wsp->fdata.fi_ysize;
	leftc = (x - wsp->x_orig) / xsize;
	x = Muls(leftc, xsize) + wsp->x_orig;
	upc = (y - wsp->y_orig) / ysize;
	y = Muls(upc, ysize) + wsp->y_orig;
#endif LANDSCAPE / PORTRAIT
	xsize--;
	ysize--;

	if ((pix_d)x >= wsp->avx_orig && (pix_d)(x + xsize) <= wsp->avx_lrc &&
	    (pix_d)y >= wsp->avy_orig && (pix_d)(y + ysize) <= wsp->avy_lrc) {

		/*
		 * We are pointing into a window and the text in the window is
		 * saved.
		 */
		mp_off();
		contour(x, y, xsize, ysize, bg_cont);	/* draw a contour */
		mp_on();

		for ( ; ; ) {

			/*
			 * Wait until a change from the mouse.
			 */
			waitmous(PRESSED);

			/*
			 * Remove the contour.
			 */
			mp_off();
			contour(x, y, xsize, ysize, PATTERN);
			mp_on();

			/*
			 * Check what has happened.
			 */
			if (cmpgp->mflag == MOUSEMVD) {

				/*
				 * The mouse has moved, remove the old mouse pointer
				 * and compute the new position.
				 */
				updmpos();
#ifdef LANDSCAPE
				tmp = wsp->fdata.fi_ysize;
				i = xsize;
				xsize = (x + xsize - pntdata.pd_xmpos) / tmp + 1;
				xsize = Muls(xsize, tmp);
				xsize--;
				x -= xsize - i;
				while ((pix_d)x < wsp->avx_orig) {
					x += tmp;
					xsize -= tmp;
				}
				if (xsize < 0) {
					x += xsize;
					xsize = 0;
				}

				tmp = wsp->fdata.fi_xsize;
#else  PORTRAIT
				tmp = wsp->fdata.fi_xsize;
				xsize = (pntdata.pd_xmpos - x) / tmp + 1;
				xsize = Muls(xsize, tmp);
				xsize--;
				while ((pix_d)(x + xsize) > wsp->avx_lrc) {
					xsize -= tmp;
				}
				if (xsize < 0) {
					xsize = 0;
				}

				tmp = wsp->fdata.fi_ysize;
#endif LANDSCAPE / PORTRAIT
				ysize = (pntdata.pd_ympos - y) / tmp + 1;
				ysize = Muls(ysize, tmp);
				ysize--;
				while ((pix_d)(y + ysize) > wsp->avy_lrc) {
					ysize -= tmp;
				}
				if (ysize < 0) {
					ysize = 0;
				}

				/*
				 * Redraw the mouse pointer and the contour.
				 */
				contour(x, y, xsize, ysize, bg_cont);
				mp_on();
			}
			else if (cmpgp->mflag == MIMKEYRE) {

				/*
				 * The middle key has been released.
				 * Compute the text area to move.
				 */
#ifdef LANDSCAPE
				cols = (ysize + 1) / wsp->fdata.fi_xsize;
				rows = (xsize + 1) / wsp->fdata.fi_ysize;
#else  PORTRAIT
				cols = (xsize + 1) / wsp->fdata.fi_xsize;
				rows = (ysize + 1) / wsp->fdata.fi_ysize;
#endif LANDSCAPE / PORTRAIT

				if (rows > 0 && cols > 0) {

					/*
					 * It is a non-empty area.
					 * Wait until the destination window is
					 * pointed out.
					 */
					mp_off();
					contour(x, y, xsize, ysize, bg_cont);	/* show the contour */
					mp_on();

					for ( ; ; ) {

						/*
						 * Wait until a change from the mouse.
						 */
						waitmous(RELEASED);

						/*
						 * Check what has happened.
						 */
						if (cmpgp->mflag == MOUSEMVD) {

							/*
							 * The mouse has moved.
							 * Remove the mouse pointer
							 * and get the new position.
							 */
							updmpos();

							/*
							 * Redraw the mouse pointer.
							 */
							mp_on();
						}
						else if (cmpgp->mflag == MIMKEYPR) {

						    /*
						     * The middle key has been pressed to point
						     * out the destination window.
						     */
						    mp_off();
						    contour(x, y, xsize, ysize, PATTERN);
						    mp_on();
						    if ((wnd = pwinno()) != MAXWINS) {
							wsp = wstatp[wnd];
							if ((wsp->status & CPTMPFILE) ||
							    (wsp->estatus & NOCPIN)) {
							    return;
							}

							/*
							 * Found pointed to window.
							 */
							cptmpname[CPTMPLEN-1] = 'A' + wnd;
							if ((x = creat(&cptmpname[0], CPTMPPROT)) < 0) {
							    return;	/* could not create */
							}
							wsp->cplen = 0;
							strd = &inbuf[0];
							i = rows;
							do {
							    strs = *(wstatp[wns]->savc + upc) + leftc;
							    tmp = cols;
							    while (tmp > 0) {
								strt = strd;
								while (strt - strd < IBSIZE - 1 && tmp > 0) {
								    *strt++ = *strs++ & CMASK;
								    tmp--;
								}

								/*
								 * Remove trailing spaces.
								 */
								while (--strt >= strd && *strt == ' ')
								    ;
								strt++;
								if ((wsp->ttyd.t_iflag & ICRNL) == 0) {
								    *strt = '\r';
								}
								else {
								    *strt = '\n';
								}
								y = strt - strd + 1;
								wsp->cplen += y;
								if (write(x, strd, y) != y) {
								    (void)close(x);
								    (void)unlink(&cptmpname[0]);
								    return;	/* error */
								}
							    }
							    upc++;	/* next row */
							} while (--i > 0);
							(void)close(x);
							wsp->status |= CPTMPFILE;
							wsp->cpseek = 0;
							cptext(wnd);
							break;	/* ready */
						    }
						    break;
						}
						else {

						    /*
						     * If something else happened, abort.
						     */
						    mp_off();
						    contour(x, y, xsize, ysize, PATTERN);
						    mp_on();
						    break;
						}
					}	/* for ( ; ; ) */
				}	/* if (rows > 0 && cols > 0) */
				break;
			}
			else {

				/*
				 * If something else happened, abort.
				 */
				break;
			}

		}	/* for ( ; ; ) */
	}	/* if ((pix_d)x >= wsp->avx_orig && ... ) */

	return;
}	/* end of midmkey() */



/*
 * The blow up box has been chosen.
 */

void
mblowup(wn)
register	wn;		/* window number */
{
	register struct	winint	 *wsp;	/* pointer to window status */
	register struct	winstruc *wc;	/* to alter the window      */

	wsp = wstatp[wn];
	wc = &ubuf.wbuf;

#ifdef LANDSCAPE
	wc->w_flags = LMODE;
#else  PORTRAIT
	wc->w_flags = PMODE;
#endif LANDSCAPE / PORTRAIT
	if (wsp->x_orig == wsp->dblow.dx_orig &&
	    wsp->y_orig == wsp->dblow.dy_orig &&
	    wsp->vx_orig == wsp->dblow.dvx_orig &&
	    wsp->vy_orig == wsp->dblow.dvy_orig &&
	    wsp->vx_size == wsp->dblow.dvx_size &&
	    wsp->vy_size == wsp->dblow.dvy_size) {

		/*
		 * Restore the window to the state when it was blown up the
		 * previous time.
		 */
#ifdef LANDSCAPE
		wc->wl_xorig = wsp->oblow.ox_orig;
		wc->wl_yorig = wsp->oblow.oy_orig;
		wc->wl_vxorig = wsp->oblow.ovx_orig;
		wc->wl_vyorig = wsp->oblow.ovy_orig;
		wc->wl_vxsize = wsp->oblow.ovx_size;
		wc->wl_vysize = wsp->oblow.ovy_size;
#else  PORTRAIT
		wc->wp_xorig = wsp->oblow.ox_orig;
		wc->wp_yorig = wsp->oblow.oy_orig;
		wc->wp_vxorig = wsp->oblow.ovx_orig;
		wc->wp_vyorig = wsp->oblow.ovy_orig;
		wc->wp_vxsize = wsp->oblow.ovx_size;
		wc->wp_vysize = wsp->oblow.ovy_size;
#endif LANDSCAPE / PORTRAIT
	}
	else {

		/*
		 * Save the current state of the window and then blow it up
		 * to the default size and location.
		 */
		wsp->oblow.ox_orig = wsp->x_orig;
		wsp->oblow.oy_orig = wsp->y_orig;
		wsp->oblow.ovx_orig = wsp->vx_orig;
		wsp->oblow.ovy_orig = wsp->vy_orig;
		wsp->oblow.ovx_size = wsp->vx_size;
		wsp->oblow.ovy_size = wsp->vy_size;
#ifdef LANDSCAPE
		wc->wl_xorig = wsp->dblow.dx_orig;
		wc->wl_yorig = wsp->dblow.dy_orig;
		wc->wl_vxorig = wsp->dblow.dvx_orig;
		wc->wl_vyorig = wsp->dblow.dvy_orig;
		wc->wl_vxsize = wsp->dblow.dvx_size;
		wc->wl_vysize = wsp->dblow.dvy_size;
#else  PORTRAIT
		wc->wp_xorig = wsp->dblow.dx_orig;
		wc->wp_yorig = wsp->dblow.dy_orig;
		wc->wp_vxorig = wsp->dblow.dvx_orig;
		wc->wp_vyorig = wsp->dblow.dvy_orig;
		wc->wp_vxsize = wsp->dblow.dvx_size;
		wc->wp_vysize = wsp->dblow.dvy_size;
#endif LANDSCAPE / PORTRAIT
	}

	/*
	 * Now we are ready to alter the window.
	 */
	(void)alterwin(wn, wc);

	return;
}	/* end of mblowup() */



/*
 * Zoom a window using the mouse.
 */

void
mzoomwin(wn)
int	wn;		/* window to zoom */
{
	register struct	winint	*wsp;	/* pointer to window status */
	register		x;	/* x coordinate             */
	register		y;	/* y coordinate             */

	wsp = wstatp[wn];

	for ( ; ; ) {

		/*
		 * Wait until a change from the mouse.
		 */
		waitmous(PRESSED);

		/*
		 * Check what has happened.
		 */
		if (cmpgp->mflag == MOUSEMVD) {

			/*
			 * The mouse has moved, update the mouse pointer.
			 */
			updmpos();
			mp_on();
		}
		else if (cmpgp->mflag != LEMKEYRE) {

			/*
			 * Abort if the left key was not released.
			 */
			break;
		}
		else {
			if (pwinno() != wn) {

				/*
				 * Abort if we does not point into the same
				 * window any longer.
				 */
				break;
			}

			/*
			 * If we still point into the zoom box, zoom around the
			 * middle character in the window.
			 */
			x = pntdata.pd_xmpos;
			y = pntdata.pd_ympos;
#ifdef LANDSCAPE
			if (x >= wsp->abx_orig + BOXOFFS && x <= wsp->abx_orig + BOXOFFS + SBOXSIZE - 1 &&
			    y >= wsp->aby_orig + BOXOFFS && y <= wsp->aby_orig + BOXOFFS + SBOXSIZE - 1) {
#else  PORTRAIT
			if (x >= wsp->abx_orig + BOXOFFS && x <= wsp->abx_orig + BOXOFFS + SBOXSIZE - 1 &&
			    y >= wsp->aby_lrc - BOXOFFS - SBOXSIZE + 1 && y <= wsp->aby_lrc - BOXOFFS) {
#endif LANDSCAPE / PORTRAIT
				x = wsp->avx_orig + Div2(wsp->vx_size);
				y = wsp->avy_orig + Div2(wsp->vy_size);
			}
			else if ((pix_d)x < wsp->avx_orig || (pix_d)x > wsp->avx_lrc ||
				 (pix_d)y < wsp->avy_orig || (pix_d)y > wsp->avy_lrc) {
				break;		/* outside window */
			}

			/*
			 * Now compute the character coordinate for the
			 * character to zoom around.
			 */
#ifdef LANDSCAPE
			x = (wsp->x_orig + wsp->x_size - 1 - x) / wsp->fdata.fi_ysize + 1;
			if ((cur_d)x > wsp->bm_cur) {
				x = wsp->bm_cur;
			}
			y = (y - wsp->y_orig) / wsp->fdata.fi_xsize + 1;
			if ((cur_d)y > wsp->rm_cur) {
				y = wsp->rm_cur;
			}
#else  PORTRAIT
			x = (x - wsp->x_orig) / wsp->fdata.fi_xsize + 1;
			if ((cur_d)x > wsp->rm_cur) {
				x = wsp->rm_cur;
			}
			y = (y - wsp->y_orig) / wsp->fdata.fi_ysize + 1;
			if ((cur_d)y > wsp->bm_cur) {
				y = wsp->bm_cur;
			}
#endif LANDSCAPE / PORTRAIT
			if (wsp->zoomlist[++(wsp->zoomindex)] == '\0') {
				wsp->zoomindex = 0;	/* start over in list */
			}

			/*
			 * Now do the zoom.
			 */
#ifdef LANDSCAPE
			(void)fontrplce(wn, wsp->zoomlist[wsp->zoomindex], y, x);
#else  PORTRAIT
			(void)fontrplce(wn, wsp->zoomlist[wsp->zoomindex], x, y);
#endif LANDSCAPE / PORTRAIT
			break;
		}
	}

	return;
}	/* end of mzoomwin() */
