/* mousehand2.c */

/*
 * This file contains routines to handle the mouse (part 2).
 */

/* 1984-08-29, Peter Andersson, Luxor Datorer AB (at DIAB) */

/*
 * Functions in this file:
 *	mmovwin()
 *	msizwin()
 *	mscrollwin()
 *	hindscroll()
 *	vindscroll()
 *	pointicon()
 *	dcontour()
 *	contour()
 *	hcontour()
 *	vcontour()
 *	updmpos()
 *	iconchk()
 *	invon()
 *	invoff()
 *	invicon()
 *	sendicon()
 *	pwinno()
 *	waitmous()
 *	moussig()
 */


#include	"../hsrc/defs.h"
#include	"../hsrc/structs.h"
#include	"../hsrc/eglobals.h"
#include	"../hsrc/graphconst.h"
#include	"../hsrc/macros.h"



/*
 * Move a window using the mouse.
 */

void
mmovwin(wn)
int	wn;		/* window to move */
{
	register struct	winint	 *wsp;		/* pointer to window status */
	register struct	winstruc *wc;		/* used to move window      */
	register struct	borddst	 *bdp;		/* pointer to border data   */
	register struct pntstruc *pdp;		/* pointer to mouse status  */
	register		 x;		/* x coordinate of contour  */
	register		 y;		/* y coordinate of contour  */
	register		 xsize;		/* width of contour         */
	register		 ysize;		/* height of contour        */
	register		 xd;		/* mouse pointer relative   */
	register		 yd;		/* contour                  */

	/*
	 * Start by computing the area occupied by the window.
	 */
	pdp = &pntdata;
	wsp = wstatp[wn];
	x = wsp->abx_orig;
	y = wsp->aby_orig;
	xsize = wsp->abx_lrc - x;
	ysize = wsp->aby_lrc - y;

	/*
	 * Compute the distance between the pointer and the upper left corner
	 * of the window.
	 */
	xd = pdp->pd_xmpos - x;
	yd = pdp->pd_ympos - y;

	/*
	 * Draw the contour of the window.
	 */
	mp_off();
	contour(x, y, xsize, ysize, bg_cont);
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
			 * The mouse has moved. Remove the old mouse pointer
			 * and get the new position.
			 */
			updmpos();
			x = pdp->pd_xmpos - xd;
			y = pdp->pd_ympos - yd;
			if (x + xsize >= SCXSIZE) {
				x = SCXSIZE - 1 - xsize;
			}
			else if (x < 0) {
				x = 0;
			}
			if (y + ysize >= SCYSIZE) {
				y = SCYSIZE - 1 - ysize;
			}
			else if (y < 0) {
				y = 0;
			}

			/*
			 * Redraw the mouse pointer and the contour on the new
			 * positions.
			 */
			contour(x, y, xsize, ysize, bg_cont);
			mp_on();
		}
		else if (cmpgp->mflag == LEMKEYRE) {

			/*
			 * The left key has been released. Move the window to
			 * the new position.
			 */
			wc = &ubuf.wbuf;
			loadstruc(wn, wc);
			bdp = &borddata[wsp->border];
#ifdef LANDSCAPE
			wc->wl_xorig = x - wsp->vx_orig + bdp->bd_down;
			wc->wl_yorig = y - wsp->vy_orig + bdp->bd_left;
#else  PORTRAIT
			wc->wp_xorig = x - wsp->vx_orig + bdp->bd_left;
			wc->wp_yorig = y - wsp->vy_orig + bdp->bd_up;
#endif LANDSCAPE / PORTRAIT
			(void)alterwin(wn, wc);
			break;
		}
		else {

			/*
			 * If something else happened, abort.
			 */
			break;
		}
	}	/* for ( ; ; ) */

	return;
}	/* end of mmovwin() */



/*
 * Alter the size of a window using the mouse.
 */

void
msizwin(wn)
int	wn;		/* window to alter */
{
	register struct	winint	 *wsp;		/* pointer to window status     */
	register struct	winstruc *wc;		/* used to alter size of window */
	register struct	borddst	 *bdp;		/* pointer to border data       */
	register struct	pntstruc *pdp;		/* pointer to mouse status      */
	register		 x;		/* x coordinate of contour      */
	register		 y;		/* y coordinate of contour      */
	register		 xsize;		/* width of contour             */
	register		 ysize;		/* height of contour            */
	register		 xd;		/* mouse pointer relative       */
	register		 yd;		/* contour                      */
	int			 tmp;		/* temporary                    */

	/*
	 * Start by computing the area occupied by the window.
	 */
	pdp = &pntdata;
	wsp = wstatp[wn];
	x = wsp->abx_orig;
	y = wsp->aby_orig;
	xsize = wsp->abx_lrc - x;
	ysize = wsp->aby_lrc - y;
	bdp = &borddata[wsp->border];

	/*
	 * Compute the distance between the pointer and the lower right corner.
	 */
#ifdef LANDSCAPE
	xd = pdp->pd_xmpos - x;
#else  PORTRAIT
	xd = x + xsize - pdp->pd_xmpos;
#endif LANDSCAPE / PORTRAIT
	yd = y + ysize - pdp->pd_ympos;

	/*
	 * Draw the contour of the window.
	 */
	mp_off();
	dcontour(x, y, xsize, ysize, bdp, bg_cont);
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
		dcontour(x, y, xsize, ysize, bdp, PATTERN);
		mp_on();

		/*
		 * Check what has happened.
		 */
		if (cmpgp->mflag == MOUSEMVD) {

			/*
			 * The mouse has moved. Remove the old mouse pointer
			 * and get the new position.
			 */
			updmpos();
#ifdef LANDSCAPE
			xsize = wsp->abx_lrc - pdp->pd_xmpos + xd;
#else  PORTRAIT
			xsize = pdp->pd_xmpos - x + xd;
#endif LANDSCAPE / PORTRAIT
			ysize = pdp->pd_ympos - y + yd;
#ifdef LANDSCAPE
			tmp = wsp->x_size + bdp->bd_up + bdp->bd_down - 1;
#else  PORTRAIT
			tmp = wsp->x_size + bdp->bd_left + bdp->bd_right - 1;
#endif LANDSCAPE / PORTRAIT
			if (xsize > tmp) {
				xsize = tmp;
			}
#ifdef LANDSCAPE
			if (xsize < 2 * DLBWIDTH) {
				xsize = 2 * DLBWIDTH;
			}
			x = wsp->abx_lrc - xsize;
			if (x < 0) {
				xsize += x;
				x = 0;
			}
			tmp = wsp->y_size + bdp->bd_left + bdp->bd_right - 1;
#else  PORTRAIT
			if (x + xsize >= SCXSIZE) {
				xsize = SCXSIZE - 1 - x;
			}
			else if (xsize < 2 * DLBWIDTH) {
				xsize = 2 * DLBWIDTH;
			}
			tmp = wsp->y_size + bdp->bd_up + bdp->bd_down - 1;
#endif LANDSCAPE / PORTRAIT
			if (ysize > tmp) {
				ysize = tmp;
			}
			if (y + ysize >= SCYSIZE) {
				ysize = SCYSIZE - 1 - y;
			}
			else if (ysize < 2 * DLBWIDTH) {
				ysize = 2 * DLBWIDTH;
			}

			/*
			 * Redraw the mouse pointer and the contour on the new
			 * positions.
			 */
			dcontour(x, y, xsize, ysize, bdp, bg_cont);
			mp_on();
		}
		else if (cmpgp->mflag == LEMKEYRE) {

			/*
			 * The left key has been released. Alter the size of
			 * the window.
			 */
			wc = &ubuf.wbuf;
			loadstruc(wn, wc);
#ifdef LANDSCAPE
			x += bdp->bd_down;
			xsize -= bdp->bd_up + bdp->bd_down - 1;
			wc->wl_xorig += wc->wl_vxsize - xsize;
			tmp = wc->wl_xsize - xsize;
			if (wc->wl_vxorig > (pix_d)tmp) {
				wc->wl_vxorig = tmp;
				wc->wl_xorig = x - tmp;
			}
			wc->wl_vxsize = xsize;
#else  PORTRAIT
			x += bdp->bd_left;
			xsize -= bdp->bd_left + bdp->bd_right - 1;
			wc->wp_vxsize = xsize;
			tmp = wc->wp_xsize - xsize;
			if (wc->wp_vxorig > (pix_d)tmp) {
				wc->wp_vxorig = tmp;
				wc->wp_xorig = x - tmp;
			}
#endif LANDSCAPE / PORTRAIT
#ifdef LANDSCAPE
			y += bdp->bd_left;
			ysize -= bdp->bd_left + bdp->bd_right - 1;
			wc->wl_vysize = ysize;
			tmp = wc->wl_ysize - ysize;
			if (wc->wl_vyorig > (pix_d)tmp) {
				wc->wl_vyorig = tmp;
				wc->wl_yorig = y - tmp;
			}
#else  PORTRAIT
			y += bdp->bd_up;
			ysize -= bdp->bd_up + bdp->bd_down - 1;
			if (wc->wp_vysize > (pix_d)ysize) {

				/*
				 * The height is smaller. Make the lower side
				 * the same as before.
				 */
				yd = wc->wp_vysize - ysize;
				wc->wp_vysize = ysize;
				wc->wp_vyorig += yd;
				wc->wp_yorig -= yd;
			}
			else {

				/*
				 * The height is greater.
				 */
				wc->wp_vysize = ysize;
				tmp = wc->wp_ysize - ysize;
				if (wc->wp_vyorig > (pix_d)tmp) {
					wc->wp_vyorig = tmp;
					wc->wp_yorig = y - tmp;
				}
			}
#endif LANDSCAPE / PORTRAIT

			/*
			 * Now we are ready to alter the window.
			 */
			(void)alterwin(wn, wc);
			break;
		}
		else {

			/*
			 * If something else happened, abort.
			 */
			break;
		}
	}	/* for ( ; ; ) */

	return;
}	/* end of msizwin() */



/*
 * Scroll a window one row or column up, down, left, or right.
 * 'flag' indicates the direction as follows:
 *   flag = 0		scroll left
 *   flag = 1		scroll right
 *   flag = 2		scroll up
 *   flag = 3		scroll down
 */

void
mscrollwin(wn, flag)
register	wn;		/* window to scroll        */
register	flag;		/* scroll direction flag   */
{
	register struct	winstruc *wc;	/* structure used to alter window */
	register struct	winint	 *wsp;	/* window status structure        */
	register	pix_d	 tmp;	/* temporary                      */

	wc = &ubuf.wbuf;
	wsp = wstatp[wn];
	loadstruc(wn, wc);

	tmp = wsp->fdata.fi_xsize;
	if (flag == 0) {
#ifdef LANDSCAPE
		wc->wl_vyorig -= tmp;
		wc->wl_yorig += tmp;
		if (wc->wl_vyorig < 0) {
			wc->wl_yorig += wc->wl_vyorig;
			wc->wl_vyorig = 0;
		}
#else  PORTRAIT
		wc->wp_vxorig -= tmp;
		wc->wp_xorig += tmp;
		if (wc->wp_vxorig < 0) {
			wc->wp_xorig += wc->wp_vxorig;
			wc->wp_vxorig = 0;
		}
#endif LANDSCAPE / PORTRAIT
	}

	if (flag == 1) {
#ifdef LANDSCAPE
		wc->wl_vyorig += tmp;
		wc->wl_yorig -= tmp;
		tmp = wc->wl_ysize - wc->wl_vysize;
		if (wc->wl_vyorig >= tmp) {
			wc->wl_yorig += wc->wl_vyorig - tmp;
			wc->wl_vyorig = tmp;
		}
#else  PORTRAIT
		wc->wp_vxorig += tmp;
		wc->wp_xorig -= tmp;
		tmp = wc->wp_xsize - wc->wp_vxsize;
		if (wc->wp_vxorig >= tmp) {
			wc->wp_xorig += wc->wp_vxorig - tmp;
			wc->wp_vxorig = tmp;
		}
#endif LANDSCAPE / PORTRAIT
	}

	tmp = wsp->fdata.fi_ysize;
	if (flag == 2) {
#ifdef LANDSCAPE
		wc->wl_vxorig += tmp;
		wc->wl_xorig -= tmp;
		tmp = wc->wl_xsize - wc->wl_vxsize;
		if (wc->wl_vxorig >= tmp) {
			wc->wl_xorig += wc->wl_vxorig - tmp;
			wc->wl_vxorig = tmp;
		}
#else  PORTRAIT
		wc->wp_vyorig -= tmp;
		wc->wp_yorig += tmp;
		if (wc->wp_vyorig < 0) {
			wc->wp_yorig += wc->wp_vyorig;
			wc->wp_vyorig = 0;
		}
#endif LANDSCAPE / PORTRAIT
	}

	if (flag == 3) {
#ifdef LANDSCAPE
		wc->wl_vxorig -= tmp;
		wc->wl_xorig += tmp;
		if (wc->wl_vxorig < 0) {
			wc->wl_xorig += wc->wl_vxorig;
			wc->wl_vxorig = 0;
		}
#else  PORTRAIT
		wc->wp_vyorig += tmp;
		wc->wp_yorig -= tmp;
		tmp = wc->wp_ysize - wc->wp_vysize;
		if (wc->wp_vyorig >= tmp) {
			wc->wp_yorig += wc->wp_vyorig - tmp;
			wc->wp_vyorig = tmp;
		}
#endif LANDSCAPE / PORTRAIT
	}

	/*
	 * Now do the scroll.
	 */
	(void)alterwin(wn, wc);

	return;
}	/* mscrollwin() */



/*
 * Scroll a window  by moving the horizontal visible indicator.
 */

void
hindscroll(wn, left)
register	wn;		/* window number          */
register	left;		/* left side of indicator */
{
	register struct	winint	 *wsp;	/* pointer to window status       */
	register struct	winstruc *wc;	/* to scroll window               */
	register		 dist;	/* distance from left side        */
	register		 tmp;	/* temporary                      */

	wsp = wstatp[wn];

	/*
	 * Compute the distance between the pointer and the left side
	 * of the indicator.
	 */
#ifdef LANDSCAPE
	dist = pntdata.pd_ympos - left;
#else  PORTRAIT
	dist = pntdata.pd_xmpos - left;
#endif LANDSCAPE / PORTRAIT

	for ( ; ; ) {

		/*
		 * Wait until a change from the mouse.
		 */
		waitmous(PRESSED);
		mp_off();

		/*
		 * Remove the old indicator.
		 */
		dsphind(wn, left, OFF);

		/*
		 * Check what has happened.
		 */
		if (cmpgp->mflag == MOUSEMVD) {

			/*
			 * The mouse has moved. Move the indicator.
			 */
			updmpos();
#ifdef LANDSCAPE
			left = pntdata.pd_ympos - dist;
			if (left < wsp->aby_orig + DLBWIDTH + LBOXSIZE) {
				left = wsp->aby_orig + DLBWIDTH + LBOXSIZE;
			}
			else if (left > wsp->aby_lrc - DLBWIDTH - LBOXSIZE - WINDSIZE + 1) {
				left = wsp->aby_lrc - DLBWIDTH - LBOXSIZE - WINDSIZE + 1;
			}
#else  PORTRAIT
			left = pntdata.pd_xmpos - dist;
			if (left < wsp->abx_orig + DLBWIDTH + LBOXSIZE) {
				left = wsp->abx_orig + DLBWIDTH + LBOXSIZE;
			}
			else if (left > wsp->abx_lrc - DLBWIDTH - LBOXSIZE - WINDSIZE + 1) {
				left = wsp->abx_lrc - DLBWIDTH - LBOXSIZE - WINDSIZE + 1;
			}
#endif LANDSCAPE / PORTRAIT
			dsphind(wn, left, ON);
			mp_on();
		}
		else {

			/*
			 * The left button has been released or something
			 * illegal has happened.
			 */
			wc = &ubuf.wbuf;
			loadstruc(wn, wc);
#ifdef LANDSCAPE
			dist = Muls(left - (wsp->aby_orig + DLBWIDTH + LBOXSIZE), wsp->y_size);
			dist /= wsp->by_size - (2 * (DLBWIDTH + LBOXSIZE) + WINDSIZE);
			dist -= Div2(wsp->vy_size);
			if (dist < 0) {
				dist = 0;
			}
			tmp = wsp->y_size - wsp->vy_size;
			if (dist > tmp) {
				dist = tmp;
			}
			wc->wl_yorig += wsp->vy_orig - dist;
			wc->wl_vyorig = dist;
			if (cmpgp->mflag != LEMKEYRE || (pix_d)dist == wsp->vy_orig) {
				dispind(wn, wsp, ON);
				mp_on();
			}
			else {
				mp_on();
				(void)alterwin(wn, wc);
			}
#else  PORTRAIT
			dist = Muls(left - (wsp->abx_orig + DLBWIDTH + LBOXSIZE), wsp->x_size);
			dist /= wsp->bx_size - (2 * (DLBWIDTH + LBOXSIZE) + WINDSIZE);
			dist -= Div2(wsp->vx_size);
			if (dist < 0) {
				dist = 0;
			}
			tmp = wsp->x_size - wsp->vx_size;
			if (dist > tmp) {
				dist = tmp;
			}
			wc->wp_xorig += wsp->vx_orig - dist;
			wc->wp_vxorig = dist;
			if (cmpgp->mflag != LEMKEYRE || (pix_d)dist == wsp->vx_orig) {
				dispind(wn, wsp, ON);
				mp_on();
			}
			else {
				mp_on();
				(void)alterwin(wn, wc);
			}
#endif LANDSCAPE / PORTRAIT
			break;		/* exit forever loop */
		}
	}

	return;
}	/* end of hindscroll() */



/*
 * Scroll a window by moving the vertical visible indicator.
 */

void
vindscroll(wn, up)
register	wn;	/* window number                                     */
register	up;	/* upper side of indicator (lower side in landscape) */
{
	register struct	winint	 *wsp;	/* pointer to window status       */
	register struct	winstruc *wc;	/* to scroll window               */
	register		 dist;	/* distance from upper side       */
	register		 tmp;	/* temporary                      */

	wsp = wstatp[wn];

	/*
	 * Compute the distance between the pointer and the upper/lower side
	 * of the indicator.
	 */
#ifdef LANDSCAPE
	dist = pntdata.pd_xmpos - up;
#else  PORTRAIT
	dist = pntdata.pd_ympos - up;
#endif LANDSCAPE / PORTRAIT

	for ( ; ; ) {

		/*
		 * Wait until a change from the mouse.
		 */
		waitmous(PRESSED);
		mp_off();

		/*
		 * Remove the old indicator.
		 */
		dspvind(wn, up, OFF);

		/*
		 * Check what has happened.
		 */
		if (cmpgp->mflag == MOUSEMVD) {

			/*
			 * The mouse has moved, move the indicator.
			 */
			updmpos();
#ifdef LANDSCAPE
			up = pntdata.pd_xmpos - dist;
			if (up < wsp->abx_orig + DLBWIDTH + LBOXSIZE) {
				up = wsp->abx_orig + DLBWIDTH + LBOXSIZE;
			}
			else if (up > wsp->abx_lrc - DLBWIDTH - LBOXSIZE - WINDSIZE + 1) {
				up = wsp->abx_lrc - DLBWIDTH - LBOXSIZE - WINDSIZE + 1;
			}
#else  PORTRAIT
			up = pntdata.pd_ympos - dist;
			if (up < wsp->aby_orig + DLBWIDTH + LBOXSIZE) {
				up = wsp->aby_orig + DLBWIDTH + LBOXSIZE;
			}
			else if (up > wsp->aby_lrc - DLBWIDTH - LBOXSIZE - WINDSIZE + 1) {
				up = wsp->aby_lrc - DLBWIDTH - LBOXSIZE - WINDSIZE + 1;
			}
#endif LANDSCAPE / PORTRAIT
			dspvind(wn, up, ON);
			mp_on();
		}
		else {

			/*
			 * The buttons has changed in some way.
			 */
			wc = &ubuf.wbuf;
			loadstruc(wn, wc);
#ifdef LANDSCAPE
			dist = Muls(up - (wsp->abx_orig + DLBWIDTH + LBOXSIZE), wsp->x_size);
			dist /= wsp->bx_size - (2 * (DLBWIDTH + LBOXSIZE) + WINDSIZE);
			dist -= Div2(wsp->vx_size);
			if (dist < 0) {
				dist = 0;
			}
			tmp = wsp->x_size - wsp->vx_size;
			if (dist > tmp) {
				dist = tmp;
			}
			wc->wl_xorig += wsp->vx_orig - dist;
			wc->wl_vxorig = dist;
			if (cmpgp->mflag != LEMKEYRE || (pix_d)dist == wsp->vx_orig) {
				dispind(wn, wsp, ON);
				mp_on();
			}
			else {
				mp_on();
				(void)alterwin(wn, wc);
			}
#else  PORTRAIT
			dist = Muls(up - (wsp->aby_orig + DLBWIDTH + LBOXSIZE), wsp->y_size);
			dist /= wsp->by_size - (2 * (DLBWIDTH + LBOXSIZE) + WINDSIZE);
			dist -= Div2(wsp->vy_size);
			if (dist < 0) {
				dist = 0;
			}
			tmp = wsp->y_size - wsp->vy_size;
			if (dist > tmp) {
				dist = tmp;
			}
			wc->wp_yorig += wsp->vy_orig - dist;
			wc->wp_vyorig = dist;
			if (cmpgp->mflag != LEMKEYRE || (pix_d)dist == wsp->vy_orig) {
				dispind(wn, wsp, ON);
				mp_on();
			}
			else {
				mp_on();
				(void)alterwin(wn, wc);
			}
#endif LANDSCAPE / PORTRAIT
			break;		/* exit forever loop */
		}
	}

	return;
}	/* end of vindscroll() */



/*
 * Check if the mouse pointer points into an icon command area in window 'wn'.
 * 'flag' is I_PRESS or I_RELEASE depending on if the left button has been
 * pressed or released.
 */

struct icondat *
pointicon(wn, flag)
register	wn;		/* window number            */
word		flag;		/* pressed or released flag */
{
	register struct	icondat	*iconp;	/* pointer to icon data          */
	register struct	winint	*wsp;	/* pointer to window status      */
	register	pix_d	left;	/* left side of area to check    */
	register	pix_d	up;	/* upper side of area to check   */
	register	pix_d	x;	/* x coordinate of mouse pointer */
	register	pix_d	y;	/* y coordinate of mouse pointer */
	register		i;	/* loop index                    */

	wsp = wstatp[wn];
	x = pntdata.pd_xmpos;
	y = pntdata.pd_ympos;

	if (x >= wsp->avx_orig && x <= wsp->avx_lrc && y >= wsp->avy_orig && y <= wsp->avy_lrc) {
		iconp = iconsave;
		i = n_icons;
		while (--i >= 0) {
			if (iconp->id_win == wn && (iconp->id_flags & flag)) {
				left = wsp->x_orig + iconp->id_xorig;
				up = wsp->y_orig + iconp->id_yorig;
				if (x >= left && x < left + iconp->id_xsize &&
				    y >= up && y < up + iconp->id_ysize) {
					return(iconp);
				}
			}
			iconp++;	/* next icon */
		}
	}

	return(NULL);		/* no hit */
}	/* end of pointicon() */



/*
 * Draw or remove a double line contour (used when altering the size of a
 * window).
 */

void
dcontour(x, y, xsize, ysize, bdp, flg)
int			x;	/* coordinate of upper left corner of outer */
int			y;	/* lines of the contour                     */
int			xsize;	/* x size of contour minus one              */
int			ysize;	/* y size of contour minus one              */
register struct borddst	*bdp;	/* pointer to border data                   */
int			flg;	/* draw or remove contour flag              */
{
	contour(x, y, xsize, ysize, flg);
#ifdef LANDSCAPE
	contour(x + bdp->bd_down - 1, y + bdp->bd_left - 1, xsize - bdp->bd_up - bdp->bd_down + 2,
		ysize - bdp->bd_left - bdp->bd_right + 2, flg);
#else  PORTRAIT
	contour(x + bdp->bd_left - 1, y + bdp->bd_up - 1, xsize - bdp->bd_left - bdp->bd_right + 2,
		ysize - bdp->bd_up - bdp->bd_down + 2, flg);
#endif LANDSCAPE / PORTRAIT

	return;
}	/* end of dcontour() */



/*
 * Draw or remove a contour of a window by inverting four lines or by drawing
 * black or white lines if it is on the background.
 * 'flg' indicates what to do with the contour (draw or remove).
 * Coordinates are in mover coordinates.
 * Warning: This routine assumes that the contour is inside the screen.
 */

void
contour(x, y, xsize, ysize, flg)
register	x;		/* coordinate of upper left corner */
register	y;		/* of the contour                  */
register	xsize;		/* x size of the contour minus one */
register	ysize;		/* y size of the contour minus one */
register	flg;		/* draw or remove contour flag     */
{
	/*
	 * The upper horizontal line.
	 */
	hcontour(x, y, xsize, flg);

	/*
	 * The lower horizontal line.
	 */
	hcontour(x, y + ysize, xsize, flg);

	/*
	 * The left vertical line.
	 */
	vcontour(x, y, ysize, flg);

	/*
	 * The right vertical line.
	 */
	vcontour(x + xsize, y, ysize, flg);

	return;
}	/* end of contour() */



/*
 * Routine used by contour() to draw horizontal lines.
 */

void
hcontour(x, y, len, flg)
register	x;		/* x coordinate of start of line */
register	y;		/* y coordinate of start of line */
register	len;		/* length minus one of line      */
int		flg;		/* draw or remove flag           */
{
	register struct	wpstruc	 *wrp;	/* pointer to a rectangle        */
	register struct	hwastruc *hwap;	/* pointer to hardware addresses */
	register		 plen;	/* partial length of line        */

	hwap = &hwa;
	do {
		wrp = &wp[0];

		/*
		 * Find the rectangle which the line is a part of.
		 */
		while ((pix_d)y < wrp->yu_p || (pix_d)y > wrp->yl_p ||
		       (pix_d)x < wrp->xl_p || (pix_d)x > wrp->xr_p) {
			wrp++;
		}
		plen = wrp->xr_p - x;
		if (plen > len) {
			plen = len;
		}

		if (wrp < wpp[MAXWINS] || wrp >= wpp[MAXWINS] + nwpart[MAXWINS]) {

			/*
			 * It is not the background, just invert this part of
			 * the line.
			 */
			Waitsp();
			Outb(GSTATFLG | COMPMFL, hwap->mflagport);
			Move(x, y, x, y, plen, 0, UX_UY);
		}
		else {

			/*
			 * It is the background. Draw a line or restore with
			 * the background pattern.
			 */
			wrp = &wpt1r[0];
			wrp->xl_p = x;
			wrp->xr_p = x + plen;
			wrp->yu_p = wrp->yl_p = y;
			sdrawbg(wrp, flg);
		}
		plen++;
		x += plen;
		len -= plen;
	} while (len >= 0) ;

	return;
}	/* end of hcontour() */



/*
 * Routine used by contour() to draw vertical lines.
 */

void
vcontour(x, y, len, flg)
register	x;		/* x coordinate of start of line */
register	y;		/* y coordinate of start of line */
register	len;		/* length minus one of line      */
int		flg;		/* draw or remove flag           */
{
	register struct	wpstruc	 *wrp;	/* pointer to a rectangle        */
	register struct	hwastruc *hwap;	/* pointer to hardware addresses */
	register		 plen;	/* partial length of line        */

	hwap = &hwa;
	do {
		wrp = &wp[0];

		/*
		 * Find the rectangle which the line is a part of.
		 */
		while ((pix_d)x < wrp->xl_p || (pix_d)x > wrp->xr_p ||
		       (pix_d)y < wrp->yu_p || (pix_d)y > wrp->yl_p) {
			wrp++;
		}
		plen = wrp->yl_p - y;
		if (plen > len) {
			plen = len;
		}

		if (wrp < wpp[MAXWINS] || wrp >= wpp[MAXWINS] + nwpart[MAXWINS]) {

			/*
			 * It is not the background, just invert this part of
			 * the line.
			 */
			Waitsp();
			Outb(GSTATFLG | COMPMFL, hwap->mflagport);
			Move(x, y, x, y, 0, plen, UX_UY);
		}
		else {

			/*
			 * It is the background. Draw a line or restore with
			 * the background pattern.
			 */
			wrp = &wpt1r[0];
			wrp->xl_p = wrp->xr_p = x;
			wrp->yu_p = y;
			wrp->yl_p = y + plen;
			sdrawbg(wrp, flg);
		}
		plen++;
		y += plen;
		len -= plen;
	} while (len >= 0) ;

	return;
}	/* end of vcontour() */



/*
 * Remove the mouse pointer and update the mouse position. Note, the mouse
 * pointer is not restored.
 */

void
updmpos()
{
	register		 i;	/* loop index                    */
	register	pix_d	 x;	/* x coordinate of mouse pointer */
	register	pix_d	 y;	/* y coordinate of mouse pointer */
	register	pix_d	 left;	/* left side of icon             */
	register	pix_d	 up;	/* upper side of icon            */
	register	pix_d	 right;	/* right side of icon            */
	register	pix_d	 low;	/* lower side of icon            */
	register struct	winint	 *wsp;	/* pointer to window status      */
	register struct	icondat	 *iconp;/* pointer to icons              */
	register struct	pntstruc *pdp;	/* pointer to mouse status       */

	pdp = &pntdata;
#ifdef LANDSCAPE
	x = pdp->pd_ykpos;
	y = pdp->pd_xkpos;
#else  PORTRAIT
	x = pdp->pd_xkpos;
	y = (SCYSIZE - 1) - pdp->pd_ykpos;
#endif LANDSCAPE / PORTRAIT
	if (x >= SCXSIZE) {
		x = SCXSIZE - 1;
	}
	else if (x < 0) {
		x = 0;
	}
	if (y >= SCYSIZE) {
		y = SCYSIZE - 1;
	}
	else if (y < 0) {
		y = 0;
	}

	/*
	 * Check if we have moved into an area with the I_ENTER flag set or
	 * moved out of an area with the I_LEAVE flag set.
	 */
	iconp = iconsave;
	i = n_icons;
	while (--i >= 0) {
		if (iconp->id_flags & (I_ENTER | I_LEAVE)) {
			wsp = wstatp[iconp->id_win];
			left = wsp->x_orig + iconp->id_xorig;
			up = wsp->y_orig + iconp->id_yorig;
			right = left + iconp->id_xsize;
			low = up + iconp->id_ysize;
			if (((iconp->id_flags & I_ENTER) &&
			     (x >= left && x < right && y >= up && y < low) &&
			     (pdp->pd_xmpos < left || pdp->pd_xmpos >= right ||
			      pdp->pd_ympos < up || pdp->pd_ympos >= low)) ||
			    ((iconp->id_flags & I_LEAVE) &&
			     (x < left || x >= right || y < up || y >= low) &&
			     (pdp->pd_xmpos >= left && pdp->pd_xmpos < right &&
			      pdp->pd_ympos >= up && pdp->pd_ympos < low))) {

				/*
				 * We have moved in or out of an marked area,
				 * send the sequence.
				 */
				mp_off();	/* first remove mouse pointer */
				if (sendicon(iconp) == YES) {
					iconp--;
				}
			}
		}
		iconp++;
	}

	/*
	 * Update the mouse position.
	 */
	pdp->pd_xmpos = x;
	pdp->pd_ympos = y;

	/*
	 * Check if we shall restore an inverted icon and/or we shall invert
	 * a new icon.
	 */
	i = pwinno();

	/*
	 * Remove mouse pointer if not removed before.
	 */
	mp_off();

	left = NO;
	if (i == MAXWINS || x < (wsp = wstatp[i])->avx_orig ||
	    x > wsp->avx_lrc || y < wsp->avy_orig || y > wsp->avy_lrc) {
		left = YES;
	}

	(struct invsave *)iconp = &invdata;
	if (((struct invsave *)iconp)->inv_flg == ON &&
	    (left != NO || (((struct invsave *)iconp)->inv_wn != i ||
	    x < ((struct invsave *)iconp)->inv_left || x > ((struct invsave *)iconp)->inv_right ||
	    y < ((struct invsave *)iconp)->inv_up || y > ((struct invsave *)iconp)->inv_low))) {
		invoff();	/* restore inverted icon */
	}
	if (i != MAXWINS && ((struct invsave *)iconp)->inv_flg == OFF) {
		invon(i);	/* check if a new icon shall be inverted */
	}

	/*
	 * Determine which mouse pointer to use.
	 */
	if (left != NO || ((wsp = wstatp[i])->estatus & ALTMPNT) == 0) {
		pdp->pd_mpp = &mpdat;
	}
	else {
		pdp->pd_mpp = wsp->mppnt;
	}

	/*
	 * Clear flag indicating that it has been computed which windows are
	 * overlapped by the mouse pointer.
	 */
	pdp->pd_flags &= ~PD_OLAP;

	return;
}	/* end of updmpos() */



/*
 * Check if the mouse pointer points to an icon and if so invert the icon
 * (if the I_INVERT flag is set).
 */

void
iconchk()
{
	register	wn;	/* window number */

	if ((wn = pwinno()) != MAXWINS) {
		invon(wn);
	}

	return;
}	/* end of iconchk() */



/*
 * Check if we are pointing into an icon area, and if so invert the area.
 */

void
invon(wn)
register	wn;	/* window which mouse pointer points into */
{
	register	pix_d	x;	/* x coordinate of mouse pointer */
	register	pix_d	y;	/* y coordinate of mouse pointer */
	register		i;	/* loop index                    */
	register struct	winint	*wsp;	/* pointer to window status      */
	register struct	icondat	*iconp;	/* pointer to icons              */
	register struct	invsave	*invp;	/* pointer to inverted icon data */

	x = pntdata.pd_xmpos;
	y = pntdata.pd_ympos;
	wsp = wstatp[wn];
	invp = &invdata;
	invp->inv_flg = OFF;	/* no icon's are supposed to be inverted */

	if (x >= wsp->avx_orig && x <= wsp->avx_lrc && y >= wsp->avy_orig && y <= wsp->avy_lrc) {
		iconp = iconsave;
		i = n_icons;
		while (--i >= 0) {
			if (iconp->id_win == wn && (iconp->id_flags & I_INVERT) &&
			    ((iconp->id_flags & I_LZERO) == 0 || wtol[wn] == 0)) {
				invp->inv_left = wsp->x_orig + iconp->id_xorig;
				invp->inv_up = wsp->y_orig + iconp->id_yorig;
				invp->inv_right = invp->inv_left + iconp->id_xsize - 1;
				invp->inv_low = invp->inv_up + iconp->id_ysize - 1;
				if (x >= invp->inv_left && x <= invp->inv_right &&
				    y >= invp->inv_up && y <= invp->inv_low) {

					/*
					 * We are pointing into an icon which
					 * shall be inverted.
					 */
					invp->inv_wn = wn;
					invicon();	/* invert the area */
					invp->inv_flg = ON;
					break;		/* exit while loop */
				}
			}
			iconp++;	/* next icon */
		}
	}

	return;
}	/* end of invon() */



/*
 * If an icon is inverted, restore to normal.
 */

void
invoff()
{
	if (invdata.inv_flg == ON) {
		invicon();
		invdata.inv_flg = OFF;
	}

	return;
}	/* end of invoff() */



/*
 * Invert the icon indicated by the limits 'inv_left', 'inv_right', 'inv_up',
 * and 'inv_low' in the invdata structure.
 */

void
invicon()
{
	register		 n;	/* number of rectangles          */
	register		 x;	/* x coord. upper left corner    */
	register		 y;	/* y coord. upper left corner    */
	register		 xsize;	/* x size                        */
	register		 ysize;	/* y size                        */
	register	short	 onoff;	/* on/off flag save              */
	register struct	wpstruc	 *wrp;	/* pointer to rectangle group    */
	register struct	hwastruc *hwap;	/* pointer to hardware addresses */
	register struct	invsave	 *invp;	/* pointer to inverted icon data */

	hwap = &hwa;
	invp = &invdata;
	onoff = pntdata.pd_onoff;
	mp_off();
	wrp = wpp[invp->inv_wn];
	n = nwpart[invp->inv_wn];
	while (--n >= 0) {
		/* x = Max(invp->inv_left, wrp->xlv_p); Generates more code! */
		x = invp->inv_left;
		if ((pix_d)x < wrp->xlv_p) {
			x = wrp->xlv_p;
		}
		/* y = Max(invp->inv_up, wrp->yuv_p); */
		y = invp->inv_up;
		if ((pix_d)y < wrp->yuv_p) {
			y = wrp->yuv_p;
		}
		/* xsize = Min(invp->inv_right, wrp->xrv_p) - x; */
		xsize = invp->inv_right;
		if ((pix_d)xsize > wrp->xrv_p) {
			xsize = wrp->xrv_p;
		}
		xsize -= x;
		/* ysize = Min(invp->inv_low, wrp->ylv_p) - y; */
		ysize = invp->inv_low;
		if ((pix_d)ysize > wrp->ylv_p) {
			ysize = wrp->ylv_p;
		}
		ysize -= y;
		if ((wrp->bflags & B_NOTEMPTY) && xsize >= 0 && ysize >= 0) {
			Waitsp();
			Outb(GSTATFLG | COMPMFL, hwap->mflagport);
			Move(x, y, x, y, xsize, ysize, UX_UY);
		}
		wrp++;
	}
	if (onoff == ON) {
		mp_on();
	}

	return;
}	/* end of invicon() */



/*
 * Put an icon sequence in the keyboard buffer for a window.
 * YES is returned if the icon was removed, otherwise NO.
 * WARNING: This function uses structure assignment.
 */

int
sendicon(iconp)
register struct	icondat	*iconp;		/* pointer to icon data */
{
	if (((iconp->id_flags & I_RQST) == 0 || wstatp[iconp->id_win]->nsrque.q_cnt > 0) &&
	    ((iconp->id_flags & I_LZERO) == 0 || wtol[iconp->id_win] == 0)) {
		putonque(iconp->id_win, &iconp->id_cmdseq[0], iconp->id_len, NO);
		if (iconp->id_flags & I_REMOVE) {

			/*
			 * The icon shall be removed.
			 */
			invoff();
			*iconp = *(iconsave + --n_icons);	/**** WARNING ****/
			ckiconcnt();		/* check if we can release any memory */
			return(YES);
		}
	}

	return(NO);
}	/* end of sendicon() */



/*
 * Return the number of the window the mouse pointer is pointing to, or MAXWINS
 * if it is pointing to the background.
 */

int
pwinno()
{
	register	pix_d	x;	/* x position of mouse pointer */
	register	pix_d	y;	/* y position of mouse pointer */
	register		wn;	/* window number               */
	register		i;	/* loop index                  */
	register struct	winint	*wsp;	/* pointer to window status    */

	x = pntdata.pd_xmpos;
	y = pntdata.pd_ympos;
	for (i = 0 ; i < nwin ; i++) {
		wn = ltow[i];
		wsp = wstatp[wn];
		if (x >= wsp->abx_orig && x <= wsp->abx_lrc && y >= wsp->aby_orig && y <= wsp->aby_lrc) {
			return(wn);
		}
	}

	return(MAXWINS);
}	/* end of pwinno() */



/*
 * Wait until a change from the mouse occurs.
 * If 'state' is PRESSED we are supposed to be in a state where a key on the
 * mouse is pressed (or the corresponding keyboard key has been pressed once).
 */

void
waitmous(state)
int	state;		/* mouse button state */
{
	int	moussig();		/* mouse signal routine */

	/*
	 * Wait until a change from the mouse.
	 */
	(void)signal(SIGCHAR, moussig);
	cmpgp->mflag = REPCHANGE;		/* allow signals */
	do {
		(void)pause();
	} while (cmpgp->mflag == REPCHANGE);

	if (cmpgp->mflag >= M_SKEY) {

		/*
		 * It is a substitute key.
		 */
		cnvsubst(state);
	}
	else {
		pntdata.pd_xkpos = cmpgp->mlocx;
		pntdata.pd_ykpos = cmpgp->mlocy;
	}

	return;
}	/* end of waitmous() */



/*
 * Dummy signal routine to make it possible for us to be waken up after a
 * pause() system call when the mouse has changed.
 */

void
moussig()
{
	(void)signal(SIGCHAR, moussig);

	return;
}	/* end of moussig() */
