/* winfunc1.c */

/*
 * This file contains relatively high level routines to handle windows for the
 * ABC1600 window handler (part 1).
 */

/* 1984-07-06, Peter Andersson, Luxor Datorer AB (at DIAB) */

/*
 * Functions in this file:
 *	addwin()
 *	removwin()
 *	chwglevel()
 *	chwtlig()
 *	chw1lev()
 *	alterwin()
 *	alter1win()
 */


#include	"../hsrc/defs.h"
#include	"../hsrc/structs.h"
#include	"../hsrc/eglobals.h"
#include	"../hsrc/graphconst.h"
#include	"../hsrc/macros.h"
#include	"../hsrc/dnixmacros.h"
#include	<signal.h>



/*
 * This function adds a window on the top level, on the highest level in a
 * window group, or on the lowest level if it is a special window and it does
 * not belong to a window group.
 * On entry the pointer argument must point to a structure of the same type
 * as that used by the request to create a new window.
 */

int
addwin(wn, wc)
register		 wn;	/* window number         */
register struct	winstruc *wc;	/* data of window to add */
{
	register struct	winint	*wsp;	/* status of created window   */
	register struct	winint	*wsp2;	/* various uses               */
	struct		winint	*twsp;	/* temp. pointer to window    */
	register		i;	/* loop index                 */
	register		tmp;	/* temporary and loop index   */
	register		xt;	/* temporary                  */
	register		yt;	/* temporary                  */
	register		lev;	/* level of new window        */
	int			wnt;	/* temporary window number    */
	register struct	wpstruc	*rgp;	/* pointer to rectangle group */
	sint		ovrlp[MAXWINS];	/* temporary save of window
					   overlapped flags           */

	wsp = wstatp[wn];
	if (wsp->status & CREATED) {
		return(WE_ALRCR);		/* window already created */
	}

	/*
	 * First check that the data is defined for landscape or portrait mode.
	 */
#ifdef LANDSCAPE
	if ((wc->w_flags & LMODE) == 0) {
		return(WE_ILMOD);
	}
#else  PORTRAIT
	if ((wc->w_flags & PMODE) == 0) {
		return(WE_ILMOD);
	}
#endif LANDSCAPE / PORTRAIT

	if ((wc->w_boxes & BX_USER) == 0) {
		wc->w_uboxes = 0;
	}

	/*
	 * Check that all the given parameters are legal.
	 */
	if (wc->w_border < 0 || wc->w_border >= MAXBORDER ||
	    wc->w_color < BLACK || wc->w_color > WHITE ||
#ifdef LANDSCAPE
	    wc->wl_font < 'A' || wc->wl_font > 'Z' ||
#else  PORTRAIT
	    wc->wp_font < 'A' || wc->wp_font > 'Z' ||
#endif LANDSCAPE / PORTRAIT
	    (wc->w_flags & (LOCK | SPECIAL)) == (LOCK | SPECIAL) ||
	    wc->w_tsig < 0 || wc->w_tsig >= NSIG ||
	    wc->w_ntsig < 0 || wc->w_ntsig >= NSIG ||
	    wc->w_csig < 0 || wc->w_csig >= NSIG ||
	    wc->w_rsig < 0 || wc->w_rsig >= NSIG ||
	    ckgrpflg(wc->w_flags) > 1 || wc->w_uboxes < 0) {
		return(WE_ILPARA);	/* illegal parameter(s) */
	}

	/*
	 * Load the desired default font.
	 */
#ifdef LANDSCAPE
	wsp->zoomlist[0] = wsp->font = wc->wl_font;
#else  PORTRAIT
	wsp->zoomlist[0] = wsp->font = wc->wp_font;
#endif LANDSCAPE / PORTRAIT
	if (adjfont(wsp->font - 'A', &wsp->fdata) != OK) {
		return(WE_NOFONT);		/* can't load the font */
	}

	/*
	 * Save the size of the default font.
	 */
	wsp->dflxsz = wsp->fdata.fi_xsize;
	wsp->dflysz = wsp->fdata.fi_ysize;

	/*
	 * If the TXTSIZE flag is set, convert to pixel sizes instead.
	 */
	if (wc->w_flags & TXTSIZE) {
#ifdef LANDSCAPE
		wc->wl_xsize *= wsp->dflxsz;
		wc->wl_ysize *= wsp->dflysz;
#else  PORTRAIT
		wc->wp_xsize *= wsp->dflxsz;
		wc->wp_ysize *= wsp->dflysz;
#endif LANDSCAPE / PORTRAIT
		txttopix(wc, wsp);
	}

	/*
	 * A special window shall be put on the lowest level.
	 */
	lev = 0;
	if (wc->w_flags & SPECIAL) {
		lev = nwin;
	}

	/*
	 * Find a possible father in this process group.
	 * WARNING: The obtained value of wsp2 is used far below.
	 */
	for (i = 0 ; i < MAXWINS ; i++) {
		wsp2 = wstatp[i];
		if (wsp2 != NULL && (wsp2->status & FATHER) && wsp2->pgid == wsp->pgid) {

			/*
			 * Found a father. Compute the absolute level of the
			 * new window.
			 */
			if (wc->w_flags & WGROUP) {

				/*
				 * The window shall belong to a window group.
				 * Check that the father and the child is of
				 * the same type, i.e. SPECIAL or non-SPECIAL
				 * windows.
				 */
				if ((wc->w_flags ^ wsp2->estatus) & SPECIAL) {
					return(WE_ILPARA);	/* different types */
				}
				lev = fndgroup(i, &wnt);	/* 'wnt' is used as dummy */
			}
			break;
		}
		wsp2 = NULL;
	}

	/*
	 * If the RELATIVE flag for the window is set, adjust the origin of
	 * the virtual screen.
	 */
	if (wc->w_flags & RELATIVE) {
		if (wsp2 == NULL) {
			return(WE_FATHER);	/* no father window */
		}
#ifdef LANDSCAPE
		wc->wl_xorig += wsp2->y_orig;
		wc->wl_yorig += wsp2->x_orig;
#else  PORTRAIT
		wc->wp_xorig += wsp2->x_orig;
		wc->wp_yorig += SCYSIZE - wsp2->y_orig - wsp2->y_size;
#endif LANDSCAPE / PORTRAIT
	}

	/*
	 * Convert the coordinates to those used internally.
	 */
	cnvcoord(wc);

	/*
	 * Check if the window will fit on the screen. If possible, adjust the
	 * values instead of returning illegal parameter status.
	 */
	(struct borddst *)rgp = &borddata[wc->w_border];	/* point to border data */
	adjustwin(wc, (struct borddst *)rgp);			/* adjust window size */

#ifdef LANDSCAPE
	if (wc->wl_vxsize <= 0 || wc->wl_vysize <= 0 ||
	    wc->wl_vxorig < 0 || wc->wl_vyorig < 0 ||
	    wc->wl_xsize > MAXWINSIZE || wc->wl_ysize > MAXWINSIZE) {
		return(WE_ILPARA);	/* cannot make window fit */
	}
	if ((wc->w_flags & ALLSCR) && (wc->wl_xsize != wc->wl_vxsize || wc->wl_ysize != wc->wl_vysize)) {
		return(WE_ALLSCR);	/* whole virtual screen must be visible */
	}
#else  PORTRAIT
	if (wc->wp_vxsize <= 0 || wc->wp_vysize <= 0 ||
	    wc->wp_vxorig < 0 || wc->wp_vyorig < 0 ||
	    wc->wp_xsize > MAXWINSIZE || wc->wp_ysize > MAXWINSIZE) {
		return(WE_ILPARA);	/* can't make window fit */
	}
	if ((wc->w_flags & ALLSCR) && (wc->wp_xsize != wc->wp_vxsize || wc->wp_ysize != wc->wp_vysize)) {
		return(WE_ALLSCR);	/* whole virtual screen must be visible */
	}
#endif LANDSCAPE / PORTRAIT

	/*
	 * Fill the internal window status structure.
	 */
#ifdef LANDSCAPE
	wsp->x_size = wc->wl_xsize;
	wsp->y_size = wc->wl_ysize;
#else  PORTRAIT
	wsp->x_size = wc->wp_xsize;
	wsp->y_size = wc->wp_ysize;
#endif LANDSCAPE / PORTRAIT
	wsp->uboxmax = wc->w_uboxes;
	wsp->uboxcnt = 0;
	wsp->boxes = wc->w_boxes;
	wsp->bordflags = ((struct borddst *)rgp)->bd_flags;	/* get border flags */
	copystruc(wsp, wc, (struct borddst *)rgp);
	wsp->border = wc->w_border;
	wsp->color = wc->w_color;
	wsp->tsig = wc->w_tsig;
	wsp->ntsig = wc->w_ntsig;
	wsp->rsig = wc->w_rsig;
	wsp->csig = wc->w_csig;
	wsp->statflg = GSTATFLG;
	if (wsp->color == WHITE) {
		wsp->statflg |= COMPMFL;
	}
	if (inversevideo != NO) {
		wsp->statflg ^= COMPMFL;
	}
	if (ttydflt.td_term & TD_SCREEN) {
		wsp->statflg ^= COMPMFL;
	}
	wsp->estatus = wc->w_flags;
#ifdef LANDSCAPE
	wsp->estatus &= ~(PMODE | OVERLAP | SAVEBITMAP | TXTSIZE);
	wsp->x_gcur = wc->wl_vyorig;
	wsp->y_gcur = wc->wl_vxorig;
#else  PORTRAIT
	wsp->estatus &= ~(LMODE | OVERLAP | SAVEBITMAP | TXTSIZE);
	wsp->x_gcur = wc->wp_vxorig;
	wsp->y_gcur = wc->wp_ysize - wc->wp_vyorig - wc->wp_vysize;
#endif LANDSCAPE / PORTRAIT

	xt = wsp->fdata.fi_xsize;
	yt = wsp->fdata.fi_ysize;
#ifdef LANDSCAPE
	i = wsp->y_size / xt;
#else  PORTRAIT
	i = wsp->x_size / xt;
#endif LANDSCAPE / PORTRAIT
	if (i > MAXCOLS) {
		i = MAXCOLS;	/* limit number of columns */
	}
	wsp->columns = i;
#ifdef LANDSCAPE
	wsp->rows = wsp->x_size / yt;
	i = wsp->vy_orig;
#else  PORTRAIT
	wsp->rows = wsp->y_size / yt;
	i = wsp->vx_orig;
#endif LANDSCAPE / PORTRAIT
	wsp->x_cur = i / xt + 1;
	if (i % xt != 0) {
		wsp->x_cur++;
	}
	if (wsp->x_cur > wsp->columns) {
		wsp->x_cur = 1;
	}
#ifdef LANDSCAPE
	i = wsp->x_size - wsp->vx_orig - wsp->vx_size;
#else  PORTRAIT
	i = wsp->vy_orig;
#endif LANDSCAPE / PORTRAIT
	wsp->y_cur = i / yt + 1;
	if (i % yt != 0) {
		wsp->y_cur++;
	}
	if (wsp->y_cur > wsp->rows) {
		wsp->y_cur = 1;
	}

	/*
	 * Do some more initializations.
	 */
	initwin(wsp);

	/*
	 * Initialize the structure for the simulated tty device driver.
	 */
	wsp->ttyd.t_delct = 0;
	wsp->ttyd.t_inq.c_cc = 0;
	wsp->ttyd.t_outq.c_cc = 0;
	wsp->ttyd.t_iflag = ttydflt.td_driver.c_iflag;
	wsp->ttyd.t_oflag = ttydflt.td_driver.c_oflag;
	wsp->ttyd.t_cflag = ttydflt.td_driver.c_cflag;
	wsp->ttyd.t_lflag = ttydflt.td_driver.c_lflag;
	cpcntlchr(&ttydflt.td_driver.c_ccs[0], &wsp->ttyd.t_ccs[0]);
	wsp->ttyd.t_ispeed = wsp->ttyd.t_ospeed = wsp->ttyd.t_cflag & CBAUD;
	wsp->ttyd.t_ccs[VCEOF] = wsp->ttyd.t_ccs[VEOF];
	wsp->ttyd.t_ccs[VCEOL] = wsp->ttyd.t_ccs[VEOL];

	/*
	 * Don't allow virtual screens smaller than 3 characters (both x and
	 * y directions).
	 */
	tmp = Muls(3, Max(wsp->fdata.fi_xsize, wsp->fdata.fi_ysize));
	if (wsp->x_size < (pix_d)tmp || wsp->y_size < (pix_d)tmp) {
		return(WE_ILPARA);	/* virtual screen too small */
	}

	/*
	 * Check that an old window or the new one with the NOOVER flag set
	 * will not be overlapped.
	 */
	yt = 0;		/* new window overlapped flag */
	if (nwin > 0) {

		/*
		 * First check if the new window will be overlapped.
		 */
		for (i = 0 ; i < lev ; i++) {
			yt += overlap(wsp, wstatp[ltow[i]]);
		}
		if (yt != 0 && (wsp->estatus & NOOVER)) {
			return(WE_LORO);	/* new window will be overlapped */
		}

		/*
		 * Check if windows on lower levels will be overlapped.
		 */
		for ( ; i < nwin ; i++) {
			twsp = wstatp[ltow[i]];
			if ((ovrlp[i] = overlap(wsp, twsp)) != 0 && (twsp->estatus & NOOVER)) {
				return(WE_LORO);	/* old window will be overlapped */
			}
		}

		/*
		 * Check if the level 0 window is LOCKed and the new window
		 * will be added on level 0 or if the new window is locked
		 * that it will be added on level 0.
		 */
		if ((lev == 0 && (wstatp[ltow[0]]->estatus & LOCK)) || (lev != 0 && (wsp->estatus & LOCK))) {
			return(WE_LORO);
		}
	}

	/*
	 * Allocate memory for the window.
	 */
	if (sav_alloc(wsp) != OK) {
		return(WE_NOMEM);		/* no memory */
	}

	wsp->status = CREATED | MOUSCHNG;	/* mark window as created */
	if ((wsp->estatus & WGROUP) && wsp2 == NULL) {
		wsp->status |= FATHER;		/* it is a father window */
	}

	/*
	 * Set overlap flag for the new window.
	 */
	if (yt != 0) {
		wsp->estatus |= OVERLAP;
	}

	/*
	 * Update all the overlapped flags for all the old windows.
	 */
	for (i = lev ; i < nwin ; i++) {
		if (ovrlp[i] != 0) {
			wstatp[ltow[i]]->estatus |= OVERLAP;
		}
	}

	/*
	 * Move all windows on level 'lev' and lower one level down.
	 */
	for (i = MAXWINS ; i >= 0 ; i--) {
		if (wtol[i] >= lev) {
			wtol[i]++;
		}
		if (i > lev) {
			ltow[i] = ltow[i-1];
		}
	}
	wtol[wn] = lev;
	ltow[lev] = wn;

	/*
	 * Make some initializations before splitting the new window.
	 */
	nwpart[wn] = 0;
	wpp[wn] = &wp[0] + rtotal;	/* point to area to store the rectangles */

	/*
	 * Now we have one more window.
	 */
	nwin++;
	if ((wsp->estatus & SPECIAL) == 0) {
		nnswin++;	/* one more non-special window */
	}

	/*
	 * Split the new window.
	 */
	wsplit(wn);

	/*
	 * Split all the rectangles belonging to the other windows when
	 * overlayed by the new window.
	 */
	rgp = &ubuf.wpt[0];
	for (i = lev + 1 ; i <= nwin ; i++) {		/* include the background */
		wnt = ltow[i];
		copyrects(wnt);		/* copy old rectangles to temp area */
		xt = yt = nwpart[wnt];
		for (tmp = 0 ; tmp < yt ; tmp++) {	/* split all rectangles in one window */
			xt = rsplit(wsp, rgp, rgp + (short)tmp, rgp + (short)xt);
		}
		tmp = rmsort(rgp, xt);		/* fix the rectangles */
		replrects(wnt, tmp);		/* insert new rectangles in wp[] */
		if (i < nwin) {			/* not for the background */
			cvisible(wnt, wpp[wnt], tmp, EXTENDED);
		}
	}

	/*
	 * Stop text cursor blink.
	 */
	offblnk();

	/*
	 * Remove the mouse pointer.
	 */
	mp_off();

	/*
	 * Remove any inverted icon.
	 */
	invoff();

	/*
	 * Alter the header for the old level 0 window from inverse to normal
	 * (if the flags says so) and, if appropriate, send a signal to the
	 * window.
	 */
	if (nwin > 1 && lev == 0) {
		disphdr(ltow[1], ON);
		wsp2 = wstatp[ltow[1]];
		if (wsp2->ntsig != 0) {
			Sigpgrp(wsp2->pgid, wsp2->ntsig);
		}
	}

	/*
	 * Draw an empty window on the screen.
	 */
	cursoff();		/* turn cursor off */
	tmp = nwpart[wn];
	rgp = wpp[wn];
	while (--tmp >= 0) {
		clrrect(wn, rgp++);
	}
	dispbox(wn);		/* display the boxes */
	curson();		/* turn cursor on in the new window */

	/*
	 * Restore possible inverted icon.
	 */
	iconchk();

	/*
	 * Insert the header if we already have one.
	 */
	disphdr(wn, ON);

	/*
	 * Restore the mouse pointer.
	 */
	chkmp_on();

	/*
	 * Enable text cursor blink again.
	 */
	onblnk();

	/*
	 * Turn off all LED's on the keyboard.
	 */
	wsp->ledflg = 0;
	adjleds();

	/*
	 * Save values for the window blow up facility.
	 */
	saveblowup(wsp);

	/*
	 * Clear flag indicating that which windows that are overlapped by the
	 * mouse pointer has been computed.
	 */
	pntdata.pd_flags &= ~PD_OLAP;

	return(W_OK);
}	/* end of addwin() */



/*
 * This function removes the window wn.
 */

void
removwin(wn)
register	wn;		/* window to remove */
{
	register		lev;		/* level of window to remove */
	register		i;		/* loop index                */
	register		j;		/* loop index                */
	register		tmp;		/* temporary                 */
	register struct	winint	*wsp;		/* pointer to window status  */
	register struct	winint	*twsp;		/* temporary pointer         */
	sint		olflgs[MAXWINS];	/* window overlapped flags   */

	wsp = wstatp[wn];
	if (wsp->status & CREATED) {

		/*
		 * The window has been created.
		 */
		lev = wtol[wn];

		/*
		 * If this window is a father, clear the WGROUP flags for all
		 * the windows belonging to this window group.
		 */
		if (wsp->status & FATHER) {
			i = MAXWINS - 1;
			do {
				twsp = wstatp[i];
				if (twsp != NULL && wsp->pgid == twsp->pgid) {
					twsp->estatus &= ~WGROUP;
				}
			} while (--i >= 0);
		}

		/*
		 * Determine which windows are overlapped by the window we are
		 * going to remove.
		 */
		i = MAXWINS - 1;	/* first clear all the flags */
		do {
			olflgs[i] = 0;
		} while (--i >= 0);
		for (i = lev + 1 ; i < nwin ; i++) {
			tmp = ltow[i];
			olflgs[tmp] = overlap(wsp, wstatp[tmp]);
		}

		/*
		 * Adjust the level-to-window and window-to-level mapping arrays.
		 */
		for (i = lev ; i < nwin ; i++) {	/* include the background */
			wtol[ltow[i]=ltow[i+1]]--;
		}

		/*
		 * Now we have one window less.
		 */
		nwin--;
		if ((wsp->estatus & SPECIAL) == 0) {
			nnswin--;	/* not a special window */
		}

		/*
		 * Split all the windows which were on lower levels than the removed
		 * one, but only if a window was overlapped by the removed window.
		 */
		for (i = lev ; i < nwin ; i++) {
			tmp = ltow[i];
			if (olflgs[tmp] != 0) {		/* if window was overlapped */
				wsplit(tmp);		/* split the window */
			}
		}
		wsplit(MAXWINS);	/* split the background */

		/*
		 * Stop text cursor blink.
		 */
		offblnk();

		/*
		 * Remove the mouse pointer.
		 */
		mp_off();

		/*
		 * Remove a possible inverted icon.
		 */
		invoff();

		/*
		 * Remove the cursor if it is the level zero window we are removing.
		 */
		if (lev == 0) {
			cursoff();
		}

		/*
		 * Fix the background first (makes it look smother).
		 */
		updatewin(MAXWINS, wn);

		/*
		 * Rewrite all the newly split windows.
		 */
		for (i = 0 ; i < MAXWINS ; i++) {
			if (olflgs[i] != 0) {		/* if window was split */
				updatewin(i, wn);
			}
		}

		/*
		 * Remove the rectangles for the removed window.
		 */
		replrects(wn, 0);

		/*
		 * If the new level 0 window was not overlapped, invert the
		 * header (if the flags says so) and, if appropriate, send
		 * a signal to the new top level window.
		 */
		if (lev == 0) {
			if (nwin != 0) {
				i = ltow[0];
				if (olflgs[i] == 0) {
					disphdr(i, ON);
				}
				twsp = wstatp[i];
				if (twsp->tsig != 0) {
					Sigpgrp(twsp->pgid, twsp->tsig);
				}
			}

			/*
			 * Turn on the cursor.
			 */
			curson();
		}

		/*
		 * Invert an icon if we point to one.
		 */
		iconchk();

		/*
		 * Restore the mouse pointer.
		 */
		chkmp_on();

		/*
		 * Enable text cursor blink again.
		 */
		onblnk();

		/*
		 * Update the overlap flags for all the windows which were on
		 * lower levels than the removed one.
		 */
		for (i = lev ; i < nwin ; i++) {
			twsp = wstatp[ltow[i]];
			twsp->estatus &= ~OVERLAP;
			for (j = 0 ; j < i ; j++) {
				if (overlap(wstatp[ltow[j]], twsp) != 0) {
					twsp->estatus |= OVERLAP;
				}
			}
		}

		/*
		 * Adjust the keyboard LED's.
		 */
		adjleds();

		/*
		 * Clear flag indicating that which windows that are
		 * overlapped by the mouse pointer has been computed.
		 */
		pntdata.pd_flags &= ~PD_OLAP;

	}	/* if (wsp->status & CREATED) */

	/*
	 * Free the memory occupied by the window.
	 */
	wstatp[wn] = NULL;
	sav_free(wsp);

	return;
}	/* end of removwin() */



/*
 * Change level of a window group. If the given window belongs to the topmost
 * group and 'flag' == YES the topmost group is moved to the bottom. If it
 * belongs to a group not on the top, the window group is moved to the top.
 */

int
chwglevel(wn, flag)
int	wn;		/* window number                       */
int	flag;		/* if top group, move to bottom if YES */
{
	register struct	winint	*wsp;		/* pointer to window status */
	register struct	winint	*wsp2;		/* pointer to window status */
	register		i;		/* loop index               */
	register		j;		/* loop index               */
	register		tlev;		/* level of topmost window  */
	int			blev;		/* level of bottommost win  */
	register		fromlev;	/* move from 'fromlev'      */
	register		tolev;		/* to 'tolev'               */
	register		imax;		/* loop limit               */
	register		jmax;		/* loop limit               */

	wsp = wstatp[wn];
	if ((wsp->status & CREATED) == 0) {
		return(WE_NOTCR);	/* window not created yet */
	}
	if (wsp->estatus & SPECIAL) {
		return(WE_SPECIAL);	/* cannot move SPECIAL window */
	}

	/*
	 * Find out which windows to move.
	 */
	tlev = fndgroup(wn, &blev);

	if (flag == NO && tlev == 0) {
		return(W_OK);	/* window group already on the top */
	}

	/*
	 * Check that it is allowed to change the level of the window group.
	 */
	if (wstatp[ltow[0]]->estatus & LOCK) {
		return(WE_LORO);	/* level 0 window is LOCKed */
	}

	if (tlev != 0) {

		/*
		 * Move window group to the top.
		 */
		i = 0;
		imax = tlev;
		jmax = blev + 1;
		fromlev = tlev;
		tolev = 0;
	}
	else {

		/*
		 * Move window group to the bottom.
		 */
		i = tlev;
		imax = blev + 1;
		jmax = nnswin;
		fromlev = blev;
		tolev = nnswin - 1;
	}
	for ( ; i < imax ; i++) {
		wsp2 = wstatp[ltow[i]];
		if (wsp2->estatus & NOOVER) {
			for (j = imax ; j < jmax ; j++) {
				if (overlap(wsp2, wstatp[ltow[j]]) != 0) {
					return(WE_LORO);	/* window will be overlapped */
				}
			}
		}
	}

	/*
	 * Now move (if we have to).
	 */
	if (fromlev != tolev) {

		/*
		 * Now we know that it is OK to move the windows.
		 */
		wsp = wstatp[ltow[0]];	/* remember old level zero window */
		i = blev - tlev;	/* number of windows to move - 1 */
		while (i-- >= 0) {
			chw1lev(ltow[fromlev], tolev);
			if (tlev != 0) {
				fromlev++;
				tolev++;
			}
			else {
				fromlev--;
				tolev--;
			}
		}

		/*
		 * Send a signal to the old level zero window if appropriate.
		 */
		if (wsp->ntsig != 0) {
			Sigpgrp(wsp->pgid, wsp->ntsig);
		}

		/*
		 * Send a signal to the new top level window if appropriate.
		 */
		wsp = wstatp[ltow[0]];
		if (wsp->tsig != 0) {
			Sigpgrp(wsp->pgid, wsp->tsig);
		}

		/*
		 * Adjust the keyboard LEDs.
		 */
		adjleds();

		/*
		 * Clear flag indicating that which windows that are
		 * overlapped by the mouse pointer has been computed.
		 */
		pntdata.pd_flags &= ~PD_OLAP;
	}

	return(W_OK);
}	/* end of chwglevel() */



/*
 * Move a window to the top inside a window group.
 */

int
chwtlig(wn)
register	wn;		/* window number */
{
	register struct	winint	*wsp;	/* pointer to window status        */
	register struct	winint	*wsp2;	/* pointer to old level 0 window   */
	register		tolev;	/* destination level               */
	register		fromlev;/* source level                    */
	register		i;	/* loop index                      */
	int			dummy;	/* used as dummy function argument */

	wsp = wstatp[wn];

	if ((wsp->status & CREATED) == 0) {
		return(WE_NOTCR);	/* window is not created yet */
	}

	/*
	 * Find the top level window in group.
	 */
	tolev = fndgroup(wn, &dummy);
	fromlev = wtol[wn];

	if (tolev != fromlev) {

		/*
		 * We have to change level of the window.
		 * Check if we will overlap any window which must not be
		 * overlapped.
		 */
		for (i = tolev ; i < fromlev ; i++) {
			wsp2 = wstatp[ltow[i]];
			if ((wsp2->estatus & NOOVER) && overlap(wsp2, wsp) != 0) {
				return(WE_LORO);	/* a window will be overlapped */
			}
		}

		/*
		 * Remember old level 0 window.
		 */
		wsp2 = wstatp[ltow[0]];

		/*
		 * We are allowed to change the level of the window.
		 */
		chw1lev(wn, tolev);
		if (tolev == 0) {

			/*
			 * We have moved the window to level 0.
			 */
			if (wsp2->ntsig != 0) {
				Sigpgrp(wsp2->pgid, wsp2->ntsig);
			}
			if (wsp->tsig != 0) {
				Sigpgrp(wsp->pgid, wsp->tsig);
			}

			/*
			 * Adjust the keyboard LEDs.
			 */
			adjleds();
		}

		/*
		 * Clear the flag indicating that which windows that are
		 * overlapped by the mouse pointer has been computed.
		 */
		pntdata.pd_flags &= ~PD_OLAP;
	}	/* if (tolev != fromlev) */

	return(W_OK);
}	/* end of chwtlig() */



/*
 * This function moves window wn to the given level. It is supposed that it is
 * OK to move the window without overlapping any window with the OVERLAP flag
 * set, etc.
 */

void
chw1lev(wn, newlev)
register	wn;		/* window number           */
register	newlev;		/* level to move window to */
{
	register		i;		/* loop index                */
	register		tmp;		/* temporary and loop index  */
	register		lev;		/* level of window wn        */
	register		n1;		/* help variable             */
	register		wnt;		/* temporary window number   */
	register struct	winint	*wsp;		/* pointer to window status  */
	register struct	wpstruc	*wrp;		/* pointer to rectangles     */
	register struct wpstruc *wrp1;		/* temp. pointer             */
	sint			ovrlp[MAXWINS];	/* overlap flags             */

	lev = wtol[wn];
	if (newlev == lev) {
		return;		/* do not need to do anything */
	}
	wsp = wstatp[wn];

	if (newlev < lev) {		/* if move to higher level */

		/*
		 * Move all the windows affected one level down.
		 */
		for (i = lev - 1 ; i >= newlev ; i--) {
			tmp = ltow[i];
			wtol[tmp]++;
			ltow[i+1] = tmp;
		}
		wtol[wn] = newlev;
		ltow[newlev] = wn;

		/*
		 * Split the moved window.
		 */
		wsplit(wn);

		/*
		 * Split all the rectangles belonging to windows that
		 * previously were on a higher level than the moved window.
		 */
		wrp = &ubuf.wpt[0];
		for (i = newlev + 1 ; i <= lev ; i++) {
			wnt = ltow[i];
			copyrects(wnt);
			n1 = tmp = nwpart[wnt];
			wrp1 = wrp;
			while (--tmp >= 0) {
				n1 = rsplit(wsp, wrp, wrp1++, wrp + n1);
			}
			tmp = rmsort(wrp, n1);
			cvisible(wnt, wrp, tmp, EXTENDED);
			replrects(wnt, tmp);
		}

		/*
		 * Stop text cursor blink.
		 */
		offblnk();

		/*
		 * Remove the mouse pointer.
		 */
		mp_off();

		/*
		 * Restore any inverted icon.
		 */
		invoff();

		if (newlev == 0) {

			/*
			 * Alter the old level zero header to normal video (if
			 * the flags says so).
			 */
			disphdr(ltow[1], ON);
		}

		/*
		 * Remove the cursor.
		 */
		cursoff();

		/*
		 * Rewrite the moved window if it was overlapped.
		 */
		if (wsp->estatus & OVERLAP) {
			tmp = 0;		/* this is not very optimal since */
			i = nwpart[wn];		/* the whole window is rewritten  */
			wrp = wpp[wn];
			while (--i >= 0) {
				clrrect(wn, wrp);
				if (wrp->bflags & B_NOTEMPTY) {
					sav_rewrit(wsp, wrp);
					tmp++;
				}
				wrp++;
			}
			if (tmp != 0) {
				redrawsig(wsp);
			}
			dispbox(wn);
		}
		disphdr(wn, ON);

	}	/* if (newlev < lev) */

	else {		/* move window to lower level */

		/*
		 * Determine which windows are overlapped by the window to
		 * move.
		 */
		i = MAXWINS - 1;	/* first clear all the flags */
		do {
			ovrlp[i] = 0;
		} while (--i >= 0);
		for (i = lev + 1 ; i <= newlev ; i++) {
			tmp = ltow[i];
			ovrlp[tmp] = overlap(wsp, wstatp[tmp]);
		}

		/*
		 * Adjust the level-to-window and window-to-level mapping
		 * arrays.
		 */
		for (i = lev ; i < newlev ; i++) {
			wtol[ltow[i]=ltow[i+1]]--;
		}
		ltow[newlev] = wn;
		wtol[wn] = newlev;

		/*
		 * Split all the windows which were on lower levels than the
		 * moved window (and on higher levels than the new level for
		 * the moved window), but only if a window was overlapped by
		 * the moved window.
		 */
		for (i = 0 ; i < MAXWINS ; i++) {
			if (ovrlp[i] != 0) {
				wsplit(i);
			}
		}
		wsplit(wn);		/* split the moved window */

		/*
		 * Stop text cursor blink.
		 */
		offblnk();

		/*
		 * Remove the mouse pointer.
		 */
		mp_off();

		/*
		 * Restore any inverted icon.
		 */
		invoff();

		/*
		 * Alter the header on the old level zero window to non-inverse
		 * (or what the flags says).
		 */
		disphdr(wn, ON);

		/*
		 * Remove the cursor.
		 */
		cursoff();

		/*
		 * Rewrite all the newly split windows, except window wn.
		 */
		for (i = 0 ; i < MAXWINS ; i++) {
			if (ovrlp[i] != 0) {		/* if window was split */
				updatewin(i, wn);	/* update it */
			}
		}

		if (lev == 0) {

			/*
			 * Alter the header of the new top level window to
			 * inverse (if the flags says so).
			 */
			if (ovrlp[ltow[0]] == 0) {
				disphdr(ltow[0], ON);
			}
		}

	}	/* else */

	/*
	 * Turn on the cursor again.
	 */
	curson();

	/*
	 * Invert an icon if we are pointing to one.
	 */
	iconchk();

	/*
	 * Restore the mouse pointer.
	 */
	chkmp_on();

	/*
	 * Enable text cursor blink again.
	 */
	onblnk();

	/*
	 * Update the overlap flags for all the non-special windows.
	 */
	for (i = 0 ; i < nnswin ; i++) {
		wsp = wstatp[ltow[i]];
		wsp->estatus &= ~OVERLAP;
		for (n1 = 0 ; n1 < i ; n1++) {
			if (overlap(wstatp[ltow[n1]], wsp) != 0) {
				wsp->estatus |= OVERLAP;
				break;
			}
		}
	}

	return;
}	/* end of chw1lev() */



/*
 * This function alters a window. If it is a father and any of the children
 * shall "follow" the father, the children are also moved. Only those children
 * which can be moved without overriding any flag are moved. The return
 * status reflects the window refered to in the call to this function.
 * Further see alter1win().
 * If 'wn' is the window number plus MAXWINS, the window will be forced to be
 * rewritten, even if this routine does not think that any change has occured.
 * This facility is used by the zoom feature.
 * Note that the structure pointed to by 'wc' is used as work area when moving
 * the children and therefore this structure will contain garbage on return.
 */

int
alterwin(wn, wc)
int			 wn;	/* window number           */
register struct	winstruc *wc;	/* data for the alteration */
{
	register struct	winint	*wsp;		/* pointer to window status  */
	int			tlev;		/* top level window in group */
	int			blev;		/* bottom level win in group */
	register		i;		/* loop index                */
	register		xo;		/* change offset, x origin   */
	register		yo;		/* change offset, y origin   */
	register		xl;		/* change offset, lower x    */
	register		yl;		/* change offset, lower y    */
	register		rstat;		/* return status             */
	int			wn2;		/* window number             */
	int			forceflag;	/* force rewrite flag        */

	forceflag = NO;
	if (wn >= MAXWINS) {
		wn -= MAXWINS;
		forceflag = YES;
	}
	wsp = wstatp[wn];

	/*
	 * Get the absolute coordinates of the window corners before we alter
	 * the window.
	 */
	xo = wsp->avx_orig;
	yo = wsp->avy_orig;
	xl = wsp->avx_lrc;
	yl = wsp->avy_lrc;

	/*
	 * Now alter the specified window.
	 */
	rstat = alter1win(wn, wc, forceflag);
	if (rstat == W_OK && (wsp->status & FATHER)) {

		/*
		 * We succeded to alter the window and it is a father window.
		 * Compute the distance which the four corners have moved.
		 */
		xo -= wsp->avx_orig;
		yo -= wsp->avy_orig;
		xl -= wsp->avx_lrc;
		yl -= wsp->avy_lrc;

		/*
		 * Find the level of the topmost and the bottommost window
		 * in the group.
		 */
		tlev = fndgroup(wn, &blev);

		/*
		 * Now move all the other windows in the group (if the flags
		 * says so).
		 */
		for (i = tlev ; i <= blev ; i++) {
			wn2 = ltow[i];
			wsp = wstatp[wn2];
			if (wn2 != wn && (wsp->estatus & (REL_ULC | REL_URC | REL_LLC | REL_LRC))) {

				/*
				 * This child window shall be moved.
				 */
				loadstruc(wn2, wc);
				if (wsp->estatus & REL_ULC) {
#ifdef LANDSCAPE
					wc->wl_xorig -= xl;
					wc->wl_yorig -= yo;
#else  PORTRAIT
					wc->wp_xorig -= xo;
					wc->wp_yorig -= yo;
#endif LANDSCAPE / PORTRAIT
				}
				else if (wsp->estatus & REL_URC) {
#ifdef LANDSCAPE
					wc->wl_xorig -= xl;
					wc->wl_yorig -= yl;
#else  PORTRAIT
					wc->wp_xorig -= xl;
					wc->wp_yorig -= yo;
#endif LANDSCAPE / PORTRAIT
				}
				else if (wsp->estatus & REL_LLC) {
#ifdef LANDSCAPE
					wc->wl_xorig -= xo;
					wc->wl_yorig -= yo;
#else  PORTRAIT
					wc->wp_xorig -= xo;
					wc->wp_yorig -= yl;
#endif LANDSCAPE / PORTRAIT
				}
				else {	/* if (wsp->estatus & REL_LRC) */
#ifdef LANDSCAPE
					wc->wl_xorig -= xo;
					wc->wl_yorig -= yl;
#else  PORTRAIT
					wc->wp_xorig -= xl;
					wc->wp_yorig -= yl;
#endif LANDSCAPE / PORTRAIT
				}

				/*
				 * Now move the child window.
				 */
				(void)alter1win(wn2, wc, NO);
			}
		}
	}

	return(rstat);
}	/* end of alterwin() */



/*
 * This function alters a window.
 * On entry the pointer argument must point to a structure of the same type as
 * that used by the request to alter a window, except that the coordinates
 * must be those used internally by the window handler.
 */

int
alter1win(wn, wc, forceflag)
register		 wn;		/* window number           */
register struct	winstruc *wc;		/* data for the alteration */
int			 forceflag;	/* force rewrite flag      */
{
	register		i;		/* loop index                 */
	register		tmp;		/* temporary                  */
	register		xt;		/* help variable              */
	register		lev;		/* level of window to move    */
	register struct	winint	*wsp;		/* pointer to window to alter */
	register struct	winint	*wsp2;		/* temp. window pointer       */
	register struct	wpstruc	*wrp;		/* pointer to rectangle group */
	struct		borddst	*bdp;		/* pointer to border data     */
	sint			ovrlp[MAXWINS];	/* window overlapped flags    */
	sint			ovrlp2[MAXWINS];

	lev = wtol[wn];
	wsp = wstatp[wn];
	if ((wsp->status & CREATED) == 0) {
		return(WE_NOTCR);	/* window not created yet */
	}

	/*
	 * Check that the data is relevant for landscape or portrait mode.
	 */
#ifdef LANDSCAPE
	if ((wc->w_flags & LMODE) == 0) {
		return(WE_ILMOD);
	}
#else  PORTRAIT
	if ((wc->w_flags & PMODE) == 0) {
		return(WE_ILMOD);
	}
#endif LANDSCAPE / PORTRAIT

	/*
	 * Check if the virtual screen and the window will fit on the real
	 * screen. If possible, try to adjust the values instead of returning
	 * illegal parameter status.
	 */
	bdp = &borddata[wsp->border];		/* point to border data */
#ifdef LANDSCAPE
	wc->wl_xsize = wsp->x_size;
	wc->wl_ysize = wsp->y_size;
#else  PORTRAIT
	wc->wp_xsize = wsp->x_size;
	wc->wp_ysize = wsp->y_size;
#endif LANDSCAPE / PORTRAIT
	adjustwin(wc, bdp);		/* adjust window size */

#ifdef LANDSCAPE
	if (wc->wl_vxsize <= 0 || wc->wl_vysize <= 0 || wc->wl_vxorig < 0 || wc->wl_vyorig < 0) {
		return(WE_ILPARA);	/* illegal parameter */
	}
	if ((wsp->estatus & ALLSCR) && (wc->wl_xsize != wc->wl_vxsize || wc->wl_ysize != wc->wl_vysize)) {
		return(WE_ALLSCR);	/* whole virtual screen must be visible */
	}
#else  PORTRAIT
	if (wc->wp_vxsize <= 0 || wc->wp_vysize <= 0 || wc->wp_vxorig < 0 || wc->wp_vyorig < 0) {
		return(WE_ILPARA);	/* illegal parameter */
	}
	if ((wsp->estatus & ALLSCR) && (wc->wp_xsize != wc->wp_vxsize || wc->wp_ysize != wc->wp_vysize)) {
		return(WE_ALLSCR);	/* whole virtual screen must be visible */
	}
#endif LANDSCAPE / PORTRAIT

	/*
	 * Compute the absolute position of the window after the move
	 * (including the border).
	 */
	wrp = &wpt1r[0];
#ifdef LANDSCAPE
	wrp->xl_p = wc->wl_xorig + wc->wl_vxorig;
	wrp->xr_p = wrp->xl_p + wc->wl_vxsize - 1 + bdp->bd_up;
	wrp->xl_p -= bdp->bd_down;
	wrp->yu_p = wc->wl_yorig + wc->wl_vyorig;
	wrp->yl_p = wrp->yu_p + wc->wl_vysize - 1 + bdp->bd_right;
	wrp->yu_p -= bdp->bd_left;
#else  PORTRAIT
	wrp->xl_p = wc->wp_xorig + wc->wp_vxorig;
	wrp->xr_p = wrp->xl_p + wc->wp_vxsize - 1 + bdp->bd_right;
	wrp->xl_p -= bdp->bd_left;
	wrp->yu_p = wc->wp_yorig + wc->wp_vyorig;
	wrp->yl_p = wrp->yu_p + wc->wp_vysize - 1 + bdp->bd_down;
	wrp->yu_p -= bdp->bd_up;
#endif LANDSCAPE / PORTRAIT

	if (wsp->abx_orig != wrp->xl_p || wsp->aby_orig != wrp->yu_p ||
	    wsp->abx_lrc != wrp->xr_p || wsp->aby_lrc != wrp->yl_p) {

		/*
		 * The position and/or the size has changed.
		 */
		if (wsp->estatus & NOMOVE) {
			return(WE_NOMOVE);	/* window must not be moved */
		}

		/*
		 * If the moved window has the NOOVER flag set, check that it
		 * will not be overlapped when it has been moved.
		 */
		if (wsp->estatus & NOOVER) {
			for (i = 0 ; i < lev ; i++) {
				if (roverlap(wrp, wstatp[ltow[i]]) != 0) {
					return(WE_LORO);	/* window will be overlapped */
				}
			}
		}

		/*
		 * Check that the moved window does not overlap any window
		 * with the NOOVER flag set.
		 * Also set flags for all windows overlapped by the window
		 * to move.
		 */
		for (i = lev + 1 ; i < nwin ; i++) {
			tmp = ltow[i];
			wsp2 = wstatp[tmp];
			if ((ovrlp[tmp] = roverlap(wrp, wsp2)) != 0 && (wsp2->estatus & NOOVER)) {
				return(WE_LORO);
			}
			ovrlp2[tmp] = overlap(wsp, wsp2);
		}

		/*
		 * Now we know that it is OK to move the window. Update the
		 * internal window status structure, but first save the old
		 * absolute location of the window.
		 */
		wrp->xl_p = wsp->abx_orig;
		wrp->xr_p = wsp->abx_lrc;
		wrp->yu_p = wsp->aby_orig;
		wrp->yl_p = wsp->aby_lrc;
		copystruc(wsp, wc, bdp);

		/*
		 * Split the moved window.
		 */
		wsplit(wn);

		/*
		 * Split all the windows which are on lower levels than the
		 * moved window, but only if they were overlapped before and/
		 * or after the move.
		 */
		for (i = lev + 1 ; i < nwin ; i++) {
			tmp = ltow[i];
			if (ovrlp[tmp] != 0 || ovrlp2[tmp] != 0) {
				wsplit(tmp);
			}
		}

		wsplit(MAXWINS);		/* split the background */

		/*
		 * Update the screen.
		 */
		xt = rmsort(wrp, rsplit(wsp, wrp, wrp, wrp + 1));

		/*
		 * Stop text cursor blink.
		 */
		offblnk();

		/*
		 * Remove the mouse pointer.
		 */
		mp_off();

		/*
		 * Restore a possible inverted icon.
		 */
		invoff();

		/*
		 * Remove the cursor.
		 */
		if (lev == 0) {
			cursoff();
		}

		/*
		 * Update the background.
		 */
		alterupd(MAXWINS, xt);

		/*
		 * Rewrite the moved window.
		 */
		i = nwpart[wn];
		wrp = wpp[wn];
		while (--i >= 0) {
			clrrect(wn, wrp++);
		}
		disphdr(wn, ON);
		dispbox(wn);
		i = nwpart[wn];
		wrp = wpp[wn];
		tmp = 0;
		while (--i >= 0) {
			if (wrp->bflags & B_NOTEMPTY) {
				tmp++;
				sav_rewrit(wsp, wrp);
			}
			wrp++;
		}
		if (tmp != 0) {
			redrawsig(wsp);
		}

		/*
		 * Turn on the cursor again.
		 */
		if (lev == 0) {
			curson();
		}

		/*
		 * Update all the windows on lower levels than the moved one.
		 */
		for (i = lev + 1 ; i < nwin ; i++) {
			tmp = ltow[i];
			if (ovrlp2[tmp] != 0) {
				alterupd(tmp, xt);
			}
		}

		/*
		 * Update the overlap flags for all the windows which are on
		 * the same and lower levels than the moved one.
		 */
		for (i = lev ; i < nwin ; i++) {
			wsp = wstatp[ltow[i]];
			wsp->estatus &= ~OVERLAP;
			for (tmp = 0 ; tmp < i ; tmp++) {
				if (overlap(wstatp[ltow[tmp]], wsp) != 0) {
					wsp->estatus |= OVERLAP;
				}
			}
		}

		/*
		 * Check if an icon shall be inverted.
		 */
		iconchk();

		/*
		 * Restore the mouse pointer.
		 */
		chkmp_on();

		/*
		 * Enable text cursor blink again.
		 */
		onblnk();

	}	/* end of if location or size has changed */

	/*
	 * If just the visible part of the viritual screen has changed, just
	 * do some updating and rewrite the contents of the window.
	 */
#ifdef LANDSCAPE
	else if (wsp->vx_orig != wc->wl_vxorig || wsp->vy_orig != wc->wl_vyorig || forceflag != NO) {
#else  PORTRAIT
	else if (wsp->vx_orig != wc->wp_vxorig || wsp->vy_orig != wc->wp_vyorig || forceflag != NO) {
#endif LANDSCAPE / PORTRAIT

		/*
		 * Stop text cursor blink.
		 */
		offblnk();

		/*
		 * Remove the mouse pointer.
		 */
		mp_off();

		/*
		 * Restore a possible inverted icon.
		 */
		invoff();

		/*
		 * Remove the old visible part indicators.
		 */
		dispind(wn, wsp, OFF);

		/*
		 * Update the internal window status structure.
		 */
		copystruc(wsp, wc, bdp);

		i = nwpart[wn];
		wrp = wpp[wn];
		cvisible(wn, wrp, i, EXTENDED);

		/*
		 * Display the new visible part indicators.
		 */
		dispind(wn, wsp, ON);

		/*
		 * Remove the text cursor (must do this in order to keep the
		 * counter up to date).
		 */
		if (lev == 0) {
			cursoff();
		}

		/*
		 * Clear the whole window and then rewrite it.
		 */
		wipe(wn, 0, SCXSIZE - 1, 0, SCYSIZE - 1);
		tmp = 0;
		while (--i >= 0) {
			if (wrp->bflags & B_NOTEMPTY) {
				tmp++;
				sav_rewrit(wsp, wrp);
			}
			wrp++;
		}
		if (tmp != 0) {
			redrawsig(wsp);		/* signal the change */
		}
		if (lev == 0) {
			curson();
		}

		/*
		 * Check if an icon shall be inverted.
		 */
		iconchk();

		/*
		 * Restore the mouse pointer.
		 */
		mp_on();

		/*
		 * Enable text cursor blink again.
		 */
		onblnk();
	}

	/*
	 * Clear flag indicating that which windows that are overlapped by the
	 * mouse pointer has been computed.
	 */
	pntdata.pd_flags &= ~PD_OLAP;

	return(W_OK);
}	/* end of alter1win() */
