/* paint.c */

/*
 * This file contains routines to take care of the paint function.
 * They are low level routines, and all coordinates are supposed to be given
 * in the mover coordinate system.
 */

/* 1985-06-07, Peter Andersson, Luxor Datorer AB */

/*
 * Functions in this file:
 *	paint()
 *	scanshadow()
 *	fillshd()
 *	fixscanbuf()
 */



#include	"../hsrc/defs.h"
#include	"../hsrc/structs.h"
#include	"../hsrc/eglobals.h"
#include	"../hsrc/graphconst.h"
#include	"../hsrc/macros.h"
#include	"../hsrc/dnixmacros.h"
#include	<dnix/fcodes.h>



/*
 * Some constants used in this file.
 */

#define		UP		1		/* scan in up direction    */
#define		DOWN		2		/* scan in down direction  */
#define		FFT		5		/* fifo is full, terminate */
#define		UPBIT		(1 << 30)	/* scan up flag            */
#define		DOWNBIT		(1 << 31)	/* scan down flag          */
#define		FIFOSIZE	512		/* size of the fifo buffer */
#define		FOURBITS	0x0f		/* lower four bits         */
#define		FIVEBITS	0x1f		/* lower five bits         */
#define		TENBITS		0x3ff		/* lower ten bits          */
#define		TWENTYBITS	0x0fffff	/* lower twenty bits       */



/*
 * Data type for the fifo buffer.
 * Each element in the buffer contains packed information as follows:
 *   - The least significant 10 bits (bits 0 to 9) contains the x coordinate
 *     of the starting point of the pixel row.
 *   - Bits 10 to 19 contains the y coordinate of the starting point of the
 *     pixel row.
 *   - Bits 20 to 29 contains the length of the pixel row.
 *   - Bits 30 contain the scan up flag and bit 31 the scan down flag.
 */

typedef	unsigned long	fifo;



/*
 * Paint status structure.
 */

struct	paintst {
	fifo	*first;		/* pointer to first remaining element in buffer   */
	fifo	*last;		/* pointer to last remaining element in buffer    */
	fifo	*fifovect;	/* pointer to fifo buffer                         */
	fifo	*fifoend;	/* pointer to end of fifo buffer                  */
	word	upvic;		/* vertical bit position mask for up direction    */
	word	dovic;		/* vertical bit position mask for down direction  */
	word	vmask;		/* vertical pattern mask                          */
	pix_d	xs;		/* x coordinate for start position                */
	pix_d	cyup;		/* current pixel line # when going up (smaller y) */
	pix_d	cydo;		/* current pixel line when going down             */
	byte	upshift;	/* shift count when going up                      */
	byte	doshift;	/* shift count when going down                    */
	sint	pst;		/* paint status (pattern or not)                  */
	byte	shift;		/* shift value                                    */
	byte	gop;		/* operation selected (pattern parameter)         */
	byte	p_stflg;	/* status flags for the window with the HOLDYFL
				   and HOLDXFL bits set                           */
	int	limitpixel;	/* limiting pixel value, i.e. value of            */
	dword	*scanbuf;	/* pointer to scan buffer                         */
	int	scany;		/* coordinate of pixel line currently in scan buf */
	struct	wpstruc	*p_wrp;	/* pointer to visible rectangles for the window   */
	int	p_rects;	/* number of visible rectangles in the window     */
};

static	struct	paintst	pdata;



/*
 * Function for paint.
 */

#define	Getbit(px, py)	((Inb(hwap->pictmemst + py * PICTSIZE / 8 + (px >> 3)) >> (7 - (px & 7))) & 1)

void
paint(wn, x, y, pnr)
register	wn;	/* window number                            */
register	x;	/* point inside area to paint, x coordinate */
register	y;	/* point inside area to paint, y coordinate */
register	pnr;	/* pattern number                           */
{
	register	fifo	 tmp;	/* packed form of coordinate     */
	register struct	winint	 *wsp;	/* pointer to window status      */
	register struct	hwastruc *hwap;	/* pointer to hardware addresses */
	register struct	paintst	 *pdp;	/* pointer to paint status       */
	register	char	 *p;	/* temporary pointer             */

	/*
	 * Initialize some pointers.
	 */
	wsp = wstatp[wn];
	hwap = &hwa;
	pdp = &pdata;

	/*
	 * Initialize some other things.
	 */
	pdp->p_wrp = wpp[wn];
	pdp->p_rects = nwpart[wn];
	pdp->p_stflg = wsp->statflg | HOLDYFL | HOLDXFL;

	/*
	 * Allocate space for the fifo buffer.
	 */
	if ((p = (char *)Sbrk((SCXSIZE / (8 * sizeof(dword)) + 2) * sizeof(dword) +
	     FIFOSIZE * sizeof(fifo))) == NULL) {
		return;		/* no memory, just skip the paint */
	}

	/*
	 * Initialize the scan buffer.
	 */
	*((dword *)p) = 1;				/* left limiting pixel */
	*((dword *)(p + SCXSIZE / 8) + 1) = 1 << 31;	/* right limiting pixel */
	pdp->scanbuf = (dword *)p + 1;

	/*
	 * Initialize the fifo buffer.
	 */
	pdp->first = pdp->last = pdp->fifovect = (fifo *)(p + (SCXSIZE / (8 * sizeof(dword)) + 2) *
		                                 sizeof(dword));
	pdp->fifoend = pdp->fifovect + (FIFOSIZE - 1);

	/*
	 * Find out which value limiting pixels have.
	 */
	Waitsp();
	pdp->limitpixel = Getbit(x, y);

	/*
	 * If pattern paint or the paint color is the same as the starting
	 * pixel, set up things for it.
	 */
	pdp->pst = 0;
	if (pnr != 0 || (pdp->limitpixel != 0 && (wsp->statflg & COMPMFL) == 0)
	    || (pdp->limitpixel == 0 && (wsp->statflg & COMPMFL))) {
		Outw(wsp->msk1[pnr], hwap->grphaddr);	/* set mask to use */
		pdp->shift = wsp->shft[pnr];
		if (wsp->termpara & TD_PHASE) {
			pdp->upshift = pphase(wsp, x, y, pnr, &pdp->upvic);
		}
		else {
			pdp->upshift = 0;
			pdp->upvic = 1;
		}
		pdp->dovic = pdp->upvic;
		pdp->doshift = pdp->upshift;
		pdp->cydo = pdp->cyup = y;
		pdp->vmask = wsp->msk2[pnr];
		pdp->gop = wsp->op[pnr];
		pdp->pst = 1;
		pdp->xs = x;
	}

	/*
	 * Set up current pixel row in the scan buffer to an impossible value.
	 */
	pdp->scany = -1;

	/*
	 * Scan from the starting point and put in fifo.
	 */
	scanshadow((fifo)((1 << 20) | (y << 10) | x), UP | DOWN);

	while (pdp->first != pdp->last && pdp->pst != FFT) {

		/*
		 * Get the next element from the fifo buffer.
		 */
		if (++(pdp->first) > pdp->fifoend) {
			pdp->first = pdp->fifovect;
		}
		tmp = *(pdp->first);
		y = (tmp >> 10) & TENBITS;

		/*
		 * Continue up.
		 */
		if ((tmp & UPBIT) && (pix_d)y > wsp->avy_orig) {

			/*
			 * We are not outside the window.
			 */
			scanshadow(tmp - (1 << 10), UP);
		}

		/*
		 * Continue down.
		 */
		if ((tmp & DOWNBIT) && (pix_d)y < wsp->avy_lrc) {

			/*
			 * We are not outside the window.
			 */
			scanshadow(tmp + (1 << 10), DOWN);
		}
	}

	/*
	 * Release the memory allocated for the fifo buffer.
	 */
	(void)Sbrk(-((SCXSIZE / (8 * sizeof(dword)) + 2) * sizeof(dword) + FIFOSIZE * sizeof(fifo)));

	return;
}	/* end of paint() */

#undef	Getbit



/*
 * Scan a pixel row in both x directions and put result in fifo buffer, while
 * filling those parts of the row which are inside the circumscribed area.
 */

#define		REGION		0	/* inside region to fill */
#define		BORDER		1	/* on border to region   */

void
scanshadow(element, dir)
fifo	element;	/* packed format of pixel row      */
int	dir;		/* scan direction (UP and/or DOWN) */
{
	register	dword	*wptr;		/* pointer to current long word */
	register struct	paintst	*pdp;		/* pointer to paint status      */
	register	dword	*firstw;	/* pointer to first long word
						   of pixel row to put in fifo  */
	register	dword	*stwp;		/* save of pointer to starting
						   long word                    */
	fifo			*lastsave;	/* save of fifo pointer         */
	register	dword	curlw;		/* currently handled long word  */
	register		bitnr;		/* bit number in current long
						   word                         */
	register		firstbit;	/* bit number in 'firstw'       */
	register		stopbit;	/* pixel beyond pixel row end
						   (from start of physical row) */
	register		state;		/* state (inside region or on
						   region border)                */
	int			xdir;		/* save of original scan dir.   */
	int			stbit;		/* save of starting bit #       */
	int			y;		/* y coord. of pixel row        */
	int			start;		/* start of found segment       */
	int			len;		/* length of found segment      */

	/*
	 * Initialize pointers.
	 */
	pdp = &pdata;

	/*
	 * Initialize some things.
	 */
	xdir = dir;			/* save original scan direction */
	stopbit = (element >> 20) & TENBITS;	/* extract length */
	y = (element >> 10) & TENBITS;		/* extract y coordinate */
	stopbit += (element &= TENBITS);/* first pixel beyond segment */
	stbit = bitnr = element & FIVEBITS;	/* extract bit # in dword */
	stwp = wptr = pdp->scanbuf + (element >> 5);
						/* starting dword */

	/*
	 * Set up the scan buffer.
	 */
	if (pdp->scany != y) {
		fixscanbuf(y);
	}

	/*
	 * Get first long word and shift first pixel to bit 0.
	 */
	curlw = *wptr;
	curlw >>= 31 - bitnr;
	if (curlw & 1) {
		state = BORDER;		/* the first pixel is on the border */
	}
	else {
		state = REGION;		/* the first pixel is inside region */

		/*
		 * Scan left until a limiting pixel. Start with the pixels in
		 * the initial long word.
		 */
		while ((curlw & 1) == 0) {
			if (bitnr != 0) {

				/*
				 * Shift next bit into test position.
				 */
				bitnr--;
				curlw >>= 1;
			}
			else {

				/*
				 * We just tested MSBit in long word.
				 */
				bitnr = 31;

				/*
				 * Now scan whole long words until non-zero.
				 */
				while ((curlw = *--wptr) == 0)
					;
			}
		}

		/*
		 * Current position is one bit to the left of the first bit
		 * to set; adjust it.
		 */
		firstbit = bitnr + 1;
		firstw = wptr;
		if (firstbit > 31) {
			firstbit = 0;
			firstw++;
		}

		/*
		 * Determine if both scan UP and DOWN.
		 */
		if (pdp->pst == 0 && (firstw < stwp || firstbit < stbit - 1)) {
			dir = UP | DOWN;
		}
	}

	/*
	 * Initialize for the scan to the right.
	 */
	wptr = stwp;
	bitnr = stbit;
	curlw = *wptr;
	curlw <<= bitnr;

	/*
	 * Scan to the right and put each element which is inside the region
	 * in the fifo buffer.
	 */
	do {
		if (state == REGION) {

			/*
			 * We are inside the region.
			 */
			while ((curlw & (1 << 31)) == 0) {
				if (bitnr != 31) {

					/*
					 * Shift next bit into test position.
					 */
					bitnr++;
					curlw <<= 1;
				}
				else {

					/*
					 * We just tested LSBit.
					 */
					bitnr = 0;
					while ((curlw = *++wptr) == 0)
						;
				}
			}

			/*
			 * Determine if both scan UP and DOWN needed.
			 */
			if (pdp->pst == 0 && ((wptr - pdp->scanbuf) << 5) + bitnr > stopbit) {
				dir = UP | DOWN;
			}

			/*
			 * We now have a segment to put in the fifo buffer.
			 */
			start = (y << 10) + ((firstw - pdp->scanbuf) << 5) + firstbit;
			len = ((wptr - firstw) << 5) + bitnr - firstbit;
			if (len > 0) {
				lastsave = pdp->last;	/* save pointer */
				if (++(pdp->last) > pdp->fifoend) {

					/*
					 * Fifo wrap around.
					 */
					pdp->last = pdp->fifovect;
				}
				if (pdp->last == pdp->first) {

					/*
					 * Fifo full, terminate.
					 */
					pdp->last = lastsave;	/* restore */
					pdp->pst = FFT;
				}
				else {

					/*
					 * Fifo not full, fill the pixel row
					 * and insert in buffer.
					 */
					fillshd((*(pdp->last) =
					        (fifo)((dir << 30) | (len << 20) | start)));
				}
			}
		}	/* if (state == REGION) */

		/*
		 * If not beyond the stop bit, scan until we come inside the
		 * region again (if ever).
		 */
		if (((wptr - pdp->scanbuf) << 5) + bitnr < stopbit) {
			state = REGION;
			while (curlw & (1 << 31)) {
				if (bitnr != 31) {

					/*
					 * Shift next bit into test position.
					 */
					bitnr++;
					curlw <<= 1;
				}
				else {

					/*
					 * We just tested LSBit.
					 */
					bitnr = 0;
					while ((curlw = *++wptr) == ~0)
						;
				}
			}

			/*
			 * Save start of new segment.
			 */
			firstw = wptr;
			firstbit = bitnr;
			dir = xdir;	/* restore original scan direction */
		}
	} while (((wptr - pdp->scanbuf) << 5) + bitnr < stopbit);

	return;
}	/* end of scanshadow() */



/*
 * Fill one continuous pixel row.
 */

void
fillshd(shdw)
register fifo	shdw;		/* packed form of pixel row to fill */
{
	register		 x;	/* x coordinate of row to fill   */
	register		 y;	/* y coordinate of row to fill   */
	register	word	 vic;	/* vertical bit position mask    */
	register		 step;	/* shift count                   */
	register		 len;	/* length of pixel row minus one */
	register struct	hwastruc *hwap;	/* pointer to hardware addresses */
	register struct	paintst	 *pdp;	/* pointer to paint data         */

	/*
	 * Initialize some pointers.
	 */
	pdp = &pdata;
	hwap = &hwa;

	/*
	 * Mask out some values from the packed data.
	 */
	len = ((shdw >> 20) & TENBITS) - 1;	/* # of bits minus 1 */
	if (len < 0) {
		return;				/* nothing to set */
	}
	x = shdw & TENBITS;		/* x coordinate of left pixel */
	y = (shdw >> 10) & TENBITS;	/* y coordinate */

	/*
	 * Set up the size and destination for the mover.
	 */
	Waitsp();
	Outb(pdp->p_stflg, hwap->mflagport);
	Outw(len | UX_UY, hwap->x_sizeport);
	Outw(0, hwap->y_sizeport);
	Outw(x, hwap->x_toport);
	Outw(y, hwap->y_toport);

	if (pdp->pst == 0) {

		/*
		 * Do not use any pattern, just fill.
		 */
		Outl((FILLXCOORD << 16) | FILLYCOORD, hwap->x_fromport);
	}
	else {

		/*
		 * Phase in the pattern.
		 */
		if (shdw & UPBIT) {
			vic = pdp->upvic;
			step = pdp->upshift;
			if (pdp->cyup != y) {
				if ((vic & pdp->vmask) || (pdp->gop & 4) == 0) {
					pdp->upshift = step -= pdp->shift;
				}
				vic >>= 1;
				if (vic == (word)0) {
					vic = (1 << 15);
				}
				pdp->upvic = vic;
				pdp->cyup = y;
			}
		}
		else {
			vic = pdp->dovic;
			step = pdp->doshift;
			if (pdp->cydo != y) {
				if ((vic & pdp->vmask) || (pdp->gop & 4) == 0) {
					pdp->doshift = step += pdp->shift;
				}
				vic <<= 1;
				if (vic == (word)0) {
					vic = 1;
				}
				pdp->dovic = vic;
				pdp->cydo = y;
			}
		}

		/*
		 * Now fill with the desired pattern.
		 */
		if (vic & pdp->vmask) {

			/*
			 * Fill using pattern.
			 */
			Outw(GRPHXCOORD + ((step + x - pdp->xs) & FOURBITS), hwap->x_fromport);
			Outw(GRPHYCOORD, hwap->y_fromport);
		}
		else {
			switch (pdp->gop) {

			case 0:
			case 4:		/* clear the pixel row */
				Outl((CLRXCOORD << 16) | CLRYCOORD, hwap->x_fromport);
				break;

			case 1:
			case 5:		/* set the pixel row */
				Outl((FILLXCOORD << 16) | FILLYCOORD, hwap->x_fromport);
				break;

			case 2:
			case 6:		/* fill with complemented mask */
				Outb(pdp->p_stflg ^ COMPMFL, hwap->mflagport);
				Outw(GRPHXCOORD + ((step + x - pdp->xs) & FOURBITS), hwap->x_fromport);
				Outw(GRPHYCOORD, hwap->y_fromport);
				break;
			}
		}
	}

	return;
}	/* end of fillshd() */



/*
 * This function copies a whole pixel row (from the screen) to the scan buffer
 * (used by the scanshadow() function).
 * Those pixels which are outside the window or non-visible are set so they
 * will act as a limiting border when scanned.
 */

void
fixscanbuf(y)
register	y;		/* y coordinate of pixel row */
{
	register	byte	 *p1;	/* pointer for various things    */
	register	byte	 *p2;	/* pointer for various things    */
	register struct	wpstruc	 *wrp;	/* pointer to visible rectangles */
	register struct	hwastruc *hwap;	/* pointer to hardware addresses */
	register		 tmp;	/* temporary                     */
	register		 n;	/* counter                       */
	register		 x1;	/* left side of rectangle        */
	register		 x2;	/* right side of rectangle       */

	hwap = &hwa;
	(struct paintst *)p1 = &pdata;
	((struct paintst *)p1)->scany = y;	/* pixel row in scan buffer */

	/*
	 * Construct the pixel row in a set up buffer in the non-visible area
	 * of the graphic memory. First set all the pixels in the area and then
	 * copy the visible parts of the whole pixel row (indicated by 'y') to
	 * their corresponding place in the set up area.
	 */
	Waitsp();
	Outb(GSTATFLG | HOLDYFL | HOLDXFL, hwap->mflagport);
	Movel((FILLXCOORD << 16) | FILLYCOORD, (XPAINTBUF << 16) | YPAINTBUF,
	      (((PICTSIZE - SCXSIZE - 1) | UX_UY) << 16) | 2);

	wrp = ((struct paintst *)p1)->p_wrp;
	n = ((struct paintst *)p1)->p_rects;

	/*
	 * If the given starting pixel was set, invert while moving.
	 */
	Waitsp();
	if (((struct paintst *)p1)->limitpixel != 0) {
		Outb(GSTATFLG | COMPMFL, hwap->mflagport);
	}
	else {
		Outb(GSTATFLG, hwap->mflagport);
	}

	while (--n >= 0) {
		if ((wrp->bflags & B_NOTEMPTY) && (pix_d)y >= wrp->yuv_p && (pix_d)y <= wrp->ylv_p) {

			/*
			 * This rectangle contains the pixel row.
			 */
			x1 = wrp->xlv_p;
			x2 = wrp->xrv_p;
			if (x1 < PICTSIZE - SCXSIZE) {
				tmp = x2;
				if (x2 >= PICTSIZE - SCXSIZE) {
					tmp = PICTSIZE - SCXSIZE - 1;
				}
				Move(x1, y, XPAINTBUF + x1, YPAINTBUF, tmp - x1, 0, UX_UY);
				Waitsp();
				x1 = PICTSIZE - SCXSIZE;
			}
			if (x1 <= x2 && x1 < 2 * (PICTSIZE - SCXSIZE)) {
				tmp = x2;
				if (x2 >= 2 * (PICTSIZE - SCXSIZE)) {
					tmp = 2 * (PICTSIZE - SCXSIZE) - 1;
				}
				Move(x1, y, XPAINTBUF - (PICTSIZE - SCXSIZE) + x1,
				     YPAINTBUF + 1, tmp - x1, 0, UX_UY);
				Waitsp();
				x1 = 2 * (PICTSIZE - SCXSIZE);
			}
			if (x1 <= x2) {
				Move(x1, y, XPAINTBUF - 2 * (PICTSIZE - SCXSIZE) + x1,
				     YPAINTBUF + 2, x2 - x1, 0, UX_UY);
				Waitsp();
			}
		}

		wrp++;		/* next rectangle */
	}

	/*
	 * Now copy the data to the scan buffer.
	 */
	(dword *)p1 = ((struct paintst *)p1)->scanbuf;
	(dword *)p2 = (dword *)(hwap->pictmemst + YPAINTBUF * PICTSIZE / 8 + XPAINTBUF / 8);
	tmp = 3;
	do {
		n = (PICTSIZE - SCXSIZE) / (8 * sizeof(dword));
		do {
			*((dword *)p1)++ = *((dword *)p2)++;
		} while (--n != 0);
		(dword *)p2 += SCXSIZE / (8 * sizeof(dword));
	} while (--tmp != 0);

	return;
}	/* end of fixscanbuf() */
