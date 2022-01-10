/* escgraph.c */

/*
 * This file contains functions which executes some of the graphic
 * functions that can be performed using escape sequences.
 */

/* 1984-08-01, Peter Andersson, Luxor Datorer AB (at DIAB) */

/*
 * Functions in this file:
 *	edot()
 *	emovea()
 *	spray()
 *	inwin()
 */


#include	"../hsrc/defs.h"
#include	"../hsrc/structs.h"
#include	"../hsrc/eglobals.h"
#include	"../hsrc/graphconst.h"
#include	"../hsrc/macros.h"
#include	"../hsrc/charcodes.h"



/*
 * Function to draw a point.
 */

#define	NM_DOT

#ifndef	NM_DOT		/* use mover to draw the point */
			/* NOTE: This version does not support pixel read */

void
edot(wn, x, y, oprtn)
int	wn;		/* window number               */
int	x;		/* x coord. of pixel to change */
int	y;		/* y coord. of pixel to change */
int	oprtn;		/* operation                   */
{
	register struct	winint	 *wsp;	/* pointer to window status        */
	register struct	hwastruc *hwap;	/* pointer to hardware addresses   */
	register		 xs;	/* save of pixel to change         */
	register		 ys;
	register	dword	 coord;	/* coordinate in mover coordinates */
	struct		wpstruc	 *fndrect();

	wsp = wstatp[wn];
	xs = x;
	ys = y;

	if (inwin(wsp, &x, &y) != NO) {

		/*
		 * Position specified is inside the virtual screen.
		 */
		wsp->x_gcur = xs;	/* update graphic cursor */
		wsp->y_gcur = ys;

		/*
		 * Check if the point is visible.
		 */
		xs = x;
		ys = y;
		if (fndrect(wn, xs, ys) != NULL) {

			/*
			 * The pixel is visible.
			 */
			coord = (xs << 16) | ys;
			hwap = &hwa;
			Waitsp();
			switch (oprtn) {

			case 0:		/* set pixel */

				Outb(wsp->statflg, hwap->mflagport);
				Movel((FILLXCOORD << 16) | FILLYCOORD, coord, UX_UY << 16);
				break;

			case 1:		/* clear pixel */

				Outb(wsp->statflg, hwap->mflagport);
				Movel((CLRXCOORD << 16) | CLRYCOORD, coord, UX_UY << 16);
				break;

			case 2:		/* complement pixel */

				Outb(wsp->statflg | COMPMFL, hwap->mflagport);
				Movel(coord, coord, UX_UY << 16);
				break;
			}	/* switch (oprtn) */
		}
	}	/* if (inwin(wsp, &x, &y) != NO) */

	return;
}	/* end of edot() */

#else		/* do not use mover to draw the point */

void
edot(wn, x, y, oprtn)
int	wn;		/* window number               */
int	x;		/* x coord. of pixel to change */
int	y;		/* y coord. of pixel to change */
int	oprtn;		/* operation                   */
{
	register struct	winint	*wsp;	/* pointer to window status   */
	register		xs;	/* save of pixel to change    */
	register		ys;
	register		tmp;	/* temporary                  */
	register		bit;	/* bit number in byte         */
	register	byte	*addr;	/* address to byte to modify  */
	register	char	*cp;	/* to construct report        */
	char			*itoa();
	struct		wpstruc	*fndrect();

	wsp = wstatp[wn];
	xs = x;
	ys = y;

	tmp = inwin(wsp, &x, &y);
	addr = (byte *)(hwa.pictmemst + Ctoaddr(x, y));
	bit = Ctobit(x);
	Waits();
	if (oprtn == 10) {

		/*
		 * Read color of pixel.
		 */
		cp = itoa(xs, &repprivate[2]);
		*cp = ';';
		cp = itoa(ys, cp + 1);
		*cp++ = ';';
		*cp++ = '1';
		*cp++ = '1';
		if (tmp != NO) {

			/*
			 * The pixel is inside the virtual screen.
			 */
			*cp++ = ';';
			*cp = '0';
			if (fndrect(wn, x, y) != NULL) {
				if (Inb(addr) & bit) {
					if ((wsp->statflg & COMPMFL) == 0) {
						*cp = '1';
					}
				}
				else if (wsp->statflg & COMPMFL) {
					*cp = '1';
				}
			}
			cp++;
		}
		*cp = DRAWPOINT;
		putonque(wn, &repprivate[0], cp - &repprivate[0] + 1, YES);
	}
	else if (tmp != NO && oprtn <= 2) {

		/*
		 * Position specified is inside the virtual screen.
		 */
		wsp->x_gcur = xs;	/* update graphic cursor */
		wsp->y_gcur = ys;

		/*
		 * Check if the point is visible.
		 */
		if (fndrect(wn, x, y) != NULL) {

			/*
			 * The pixel is visible.
			 */
			tmp = oprtn;
			if (wsp->statflg & COMPMFL) {
				if (tmp < 2) {
					tmp ^= 1;
				}
			}
			switch (tmp) {

			case 0:		/* set pixel */

				Orb(bit, addr);
				break;

			case 1:		/* clear pixel */

				Andb(~bit, addr);
				break;

			case 2:		/* complement pixel */

				Xorb(bit, addr);
				break;
			}	/* switch (tmp) */
		}
	}

	return;
}	/* end of edot() */

#endif



/*
 * Function for move. I.e. move an area in a window using the mover.
 * Note this is only allowed if the from and to area are completely inside the
 * virtual screen.
 */

void
emovea(wn, fromx, fromy, tox, toy, width, height, oprtn)
int	wn;	/* window number                                  */
int	fromx;	/* x coordinate of lower left corner of from area */
int	fromy;	/* y coordinate of lower left corner of from area */
int	tox;	/* x coordinate of lower left corner of to area   */
int	toy;	/* y coordinate of lower left corner of to area   */
int	width;	/* width of area to move                          */
int	height;	/* height of area to move                         */
int	oprtn;	/* invert while moving if oprtn == 1              */
{
	register struct	winint	 *wsp;	/* pointer to window status      */
	register struct hwastruc *hwap;	/* pointer to hardware addresses */
	register struct	wpstruc	 *wrp1;	/* move from rectangles          */
	register struct	wpstruc	 *wrp2;	/* move to rectangles            */
	struct		wpstruc	 *wrp;	/* pointer to rectangles for win */
	int			 n1;	/* from rectangles counter       */
	int			 n2;	/* to rectangles counter         */
	int			 n;	/* # of rectangles in window     */
	int			 sleft;	/* left side of source rect.     */
	int			 sup;	/* upper side of source rect.    */
	int			 dleft;	/* left side of dest. rect.      */
	int			 dright;/* right side of dest. rect.     */
	int			 dup;	/* upper side of dest. rect.     */
	int			 dlow;	/* lower side of dest. rect.     */
	register		 xs;	/* x source coord. of moved      */
	register		 xd;	/* x destination coord. of moved */
	register		 xsize;	/* x size of moved rectangle     */
	register		 ys;	/* y source coord. of moved      */
	register		 yd;	/* y source coord. of moved      */
	register		 ysize;	/* y size of moved rectangle     */

	if (width > 0 && height > 0) {

		/*
		 * The area is non-zero.
		 */
		wsp = wstatp[wn];
		hwap = &hwa;
		Waitsp();
		if (oprtn != 1) {
			Outb(GSTATFLG, hwap->mflagport);
		}
		else {
			Outb(GSTATFLG | COMPMFL, hwap->mflagport);
		}

		/*
		 * Adjust the coordinates using the graphic origin.
		 */
		fromx += wsp->x_gorig;
		fromy += wsp->y_gorig;
		tox += wsp->x_gorig;
		toy += wsp->y_gorig;
#ifdef LANDSCAPE
		if ((pix_d)(tox + width) > wsp->y_size || (pix_d)(fromx + width) > wsp->y_size ||
		    (pix_d)(toy + height) > wsp->x_size || (pix_d)(fromy + height) > wsp->x_size) {
			return;		/* does not fit */
		}

		/*
		 * Convert to absolute coordinates.
		 */
		xs = fromx;
		fromx = fromy + wsp->x_orig;
		fromy = xs + wsp->y_orig;
		xs = tox;
		tox = toy + wsp->x_orig;
		toy = xs + wsp->y_orig;
		xs = height - 1;
		height = width - 1;
		width = xs;
#else  PORTRAIT
		if ((pix_d)(tox + width) > wsp->x_size || (pix_d)(fromx + width) > wsp->x_size ||
		    (pix_d)(toy + height) > wsp->y_size || (pix_d)(fromy + height) > wsp->y_size) {
			return;		/* does not fit */
		}

		/*
		 * Convert to absolute coordinates.
		 */
		fromx += wsp->x_orig;
		tox += wsp->x_orig;
		fromy = wsp->y_orig + wsp->y_size - fromy - height;
		toy = wsp->y_orig + wsp->y_size - toy - height;
		width--;
		height--;
#endif LANDSCAPE / PORTRAIT

		/*
		 * Go through all the rectangles of the window and do the move
		 * for the areas where both the destination and the source is
		 * visible.
		 */
		wrp = wrp1 = wpp[wn];
		n = n1 = nwpart[wn];
		while (--n1 >= 0) {
			if ((wrp1->bflags & B_NOTEMPTY) &&
			    wrp1->xrv_p >= (pix_d)fromx && wrp1->xlv_p <= (pix_d)(fromx + width) &&
			    wrp1->ylv_p >= (pix_d)fromy && wrp1->yuv_p <= (pix_d)(fromy + height)) {

				/*
				 * This rectangle belongs to the source area.
				 */
				sleft = wrp1->xlv_p;
				if (sleft < fromx) {
					sleft = fromx;
				}
				sup = wrp1->yuv_p;
				if (sup < fromy) {
					sup = fromy;
				}
				xd = tox - fromx;
				yd = toy - fromy;
				dleft = sleft + xd;
				dright = fromx + width;
				if ((pix_d)dright > wrp1->xrv_p) {
					dright = wrp1->xrv_p;
				}
				dright += xd;
				dup = sup + yd;
				dlow = fromy + height;
				if ((pix_d)dlow > wrp1->ylv_p) {
					dlow = wrp1->ylv_p;
				}
				dlow += yd;
				wrp2 = wrp;
				n2 = n;
				while (--n2 >= 0) {
					if ((wrp2->bflags & B_NOTEMPTY) &&
					    wrp2->xrv_p >= (pix_d)dleft &&
					    wrp2->xlv_p <= (pix_d)dright &&
					    wrp2->ylv_p >= (pix_d)dup &&
					    wrp2->yuv_p <= (pix_d)dlow) {

						/*
						 * This rectangle belongs to
						 * the destination area.
						 */
						xs = sleft;
						xd = dleft;
						xsize = dright - xd;
						if ((pix_d)xd < wrp2->xlv_p) {
							xd = wrp2->xlv_p;
							xs += xd - dleft;
							xsize -= xd - dleft;
						}
						if ((pix_d)dright > wrp2->xrv_p) {
							xsize -= dright - wrp2->xrv_p;
						}
						ys = sup;
						yd = dup;
						ysize = dlow - yd;
						if ((pix_d)yd < wrp2->yuv_p) {
							yd = wrp2->yuv_p;
							ys += yd - dup;
							ysize -= yd - dup;
						}
						if ((pix_d)dlow > wrp2->ylv_p) {
							ysize -= dlow - wrp2->ylv_p;
						}

						/*
						 * Do the move for 4 different
						 * cases.
						 */
						Waitsp();
						if (toy > fromy && tox > fromx) {

							/*
							 * Move down to the
							 * right.
							 * Count down in both
							 * x and y directions
							 * ==> negative sizes.
							 */
							Move(xs + xsize, ys + ysize,
							     xd + xsize, yd + ysize,
							     (-xsize) & 0x3ff, -ysize, DX_DY);
						}
						else if (tox > fromx && toy <= fromy) {

							/*
							 * Move up to the
							 * right.
							 * Count down in x
							 * ==> negative x size.
							 */
							Move(xs + xsize, ys, xd + xsize, yd,
							     (-xsize) & 0x3ff, ysize, DX_UY);
						}
						else if (tox <= fromx && toy <= fromy) {

							/*
							 * Move up to the left.
							 * Count up in both x
							 * and y direction ==>
							 * positive sizes.
							 */
							Move(xs, ys, xd, yd, xsize, ysize, UX_UY);
						}
						else {

							/*
							 * Move down to the
							 * left.
							 * Count down in y
							 * direction ==>
							 * negative y size.
							 */
							Move(xs, ys + ysize, xd, yd + ysize,
							     xsize, -ysize, UX_DY);
						}
					}
					wrp2++;		/* next destination rectangle */
				}	/* while (--n2 >= 0) */
			}
			wrp1++;		/* next source rectangle */
		}	/* while (--n1 >= 0) */
	}

	return;
}	/* end of emovea() */



/*
 * Spray an area using the given pattern as mask.
 */

void
spray(wn, x, y, pnr, lop)
int		wn;		/* window number               */
register	x;		/* x coordinate of given point */
register	y;		/* y coordinate of given point */
int		pnr;		/* pattern number              */
int		lop;		/* logical operation           */
{
	register struct	winint	 *wsp;	/* pointer to window status      */
	register struct	hwastruc *hwap;	/* pointer to hardware addresses */
	register	dword	 *p1;	/* to construct new bitmap       */
	register	dword	 *p2;	/* to construct	new bitmap       */
	dword			 *p3;
	register		 xd;	/* x coordinate of work area     */
	int			 yd;	/* y coordinate of work area     */
	register		 xsize;	/* size of work area             */
	register		 ysize;
	register		 tmp;	/* temporary                     */
	int			 grmp;	/* graphic mask pointer          */
	word			 vic;	/* vertical mask mask            */

	wsp = wstatp[wn];
	xsize = ysize = 8 * sizeof(dword) - 1;
#ifndef LANDSCAPE
	y -= ysize;
#endif	NOT LANDSCAPE

	/*
	 * Initialize things for pattern.
	 */
	if (wsp->termpara & TD_PHASE) {
		grmp = pphase(wsp, x, y, pnr, &vic);
	}
	else {
		vic = 1;
		grmp = 0;
	}

	/*
	 * Compute which part of the area to spray is inside window.
	 */
	xd = XSPRAYCOORD;
	tmp = x - wsp->avx_orig;
	if (tmp < 0) {
		xsize += tmp;
		xd -= tmp;
		x = wsp->avx_orig;
	}
	yd = YSPRAYCOORD;
	tmp = y - wsp->avy_orig;
	if (tmp < 0) {
		ysize += tmp;
		yd -= tmp;
		y = wsp->avy_orig;
	}
	tmp = wsp->avx_lrc - x;
	if (xsize > tmp) {
		xsize = tmp;
	}
	tmp = wsp->avy_lrc - y;
	if (ysize > tmp) {
		ysize = tmp;
	}

	/*
	 * If anything left, do the rest.
	 */
	if (xsize >= 0 && ysize >= 0) {
		hwap = &hwa;

		/*
		 * Move to a work area (do two copies).
		 */
		Waitsp();
		Outb(wsp->statflg, hwap->mflagport);
		Outw(xsize | UX_UY, hwap->x_sizeport);
		Outw(ysize, hwap->y_sizeport);
		Outw(xd, hwap->x_toport);
		Outw(yd, hwap->y_toport);
		Outw(x, hwap->x_fromport);
		Outw(y, hwap->y_fromport);
		Waitsp();
		Outw(xd + 8 * sizeof(dword), hwap->x_toport);
		Outw(yd, hwap->y_toport);
		Outw(x, hwap->x_fromport);
		Outw(y, hwap->y_fromport);

		/*
		 * Fill the left copy with the desired pattern.
		 */
		tmp = pnr;	/* move to register variable */
		p1 = &wsp->spraym.sp_mask[0];
		p3 = p2 = (dword *)(hwap->pictmemst + (YSPRAYCOORD * PICTSIZE + XSPRAYCOORD) / 8);
		Waitsp();
		Outw(wsp->msk1[tmp], hwap->grphaddr);	/* set up mask */
		Outb(GSTATFLG | HOLDYFL | HOLDXFL, hwap->mflagport);
		fillrect(XSPRAYCOORD, YSPRAYCOORD, 8 * sizeof(dword) - 1, 8 * sizeof(dword) - 1,
			 wsp->msk2[tmp], wsp->shft[tmp], wsp->op[tmp], grmp, vic,
			 GSTATFLG | HOLDYFL | HOLDXFL);

		/*
		 * Do an AND operation on the filled area using the spray mask.
		 */
		Waitsp();
		tmp = 8 * sizeof(dword);
		do {
			Andl(*p1++, p2);
			p2 += PICTSIZE / (8 * sizeof(dword));
		} while (--tmp != 0);

		/*
		 * Concatenate the two copies using the desired logical
		 * operation.
		 */
		if (lop != 0) {		/* if not replace */
			p2 = p3;
			p1 = p2 + 1;
			tmp = 8 * sizeof(dword);
			do {
				switch (lop) {

				case 1:		/* set set bits in mask */
					Orl(Inl(p1), p2);
					break;

				case 2:		/* clear set bits in mask */
					Outl(Inl(p1) & ~Inl(p2), p2);
					break;

				case 3:		/* complement set bits in mask */
					Xorl(Inl(p1), p2);
					break;
				}
				p1 += PICTSIZE / (8 * sizeof(dword));
				p2 += PICTSIZE / (8 * sizeof(dword));
			} while (--tmp != 0);
		}

		/*
		 * Now copy the data out into the window again.
		 * We know that the whole rectangle belongs to the area inside
		 * the window borders, so it is OK to use the d1box() function.
		 */
		Outb(wsp->statflg, hwap->mflagport);
		d1box(wn, x, xsize, y, ysize, xd, yd);
	}

	return;
}	/* end of spray() */



/*
 * Check if a given coordinate in the graphic cursor coordinate system is inside
 * the virtual screen of a window and if so return YES, else NO.
 * If inside, the coordinates are converted to absolute pixel positions in mover
 * coordinates.
 */

int
inwin(wsp, x, y)
register struct	winint	*wsp;	/* pointer to window status */
register		*x;	/* pointer to x coordinate  */
register		*y;	/* pointer to y coordinate  */
{
#ifdef LANDSCAPE
	register	tmp;	/* temporary */

	*x += wsp->x_gorig;
	*y += wsp->y_gorig;
	if (*x >= wsp->y_size || *y >= wsp->x_size) {
		return(NO);	/* outside virtual screen */
	}
	tmp = *x;
	*x = *y + wsp->x_orig;
	*y = tmp + wsp->y_orig;
#else  PORTRAIT
	*x += wsp->x_gorig;
	*y += wsp->y_gorig;
	if (*x >= wsp->x_size || *y >= wsp->y_size) {
		return(NO);	/* outside virtual screen */
	}
	*x += wsp->x_orig;	/* convert logical pos. to abs pos. */
	*y = wsp->y_orig + wsp->y_size - 1 - *y;
#endif LANDSCAPE / PORTRAIT

	return(YES);		/* inside virtual screen */
}	/* end of inwin() */
