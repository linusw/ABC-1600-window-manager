/* wincntrl.c */

/*
 * This file contains routines to control the windows in different ways,
 * such as erasing and scrolling.
 */

/* 1984-07-26, Peter Andersson, Luxor Datorer AB (at DIAB) */

/*
 * Functions in this file:
 *	initwin()
 *	invertwin()
 *	fontrplce()
 *	eraseln()
 *	erasescr()
 *	chkscroll()
 *	scrollwin()
 *	clrfast()
 *	onblnk()
 *	offblnk()
 *	curson()
 *	cursoff()
 *	mpover()
 *	newfont()
 *	adjfont()
 *	loadfont()
 *	fonthand()
 */


#include	"../hsrc/defs.h"
#include	"../hsrc/structs.h"
#include	"../hsrc/eglobals.h"
#include	"../hsrc/graphconst.h"
#include	"../hsrc/macros.h"
#include	<dnix/fcodes.h>
#include	"../../gdrv/x35g.h"



/*
 * Do some of the things to be done when setting a window to its initial state.
 * Used when creating a window and when the reset escape sequence is handled.
 */

void
initwin(wsp)
register struct	winint	*wsp;	/* pointer to window status */
{
	register	i;	/* loop index */

	wsp->umr_cur = 1;
	wsp->x_coffs = 0;
	wsp->y_coffs = 0;
	wsp->x_gorig = 0;
	wsp->y_gorig = 0;
	wsp->bmr_cur = wsp->bm_cur = wsp->rows;
	wsp->rm_cur = wsp->columns;
	wsp->termpara = ttydflt.td_term;

	/*
	 * Set to inverse character attribute if the flags says so.
	 */
	wsp->chstatflg = wsp->statflg;
	if (wsp->termpara & TD_REVERSE) {
		wsp->chstatflg ^= COMPMFL;
	}

	/*
	 * Set up the default tab stops.
	 */
	for (i = 0 ; i < TSTOPSIZE ; i++) {
		wsp->tabstop[i] = ttydflt.td_tbstop[i];
	}

	/*
	 * Reset the graphic pattern tables to their default values.
	 */
	for (i = 0 ; i < 16 ; i++) {
		wsp->msk1[i] = msk1[i];
		wsp->msk2[i] = msk2[i];
		wsp->shft[i] = shft[i];
		wsp->op[i] = op[i];
	}

	/*
	 * Clear the LED flags.
	 */
	wsp->ledflg = 0;

	return;
}	/* end of initwin() */



/*
 * Invert window 'wn'. Also invert the COMPMFL flag in the 'statflg' and
 * 'chstatflg' status bytes for the window and invert the TD_SCREEN flag in
 * the 'termpara' status word.
 */

void
invertwin(wn)
int	wn;		/* window to invert */
{
	register struct	winint	 *wsp;	/* pointer to window status       */
	register struct	wpstruc	 *wrp;	/* pointer to rectangle group     */
	register struct	hwastruc *hwap;	/* pointer to hardware addresses  */
	register		 n;	/* # of rectangles                */
	register	pix_d	 x;	/* left side of rectangle         */
	register	pix_d	 y;	/* upper side of rectangle        */
	register	short	 onoff;	/* mouse pointer on/off flag save */

	hwap = &hwa;
	n = wn;			/* move to register variable */
	wsp = wstatp[n];
	wrp = wpp[n];
	n = nwpart[n];

	onoff = pntdata.pd_onoff;
	mp_off();	/* make sure mouse pointer is removed */
	Waitsp();
	Outb(GSTATFLG | COMPMFL, hwap->mflagport);
	while (--n >= 0) {
		x = wrp->xl_p;
		y = wrp->yu_p;
		Waitsp();
		Move(x, y, x, y, wrp->xr_p - x, wrp->yl_p - y, UX_UY);
		wrp++;
	}
	if (onoff == ON) {
		mp_on();	/* restore mouse pointer */
	}
	wsp->statflg ^= COMPMFL;
	wsp->chstatflg ^= COMPMFL;
	wsp->termpara ^= TD_SCREEN;

	return;
}	/* end of invertwin() */



/*
 * Replace the old default font with a new one and rewrite the window with the
 * new font. The size of the virtual screen is adjusted so it contains the
 * same number of characters as with the old default font. However the size of
 * the window is, if possible, kept the same.
 * Returns WE_FONT if the font cannot be loaded, WE_ALLSCR if the ALLSCR flag
 * for the window is set, WE_NOMOVE if the NOMOVE flag for the window is set,
 * WE_TSAVE if the text contents of the window is not saved, or W_OK if
 * everything went OK (in case of an error the old font is retained).
 */

int
fontrplce(wn, fn, xcoord, ycoord)
int	wn;		/* window number                          */
int	fn;		/* the new default font                   */
int	xcoord;		/* x character coordinate of middle point */
int	ycoord;		/* y character coordinate of middle point */
{
	register struct	winint	 *wsp;	/* pointer to window status */
	register struct	winstruc *wc;	/* used to alter the window */
	register struct	icondat	 *iconp;/* pointer to icon data     */
	register		 x;	/* temporary                */
	register		 y;	/* temporary                */
	register	pix_d	 fxsiz;	/* save of old font box     */
	register	pix_d	 fysiz;	/* size                     */
#ifdef LANDSCAPE
	register	pix_d	 xszsav;/* save of old x size       */
#endif LANDSCAPE

	wsp = wstatp[wn];
	if ((wsp->status & GRAPHIC) || (wsp->estatus & SAVETEXT) == 0) {
		return(WE_TSAVE);	/* text contents not saved */
	}
	if (wsp->estatus & ALLSCR) {
		return(WE_ALLSCR);
	}
	if (wsp->estatus & NOMOVE) {
		return(WE_NOMOVE);
	}

	wc = &ubuf.wbuf;
	loadstruc(wn, wc);

	/*
	 * Save the size of the old font box.
	 */
	fxsiz = wsp->fdata.fi_xsize;
	fysiz = wsp->fdata.fi_ysize;

#ifdef LANDSCAPE
	/*
	 * Save old x size of virtual screen.
	 */
	xszsav = wsp->x_size;
#endif LANDSCAPE

	/*
	 * Compute the number of pixels left and unused (by characters) at the
	 * right and lower sides.
	 */
#ifdef LANDSCAPE
	x = wsp->y_size - Muls(wsp->rm_cur, fxsiz);
	y = wsp->x_size - Muls(wsp->bm_cur, fysiz);
#else  PORTRAIT
	x = wsp->x_size - Muls(wsp->rm_cur, fxsiz);
	y = wsp->y_size - Muls(wsp->bm_cur, fysiz);
#endif LANDSCAPE / PORTRAIT

	/*
	 * Load the new font to be used.
	 */
	if (adjfont(fn - 'A', &wsp->fdata) != OK) {
		return(WE_NOFONT);
	}

	/*
	 * Now we have a new default font.
	 */
	wsp->font = fn;

	/*
	 * Save the size of the new default font.
	 */
	wsp->dflxsz = wsp->fdata.fi_xsize;
	wsp->dflysz = wsp->fdata.fi_ysize;

	/*
	 * Stop text cursor blink.
	 */
	offblnk();

	/*
	 * Remove the old visible indicators (alterwin will probably fail to
	 * do this).
	 */
	dispind(wn, wsp, OFF);

	/*
	 * Compute the new size of the virtual screen.
	 */
#ifdef LANDSCAPE
	wsp->y_size = Muls(wsp->rm_cur, wsp->fdata.fi_xsize) + x;
	wsp->x_size = Muls(wsp->bm_cur, wsp->fdata.fi_ysize) + y;
	if (wsp->x_size < wc->wl_vxsize) {
		wc->wl_vxsize = wsp->x_size;
	}
	if (wsp->y_size < wc->wl_vysize) {
		wc->wl_vysize = wsp->y_size;
	}

	/*
	 * Compute the new origin of the virtual screen.
	 */
	wc->wl_xorig = wsp->avx_orig + Div2(wc->wl_vxsize) - wsp->x_size +
		       Muls(wsp->fdata.fi_ysize, ycoord) - Div2(wsp->fdata.fi_ysize);
	x = wsp->avx_orig + wc->wl_vxsize - wsp->x_size;
	if (wc->wl_xorig < (pix_d)x) {
		wc->wl_xorig = x;
	}
	if (wc->wl_xorig > wsp->avx_orig) {
		wc->wl_xorig = wsp->avx_orig;
	}
	wc->wl_yorig = wsp->avy_orig + Div2(wc->wl_vysize) - Muls(wsp->fdata.fi_xsize, xcoord - 1) -
		       Div2(wsp->fdata.fi_xsize);
	y = wsp->avy_orig + wc->wl_vysize - wsp->y_size;
	if (wc->wl_yorig < (pix_d)y) {
		wc->wl_yorig = y;
	}
	if (wc->wl_yorig > wsp->avy_orig) {
		wc->wl_yorig = wsp->avy_orig;
	}

	/*
	 * Compute the new origin of the window.
	 */
	wc->wl_vxorig = wsp->avx_orig - wc->wl_xorig;
	wc->wl_vyorig = wsp->avy_orig - wc->wl_yorig;
#else  PORTRAIT
	wsp->x_size = Muls(wsp->rm_cur, wsp->fdata.fi_xsize) + x;
	wsp->y_size = Muls(wsp->bm_cur, wsp->fdata.fi_ysize) + y;
	if (wsp->x_size < wc->wp_vxsize) {
		wc->wp_vxsize = wsp->x_size;
	}
	if (wsp->y_size < wc->wp_vysize) {
		wc->wp_vysize = wsp->y_size;
	}

	/*
	 * Compute the new origin of the virtual screen.
	 */
	wc->wp_xorig = wsp->avx_orig + Div2(wc->wp_vxsize) - Muls(wsp->fdata.fi_xsize, xcoord - 1) -
		       Div2(wsp->fdata.fi_xsize);
	x = wsp->avx_orig + wc->wp_vxsize - wsp->x_size;
	if (wc->wp_xorig < (pix_d)x) {
		wc->wp_xorig = x;
	}
	if (wc->wp_xorig > wsp->avx_orig) {
		wc->wp_xorig = wsp->avx_orig;
	}
	wc->wp_yorig = wsp->avy_orig + Div2(wc->wp_vysize) - Muls(wsp->fdata.fi_ysize, ycoord - 1) -
		       Div2(wsp->fdata.fi_ysize);
	y = wsp->avy_orig + wc->wp_vysize - wsp->y_size;
	if (wc->wp_yorig < (pix_d)y) {
		wc->wp_yorig = y;
	}
	if (wc->wp_yorig > wsp->avy_orig) {
		wc->wp_yorig = wsp->avy_orig;
	}

	/*
	 * Compute the new origin of the window.
	 */
	wc->wp_vxorig = wsp->avx_orig - wc->wp_xorig;
	wc->wp_vyorig = wsp->avy_orig - wc->wp_yorig;
#endif LANDSCAPE / PORTRAIT

	/*
	 * Update the location of all icons having the I_TEXT flag set and
	 * belonging to this window.
	 */
	iconp = iconsave;
	x = n_icons;
	while (--x >= 0) {
		if (iconp->id_win == wn && (iconp->id_flags & I_TEXT)) {
#ifdef LANDSCAPE
			iconp->id_xorig = (xszsav - iconp->id_xorig - iconp->id_xsize) / fysiz;
			iconp->id_xsize /= fysiz;
			iconp->id_xsize *= wsp->fdata.fi_ysize;
			iconp->id_xorig = wsp->x_size - Muls(iconp->id_xorig, wsp->fdata.fi_ysize) -
					  iconp->id_xsize;
			iconp->id_yorig /= fxsiz;
			iconp->id_yorig *= wsp->fdata.fi_xsize;
			iconp->id_ysize /= fxsiz;
			iconp->id_ysize *= wsp->fdata.fi_xsize;
#else  PORTRAIT
			iconp->id_xorig /= fxsiz;
			iconp->id_xorig *= wsp->fdata.fi_xsize;
			iconp->id_yorig /= fysiz;
			iconp->id_yorig *= wsp->fdata.fi_ysize;
			iconp->id_xsize /= fxsiz;
			iconp->id_xsize *= wsp->fdata.fi_xsize;
			iconp->id_ysize /= fysiz;
			iconp->id_ysize *= wsp->fdata.fi_ysize;
#endif LANDSCAPE / PORTRAIT
		}
		iconp++;	/* next icon */
	}

	/*
	 * Reset the graphic cursor position.
	 */
	wsp->x_gcur = 0;
	wsp->y_gcur = 0;
	wsp->pshc.sx_gcur = 0;
	wsp->pshc.sy_gcur = 0;

	/*
	 * Reset the graphic origin.
	 */
	wsp->x_gorig = 0;
	wsp->y_gorig = 0;
	wsp->pshc.sx_gorig = 0;
	wsp->pshc.sy_gorig = 0;

	if (wsp->pshc.sstatus & GRAPHIC) {

		/*
		 * Reset the saved cursor flag to avoid problems.
		 */
		wsp->status &= ~CURSAVED;
	}
	else {

		/*
		 * Update the font saved by push cursor.
		 */
		wsp->pshc.scurfnt = wsp->fdata.fi_cur;
	}

	/*
	 * Now we are ready to alter the window.
	 */
	(void)alterwin(wn + MAXWINS, wc);

	/*
	 * Save new values for the window blow up facility.
	 */
	saveblowup(wsp);

	/*
	 * Enable text cursor blink again.
	 */
	onblnk();

	return(W_OK);
}	/* end of fontrplce() */



/*
 * This function erases the current line (the line where the cursor is
 * positioned) or a part of it in a window.
 * If flg == 0, erase the whole line.
 * If flg > 0, erase from the current cursor position to the end of the line
 *             (inclusive).
 * If flg < 0, erase from the start of the line to the current cursor position
 *             (inclusive).
 * The corresponding part in the window text save area is also erased.
 * The cursor position is not changed.
 */

void
eraseln(wn, flg)
register	wn;		/* window to erase in         */
register	flg;		/* flag indicating the action */
{
	register struct	winint	*wsp;		/* pointer to window status */
	register		ac_left;	/* left side to erase       */
	register		ac_up;		/* upper side to erase      */
	register		ac_low;		/* lower side to erase      */

	wsp = wstatp[wn];

	sav_eln(wsp, flg);	/* erase in window text save area */
#ifdef LANDSCAPE
	ac_up = wsp->x_orig + wsp->x_size - 1 - Muls(wsp->y_cur - 1, wsp->fdata.fi_ysize) - wsp->y_coffs;
	ac_low = ac_up - wsp->fdata.fi_ysize + 1;
#else  PORTRAIT
	ac_up = wsp->y_orig + Muls(wsp->y_cur - 1, wsp->fdata.fi_ysize) + wsp->y_coffs;
	ac_low = ac_up + wsp->fdata.fi_ysize - 1;
#endif LANDSCAPE / PORTRAIT

	if (flg == 0) {

		/*
		 * Erase the whole line.
		 */
#ifdef LANDSCAPE
		wipe(wn, ac_low, ac_up, wsp->avy_orig, wsp->avy_lrc);
#else  PORTRAIT
		wipe(wn, wsp->avx_orig, wsp->avx_lrc, ac_up, ac_low);
#endif LANDSCAPE / PORTRAIT
	}
	else {
#ifdef LANDSCAPE
		ac_left = wsp->y_orig + Muls(wsp->x_cur - 1, wsp->fdata.fi_xsize) + wsp->x_coffs;
		if (flg > 0) {

			/*
			 * Erase from the current cursor position to the end of
			 * the line.
			 */
			wipe(wn, ac_low, ac_up, ac_left, wsp->avy_lrc);
		}
		else {

			/*
			 * Erase from the start of the line up to and including
			 * the current cursor position.
			 */
			wipe(wn, ac_low, ac_up, wsp->avy_orig, ac_left + wsp->fdata.fi_xsize - 1);
		}
#else  PORTRAIT
		ac_left = wsp->x_orig + Muls(wsp->x_cur - 1, wsp->fdata.fi_xsize) + wsp->x_coffs;
		if (flg > 0) {

			/*
			 * Erase from the current cursor position to the end of
			 * the line.
			 */
			wipe(wn, ac_left, wsp->avx_lrc, ac_up, ac_low);
		}
		else {

			/*
			 * Erase from the start of the line up to and including
			 * the current cursor position.
			 */
			wipe(wn, wsp->avx_orig, ac_left + wsp->fdata.fi_xsize - 1, ac_up, ac_low);
		}
#endif LANDSCAPE / PORTRAIT
	}

	return;
}	/* end of eraseln() */



/*
 * This function erases the scroll region or a part of it of a window.
 * If flg == 0, erase the whole scroll region.
 * If flg > 0, erase from the current cursor position to the end of the scroll
 *             region (inclusive).
 * If flg < 0, erase from the beginning of the scroll region to the current
 *             cursor position (inclusive).
 * Also the corresponding part in the window text save area is erased.
 * The cursor position is not changed.
 */

void
erasescr(wn, flg)
register	wn;		/* window to erase in         */
register	flg;		/* flag indicating the action */
{
	register struct	winint	*wsp;	/* pointer to window status       */
	register		um;	/* upper margin (pixels) to erase */
	register		lm;	/* lower margin (pixels) to erase */

	wsp = wstatp[wn];
#ifdef LANDSCAPE
	um = wsp->x_orig + wsp->x_size - 1 - wsp->y_coffs;
	lm = um - Muls(wsp->bmr_cur, wsp->fdata.fi_ysize) + 1;
	um -= Muls(wsp->umr_cur - 1, wsp->fdata.fi_ysize);
#else  PORTRAIT
	um = wsp->y_orig + Muls(wsp->umr_cur - 1, wsp->fdata.fi_ysize) + wsp->y_coffs;
	lm = wsp->y_orig + Muls(wsp->bmr_cur, wsp->fdata.fi_ysize) - 1 + wsp->y_coffs;
#endif LANDSCAPE / PORTRAIT

	if (flg == 0) {

		/*
		 * Erase the whole scroll region.
		 */
#ifdef LANDSCAPE
		wipe(wn, lm, um, wsp->avy_orig, wsp->avy_lrc);
#else  PORTRAIT
		wipe(wn, wsp->avx_orig, wsp->avx_lrc, um, lm);
#endif LANDSCAPE / PORTRAIT
	}
	else {

		/*
		 * Erase from the beginning or to the end of the scroll region.
		 */
		eraseln(wn, flg);	/* erase a part of the current line */
#ifdef LANDSCAPE
		if (flg > 0) {

			/*
			 * Erase to the end of the scroll region.
			 */
			wipe(wn, lm, wsp->x_orig + wsp->x_size - 1 - Muls(wsp->y_cur, wsp->fdata.fi_ysize) -
			     wsp->y_coffs, wsp->avy_orig, wsp->avy_lrc);
		}
		else {

			/*
			 * Erase from the beginning of the scroll region.
			 */
			wipe(wn, wsp->x_orig + wsp->x_size - 1 - Muls(wsp->y_cur - 1, wsp->fdata.fi_ysize)
			     + 1 - wsp->y_coffs, um, wsp->avy_orig, wsp->avy_lrc);
		}
#else  PORTRAIT
		if (flg > 0) {

			/*
			 * Erase to the end of the scroll region.
			 */
			wipe(wn, wsp->avx_orig, wsp->avx_lrc,
			     wsp->y_orig + Muls(wsp->y_cur, wsp->fdata.fi_ysize) + wsp->y_coffs, lm);
		}
		else {

			/*
			 * Erase from the beginning of the scroll region.
			 */
			wipe(wn, wsp->avx_orig, wsp->avx_lrc, um,
			     wsp->y_orig + Muls(wsp->y_cur - 1, wsp->fdata.fi_ysize) - 1 + wsp->y_coffs);
		}
#endif LANDSCAPE / PORTRAIT
	}

	sav_esc(wsp, flg);	/* erase in the window text save area */

	return;
}	/* end of erasescr() */



/*
 * Check if the whole font box where the text cursor is positioned is visible
 * and if not scroll the window so it becomes visible.
 */

void
chkscroll(wn)
register	wn;		/* window to check */
{
	register struct	winint	 *wsp;	/* pointer to window status   */
	register struct	winstruc *wc;	/* used to scroll window      */
	register		 x;	/* x pixel position of cursor */
	register		 y;	/* y pixel position of cursor */
	register		 boolx;	/* boolean flag               */
	register		 booly;	/* boolean flag               */

	wsp = wstatp[wn];
	boolx = NO;
	booly = NO;
#ifdef LANDSCAPE
	x = wsp->x_size - Muls(wsp->y_cur - 1, wsp->fdata.fi_ysize) - wsp->y_coffs;
	y = Muls(wsp->x_cur, wsp->fdata.fi_xsize) + wsp->x_coffs;

	/*
	 * Check if the whole character at the cursor position is visible and
	 * if not scroll the window.
	 */
	if ((pix_d)(x - wsp->fdata.fi_ysize) < wsp->vx_orig || x >= wsp->vx_orig + wsp->vx_size) {
		boolx = YES;
	}
	if ((pix_d)(y - wsp->fdata.fi_xsize) < wsp->vy_orig || y >= wsp->vy_orig + wsp->vy_size) {
		booly = YES;
	}
	if (boolx != NO || booly != NO) {

		/*
		 * We have to scroll the window. Try to scroll in a way that
		 * puts the cursor in the middle of the window (horizontally
		 * and/or vertically depending on what makes the whole cursor
		 * not visible.
		 */
		wc = &ubuf.wbuf;
		loadstruc(wn, wc);
		if (boolx != NO) {
			x -= Div2(wc->wl_vxsize);
			if (x < 0) {
				x = 0;
			}
			else {
				boolx = wc->wl_xsize - wc->wl_vxsize;
				if (x > boolx) {
					x = boolx;
				}
			}
			wc->wl_xorig -= x - wc->wl_vxorig;
			wc->wl_vxorig = x;
		}
		if (booly != NO) {
			y -= Div2(wc->wl_vysize);
			if (y < 0) {
				y = 0;
			}
			else {
				booly = wc->wl_ysize - wc->wl_vysize;
				if (y > booly) {
					y = booly;
				}
			}
			wc->wl_yorig -= y - wc->wl_vyorig;
			wc->wl_vyorig = y;
		}

		/*
		 * Do the scroll.
		 */
		(void)alterwin(wn, wc);
	}
#else  PORTRAIT
	x = Muls(wsp->x_cur, wsp->fdata.fi_xsize) + wsp->x_coffs;
	y = Muls(wsp->y_cur, wsp->fdata.fi_ysize) + wsp->y_coffs;

	/*
	 * Check if the whole character at the cursor position is visible
	 * and if not scroll the window.
	 */
	if ((pix_d)(x - wsp->fdata.fi_xsize) < wsp->vx_orig || x >= wsp->vx_orig + wsp->vx_size) {
		boolx = YES;
	}
	if ((pix_d)(y - wsp->fdata.fi_ysize) < wsp->vy_orig || y >= wsp->vy_orig + wsp->vy_size) {
		booly = YES;
	}
	if (boolx != NO || booly != NO) {

		/*
		 * We have to scroll the window. Try to scroll in a way that
		 * puts the cursor in the middle of the window (horizontally
		 * and/or vertically depending on what makes the whole cursor
		 * not visible).
		 */
		wc = &ubuf.wbuf;
		loadstruc(wn, wc);
		if (boolx != NO) {
			x -= Div2(wc->wp_vxsize);
			if (x < 0) {
				x = 0;
			}
			else {
				boolx = wc->wp_xsize - wc->wp_vxsize;
				if (x > boolx) {
					x = boolx;
				}
			}
			wc->wp_xorig -= x - wc->wp_vxorig;
			wc->wp_vxorig = x;
		}
		if (booly != NO) {
			y -= Div2(wc->wp_vysize);
			if (y < 0) {
				y = 0;
			}
			else {
				booly = wc->wp_ysize - wc->wp_vysize;
				if (y > booly) {
					y = booly;
				}
			}
			wc->wp_yorig -= y - wc->wp_vyorig;
			wc->wp_vyorig = y;
		}

		/*
		 * Do the scroll.
		 */
		(void)alterwin(wn, wc);
	}
#endif LANDSCAPE / PORTRAIT

	return;
}	/* end of chkscroll() */



/*
 * Scroll the contents of a window a number of character lines, both on the
 * screen and in the window text save area.
 * If 'lines' is positive, scroll up (normal scroll).
 * If 'lines' is negative, scroll down (reverse scroll).
 * The cursor position is not changed by this function.
 */

void
scrollwin(wn, lines)
int	wn;		/* window to scroll     */
int	lines;		/* # of lines to scroll */
{
	register struct	winint	 *wsp;	/* pointer to window status           */
	register struct	wpstruc	 *wrp;	/* pointer to rectangle group         */
	register struct	hwastruc *hwap;	/* pointer to hardware addresses      */
	register		 x;	/* x coord. of a rect. to scroll      */
	register		 y;	/* y coord. of a rect. to scroll      */
	register		 adown;	/* lowmost pixel in scroll region     */
	register		 aup;	/* upmost pixel in scroll region      */
	register		 sizy;	/* y size to move for a rectangle     */
	register		 pysc;	/* number of pixels to scroll         */
	int			 n;	/* counter                            */

	x = lines;		/* put lines and wn in register variables */
	y = wn;			/* to save some space */
	wsp = wstatp[y];

	/*
	 * If big scroll, clear the window instead.
	 */
	if (Abs(x) > wsp->bmr_cur - wsp->umr_cur) {
		erasescr(y, 0);
	}
	else {

		/*
		 * Scroll the window text save area.
		 */
		sav_scroll(wsp, x);

#ifdef LANDSCAPE
		aup = wsp->x_orig + wsp->x_size - 1 - wsp->y_coffs;
		adown = aup - Muls(wsp->bmr_cur, wsp->fdata.fi_ysize) + 1;
		aup -= Muls(wsp->umr_cur - 1, wsp->fdata.fi_ysize);
#else  PORTRAIT
		adown = wsp->y_orig + Muls(wsp->bmr_cur, wsp->fdata.fi_ysize) - 1 + wsp->y_coffs;
		aup = wsp->y_orig + Muls(wsp->umr_cur - 1, wsp->fdata.fi_ysize) + wsp->y_coffs;
#endif LANDSCAPE / PORTRAIT
		pysc = Muls(x, wsp->fdata.fi_ysize);	/* number of pixels to scroll */
		wrp = wpp[y];
		n = nwpart[y];

		/*
		 * Set up graphics for the clrfast() routine.
		 */
		hwap = &hwa;
		Waitsp();
		if (wsp->statflg & COMPMFL) {
			Outw(~0, hwap->grphaddr);
		}
		else {
			Outw(0, hwap->grphaddr);
		}

		if (x > 0) {		/* if normal scroll */

			/*
			 * Scroll all the visible rectangles of the window,
			 * one by one.
			 */
			while (--n >= 0) {
				if (wrp->bflags & B_NOTEMPTY) {
#ifdef LANDSCAPE
					x = wrp->yuv_p;
					y = wrp->xrv_p;
					if (y > aup) {
						y = aup;
					}
					sizy = wrp->xlv_p;
					if (sizy < adown) {
						sizy = adown;
					}
					sizy = y - sizy - pysc;
					if (sizy >= 0) {	/* Ok to scroll */
						Waitsp();
						Outb(GSTATFLG, hwap->mflagport);
						Move(y - pysc, x, y, x, (-sizy) & 0x3ff,
						     wrp->ylv_p - x, DX_UY);
					}

					/*
					 * If necessary fill in text using the
					 * saved text contents, if any.
					 */
					x = wrp->xlv_p;		/* save */
					y = wrp->xrv_p;
					wrp->xrv_p = x + pysc - 1;
					if (wrp->xrv_p > (pix_d)y) {
						wrp->xrv_p = y;
					}
					if (wrp->xrv_p > (pix_d)aup) {
						wrp->xrv_p = aup;
					}
					if (x < adown + pysc) {
						wrp->xlv_p = adown + pysc;
					}
					if (wrp->xlv_p <= wrp->xrv_p) {
						clrfast(wrp);
						sav_rewrit(wsp, wrp);
					}
					wrp->xlv_p = x;		/* restore */
					wrp->xrv_p = y;
#else  PORTRAIT
					x = wrp->xlv_p;
					y = wrp->yuv_p;
					if (y < aup) {
						y = aup;
					}
					sizy = wrp->ylv_p;
					if (sizy > adown) {
						sizy = adown;
					}
					sizy -= pysc + y;
					if (sizy >= 0) {	/* Ok to scroll */
						Waitsp();
						Outb(GSTATFLG, hwap->mflagport);
						Move(x, y + pysc, x, y, wrp->xrv_p - x, sizy, UX_UY);
					}

					/*
					 * If necessary fill in text using the
					 * saved text contents, if any.
					 */
					x = wrp->yuv_p;		/* save */
					y = wrp->ylv_p;
					wrp->yuv_p = y - pysc + 1;
					if (wrp->yuv_p < (pix_d)x) {
						wrp->yuv_p = x;
					}
					if (wrp->yuv_p < (pix_d)aup) {
						wrp->yuv_p = aup;
					}
					if (y > adown - pysc) {
						wrp->ylv_p = adown - pysc;
					}
					if (wrp->yuv_p <= wrp->ylv_p) {
						clrfast(wrp);
						sav_rewrit(wsp, wrp);
					}
					wrp->yuv_p = x;		/* restore */
					wrp->ylv_p = y;
#endif LANDSCAPE / PORTRAIT
				}	/* if (wrp->bflags & B_NOTEMPTY) */

				wrp++;		/* next rectangle */
			}	/* while (--n >= 0) */

			/*
			 * Now clear the bottom 'lines' lines in the scroll region.
			 */
#ifdef LANDSCAPE
			wipe(wn, adown, adown + pysc - 1, wsp->avy_orig, wsp->avy_lrc);
#else  PORTRAIT
			wipe(wn, wsp->avx_orig, wsp->avx_lrc, adown - pysc + 1, adown);
#endif LANDSCAPE / PORTRAIT

		}	/* if (x > 0) */

		else if (x < 0) {		/* if reverse scroll */

			wrp += n;		/* scroll the last rectangle first */

			/*
			 * Scroll all the visible rectangles of the window, one
			 * by one. Note, here 'pysc' is less than zero.
			 */
			while (--n >= 0) {
				wrp--;
				if (wrp->bflags & B_NOTEMPTY) {
#ifdef LANDSCAPE
					x = wrp->yuv_p;
					y = wrp->xlv_p;
					if (y < adown) {
						y = adown;
					}
					sizy = wrp->xrv_p;
					if (sizy > aup) {
						sizy = aup;
					}
					sizy -= y - pysc;
					if (sizy >= 0) {

						/*
						 * Ok to scroll.
						 */
						Waitsp();
						Outb(GSTATFLG, hwap->mflagport);
						Move(y - pysc, x, y, x, sizy, wrp->ylv_p - x, UX_UY);
					}

					/*
					 * If necessary fill in text using the
					 * saved text contents if any.
					 */
					x = wrp->xlv_p;		/* save */
					y = wrp->xrv_p;
					wrp->xlv_p = y + pysc + 1;
					if (wrp->xlv_p < (pix_d)x) {
						wrp->xlv_p = x;
					}
					if (wrp->xlv_p < (pix_d)adown) {
						wrp->xlv_p = adown;
					}
					if (y > aup + pysc) {
						wrp->xrv_p = aup + pysc;
					}
					if (wrp->xlv_p <= wrp->xrv_p) {
						clrfast(wrp);
						sav_rewrit(wsp, wrp);
					}
					wrp->xlv_p = x;
					wrp->xrv_p = y;
#else  PORTRAIT
					x = wrp->xlv_p;		/* left side */
					y = wrp->ylv_p;		/* lower side */
					if (y > adown) {
						y = adown;
					}
					sizy = wrp->yuv_p;
					if (sizy < aup) {
						sizy = aup;
					}
					sizy = y - sizy + pysc;
					if (sizy >= 0) {

						/*
						 * Ok to scroll.
						 */
						Waitsp();
						Outb(GSTATFLG, hwap->mflagport);

						/*
						 * We have to count down in the y direction
						 * in the mover, so the y size must be negative.
						 */
						Move(x, y + pysc, x, y, wrp->xrv_p - x, -sizy, UX_DY);
					}

					/*
					 * If necessary fill in text using the saved
					 * text contents, if any.
					 */
					x = wrp->yuv_p;		/* save */
					y = wrp->ylv_p;
					wrp->ylv_p = x - pysc - 1;
					if (wrp->ylv_p > (pix_d)y) {
						wrp->ylv_p = y;
					}
					if (wrp->ylv_p > (pix_d)adown) {
						wrp->ylv_p = adown;
					}
					if (x < aup - pysc) {
						wrp->yuv_p = aup - pysc;
					}
					if (wrp->yuv_p <= wrp->ylv_p) {
						clrfast(wrp);
						sav_rewrit(wsp, wrp);
					}
					wrp->yuv_p = x;		/* restore */
					wrp->ylv_p = y;
#endif LANDSCAPE / PORTRAIT
				}	/* if (wrp->bflags & B_NOTEMPTY) */

			}	/* while (--n >= 0) */

			/*
			 * Now clear the top '-lines' lines in the scroll region.
			 */
#ifdef LANDSCAPE
			wipe(wn, aup + pysc + 1, aup, wsp->avy_orig, wsp->avy_lrc);
#else  PORTRAIT
			wipe(wn, wsp->avx_orig, wsp->avx_lrc, aup, aup - pysc - 1);
#endif LANDSCAPE / PORTRAIT

		}	/* else if (x < 0) */
	}	/* else */

	return;
}	/* end of scrollwin() */



/*
 * This function is used by scrollwin() to clear an area fast. On entry it is
 * supposed that the mover flag port is set accordingly and that the
 * 'hwa.grphaddr' word contains the background pattern of the window.
 */

void
clrfast(wrp)
register struct	wpstruc	*wrp;	/* pointer to rectangle to clear */
{
	register struct	hwastruc *hwap;		/* pointer to hardware addresses */

	hwap = &hwa;
	Waitsp();
	Outb(GSTATFLG | HOLDYFL | HOLDXFL, hwap->mflagport);
	Grphrect(wrp->xlv_p, wrp->yuv_p, wrp->xrv_p - wrp->xlv_p, wrp->ylv_p - wrp->yuv_p);

	return;
}	/* end of clrfast() */



/*
 * Routine to enable text cursor blink.
 * A count common to onblnk() and offblnk() is kept so we know when it is
 * time to enable the blink again. When the count reaches zero it is time
 * to enable blink.
 */

void
onblnk()
{
	if (--(curdata.cs_bcnt) == 0 && curdata.cs_curvis != NO && nwin > 0 &&
	    (wstatp[ltow[0]]->termpara & TD_NONBLNK) == 0) {
		cmpgp->whc.flgs &= ~WHNOACT;
	}

	return;
}	/* end of onblnk() */



/*
 * Routine to stop text cursor blink. See comments for onblnk().
 */

void
offblnk()
{
	register struct	iogrst	 *iop;	/* pointer to shared memory page */
	register struct	hwastruc *hwap;	/* pointer to hardware addresses */

	curdata.cs_bcnt++;
	iop = cmpgp;
	if ((iop->whc.flgs & WHNOACT) == 0) {
		iop->whc.flgs |= WHNOACT;

		/*
		 * Make sure that the cursor is visible (i.e. the cursor area
		 * is inverted).
		 */
		if ((iop->whc.flgs & WHCPRES) == 0) {
			hwap = &hwa;
			Waitsp();
			Outb(GSTATFLG | COMPMFL, hwap->mflagport);
			Outw(iop->whc.xsize, hwap->x_sizeport);
			Outw(iop->whc.ysize, hwap->y_sizeport);
			Outw(iop->whc.xpos, hwap->x_toport);
			Outw(iop->whc.ypos, hwap->y_toport);
			Outw(iop->whc.xpos, hwap->x_fromport);
			Outw(iop->whc.ypos, hwap->y_fromport);
			iop->whc.flgs |= WHCPRES;
		}
	}

	return;
}	/* end of offblnk() */



/*
 * Routine to turn the text cursor on.
 * A count common to curson() and cursoff() is kept so we know when it is
 * time to turn on the cursor again. When the count reaches zero it is time
 * to turn the cursor on.
 */

void
curson()
{
	register		 x;		/* absolute position of cursor      */
	register		 y;		/* absolute position of cursor      */
	register		 xs;		/* size of visible part of cursor   */
	register		 ys;		/* size of visible part of cursor   */
	register struct	winint	 *wsp;		/* pointer to level 0 window status */
	register struct hwastruc *hwap;		/* pointer to hardware addresses    */
	register struct	cursave	 *cdp;		/* pointer to cursor status         */
	register struct	iogrst	 *iop;		/* pointer to shared memory page    */
	register		 tmp;		/* temporary                        */

	cdp = &curdata;
	if (--(cdp->cs_ccnt) == 0 && nwin > 0) {
		cdp->cs_curvis = NO;
		wsp = wstatp[ltow[0]];
		if ((wsp->estatus & NOCURSOR) == 0) {

			/*
			 * Cursor on attribute.
			 */
#ifdef LANDSCAPE
			xs = wsp->fdata.fi_ysize;
			ys = wsp->fdata.fi_xsize;
			x = wsp->x_orig + wsp->x_size - Muls(wsp->y_cur, xs) - wsp->y_coffs;
			y = wsp->y_orig + Muls(wsp->x_cur - 1, ys) + wsp->x_coffs;
#else  PORTRAIT
			xs = wsp->fdata.fi_xsize;
			ys = wsp->fdata.fi_ysize;
			x = wsp->x_orig + Muls(wsp->x_cur - 1, xs) + wsp->x_coffs;
			y = wsp->y_orig + Muls(wsp->y_cur - 1, ys) + wsp->y_coffs;
#endif LANDSCAPE / PORTRAIT

			if (wsp->termpara & TD_CUNDER) {

				/*
				 * Underline cursor.
				 */
#ifdef LANDSCAPE
				xs /= 5;
				if (xs == 0) {
					xs = 1;
				}
#else  PORTRAIT
				ys /= 5;
				if (ys == 0) {
					ys = 1;
				}
				y += wsp->fdata.fi_ysize - ys;
#endif LANDSCAPE / PORTRAIT
			}

			tmp = wsp->avx_orig;
			if (x < tmp) {
				xs -= tmp - x;
				x = tmp;
			}
			tmp = wsp->avy_orig;
			if (y < tmp) {
				ys -= tmp - y;
				y = tmp;
			}

			tmp = wsp->avx_lrc;
			if (x + --xs > tmp) {
				xs = tmp - x;
			}
			tmp = wsp->avy_lrc;
			if (y + --ys > tmp) {
				ys = tmp - y;
			}

			/*
			 * Now display the cursor (if any part of it is
			 * visible) by inverting the area occupied by the
			 * cursor.
			 */
			if (xs >= 0 && ys >= 0) {
				iop = cmpgp;
				xs |= UX_UY;
				iop->whc.xpos = x;
				iop->whc.ypos = y;
				iop->whc.xsize = xs;
				iop->whc.ysize = ys;
				hwap = &hwa;
				Waitsp();
				Outb(GSTATFLG | COMPMFL, hwap->mflagport);
				Outw(xs, hwap->x_sizeport);
				Outw(ys, hwap->y_sizeport);
				Outw(x, hwap->x_toport);
				Outw(y, hwap->y_toport);
				Outw(x, hwap->x_fromport);
				Outw(y, hwap->y_fromport);
				cdp->cs_curvis = YES;
			}
		}
	}

	return;
}	/* end of curson() */



/*
 * Routine to turn the text cursor off. See comments for curson().
 */

void
cursoff()
{
	register struct	hwastruc *hwap;		/* pointer to hardware addresses */
	register struct	iogrst	 *iop;		/* pointer to shared memory page */

	if (curdata.cs_ccnt++ == 0 && nwin > 0 && curdata.cs_curvis != NO) {
		hwap = &hwa;
		iop = cmpgp;
		Waitsp();
		Outb(GSTATFLG | COMPMFL, hwap->mflagport);
		Outw(iop->whc.xsize, hwap->x_sizeport);
		Outw(iop->whc.ysize, hwap->y_sizeport);
		Outw(iop->whc.xpos, hwap->x_toport);
		Outw(iop->whc.ypos, hwap->y_toport);
		Outw(iop->whc.xpos, hwap->x_fromport);
		Outw(iop->whc.ypos, hwap->y_fromport);
	}

	return;
}	/* end of cursoff() */



/*
 * This function sets a flag indicating that the window (excluding the border)
 * is overlapped by the mouse pointer if this is the case. This is repeated for
 * all currently created windows.
 */

void
mpover()
{
	register struct	winint	 *wsp;	/* pointer to window status */
	register struct	wpstruc	 *wrp;	/* pointer to rectangles    */
	register struct	pntstruc *pdp;	/* mouse pointer data       */
	register		 i;	/* loop index               */
	register		 n;	/* counter                  */

	pdp = &pntdata;
	i = MAXWINS - 1;
	do {
		wsp = wstatp[i];
		if (wsp != NULL && (wsp->status & CREATED)) {

			/*
			 * This window is created. Check if it is overlapped
			 * by the mouse pointer.
			 */
			wrp = wpp[i];
			n = nwpart[i];
			wsp->status &= ~MPOVER;
			while (--n >= 0) {
				if ((wrp->bflags & B_NOTEMPTY) &&
				    wrp->xlv_p <= pdp->pd_x + pdp->pd_xsize && wrp->xrv_p >= pdp->pd_x &&
				    wrp->yuv_p <= pdp->pd_y + pdp->pd_ysize && wrp->ylv_p >= pdp->pd_y) {
					wsp->status |= MPOVER;	/* it overlappes */
					break;
				}
				wrp++;		/* next rectangle */
			}
		}
	} while (--i >= 0);

	/*
	 * Set flag indicating that we have computed which windows are
	 * overlapped by the mouse pointer.
	 */
	pdp->pd_flags |= PD_OLAP;

	return;
}	/* end of mpover() */



/*
 * Change font in window wn.
 */

void
newfont(wn, fn)
register	wn;		/* window to change font for */
int		fn;		/* new font number           */
{
	register struct	winint	*wsp;	/* pointer to window status         */
	register struct	iogrst	*iop;	/* pointer to shared memory page    */
	register	fontinf	*fdp;	/* pointer to font data             */
	register		x;	/* pixel position of text cursor, x */
	register		y;	/* pixel position of text cursor, y */
	register		tmp;	/* temporary                        */

	if (loadfont(fn) != OK) {

		/*
		 * We failed to load the new font, keep the old one.
		 */
		return;
	}
	wsp = wstatp[wn];
	iop = cmpgp;
	fdp = &iop->fonttab[fn];
	tmp = 3 * Max(fdp->fbzx, fdp->fbzy);
	if (wsp->x_size < (pix_d)tmp || wsp->y_size < (pix_d)tmp) {

		/*
		 * The font is too big for the virtual screen, ignore it.
		 */
		return;
	}

	/*
	 * Now we know that we can use the font.
	 * Compute the coordinate of the left side of the baseline of the
	 * current cursor position.
	 */
	x = Muls(wsp->x_cur - 1, wsp->fdata.fi_xsize) + wsp->x_coffs;
	y = Muls(wsp->y_cur, wsp->fdata.fi_ysize) - wsp->fdata.fi_base + fdp->basl -
	    fdp->fbzy + wsp->y_coffs;
#ifdef LANDSCAPE
	tmp = wsp->y_size - fdp->fbzx;
#else  PORTRAIT
	tmp = wsp->x_size - fdp->fbzx;
#endif LANDSCAPE / PORTRAIT
	if (x > tmp) {
		x = tmp;
	}
	if (y < 0) {
		y = 0;
	}
#ifdef LANDSCAPE
	tmp = wsp->x_size - fdp->fbzy;
#else  PORTRAIT
	tmp = wsp->y_size - fdp->fbzy;
#endif LANDSCAPE / PORTRAIT
	if (y > tmp) {
		y = tmp;
	}

	/*
	 * Now we have computed all things needed to align the new font, set
	 * up all data for it.
	 */
	(void)adjfont(fn, &wsp->fdata);
	wsp->x_coffs = x % wsp->fdata.fi_xsize;
	wsp->y_coffs = y % wsp->fdata.fi_ysize;
	wsp->x_cur = (x - wsp->x_coffs) / wsp->fdata.fi_xsize + 1;
	wsp->y_cur = (y - wsp->y_coffs) / wsp->fdata.fi_ysize + 1;
#ifdef LANDSCAPE
	wsp->rm_cur = (wsp->y_size - wsp->x_coffs) / wsp->fdata.fi_xsize;
	wsp->bmr_cur = wsp->bm_cur = (wsp->x_size - wsp->y_coffs) / wsp->fdata.fi_ysize;
#else  PORTRAIT
	wsp->rm_cur = (wsp->x_size - wsp->x_coffs) / wsp->fdata.fi_xsize;
	wsp->bmr_cur = wsp->bm_cur = (wsp->y_size - wsp->y_coffs) / wsp->fdata.fi_ysize;
#endif LANDSCAPE / PORTRAIT
	wsp->umr_cur = 1;
	wsp->status |= GRAPHIC;		/* consider text as graphics */
	cvisible(wn, wpp[wn], nwpart[wn], EXTENDED);

	return;
}	/* end of newfont() */



/*
 * This function sets up all data for a font. If the font is not active, it is
 * loaded first.
 */

int
adjfont(fn, fip)
int			fn;		/* font number - 'A'        */
register struct	finfo	*fip;		/* data structure to set up */
{
	register fontinf *fdp;		/* pointer to area to fetch info from */
	register dword	 *fbp;		/* font box pointers store pointer    */
	register	 bzx;		/* font size, x                       */
	register	 bzy;		/* font size, y                       */
	register	 x;		/* pixel coordinate                   */
	register	 y;		/* pixel coordinate                   */
	unsigned char	 ascii;		/* ascii value of font box            */
	register	 cnt;		/* counter                            */
	register	 cntup;		/* counter limit                      */

	/*
	 * First load the font if necessary.
	 */
	if (loadfont(fn) != OK) {
		return(ERROR);
	}
	fdp = &cmpgp->fonttab[fn];
	fip->fi_flag &= ~NOFONT;
	fip->fi_cur = fn;
#ifdef LANDSCAPE
	fip->fi_xsize = bzy = fdp->fbzx;
	fip->fi_ysize = bzx = fdp->fbzy;
#else  PORTRAIT
	fip->fi_xsize = bzx = fdp->fbzx;
	fip->fi_ysize = bzy = fdp->fbzy;
#endif LANDSCAPE / PORTRAIT
	fip->fi_base = fdp->basl;
#ifdef LANDSCAPE
	fip->fi_chb = 116 / bzy;
#else  PORTRAIT
	fip->fi_chb = 256 / bzx;
#endif LANDSCAPE / PORTRAIT
	fip->fi_cfirst = fdp->firstc;
	fip->fi_clast = fdp->lastc;
	fip->fi_start = fdp->start;
	fip->fi_used |= 1 << fn;
	fip->fi_size = (((bzx - 1) | UX_UY) << 16) | (bzy - 1);
	fip->fi_fbas = &fip->fi_cgtab[0] - ' ';

	/*
	 * Now set up all the mover pointers to the font boxes. Font boxes not
	 * present in the range 0x20 to 0x7f are set to point to the first
	 * defined character font box.
	 */
	x = 768;
	y = fip->fi_start;
	ascii = fip->fi_clast + 1;
	fbp = fip->fi_fbas + ascii;
	while (ascii++ <= MAX7BCHR) {
		*fbp++ = (x << 16) | y;
	}
	ascii = ' ';
	fbp = fip->fi_fbas + ' ';
	while (ascii < fip->fi_cfirst) {
		*fbp++ = (x << 16) | y;
		ascii++;
	}
	cntup = (fdp->bwidth * 8) / bzx;
	cnt = 1;
	while (ascii++ <= fip->fi_clast) {
		*fbp++ = (x << 16) | y;
		x += bzx;
		if (cnt >= cntup) {
			x = 768;
			y += bzy;
			cnt = 0;
		}
		cnt++;
	}

	return(OK);
}	/* end of adjfont() */



/*
 * This function loads a new font. It returns OK if success, otherwise ERROR.
 */

int
loadfont(fn)
int	fn;	/* number of the new font - 'A' */
{
	register		i;	/* index                         */
	register		j;	/* loop index                    */
	register	long	mask;	/* to mask out "used" flags      */
	register		cnt;	/* counter                       */
	register		state;	/* state                         */
	register struct	winint	*wsp;	/* pointer to window status      */
	register struct	iogrst	*iop;	/* pointer to shared memory page */
	register	fontinf	*fdp;	/* pointer to font info          */
	register	fontinf	*newf;	/* pointer to new font info      */

	iop = cmpgp;
	newf = &iop->fonttab[fn];
	if ((newf->stat & FONTACTIVE) == 0) {

		/*
		 * The desired font is not in the graphic memory. Try to load
		 * the font. A special scheme is used if we have to free fonts:
		 * i)   First fonts which have not been used in the history of
		 *      any of the currently created windows are freed
		 *      (state = 0).
		 * ii)  Secondly fonts which are not the current font in any
		 *      window are freed (state = 1).
		 * iii) Thirdly the remaining fonts are freed (state = 2).
		 * Note that font 'A' is never freed.
		 */
		state = 0;
		i = MAXFONTCNT - 1;
		fdp = &iop->fonttab[MAXFONTCNT-1];
		cnt = 0;

		/*
		 * Stop text cursor blink.
		 */
		offblnk();

		while (state <= 2) {
			if (cnt == 0) {

				/*
				 * Try to load the font.
				 */
				if (fonthand(PFLOAD, fn) >= 0) {

					/*
					 * The font is now loaded.
					 */
					break;
				}
			}

			/*
			 * Skip not active fonts.
			 */
			while ((fdp->stat & FONTACTIVE) == 0 && i > 0) {
				fdp--;
				i--;
			}
			if (i == 0) {

				/*
				 * Go to the next state.
				 */
				state++;
				i = MAXFONTCNT - 1;
				fdp = &iop->fonttab[MAXFONTCNT-1];
				cnt = 1;	/* do not try to load */
			}
			else {
				cnt = 0;
				mask = 1 << i;
				j = MAXWINS - 1;
				do {
					wsp = wstatp[j];
					if (wsp != NULL && (wsp->status & CREATED)) {
						if ((state == 0 && (wsp->fdata.fi_used & mask)) ||
						    (state == 1 && wsp->fdata.fi_cur == i)) {
							cnt = 1;
						}
					}
				} while (--j >= 0);

				if (cnt == 0) {

					/*
					 * Free a font.
					 */
					fonthand(PFREMOVE, i);
				}

				/*
				 * Next font.
				 */
				fdp--;
				i--;
			}
		}	/* while (state <= 2) */

		/*
		 * Enable text cursor blink again.
		 */
		onblnk();

		/*
		 * Now go through all the windows and mark windows with not
		 * active or moved fonts.
		 */
		i = MAXWINS - 1;
		do {
			wsp = wstatp[i];
			if (wsp != NULL && (wsp->status & CREATED)) {
				fdp = &iop->fonttab[wsp->fdata.fi_cur];
				if ((fdp->stat & FONTACTIVE) == 0 ||
				    fdp->start != wsp->fdata.fi_start) {
					wsp->fdata.fi_flag |= NOFONT;
				}
			}
		} while (--i >= 0);

		/*
		 * Check if we succeded to load the font and that the font
		 * data is OK.
		 */
		if ((newf->stat & FONTACTIVE) == 0 || newf->fbzx == 0 || newf->fbzy == 0 ||
		    newf->fbzx > 32 || newf->fbzy > 32 || newf->basl >= newf->fbzy ||
		    newf->firstc < ' ' || newf->lastc > MAX7BCHR || newf->firstc > newf->lastc) {
			return(ERROR);		/* failed to load the font */
		}
	}

	return(OK);
}	/* end of loadfont() */



/*
 * This function loads or frees the font with the specified font number.
 */

int
fonthand(code, fn)
int	code;	/* PFLOAD or PFREMOVE depending on desired function */
char	fn;	/* font number - 'A'                                */
{
	fn += 'A';

	/*
	 * The mover status flags must be set up because this is not done
	 * by the tty driver.
	 */
	Waits();
	Outb(GSTATFLG, hwa.mflagport);
	return(dnix(F_IOCW, CON_LU, &fn, 1, code, 0, 0, 0));
}	/* end of fonthand() */
