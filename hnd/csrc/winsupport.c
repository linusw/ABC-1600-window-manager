/* winsupport.c */

/*
 * This file contains routines to support some I/O control commands to the
 * window handler.
 */

/* 1984-12-10, Peter Andersson, Luxor Datorer AB */

/*
 * Functions in this file:
 *	wzoom()
 *	getvisible()
 *	turn32x32()
 *	chmpnt()
 *	chbgpatt()
 *	invvideo()
 *	invbgpatt()
 *	invmpnt()
 *	scr_restore()
 *	pictrd()
 *	gettext()
 *	usrdbox()
 */


#include	"../hsrc/defs.h"
#include	"../hsrc/structs.h"
#include	"../hsrc/eglobals.h"
#include	"../hsrc/graphconst.h"
#include	"../hsrc/macros.h"
#include	"../hsrc/dnixmacros.h"
#include	<dnix/ireq.h>
#include	<dnix/errno.h>
#include	<dnix/fcodes.h>
#include	<signal.h>


/*
 * Structure containing a received request.
 */

extern	struct	ireq	rqst;



/*
 * Set up a new zoom list. Returns an appropriate value to terminate the
 * request.
 */

int
wzoom()
{
	static	 struct	zoomlst	zoombuf;	/* buffer to read structure */
	register struct	winint	*wsp;		/* pointer to window status */
	register struct	zoomlst	*zp;		/* pointer to zoombuf       */
	register	char	*cp1;
	register	char	*cp2;
	register		i;		/* loop index               */

	zp = &zoombuf;

	/*
	 * Read the structure with the new zoom list from the user's buffer.
	 */
	if ((i = ureadbuf(rqst.ir_rn, (char *)zp, sizeof(struct zoomlst), 0)) < 0) {
		return(i);	/* we failed */
	}
#ifdef LANDSCAPE
	if ((zp->z_flags & Z_LMODE) == 0) {
		zp->z_rstat = WE_ILMOD;		/* not given for landscape mode */
	}
#else  PORTRAIT
	if ((zp->z_flags & Z_PMODE) == 0) {
		zp->z_rstat = WE_ILMOD;		/* not given for portrait mode */
	}
#endif LANDSCAPE / PORTRAIT
	else {
		zp->z_rstat = W_OK;

		/*
		 * Check that the list contains legal fonts.
		 */
#ifdef LANDSCAPE
		cp1 = cp2 = &zp->zl_list[0];
#else  PORTRAIT
		cp1 = cp2 = &zp->zp_list[0];
#endif LANDSCAPE / PORTRAIT
		i = ZOOMSIZE;
		while (*cp1 != '\0' && --i >= 0) {
			if (*cp1 < 'A' || *cp1 > 'Z') {
				zp->z_rstat = WE_ILPARA;
				break;
			}
			cp1++;
		}

		/*
		 * If the fonts were legal, copy the list to the window
		 * status structure.
		 */
		if (zp->z_rstat == W_OK) {
			wsp = wstatp[rqst.ir_opid];
			wsp->zoomindex = 0;
			cp1 = &wsp->zoomlist[0];
			*cp1++ = wsp->font;
			i = ZOOMSIZE;
			while (*cp2 != '\0' && --i >= 0) {
				*cp1++ = *cp2++;
			}
			*cp1 = '\0';
		}
	}

	/*
	 * Write the structure back to the user's buffer.
	 */
	return(uwritbuf(rqst.ir_rn, (char *)zp, sizeof(struct zoomlst), 0));
}	/* end of wzoom() */



/*
 * Function to return the visible parts of a virtual screen or the background.
 */

int
getvisible()
{
	register struct	wpstruc	*wrp;		/* pointer to rectangle group       */
	register struct	winint	*wsp;		/* pointer to window status         */
	register struct	rectdes	*rectp;		/* pointer to a computed rectangle  */
	register struct	ireq	*rqstp;		/* pointer to request               */
	register		n;		/* number of rectangles in nwpart[] */
	register		n2;		/* real number of rectangles        */
	register		wn;		/* window number                    */
	struct		visdes	vb;		/* parameter structure              */

	rqstp = &rqst;
	wn = rqstp->ir_opid;

	/*
	 * Read the parameter structure from the user's buffer.
	 */
	if (ureadbuf(rqstp->ir_rn, (char *)(&vb), sizeof(struct visdes), 0) < 0) {
		return(EFAULT);
	}
	if (wn == MAXWINS || ((wsp = wstatp[wn])->status & CREATED)) {
		n = nwpart[wn];
		n2 = 0;
		wrp = wpp[wn];
		rectp = &ubuf.rectbuf[0];

		/*
		 * Fill in all the visible rectangles.
		 */
		while (--n >= 0) {
			if (wn == MAXWINS) {
#ifdef LANDSCAPE
				rectp->r_xorig = wrp->yu_p;
				rectp->r_yorig = wrp->xl_p;
				rectp->r_xsize = wrp->yl_p - wrp->yu_p + 1;
				(rectp++)->r_ysize = wrp->xr_p - wrp->xl_p + 1;
#else  PORTRAIT
				rectp->r_xorig = wrp->xl_p;
				rectp->r_yorig = (SCYSIZE - 1) - wrp->yl_p;
				rectp->r_xsize = wrp->xr_p - wrp->xl_p + 1;
				(rectp++)->r_ysize = wrp->yl_p - wrp->yu_p + 1;
#endif LANDSCAPE / PORTRAIT
				n2++;
			}
			else if (wrp->bflags & B_NOTEMPTY) {
#ifdef LANDSCAPE
				rectp->r_xorig = wrp->yuv_p - wsp->y_orig;
				rectp->r_yorig = wrp->xlv_p - wsp->x_orig;
				rectp->r_xsize = wrp->ylv_p - wrp->yuv_p + 1;
				(rectp++)->r_ysize = wrp->xrv_p - wrp->xlv_p + 1;
#else  PORTRAIT
				rectp->r_xorig = wrp->xlv_p - wsp->x_orig;
				rectp->r_yorig = wsp->y_orig + wsp->y_size - 1 - wrp->ylv_p;
				rectp->r_xsize = wrp->xrv_p - wrp->xlv_p + 1;
				(rectp++)->r_ysize = wrp->ylv_p - wrp->yuv_p + 1;
#endif LANDSCAPE / PORTRAIT
				n2++;
			}
			wrp++;
		}

		/*
		 * Write the rectangles to the user's buffer if there is enough
		 * space.
		 */
		if (vb.v_nrect < n2) {
			vb.v_rstat = WE_SPACE;
		}
		else {
			vb.v_rstat = W_OK;

			/*
			 * Write the rectangles to the user's buffer.
			 */
			if (uwritbuf(rqstp->ir_rn, (char *)(&ubuf.rectbuf[0]), n2 * sizeof(struct rectdes),
			    sizeof(struct visdes)) < 0) {
				return(EFAULT);
			}
		}
		vb.v_nrect = n2;
	}
	else {
		vb.v_rstat = WE_NOTCR;		/* window not created */
	}

	/*
	 * Write the parameter structure back to the user's buffer.
	 */
	if (uwritbuf(rqstp->ir_rn, (char *)(&vb), sizeof(struct visdes), 0) < 0) {
		return(EFAULT);
	}

	return(0);
}	/* end of getvisible() */



#ifdef LANDSCAPE

/*
 * Turn a bit 32x32 pixels bit pattern 90 degrees clockwise.
 */

void
turn32x32(source, dest)
dword	*source;	/* pointer to bit pattern to turn    */
dword	*dest;		/* destination of turned bit pattern */
{
	register dword	*s1;	/* pointer to source             */
	register dword	*d1;	/* pointer to destination        */
	register dword	mask1;	/* mask when turning bit pattern */
	register dword	mask2;	/* mask when turning bit pattern */
	register dword	work;	/* work variable                 */

	d1 = dest + 8 * sizeof(dword);
	for (mask1 = 1 ; mask1 != 0 ; mask1 <<= 1) {
		work = 0;
		s1 = source;
		for (mask2 = 1 ; mask2 != 0 ; mask2 <<= 1) {
			if (*s1++ & mask1) {
				work |= mask2;
			}
		}
		*--d1 = work;
	}

	return;
}	/* end of turn32x32() */

#endif LANDSCAPE



/*
 * Get a new mouse pointer for a window or a new global mouse pointer.
 */

int
chmpnt(mpp)
register struct	mpstruc	*mpp;	/* pointer to destination structure */
{
	register struct	npstruc	*npp;	/* pointer to source structure */
	register		i;	/* loop index                  */
#ifdef LANDSCAPE
	register	dword	*p1;	/* used to fix the mouse       */
	register	dword	*p2;	/* pointer for landscape mode  */
	dword			*p3;
	dword			*p4;
#endif LANDSCAPE

	npp = &ubuf.npbuf;

	/*
	 * Check if the values are legal.
	 */
	if (npp->np_xsize <= 0 || npp->np_xsize > MPSIZE ||
	    npp->np_ysize <= 0 || npp->np_ysize > MPSIZE ||
	    npp->np_xpnt < 0 || npp->np_xpnt >= npp->np_xsize ||
	    npp->np_ypnt < 0 || npp->np_ypnt >= npp->np_ysize) {
		return(WE_ILPARA);	/* illegal parameter */
	}

	/*
	 * The pointer is ok, fill in the mouse data structure.
	 */
#ifdef LANDSCAPE
	mpp->mp_xsize = npp->np_ysize - 1;
	mpp->mp_ysize = npp->np_xsize - 1;
	mpp->mp_xpnt = mpp->mp_xsize - npp->np_ypnt;
	mpp->mp_ypnt = npp->np_xpnt;

	/*
	 * Move the visible part of the mouse pointer to the lower side (i.e.
	 * to higher memory) of the arrays to hold it.
	 */
	i = npp->np_ysize;
	p1 = &npp->np_and[MPSIZE];
	p2 = &npp->np_and[i];
	p3 = &npp->np_or[MPSIZE];
	p4 = &npp->np_or[i];
	do {
		*--p1 = *--p2;
		*--p3 = *--p4;
	} while (--i != 0);

	/*
	 * Turn the bit pattern 90 degrees clockwise.
	 */
	turn32x32(&(npp->np_and[0]), &(mpp->mp_and[0]));
	turn32x32(&(npp->np_or[0]), &(mpp->mp_or[0]));
#else  PORTRAIT
	mpp->mp_xsize = npp->np_xsize - 1;
	mpp->mp_ysize = npp->np_ysize - 1;
	mpp->mp_xpnt = npp->np_xpnt;
	mpp->mp_ypnt = npp->np_ypnt;
	for (i = 0 ; i < MPSIZE ; i++) {
		mpp->mp_and[i] = npp->np_and[i];
		mpp->mp_or[i] = npp->np_or[i];
	}
#endif LANDSCAPE / PORTRAIT
	if (inversevideo != NO) {
		invmpnt(mpp);
	}

	/*
	 * Show the new mouse pointer if appropriate.
	 */
	offblnk();		/* stop text cursor blink */
	mp_off();
	mp_on();
	onblnk();		/* enable text cursor blink again */

	return(W_OK);		/* OK */
}	/* end of chmpnt() */



/*
 * Change the background pattern.
 */

void
chbgpatt()
{
	register		i;	/* loop index              */
	register		j;	/* loop index              */
	register	word	x;	/* to copy bit pattern     */
	register		bitcnt;	/* # of set bits           */
	register	word	*addr;	/* address to pattern save */
	register struct	wpstruc	*wrp;	/* pointer to rectangles   */
#ifdef LANDSCAPE
	register	word	mask1;	/* to turn pattern         */
	register	word	mask2;
#endif LANDSCAPE

	addr = (word *)(hwa.pictmemst + (BGPYCOORD * PICTSIZE + BGPXCOORD) / 8);
	bitcnt = 0;

	/*
	 * Stop text cursor blink.
	 */
	offblnk();

	Waits();
#ifdef LANDSCAPE

	/*
	 * Turn the pattern 90 degrees clockwise.
	 */
	mask1 = (1 << (BGPSIZE - 1));
	i = BGPSIZE;
	do {
		x = 0;
		mask2 = 1;
		for (j = 0 ; j < BGPSIZE ; j++) {
			if (ubuf.cbbuf.cb_bitmap[j] & mask1) {
				x |= mask2;
				bitcnt++;	/* one more set bit */
			}
			mask2 <<= 1;
		}
		mask1 >>= 1;
		Outw(x, addr);
		Outw(x, addr + 1);
		Outw(x, addr + BGPSIZE * (PICTSIZE / (8 * sizeof(word))));
		Outw(x, addr + BGPSIZE * (PICTSIZE / (8 * sizeof(word))) + 1);
		addr += PICTSIZE / (8 * sizeof(word));
	} while (--i != 0);
#else  PORTRAIT
	for (i = 0 ; i < BGPSIZE ; i++) {
		x = ubuf.cbbuf.cb_bitmap[i];
		Outw(x, addr);
		Outw(x, addr + 1);
		Outw(x, addr + BGPSIZE * (PICTSIZE / (8 * sizeof(word))));
		Outw(x, addr + BGPSIZE * (PICTSIZE / (8 * sizeof(word))) + 1);
		addr += PICTSIZE / (8 * sizeof(word));

		/*
		 * Count the number of set bits.
		 */
		j = 8 * sizeof(word);
		do {
			if (x & 1) {
				bitcnt++;
			}
			x >>= 1;
		} while (--j != 0);
	}
#endif LANDSCAPE / PORTRAIT

	/*
	 * Set the pattern to use when drawing window contours on the
	 * background.
	 */
	if (bitcnt >= BGPSIZE * sizeof(word) * 8 / 2) {
		bg_cont = BLACK;
	}
	else {
		bg_cont = WHITE;
	}
	if (inversevideo == YES) {
		invbgpatt();
	}

	/*
	 * Now redraw the background with the new pattern.
	 */
	wrp = wpp[MAXWINS];
	i = nwpart[MAXWINS];
	mp_off();
	while (--i >= 0) {
		sdrawbg(wrp++, PATTERN);
	}
	mp_on();

	/*
	 * Enable text cursor blink again.
	 */
	onblnk();

	return;
}	/* end of chbgpatt() */



/*
 * Change from normal video to inverse video or vice versa.
 */

void
invvideo(flag)
register	flag;	/* NO = normal, YES = inverse video */
{
	register		 i;	/* loop index and temporary      */
	register	dword	 x;	/* to invert the grey pattern    */
	register struct	winint	 *wsp;	/* pointer to window status      */
	register struct	hwastruc *hwap;	/* pointer to hardware addresses */

	if (inversevideo != flag) {

		/*
		 * We have to change from normal to inverse video or vice
		 * versa.
		 */
		inversevideo = flag;
		i = MAXWINS - 1;
		do {
			wsp = wstatp[i];
			if (wsp != NULL && (wsp->status & CREATED)) {
				wsp->statflg ^= COMPMFL;
				wsp->chstatflg ^= COMPMFL;
				if (wsp->estatus & ALTMPNT) {
					invmpnt(wsp->mppnt);
				}
			}
		} while (--i >= 0);

		/*
		 * Invert the whole screen.
		 */
		offblnk();		/* stop text cursor blink */
		hwap = &hwa;
		mp_off();		/* remove mouse pointer */
		Waitsp();
		Outb(GSTATFLG | COMPMFL, hwap->mflagport);
		Move(0, 0, 0, 0, SCXSIZE - 1, SCYSIZE - 1, UX_UY);
/*		Movel(0, 0, (((SCXSIZE - 1) | UX_UY) << 16) | (SCYSIZE - 1)); Don't work, why */

		/*
		 * Adjust data for the background.
		 */
		invbgpatt();

		/*
		 * Adjust data for the global mouse pointer.
		 */
		invmpnt(&mpdat);
		mp_on();	/* restore mouse pointer */
		onblnk();	/* enable text cursor blink again */
	}

	return;
}	/* end of invvideo() */



/*
 * Invert the pattern in the background pattern save area.
 */

void
invbgpatt()
{
	register dword	*addr;		/* pointer to save area */
	register	i;		/* loop index           */

	if (bg_cont == BLACK) {
		bg_cont = WHITE;
	}
	else {
		bg_cont = BLACK;
	}
	i = BGPYSIZE;
	addr = (dword *)(hwa.pictmemst + (BGPYCOORD * PICTSIZE + BGPXCOORD) / 8);
	Waits();
	do {
		Outl(~Inl(addr), addr);
		addr += PICTSIZE / (8 * sizeof(dword));
	} while (--i != 0);

	return;
}	/* end of invbgpatt() */



/*
 * Invert the bit pattern for a mouse pointer.
 */

void
invmpnt(mpp)
struct	mpstruc	*mpp;	/* pointer to mouse pointer data */
{
	register dword	x;	/* to invert mouse pointer */
	register	i;	/* loop index              */
	register dword	*orp;
	register dword	*andp;

	orp = &mpp->mp_or[0];
	andp = &mpp->mp_and[0];
	i = MPSIZE;
	do {
		x = ~*andp;
		*andp++ = ~*orp;
		*orp++ = x;
	} while (--i != 0);

	return;
}	/* end of invmpnt() */



/*
 * Restore the whole screen, i.e. rewrite the screen.
 */

void
scr_restore()
{
	register struct	winint	*wsp;	/* pointer to window status */
	register struct	wpstruc	*wrp1;	/* pointer to rectangles    */
	register		n1;	/* # of rectangles          */
	register		n2;	/* # of rectangles          */
	register		wn;	/* window number            */
	register		npart;	/* # of non-empty rects.    */

	/*
	 * Stop text cursor blink.
	 */
	offblnk();

	/*
	 * Remove the cursor (in order to keep counter up to date).
	 */
	cursoff();

	/*
	 * Put the console cursor at the home position.
	 */
	conwrite("\033[H", 3);

	/*
	 * Start by filling the whole screen with the current background.
	 */
	wrp1 = &ubuf.wpt[0];
	wrp1->xl_p = 0;
	wrp1->xr_p = SCXSIZE - 1;
	wrp1->yu_p = 0;
	wrp1->yl_p = SCYSIZE - 1;
	sdrawbg(wrp1, PATTERN);

	/*
	 * Redraw all the windows.
	 */
	for (wn = 0 ; wn < MAXWINS ; wn++) {
		if ((wsp = wstatp[wn]) != NULL && (wsp->status & CREATED)) {
			n1 = n2 = nwpart[wn];
			wrp1 = wpp[wn];
			while (--n1 >= 0) {
				clrrect(wn, wrp1++);
			}
			disphdr(wn, ON);
			dispbox(wn);
			npart = 0;
			while (--n2 >= 0) {
				wrp1--;
				if (wrp1->bflags & B_NOTEMPTY) {
					npart++;
					sav_rewrit(wsp, wrp1);
				}
			}
			if (npart != 0) {
				redrawsig(wsp);		/* signal the change */
			}
		}
	}

	/*
	 * Turn on the cursor, invert a possible icon, and show the mouse
	 * pointer.
	 */
	curson();
	chkmp_on();
	iconchk();

	/*
	 * Enable text cursor blink again.
	 */
	onblnk();

	return;
}	/* end of scr_restore() */



/*
 * Function to read the picture memory for a window or the whole screen.
 * An appropriate value to terminate the request is returned.
 */

/*
 * Macro to get the value of a bit in the picture memory.
 */
#define	Getbit(x, y)	((Inb(hwa.pictmemst + y * PICTSIZE / 8 + (x >> 3)) >> (7 - (x & 7))) & 1)

/*
 * Macro to set a bit in the destination buffer.
 * This is a little bit clumpsy, but it was necessary to do it like this to
 * avoid compiler error when the Microsoft compiler was used.
 */
#define	Setbit(dest)	{ byte	*tmp; \
			  tmp = &ubp->bitmap[dest>>3]; \
			  *tmp = *tmp | (1 << (7 - (dest & 7))); }

int
pictrd()
{
	register struct	ireq	*rqstp;		/* pointer to request            */
	register union	buffer	*ubp;		/* pointer to copy buffer        */
	register struct	winint	*wsp;		/* pointer to window status      */
	register struct	wpstruc	*wrp;		/* pointer to rectangles         */
	register		wn;		/* window number                 */
	register		x;		/* current pixel, x coordinate   */
	register		y;		/* current pixel, y coordinate   */
	register		xend;		/* end pixel coordinate, x       */
	register		yend;		/* end pixel coordinate, y       */
	register	long	dest;		/* destination bit no.           */
	int			bitval;		/* value of set pixel            */
	int			offset;		/* offset into the user's buffer */
#ifdef LANDSCAPE
	int			ysave;		/* save of starting y coordinate */
#else  PORTRAIT
	int			xsave;		/* save of starting x coordinate */
#endif LANDSCAPE / PORTRAIT
	struct		wpstruc	*fndrect();

	rqstp = &rqst;
	ubp = &ubuf;
	wn = rqstp->ir_opid;

	/*
	 * Check if there is any bitmap to read.
	 */
	if (wn != MAXWINS && ((wsp = wstatp[wn])->status & CREATED) == 0) {
		return(ENXIO);
	}

	/*
	 * Read data describing the area to read.
	 */
	if (ureadbuf(rqstp->ir_rn, (char *)(&ubp->pblbuf), sizeof(struct wpictblk), 0) < 0) {
		return(EFAULT);
	}
#ifdef LANDSCAPE
	x = ubp->pblbuf.p_yaddr;
	y = ubp->pblbuf.p_xaddr;
#else  PORTRAIT
	x = ubp->pblbuf.p_xaddr;
	y = ubp->pblbuf.p_yaddr;
#endif LANDSCAPE / PORTRAIT
	if (x < 0 || y < 0 || ubp->pblbuf.p_width <= 0 || ubp->pblbuf.p_height <= 0) {
		return(ENXIO);		/* illegal parameters */
	}
	if (wn == MAXWINS) {
#ifdef LANDSCAPE
		xend = x + ubp->pblbuf.p_height - 1;
		yend = y + ubp->pblbuf.p_width - 1;
		if (yend >= SCYSIZE || xend >= SCXSIZE) {
			return(ENXIO);		/* illegal parameters */
		}
#else  PORTRAIT
		xend = x + ubp->pblbuf.p_width - 1;
		y = (SCYSIZE - 1) - y;
		yend = y - ubp->pblbuf.p_height + 1;
		if (yend < 0 || xend >= SCXSIZE) {
			return(ENXIO);		/* illegal parameters */
		}
#endif LANDSCAPE / PORTRAIT
		bitval = 1;

		/*
		 * Stop text cursor blink.
		 */
		offblnk();
	}
	else {
#ifdef LANDSCAPE
		if ((pix_d)(x + ubp->pblbuf.p_height) > wsp->x_size ||
		    (pix_d)(y + ubp->pblbuf.p_width) > wsp->y_size) {
			return(ENXIO);		/* illegal parameters */
		}
		x += wsp->x_orig;
		xend = x + ubp->pblbuf.p_height - 1;
		y += wsp->y_orig;
		yend = y + ubp->pblbuf.p_width - 1;
#else  PORTRAIT
		if ((pix_d)(x + ubp->pblbuf.p_width) > wsp->x_size ||
		    (pix_d)(y + ubp->pblbuf.p_height) > wsp->y_size) {
			return(ENXIO);		/* illegal parameters */
		}
		x += wsp->x_orig;
		xend = x + ubp->pblbuf.p_width - 1;
		y = (wsp->y_size - 1) - y + wsp->y_orig;
		yend = y - ubp->pblbuf.p_height + 1;
#endif LANDSCAPE / PORTRAIT
		if (wsp->statflg & COMPMFL) {
			bitval = 0;
		}
		else {
			bitval = 1;
		}

		/*
		 * Stop text cursor blink.
		 */
		offblnk();

		/*
		 * Remove the mouse pointer.
		 */
		mp_off();
	}

	/*
	 * Now copy the bitmap to the user's memory.
	 */
#ifdef LANDSCAPE
	ysave = y;
#else  PORTRAIT
	xsave = x;
#endif LANDSCAPE / PORTRAIT
	offset = sizeof(struct wpictblk);	/* copy to behind the parameter structure */
	dest = (long)0;
	wrp = NULL;
	Waits();
#ifdef LANDSCAPE
	while (x <= xend) {
		while (y <= yend) {
#else  PORTRAIT
	while (y >= yend) {
		while (x <= xend) {
#endif LANDSCAPE / PORTRAIT

			/*
			 * If the copy buffer is full, copy to the user's buffer.
			 */
			if ((dest >> 3) >= BITBUFSIZE) {
				if (uwritbuf(rqstp->ir_rn, (char *)(&ubp->bitmap[0]),
				    BITBUFSIZE, offset) < 0) {
					if (wn != MAXWINS) {
						mp_on();
					}
					onblnk();	/* enable text cursor blink */
					return(EFAULT);
				}
				offset += BITBUFSIZE;
				dest = (long)0;
			}

			/*
			 * Clear the next byte in the destination buffer if
			 * we will start on a new byte.
			 */
			if ((dest & 7) == 0) {
				ubp->bitmap[dest>>3] = 0;
			}

			/*
			 * Find out if the next pixel is set and if so set the
			 * bit in the destination buffer.
			 */
			if (wn != MAXWINS && (wrp == NULL || wrp->xlv_p > (pix_d)x ||
			    wrp->xrv_p < (pix_d)x || wrp->yuv_p > (pix_d)y || wrp->ylv_p < (pix_d)y)) {
				wrp = fndrect(wn, x, y);
			}
			if ((wn == MAXWINS || wrp != NULL) && Getbit(x, y) == bitval) {
				Setbit(dest);
			}
#ifdef LANDSCAPE
			y++;		/* next pixel */
#else  PORTRAIT
			x++;		/* next pixel */
#endif LANDSCAPE / PORTRAIT
			dest++;		/* next destination pixel */
		}	/* while (x <= xend) or while (y <= yend) */

#ifdef LANDSCAPE
		y = ysave;
		x++;		/* next pixel row */
#else  PORTRAIT
		x = xsave;
		y--;		/* previous pixel row */
#endif LANDSCAPE / PORTRAIT
		dest += (long)7;
		dest &= (long)(~7);
	}	/* while (y >= yend) or while (x <= xend) */

	if (wn != MAXWINS) {
		mp_on();
	}

	/*
	 * Enable text cursor blink.
	 */
	onblnk();

	/*
	 * Write the last block to the user's buffer, if anything.
	 */
	if (dest > 0) {
		dest >>= 3;
		if (uwritbuf(rqstp->ir_rn, (char *)(&ubp->bitmap[0]), dest, offset) < 0) {
			return(EFAULT);
		}
	}

	return(0);		/* everything is ok */
}	/* end of pictrd() */

#undef	Getbit
#undef	Setbit



/*
 * Get the text contents of a window.
 * An appropriate value to terminate the request is returned.
 */

int
gettext()
{
	register struct	ireq	*rqstp;		/* pointer to request       */
	register union	buffer	*ubp;		/* pointer to copy buffer   */
	register struct	winint	*wsp;		/* pointer to window status */
	register unsigned char	*strs;		/* source to copy from      */
	register		row;		/* first row                */
	register		col;		/* first column             */
	register		rcnt;		/* number of rows           */
	register		ccnt;		/* number of columns        */
	register		offset;		/* offset when copying      */
	register		index;		/* index                    */
	int			tmp;		/* temporary                */

	rqstp = &rqst;
	ubp = &ubuf;
	offset = sizeof(struct txtstruc);

	/*
	 * Read the structure from the user's buffer.
	 */
	if (ureadbuf(rqstp->ir_rn, (char *)(&ubp->txtbuf), offset, 0) < 0) {
		return(EFAULT);
	}
	wsp = wstatp[rqstp->ir_opid];
	row = ubp->txtbuf.tx_row - 1;
	col = ubp->txtbuf.tx_col - 1;
	rcnt = ubp->txtbuf.tx_rcnt;
	ccnt = ubp->txtbuf.tx_ccnt;

	/*
	 * Check if it is ok to copy the text contents and that the parameters
	 * are legal.
	 */
	if ((wsp->estatus & SAVETEXT) == 0 || (wsp->status & GRAPHIC)) {
		ubp->txtbuf.tx_rstat = WE_TSAVE;
	}
	else if (row < 0 || col < 0 || rcnt <= 0 || ccnt <= 0 ||
	         row + rcnt > wsp->rows || col + ccnt > wsp->columns) {
		ubp->txtbuf.tx_rstat = WE_ILPARA;
	}
	else {
		ubp->txtbuf.tx_rstat = W_OK;	/* Ok */
	}

	/*
	 * Write the structure back to the user's buffer.
	 */
	if (uwritbuf(rqstp->ir_rn, (char *)(&ubp->txtbuf), offset, 0) < 0) {
		return(EFAULT);
	}

	if (ubp->txtbuf.tx_rstat == W_OK) {

		/*
		 * Copy the text contents to the user's buffer.
		 */
		index = 0;
		do {
			strs = *(wsp->savc + row) + col;
			tmp = ccnt;
			do {
				if (index >= TCPBUFSIZE) {
					if (uwritbuf(rqstp->ir_rn, &ubp->tcpbuf[0], index, offset) < 0) {
						return(EFAULT);
					}
					offset += index;
					index = 0;
				}
				ubp->tcpbuf[index++] = *strs++ & CMASK;
			} while (--tmp != 0);
			row++;		/* next row */
		} while (--rcnt != 0);

		/*
		 * Copy the last block to the user's buffer.
		 */
		if (uwritbuf(rqstp->ir_rn, &ubp->tcpbuf[0], index, offset) < 0) {
			return(EFAULT);
		}
	}

	return(0);
}	/* end of gettext() */



/*
 * Set up a new user defined box.
 * The box is displayed in the left border (if it fits).
 */

int
usrdbox(wn, udbp)
int		wn;		/* window number            */
struct	userbox	*udbp;		/* pointer to user box data */
{
	register struct	winint	*wsp;	/* pointer to window status       */
	register struct	uboxst	*uibp;	/* pointer to internal user box   */
	register	word	*p1;	/* to copy user box bitmap        */
	register	word	*p2;	/* to copy user box bitmap        */
	register		i;	/* counter                        */
#ifdef LANDSCAPE
	register	word	mask1;	/* mask when turning pattern      */
	register	word	mask2;	/* mask when turning pattern      */
	register	word	x;	/* work area when turning pattern */
#endif LANDSCAPE

	wsp = wstatp[wn];
	if ((wsp->status & CREATED) == 0) {
		return(WE_NOTCR);	/* window not created yet */
	}
	if (wsp->uboxcnt >= wsp->uboxmax) {
		return(WE_SPACE);	/* no entry left */
	}

	uibp = wsp->uboxp + wsp->uboxcnt;	/* point to next free entry */
	uibp->ub_sig = udbp->bx_sig;		/* get signal to send */
	if (uibp->ub_sig < 0 || uibp->ub_sig >= NSIG) {
		return(WE_ILPARA);	/* illegal signal number */
	}

	/*
	 * Copy the bitmap to the internal store area.
	 */
	p2 = &uibp->ub_bmap[0];
#ifdef LANDSCAPE

	/*
	 * For landscape mode we have to turn the bit pattern 90 degrees
	 * clockwise.
	 */
	for (mask1 = 1 << (UBOXSIZE - 1) ; mask1 != 0 ; mask1 >>= 1) {
		x = 0;
		p1 = &udbp->bx_bmap[0];
		for (mask2 = 1 ; mask2 != 0 ; mask2 <<= 1) {
			if (*p1++ & mask1) {
				x |= mask2;
			}
		}
		*p2++ = x;
	}
#else  PORTRAIT
	p1 = &udbp->bx_bmap[0];
	i = UBOXSIZE;
	do {
		*p2++ = *p1++;
	} while (--i != 0);
#endif LANDSCAPE / PORTRAIT

	/*
	 * Now display the box if it fits.
	 */
#ifdef LANDSCAPE
	if ((wsp->bordflags & BD_LEFT) && (wsp->bx_size - 2 * DLBWIDTH) / UBOXTSIZ > wsp->uboxcnt) {
#else  PORTRAIT
	if ((wsp->bordflags & BD_LEFT) && (wsp->by_size - 2 * DLBWIDTH) / UBOXTSIZ > wsp->uboxcnt) {
#endif LANDSCAPE / PORTRAIT

		/*
		 * The new box will fit.
		 */
		wsp->ubxcrrnt++;	/* one more visible box */
		p1 = (word *)(hwa.pictmemst + ((SETUPYCOORD + UBOXSIZE) * PICTSIZE + SETUPXCOORD) / 8);
		i = UBOXSIZE;

		/*
		 * Stop text cursor blink.
		 */
		offblnk();

		/*
		 * Remove the mouse pointer.
		 */
		mp_off();
		Waits();
		do {
			p1 -= PICTSIZE / (8 * sizeof(word));
			Outw(*--p2, p1);
		} while (--i != 0);

		/*
		 * Set up the mover flags and move out the box in the border.
		 */
		Outb(wsp->statflg, hwa.mflagport);
#ifdef LANDSCAPE
		i = wsp->abx_lrc - Muls(wsp->uboxcnt, UBOXTSIZ) - DLBWIDTH - (UBOXSIZE - 1);
		d1box(wn, i, UBOXSIZE - 1, wsp->aby_orig + BOXOFFS,
		      UBOXSIZE - 1, SETUPXCOORD, SETUPYCOORD);
#else  PORTRAIT
		i = wsp->aby_orig + Muls(wsp->uboxcnt, UBOXTSIZ) + DLBWIDTH;
		d1box(wn, wsp->abx_orig + BOXOFFS, UBOXSIZE - 1, i,
		      UBOXSIZE - 1, SETUPXCOORD, SETUPYCOORD);
#endif LANDSCAPE / PORTRAIT

		/*
		 * Restore the mouse pointer.
		 */
		mp_on();

		/*
		 * Enable text cursor blink again.
		 */
		onblnk();
	}

	wsp->uboxcnt++;		/* one more user defined box */

	return(W_OK);
}	/* end of usrdbox() */
