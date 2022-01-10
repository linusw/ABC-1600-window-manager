/* winhand.c */

/*
 * This file contains low level routines to handle windows for the ABC1600
 * window handler.
 */

/* 1984-06-29, Peter Andersson, Luxor Datorer AB (at DIAB) */

/*
 * Functions in this file:
 *	wsplit()
 *	rmsort()
 *	rsplit()
 *	overlap()
 *	roverlap()
 *	cvisible()
 *	copyrects()
 *	replrects()
 */


#include	"../hsrc/defs.h"
#include	"../hsrc/structs.h"
#include	"../hsrc/eglobals.h"
#include	"../hsrc/graphconst.h"
#include	"../hsrc/macros.h"
#ifdef SHOULD_NOT_HAPPEN
#include	<dnix/fcodes.h>
#endif SHOULD_NOT_HAPPEN

/*
 * Split the visible part of a window into rectangles which are not overlayed
 * by other windows.
 * Also the background can be split by this routine by using the window number
 * MAXWINS.
 */

void
wsplit(wn)
register	wn;			/* window to split */
{
	register		i;	/* loop index                         */
	register		nrect;	/* resulting rectangle count          */
	register		n;	/* counter                            */
	register struct	winint	*wsp;	/* pointer to window status structure */
	register struct	wpstruc	*wrp;	/* pointer to rectangle group         */
	register struct	wpstruc	*wrp1;	/* temp. pointer to rectangle group   */

	/*
	 * Initialize the first element of the rectangle group to be the whole
	 * window to be split, or the whole screen if it is the background.
	 */
	nrect = 1;
	wrp = &ubuf.wpt[0];
	if (wn != MAXWINS) {		/* if not background */
		wsp = wstatp[wn];
		wrp->xl_p = wsp->abx_orig;
		wrp->xr_p = wsp->abx_lrc;
		wrp->yu_p = wsp->aby_orig;
		wrp->yl_p = wsp->aby_lrc;
	}
	else {
		wrp->xl_p = 0;
		wrp->xr_p = SCXSIZE - 1;
		wrp->yu_p = 0;
		wrp->yl_p = SCYSIZE - 1;
	}

	/*
	 * Split the window into smaller rectangles.
	 */
	i = wtol[wn];
	while (--i >= 0) {
		wrp1 = wrp;
		n = nrect;
		while (--n >= 0) {
			nrect = rsplit(wstatp[ltow[i]], wrp, wrp1++, wrp + nrect);
		}

		/*
		 * Concatenate rectangles and remove all rectangles of zero area.
		 */
		nrect = rmsort(wrp, nrect);
	}
	if (wn != MAXWINS) {
		cvisible(wn, wrp, nrect, EXTENDED);	/* compute additional data */
	}
	replrects(wn, nrect);				/* replace old rectangles */

	return;
}	/* end of wsplit() */



/*
 * Try to concatenate rectangles and then remove all rectangles of zero area.
 * The number of rectangles left when the zero area ones have been removed is
 * returned.
 * WARNING: THIS FUNCTION USES STRUCTURE ASSIGNMENT.
 */

/*
 * Macro to check if a rectangle is of non-zero area.
 */

#define	Nonzerorect(r)	((r)->xl_p <= (r)->xr_p && (r)->yu_p <= (r)->yl_p)

int
rmsort(rgp, nrect)
register struct	wpstruc	*rgp;		/* pointer to rectangle group   */
register		nrect;		/* no. of rectangles in group   */
{
	register		i;	/* loop index                   */
	register		j;	/* loop index                   */
	register struct	wpstruc	*wrp1;	/* pointer used for copying     */
	register struct	wpstruc	*wrp2;	/* pointer used for copying     */

	/*
	 * First try to concatenate rectangles.
	 */
	wrp1 = rgp;
	i = nrect;
	while (--i >= 0) {
		if (Nonzerorect(wrp1)) {
			wrp2 = rgp;
			j = nrect;
			while (--j >= 0) {
				if (wrp1->yl_p + 1 == wrp2->yu_p && Nonzerorect(wrp2) &&
				    wrp1->xl_p == wrp2->xl_p && wrp1->xr_p == wrp2->xr_p) {
					wrp1->yl_p = wrp2->yl_p;	/* concatenate */
					wrp2->xr_p = (-1);		/* make other zero area */
				}
				wrp2++;
			}
		}	/* if (Nonzerorect(wrp1)) */
		wrp1++;
	}

	/*
	 * Remove all windows of zero area.
	 */
	i = 0;
	while (i < nrect) {
		if (Nonzerorect(rgp)) {

			/*
			 * Non-zero area rectangle.
			 */
			rgp++;
			i++;
		}
		else {

			/*
			 * Zero area rectangle, remove it.
			 */
			nrect--;
			*rgp = *--wrp1;		/**** WARNING ****/
		}
	}

	return(nrect);
}	/* end of rmsort() */

#undef	Nonzerorect



/*
 * This function splits a rectangle pointed to by rsp into smaller
 * rectangles if it is overlayed by the window pointed to by wsp.
 * The new number of rectangles in the rectangle group is returned.
 * A rectangle is a whole window or a piece of a window obtained by an earlier
 * call to this function.
 *
 * In the function the x,y coordinates for the corners of the rectangle
 * and the window are as follows:
 *
 *       lr,ur              rr,ur        lw,uw              rw,uw
 *         ####################            ####################
 *         #                  #            #                  #
 *         #                  #            #                  #
 *         #    RECTANGLE     #            #     WINDOW       #
 *         #                  #            #                  #
 *         #                  #            #                  #
 *         ####################            ####################
 *       lr,dr              rr,dr        lw,dw              rw,dw
 *
 *
 * Note that it is possible that some of the new rectangles obtained will
 * have an empty area. These empty areas can be removed when the decomposition
 * of a window into rectangles is complete.
 */

/*
 * Some constants.
 */
#define		UP_LEFT		1
#define		UP_RIGHT	2
#define		LOW_LEFT	4
#define		LOW_RIGHT	8


int
rsplit(wsp, rgp, rsp, rfp)
register struct	winint	*wsp;	/* pointer to window status structure          */
struct 		wpstruc	*rgp;	/* pointer to base of rectangle group          */
register struct wpstruc	*rsp;	/* pointer to rectangle to split               */
register struct	wpstruc	*rfp;	/* pointer to first free area after rectangles */
{
	register pix_d	lw;	/* coordinate of left side of window            */
	register pix_d	rw;	/* coordinate of right side of window           */
	register pix_d	uw;	/* coordinate of upper side of window           */
	register pix_d	dw;	/* coordinate of lower (down) side of window    */
	register pix_d	lr;	/* coordinate of left side of rectangle         */
	register pix_d	rr;	/* coordinate of right side of rectangle        */
	register pix_d	ur;	/* coordinate of upper side of rectangle        */
	register pix_d	dr;	/* coordinate of lower (down) side of rectangle */
	int		flags;	/* corners inside flags                         */


	/*
	 * Load the coordinates of the sides of the rectangle.
	 */
	lr = rsp->xl_p;
	rr = rsp->xr_p;
	ur = rsp->yu_p;
	dr = rsp->yl_p;

	/*
	 * If it is a zero area rectangle the following code may not work, so
	 * don't execute it in that case.
	 */
	if (lr <= rr && ur <= dr) {

		/*
		 * Load the coordinates of the sides of the window (including the border).
		 */
		lw = wsp->abx_orig;
		rw = wsp->abx_lrc;
		uw = wsp->aby_orig;
		dw = wsp->aby_lrc;

		/*
		 * Check which of the corners of the window are inside the rectangle.
		 *
		 * Upper left or right corner of the window inside the rectangle?
		 */
		flags = 0;
		if (uw >= ur && uw <= dr) {
			if (lw >= lr && lw <= rr) {
				flags = UP_LEFT;
			}
			if (rw >= lr && rw <= rr) {
				flags |= UP_RIGHT;
			}
		}

		/*
		 * Lower left or right corner of the window inside the rectangle?
		 */
		if (dw >= ur && dw <= dr) {
			if (lw >= lr && lw <= rr) {
				flags |= LOW_LEFT;
			}
			if (rw >= lr && rw <= rr) {
				flags |= LOW_RIGHT;
			}
		}

		/*
		 * Now the behaviour depends on how many corners there were
		 * inside the rectangle.
		 */
		switch (flags) {
		case 0:				/* no corners inside */

			/*
			 * In this case we have to obtain more information.
			 * To do so we have to count how many of the corners of the
			 * rectangle that are inside the window.
			 * Note that flags is 0 here.
			 *
			 * Upper left or right corner of the rectangle inside the window?
			 */
			if (ur >= uw && ur <= dw) {
				if (lr >= lw && lr <= rw) {
					flags = UP_LEFT;
				}
				if (rr >= lw && rr <= rw) {
					flags |= UP_RIGHT;
				}
			}

			/*
			 * Lower left or right corner of the rectangle inside the window?
			 */
			if (dr >= uw && dr <= dw) {
				if (lr >= lw && lr <= rw) {
					flags |= LOW_LEFT;
				}
				if (rr >= lw && rr <= rw) {
					flags |= LOW_RIGHT;
				}
			}

			/*
			 * Now check which case it is.
			 */
			switch (flags) {
			case 0:				/* no corners inside */

				/*
				 * If it is 0 two possibilities exist:
				 * 1) There is no overlay or
				 * 2) The rectangle and the window are placed in a
				 *    cross-like formation.
				 * To check if it is a cross-like formation, check if
				 * any of the horizontal sides of the window intersects
				 * a vertical side of the rectangle or vice versa.
				 */
				if (lw <= lr && rw >= lr && ur <= uw && dr >= uw) {

					/*
					 * The vertical sides of the rectangle are intersected.
					 */
					rfp->xl_p = lr;
					rfp->xr_p = rr;
					rfp->yu_p = ++dw;
					(rfp++)->yl_p = dr;

					rsp->yl_p = --uw;
				}
				else if (uw <= ur && dw >= ur && lr <= lw && rr >= lw) {

					/*
					 * The horizontal sides of the rectangle are intersected.
					 */
					rfp->xl_p = ++rw;
					rfp->xr_p = rr;
					rfp->yu_p = ur;
					(rfp++)->yl_p = dr;

					rsp->xr_p = --lw;
				}

				break;

#ifdef SHOULD_NOT_HAPPEN
			case UP_LEFT:			/* 1 corner inside */
			case UP_RIGHT:
			case LOW_LEFT:
			case LOW_RIGHT:

				/*
				 * One corner inside should not occur because in this
				 * case there is also one corner of the window inside
				 * the rectangle.
				 */
				(void)dnix(F_LOGC, "rsplit: Error, impossible case occured!\n");
				break;
#endif SHOULD_NOT_HAPPEN

			/*
			 * If two corners inside, one of the sides of the rectangle
			 * is overlayed. Remove that side.
			 */
			case UP_RIGHT | LOW_RIGHT:
				rsp->xr_p = --lw;	/* remove right side */
				break;

			case UP_LEFT | LOW_LEFT:
				rsp->xl_p = ++rw;	/* remove left side */
				break;

			case LOW_LEFT | LOW_RIGHT:
				rsp->yl_p = --uw;	/* remove lower side */
				break;

			case UP_LEFT | UP_RIGHT:
				rsp->yu_p = ++dw;	/* remove upper side */
				break;

			/*
			 * If 4 corners inside the window, the rectangle is completely
			 * overlayed by the window. Remove it.
			 */
			case UP_LEFT | UP_RIGHT | LOW_LEFT | LOW_RIGHT:
				rsp->xr_p = -1;		/* this will make the
							   width of the window
							   <0, and we can there-
							   fore remove it later */
				break;
			}	/* end of inner switch body */

			break;

		/*
		 * If 1 corner inside rectangle, it shall be split into two new
		 * rectangles.
		 */
		case UP_LEFT:
			rsp->yu_p = uw;
			rsp->xr_p = --lw;

			rfp->xl_p = lr;
			rfp->xr_p = rr;
			rfp->yu_p = ur;
			(rfp++)->yl_p = --uw;
			break;

		case UP_RIGHT:
			rsp->yu_p = uw;
			rsp->xl_p = ++rw;

			rfp->xl_p = lr;
			rfp->xr_p = rr;
			rfp->yu_p = ur;
			(rfp++)->yl_p = --uw;
			break;

		case LOW_LEFT:
			rsp->yl_p = dw;
			rsp->xr_p = --lw;

			rfp->xl_p = lr;
			rfp->xr_p = rr;
			rfp->yu_p = ++dw;
			(rfp++)->yl_p = dr;
			break;

		case LOW_RIGHT:
			rsp->yl_p = dw;
			rsp->xl_p = ++rw;

			rfp->xl_p = lr;
			rfp->xr_p = rr;
			rfp->yu_p = ++dw;
			(rfp++)->yl_p = dr;
			break;

		/*
		 * If 2 corners inside the rectangle, the rectangle shall be split into
		 * 3 rectangles.
		 */
		case UP_RIGHT | LOW_RIGHT:
			rfp->xl_p = ++rw;
			rfp->xr_p = rr;
			rfp->yu_p = uw;
			(rfp++)->yl_p = dw;

			rfp->xl_p = lr;
			rfp->xr_p = rr;
			rfp->yu_p = ++dw;
			(rfp++)->yl_p = dr;

			rsp->yl_p = --uw;
			break;

		case UP_LEFT | LOW_LEFT:
			rfp->xl_p = lr;
			rfp->xr_p = --lw;
			rfp->yu_p = uw;
			(rfp++)->yl_p = dw;

			rfp->xl_p = lr;
			rfp->xr_p = rr;
			rfp->yu_p = ++dw;
			(rfp++)->yl_p = dr;

			rsp->yl_p = --uw;
			break;

		case LOW_LEFT | LOW_RIGHT:
			rfp->xl_p = ++rw;
			rfp->xr_p = rr;
			rfp->yu_p = ur;
			(rfp++)->yl_p = dw;

			rsp->xr_p = --lw;
			rsp->yl_p = dw;

			rfp->xl_p = lr;
			rfp->xr_p = rr;
			rfp->yu_p = ++dw;
			(rfp++)->yl_p = dr;
			break;

		case UP_LEFT | UP_RIGHT:
			rfp->xl_p = lr;
			rfp->xr_p = --lw;
			rfp->yu_p = uw;
			(rfp++)->yl_p = dr;

			rfp->xl_p = ++rw;
			rfp->xr_p = rr;
			rfp->yu_p = uw;
			(rfp++)->yl_p = dr;

			rsp->yl_p = --uw;
			break;

		/*
		 * If 4 corners inside the rectangle, the whole window is inside the
		 *  rectangle. The rectangle shall therefore be split into 4 rectangles.
		 */
		case UP_LEFT | UP_RIGHT | LOW_LEFT | LOW_RIGHT:
			rfp->xl_p = lr;
			rfp->xr_p = --lw;
			rfp->yu_p = uw;
			(rfp++)->yl_p = dw;

			rfp->xl_p = ++rw;
			rfp->xr_p = rr;
			rfp->yu_p = uw;
			(rfp++)->yl_p = dw;

			rfp->xl_p = lr;
			rfp->xr_p = rr;
			rfp->yu_p = ++dw;
			(rfp++)->yl_p = dr;

			rsp->yl_p = --uw;

			break;

		}		/* end of outer switch body */

	}	/* if (lr <= rr && ur <= dr) */

	return(rfp - rgp);
}	/* end of rsplit() */

#undef	UP_LEFT
#undef	UP_RIGHT
#undef	LOW_LEFT
#undef	LOW_RIGHT



/*
 * Check if the window pointed to by wsp1 overlaps the window pointed to by
 * wsp2 or vice versa.
 * One is returned if overlapped, else zero.
 */

int
overlap(wsp1, wsp2)
register struct	winint	*wsp1;
register struct	winint	*wsp2;
{
	return(wsp1->abx_lrc >= wsp2->abx_orig && wsp1->abx_orig <= wsp2->abx_lrc &&
	       wsp1->aby_lrc >= wsp2->aby_orig && wsp1->aby_orig <= wsp2->aby_lrc);
}	/* end of overlap() */



/*
 * Check if the window pointed to by wsp overlaps the rectangle pointed to by
 * wrp or vice versa.
 * One is returned if overlap, else zero.
 */

int
roverlap(wrp, wsp)
register struct	wpstruc	*wrp;
register struct	winint	*wsp;
{
	return(wrp->xr_p >= wsp->abx_orig && wrp->xl_p <= wsp->abx_lrc &&
	       wrp->yl_p >= wsp->aby_orig && wrp->yu_p <= wsp->aby_lrc);
}	/* end of roverlap() */



/*
 * Compute the area of the rectangles, excluding the border, and set a flag to
 * indicate if anything is left of the rectangle when the border is removed.
 * Also, if 'flag' is EXTENDED, compute (in character coordinates) the largest
 * area of the window that may be visible.
 */

void
cvisible(wn, wrp, nrect, flag)
int			wn;		/* window number                      */
register struct	wpstruc	*wrp;		/* pointer to rectangle group         */
int			nrect;		/* # of rectangles in rectangle group */
int			flag;		/* flag indicating the action         */
{
	register struct winint	*wsp;		/* pointer to window status structure         */
	register	pix_d	tmp;		/* temporary                                  */
	register	pix_d	side;		/* side of the rectangle not including border */
	register	pix_d	leftmost;	/* leftmost pixel pos. that may be visible    */
	register	pix_d	rightmost;	/* rightmost pixel pos. that may be visible   */
	register	pix_d	upmost;		/* uppermost pixel pos. that may be visible   */
	register	pix_d	lowmost;	/* lowermost pixel pos. that may be visible   */

	wsp = wstatp[wn];

	leftmost = upmost = BIG;
	rightmost = lowmost = SMALL;

	while (--nrect >= 0) {
		wrp->bflags = 0;
		side = wrp->xl_p;
		if (side < wsp->avx_orig) {
			side = wsp->avx_orig;
			wrp->bflags = B_LEFT;
		}
		wrp->xlv_p = side;		/* left side excluding border */
		if (side < leftmost) {
			leftmost = side;
		}

		side = wrp->xr_p;
		if (side > wsp->avx_lrc) {
			side = wsp->avx_lrc;
			wrp->bflags |= B_RIGHT;
		}
		wrp->xrv_p = side;		/* right side excluding border */
		if (side > rightmost) {
			rightmost = side;
		}

		side = wrp->yu_p;
		if (side < wsp->avy_orig) {
			side = wsp->avy_orig;
			wrp->bflags |= B_UP;
		}
		wrp->yuv_p = side;		/* upper side excluding border */
		if (side < upmost) {
			upmost = side;
		}

		side = wrp->yl_p;
		if (side > wsp->avy_lrc) {
			side = wsp->avy_lrc;
			wrp->bflags |= B_LOW;
		}
		wrp->ylv_p = side;		/* lower side excluding border */
		if (side > lowmost) {
			lowmost = side;
		}

		if (wrp->xlv_p <= wrp->xrv_p && wrp->yuv_p <= side) {
			wrp->bflags |= B_NOTEMPTY;	/* set flag indicating
							   non-zero area      */
		}
		wrp++;				/* next rectangle */
	}

	if (flag != NORMAL) {

		/*
		 * Set leftmost, rightmost, uppermost, and lowermost characters
		 * that may be visible in the window.
		 */
#ifdef LANDSCAPE
		side = wsp->y_orig + wsp->x_coffs;
		tmp = wsp->fdata.fi_xsize;
		wsp->cl_vis = (upmost - side) / tmp + 1;
		wsp->cr_vis = (lowmost - side) / tmp + 1;

		side = wsp->x_orig + wsp->x_size - 1 - wsp->y_coffs;
		tmp = wsp->fdata.fi_ysize;
		wsp->cu_vis = (side - rightmost) / tmp + 1;
		wsp->cd_vis = (side - leftmost) / tmp + 1;
#else  PORTRAIT
		side = wsp->x_orig + wsp->x_coffs;
		tmp = wsp->fdata.fi_xsize;
		wsp->cl_vis = (leftmost - side) / tmp + 1;
		wsp->cr_vis = (rightmost - side) / tmp + 1;

		side = wsp->y_orig + wsp->y_coffs;
		tmp = wsp->fdata.fi_ysize;
		wsp->cu_vis = (upmost - side) / tmp + 1;
		wsp->cd_vis = (lowmost - side) / tmp + 1;
#endif LANDSCAPE / PORTRAIT
	}

	return;
}	/* end of cvisible() */



/*
 * This function copies the rectangles for window wn to the temporary
 * area ubuf.wpt[].
 * WARNING: THIS FUNCTION USES STRUCTURE ASSIGNMENT.
 */

void
copyrects(wn)
int	wn;
{
	register struct	wpstruc	*wrp1;
	register struct	wpstruc	*wrp2;
	register		n;		/* no. of rectangles */

	wrp1 = wpp[wn];
	wrp2 = &ubuf.wpt[0];
	n = nwpart[wn];
	while (--n >= 0) {
		*wrp2++ = *wrp1++;	/**** WARNING ****/
	}

	return;
}	/* end of copyrects() */



/*
 * This function replaces the rectangles for a window with the rectangles in
 * the temporary area ubuf.wpt[].
 * WARNING: THIS ROUTINE USES STRUCTURE ASSIGNMENT.
 */

void
replrects(wn, cnt)
register	wn;	/* window to replace rectangles for */
register	cnt;	/* new number of rectangles         */
{
	register struct	wpstruc	*wrp;
	register struct	wpstruc	*wrp1;
	register struct	wpstruc	*wrp2;
	register struct	wpstruc	**wrpp;	/* pointer to the wpp[] array    */
	register		i;	/* loop index                    */
	register		n;	/* previous number of rectangles */
	register		nm;	/* number of rectangles to move  */
	register		diff;	/* rectangle count difference    */

	n = nwpart[wn];
	wrp = wpp[wn];
	nm = &wp[0] + rtotal - wrp - n;		/* # of rectangles to move */
	diff = cnt - n;

	/*
	 * Update the pointers in the wpp[] array.
	 */
	wrpp = &wpp[0];
	for (i = 0 ; i <= MAXWINS ; i++) {
		if (*wrpp > wrp || (i != wn && *wrpp == wrp && nwpart[i] != 0)) {
			*wrpp += diff;
		}
		wrpp++;
	}

	if (cnt > n) {
		wrp1 = &wp[0] + rtotal;
		wrp2 = wrp1 + diff;
		while (--nm >= 0) {
			*--wrp2 = *--wrp1;	/**** WARNING ****/
		}
	}
	else if (cnt < n) {
		wrp1 = wrp + (short)n;		/* use muls assembler */
		wrp2 = wrp + (short)cnt;	/* instruction */
		while (--nm >= 0) {
			*wrp2++ = *wrp1++;	/**** WARNING ****/
		}
	}
	nwpart[wn] = cnt;	/* new rectangle count */
	rtotal += diff;		/* new total rectangle count */
	wrp1 = &ubuf.wpt[0];
	while (--cnt >= 0) {
		*wrp++ = *wrp1++;		/**** WARNING ****/
	}

	return;
}	/* end of replrects() */
