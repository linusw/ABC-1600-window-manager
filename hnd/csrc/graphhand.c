/* graphhand.c */

/*
 * This file contains low level graphic routines. These routines always works
 * in the mover coordinate system.
 */

/* 1984-08-05, Peter Andersson, Luxor Datorer AB (at DIAB) */

/*
 * Functions in this file:
 *	bline()
 *	circle()
 *	fill()
 *	fillcircle()
 *	fillrect()
 *	pphase()
 *	fndrect()
 */


#include	"../hsrc/defs.h"
#include	"../hsrc/structs.h"
#include	"../hsrc/eglobals.h"
#include	"../hsrc/graphconst.h"
#include	"../hsrc/macros.h"



/*
 * Line drawing according to Bresenham's algorithm from article in MINI-MICRO
 * SYSTEMS, Nov. 1983, pp. 204.
 * Lines are always drawn from left to right. They are divided into four cases
 * depending on direction.
 * On entry the start and ending points must be in absolute mover coordinates.
 */

void
bline(wn, x1, y1, x2, y2, mask, iflag)
int		wn;		/* window number                             */
register	x1;		/* starting point of the line                */
register	y1;
int		x2;		/* ending point of the line                  */
int		y2;
word		mask;		/* graphic mask to use when drawing the line */
int		iflag;		/* inverse line if YES                       */
{
	register struct	winint	 *wsp;	/* pointer to window status         */
	register struct	wpstruc	 *wrp;	/* pointer to rectangle group       */
	register struct	hwastruc *hwap;	/* pointer to hardware addresses    */
	register		 a;	/* incrementation value             */
	register		 b;	/* incrementation value             */
	register		 e;	/* error term                       */
	register	long	 grmp;	/* graphic mask pointer             */

	struct	wpstruc		 *fndrect();

	wsp = wstatp[wn];
	hwap = &hwa;

	/*
	 * Set screen to graphic mode.
	 */
	Setlgrmod(wsp->statflg, grmp, mask, iflag);

	/*
	 * Make x1 always less or equal x2.
	 */
	if (x1 > x2) {
		a = x1;
		x1 = x2;
		x2 = a;
		a = y1;
		y1 = y2;
		y2 = a;
	}

	/*
	 * Special cases for inverted, solid white, or solid black vertical or
	 * horizontal lines.
	 */
	if ((iflag != NO || mask == ~0 || mask == 0) && (x1 == x2 || y1 == y2)) {
		e = nwpart[wn];
		wrp = wpp[wn];
		if (y1 > y2) {		/* make y2 > y1 */
			a = y1;
			y1 = y2;
			y2 = a;
		}
		while (--e >= 0) {
			if ((wrp->bflags & B_NOTEMPTY) && (pix_d)x2 >= wrp->xlv_p &&
			    (pix_d)x1 <= wrp->xrv_p && (pix_d)y2 >= wrp->yuv_p &&
			    (pix_d)y1 <= wrp->ylv_p) {
				Waitsp();
				if (y1 == y2) {
					a = wrp->xlv_p;
					if (a < x1) {
						a = x1;
					}
					b = wrp->xrv_p;
					if (b > x2) {
						b = x2;
					}
					Outw((b - a) | UX_UY, hwap->x_sizeport);
					Outw(0, hwap->y_sizeport);
					b = y1;
				}
				else {		/* x1 == x2 */
					a = wrp->yuv_p;
					if (a < y1) {
						a = y1;
					}
					b = wrp->ylv_p;
					if (b > y2) {
						b = y2;
					}
					Outw(UX_UY, hwap->x_sizeport);
					Outw(b - a, hwap->y_sizeport);
					b = a;
					a = x1;
				}
				Outw(a, hwap->x_toport);
				Outw(b, hwap->y_toport);
				if (iflag == NO) {
					Outl((GRPHXCOORD << 16) | GRPHYCOORD, hwap->x_fromport);
				}
				else {
					Outw(a, hwap->x_fromport);
					Outw(b, hwap->y_fromport);
				}
			}
			wrp++;		/* next rectangle */
		}
		return;
	}

	wrp = NULL;		/* no rectangle found yet */

	if (y2 >= y1 && (x2 - x1) >= (y2 - y1)) {

		/*
		 * Direction between 0 and +45 degrees.
		 */
		a = 2 * (y2 - y1);
		b = x1 - x2;
		e = a + b;
		b += b + a;
		while (x1 < x2) {
			if (wrp == NULL || (pix_d)x1 > wrp->xrv_p || (pix_d)y1 > wrp->ylv_p) {
				wrp = fndrect(wn, x1, y1);	/* find new rectangle */
			}
			if (wrp != NULL) {	/* if point is visible */
				Setldot(x1, y1, grmp, iflag);
			}
			if (grmp-- == GRPHXCOORD) {
				grmp += 16;
			}
			if (e <= 0) {
				e += a;
			}
			else {
				e += b;
				y1++;
			}
			x1++;
		}
	}

	else if (y1 >= y2 && (x2 - x1) >= (y1 - y2)) {

		/*
		 * Direction between 0 and -45 degrees.
		 */
		a = 2 * (y1 - y2);
		b = x1 - x2;
		e = a + b;
		b += b + a;
		while (x1 < x2) {
			if (wrp == NULL || (pix_d)x1 > wrp->xrv_p || (pix_d)y1 < wrp->yuv_p) {
				wrp = fndrect(wn, x1, y1);	/* find new rectangle */
			}
			if (wrp != NULL) {
				Setldot(x1, y1, grmp, iflag);
			}
			if (grmp-- == GRPHXCOORD) {
				grmp += 16;
			}
			if (e <= 0) {
				e += a;
			}
			else {
				e += b;
				y1--;
			}
			x1++;
		}
	}

	else if (x2 >= x1 && (y2 - y1) >= (x2 - x1)) {

		/*
		 * Direction between +45 and +90 degrees.
		 */
		a = 2 * (x2 - x1);
		b = y1 - y2;
		e = a + b;
		b += b + a;
		while (y1 < y2) {
			if (wrp == NULL || (pix_d)x1 > wrp->xrv_p || (pix_d)y1 > wrp->ylv_p) {
				wrp = fndrect(wn, x1, y1);
			}
			if (wrp != NULL) {
				Setldot(x1, y1, grmp, iflag);
			}
			if (grmp-- == GRPHXCOORD) {
				grmp += 16;
			}
			if (e <= 0) {
				e += a;
			}
			else {
				e += b;
				x1++;
			}
			y1++;
		}
	}

	else {

		/*
		 * Direction between -45 and -90 degrees.
		 */
		a = 2 * (x2 - x1);
		b = y2 - y1;
		e = a + b;
		b += b + a;
		while (y1 > y2) {
			if (wrp == NULL || (pix_d)x1 > wrp->xrv_p || (pix_d)y1 < wrp->yuv_p) {
				wrp = fndrect(wn, x1, y1);
			}
			if (wrp != NULL) {
				Setldot(x1, y1, grmp, iflag);
			}
			if (grmp-- == GRPHXCOORD) {
				grmp += 16;
			}
			if (e <= 0) {
				e += a;
			}
			else {
				e += b;
				x1++;
			}
			y1--;
		}
	}

	/*
	 * Finish with the last pixel of the line.
	 */
	x1 = x2;
	y1 = y2;
	if (wrp == NULL || (pix_d)x1 < wrp->xlv_p || (pix_d)x1 > wrp->xrv_p ||
	    (pix_d)y1 < wrp->yuv_p || (pix_d)y1 > wrp->ylv_p) {
		wrp = fndrect(wn, x1, y1);
	}
	if (wrp != NULL) {
		Setldot(x1, y1, grmp, iflag);
	}

	return;
}	/* end of bline() */



/*
 * Routine to draw an arc in a window.
 * On entry the given coordinates must be absolute mover coordinates.
 * On exit the graphic cursor for the window is updated.
 */

void
circle(wn, xc, yc, xd, yd, cnt, mask, iflag)
int		wn;	/* window number                          */
int		xc;	/* x coord. of origin of arc              */
int		yc;	/* y coord. of origin of arc              */
int		xd;	/* x distance from origin to start of arc */
int		yd;	/* y distance from origin to start of arc */
register	cnt;	/* number of pixels to draw in arc        */
word		mask;	/* pattern of arc line                    */
int		iflag;	/* inverse arc if YES                     */
{
	register struct	winint	 *wsp;	/* pointer to window status      */
	register struct	wpstruc	 *wrp;	/* pointer to rectangle group    */
	register struct	hwastruc *hwap;	/* pointer to hardware addresses */
	register		 x;	/* x distance from origin        */
	register		 y;	/* y distance from origin        */
	register		 l;
	register		 flags;	/* flags                         */
	int			 xx;	/* absolute x position of pixel  */
	int			 yy;	/* absolute y position of pixel  */
	int			 x_lrc;
	int			 y_lrc;
	int			 lchx;	/* last pixel in arc inside      */
	int			 lchy;	/* virtual screen                */
	register	long	 grmp;	/* graphic mask pointer          */

	struct	wpstruc		 *fndrect();

	wsp = wstatp[wn];
	hwap = &hwa;
	x_lrc = wsp->x_orig + wsp->x_size - 1;
	y_lrc = wsp->y_orig + wsp->y_size - 1;
	wrp = NULL;				/* not inside a rectangle */
	l = 0;
	x = xd;
	y = yd;

	/*
	 * Init graphic cursor position.
	 */
	lchx = x + xc;
	lchy = y + yc;

	/*
	 * Set screen to graphic mode.
	 */
	Setlgrmod(wsp->statflg, grmp, mask, iflag);

	/*
	 * Draw the arc.
	 */
	while (cnt > 0) {
		xx = xc + x;
		yy = yc + y;
		if (wrp == NULL || (pix_d)xx < wrp->xlv_p || (pix_d)xx > wrp->xrv_p ||
		    (pix_d)yy < wrp->yuv_p || (pix_d)yy > wrp->ylv_p) {
			wrp = fndrect(wn, xx, yy);	/* find new rectangle */
		}
		if (wrp != NULL) {	/* if pixel is visible */
			Setldot(xx, yy, grmp, iflag);
			lchx = xx;	/* remember last changed pixel */
			lchy = yy;
		}
		else if ((pix_d)xx >= wsp->x_orig && xx <= x_lrc &&
			 (pix_d)yy >= wsp->y_orig && yy <= y_lrc) {
			lchx = xx;	/* remember pixel if inside */
			lchy = yy;	/* virtual screen */
		}
		if (grmp-- == GRPHXCOORD) {		/* rotate mask */
			grmp += 16;
		}

		flags = 0;
		if (x < 0) {
			flags = 8;
		}
		if (y < 0) {
			flags |= 4;
		}
		if (Abs(y) < Abs(x)) {
			flags |= 2;
		}

		switch (Div2(flags)) {

		case 0:

			if (l + x + x <= y) {
				flags++;
			}
			break;

		case 1:

			if (l + x + 1 < y + y) {
				flags++;
			}
			break;

		case 3:

			if (l + 1 < x + y + y) {
				flags++;
			}
			break;

		case 2:

			if (l + 1 < x + x + y) {
				flags++;
			}
			break;

		case 6:

			if (l + y + 1 < x + x) {
				flags++;
			}
			break;

		case 7:

			if (l + y + y + 1 < x) {
				flags++;
			}
			break;

		case 5:

			if (l + x + 1 + y + y < 0) {
				flags++;
			}
			break;

		case 4:

			if (l + x + x + 1 + y < 0) {
				flags++;
			}
			break;

		}	/* switch (Div2(flags)) */

		switch (flags) {

		case 0:
		case 3:

			x++;
			l += x + x - y - y;
			y--;
			cnt -= 2;
			break;

		case 1:
		case 8:

			l += x + x + 1;
			x++;
			cnt--;
			break;

		case 2:
		case 7:

			l -= y + y - 1;
			y--;
			cnt--;
			break;

		case 5:
		case 6:

			x--;
			l -= x + x + y + y;
			y--;
			cnt -= 2;
			break;

		case 4:
		case 13:

			l -= x + x - 1;
			x--;
			cnt--;
			break;

		case 12:
		case 15:

			y++;
			l += y + y - x - x;
			x--;
			cnt -= 2;
			break;

		case 11:
		case 14:

			l += y + y + 1;
			y++;
			cnt--;
			break;

		case 9:
		case 10:

			x++;
			l += x + x + y + y;
			y++;
			cnt -= 2;
			break;

		}	/* switch (flags) */

	}	/* while (cnt > 0) */

	/*
	 * Update current graphic cursor position to the end of the drawn arc.
	 */
#ifdef LANDSCAPE
	wsp->x_gcur = lchy - wsp->y_orig - wsp->x_gorig;
	wsp->y_gcur = lchx - wsp->x_orig - wsp->y_gorig;
#else  PORTRAIT
	wsp->x_gcur = lchx - wsp->x_orig - wsp->x_gorig;
	wsp->y_gcur = y_lrc - lchy - wsp->y_gorig;
#endif LANDSCAPE / PORTRAIT

	return;
}	/* end of circle() */



/*
 * Fill a rectangle with the desired pattern.
 * For a more detailed explanation of the mask, vmask, step, and op arguments,
 * see the documentation of the escape sequences.
 */

void
fill(wn, x1, y1, x2, y2, pnr)
int	wn;	/* window number                           */
int	x1;	/* upper left corner of rectangle to fill  */
int	y1;
int	x2;	/* lower right corner of rectangle to fill */
int	y2;
int	pnr;	/* pattern number                          */
{
	register struct	winint	 *wsp;	/* pointer to window status          */
	register struct	wpstruc	 *wrp;	/* pointer to rectangle group        */
	register struct hwastruc *hwap;	/* pointer to hardware addresses     */
	int			 n;	/* number of rectangles in window    */
	register		 xr1;	/* upper left corner of subrectangle */
	register		 yr1;	/* to fill                           */
	word			 vmask;	/* vertical mask                     */
	register		 step;	/* number of bits to shift 'mask'    */
	int			 oprtn;	/* operation when 'vmask' bit is 0   */
	register	word	 vic;	/* to mask out next bit in 'vmask'   */
	register		 start;	/* offset into 'mask'                */
	register		 tmp;	/* temporary                         */
	word			 vicini;/* used to obtain phased pattern     */
	int			 grmpini;
	byte			 stflg;	/* mover status flags                */

	hwap = &hwa;
	wsp = wstatp[wn];
	wrp = wpp[wn];
	n = nwpart[wn];
	stflg = wsp->statflg | HOLDYFL | HOLDXFL;
	if (wsp->termpara & TD_PHASE) {
		grmpini = pphase(wsp, x1, y1, pnr, &vicini);
	}
	else {
		vicini = 1;
		grmpini = 0;
	}
	vmask = wsp->msk2[pnr];
	step = wsp->shft[pnr];
	oprtn = wsp->op[pnr];

	/*
	 * Set to hold x and y mover registers.
	 */
	Waitsp();
	Outb(stflg, hwap->mflagport);
	Outw(wsp->msk1[pnr], hwap->grphaddr);	/* set mask to use */

	/*
	 * Now fill all the rectangles which are inside the given area to fill.
	 */
	while (--n >= 0) {
		if ((wrp->bflags & B_NOTEMPTY) && (pix_d)x2 >= wrp->xlv_p && (pix_d)x1 <= wrp->xrv_p &&
		    (pix_d)y2 >= wrp->yuv_p && (pix_d)y1 <= wrp->ylv_p) {

			/*
			 * The whole or a part of this rectangle shall be filled.
			 */
			xr1 = wrp->xlv_p;
			if (xr1 < x1) {
				xr1 = x1;
			}
			yr1 = wrp->yuv_p;
			if (yr1 < y1) {
				yr1 = y1;
			}

			/*
			 * Compute the pattern for the upper left corner of the
			 * sub-rectangle we are going to fill.
			 */
			vic = vicini;
			start = grmpini + xr1 - x1;
			tmp = yr1 - y1;
			while (--tmp >= 0) {	/* simulate filling */
				if ((vic & vmask) != 0 || (oprtn & 4) == 0) {
					start += step;
				}
				if ((vic <<= 1) == 0) {
					vic = 1;
				}
			}

			/*
			 * Fill the rectangle.
			 */
			tmp = wrp->xrv_p;
			if (tmp > x2) {
				tmp = x2;
			}
			pnr = wrp->ylv_p;
			if (pnr > y2) {
				pnr = y2;
			}
			fillrect(xr1, yr1, tmp - xr1, pnr - yr1, vmask, step, oprtn, start, vic, stflg);
		}	/* if ((wrp->bflags & B_NOTEMPTY) && .... ) */

		wrp++;		/* next rectangle */

	}	/* while (--n >= 0) */

	return;
}	/* end of fill() */



/*
 * Routine to draw a filled circle using a pattern.
 */

void
fillcircle(wn, xc, yc, r, pnr)
int	wn;		/* window number                 */
int	xc;		/* x coordinate of circle center */
int	yc;		/* y coordinate of circle center */
int	r;		/* radius of circle              */
int	pnr;		/* pattern number                */
{
	register struct	winint	 *wsp;	/* pointer to window status          */
	register struct hwastruc *hwap;	/* pointer to hardware addresses     */
	register struct	wpstruc	 *wrp;	/* pointer to visible rectangles     */
	register		 x;	/* x coordinate relative origin      */
	register		 y;	/* y coordinate relative origin      */
	int			 xmax;	/* max x coord. at current row       */
	int			 yold;	/* save of old pixel row             */
	register		 l;
	register		 cnt;	/* length of outer circle line       */
	word			 vmask;	/* vertical mask                     */
	int			 shift;	/* shift count                       */
	int			 oprtn;	/* operation                         */
	int			 grmp;	/* graphic mask pointer              */
	word			 vic;	/* vertical mask mask                */
	register		 flags;	/* used by circle drawing algorithm  */
	int			 xstart;/* start of pixel row to draw        */
	int			 xend;	/* end of pixel row to draw          */
	int			 xl;	/* left side of visible part to draw */
	int			 xr;	/* right side of vis. part to draw   */
	byte			 stflg;	/* mover status flags                */

	wsp = wstatp[wn];
	hwap = &hwa;

	/*
	 * Initialize things.
	 */
	x = 0;		/* starting x coordinate relative origin */
	xmax = 0;
	y = r;		/* starting y coordinate relative origin */
	l = 0;
	cnt = 4 * y + 1;	/* length of a half circle */
	stflg = wsp->statflg | HOLDYFL | HOLDXFL;
	vmask = wsp->msk2[pnr];
	shift = wsp->shft[pnr];
	oprtn = wsp->op[pnr];

	/*
	 * Initialize things for pattern.
	 */
	if (wsp->termpara & TD_PHASE) {
		grmp = pphase(wsp, xc, yc + y, pnr, &vic);
	}
	else {
		grmp = 0;
		vic = 1 << 15;
	}

	/*
	 * Set up mover flags and horizontal graphic mask.
	 */
	Waitsp();
	Outb(stflg, hwap->mflagport);
	Outw(wsp->msk1[pnr], hwap->grphaddr);

	/*
	 * Loop to draw filled circle.
	 */
	while (cnt > 0) {
		yold = y;

		/*
		 * Compute the next point in the outer side of the circle
		 * (note that x >= 0).
		 * This is a stripped version of the algorithm used in
		 * circle() and it should be replaced by a better one.
		 */
		flags = 0;
		if (y < 0) {
			flags |= 4;
		}
		if (Abs(y) < x) {
			flags |= 2;
		}

		switch (Div2(flags)) {

		case 0:

			if (l + 2 * x <= y) {
				flags++;
			}
			break;

		case 1:

			if (l + x + 1 < 2 * y) {
				flags++;
			}
			break;

		case 3:

			if (l + 1 < x + 2 * y) {
				flags++;
			}
			break;

		case 2:

			if (l + 1 < 2 * x + y) {
				flags++;
			}
			break;
		}

		switch (flags) {

		case 0:
		case 3:

			x++;
			l += 2 * (x - y);
			y--;
			cnt -= 2;
			break;

		case 1:

			l += 2 * x + 1;
			x++;
			cnt--;
			break;

		case 2:
		case 7:

			l -= 2 * y - 1;
			y--;
			cnt--;
			break;

		case 5:
		case 6:

			x--;
			l -= 2 * (x + y);
			y--;
			cnt -= 2;
			break;

		case 4:

			l -= 2 * x - 1;
			x--;
			cnt--;
			break;
		}

		/*
		 * If we still are on the same pixel row, just adjust the
		 * maximum x value.
		 */
		if (yold == y && xmax < x) {
			xmax = x;
		}

		/*
		 * If we have come to a new pixel row or it is the last one,
		 * output a row on the screen.
		 */
		if (yold != y || cnt <= 0) {
			xstart = xc - xmax;
			xend = xc + xmax;
			yold += yc;
			flags = nwpart[wn];
			wrp = wpp[wn];

			while (--flags >= 0) {
				if ((wrp->bflags & B_NOTEMPTY) && (pix_d)yold >= wrp->yuv_p &&
				    (pix_d)yold <= wrp->ylv_p) {
					xl = wrp->xlv_p;
					if (xl < xstart) {
						xl = xstart;
					}
					xr = wrp->xrv_p;
					if (xr > xend) {
						xr = xend;
					}
					if (xl <= xr) {
						Waitsp();

						/*
						 * Set up the size.
						 */
						Outw((xr - xl) | UX_UY, hwap->x_sizeport);
						Outw(0, hwap->y_sizeport);

						/*
						 * Set up destination.
						 */
						Outw(xl, hwap->x_toport);
						Outw(yold, hwap->y_toport);
						xl = GRPHXCOORD + ((grmp - xmax + xl - xstart) & 0x0f);
						if (vmask & vic) {
							Outw(xl, hwap->x_fromport);
							Outw(GRPHYCOORD, hwap->y_fromport);
						}
						else {
							switch (oprtn & 3) {

							case 0:		/* clear line */
								Outl((CLRXCOORD << 16) | CLRYCOORD,
								     hwap->x_fromport);
								break;

							case 1:		/* set line */
								Outl((FILLXCOORD << 16) | FILLYCOORD,
								     hwap->x_fromport);
								break;

							case 2:		/* use mask but complemented */
								Outb(stflg ^ COMPMFL, hwap->mflagport);
								Outw(xl, hwap->x_fromport);
								Outw(GRPHYCOORD, hwap->y_fromport);
								Waitsp();
								Outb(stflg, hwap->mflagport);
								break;

							case 3:		/* leave line as it is */
								break;
							}
						}	/* else */
					}	/* if (xl <= xr) */
				}

				/*
				 * Next rectangle.
				 */
				wrp++;
			}

			/*
			 * Initialize maximal x value for next pixel row.
			 */
			xmax = x;

			/*
			 * Update things concerning the pattern.
			 */
			if ((vmask & vic) || (oprtn & 4) == 0) {
				grmp -= shift;
			}
			if ((vic >>= 1) == 0) {
				vic = 1 << 15;
			}
		}	/* if (yold != y || cnt <= 0) */
	}	/* while (cnt > 0) */

	return;
}	/* end of fillcircle() */



/*
 * Fill the given rectangle.
 */

void
fillrect(x, y, xsize, ysize, vmask, shift, oprtn, grmp, vic, stflg)
int		x;		/* x coordinate of upper left corner */
register	y;		/* y coordinate of upper left corner */
int		xsize;		/* width of rectangle minus one      */
int		ysize;		/* height of rectangle minus one     */
word		vmask;		/* vertical mask                     */
register	shift;		/* shift count                       */
register	oprtn;		/* operation                         */
register	grmp;		/* graphic mask pointer              */
word		vic;		/* vertical mask mask                */
byte		stflg;		/* mover status flags                */
{
	register struct	hwastruc *hwap;	/* pointer to hardware addresses */

	hwap = &hwa;

	/*
	 * Set up the size.
	 */
	Waitsp();
	Outw(xsize | UX_UY, hwap->x_sizeport);
	Outw(0, hwap->y_sizeport);

	/*
	 * Fill the rectangle one pixel row at a time.
	 */
	while (ysize-- >= 0) {
		Waitsp();
		Outw(x, hwap->x_toport);
		Outw(y, hwap->y_toport);
		if (vmask & vic) {
			Outw(GRPHXCOORD + (grmp & 0x0f), hwap->x_fromport);
			Outw(GRPHYCOORD, hwap->y_fromport);
			grmp += shift;
		}
		else {
			switch (oprtn & 3) {

			case 0:		/* clear the line */
				Outl((CLRXCOORD << 16) | CLRYCOORD, hwap->x_fromport);
				break;

			case 1:		/* set the line */
				Outl((FILLXCOORD << 16) | FILLYCOORD, hwap->x_fromport);
				break;

			case 2:		/* use mask but complemented */
				Outb(stflg ^ COMPMFL, hwap->mflagport);
				Outw(GRPHXCOORD + (grmp & 0x0f), hwap->x_fromport);
				Outw(GRPHYCOORD, hwap->y_fromport);
				Waitsp();
				Outb(stflg, hwap->mflagport);
				break;

			case 3:		/* leave line as it is */
				break;
			}
			if ((oprtn & 4) == 0) {
				grmp += shift;
			}
		}

		/*
		 * Rotate vertical mask mask.
		 */
		if ((vic <<= 1) == 0) {
			vic = 1;
		}

		/*
		 * Next pixel row.
		 */
		y++;
	}

	return;
}	/* end of fillrect() */



/*
 * This function phases the pattern relative 'x_orig' and 'y_orig'. Note that
 * this function works, even if the given point is outside the virtual screen.
 * It returns the graphic mask pointer and in a pointer argument the vertical
 * mask mask.
 */

int
pphase(wsp, x, y, pnr, vicp)
register struct	winint	*wsp;	/* pointer to window status    */
int			x;	/* point to compute phase for  */
register		y;
register		pnr;	/* pattern number              */
word			*vicp;	/* returned vertical mask mask */
{
	register	grmp;	/* used to compute graphic mask pointer */
	register word	vmsk1;	/* vertical mask                        */
	register word	vmsk2;	/* vertical mask                        */

	y -= wsp->y_orig;
	y &= 0x3ff;		/* make sure positive */

	/*
	 * Compute the vertical mask mask.
	 */
	*vicp = 1 << (y & 0x0f);

	/*
	 * Compute the rotate count caused by the vertical position.
	 */
	if ((wsp->op[pnr] & 4) == 0) {
		grmp = y;
	}
	else {

		/*
		 * Count the number of set bits in the vertical mask.
		 */
		vmsk1 = vmsk2 = wsp->msk2[pnr];
		grmp = 0;
		do {
			if (vmsk1 & 1) {
				grmp++;
			}
		} while ((vmsk1 >>= 1) != 0);
		grmp = Muls(grmp, Div16(y));
		y &= 0x0f;
		while (--y >= 0) {
			if (vmsk2 & 1) {
				grmp++;
			}
			vmsk2 >>= 1;
		}

	}

	/*
	 * Finally multiply with the shift count and add the effect of the
	 * horizontal position to the graphic mask pointer.
	 */
	return((Muls(grmp, wsp->shft[pnr]) + x - wsp->x_orig) & 0x0f);
}	/* end of pphase() */



/*
 * This function returns a pointer to a rectangle in window wn in which the
 * pixel at x,y is visible.
 * The coordinates of the pixel must be absolute and in the mover coordinate
 * system.
 * If the pixel is not visible, NULL is returned.
 */

struct wpstruc *
fndrect(wn, x, y)
int		wn;		/* window number         */
register pix_d	x;		/* x coordinate of pixel */
register pix_d	y;		/* y coordinate of pixel */
{
	register struct	wpstruc	*wrp;	/* pointer to rectangle group */
	register		cnt;	/* counter                    */

	cnt = nwpart[wn];
	wrp = wpp[wn];

	while (--cnt >= 0) {
		if ((wrp->bflags & B_NOTEMPTY) && x >= wrp->xlv_p && x <= wrp->xrv_p &&
		    y >= wrp->yuv_p && y <= wrp->ylv_p) {
			return(wrp);
		}
		wrp++;
	}

	return(NULL);
}	/* end of fndrect() */
