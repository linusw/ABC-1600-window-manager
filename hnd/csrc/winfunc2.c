/* winfunc2.c */

/*
 * This file contains relatively high level routines to handle windows for the
 * ABC1600 window handler (part 2).
 */

/* 1984-07-06, Peter Andersson, Luxor Datorer AB (at DIAB) */

/*
 * Functions in this file:
 *	setdflsz()
 *	fndgroup()
 *	ckgrpflg()
 *	txttopix()
 *	alterupd()
 *	winheader()
 *	addicon()
 *	rmicons()
 *	newflags()
 *	winstat()
 *	loadstruc()
 *	cnvcoord()
 *	updatewin()
 *	updrects()
 *	adjustwin()
 *	copystruc()
 *	saveblowup()
 */


#include	"../hsrc/defs.h"
#include	"../hsrc/structs.h"
#include	"../hsrc/eglobals.h"
#include	"../hsrc/graphconst.h"
#include	"../hsrc/macros.h"
#include	"../hsrc/dnixmacros.h"
#include	<dnix/fcodes.h>



/*
 * Set up a new default size and location for a window (to be used when the
 * blow up box is used).
 * Returns WE_NOTCR if the window is not created, WE_ILMOD if data for the
 * present screen mode is not given, WE_ILPARA if the given data is illegal,
 * or W_OK if everything is OK.
 */

int
setdflsz(wc, wsp)
register struct	winstruc *wc;	/* data for new default size         */
register struct	winint	 *wsp;	/* pointer to internal window status */
{
	if ((wsp->status & CREATED) == 0) {
		return(WE_NOTCR);	/* window not created */
	}

	/*
	 * Check if data for the present screen mode is given.
	 */
#ifdef LANDSCAPE
	if ((wc->w_flags & LMODE) == 0) {
		return(WE_ILMOD);
	}
	wc->wl_xsize = wsp->x_size;
	wc->wl_ysize = wsp->y_size;
#else  PORTRAIT
	if ((wc->w_flags & PMODE) == 0) {
		return(WE_ILMOD);
	}
	wc->wp_xsize = wsp->x_size;
	wc->wp_ysize = wsp->y_size;
#endif LANDSCAPE / PORTRAIT

	/*
	 * Adjust the default window size.
	 */
	adjustwin(wc, &borddata[wsp->border]);
#ifdef LANDSCAPE

	/*
	 * Check that the parameters are legal.
	 */
	if (wc->wl_vxsize <= 0 || wc->wl_vysize <= 0 || wc->wl_vxorig < 0 || wc->wl_vyorig < 0) {
		return(WE_ILPARA);
	}

	/*
	 * They are OK. Save new default size.
	 */
	wsp->dblow.dx_orig = wc->wl_xorig;
	wsp->dblow.dy_orig = wc->wl_yorig;
	wsp->dblow.dvx_orig = wc->wl_vxorig;
	wsp->dblow.dvy_orig = wc->wl_vyorig;
	wsp->dblow.dvx_size = wc->wl_vxsize;
	wsp->dblow.dvy_size = wc->wl_vysize;
#else  PORTRAIT

	/*
	 * Check that the parameters are legal.
	 */
	if (wc->wp_vxsize <= 0 || wc->wp_vysize <= 0 || wc->wp_vxorig < 0 || wc->wp_vyorig < 0) {
		return(WE_ILPARA);
	}

	/*
	 * They are OK. Save new default size.
	 */
	wsp->dblow.dx_orig = wc->wp_xorig;
	wsp->dblow.dy_orig = wc->wp_yorig;
	wsp->dblow.dvx_orig = wc->wp_vxorig;
	wsp->dblow.dvy_orig = wc->wp_vyorig;
	wsp->dblow.dvx_size = wc->wp_vxsize;
	wsp->dblow.dvy_size = wc->wp_vysize;
#endif LANDSCAPE / PORTRAIT

	return(W_OK);
}	/* end of setdflsz() */



/*
 * Given a window number, this function finds the topmost and lowermost
 * windows in the window group.
 * The level of the topmost window is returned as the return value and the
 * level of the lowermost window is returned in the variable pointed to by
 * the pointer argument.
 */

int
fndgroup(wn, blevp)
int	wn;		/* window number                           */
int	*blevp;		/* pointer to returned lowest level window */
{
	register		tlev;	/* topmost window           */
	register		blev;	/* lowermost window         */
	register struct	winint	*wsp1;	/* pointer to window status */
	register struct	winint	*wsp2;	/* pointer to window status */

	tlev = blev = wtol[wn];
	wsp1 = wstatp[wn];
	if (wsp1->estatus & WGROUP) {

		/*
		 * The window belongs to a window group.
		 */
		while (--tlev >= 0 && ((wsp2 = wstatp[ltow[tlev]])->estatus & WGROUP) &&
		       wsp1->pgid == wsp2->pgid)
			;
		tlev++;		/* compensate */

		while (++blev < nnswin && ((wsp2 = wstatp[ltow[blev]])->estatus & WGROUP) &&
		       wsp1->pgid == wsp2->pgid)
			;
		blev--;		/* compensate */
	}

	*blevp = blev;
	return(tlev);
}	/* end of fndgroup() */



/*
 * This function counts how many of the flags indicating which corner a child
 * window shall follow are set. It is not allowed to have more than one of the
 * flags set.
 */

int
ckgrpflg(flags)
register uflags	flags;		/* contains flags to count */
{
	register	cnt;	/* flag counter */

	cnt = 0;
	if (flags & REL_ULC) {
		cnt = 1;
	}
	if (flags & REL_URC) {
		cnt++;
	}
	if (flags & REL_LLC) {
		cnt++;
	}
	if (flags & REL_LRC) {
		cnt++;
	}

	return(cnt);
}	/* end of ckgrpflg() */



/*
 * Convert sizes for a window given in units of font boxes to pixel sizes.
 */

void
txttopix(wc, wsp)
register struct	winstruc *wc;	/* pointer to window data to convert */
register struct	winint	 *wsp;	/* pointer to internal window status */
{
	register pix_d	x;	/* x size of font box */
	register pix_d	y;	/* y size of font box */

	x = wsp->dflxsz;
	y = wsp->dflysz;

#ifdef LANDSCAPE
	wc->wl_vxorig = Muls(wc->wl_vxorig - 1, x);
	wc->wl_vyorig = wc->wl_ysize - Muls(wc->wl_vyorig + wc->wl_vysize - 1, y);
	wc->wl_vxsize *= x;
	wc->wl_vysize *= y;
#else  PORTRAIT
	wc->wp_vxorig = Muls(wc->wp_vxorig - 1, x);
	wc->wp_vyorig = wc->wp_ysize - Muls(wc->wp_vyorig + wc->wp_vysize - 1, y);
	wc->wp_vxsize *= x;
	wc->wp_vysize *= y;
#endif LANDSCAPE / PORTRAIT

	return;
}	/* end of txttopix() */



/*
 * Function used by alterwin() to update a window or the background on the
 * screen.
 */

void
alterupd(wn, n)
register	wn;		/* window to update                     */
register	n;		/* # of rectangles to overlay window wn */
{
	register struct	winint	*wsp;	/* window status structure    */
	register struct	wpstruc	*wrp;	/* pointer to rectangle group */
	register struct	wpstruc	*wrp1;	/* pointer to rectangle group */
	register		npart;	/* # of non-empty rectangles  */

	wsp = wstatp[wn];
	wrp = &ubuf.wpt[0];
	wrp1 = &wpt1r[0];
	npart = 0;

	while (--n >= 0) {
		wrp->xl_p = wrp1->xl_p;
		wrp->xr_p = wrp1->xr_p;
		wrp->yu_p = wrp1->yu_p;
		wrp->yl_p = (wrp1++)->yl_p;
		if (wn != MAXWINS) {	/* if not background */
			if (wrp->xl_p < wsp->abx_orig) {
				wrp->xl_p = wsp->abx_orig;
			}
			if (wrp->xr_p > wsp->abx_lrc) {
				wrp->xr_p = wsp->abx_lrc;
			}
			if (wrp->yu_p < wsp->aby_orig) {
				wrp->yu_p = wsp->aby_orig;
			}
			if (wrp->yl_p > wsp->aby_lrc) {
				wrp->yl_p = wsp->aby_lrc;
			}
		}
		npart += updrects(wn, wrp);	/* update one rectangle */
	}
	if (wn != MAXWINS) {
		disphdr(wn, ON);	/* display header if not background */
		dispbox(wn);		/* display boxes in the border */
		if (npart != 0) {
			redrawsig(wsp);		/* signal the change */
		}
	}

	return;
}	/* end of alterupd() */



/*
 * This function inserts a header into a window.
 * On entry a pointer to a structure of type headstruc must be supplied, which
 * contains all the necessary parameters.
 */

void
winheader(wn, hsp)
register		  wn;	/* window number                          */
register struct	headstruc *hsp;	/* pointer to structure containing header */
{
	register struct	winint	*wsp;	/* pointer to window status structure */
	register		i;	/* index                              */
	register		j;	/* index                              */
	register	char	c;	/* character                          */

	wsp = wstatp[wn];

	/*
	 * Stop text cursor blink.
	 */
	offblnk();

	if (wsp->status & CREATED) {

		/*
		 * Remove the mouse pointer.
		 */
		mp_off();

		/*
		 * Start by removing the old header from the screen. This
		 * eliminates the problem if the new header is shorter than
		 * the old one.
		 */
		disphdr(wn, OFF);
	}

	i = j = 0;
	while (i < HDRSIZE && (c = hsp->h_hdr[i++] & CMASK) != '\0') {
		if (c >= ' ' && c <= MAX7BCHR) {  /* if printable character */
			wsp->hdr[j++] = c;
		}
	}
	wsp->hdrlen = j;			/* set length of header   */
	wsp->status &= ~(INVHEAD | INVTOP);
	if (hsp->h_flags & H_INVHD) {
		wsp->status |= INVHEAD;
	}
	if (hsp->h_flags & H_INVTOP) {
		wsp->status |= INVTOP;
	}
	if (wsp->status & CREATED) {

		/*
		 * Now display the header in the window.
		 */
		disphdr(wn, ON);

		/*
		 * Restore the mouse pointer.
		 */
		mp_on();
	}

	/*
	 * Enable text cursor blink again.
	 */
	onblnk();

	return;
}	/* end of winheader() */



/*
 * Add an icon to window 'wn'.
 */

int
addicon(wn, icp)
register		wn;		/* window to add icon to */
register struct	winicon	*icp;		/* data for new icon     */
{
	register		 i;		/* loop index                  */
	register		 xorig;		/* x origin of icon (internal) */
	register		 yorig;		/* internally used y origin    */
	register		 xsize;		/* x size of icon              */
	register		 ysize;		/* y size of icon              */
	register struct	winint	 *wsp;		/* pointer to window status    */
	register struct	icondat	 *iconp;	/* pointer into icon save area */
	register struct	pntstruc *pdp;		/* pointer to mouse status     */

	pdp = &pntdata;
	wsp = wstatp[wn];
	if ((wsp->status & CREATED) == 0) {
		return(WE_NOTCR);	/* window not created yet */
	}

	/*
	 * Check that the data is relevant for landscape or portrait mode.
	 */
#ifdef LANDSCAPE
	if ((icp->i_flags & I_LMODE) == 0) {
		return(WE_ILMOD);
	}
#else  PORTRAIT
	if ((icp->i_flags & I_PMODE) == 0) {
		return(WE_ILMOD);
	}
#endif LANDSCAPE / PORTRAIT

	if (n_icons >= tot_icons) {

		/*
		 * We have to allocate more memory for icons.
		 */
		if ((char *)Sbrk(ICONCHUNK * sizeof(struct icondat)) == NULL) {
			return(WE_NOICON);	/* cannot allocate memory */
		}
		tot_icons += ICONCHUNK;
	}

	iconp = iconsave;

#ifdef LANDSCAPE
	xsize = icp->il_ysize;
	ysize = icp->il_xsize;
	if ((icp->i_flags & I_TEXT) == 0) {
		xorig = icp->il_yorig;
		yorig = icp->il_xorig;
	}
	else {
		xsize = Muls(xsize, wsp->fdata.fi_ysize);
		ysize = Muls(ysize, wsp->fdata.fi_xsize);
		xorig = wsp->x_size - Muls(icp->il_yorig - 1, wsp->fdata.fi_ysize) - xsize - wsp->y_coffs;
		yorig = Muls(icp->il_xorig - 1, wsp->fdata.fi_xsize) + wsp->x_coffs;
	}
#else  PORTRAIT
	xsize = icp->ip_xsize;
	ysize = icp->ip_ysize;
	if ((icp->i_flags & I_TEXT) == 0) {
		xorig = icp->ip_xorig;
		yorig = wsp->y_size - icp->ip_yorig - ysize;
	}
	else {
		xorig = Muls(icp->ip_xorig - 1, wsp->fdata.fi_xsize) + wsp->x_coffs;
		yorig = Muls(icp->ip_yorig - 1, wsp->fdata.fi_ysize) + wsp->y_coffs;
		xsize = Muls(xsize, wsp->fdata.fi_xsize);
		ysize = Muls(ysize, wsp->fdata.fi_ysize);
	}
#endif LANDSCAPE / PORTRAIT

	/*
	 * Check if the icon will fit in the virtual screen and that it does
	 * not overlap another icon (not allowed if neither of the I_ENTER or
	 * I_LEAVE flags are set).
	 */
	if (xorig < 0 || xsize <= 0 || xorig + xsize > wsp->x_size ||
	    yorig < 0 || ysize <= 0 || yorig + ysize > wsp->y_size) {
		return(WE_ILPARA);		/* illegal position or size */
	}
	if (icp->i_flags & (I_ENTER | I_LEAVE)) {
		icp->i_flags &= ~(I_INVERT | I_PRESS | I_RELEASE);
	}
	else {
		i = n_icons;
		while (--i >= 0) {
			if (iconp->id_win == wn &&
			    (iconp->id_flags & (I_ENTER | I_LEAVE)) == 0 &&
			    iconp->id_xorig < (pix_d)(xorig + xsize) &&
			    iconp->id_xorig + iconp->id_xsize > (pix_d)xorig &&
			    iconp->id_yorig < (pix_d)(yorig + ysize) &&
			    iconp->id_yorig + iconp->id_ysize > (pix_d)yorig) {
				return(WE_ONICON);	/* it would overlap another icon */
			}
			iconp++;
		}
	}

	iconp = (iconsave + n_icons);
	iconp->id_xorig = xorig;
	iconp->id_yorig = yorig;
	iconp->id_xsize = xsize;
	iconp->id_ysize = ysize;
	iconp->id_flags = icp->i_flags;
	if (wsp->status & GRAPHIC) {

		/*
		 * If the GRAPHIC flag is set the location of the icon must
		 * not be updated when the default font is changed.
		 */
		iconp->id_flags &= ~I_TEXT;
	}

	i = 0;
	while (i < ICONSEQLEN && (iconp->id_cmdseq[i] = icp->i_cmdseq[i])) {
		i++;
	}
	n_icons++;				/* one more icon */
	iconp->id_len = i;
	iconp->id_win = wn;
	if ((iconp->id_flags & (I_PRESS | I_RELEASE | I_ENTER | I_LEAVE)) == 0) {
		iconp->id_flags |= I_PRESS;
	}

	/*
	 * Stop text cursor blink.
	 */
	offblnk();

	/*
	 * Invert the icon if we already point to it.
	 */
	if (invdata.inv_flg == OFF) {
		invon(wn);
	}

	/*
	 * Check if the icon sequence shall be sent immediately.
	 */
	if (iconp->id_flags & I_SETCHK) {
		xorig += wsp->x_orig;
		yorig += wsp->y_orig;
		if (pdp->pd_xmpos >= (pix_d)xorig && pdp->pd_xmpos < (pix_d)(xorig + xsize) &&
		    pdp->pd_ympos >= (pix_d)yorig && pdp->pd_ympos < (pix_d)(yorig + ysize)) {
			if (iconp->id_flags & I_ENTER) {
				(void)sendicon(iconp);
			}
		}
		else if (iconp->id_flags & I_LEAVE) {
			(void)sendicon(iconp);
		}
	}

	/*
	 * Enable text cursor blink again.
	 */
	onblnk();

	return(W_OK);			/* OK */
}	/* end of addicon() */



/*
 * Remove all icons belonging to window 'wn'.
 * WARNING: This function uses structure assignment.
 */

void
rmicons(wn)
register	wn;		/* window to remove icons for */
{
	register		i;	 /* loop index           */
	register struct	icondat	*iconp;	 /* pointer to icon data */
	register struct	icondat	*iconps; /* pointer to icon data */

	/*
	 * If an icon in this window is inverted, restore it.
	 */
	if (invdata.inv_wn == wn) {
		offblnk();	/* stop text cursor blink */
		invoff();
		onblnk();	/* enable text cursor blink again */
	}

	iconp = iconps = iconsave;
	i = n_icons;
	while (--i >= 0) {
		if (iconp->id_win == wn) {
			*iconp = *(iconps + --n_icons);		/**** WARNING ****/
		}
		else {
			iconp++;
		}
	}

	/*
	 * Check if we can release any memory.
	 */
	ckiconcnt();

	return;
}	/* end of rmicons() */



/*
 * Alter the flags for a window.
 * The following flags may be altered: LOCK, NOOVER, NOCURSOR, NOMOVE, ALLSCR,
 * KEYSCROLL, WRITSCROLL, NOCPIN, NOCPOUT, REL_ULC, REL_URC, REL_LLC, and
 * REL_LRC.
 * The following flags are ignored: PMODE, LMODE, SAVETEXT, SAVEBITMAP,
 * OVERLAP, SPECIAL, ALTMPNT, and WGROUP.
 */

int
newflags(wn, flp)
int		 wn;		/* window to alter flags for      */
struct	flgstruc *flp;		/* pointer to new flags structure */
{
	register struct	winint	*wsp;	/* pointer to window status */
	register	uflags	flags;	/* the new flags            */
	register		lev;	/* level of window          */

	lev = wtol[wn];
	wsp = wstatp[wn];

	flags = flp->f_flags & (LOCK | NOOVER | NOCURSOR | NOMOVE | ALLSCR |  KEYSCROLL |
	        WRITSCROLL | NOCPIN | NOCPOUT | REL_ULC | REL_URC | REL_LLC | REL_LRC);
#ifdef LANDSCAPE
	flags |= wsp->estatus & (LMODE | SAVETEXT | SAVEBITMAP | OVERLAP | SPECIAL | ALTMPNT | WGROUP);
#else  PORTRAIT
	flags |= wsp->estatus & (PMODE | SAVETEXT | SAVEBITMAP | OVERLAP | SPECIAL | ALTMPNT | WGROUP);
#endif LANDSCAPE / PORTRAIT

	if ((flags & (LOCK | SPECIAL)) == (LOCK | SPECIAL) || ckgrpflg(flags) > 1) {
		return(WE_ILPARA);	/* illegal flag combination */
	}
	if (((flags & LOCK) && lev != 0) || ((flags & NOOVER) && (flags & OVERLAP))) {
		return(WE_LORO);	/* not zero level or overlapped */
	}
	if ((flags & ALLSCR) && (wsp->x_size != wsp->vx_size || wsp->y_size != wsp->vy_size)) {
		return(WE_ALLSCR);	/* the whole virtual screen is not visible */
	}

	/*
	 * The flags are OK. Replace the old ones.
	 */
	wsp->estatus = flags;

	/*
	 * Fix the cursor if the NOCURSOR flag have changed.
	 */
	if (lev == 0) {
		offblnk();	/* stop text cursor blink */
		mp_off();	/* remove the mouse pointer */
		cursoff();
		curson();
		mp_on();	/* restore the mouse pointer */
		onblnk();	/* enable text cursor blink again */
	}

	return(W_OK);		/* OK */
}	/* end of newflags() */



/*
 * Load the structure pointed to by 'wc' (of type winstruc) with the values
 * from the window 'wn' and then convert the coordinates to the external ones,
 * i.e. those used when making requests to the window handler.
 */

void
winstat(wn, wc)
int		 wn;	/* window number               */
struct	winstruc *wc;	/* pointer to window structure */
{
	loadstruc(wn, wc);
	cnvcoord(wc);

	return;
}	/* end of winstat() */



/*
 * Load the structure pointed to by wc (of type winstruc) with the values from
 * the window wn (the coordinates for the window is the internal ones).
 */

void
loadstruc(wn, wc)
register		 wn;	/* window number     */
register struct	winstruc *wc;	/* structure to load */
{
	register struct	winint	*wsp;	/* pointer to window status structure */

	wsp = wstatp[wn];

#ifdef LANDSCAPE
	wc->wl_xorig = wsp->x_orig;
	wc->wl_yorig = wsp->y_orig;
	wc->wl_xsize = wsp->x_size;
	wc->wl_ysize = wsp->y_size;
	wc->wl_vxorig = wsp->vx_orig;
	wc->wl_vyorig = wsp->vy_orig;
	wc->wl_vxsize = wsp->vx_size;
	wc->wl_vysize = wsp->vy_size;
	wc->wl_font = wsp->font;
#else  PORTRAIT
	wc->wp_xorig = wsp->x_orig;
	wc->wp_yorig = wsp->y_orig;
	wc->wp_xsize = wsp->x_size;
	wc->wp_ysize = wsp->y_size;
	wc->wp_vxorig = wsp->vx_orig;
	wc->wp_vyorig = wsp->vy_orig;
	wc->wp_vxsize = wsp->vx_size;
	wc->wp_vysize = wsp->vy_size;
	wc->wp_font = wsp->font;
#endif LANDSCAPE / PORTRAIT
	wc->w_border = wsp->border;
	wc->w_color = wsp->color;
	wc->w_curfont = wsp->fdata.fi_cur + 'A';
	wc->w_flags = wsp->estatus;
	wc->w_level = wtol[wn];
	wc->w_uboxes = wsp->uboxmax;
	wc->w_xcur = wsp->x_cur;
	wc->w_ycur = wsp->y_cur;
	wc->w_xgcur = wsp->x_gcur;
	wc->w_ygcur = wsp->y_gcur;
	wc->w_tsig = wsp->tsig;
	wc->w_ntsig = wsp->ntsig;
	wc->w_rsig = wsp->rsig;
	wc->w_csig = wsp->csig;
	wc->w_boxes = wsp->boxes;

	return;
}	/* end of loadstruc() */



/*
 * Convert the coordinates for a window from the external to the internal
 * format or vice versa.
 */

void
cnvcoord(wc)
register struct	winstruc *wc;	/* pointer to window data */
{
#ifdef LANDSCAPE
	register pix_d	tmp;		/* temporary */

	tmp = wc->wl_xorig;
	wc->wl_xorig = wc->wl_yorig;
	wc->wl_yorig = tmp;
	tmp = wc->wl_vxorig;
	wc->wl_vxorig = wc->wl_vyorig;
	wc->wl_vyorig = tmp;
	tmp = wc->wl_xsize;
	wc->wl_xsize = wc->wl_ysize;
	wc->wl_ysize = tmp;
	tmp = wc->wl_vxsize;
	wc->wl_vxsize = wc->wl_vysize;
	wc->wl_vysize = tmp;
#else  PORTRAIT
	wc->wp_yorig = SCYSIZE - wc->wp_yorig - wc->wp_ysize;
	wc->wp_vyorig = wc->wp_ysize - wc->wp_vyorig - wc->wp_vysize;
#endif LANDSCAPE / PORTRAIT

	return;
}	/* end of cnvcoord() */



/*
 * Update the window wnu on the screen when the window wnr has been removed or
 * been moved to a lower level.
 * This is done by first computing the area of wnu which become visible when wnr
 * was removed, then clearing that area, and lastly write the text contents in
 * the area.
 * The background can also be updated by using the window number MAXWINS for wnu.
 * Note that this routine assumes that wnr really overlapped wnu and it will
 * produce garbage if that was not the case.
 */

void
updatewin(wnu, wnr)
register	wnu;	/* window to update */
int		wnr;	/* removed window   */
{
	register struct	winint	*wspu;	/* pointer to wnu window status structure */
	register struct	winint	*wspr;	/* pointer to wnr window status structure */
	register struct	wpstruc	*wrp;	/* pointer into temporary rectangle group */
	register		npart;	/* number of non-empty rectangles         */

	wspr = wstatp[wnr];
	wrp = &ubuf.wpt[0];
	if (wnu == MAXWINS) {		/* if update background */
		wrp->xl_p = wspr->abx_orig;
		wrp->xr_p = wspr->abx_lrc;
		wrp->yu_p = wspr->aby_orig;
		wrp->yl_p = wspr->aby_lrc;
	}
	else {
		wspu = wstatp[wnu];
		wrp->xl_p = Max(wspr->abx_orig, wspu->abx_orig);
		wrp->xr_p = Min(wspr->abx_lrc, wspu->abx_lrc);
		wrp->yu_p = Max(wspr->aby_orig, wspu->aby_orig);
		wrp->yl_p = Min(wspr->aby_lrc, wspu->aby_lrc);
	}

	/*
	 * Update the window wnu when overlayed by the rectangle pointed to by wrp.
	 */
	npart = updrects(wnu, wrp);
	if (wnu != MAXWINS) {
		disphdr(wnu, ON);	/* display header if not background */
		dispbox(wnu);		/* display the boxes in the border */
		if (npart != 0) {
			redrawsig(wspu);	/* signal the change */
		}
	}

	return;
}	/* end of updatewin() */



/*
 * Update the window wnu on the screen when the rectangle pointed to by wrp has
 * been removed or changed level.
 * This is done by first computing the area of wnu which become visible when the
 * rectangle was removed, then clearing that area, and lastly write the text
 * contents in the area.
 * The background can also be updated by using the window number MAXWINS for wnu.
 * The number of rectangles which contains more than a window border is returned.
 */

int
updrects(wnu, wrp)
register		wnu;		/* window to update      */
register struct	wpstruc	*wrp;		/* pointer to rectangles */
{
	register		nrect;		/* number of rectangles  */
	register		i;		/* loop index            */
	register		n;		/* help variable         */
	register		npart;		/* # of non-empty rects. */
	register struct	wpstruc	*wrp1;		/* pointer to rectangles */

	/*
	 * Split the rectangle by overlaying it with windows on higher levels
	 * than wnu.
	 */
	nrect = 1;			/* initially it is 1 rectangle */
	i = wtol[wnu];
	while (--i >= 0) {
		wrp1 = wrp;
		n = nrect;
		while (--n >= 0) {
			nrect = rsplit(wstatp[ltow[i]], wrp, wrp1++, wrp + nrect);
		}

		/*
		 * Concatenate rectangles and then remove all rectangles of zero area.
		 */
		nrect = rmsort(wrp, nrect);
	}
	if (wnu != MAXWINS) {				/* not for background */
		cvisible(wnu, wrp, nrect, NORMAL);	/* compute additional data */
	}

	/*
	 * Clear all the obtained rectangles.
	 */
	wrp1 = wrp;
	i = nrect;
	while (--i >= 0) {
		clrrect(wnu, wrp1++);
	}

	/*
	 * Rewrite all the rectangles.
	 */
	npart = 0;
	if (wnu != MAXWINS) {
		while (--nrect >= 0) {
			if (wrp->bflags & B_NOTEMPTY) {
				npart++;
				sav_rewrit(wstatp[wnu], wrp);
			}
			wrp++;
		}
	}

	return(npart);
}	/* end of updrects() */



/*
 * Internal function used by addwin(), alter1win(), and setdflsz() to if
 * possible adjust the size of the window so it will fit on the screen.
 */

void
adjustwin(wc, bdp)
register struct	winstruc *wc;	/* data to adjust         */
register struct borddst	 *bdp;	/* pointer to border data */
{
	register	tmp;	/* temporary */

#ifdef LANDSCAPE
	tmp = wc->wl_xorig + wc->wl_vxorig - bdp->bd_down;
	if (tmp < 0) {			/* make sure that the upper left */
		wc->wl_vxorig -= tmp;	/* corner of the border is inside */
		wc->wl_vxsize += tmp;	/* the screen */
		tmp = 0;
	}
	tmp += wc->wl_vxsize + bdp->bd_up + bdp->bd_down - SCXSIZE;
	if (tmp > 0) {			/* make sure that the lower right */
		wc->wl_vxsize -= tmp;	/* corner of the border is inside */
	}				/* the screen */

	tmp = wc->wl_yorig + wc->wl_vyorig - bdp->bd_left;
	if (tmp < 0) {
		wc->wl_vyorig -= tmp;
		wc->wl_vysize += tmp;
		tmp = 0;
	}
	tmp += wc->wl_vysize + bdp->bd_left + bdp->bd_right - SCYSIZE;
	if (tmp > 0) {
		wc->wl_vysize -= tmp;
	}

	tmp = wc->wl_xsize - wc->wl_vxorig;
	if (wc->wl_vxsize > (pix_d)tmp) {	/* make window fit */
		wc->wl_vxsize = tmp;		/* inside virtual */
	}					/* screen */
	tmp = wc->wl_ysize - wc->wl_vyorig;
	if (wc->wl_vysize > (pix_d)tmp) {
		wc->wl_vysize = tmp;
	}
#else  PORTRAIT
	tmp = wc->wp_xorig + wc->wp_vxorig - bdp->bd_left;
	if (tmp < 0) {			/* make sure that the upper left */
		wc->wp_vxorig -= tmp;	/* corner of the border is inside */
		wc->wp_vxsize += tmp;	/* the screen */
		tmp = 0;
	}
	tmp += wc->wp_vxsize + bdp->bd_left + bdp->bd_right - SCXSIZE;
	if (tmp > 0) {			/* make sure that the lower right */
		wc->wp_vxsize -= tmp;	/* corner of the border is inside */
	}				/* the screen                     */

	tmp = wc->wp_yorig + wc->wp_vyorig - bdp->bd_up;
	if (tmp < 0) {
		wc->wp_vyorig -= tmp;
		wc->wp_vysize += tmp;
		tmp = 0;
	}
	tmp += wc->wp_vysize + bdp->bd_up + bdp->bd_down - SCYSIZE;
	if (tmp > 0) {
		wc->wp_vysize -= tmp;
	}

	tmp = wc->wp_xsize - wc->wp_vxorig;
	if (wc->wp_vxsize > (pix_d)tmp) {	/* make window fit */
		wc->wp_vxsize = tmp;		/* inside virtual  */
	}					/* screen          */
	tmp = wc->wp_ysize - wc->wp_vyorig;
	if (wc->wp_vysize > (pix_d)tmp) {
		wc->wp_vysize = tmp;
	}
#endif LANDSCAPE / PORTRAIT

	return;
}	/* end of adjustwin() */



/*
 * Internal function used by addwin() and alter1win() to copy data from the
 * received structure to the internal one and also doing some calculations.
 */

void
copystruc(wsp, wc, bdp)
register struct	winint	 *wsp;	/* structure to update    */
register struct	winstruc *wc;	/* received structure     */
register struct	borddst	 *bdp;	/* pointer to border data */
{
	register word	bordf;	/* border data flags   */
	register word	bpres;	/* present boxes flags */

#ifdef LANDSCAPE
	wsp->x_orig = wc->wl_xorig;
	wsp->y_orig = wc->wl_yorig;
	wsp->vx_orig = wc->wl_vxorig;
	wsp->vy_orig = wc->wl_vyorig;
	wsp->vx_size = wc->wl_vxsize;
	wsp->vy_size = wc->wl_vysize;
#else  PORTRAIT
	wsp->x_orig = wc->wp_xorig;
	wsp->y_orig = wc->wp_yorig;
	wsp->vx_orig = wc->wp_vxorig;
	wsp->vy_orig = wc->wp_vyorig;
	wsp->vx_size = wc->wp_vxsize;
	wsp->vy_size = wc->wp_vysize;
#endif LANDSCAPE / PORTRAIT
	wsp->avx_orig = wsp->x_orig + wsp->vx_orig;
	wsp->avy_orig = wsp->y_orig + wsp->vy_orig;
	wsp->avx_lrc = wsp->avx_orig + wsp->vx_size - 1;
	wsp->avy_lrc = wsp->avy_orig + wsp->vy_size - 1;
#ifdef LANDSCAPE
	wsp->abx_orig = wsp->avx_orig - bdp->bd_down;
	wsp->aby_orig = wsp->avy_orig - bdp->bd_left;
	wsp->abx_lrc = wsp->avx_lrc + bdp->bd_up;
	wsp->aby_lrc = wsp->avy_lrc + bdp->bd_right;
#else  PORTRAIT
	wsp->abx_orig = wsp->avx_orig - bdp->bd_left;
	wsp->aby_orig = wsp->avy_orig - bdp->bd_up;
	wsp->abx_lrc = wsp->avx_lrc + bdp->bd_right;
	wsp->aby_lrc = wsp->avy_lrc + bdp->bd_down;
#endif LANDSCAPE / PORTRAIT
	wsp->bx_size = wsp->abx_lrc - wsp->abx_orig + 1;
	wsp->by_size = wsp->aby_lrc - wsp->aby_orig + 1;

	/*
	 * Now find out which boxes and indicators are present in the border.
	 */
	bordf = wsp->bordflags;
	bpres = wsp->boxes & ~(BXI_HIND | BXI_VIND);
	if ((bordf & BD_DLC) == 0) {
		bpres &= ~BX_ZOOM;
	}
	if ((bordf & BD_ULC) == 0) {
		bpres &= ~BX_CLOS;
	}
	if ((bordf & BD_URC) == 0) {
		bpres &= ~BX_MOVE;
	}
	if ((bordf & BD_DRC) == 0) {
		bpres &= ~BX_SIZE;
	}
#ifdef LANDSCAPE
	if ((bordf & BD_RIGHT) == 0 || wsp->bx_size < SCRBVIS + 2 * DLBWIDTH ||
	    (wsp->vx_size == wsp->x_size && (bpres & BX_AVIS))) {
		bpres &= ~BX_VSCR;
	}
	else if ((bpres & BX_VSCR) && wsp->bx_size >= INDVIS + 2 * DLBWIDTH) {
		bpres |= BXI_VIND;
	}
	if ((bordf & BD_DOWN) == 0 || wsp->by_size < SCRBVIS + 2 * DLBWIDTH ||
	    (wsp->vy_size == wsp->y_size && (bpres & BX_AVIS))) {
		bpres &= ~BX_HSCR;
	}
	else if ((bpres & BX_HSCR) && wsp->by_size >= INDVIS + 2 * DLBWIDTH) {
		bpres |= BXI_HIND;
	}
	if ((bordf & BD_UP) == 0 || wsp->by_size < 4 * DLBWIDTH) {
		bpres &= ~(BX_BLOW | BX_HELP);
	}
#else  PORTRAIT
	if ((bordf & BD_RIGHT) == 0 || wsp->by_size < SCRBVIS + 2 * DLBWIDTH ||
	    (wsp->vy_size == wsp->y_size && (bpres & BX_AVIS))) {
		bpres &= ~BX_VSCR;
	}
	else if ((bpres & BX_VSCR) && wsp->by_size >= INDVIS + 2 * DLBWIDTH) {
		bpres |= BXI_VIND;
	}
	if ((bordf & BD_DOWN) == 0 || wsp->bx_size < SCRBVIS + 2 * DLBWIDTH ||
	    (wsp->vx_size == wsp->x_size && (bpres & BX_AVIS))) {
		bpres &= ~BX_HSCR;
	}
	else if ((bpres & BX_HSCR) && wsp->bx_size >= INDVIS + 2 * DLBWIDTH) {
		bpres |= BXI_HIND;
	}
	if ((bordf & BD_UP) == 0 || wsp->bx_size < 4 * DLBWIDTH) {
		bpres &= ~(BX_BLOW | BX_HELP);
	}
#endif LANDSCAPE / PORTRAIT
	wsp->bpresent = bpres;

	return;
}	/* end of copystruc() */



/*
 * Initialize values for the window blow up facility.
 */

void
saveblowup(wsp)
register struct	winint	*wsp;	/* pointer to window status */
{
	wsp->dblow.dx_orig = wsp->oblow.ox_orig = wsp->x_orig;
	wsp->dblow.dy_orig = wsp->oblow.oy_orig = wsp->y_orig;
	wsp->dblow.dvx_orig = wsp->oblow.ovx_orig = wsp->vx_orig;
	wsp->dblow.dvy_orig = wsp->oblow.ovy_orig = wsp->vy_orig;
	wsp->dblow.dvx_size = wsp->oblow.ovx_size = wsp->vx_size;
	wsp->dblow.dvy_size = wsp->oblow.ovy_size = wsp->vy_size;

	return;
}	/* end of saveblowup() */
