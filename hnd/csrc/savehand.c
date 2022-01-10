/* savehand.c */

/*
 * This file contains routines to take care of the saving and updating of the
 * text contents of the virtual screens.
 */

/* 1984-08-16, Peter Andersson, Luxor Datorer AB (at DIAB) */

/*
 * Functions in this file:
 *	sav_alloc()
 *	allocmem()
 *	ckiconcnt()
 *	sav_free()
 *	sav_eln()
 *	sav_esc()
 *	sav_scroll()
 *	sav_rewrit()
 */


#include	"../hsrc/defs.h"
#include	"../hsrc/structs.h"
#include	"../hsrc/eglobals.h"
#include	"../hsrc/graphconst.h"
#include	"../hsrc/macros.h"
#include	"../hsrc/dnixmacros.h"
#include	<signal.h>
#include	<dnix/fcodes.h>



/*
 * Allocate space for the input and output character buffers, mouse pointer,
 * user boxes, and the text save area for a window. Space for text saving is
 * only allocated if the text save flag SAVETEXT in the status word for the
 * window is set.
 * Returns OK if it was possible to allocate space, otherwise ERROR.
 */

int
sav_alloc(wsp)
register struct	winint	*wsp;		/* pointer to window status         */
{
	register unsigned char	*cp;	/* character pointer                */
	register unsigned char	**cpp;	/* character pointer array          */
	register		i;	/* loop index                       */
	register		lines;	/* # of lines in the virtual screen */
	register		cols;	/* # of col. in the virtual screen  */
	register		ubspace;/* space to hold user defined boxes */
	char		  *allocmem();	/* memory allocator                 */

	ubspace = Muls(wsp->uboxmax, sizeof(struct uboxst));

	/*
	 * Always allocate space for the input character buffer (TTYHOG
	 * elements) and for the character output buffer (TTYHOG + 2 elements).
	 */
	i = TTYHOG + (TTYHOG + 2) + ubspace;
	if (wsp->estatus & ALTMPNT) {
		i += sizeof(struct mpstruc);	/* mouse pointer data */
	}
	if (wsp->estatus & SAVETEXT) {

		/*
		 * The text contents of the virtual screen shall be saved.
		 */
		lines = wsp->rows;
		cols = wsp->columns;
		i += lines * (sizeof(unsigned char *) + cols * sizeof(unsigned char));
	}

	/*
	 * Allocate the space.
	 */
	i = Div2(i + 1) * 2;
	if ((cp = (unsigned char *)allocmem(i)) == NULL) {
		return(ERROR);		/* can't allocate memory */
	}
	wsp->alloccnt = i;

	wsp->uboxp = (struct uboxst *)cp;	/* pointer to user boxes */
	cp += ubspace;
	if (wsp->estatus & ALTMPNT) {

		/*
		 * Mouse pointer data structure.
		 */
		wsp->mppnt = (struct mpstruc *)cp;

		/*
		 * Copy the global mouse pointer to the structure.
		 */
		((struct mpstruc *)cp)->mp_xsize = mpdat.mp_xsize;
		((struct mpstruc *)cp)->mp_ysize = mpdat.mp_ysize;
		((struct mpstruc *)cp)->mp_xpnt = mpdat.mp_xpnt;
		((struct mpstruc *)cp)->mp_ypnt = mpdat.mp_ypnt;
		for (i = 0 ; i < MPSIZE ; i++) {
			((struct mpstruc *)cp)->mp_and[i] = mpdat.mp_and[i];
			((struct mpstruc *)cp)->mp_or[i] = mpdat.mp_or[i];
		}
		cp += sizeof(struct mpstruc);
	}

	if (wsp->estatus & SAVETEXT) {

		/*
		 * Initialize the pointers to the start of the lines and fill
		 * the copy of the virtual screen with spaces.
		 */
		wsp->savc = cpp = (unsigned char **)cp;
		cp += lines * sizeof(unsigned char *);
		do {
			*cpp++ = cp;
			i = cols;
			do {
				*cp++ = ' ';
			} while (--i > 0);
		} while (--lines > 0);
	}	/* if (wsp->estatus & SAVETEXT) */

	/*
	 * Initialize the pointers to the input and output buffers.
	 */
	wsp->ttyd.t_inq.c_cb = cp;
	wsp->ttyd.t_inq.c_cf = cp;
	wsp->ttyd.t_inq.c_cl = cp;
	wsp->ttyd.t_outq.c_cb = cp + TTYHOG;

	return(OK);	/* OK */
}	/* end of sav_alloc() */



/*
 * Allocate 'size' more bytes of memory and update the 'mem_high' pointer.
 * A pointer to the allocated memory is returned.
 * This routine should only be used to allocate memory for window status
 * structures and text save areas, not for icons.
 * The icon data is moved to higher memory.
 * This routine should not be used to release memory.
 */

char *
allocmem(size)
register	size;		/* number of bytes to allocate */
{
	register char	*cp;	/* pointer to allocated memory */
	register char	*cp1;
	register char	*cp2;
	register	cnt;	/* number of bytes to move     */

	if ((char *)Sbrk(size) == NULL) {
		return(NULL);
	}
	cp = mem_high;
	mem_high += size;

	/*
	 * Move the icon data to higher memory.
	 */
	cnt = n_icons * sizeof(struct icondat);
	cp1 = (char *)iconsave + cnt;
	cp2 = cp1 + size;
	while (--cnt >= 0) {
		*--cp2 = *--cp1;
	}
	iconsave = (struct icondat *)cp2;

	return(cp);
}	/* end of allocmem() */



/*
 * Release memory if we have more than ICONCHUNK + ICONHYST free icon data
 * entries.
 */

void
ckiconcnt()
{
	int	cnt;

	cnt = tot_icons - n_icons;
	if (cnt > ICONCHUNK + ICONHYST) {

		/*
		 * Release memory.
		 */
		cnt /= ICONCHUNK;
		cnt *= ICONCHUNK;
		tot_icons -= cnt;
		(void)Sbrk(-(cnt * sizeof(struct icondat)));
	}

	return;
}	/* end of ckiconcnt() */



/*
 * Free the space allocated for a window.
 */

#define		WININTSIZE	(((sizeof(struct winint) + 1) / 2) * 2)

void
sav_free(wsp)
struct	winint	*wsp;		/* pointer to window status */
{
	register	char	*lp;	/* start of area to copy               */
	register	char	*mp;	/* the distance to copy increases here */
	register	char	*tp;	/* temporary used when copying         */
	register struct	winint	*wsp2;	/* window to adjust pointers for       */
	register		i;	/* loop index                          */
	register		n;	/* number of rows in virtual screen    */
	register		size;	/* decrement count                     */
	register		tsize;	/* total amount of memory to release   */
	int			termtrap();	/* terminate signal catch func */

	/*
	 * Ignore the terminate signal, otherwise things may go wrong if we
	 * receive one during the move of the data areas.
	 */
	(void)signal(SIGTERM, SIG_IGN);

	/*
	 * First compute the pointers to the areas to move.
	 */
	lp = (char *)wsp;
	if (wsp->status & CREATED) {
		mp = (char *)wsp->uboxp;
	}
	else {
		mp = mem_high;
	}
	tsize = wsp->alloccnt + WININTSIZE;

	/*
	 * Adjust all the pointers in the structures and the pointers to the
	 * structures themselves.
	 */
	for (i = 0 ; i < MAXWINS ; i++) {
		wsp2 = wstatp[i];
		if (wsp2 != NULL) {
			if (wsp2->status & CREATED) {
				size = 0;
				if ((char *)wsp2->ttyd.t_inq.c_cb > mp) {
					size = wsp->alloccnt;
				}
				if ((char *)wsp2->ttyd.t_inq.c_cb > lp) {
					size += WININTSIZE;
				}

				/*
				 * Adjust the pointer to the user boxes.
				 */
				(char *)wsp2->uboxp -= size;

				/*
				 * Adjust the pointers to the different lines
				 * of the saved contents of the virtual screen.
				 */
				if (wsp2->estatus & SAVETEXT) {
					(unsigned char **)tp = wsp2->savc;
					n = wsp2->rows;
					do {
						(char *)(*((unsigned char **)tp)) -= size;
						((unsigned char **)tp)++;
					} while (--n > 0);
					(char *)wsp2->savc -= size;
				}

				/*
				 * Adjust the different buffer pointers.
				 */
				(char *)wsp2->ttyd.t_inq.c_cb -= size;
				(char *)wsp2->ttyd.t_inq.c_cf -= size;
				(char *)wsp2->ttyd.t_inq.c_cl -= size;
				(char *)wsp2->ttyd.t_outq.c_cb -= size;
				(char *)wsp2->ttyd.t_outq.c_cn -= size;

				/*
				 * Adjust the pointer to the windows mouse
				 * pointer.
				 */
				if (wsp2->mppnt == pntdata.pd_mpp) {
					(char *)pntdata.pd_mpp -= size;
				}
				(char *)wsp2->mppnt -= size;
			}

			/*
			 * Adjust the pointer to the structure.
			 */
			if ((char *)wsp2 > mp) {
				size = tsize;
			}
			else if ((char *)wsp2 > lp) {
				size = WININTSIZE;
			}
			else {
				size = 0;
			}
			(char *)wstatp[i] -= size;

			/*
			 * Adjust the pointer to the font box pointers.
			 */
			(char *)wsp2->fdata.fi_fbas -= size;
		}
	}	/* for (i = 0 ; i < MAXWINS ; i++) */

	/*
	 * Now move all the data.
	 */
	tp = lp + WININTSIZE;
	i = mp - tp;
	n = mem_high - mp - wsp->alloccnt;
	while (--i >= 0) {
		*lp++ = *tp++;
	}
	tp = lp + tsize;
	while (--n >= 0) {
		*lp++ = *tp++;
	}

	/*
	 * Move all the icon data structures 'tsize' byte down.
	 */
	i = n_icons * sizeof(struct icondat);
	lp = (char *)iconsave;
	(char *)iconsave = tp = lp - tsize;
	while (--i >= 0) {
		*tp++ = *lp++;
	}

	/*
	 * Finish by releasing the memory.
	 */
	mem_high -= tsize;
	(void)Sbrk(-tsize);

	/*
	 * Enable the terminate signal again.
	 */
	(void)signal(SIGTERM, termtrap);

	return;
}	/* end of sav_free() */

#undef	WININTSIZE



/*
 * This function erases the whole or a part of the current line in the text
 * save area if the SAVETEXT status flag is set.
 * If flg == 0, erase the whole line.
 * If flg > 0, erase from the current cursor position to the end of the line
 *   (inclusive).
 * If flg < 0, erase from the start of the line to the current cursor position
 *   (inclusive).
 */

void
sav_eln(wsp, flg)
register struct	winint	*wsp;	/* pointer to window status */
register		flg;	/* action flag              */
{
	register unsigned char	*cp;	/* pointer used to erase    */
	register		cnt;	/* counter                  */

	if ((wsp->estatus & SAVETEXT) && (wsp->status & GRAPHIC) == 0) {

		/*
		 * The text contents for this window is saved.
		 */
		cp = *(wsp->savc + wsp->y_cur - 1);
		if (flg == 0) {

			/*
			 * Erase the whole line.
			 */
			cnt = wsp->rm_cur;
		}
		else if (flg < 0) {

			/*
			 * Erase from the start of the line.
			 */
			cnt = wsp->x_cur;
		}
		else {

			/*
			 * Erase to the end of the line.
			 */
			cp += wsp->x_cur - 1;
			cnt = wsp->rm_cur - wsp->x_cur + 1;
		}

		while (--cnt >= 0) {
			*cp++ = ' ';
		}
	}

	return;
}	/* end of sav_eln() */



/*
 * This function erases the whole or a part of the scroll region in a window
 * text save area if the SAVETEXT status flag is set.
 * If flg == 0, erase the whole scroll region.
 * If flg > 0, erase from the line below the current cursor position to the
 *   end of the scroll region.
 * If flg < 0, erase from the beginning of the scroll region to the line above
 *   the current cursor position.
 */

void
sav_esc(wsp, flg)
register struct	winint	*wsp;	/* pointer to window status */
register		flg;	/* action flag              */
{
	register unsigned char	*cp;	/* pointer used for erasing */
	register unsigned char	**cpp;	/* pointer into lines array */
	register		cnt;	/* counter                  */
	register		i;	/* counter                  */
	register		cols;	/* number of columns        */

	if ((wsp->estatus & SAVETEXT) && (wsp->status & GRAPHIC) == 0) {
		if (flg == 0) {

			/*
			 * Erase the whole scroll region.
			 */
			cpp = wsp->savc + wsp->umr_cur - 1;
			cnt = wsp->bmr_cur - wsp->umr_cur + 1;
		}
		else if (flg < 0) {

			/*
			 * Erase from the beginning of the scroll region.
			 */
			cpp = wsp->savc + wsp->umr_cur - 1;
			cnt = wsp->y_cur - wsp->umr_cur;
		}
		else {

			/*
			 * Erase to the end of the scroll region.
			 */
			cpp = wsp->savc + wsp->y_cur;
			cnt = wsp->bmr_cur - wsp->y_cur;
		}

		cols = wsp->rm_cur;
		while (--cnt >= 0) {
			cp = *cpp++;
			i = cols;
			do {
				*cp++ = ' ';
			} while (--i > 0);
		}
	}

	return;
}	/* end of sav_esc() */



/*
 * This function "scrolls" the contents of the text save area for a window a
 * number of lines if the SAVETEXT status flag is set.
 * If 'lines' is positive, scroll up (normal scroll) else scroll down (reverse
 * scroll).
 */

void
sav_scroll(wsp, lines)
register struct	winint	*wsp;	/* pointer to window status  */
register		lines;	/* number of lines to scroll */
{
	register unsigned char	*cp;
	register unsigned char	**cpp;
	register		cnt;	/* counter */

	if ((wsp->estatus & SAVETEXT) && (wsp->status & GRAPHIC) == 0) {
		if (lines > 0) {
			do {	/* scroll one line 'lines' times */
				cpp = wsp->savc + wsp->umr_cur - 1;
				cp = *cpp;
				cnt = wsp->bmr_cur - wsp->umr_cur;
				while (--cnt >= 0) {
					*cpp = *(cpp + 1);
					cpp++;
				}
				*cpp = cp;	/* the bottom line */
				cnt = wsp->rm_cur;
				do {		/* clear the last line */
					*cp++ = ' ';
				} while (--cnt > 0);
			} while (--lines > 0);
		}
		else {
			while (lines++ < 0) {
				cpp = wsp->savc + wsp->bmr_cur - 1;
				cp = *cpp;
				cnt = wsp->bmr_cur - wsp->umr_cur;
				while (--cnt >= 0) {
					*cpp = *(cpp - 1);
					cpp--;
				}
				*cpp = cp;	/* this becomes the new top line */
				cnt = wsp->rm_cur;
				do {		/* clear the top line */
					*cp++ = ' ';
				} while (--cnt > 0);
			}
		}
	}

	return;
}	/* end of sav_scroll() */



/*
 * Rewrite the rectangle pointed to by 'wrp' in the window whose status is
 * pointed to by 'wsp'. Note that it is supposed that the rectangle has been
 * cleared.
 */

void
sav_rewrit(wsp, wrp)
register struct	winint	*wsp;	/* pointer to window status */
register struct	wpstruc	*wrp;	/* pointer to rectangle     */
{
	register struct	hwastruc *hwap;		/* pointer to hardware addresses      */
	register	  byte	 statflg;	/* mover status flags                 */
	register unsigned char	 flag;		/* flag                               */
	register unsigned char	 c;		/* character                          */
	register		 x;		/* various uses                       */
	register		 y;		/* various uses                       */
	register unsigned char	 *str;		/* pointer to characters to display   */
	unsigned	  char	 *str2;		/* used to remove trailing spaces     */
	register		 leftp;		/* left side of char block on screen  */
	register		 upp;		/* upper side of char block on screen */
	int			 rightp;	/* right side of char block on screen */
	int			 downp;		/* lower side of char block on screen */
	int			 leftc;		/* column of left char in rectangle   */
	int			 cols;		/* # of columns in rectangle          */
	int			 upc;		/* row # of upper chars in rectangle  */
	int			 rows;		/* # of rows in rectangle             */
	int			 len;		/* length of row to display           */

	if ((wsp->estatus & SAVETEXT) && (wsp->status & GRAPHIC) == 0 && (wrp->bflags & B_NOTEMPTY)) {

		/*
		 * The text is saved and the rectangle is not empty.
		 */
#ifdef LANDSCAPE
		leftc = (wrp->yuv_p - wsp->y_orig) / wsp->fdata.fi_xsize;
		cols = (wrp->ylv_p - wsp->y_orig) / wsp->fdata.fi_xsize + 1;
#else  PORTRAIT
		leftc = (wrp->xlv_p - wsp->x_orig) / wsp->fdata.fi_xsize;
		cols = (wrp->xrv_p - wsp->x_orig) / wsp->fdata.fi_xsize + 1;
#endif LANDSCAPE / PORTRAIT
		if ((cur_d)cols > wsp->columns) {
			cols = wsp->columns;
		}
		cols -= leftc;
#ifdef LANDSCAPE
		upc = (wsp->x_orig + wsp->x_size - 1 - wrp->xrv_p) / wsp->fdata.fi_ysize;
		rows = (wsp->x_orig + wsp->x_size - 1 - wrp->xlv_p) / wsp->fdata.fi_ysize + 1;
#else  PORTRAIT
		upc = (wrp->yuv_p - wsp->y_orig) / wsp->fdata.fi_ysize;
		rows = (wrp->ylv_p - wsp->y_orig) / wsp->fdata.fi_ysize + 1;
#endif LANDSCAPE / PORTRAIT
		if ((cur_d)rows > wsp->rows) {
			rows = wsp->rows;
		}
		rows -= upc;

		if (cols > 0) {

			/*
			 * Make sure the font is loaded and set up.
			 */
			if (wsp->fdata.fi_flag & NOFONT) {
				if (adjfont(wsp->fdata.fi_cur, &wsp->fdata) != OK) {
					return;		/* skip if failed */
				}
			}

#ifdef LANDSCAPE
			upp = wsp->x_orig + wsp->x_size - 1 - Muls(upc, wsp->fdata.fi_ysize);
#else  PORTRAIT
			upp = wsp->y_orig + Muls(upc, wsp->fdata.fi_ysize);
#endif LANDSCAPE / PORTRAIT
			hwap = &hwa;

			/*
			 * Display the characters row by row.
			 */
			while (--rows >= 0) {
				str2 = str = *(wsp->savc + upc) + leftc;
#ifdef LANDSCAPE
				leftp = wsp->y_orig + Muls(leftc, wsp->fdata.fi_xsize);
				downp = upp - wsp->fdata.fi_ysize + 1;
#else  PORTRAIT
				leftp = wsp->x_orig + Muls(leftc, wsp->fdata.fi_xsize);
				downp = upp + wsp->fdata.fi_ysize - 1;
#endif LANDSCAPE / PORTRAIT
				len = cols;

				/*
				 * Remove trailing spaces.
				 */
				str += len;
				while (--str >= str2 && *str == ' ') {
					len--;
				}
				str = str2;

				while (len > 0) {
					/* x = Min(wsp->fdata.fi_chb, len); */
					x = wsp->fdata.fi_chb;
					if (len < x) {
						x = len;
					}
					len -= x;
					rightp = leftp + Muls(x, wsp->fdata.fi_xsize) - 1;

					/*
					 * Move the characters to the set up area.
					 */
#ifdef LANDSCAPE
					if (*str & REVATTR) {
						statflg = GSTATFLG | LAPOFL | COMPMFL;
						flag = REVATTR;
					}
					else {
						statflg = GSTATFLG | LAPOFL;
						flag = 0;
					}
#else  PORTRAIT
					if (*str & REVATTR) {
						statflg = GSTATFLG | COMPMFL;
						flag = REVATTR;
					}
					else {
						statflg = GSTATFLG;
						flag = 0;
					}
#endif LANDSCAPE / PORTRAIT
					Waitsp();
					Outb(statflg, hwap->mflagport);
					Outl(wsp->fdata.fi_size, hwap->x_sizeport);
					Outl((SETUPXCOORD << 16) | SETUPYCOORD, hwap->x_toport);
					do {
						c = *str++;
						if ((c & REVATTR) != flag) {
							statflg ^= COMPMFL;
							flag ^= REVATTR;
							Outb(statflg, hwap->mflagport);
						}
						Outl(*(wsp->fdata.fi_fbas + (c & CMASK)), hwap->x_fromport);
						Waitsp();
					} while (--x != 0);

					/*
					 * Now move out the part that belongs to the rectangle.
					 */
					Outb(wsp->statflg, hwap->mflagport);
#ifdef LANDSCAPE
					x = wrp->xlv_p - downp;
					if (x < 0) {
						x = 0;
					}
					y = wrp->yuv_p - leftp;
					if (y < 0) {
						y = 0;
					}

					/*
					 * Move it out.
					 */
					Move(SETUPXCOORD + x, SETUPYCOORD + y,
					     downp + x, leftp + y,
					     Min(upp, wrp->xrv_p) - downp - x,
					     Min(rightp, wrp->ylv_p) - leftp - y, UX_UY);
#else  PORTRAIT
					x = wrp->xlv_p - leftp;
					if (x < 0) {
						x = 0;
					}
					y = wrp->yuv_p - upp;
					if (y < 0) {
						y = 0;
					}

					/*
					 * Move it out.
					 */
					leftp += x;
					Move(SETUPXCOORD + x, SETUPYCOORD + y,
					     leftp, upp + y,
					     Min(rightp, wrp->xrv_p) - leftp,
					     Min(downp, wrp->ylv_p) - upp - y, UX_UY);
#endif LANDSCAPE / PORTRAIT
					leftp = rightp + 1;	/* update left side */

				}	/* while (len > 0) */

#ifdef LANDSCAPE
				upp = downp - 1;	/* update upper side */
#else  PORTRAIT
				upp = downp + 1;	/* update upper side */
#endif LANDSCAPE / PORTRAIT
				upc++;			/* next row */

			}	/* while (--rows >= 0) */
		}	/* if (cols > 0) */
	}	/* if ((wsp->estatus & SAVETEXT) && .... ) */

	return;
}	/* end of sav_rewrit() */
