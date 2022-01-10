/* charhand.c */

/*
 * This file contains routines to handle the output of characters and take care
 * of control characters and escape sequences for the ABC1600 window handler.
 */

/* 1984-07-20, Peter Andersson, Luxor Datorer AB (at DIAB) */

/*
 * Functions in this file:
 *	win_out()
 *	dispchrs()
 *	disphdr()
 *	eschand()
 *	repmous()
 *	adjleds()
 *	itoa()
 */


#include	"../hsrc/defs.h"
#include	"../hsrc/structs.h"
#include	"../hsrc/eglobals.h"
#include	"../hsrc/graphconst.h"
#include	"../hsrc/macros.h"
#include	"../hsrc/charcodes.h"
#include	"../../gdrv/kybrdcodes.h"



/*
 * Output printable characters, control characters, and escape sequences to the
 * window wn.
 * This is a combination of a VT-100 and the UNIX tty device driver.
 */

void
win_out(wn, str, len)
int			wn;		/* window number     */
register unsigned char	*str;		/* string to output  */
register		len;		/* # of chars in str */
{
	register struct	winint	*wsp;	/* pointer to window status           */
	register unsigned char	*str2;	/* str save                           */
	register unsigned char	*cp;	/* to save characters                 */
	register		tmp;	/* temporary                          */
	register		chrem;	/* remaining character pos. on current
					   line                               */
	register		cnt;	/* counter                            */
	register	t_stop	tmask;	/* to mask out tab stop marks         */
	int			sflg;	/* scroll before output chars flag    */
	register unsigned char	invflg;	/* reverse video flag                 */
	unsigned char	*eschand();	/* routine to take care of escape
					   sequences                          */

	wsp = wstatp[wn];

	/*
	 * If an escape sequence is pending, complete it.
	 */
	if (wsp->status & ESCMODE) {
		len -= (str2 = eschand(wn, str, len)) - str;
		str = str2;
	}

	/*
	 * Now process the rest of the characters.
	 */
	while (len > 0) {
		chrem = wsp->rm_cur - wsp->x_cur + 1;	/* remaining char pos. on this line */
		if (chrem == 1 && (wsp->termpara & TD_WRAP) && (wsp->status & ENDLINE)) {
			/*
			 * We must scroll before output next character.
			 */
			chrem = wsp->rm_cur;	/* we can output one line */
			sflg = YES;
		}
		else {
			sflg = NO;
		}
		str2 = str;		/* save start of string */

		/*
		 * Determine how many characters we shall output in one run.
		 */
		while (len > 0 && (tmp = (*str & CMASK)) >= wsp->fdata.fi_cfirst
		       && tmp <= wsp->fdata.fi_clast && chrem > 0) {
			str++;
			len--;
			chrem--;
		}
		cnt = str - str2;	/* number of chars to output in one run */
		if (cnt > 0) {		/* if any printable characters */
			if (sflg != NO) {

				/*
				 * Force a CR-LF.
				 */
				wsp->x_cur = 1;
				if (wsp->y_cur == wsp->bmr_cur) {
					if (wsp->termpara & TD_NOSCR) {

						/*
						 * No scroll (page) mode.
						 */
						wsp->y_cur = wsp->umr_cur;
					}
					else {

						/*
						 * We must scroll the window.
						 */
						scrollwin(wn, 1);
					}
				}
				else if (wsp->y_cur < wsp->bm_cur) {
					wsp->y_cur++;
				}
			}
			dispchrs(wn, str2, cnt);	/* display chars */

			/*
			 * Save the characters.
			 */
			if ((wsp->estatus & SAVETEXT) && (wsp->status & GRAPHIC) == 0) {

				/*
				 * The text shall be saved.
				 */
				if (wsp->termpara & TD_REVERSE) {
					invflg = REVATTR;
				}
				else {
					invflg = 0;
				}
				cp = *(wsp->savc + wsp->y_cur - 1) + wsp->x_cur - 1;
				chrem = cnt;
				do {
					*cp++ = (*str2++ & CMASK) | invflg;
				} while (--chrem != 0);
			}

			/*
			 * Update the cursor position.
			 */
			wsp->x_cur += cnt;
			if (wsp->x_cur > wsp->rm_cur) {
				wsp->x_cur = wsp->rm_cur;
				wsp->status |= ENDLINE;
			}
			else {
				wsp->status &= ~ENDLINE;
			}
		}	/* if (cnt > 0) */

		/*
		 * If characters left and the next character is a control
		 * character, process it.
		 */
		if (len > 0 && (tmp < wsp->fdata.fi_cfirst || tmp > wsp->fdata.fi_clast)) {
			str++;
			len--;

			/*
			 * Process the control character.
			 */
			sflg = wsp->ttyd.t_oflag;
			if ((sflg & OPOST) == 0) {
				sflg = 0;
			}
			else if (tmp == '\r' && (sflg & OCRNL)) {
				tmp = '\n';
				sflg = 0;
			}

			switch (tmp) {
			case '\0':

				/*
				 * Nulls are used for padding, so some programs
				 * sends a lot of them.
				 */
				while (len > 0 && *str == '\0') {
					len--;
					str++;
				}
				break;

			case '\n':		/* newline */
			case VT:		/* vertical tab */
			case FF:		/* form feed */

				if (wsp->y_cur == wsp->bmr_cur) {
					if (wsp->termpara & TD_NOSCR) {

						/*
						 * No scroll (page) mode.
						 */
						wsp->y_cur = wsp->umr_cur;
					}
					else {

						/*
						 * We must scroll the window.
						 */
						cnt = 1;	/* # of newlines */
						while (len > 0 && *str >= '\n' && *str <= FF) {
							str++;
							len--;
							cnt++;
						}
						scrollwin(wn, cnt);	/* scroll window */
					}
				}
				else if (wsp->y_cur < wsp->bm_cur) {
					wsp->y_cur++;
				}

				if ((wsp->termpara & TD_NL) == 0 && (sflg & ONLCR) == 0) {
					break;	/* don't add a carriage return */
				}

			case '\r':		/* carriage return */

				wsp->x_cur = 1;
				break;

			case '\b':		/* backspace */

				if (wsp->x_cur >= 2) {
					wsp->x_cur--;	/* back cursor */
				}
				break;

			case '\t':		/* tab */

				tmp = wsp->x_cur - 1;
				chrem = 0;
				(t_stop *)cp = &wsp->tabstop[(unsigned)tmp/TP_PER_EL];
				tmask = 1 << (tmp % TP_PER_EL);
				while (++tmp < wsp->rm_cur) {
					chrem++;
					if ((tmask <<= 1) == 0) {
						tmask = 1;
						((t_stop *)cp)++;
					}
					if (*((t_stop *)cp) & tmask) {
						tmp++;
						break;	/* found tab stop */
					}
				}
				if ((sflg & TABDLY) != TAB3) {
					wsp->x_cur = tmp;
				}
				else {

					/*
					 * Expand the tab to spaces.
					 */
					while (chrem > 0) {
						if (chrem > 8) {
							tmp = 8;
						}
						else {
							tmp = chrem;
						}

						/*
						 * The string contains 8 spaces!
						 */
						win_out(wn, (unsigned char *)"        ", tmp);
						chrem -= tmp;
					}
				}
				break;

			case BELL:		/* bell */

				if (wtol[wn] == 0) {
					(void)sendkybrd(K_BELL);
				}
				break;

			case ESC:		/* escape sequence */

				len -= (str2 = eschand(wn, str, len)) - str;
				str = str2;
				break;

			default:
				break;		/* just skip other control chars */
			}	/* switch (*str++) */
		}

	}	/* while (len > 0) */

	return;
}	/* end of win_out() */



/*
 * This function displays a string of characters in a window.
 * The string is supposed to contain no control characters and it is also
 * supposed to fit on the current line in the virtual screen.
 *
 * The method is as follows:
 * First all characters which surely aren't visible in the window are removed
 * (actually only leading and trailing characters may be stripped of).
 * Then the font boxes corresponding to the characters are moved in chunks to a
 * set up area and the set up area are then copied out in the window. This is
 * repeated until no more characters remain.
 */

void
dispchrs(wn, str, len)
int			wn;	/* window number              */
register unsigned char	*str;	/* characters to display      */
int			len;	/* # of characters to display */
{
	register		 x;	/* various uses                       */
	register		 y;	/* various uses                       */
	register		 left;	/* left side of char block on screen  */
	register		 right;	/* right side of char block on screen */
	register		 up;	/* upper side of char block on screen */
	register		 down;	/* lower side of char block on screen */
	int			 tmp;	/* temporary                          */
	register struct	winint	 *wsp;	/* pointer to window status           */
	register struct	wpstruc	 *wrp;	/* pointer to rectangle group         */
	register struct	hwastruc *hwap;	/* pointer to hardware addresses      */

	wsp = wstatp[wn];

	/*
	 * Check if the current line may be visible in the window.
	 */
	y = wsp->y_cur;
	if ((cur_d)y >= wsp->cu_vis && (cur_d)y <= wsp->cd_vis) {

		/*
		 * Now found out which characters in the string may be visible.
		 */
		x = wsp->x_cur;
		if ((left = wsp->cl_vis - x) <= 0) {
			right = x - 1;	/* we have to subtr. 1 later on anyway */
		}
		else {
			str += left;
			len -= left;
			right = x + left - 1;
		}
		if ((left = right + len - wsp->cr_vis) > 0) {
			len -= left;
		}
		if (len > 0) {

			/*
			 * Try to load the font if it is not active or set up.
			 */
			if (wsp->fdata.fi_flag & NOFONT) {
				if (adjfont(wsp->fdata.fi_cur, &wsp->fdata) != OK) {
					return;		/* skip if we failed */
				}
			}

			/*
			 * Compute as much as possible of the absolute pixel area on the
			 * screen corresponding to the character block.
			 */
#ifdef LANDSCAPE
			left = wsp->y_orig + Muls(right, wsp->fdata.fi_xsize) + wsp->x_coffs;
			x = wsp->fdata.fi_ysize;
			up = wsp->x_orig + wsp->x_size - 1 - Muls(y - 1, x) - wsp->y_coffs;
			down = up - x + 1;
#else  PORTRAIT
			left = wsp->x_orig + Muls(right, wsp->fdata.fi_xsize) + wsp->x_coffs;
			x = wsp->fdata.fi_ysize;
			up = wsp->y_orig + Muls(y - 1, x) + wsp->y_coffs;
			down = up + x - 1;
#endif LANDSCAPE / PORTRAIT

			/*
			 * Now display the characters (if any left).
			 */
			hwap = &hwa;
			while (len > 0) {
				/* x = Min(wsp->fdata.fi_chb, len); */	/* chunk to display */
				x = wsp->fdata.fi_chb;
				if (len < x) {
					x = len;
				}
				len -= x;
				right = left + Muls(x, wsp->fdata.fi_xsize) - 1;   /* right side */

				/*
				 * Move the characters to the set up area.
				 */
				Waitsp();
#ifdef LANDSCAPE
				Outb(GSTATFLG | LAPOFL, hwap->mflagport);	/* don't invert */
#else  PORTRAIT
				Outb(GSTATFLG, hwap->mflagport);		/* don't invert */
#endif LANDSCAPE / PORTRAIT
				Outl(wsp->fdata.fi_size, hwap->x_sizeport);	/* size of font */
				Outl((SETUPXCOORD << 16) | SETUPYCOORD, hwap->x_toport);
								/* to addr is set up area */
				do {
					Outl(*(wsp->fdata.fi_fbas + (*str++ & CMASK)), hwap->x_fromport);
					Waitsp();
				} while (--x != 0);
				if (wsp->termpara & TD_USCORE) {

					/*
					 * The underscore attribute is on. Underline the
					 * characters in the set up area.
					 */
					Outb(GSTATFLG | HOLDYFL | HOLDXFL, hwap->mflagport);
#ifdef LANDSCAPE
					Move(FILLXCOORD, FILLYCOORD, SETUPXCOORD,
					     SETUPYCOORD, 0, right - left, UX_UY);
#else  PORTRAIT
					Move(FILLXCOORD, FILLYCOORD, SETUPXCOORD,
					     SETUPYCOORD + wsp->fdata.fi_ysize - 1,
					     right - left, 0, UX_UY);
#endif LANDSCAPE / PORTRAIT
					Waitsp();
				}

				Outb(wsp->chstatflg, hwap->mflagport);	/* set inverse or normal video */

				/*
				 * Now scan all the rectangles and move out the whole block
				 * or a part of it if the block is inside the rectangle.
				 */
				wrp = wpp[wn];	/* point to rectangles for this window */
				tmp = nwpart[wn];	/* # of rectangles */
				while (--tmp >= 0) {

					/*
					 * Is any part of the character block inside this
					 * rectangle?
					 */
#ifdef LANDSCAPE
					if ((wrp->bflags & B_NOTEMPTY) && (pix_d)left <= wrp->ylv_p &&
					    (pix_d)right >= wrp->yuv_p && (pix_d)up >= wrp->xlv_p &&
					    (pix_d)down <= wrp->xrv_p) {

						/*
						 * Compute the part of the character
						 * block to move out.
						 */
						x = wrp->xlv_p - down;
						if (x < 0) {
							x = 0;
						}
						y = wrp->yuv_p - left;
						if (y < 0) {
							y = 0;
						}

						/*
						 * Move it out.
						 */
						Waitsp();
						Move(SETUPXCOORD + x, SETUPYCOORD + y,
						     down + x, left + y,
						     Min(up, wrp->xrv_p) - down - x,
						     Min(right, wrp->ylv_p) - left - y,
						     UX_UY);
					}
#else  PORTRAIT
					if ((wrp->bflags & B_NOTEMPTY) && (pix_d)left <= wrp->xrv_p &&
					    (pix_d)right >= wrp->xlv_p && (pix_d)up <= wrp->ylv_p &&
					    (pix_d)down >= wrp->yuv_p) {

						/*
						 * Compute the part of the character block
						 * to move out.
						 */
						x = wrp->xlv_p - left;
						if (x < 0) {
							x = 0;
						}
						y = wrp->yuv_p - up;
						if (y < 0) {
							y = 0;
						}

						/*
						 * Move it out.
						 */
						Waitsp();
						Move(SETUPXCOORD + x, SETUPYCOORD + y,
						     left + x, up + y,
						     Min(right, wrp->xrv_p) - left - x,
						     Min(down, wrp->ylv_p) - up - y,
						     UX_UY);
					}	/* if ((wrp->bflags & B_NOTEMPTY ...) */
#endif LANDSCAPE / PORTRAIT
					wrp++;		/* next rectangle */
				}	/* while (--tmp >= 0) */

				left = right + 1;	/* update left side */

				/*
				 * If there are any input characters to echo, do that
				 * if we are not writing to the level 0 window (to
				 * avoid disorder between the saving of characters and
				 * the display of them).
				 */
				if (keyin == KEY && wtol[wn] != 0) {
					ttyinput();
				}
			}	/* while (len > 0) */
		}
	}	/* if ((cur_d)y >= wsp->cu_vis && ....) */

	return;
}	/* end of dispchrs() */



/*
 * Display the window header for window wn, if there is any.
 * If the border type does not allow a header or the header is too long to fit
 * in the current border, nothing is displayed.
 * The header is supposed to contain no control characters.
 * The method used is approximately the same as in the dispchrs() function.
 */

void
disphdr(wn, cflag)
int	wn;		/* window number, also used as window level */
int	cflag;		/* clear header flag (clear if OFF)         */
{
	register struct	winint	 *wsp;	/* pointer to window status       */
	register unsigned char	 *str;	/* used to point to header string */
	register struct	finfo	 *hdfp;	/* pointer to header font data    */
	register struct	hwastruc *hwap;	/* pointer to hardware addresses  */
	register		 x;	/* various uses                   */
	register		 left;	/* left side of header chunk      */
	register		 right;	/* right side of header chunk     */
	register		 up;	/* upper side of header chunk     */
	register		 down;	/* lower side of header chunk     */
	register		 len;	/* length of header               */
	byte			 stflg;	/* flag to write to mover status
					   port                           */

	hdfp = &hdf_data;

	/*
	 * First check that the header font (font A) is present.
	 */
	if (hdfp->fi_flag & NOFONT) {
		return;		/* skip if font not present */
	}
	wsp = wstatp[wn];
	len = wsp->hdrlen;
	x = Muls(len, hdfp->fi_xsize);		/* header length in pixels */

#ifdef LANDSCAPE
	left = wsp->by_size - 2 * DLBWIDTH;	/* compute free width */
	if (wsp->bpresent & (BX_BLOW | BX_HELP)) {
		left -= 2 * DLBWIDTH;
	}
	if ((wsp->bordflags & BD_UP) && x != 0 && x <= left) {
		str = wsp->hdr;		/* point to header */
		left = wsp->avy_orig + Div2(wsp->vy_size - x);
		up = wsp->abx_lrc - HEADOFFS;
		down = up - hdfp->fi_ysize + 1;
		if (down < wsp->abx_lrc - DLBWIDTH + 6) {
			down = wsp->abx_lrc - DLBWIDTH + 6;
		}
#else  PORTRAIT
	left = wsp->bx_size - 2 * DLBWIDTH;	/* compute free width */
	if (wsp->bpresent & (BX_BLOW | BX_HELP)) {
		left -= 2 * DLBWIDTH;
	}
	if ((wsp->bordflags & BD_UP) && x != 0 && x <= left) {
		str = wsp->hdr;		/* point to header */
		left = wsp->avx_orig + Div2(wsp->vx_size - x);
		up = wsp->aby_orig + HEADOFFS;
		down = up + hdfp->fi_ysize - 1;
		if (down > wsp->aby_orig + DLBWIDTH - 6) {
			down = wsp->aby_orig + DLBWIDTH - 6;
		}
#endif LANDSCAPE / PORTRAIT
		hwap = &hwa;
		while (len > 0) {
			/* x = Min(hdfp->fi_chb, len); */
			x = hdfp->fi_chb;
			if (len < x) {
				x = len;
			}
			len -= x;
			right = left + Muls(x, hdfp->fi_xsize) - 1;

			/*
			 * Move the characters to the set up area.
			 */
#ifdef LANDSCAPE
			stflg = GSTATFLG | LAPOFL;
#else  PORTRAIT
			stflg = GSTATFLG;
#endif LANDSCAPE / PORTRAIT
			if (cflag != OFF) {	/* if not clear header */
				if (wsp->status & INVHEAD) {
					stflg ^= COMPMFL;
				}
				if (wtol[wn] == 0 && (wsp->status & INVTOP)) {
					stflg ^= COMPMFL;
				}
			}
			Waitsp();
			Outb(stflg, hwap->mflagport);
			Outl(hdfp->fi_size, hwap->x_sizeport);
			Outl((SETUPXCOORD << 16) | SETUPYCOORD, hwap->x_toport);

			if (cflag != OFF) {
				do {
					Outl(*(hdfp->fi_fbas + *str++), hwap->x_fromport);
					Waitsp();
				} while (--x != 0);
			}
			else {
				do {
					Outl(*(hdfp->fi_fbas + ' '), hwap->x_fromport);
					Waitsp();
				} while (--x != 0);
			}
			Outb(wsp->statflg, hwap->mflagport);

			/*
			 * Now scan all the rectangles and move out the
			 * whole block or a part of it if the block is
			 * inside the rectangle.
			 */
#ifdef LANDSCAPE
			d1box(wn, down, up - down, left, right - left, SETUPXCOORD, SETUPYCOORD);
#else  PORTRAIT
			d1box(wn, left, right - left, up, down - up, SETUPXCOORD, SETUPYCOORD);
#endif LANDSCAPE / PORTRAIT

			left = right + 1;	/* update left side */
		}	/* while (len > 0) */

	}	/* if ((wsp->bordflags & BD_UP) && ... ) */

	return;
}	/* end of disphdr() */



/*
 * Function to take care of the decoding and execution of escape sequences.
 * On entry the escape code is supposed to has been skipped (in case of a new
 * escape sequence) or a sequence under processing is going to be completed.
 * On exit a pointer to the first character after the escape sequence or beyond
 * the string is returned.
 */

#define	Intermed(c)	((c) >= ' ' && (c) <= '/')
#define	Endescseq(c)	((c) >= '0' && (c) <= '~')
#define	Endctrlseq(c)	((c) >= '@' && (c) <= '~')


unsigned char *
eschand(wn, str, len)
register		wn;	/* window to send escape sequence to              */
register unsigned char	*str;	/* string containing the sequence or a part of it */
int			len;	/* length of str                                  */
{
	register struct	winint	*wsp;		/* pointer to window status             */
	register	pix_d	*para;		/* pointer to escape parameters         */
	register		npara;		/* # of escape parameters               */
	register unsigned char	c;		/* a character in str                   */
	register		i;		/* loop index and temporary variable    */
	register		j;		/* loop index and temporary variable    */
	register		val;		/* temporary value                      */
	byte			savflags;	/* to save mover flags for the window   */
	word			savterm;	/* to save terminal flags for window    */
	register	char	*cp;		/* various uses                         */
	int			x1;		/* used when convert to mover coord.    */
	int			y1;
	int			x2;
	int			y2;
	t_stop			tmask;		/* used to mask out tab stop marks      */
	static	unsigned char	newlin = '\n';	/* 1 character string containg an nl    */
	char			*itoa();	/* integer to ASCII conversion          */
	struct		wpstruc	*fndrect();

	/*
	 * Scan the string, while extracting possible parameters and put them
	 * in a stack, to check for a closed escape sequence within the given
	 * length. If not a closed sequence, the escape sequence will be taken
	 * care of later.
	 * Control characters embedded in the sequence are taken care of
	 * immediately.
	 */
	wsp = wstatp[wn];

restart:		/* jump back here if an escape is encountered */
	if ((wsp->status & ESCMODE) == 0) {	/* if start new sequence */
		wsp->status |= ESCMODE;
		wsp->status &= ~(ESCTRLSEQ | ESCGRSEQ | ESCSEQ | SELCHSET);
		wsp->nescpara = 0;
		wsp->escpara[0] = 0;
	}

	if (len <= 0) {
		return(str);	/* complete later */
	}

	/*
	 * If the type of escape sequence is not determined yet, do that.
	 */
	if ((wsp->status & (ESCTRLSEQ | ESCGRSEQ | ESCSEQ | SELCHSET)) == 0) {
		c = *str++ & CMASK;
		len--;

		switch (c) {

		case '[':		/* control escape sequence */

			wsp->status |= ESCTRLSEQ;
			break;

		case ':':		/* ABC1600 private escape sequence */

			wsp->status |= ESCGRSEQ;
			break;

		case '(':
		case ')':		/* select new character set */

			wsp->status |= SELCHSET;
			break;

		case ESC:		/* start a new escape sequence */

			wsp->status &= ~ESCMODE;
			goto restart;

		case CAN:
		case SUB:		/* abort the escape sequence */

			wsp->status &= ~ESCMODE;
			return(str);

		default:

			if (c < ' ') {	/* control character, execute immediately */
				wsp->status &= ~ESCMODE;	/* not escape mode */
				win_out(wn, str - 1, 1);
				goto restart;			/* start over */
			}
			else {		/* it is an esacpe sequence without parameters */
				wsp->status |= ESCSEQ;
				str--;		/* compensate */
				len++;
			}

			break;

		}	/* switch (c) */
	}	/* if ((wsp->status & (ESCTRLSEQ | ESCGRSEQ | ESCSEQ | SELCHSET)) == 0) */

	/*
	 * The type of the escape sequence is determined, perform the rest.
	 */
	if (wsp->status & (ESCTRLSEQ | ESCGRSEQ)) {
		npara = wsp->nescpara;
		para = &wsp->escpara[npara];

		/*
		 * If it is a control or an ABC1600 private escape sequence,
		 * extract the parameters.
		 */
		while (--len >= 0) {
			c = *str & CMASK;
			if (c == ';') {		/* new parameter */
				if (++npara >= MAXESCPARA) {

					/*
					 * Too many parameters in escape sequence, abort.
					 */
					wsp->status &= ~ESCMODE;
					return(str);
				}
				*++para = 0;		/* zero next parameter */
			}
			else if (c >= '0' && c <= '?') {
				*para = 10 * (*para) + c - '0';
			}
			else if (Endctrlseq(c)) {	/* if terminating char */
				break;
			}
			else if (c == ESC) {	/* start a new sequence */
				str++;
				wsp->status &= ~ESCMODE;
				goto restart;
			}
			else if (c == CAN || c == SUB) {

				/*
				 * Abort the escape sequence.
				 */
				wsp->status &= ~ESCMODE;
				return(++str);
			}
			else if (c < ' ') {

				/*
				 * Control characters executes immediately.
				 */
				wsp->status &= ~ESCMODE;
				win_out(wn, str, 1);
				wsp->status |= ESCMODE;
			}
			else if (!Intermed(c)) {

				/*
				 * Illegal character in escape sequence.
				 */
				wsp->status &= ~ESCMODE;
				return(str);
			}

			str++;		/* next character */

		}	/* while (--len >= 0) */

		if (len < 0) {
			wsp->nescpara = npara;	/* save no. of parameters */
			return(str);		/* escape sequence not complete */
		}
		str++;			/* point beyond processed characters */

		i = ++npara;
		(pix_d *)cp = para = &wsp->escpara[0];

		/*
		 * Make sure that no parameters are negative or big enough to
		 * cause trouble.
		 */
		if ((wsp->status & ESCGRSEQ) == 0 || c != DEFPATT) {
			do {
				if (*((pix_d *)cp) < 0) {
					*((pix_d *)cp) = 0;
				}
				else if (*((pix_d *)cp) > MAXVALPARA) {
					*((pix_d *)cp) = MAXVALPARA;
				}
				((pix_d *)cp)++;
			} while (--i > 0);
		}

		if (wsp->status & ESCTRLSEQ) {
			switch (c) {

			case CUU:	/* cursor up */
			case CUD:	/* cursor down */
			case CUF:	/* cursor forward */
			case CUB:	/* cursor backward */

				do {
					val = *para++;
					if (val == 0) {
						val = 1;
					}

					switch (c) {

					case CUU:	/* cursor up */

						i = wsp->y_cur;		/* save old value */
						wsp->y_cur -= val;
						if (wsp->y_cur < wsp->umr_cur) {
							if ((cur_d)i >= wsp->umr_cur) {
								wsp->y_cur = wsp->umr_cur;
							}
							else if (wsp->y_cur < 1) {
								wsp->y_cur = 1;
							}
						}
						break;

					case CUD:	/* cursor down */

						i = wsp->y_cur;	/* save old value */
						wsp->y_cur += val;
						if (wsp->y_cur > wsp->bmr_cur) {
							if ((cur_d)i <= wsp->bmr_cur) {
								wsp->y_cur = wsp->bmr_cur;
							}
							else if (wsp->y_cur > wsp->bm_cur) {
								wsp->y_cur = wsp->bm_cur;
							}
						}
						break;

					case CUF:	/* cursor forward */

						wsp->x_cur += val;
						if (wsp->x_cur > wsp->rm_cur) {
							wsp->x_cur = wsp->rm_cur;
						}
						break;

					case CUB:	/* cursor backward */

						wsp->x_cur -= val;
						if (wsp->x_cur < 1) {
							wsp->x_cur = 1;
						}
						break;
					}	/* inner switch (c) */
				} while (--npara > 0);

				break;

			case CUP:	/* cursor position */
			case CUP1:

				if (npara < 2) {
					*(para + 1) = 0;
				}
				if ((i = *(para + 1)) <= wsp->rm_cur) {
					if (i == 0) {
						i = 1;
					}
					if ((val = *para) == 0) {
						val = 1;
					}
					if (wsp->termpara & TD_ORIGIN) {
						if (wsp->umr_cur + val - 1 <= wsp->bmr_cur) {
							wsp->y_cur = wsp->umr_cur + val - 1;
							wsp->x_cur = i;
						}
					}
					else if ((cur_d)val <= wsp->bm_cur) {
						wsp->y_cur = val;
						wsp->x_cur = i;
					}
				}
				break;

			case EIL:	/* erase in line */

				do {
					switch (*para++) {
					case 0:		/* erase to end of line */
						eraseln(wn, 1);
						break;
					case 1:		/* erase from beginning of line */
						eraseln(wn, -1);
						break;
					case 2:		/* erase the whole line */
						eraseln(wn, 0);
						break;
					}
				} while (--npara > 0);
				break;

			case ED:	/* erase in display */

				do {
					switch (*para++) {
					case 0:		/* erase to end of screen */
						erasescr(wn, 1);
						break;
					case 1:		/* erase from start of screen */
						erasescr(wn, -1);
						break;
					case 2:		/* erase the whole screen */
						erasescr(wn, 0);
						break;
					}
				} while (--npara > 0);
				break;

			case TBC:	/* tabulation clear */

				do {
					switch (*para++) {

					case 0:		/* clear tab stop at cur. pos. */

						i = wsp->x_cur - 1;
						wsp->tabstop[(unsigned)i/TP_PER_EL] &=
							~(1 << (i % TP_PER_EL));
						break;

					case 3:		/* clear all horizontal tab stops */

						(t_stop *)cp = &wsp->tabstop[0];
						i = TSTOPSIZE;
						do {
							*((t_stop *)cp)++ = 0;
						} while (--i != 0);
						break;
					}	/* switch (*para++) */
				} while (--npara > 0);
				break;

			case STBM:	/* set top and bottom margin (region) */

				i = *para;		/* top margin */
				j = *(para + 1);	/* bottom margin */
				if (i == 0) {
					i = 1;
				}
				if ((cur_d)j > wsp->bm_cur) {
					j = wsp->bm_cur;	/* limit to bottom */
				}
				if (npara == 2 && i < j) {
					wsp->umr_cur = i;
					wsp->bmr_cur = j;
				}
				else {

					/*
					 * Reset to default values.
					 */
					wsp->umr_cur = 1;
					wsp->bmr_cur = wsp->bm_cur;
				}

				/*
				 * Put the cursor in home position.
				 */
				wsp->x_cur = 1;
				if (wsp->termpara & TD_ORIGIN) {
					wsp->y_cur = wsp->umr_cur;
				}
				else {
					wsp->y_cur = 1;
				}
				break;

			case CAT:	/* character attributes */

				do {
					switch (*para++) {

					case 0:		/* attributes off */

						wsp->chstatflg = wsp->statflg;
						wsp->termpara &= ~(TD_USCORE | TD_REVERSE);
						break;

					case 4:		/* underscore */

						wsp->termpara |= TD_USCORE;
						break;

					case 1:		/* bold or increased */
					case 5:		/* blink */
					case 7:		/* reverse */

						wsp->chstatflg = wsp->statflg ^ COMPMFL;
						wsp->termpara |= TD_REVERSE;
						break;
					}	/* switch (*para++) */
				} while (--npara > 0);
				break;

			case SM:		/* set mode */

				do {
					switch (*para++) {

					case LNM:

						wsp->termpara |= TD_NL;
						break;

					case DECSCNM:

						if ((wsp->termpara & TD_SCREEN) == 0) {
							invertwin(wn);
						}
						break;

					case DECOM:

						wsp->termpara |= TD_ORIGIN;
						break;

					case DECAWM:

						wsp->termpara |= TD_WRAP;
						break;

					case TWPSMOD:	/* set page mode */

						wsp->termpara |= TD_NOSCR;
						break;

					case TWCTYP:	/* underline cursor */

						wsp->termpara |= TD_CUNDER;
						break;

					case TWCBLNK:	/* blinking cursor */

						wsp->termpara &= ~TD_NONBLNK;
						break;

					case TWCVIS:	/* cursor off */

						wsp->estatus |= NOCURSOR;
						break;

					}	/* switch (*para++) */
				} while (--npara > 0);
				break;

			case RM:		/* reset mode */

				do {
					switch (*para++) {

					case LNM:

						wsp->termpara &= ~TD_NL;
						break;

					case DECSCNM:

						if (wsp->termpara & TD_SCREEN) {
							invertwin(wn);
						}
						break;

					case DECOM:

						wsp->termpara &= ~TD_ORIGIN;
						break;

					case DECAWM:

						wsp->termpara &= ~TD_WRAP;
						break;

					case TWPSMOD:	/* reset page mode (scroll mode) */

						wsp->termpara &= ~TD_NOSCR;
						break;

					case TWCTYP:	/* reverse block cursor */

						wsp->termpara &= ~TD_CUNDER;
						break;

					case TWCBLNK:	/* non-blinking cursor */

						wsp->termpara |= TD_NONBLNK;
						break;

					case TWCVIS:	/* cursor on */

						wsp->estatus &= ~NOCURSOR;
						break;

					}	/* switch (*para++) */
				} while (--npara > 0);
				break;

			case DSR:		/* device status report */

				do {
					switch (*para++) {

					case 6:		/* report active (text) cursor pos. */

						i = wsp->y_cur;
						if (wsp->termpara & TD_ORIGIN) {
							i -= wsp->umr_cur - 1;
						}
						if (i <= 0) {
							i = 1;
						}
						cp = itoa(i, &repvt100[2]);
						*cp++ = ';';
						cp = itoa(wsp->x_cur, cp);
						*cp = CPR;
						putonque(wn, &repvt100[0], cp - &repvt100[0] + 1, YES);
						break;

					case TWSMOD:		/* report screen mode */

						putonque(wn, SMR, SMRLEN, YES);
						break;
					}
				} while (--npara > 0);
				break;

			case DECLL:		/* load LED's */

				do {
					val = *para++;
					if (val <= 8) {
						if (val == 0) {
							wsp->ledflg &= (1 << 9) | (1 << 10);
						}
						else {
							wsp->ledflg |= 1 << val;
						}
					}
				} while (--npara > 0);
				adjleds();
				break;

			case BTAB:		/* tabulation backward */

				val = wsp->x_cur - 1;
				(t_stop *)cp = &wsp->tabstop[(unsigned)val/TP_PER_EL];
				tmask = 1 << (val % TP_PER_EL);
				while (val > 0) {
					val--;
					if ((tmask >>= 1) == 0) {
						tmask = 1 << (TP_PER_EL - 1);
						((t_stop *)cp)--;
					}
					if (*((t_stop *)cp) & tmask) {
						break;	/* found tab stop */
					}
				}
				wsp->x_cur = val + 1;
				break;

			}	/* switch (c) */

		}	/* if (wsp->status & ESCTRLSEQ) */

		else {

			/*
			 * ABC1600 private escape sequence.
			 */
			savflags = wsp->statflg;	/* save mover status flags */

			switch (c) {

			case DRAWLINE:		/* draw line */

				/* ESC : <x> ; <y> ; <pattnr> ; <colnr> d */

				if (npara >= 2) {
					val = 0;
					if (npara != 2) {
						val = *(para + 2) & 0x0f;
					}
					if (npara < 4 || *(para + 3) == 0) {

						/*
						 * Color number not given.
						 */
						wsp->statflg ^= COMPMFL;
					}
					npara = NO;
					goto linelabel;
				}
				break;

			case INVLINE:		/* invert line */

				/* ESC : <x> ; <y> i */

				if (npara >= 2) {
					val = 0;
					npara = YES;
linelabel:
					x1 = i = *para;
					y1 = j = *(para + 1);
					x2 = wsp->x_gcur;
					y2 = wsp->y_gcur;
					if (inwin(wsp, &x1, &y1) != NO && inwin(wsp, &x2, &y2) != NO) {

						/*
						 * The whole line is inside the
						 * virtual screen.
						 */
						bline(wn, x1, y1, x2, y2, wsp->msk1[val], npara);
						wsp->x_gcur = i;
						wsp->y_gcur = j;
					}
				}
				break;

			case MOVGRPHCUR:	/* move graphic cursor */

				/* ESC : <x> ; <y> m */

				if (npara >= 2) {
					x1 = i = *para;
					y1 = j = *(para + 1);
					if (inwin(wsp, &x1, &y1) != NO) {

						/*
						 * The point is inside the
						 * virtual screen.
						 */
						wsp->x_gcur = i;
						wsp->y_gcur = j;
					}
				}
				break;

			case FILLAREA:		/* fill area */

				/* ESC : <x> ; <y> ; <pattnr> ; <colnr> f */

				if (npara >= 2) {
					if (npara < 4 || *(para + 3) == 0) {

						/*
						 * Color number not given.
						 */
						wsp->statflg ^= COMPMFL;
					}
					x1 = i = *para;
					y1 = j = *(para + 1);
					x2 = wsp->x_gcur;
					y2 = wsp->y_gcur;
					if (inwin(wsp, &x1, &y1) != NO && inwin(wsp, &x2, &y2) != NO) {

						/*
						 * The whole rectangle is inside
						 * the virtual screen.
						 */
						wsp->x_gcur = i;
						wsp->y_gcur = j;
						if (x1 > x2) {
							i = x2;
							x2 = x1;
							x1 = i;
						}
						if (y1 > y2) {
							i = y2;
							y2 = y1;
							y1 = i;
						}
						val = 0;
						if (npara >= 3) {
							val = *(para + 2) & 0x0f;
						}
						fill(wn, x1, y1, x2, y2, val);
					}
				}
				break;

			case PAINTAREA:		/* paint area */

				/* ESC : <x> ; <y> ; <pattnr> ; <colnr> F */

				if (npara >= 2) {
					i = *(para + 2) & 0x0f;
					if (npara < 3) {
						i = 0;
					}
					if (npara < 4 || *(para + 3) == 0) {

						/*
						 * Color number not given.
						 */
						wsp->statflg ^= COMPMFL;
					}
					x1 = val = *para;
					y1 = npara = *(para + 1);
					if (inwin(wsp, &x1, &y1) != NO) {
						if (fndrect(wn, x1, y1) != NULL) {
							paint(wn, x1, y1, i);
						}
						wsp->x_gcur = val;
						wsp->y_gcur = npara;
					}
				}
				break;

			case FILLCIRC:		/* fill circle */

				/* ESC : <x> ; <y> ; <rad> ; <pattnr> ; <colnr> c */

				if (npara >= 3) {
					if (npara < 5 || *(para + 4) == 0) {

						/*
						 * Color number not given.
						 */
						wsp->statflg ^= COMPMFL;
					}
					x1 = i = *para;
					y1 = j = *(para + 1);
					if (inwin(wsp, &x1, &y1) != NO) {

						/*
						 * Update graphic cursor pos.
						 * and fill the circle.
						 */
						wsp->x_gcur = i;
						wsp->y_gcur = j;
						val = 0;
						if (npara >= 4) {
							val = *(para + 3) & 0x0f;
						}
						fillcircle(wn, x1, y1, *(para + 2), val);
					}
				}
				break;

			case DRAWARC:		/* draw arc */

				/* ESC : <x> ; <y> ; <len> ; <pattnr> ; <colnr> a */

				if (npara >= 3) {
					if (npara < 5 || *(para + 4) == 0) {

						/*
						 * Color number not given.
						 */
						wsp->statflg ^= COMPMFL;
					}
					val = 0;
					if (npara >= 4) {
						val = *(para + 3) & 0x0f;
					}
					npara = NO;
					goto arclabel;
				}
				break;

			case INVARC:		/* draw inverted arc */

				/* ESC : <x> ; <y> ; <len> I */

				if (npara >= 3) {
					val = 0;
					npara = YES;
arclabel:
					x1 = *para;
					y1 = *(para + 1);
					x2 = wsp->x_gcur;
					y2 = wsp->y_gcur;
					if (inwin(wsp, &x1, &y1) != NO && inwin(wsp, &x2, &y2) != NO) {
						circle(wn, x1, y1, x2 - x1, y2 - y1, *(para + 2),
						       wsp->msk1[val], npara);
					}
				}
				break;

			case DRAWPOINT:		/* draw point */

				/* ESC : <x> ; <y> ; <op> ; <colnr> p */

				while (npara < 3) {
					*(para + npara++) = 0;
				}
				edot(wn, *para, *(para + 1), *(para + 2));
				break;

			case DEFPATT:		/* define pattern */

				/* ESC : <pattnr> ; <msk1> ; <msk2> ; <shft> ; <op> R */

				if (npara >= 5 && (val = (*para & 0x0f)) != 0) {
					wsp->msk1[val] = *(para + 1);
					wsp->msk2[val] = *(para + 2);
					wsp->shft[val] = *(para + 3);
					wsp->op[val] = *(para + 4);
				}
				break;

			case MOVAREA:		/* move area */

				/* ESC : <xsrc> ; <ysrc> ; <xdest> ; <ydest> ; */
				/*       <width> ; <height> ; <op> r           */

				if (npara >= 6) {
					val = *(para + 6);
					if (npara == 6) {
						val = 0;	/* default, don't invert */
					}
					emovea(wn, *para, *(para + 1), *(para + 2), *(para + 3),
					       *(para + 4), *(para + 5), val);
				}
				break;

			case DSR_ABC1600:	/* device status report */

				/* ESC : <sel> n */

				do {
					switch (*para++) {

					case 1:		/* report graphic cursor */

						cp = itoa(wsp->x_gcur, &repprivate[2]);
						*cp++ = ';';
						cp = itoa(wsp->y_gcur, cp);
						*cp = CPR_ABC1600;
						putonque(wn, &repprivate[0], cp - &repprivate[0] + 1, YES);
						break;

					case 2:		/* report mouse position (immediately) */

						if (wtol[wn] == 0) {
							repmous(wn);
						}
						else {
							wsp->status |= MOUSREP;
						}
						break;

					case 3:		/* report window size and current font */

#ifdef LANDSCAPE
						cp = itoa(wsp->y_size, &repprivate[2]);
						*cp++ = ';';
						cp = itoa(wsp->x_size, cp);
#else  PORTRAIT
						cp = itoa(wsp->x_size, &repprivate[2]);
						*cp++ = ';';
						cp = itoa(wsp->y_size, cp);
#endif LANDSCAPE / PORTRAIT
						*cp++ = ';';
						cp = itoa(wsp->fdata.fi_xsize, cp);
						*cp++ = ';';
						cp = itoa(wsp->fdata.fi_ysize, cp);
						*cp++ = ';';
						cp = itoa(wsp->fdata.fi_base, cp);
						*cp++ = ';';
						cp = itoa(wsp->fdata.fi_cur + 'A', cp);
						*cp = REPSFS;
						putonque(wn, &repprivate[0], cp - &repprivate[0] + 1, YES);
						break;
					}
				} while (--npara > 0);
				break;

			case PTG:	/* put text cursor at graphic cursor */

				/* ESC : <sel> H */

				do {
					i = wsp->x_gcur + wsp->x_gorig;
#ifdef LANDSCAPE
					val = wsp->y_size - wsp->fdata.fi_xsize;
#else  PORTRAIT
					val = wsp->x_size - wsp->fdata.fi_xsize;
#endif LANDSCAPE / PORTRAIT
					if (i > val) {
						i = val;
					}
#ifdef LANDSCAPE
					j = wsp->x_size - 1 - wsp->y_gcur - wsp->y_gorig;
#else  PORTRAIT
					j = wsp->y_size - 1 - wsp->y_gcur - wsp->y_gorig;
#endif LANDSCAPE / PORTRAIT
					switch (*para++) {

					case 0:		/* upper left corner */
						break;

					case 1:		/* lower left corner */
						j -= wsp->fdata.fi_ysize - 1;
						break;

					case 2:		/* left side of base line */
						j -= wsp->fdata.fi_ysize - 1 - wsp->fdata.fi_base;
						break;

					default:
						goto illpara;	/* illegal parameter */
					}
					if (j < 0) {
						j = 0;
					}
#ifdef LANDSCAPE
					val = wsp->x_size - wsp->fdata.fi_ysize;
#else  PORTRAIT
					val = wsp->y_size - wsp->fdata.fi_ysize;
#endif LANDSCAPE / PORTRAIT
					if (j > val) {
						j = val;
					}
					wsp->x_coffs = i % wsp->fdata.fi_xsize;
					wsp->y_coffs = j % wsp->fdata.fi_ysize;
					wsp->x_cur = (i - wsp->x_coffs) / wsp->fdata.fi_xsize + 1;
					wsp->y_cur = (j - wsp->y_coffs) / wsp->fdata.fi_ysize + 1;
#ifdef LANDSCAPE
					wsp->rm_cur = (wsp->y_size - wsp->x_coffs) / wsp->fdata.fi_xsize;
					wsp->bm_cur = (wsp->x_size - wsp->y_coffs) / wsp->fdata.fi_ysize;
#else  PORTRAIT
					wsp->rm_cur = (wsp->x_size - wsp->x_coffs) / wsp->fdata.fi_xsize;
					wsp->bm_cur = (wsp->y_size - wsp->y_coffs) / wsp->fdata.fi_ysize;
#endif LANDSCAPE / PORTRAIT
					if (wsp->bmr_cur > wsp->bm_cur) {
						wsp->bmr_cur = wsp->bm_cur;
					}
					if (wsp->umr_cur >= wsp->bmr_cur) {
						wsp->umr_cur = wsp->bmr_cur - 1;
					}
					if (wsp->x_coffs != 0 || wsp->y_coffs != 0) {
						wsp->status |= GRAPHIC;	/* consider text as graphics */
					}

					/*
					 * Compute the leftmost, rightmost, uppermost,
					 * and lowermost character positions that are
					 * visible.
					 */
					cvisible(wn, wpp[wn], nwpart[wn], EXTENDED);

illpara:				;
				} while (--npara > 0);
				break;

			case MCNTRL:		/* mouse control */

				/* ESC : <sel> M */

				do {
					switch (*para++) {

					case 7:		/* report when mouse change */
						wsp->status |= MOUSREP;
						if (wtol[wn] == 0 && (wsp->status & MOUSCHNG)) {
							repmous(wn);
							wsp->status &= ~(MOUSREP | MOUSBREP
								       | MOUSCHNG | MOUSBCHNG);
						}
						break;

					case 8:		/* report when button change */
						wsp->status |= MOUSBREP;
						if (wtol[wn] == 0 && (wsp->status & MOUSBCHNG)) {
							repmous(wn);
							wsp->status &= ~(MOUSREP | MOUSBREP
								       | MOUSCHNG | MOUSBCHNG);
						}
						break;
					}
				} while (--npara > 0);
				break;

			case GRORG:		/* set graphic origin */

				/* ESC : <x> ; <y> O */

#ifdef LANDSCAPE
				if (npara >= 2 && *para < wsp->y_size && *(para + 1) < wsp->x_size) {
					wsp->x_gorig = *para;
					wsp->y_gorig = *(para + 1);
					wsp->x_gcur = 0;
					wsp->y_gcur = 0;
				}
#else  PORTRAIT
				if (npara >= 2 && *para < wsp->x_size && *(para + 1) < wsp->y_size) {
					wsp->x_gorig = *para;
					wsp->y_gorig = *(para + 1);
					wsp->x_gcur = 0;
					wsp->y_gcur = 0;
				}
#endif LANDSCAPE / PORTRAIT
				break;

			case CLRALL:		/* clear all */

				/* ESC : J */

				wsp->x_cur = wsp->y_cur = wsp->umr_cur = 1;
				wsp->x_coffs = 0;
				wsp->y_coffs = 0;
				wsp->x_gcur = 0;
				wsp->y_gcur = 0;
				wsp->x_gorig = 0;
				wsp->y_gorig = 0;
#ifdef LANDSCAPE
				wsp->rm_cur = wsp->y_size / wsp->fdata.fi_xsize;
				wsp->bmr_cur = wsp->bm_cur = wsp->x_size / wsp->fdata.fi_ysize;
#else  PORTRAIT
				wsp->rm_cur = wsp->x_size / wsp->fdata.fi_xsize;
				wsp->bmr_cur = wsp->bm_cur = wsp->y_size / wsp->fdata.fi_ysize;
#endif LANDSCAPE / PORTRAIT
				if (wsp->rm_cur > MAXCOLS) {
					wsp->rm_cur = MAXCOLS;
				}
				if (wsp->font == wsp->fdata.fi_cur + 'A') {

					/*
					 * It is the default font, reset the
					 * GRAPHIC flag and the cursor saved
					 * flag for the window.
					 */
					wsp->status &= ~(GRAPHIC | CURSAVED);
				}

				/*
				 * Compute the leftmost, rightmost, uppermost,
				 * and lowermost character positions that are
				 * visible.
				 */
				cvisible(wn, wpp[wn], nwpart[wn], EXTENDED);

				/*
				 * Clear the whole virtual screen.
				 */
				wipe(wn, 0, SCXSIZE - 1, 0, SCYSIZE - 1);
				sav_esc(wsp, 0);
				break;

			case ABCLL:		/* private load LED's */

				/* ESC : <sel> q */

				do {
					switch (*para++) {

					case 0:		/* turn INS ant ALT key LED's off */

						wsp->ledflg &= ~((1 << 9) | (1 << 10));
						break;

					case 1:		/* turn INS key LED on */

						wsp->ledflg |= 1 << 9;
						break;

					case 2:		/* turn ALT key LED on */

						wsp->ledflg |= 1 << 10;
						break;
					}
				} while (--npara > 0);
				adjleds();	/* update LED's on keyboard */
				break;

			case SM_ABC1600:	/* private set mode */

				/* ESC : <n> h */

				do {
					if (*para++ == PHASEMOD) {
						wsp->termpara |= TD_PHASE;
					}
				} while (--npara > 0);
				break;

			case RM_ABC1600:	/* private reset mode */

				/* ESC : <n> l */

				do {
					if (*para++ == PHASEMOD) {
						wsp->termpara &= ~TD_PHASE;
					}
				} while (--npara > 0);
				break;

			case SPRAY:		/* spray with pattern */

				/* ESC : <x> ; <y> ; <pattnr> ; <op> s */

				if (npara >= 2) {
					x1 = i = *para;
					y1 = j = *(para + 1);
					val = 0;
					if (npara >= 4) {
						val = *(para + 3);
					}
					if (val <= 3 && inwin(wsp, &x1, &y1) != NO) {
						wsp->x_gcur = i;
						wsp->y_gcur = j;
						i = 0;
						if (npara >= 3) {
							i = *(para + 2) & 0x0f;
						}
						spray(wn, x1, y1, i, val);
					}
				}
				break;

			}	/* switch (c) */

			wsp->statflg = savflags;	/* restore mover status flags */

		}	/* else (ABC1600 private) */

	}	/* if (wsp->status & (ESCTRLSEQ | ESCGRSEQ)) */

	else {

		/*
		 * Escape sequence without parameters or select character set.
		 */
		while (--len >= 0) {
			c = *str & CMASK;

			if (Endescseq(c)) {
				break;
			}
			else if (c == ESC) {
				str++;
				wsp->status &= ~ESCMODE;
				goto restart;	/* start a new sequence */
			}
			else if (c == CAN || c == SUB) {

				/*
				 * Abort the escape sequence.
				 */
				wsp->status &= ~ESCMODE;
				return(++str);
			}
			else if (c < ' ') {	/* execute control character */
				wsp->status &= ~ESCMODE;
				win_out(wn, str, 1);
				wsp->status |= ESCMODE;
			}

			str++;		/* next character */

		}	/* while (--len >= 0) */

		if (len < 0) {
			return(str);		/* escape sequence not complete */
		}
		str++;		/* point beyond last processed character */

		if (wsp->status & SELCHSET) {

			/*
			 * Select character set.
			 */
			c -= 'A';
			if (c < MAXFONTCNT && c != wsp->fdata.fi_cur) {
				newfont(wn, c);		/* change font */
			}
		}
		else {

			/*
			 * Escape sequence without parameters.
			 */
			switch (c) {

			case NEL:	/* next line */

				savterm = wsp->termpara;	/* save status */
				wsp->termpara |= TD_NL;		/* '\n' is CR-LF */
				wsp->status &= ~ESCMODE;	/* not escape */
				win_out(wn, &newlin, 1);
				wsp->termpara = savterm;	/* restore status */
				break;

			case IND:	/* index */

				if (wsp->y_cur == wsp->bmr_cur) {	/* if last line in region */
					if (wsp->termpara & TD_NOSCR) {

						/*
						 * No scroll (page) mode.
						 */
						wsp->y_cur = wsp->umr_cur;
					}
					else {

						/*
						 * Scroll 1 line .
						 */
						scrollwin(wn, 1);
					}
				}
				else if (wsp->y_cur > 1) {
					wsp->y_cur++;		/* move cursor down */
				}
				break;

			case RI:	/* reverse index */

				if (wsp->y_cur == wsp->umr_cur) {	/* if first line in region */
					if (wsp->termpara & TD_NOSCR) {

						/*
						 * No scroll (page) mode.
						 */
						wsp->y_cur = wsp->bmr_cur;
					}
					else {

						/*
						 * Scroll 1 line (reversed).
						 */
						scrollwin(wn, -1);
					}
				}
				else if (wsp->y_cur > 1) {
					wsp->y_cur--;		/* move cursor up */
				}
				break;

			case HTS:	/* horizontal tabulation set */

				i = wsp->x_cur - 1;
				wsp->tabstop[(unsigned)i/TP_PER_EL] |= 1 << (i % TP_PER_EL);
				break;

			case SC:	/* save cursor */

				wsp->pshc.sx_cur = wsp->x_cur;
				wsp->pshc.sy_cur = wsp->y_cur;
				wsp->pshc.sx_gcur = wsp->x_gcur;
				wsp->pshc.sy_gcur = wsp->y_gcur;
				wsp->pshc.sx_gorig = wsp->x_gorig;
				wsp->pshc.sy_gorig = wsp->y_gorig;
				wsp->pshc.sx_coffs = wsp->x_coffs;
				wsp->pshc.sy_coffs = wsp->y_coffs;
				wsp->pshc.srm_cur = wsp->rm_cur;
				wsp->pshc.sbm_cur = wsp->bm_cur;
				wsp->pshc.sumr_cur = wsp->umr_cur;
				wsp->pshc.sbmr_cur = wsp->bmr_cur;
				wsp->pshc.stermpara = wsp->termpara;
				wsp->pshc.sstatus = wsp->status;	/* save GRAPHIC flag */
				wsp->pshc.scurfnt = wsp->fdata.fi_cur;
				wsp->status |= CURSAVED;	/* set cursor saved flag */
				break;

			case RC:	/* restore cursor */

				if (wsp->status & CURSAVED) {	/* if cursor saved */
					if (wsp->pshc.scurfnt != wsp->fdata.fi_cur) {
						if (adjfont(wsp->pshc.scurfnt, &wsp->fdata) != OK) {

							/*
							 * Cannot load font,
							 * ignore escape
							 * sequence.
							 */
							break;
						}
					}
					wsp->x_cur = wsp->pshc.sx_cur;
					wsp->y_cur = wsp->pshc.sy_cur;
					wsp->x_gcur = wsp->pshc.sx_gcur;
					wsp->y_gcur = wsp->pshc.sy_gcur;
					wsp->x_gorig = wsp->pshc.sx_gorig;
					wsp->y_gorig = wsp->pshc.sy_gorig;
					wsp->x_coffs = wsp->pshc.sx_coffs;
					wsp->y_coffs = wsp->pshc.sy_coffs;
					wsp->rm_cur = wsp->pshc.srm_cur;
					wsp->bm_cur = wsp->pshc.sbm_cur;
					wsp->umr_cur = wsp->pshc.sumr_cur;
					wsp->bmr_cur = wsp->pshc.sbmr_cur;
					wsp->termpara &= ~(TD_REVERSE | TD_USCORE);
					wsp->termpara |= wsp->pshc.stermpara & (TD_REVERSE | TD_USCORE);
					if (wsp->termpara & TD_REVERSE) {
						wsp->chstatflg = wsp->statflg ^ COMPMFL;
					}
					else {
						wsp->chstatflg = wsp->statflg;
					}
					cvisible(wn, wpp[wn], nwpart[wn], EXTENDED);
				}
				else {				/* cursor home */
					wsp->x_cur = 1;
					if (wsp->termpara & TD_ORIGIN) {
						wsp->y_cur = wsp->umr_cur;
					}
					else {
						wsp->y_cur = 1;
					}
					wsp->x_gcur = 0;
					wsp->y_gcur = 0;
					wsp->x_gorig = 0;
					wsp->y_gorig = 0;
				}
				break;

			case RIS:	/* reset to initial state */

				wsp->x_cur = wsp->y_cur = 1;
				wsp->x_gcur = 0;
				wsp->y_gcur = 0;
				wsp->status |= MOUSCHNG | NOFONT;
				wsp->status &= ~(CURSAVED | ENDLINE | GRAPHIC | MOUSREP);
				wsp->estatus &= ~NOCURSOR;

				/*
				 * Invert the window to its normal state if
				 * necessary.
				 */
				if ((wsp->termpara ^ ttydflt.td_term) & TD_SCREEN) {
					invertwin(wn);
				}

				/*
				 * Do some more things.
				 */
				initwin(wsp);

				/*
				 * Change to the default font.
				 */
				wsp->fdata.fi_cur = wsp->font - 'A';
				(void)adjfont(wsp->fdata.fi_cur, &wsp->fdata);

				/*
				 * Compute the leftmost, rightmost, uppermost,
				 * and lowermost characters visible in the
				 * window.
				 */
				cvisible(wn, wpp[wn], nwpart[wn], EXTENDED);

				/*
				 * Erase the window.
				 */
				erasescr(wn, 0);

				/*
				 * Clear all LED's.
				 */
				adjleds();
				break;

			}	/* switch (c) */
		}	/* else */
	}	/* else */

	wsp->status &= ~ESCMODE;	/* clear escape flag */

	/*
	 * Check if a new escape sequence is following immediately and in such
	 * case, process it immediately.
	 */
	if (len > 0 && *str == ESC) {
		str++;
		len--;
		goto restart;
	}

	return(str);
}	/* end of eschand() */

#undef	Intermed
#undef	Endescseq
#undef	Endctrlseq



/*
 * Report the mouse position.
 */

void
repmous(wn)
int	wn;	/* window to report to */
{
	register		 val;
	register struct	winint	 *wsp;		/* pointer to window status */
	register struct	pntstruc *pdp;		/* pointer to mouse status  */
	register	char	 *cp;
	char			 *itoa();

	wsp = wstatp[wn];
	pdp = &pntdata;

	/*
	 * Compute the x position.
	 */
#ifdef LANDSCAPE
	val = pdp->pd_ympos - wsp->y_orig;
	if (val < 0) {
		val = 0;
	}
	else if ((pix_d)val >= wsp->y_size) {
		val = wsp->y_size - 1;
	}
#else  PORTRAIT
	val = pdp->pd_xmpos - wsp->x_orig;
	if (val < 0) {
		val = 0;
	}
	else if ((pix_d)val >= wsp->x_size) {
		val = wsp->x_size - 1;
	}
#endif LANDSCAPE / PORTRAIT
	cp = itoa(val, &repprivate[2]);
	*cp++ = ';';

	/*
	 * Compute the y position.
	 */
#ifdef LANDSCAPE
	val = pdp->pd_xmpos - wsp->x_orig;
	if (val < 0) {
		val = 0;
	}
	else if ((pix_d)val >= wsp->x_size) {
		val = wsp->x_size - 1;
	}
#else  PORTRAIT
	val = wsp->y_size - 1 - pdp->pd_ympos + wsp->y_orig;
	if (val < 0) {
		val = 0;
	}
	else if ((pix_d)val >= wsp->y_size) {
		val = wsp->y_size - 1;
	}
#endif LANDSCAPE / PORTRAIT
	cp = itoa(val, cp);
	*cp++ = ';';

	/*
	 * The status of the left and possible the middle mouse button.
	 */
	val = '0';
	if (pdp->pd_flags & PD_LPRESS) {
		val++;
	}
	if ((wsp->estatus & NOCPOUT) && (pdp->pd_flags & PD_MPRESS)) {
		val += 2;
	}
	*cp++ = val;
	*cp = MPR;
	putonque(wn, &repprivate[0], cp - &repprivate[0] + 1, YES);

	return;
}	/* end of repmous() */



/*
 * This function checks if the LED flags for the top level window are equal
 * to the state of the keyboard LED's, and if not adjusts the keyboard LED's.
 */

void
adjleds()
{
	static	byte	oncode[] = {	/* codes to send to the keyboard to */
		0,	/* not used */	/* turn the LED's on                */
		K_ONLD1,/* LED 1 */
		K_ONLD2,/* LED 2 */
		K_ONLD3,/* LED 3 */
		K_ONLD4,/* LED 4 */
		K_ONLD5,/* LED 5 */
		K_ONLD6,/* LED 6 */
		K_ONLD7,/* LED 7 */
		K_ONLD8,/* LED 8 */
		K_ONINS,/* INS key LED */
		K_ONALT	/* ALT key LED */
	};
	static	byte	offcode[] = {	/* codes to send to the keyboard to */
		0,	/* not used */	/* turn the LED's off               */
		K_OFLD1,/* LED 1 */
		K_OFLD2,/* LED 2 */
		K_OFLD3,/* LED 3 */
		K_OFLD4,/* LED 4 */
		K_OFLD5,/* LED 5 */
		K_OFLD6,/* LED 6 */
		K_OFLD7,/* LED 7 */
		K_OFLD8,/* LED 8 */
		K_OFINS,/* INS key LED */
		K_OFALT	/* ALT key LED */
	};
	static	 word	kledflg = 0;	/* flags indicating which LED's are
					   turned on, bit 1 to 8 corresponds
					   to LED's 1 to 8 (bit 0 not used)
					   bit 9 is the INS key LED and bit
					   10 the ALT key LED               */
	register word	flags;		/* LED flags for top window         */
	register word	mask;		/* mask                             */
	register	i;		/* loop index                       */
	register byte	val;		/* value to write to keyboard       */

	if (nwin == 0) {
		flags = 0;
	}
	else {
		flags = wstatp[ltow[0]]->ledflg;
	}
	mask = 1;
	for (i = 1 ; i <= 10 ; i++) {
		mask <<= 1;
		if ((kledflg ^ flags) & mask) {

			/*
			 * This LED must be changed.
			 */
			if (flags & mask) {
				val = oncode[i];
			}
			else {
				val = offcode[i];
			}
			if (sendkybrd(val) < 0) {
				flags ^= mask;
			}
		}
	}
	kledflg = flags;	/* save new status of keyboard LED's */

	return;
}	/* end of adjleds() */



/*
 * This function converts a positive integer to an ASCII string.
 * A pointer beyond the ASCII string is returned.
 * For sequrity reasons it is checked if the integer is positive, and if not
 * it is converted to a '0'.
 */

char *
itoa(val, cp)
register	val;		/* integer to convert      */
register char	*cp;		/* where to put the string */
{
	register char	*tmpcp;		/* temporary pointer           */
	char		tmpbuf[8];	/* temporary conversion buffer */

	tmpcp = &tmpbuf[8];
	if (val <= 0) {
		*--tmpcp = '0';
	}
	else {
		do {
			*--tmpcp = val % 10 + '0';
		} while ((val /= 10) > 0);
	}

	/*
	 * Copy the result to the desired destination.
	 */
	do {
		*cp++ = *tmpcp++;
	} while (tmpcp < &tmpbuf[8]);

	return(cp);
}	/* end of itoa() */
