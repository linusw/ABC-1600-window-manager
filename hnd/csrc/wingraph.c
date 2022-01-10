/* wingraph.c */

/*
 * This file contains special purpose graphic routines for drawing and updating
 * windows for the ABC1600 window handler.
 */

/* 1984-07-11, Peter Andersson, Luxor Datorer AB (at DIAB) */

/*
 * Functions in this file:
 *	clrrect()
 *	sdrawbg()
 *	wipe()
 *	dispbox()
 *	dispind()
 *	hindloc()
 *	vindloc()
 *	dsphind()
 *	dspvind()
 *	d1box()
 */


#include	"../hsrc/defs.h"
#include	"../hsrc/structs.h"
#include	"../hsrc/eglobals.h"
#include	"../hsrc/graphconst.h"
#include	"../hsrc/macros.h"



/*
 * Clear rectangle pointed to by wrp in window wn by filling the rectangle with
 * the background pattern for the window and drawing the border (if visible).
 * Also a rectangle in the background can be filled by using the window number
 * MAXWINS.
 */

void
clrrect(wn, wrp)
int			wn;		/* window number        */
register struct	wpstruc	*wrp;		/* pointer to rectangle */
{
	register struct	winint	 *wsp;		/* pointer to window status structure */
	register struct	hwastruc *hwap;		/* pointer to hardware addresses      */
	register	word	 bd;		/* border flags                       */
	register		 s;		/* help variable                      */
	register		 l;		/* help variable                      */
	register		 tmp;		/* temporary                          */

	if (wn == MAXWINS) {
		sdrawbg(wrp, PATTERN);		/* it is background */
	}
	else {
		hwap = &hwa;
		wsp = wstatp[wn];

		/*
		 * Set to hold the x and y mover counters.
		 */
		Waitsp();
		Outb(wsp->statflg | HOLDYFL | HOLDXFL, hwap->mflagport);

		Clearrect(wrp->xl_p, wrp->yu_p, wrp->xr_p - wrp->xl_p, wrp->yl_p - wrp->yu_p);

		/*
		 * Draw the outer line in the border.
		 */
		if (wsp->border != NOBORDER) {
			bd = wsp->bordflags;
			s = wrp->yu_p;
			l = wrp->yl_p;
			if ((pix_d)s == wsp->aby_orig) {
				s++;
			}
			if ((pix_d)l == wsp->aby_lrc) {
				l--;
			}
			l -= s;
			if (l >= 0) {
				tmp = wsp->abx_orig + 1;
				if ((pix_d)tmp >= wrp->xl_p && (pix_d)tmp <= wrp->xr_p) {
					Waitsp();
					Vline(tmp, s, l);
				}
				tmp = wsp->abx_lrc - 1;
				if ((pix_d)tmp >= wrp->xl_p && (pix_d)tmp <= wrp->xr_p) {
					Waitsp();
					Vline(tmp, s, l);
				}
			}
			s = wrp->xl_p;
			l = wrp->xr_p;
			if ((pix_d)s == wsp->abx_orig) {
				s++;
			}
			if ((pix_d)l == wsp->abx_lrc) {
				l--;
			}
			l -= s;
			if (l >= 0) {
				tmp = wsp->aby_orig + 1;
				if ((pix_d)tmp >= wrp->yu_p && (pix_d)tmp <= wrp->yl_p) {
					Waitsp();
					Hline(s, tmp, l);
				}
				tmp = wsp->aby_lrc - 1;
				if ((pix_d)tmp >= wrp->yu_p && (pix_d)tmp <= wrp->yl_p) {
					Waitsp();
					Hline(s, tmp, l);
				}
			}

			/*
			 * Draw possible inner lines in the border.
			 */
			s = wsp->aby_orig + 1;
#ifdef LANDSCAPE
			if (bd & BD_LEFT) {
#else  PORTRAIT
			if (bd & BD_UP) {
#endif LANDSCAPE / PORTRAIT
				s += (DLBWIDTH - 4);
			}
			if (wrp->yu_p >= (pix_d)s) {
				s = wrp->yu_p;
			}
			l = wrp->yl_p - s;
			tmp = wrp->yl_p - wsp->aby_lrc + 1;
#ifdef LANDSCAPE
			if (bd & BD_RIGHT) {
#else  PORTRAIT
			if (bd & BD_DOWN) {
#endif LANDSCAPE / PORTRAIT
				tmp += (DLBWIDTH - 4);
			}
			if (tmp > 0) {
				l -= tmp;
			}
			if (l >= 0) {
#ifdef LANDSCAPE
				if (bd & BD_DOWN) {
#else  PORTRAIT
				if (bd & BD_LEFT) {
#endif LANDSCAPE / PORTRAIT
					tmp = wsp->abx_orig + (DLBWIDTH - 3);
					if ((pix_d)tmp >= wrp->xl_p && (pix_d)tmp <= wrp->xr_p) {
						Waitsp();
						Vline(tmp, s, l);
					}
				}
#ifdef LANDSCAPE
				if (bd & BD_UP) {
#else  PORTRAIT
				if (bd & BD_RIGHT) {
#endif LANDSCAPE / PORTRAIT
					tmp = wsp->abx_lrc - (DLBWIDTH - 3);
					if ((pix_d)tmp >= wrp->xl_p && (pix_d)tmp <= wrp->xr_p) {
						Waitsp();
						Vline(tmp, s, l);
					}
				}
			}

			s = wsp->abx_orig + 1;
#ifdef LANDSCAPE
			if (bd & BD_DOWN) {
#else  PORTRAIT
			if (bd & BD_LEFT) {
#endif LANDSCAPE / PORTRAIT
				s += (DLBWIDTH - 4);
			}
			if (wrp->xl_p >= (pix_d)s) {
				s = wrp->xl_p;
			}
			l = wrp->xr_p - s;
			tmp = wrp->xr_p - wsp->abx_lrc + 1;
#ifdef LANDSCAPE
			if (bd & BD_UP) {
#else  PORTRAIT
			if (bd & BD_RIGHT) {
#endif LANDSCAPE / PORTRAIT
				tmp += (DLBWIDTH - 4);
			}
			if (tmp > 0) {
				l -= tmp;
			}
			if (l >= 0) {
#ifdef LANDSCAPE
				if (bd & BD_LEFT) {
#else  PORTRAIT
				if (bd & BD_UP) {
#endif LANDSCAPE / PORTRAIT
					tmp = wsp->aby_orig + (DLBWIDTH - 3);
					if ((pix_d)tmp >= wrp->yu_p && (pix_d)tmp <= wrp->yl_p) {
						Waitsp();
						Hline(s, tmp, l);
					}
				}
#ifdef LANDSCAPE
				if (bd & BD_RIGHT) {
#else  PORTRAIT
				if (bd & BD_DOWN) {
#endif LANDSCAPE / PORTRAIT
					tmp = wsp->aby_lrc - (DLBWIDTH - 3);
					if ((pix_d)tmp >= wrp->yu_p && (pix_d)tmp <= wrp->yl_p) {
						Waitsp();
						Hline(s, tmp, l);
					}
				}
			}
		}
	}

	return;
}	/* end of clrrect() */



/*
 * If bg == PATTERN, this function rewrites a rectangle in the background by
 * filling the rectangle with the pattern for the background.
 * Otherwise the value of 'bg' determines if the rectangle shall be BLACK or
 * WHITE. This is used to draw window contours in the background.
 */

void
sdrawbg(wrp, bg)
register struct	wpstruc	*wrp;		/* pointer to rectangle */
int			bg;		/* background pattern   */
{
	register	dword	 from;	/* where to move from            */
	register		 n;	/* counter                       */
	register		 xsz;	/* size in x                     */
	register		 ysz;	/* size in y                     */
	register	dword	 size;	/* size in mover coord.          */
	register struct	hwastruc *hwap;	/* pointer to hardware addresses */

	hwap = &hwa;
	Waitsp();
	if (bg != PATTERN) {

		/*
		 * Set to hold the x and y from mover counters.
		 */
		Outb(GSTATFLG | HOLDYFL | HOLDXFL, hwap->mflagport);

		if (bg == BLACK) {
			Outw(0, hwap->grphaddr);	/* black rectangle */
		}
		else {
			Outw(~0, hwap->grphaddr);	/* white rectangle */
		}
		Grphrect(wrp->xl_p, wrp->yu_p, wrp->xr_p - wrp->xl_p, wrp->yl_p - wrp->yu_p);
	}
	else {
		from = ((BGPXCOORD + (wrp->xl_p & 0x0f)) << 16) | (BGPYCOORD + (wrp->yu_p & 0x0f));
		xsz = wrp->xr_p - wrp->xl_p;
		ysz = wrp->yl_p - wrp->yu_p;
		if (xsz >= BGPXSIZE / 2) {
			size = (((BGPXSIZE / 2 - 1) | UX_UY) << 16) | (BGPYSIZE / 2 - 1);
		}
		else {
			size = ((xsz | UX_UY) << 16) | (BGPYSIZE / 2 - 1);
		}
		n = (ysz + 1) / (BGPYSIZE / 2);

		/*
		 * Use the behaviour of the mover counters in landscape mode.
		 */
		Outb(GSTATFLG | LAPOFL, hwap->mflagport);
		Outl(size, hwap->x_sizeport);
		Outl((wrp->xl_p << 16) | wrp->yu_p, hwap->x_toport);
		while (--n >= 0) {
			Outl(from, hwap->x_fromport);
			Waitsp();
		}
		if ((n = (ysz + 1) % (BGPYSIZE / 2)) != 0) {
			Outl((size & (0xffff << 16)) | --n, hwap->x_sizeport);
			Outl(from, hwap->x_fromport);
		}
		if ((xsz -= BGPXSIZE / 2) >= 0) {		/* if anything left to fill */
			Waitsp();
			Move(wrp->xl_p, wrp->yu_p, wrp->xl_p + BGPXSIZE / 2, wrp->yu_p, xsz, ysz, UX_UY);
		}
	}

	return;
}	/* end of sdrawbg() */



/*
 * This function clears the part of the rectangle indicated by the arguments.
 * Only those areas which are visible in window wn are cleared (the border is
 * not cleared).
 * The argument must be in the mover coordinate system.
 * Note: To clear the whole visible part of a window, it is ok to specify the
 * whole screen.
 */

void
wipe(wn, left, right, up, down)
int		wn;		/* window to clear in                       */
register	left;		/* left abs. pixel coord. of rect. to clear */
register	right;		/* right coordinate                         */
register	up;		/* upper coordinate                         */
register	down;		/* lower coordinate                         */
{
	register struct	wpstruc	 *wrp;	/* pointer to rectangle group    */
	register struct	hwastruc *hwap;	/* pointer to hardware addresses */
	register		 n;	/* # of rectangles               */
	register		 tmp;	/* temporary                     */

	if (left <= right && up <= down) {
		hwap = &hwa;
		tmp = wn;
		wrp = wpp[tmp];
		n = nwpart[tmp];

		/*
		 * Set to hold the mover from counters.
		 */
		Waitsp();
		Outb(wstatp[tmp]->statflg | HOLDYFL | HOLDXFL, hwap->mflagport);

		while (--n >= 0) {
			if ((wrp->bflags & B_NOTEMPTY) && (pix_d)left <= wrp->xrv_p &&
			    (pix_d)right >= wrp->xlv_p && (pix_d)up <= wrp->ylv_p &&
			    (pix_d)down >= wrp->yuv_p) {
				wn = Max(left, wrp->xlv_p);
				tmp = Max(up, wrp->yuv_p);
				Waitsp();
				Clearrect(wn, tmp, Min(right, wrp->xrv_p) - wn, Min(down, wrp->ylv_p) - tmp);
			}	/* if (wrp->bflags & .... ) */

			wrp++;		/* next rectangle */
		}	/* while (--n >= 0) */
	}

	return;
}	/* end of wipe() */



/*
 * Insert the different boxes in the border if it is an appropriate
 * border and the corresponding flags are set.
 */

void
dispbox(wn)
register	wn;		/* window number */
{
	register struct	winint	*wsp;	/* pointer to window status      */
	register struct	boxactd	*bxp;	/* pointer to box data           */
	register	word	*sp;	/* used to display user boxes    */
	register	word	*dp;	/* used to display user boxes    */
	register		left;	/* abs. pos. of left part of box */
	register		up;	/* upper part                    */
	register		n;	/* counter                       */

	wsp = wstatp[wn];
	n = BOXCOUNT;
	bxp = &boxdata[0];
	Waits();
	Outb(wsp->statflg, hwa.mflagport);

	do {
		if (wsp->bpresent & bxp->ba_flags) {

			/*
			 * This box shall be displayed.
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
			d1box(wn, left, bxp->ba_xsiz, up, bxp->ba_ysiz, bxp->ba_xsrc, bxp->ba_ysrc);
		}
		bxp++;		/* next box */
	} while (--n != 0);

	/*
	 * Display the user defined boxes.
	 */
	wsp->ubxcrrnt = 0;
	if (wsp->bordflags & BD_LEFT) {
		(struct uboxst *)bxp = wsp->uboxp;
#ifdef LANDSCAPE
		up = wsp->abx_lrc - DLBWIDTH - (UBOXSIZE - 1);
		n = (wsp->bx_size - 2 * DLBWIDTH) / UBOXTSIZ;
#else  PORTRAIT
		up = wsp->aby_orig + DLBWIDTH;
		n = (wsp->by_size - 2 * DLBWIDTH) / UBOXTSIZ;
#endif LANDSCAPE / PORTRAIT
		if (n > wsp->uboxcnt) {
			n = wsp->uboxcnt;
		}
		wsp->ubxcrrnt = n;

		while (--n >= 0) {

			/*
			 * Move the bitpattern for the box to a set up area.
			 */
			dp = (word *)(hwa.pictmemst + (SETUPYCOORD * PICTSIZE + SETUPXCOORD) / 8);
			sp = &((struct uboxst *)bxp)->ub_bmap[0];
			left = UBOXSIZE;
			Waits();
			do {
				Outw(*sp++, dp);
				dp += PICTSIZE / (8 * sizeof(word));
			} while (--left != 0);

			/*
			 * Move the box out in the border.
			 */
#ifdef LANDSCAPE
			d1box(wn, up, UBOXSIZE - 1, wsp->aby_orig + BOXOFFS,
			      UBOXSIZE - 1, SETUPXCOORD, SETUPYCOORD);
			up -= UBOXTSIZ;
#else  PORTRAIT
			d1box(wn, wsp->abx_orig + BOXOFFS, UBOXSIZE - 1, up,
			      UBOXSIZE - 1, SETUPXCOORD, SETUPYCOORD);
			up += UBOXTSIZ;
#endif LANDSCAPE / PORTRAIT

			/*
			 * Next box.
			 */
			((struct uboxst *)bxp)++;
		}
	}

	/*
	 * Display the visible part indicators.
	 */
	dispind(wn, wsp, ON);

	return;
}	/* end of dispbox() */



/*
 * Display or remove the visible part indicators.
 * If 'flag' is OFF, remove them else display them.
 */

void
dispind(wn, wsp, flag)
int			wn;	/* window to display indicators for  */
register struct	winint	*wsp;	/* pointer to window status          */
int			flag;	/* flag indicating remove or display */
{

	/*
	 * Start with the horizontal visible indicator.
	 */
	if (wsp->bpresent & BXI_HIND) {

		/*
		 * Enough space to show the indicator.
		 */
		dsphind(wn, hindloc(wsp), flag);
	}

	/*
	 * The vertical visible indicator.
	 */
	if (wsp->bpresent & BXI_VIND) {

		/*
		 * Enough space to show the indicator.
		 */
		dspvind(wn, vindloc(wsp), flag);
	}

	return;
}	/* end of dispind() */



/*
 * Compute the left side of the area occupied by the horizontal visible
 * indicator for the window whose status is pointed to by 'wsp'.
 */

int
hindloc(wsp)
register struct	winint	*wsp;		/* pointer to window status */
{
	register	size;		/* horizontal size of window */
	register	left;		/* left side of the indicator */

#ifdef LANDSCAPE
	size = wsp->by_size - (2 * (DLBWIDTH + LBOXSIZE) + WINDSIZE);
	left = Muls(size, wsp->vy_orig + Div2(wsp->vy_size));
	left /= wsp->y_size;
	if (left > size) {
		left = size;
	}

	return(left + wsp->aby_orig + DLBWIDTH + LBOXSIZE);
#else  PORTRAIT
	size = wsp->bx_size - (2 * (DLBWIDTH + LBOXSIZE) + WINDSIZE);
	left = Muls(size, wsp->vx_orig + Div2(wsp->vx_size));
	left /= wsp->x_size;
	if (left > size) {
		left = size;
	}

	return(left + wsp->abx_orig + DLBWIDTH + LBOXSIZE);
#endif LANDSCAPE / PORTRAIT
}	/* end of hindloc() */



/*
 * Compute the upper side of the area occupied by the vertical visible
 * indicator for the window whose status is pointed to by 'wsp'.
 * In landscape mode is the lower side computed instead.
 */

int
vindloc(wsp)
register struct	winint	*wsp;		/* pointer to window status */
{
	register	size;		/* vertical size of window */
	register	up;		/* upper side of indicator */

#ifdef LANDSCAPE
	size = wsp->bx_size - (2 * (DLBWIDTH + LBOXSIZE) + WINDSIZE);
	up = Muls(size, wsp->vx_orig + Div2(wsp->vx_size));
	up /= wsp->x_size;
	if (up > size) {
		up = size;
	}

	return(up + wsp->abx_orig + DLBWIDTH + LBOXSIZE);
#else  PORTRAIT
	size = wsp->by_size - (2 * (DLBWIDTH + LBOXSIZE) + WINDSIZE);
	up = Muls(size, wsp->vy_orig + Div2(wsp->vy_size));
	up /= wsp->y_size;
	if (up > size) {
		up = size;
	}

	return(up + wsp->aby_orig + DLBWIDTH + LBOXSIZE);
#endif LANDSCAPE / PORTRAIT
}	/* end of vindloc() */



/*
 * Display or remove the horizontal visible indicator for window 'wn' with
 * the left side of it given by the argument.
 */

void
dsphind(wn, left, flag)
int	wn;		/* window number           */
int	left;		/* left side of indicator  */
int	flag;		/* ON if display, else OFF */
{
	register		fromx;	/* from coordinate, x       */
	register		fromy;	/* from coordinate, y       */
	register struct	winint	*wsp;	/* pointer to window status */

	wsp = wstatp[wn];
	if (flag == OFF) {
		fromx = CINDXCOORD;
		fromy = CINDYCOORD;
	}
	else {
		fromx = HINDXCOORD;
		fromy = HINDYCOORD;
	}
	Waits();
	Outb(wsp->statflg, hwa.mflagport);
#ifdef LANDSCAPE
	d1box(wn, wsp->abx_orig + INDOFFS, PINDSIZE - 1, left, WINDSIZE - 1, fromx, fromy);
#else  PORTRAIT
	d1box(wn, left, WINDSIZE - 1, wsp->aby_lrc - INDOFFS - PINDSIZE + 1, PINDSIZE - 1, fromx, fromy);
#endif LANDSCAPE / PORTRAIT

	return;
}	/* end of dsphind() */



/*
 * Display or remove the vertical visible indicator for window 'wn' with the
 * upper side given by the argument.
 */

void
dspvind(wn, up, flag)
int	wn;		/* window number           */
int	up;		/* upper side of indicator */
int	flag;		/* ON if display, else OFF */
{
	register		fromx;	/* from coordinate, x       */
	register		fromy;	/* from coordinate, y       */
	register struct	winint	*wsp;	/* pointer to window status */

	wsp = wstatp[wn];
	if (flag == OFF) {
		fromx = CINDXCOORD;
		fromy = CINDYCOORD;
	}
	else {
		fromx = VINDXCOORD;
		fromy = VINDYCOORD;
	}
	Waits();
	Outb(wsp->statflg, hwa.mflagport);
#ifdef LANDSCAPE
	d1box(wn, up, WINDSIZE - 1, wsp->aby_lrc - INDOFFS - PINDSIZE + 1, PINDSIZE - 1, fromx, fromy);
#else  PORTRAIT
	d1box(wn, wsp->abx_lrc - INDOFFS - PINDSIZE + 1, PINDSIZE - 1, up, WINDSIZE - 1, fromx, fromy);
#endif LANDSCAPE / PORTRAIT

	return;
}	/* end of dspvind() */



/*
 * Copies the given area into window 'wn'. Only those areas which are visible
 * in the window (including the border) are copied.
 */

void
d1box(wn, left, xsize, up, ysize, fromx, fromy)
int		wn;		/* window number                           */
register	left;		/* abs. left pos. of the box on the screen */
register	xsize;		/* x size - 1                              */
register	up;		/* abs. upper pos.                         */
register	ysize;		/* y size - 1                              */
int		fromx;		/* from x coordinate                       */
int		fromy;		/* from y coordinate                       */
{
	register		 x;	/* temporary                          */
	register		 y;	/* temporary                          */
	int			 n;	/* # of rectangles in rectangle group */
	register struct	hwastruc *hwap;	/* pointer to hardware addresses      */
	register struct	wpstruc	 *wrp;	/* pointer to rectangle group         */

	xsize += left;		/* convert to right side */
	ysize += up;		/* convert to lower size */
	hwap = &hwa;
	wrp = wpp[wn];
	n = nwpart[wn];

	/*
	 * Scan all the rectangles and move out a part of the box if it is
	 * inside the rectangle.
	 */
	while (--n >= 0) {
		if ((pix_d)left <= wrp->xr_p && (pix_d)xsize >= wrp->xl_p &&
		    (pix_d)up <= wrp->yl_p && (pix_d)ysize >= wrp->yu_p) {

			/*
			 * Compute the part of the box to move out.
			 */
			x = wrp->xl_p - left;
			if (x < 0) {
				x = 0;
			}
			y = wrp->yu_p - up;
			if (y < 0) {
				y = 0;
			}

			/*
			 * Move it out.
			 */
			Waitsp();
			Move(fromx + x, fromy + y, left + x, up + y,
			     Min(xsize, wrp->xr_p) - left - x,
			     Min(ysize, wrp->yl_p) - up - y, UX_UY);
		}
		wrp++;		/* next rectangle */
	}	/* while (--n >= 0) */

	return;
}	/* end of d1box() */
