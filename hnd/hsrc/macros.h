/* macros.h */

/*
 * This file contains macro definitions for the ABC1600 window handler.
 */

/* 1984-07-10, Peter Andersson, Luxor Datorer AB (at DIAB) */

/*
 * ***** THE HEADER FILE defs.h MUST BE INCLUDED BEFORE THIS FILE *****
 */


/*
 * Write a byte, word (2 bytes), and a long word (4 bytes) to the specified
 * address.
 */

#define	Outb(val,addr)	(*((byte *)(addr)) = (byte)(val))
#ifdef NS32
#define	Outw(val,addr)	Outword(val, addr)
#define	Outl(val,addr)	Outdouble(val, addr)
#else  M68K
#define	Outw(val,addr)	(*((word *)(addr)) = (word)(val))
#define	Outl(val,addr)	(*((dword *)(addr)) = (dword)(val))
#endif NS32 / M68K

/*
 * Or, And, and Xor operations on a byte and a long (double) word.
 */

#define	Orb(val,addr)	(*((byte *)(addr)) |= (byte)(val))
#define	Andb(val,addr)	(*((byte *)(addr)) &= (byte)(val))
#define	Xorb(val,addr)	(*((byte *)(addr)) ^= (byte)(val))
#ifdef NS32
#define	Orl(val,addr)	Ordouble(val, addr)
#define	Andl(val,addr)	Anddouble(val, addr)
#define	Xorl(val,addr)	Xordouble(val, addr)
#else  M68K
#define	Orl(val,addr)	(*((dword *)(addr)) |= (dword)(val))
#define	Andl(val,addr)	(*((dword *)(addr)) &= (dword)(val))
#define	Xorl(val,addr)	(*((dword *)(addr)) ^= (dword)(val))
#endif NS32 / M68K

/*
 * Read a byte or a long word (4 bytes) from the specified address.
 */

#define	Inb(addr)	(*((byte *)(addr)))
#ifdef NS32
#define	Inl(addr)	Indouble(addr)
#else  M68K
#define	Inl(addr)	(*((dword *)(addr)))
#endif NS32 / M68K

/*
 * Wait until mover ready.
 * 'Waits' and 'Waitsp' is equivalent, except that 'Waitsp' uses a pointer
 * to the structure containing the hardware addresses.
 */

#define	Waits()		while (Inb(hwa.mstatusport) & BUSYFLG) ;
#define	Waitsp()	while (Inb(hwap->mstatusport) & BUSYFLG) ;

/*
 * Set the screen to graphic mode, i.e. set to hold mover counters and (if the
 * pixels shall not just be inverted) initialize the graphic mask to use.
 */

#define	Setlgrmod(s,grpp,mask, flag)	{ Waitsp(); \
					  if (flag != NO) { \
						  Outb(GSTATFLG | COMPMFL, hwap->mflagport); \
					  } \
					  else { \
						  Outb(s | HOLDYFL | HOLDXFL, hwap->mflagport); \
						  grpp = GRPHXCOORD + 15; \
						  Outw(mask, hwap->grphaddr); \
					  } \
					  Outl(UX_UY << 16, hwap->x_sizeport); }

/*
 * Set or invert a pixel dot at position x,y.
 */

#define	Setldot(x,y,grpp,flag)	{ Outw(x, hwap->x_toport); \
				  Outw(y, hwap->y_toport); \
				  if (flag != NO) { \
					  Outw(x, hwap->x_fromport); \
					  Outw(y, hwap->y_fromport); \
				  } \
				  else { \
					  Outw(grpp, hwap->x_fromport); \
					  Outw(GRPHYCOORD, hwap->y_fromport); \
				  } }

/*
 * Move a rectangle with upper left corner at fromx,fromy and size sizex+1,
 * sizey+1 to tox,toy. Flag indicates whether the mover counters shall count
 * up or down.
 */

#define	Move(fromx,fromy,tox,toy,sizex,sizey,flg)	{ Outw((sizex) | (flg),hwap->x_sizeport); \
							  Outw(sizey,hwap->y_sizeport); \
							  Outw(tox,hwap->x_toport); \
							  Outw(toy,hwap->y_toport); \
							  Outw(fromx,hwap->x_fromport); \
							  Outw(fromy,hwap->y_fromport); }

/*
 * Move an area from 'from' to 'to' of size 'size'. 'from', 'to', and 'size'
 * are supposed to be in mover coordinates and 'size' to contain the mover
 * count up/down flags.
 */

#define	Movel(from,to,size)	{ Outl(size,hwap->x_sizeport); \
				  Outl(to,hwap->x_toport); \
				  Outl(from,hwap->x_fromport); }

/*
 * This macro draws a vertical line.
 * It supposes the x and y mover counters to have been hold.
 */

#define	Vline(x,y,len)	Fillrect(x,y,0,len)

/*
 * This macro draws a horizontal line.
 * It supposes the x and y mover counters to have been hold.
 */

#define	Hline(x,y,len)	Fillrect(x,y,len,0)

/*
 * This macro clears a rectangle.
 * It supposes the x and y mover counters to have been hold.
 */

#define	Clearrect(x,y,xsize,ysize)	{ Outw((xsize) | UX_UY, hwap->x_sizeport); \
					  Outw(ysize, hwap->y_sizeport); \
					  Outw(x, hwap->x_toport); \
					  Outw(y, hwap->y_toport); \
					  Outl((CLRXCOORD << 16) | CLRYCOORD, hwap->x_fromport); }

/*
 * This macro fills a rectangle.
 * It supposes the x and y mover counters to have been hold.
 */

#define	Fillrect(x,y,xsize,ysize)	{ Outw((xsize) | UX_UY, hwap->x_sizeport); \
					  Outw(ysize, hwap->y_sizeport); \
					  Outw(x, hwap->x_toport); \
					  Outw(y, hwap->y_toport); \
					  Outl((FILLXCOORD << 16) | FILLYCOORD, hwap->x_fromport); }

/*
 * This macro fills a rectangle with a pattern.
 * It supposes 'hwa.grphaddr' to be filled with an appropriate pattern and
 * the x and y mover counters to have been hold.
 */

#define	Grphrect(x,y,xsize,ysize)	{ Outw((xsize) | UX_UY, hwap->x_sizeport); \
					  Outw(ysize, hwap->y_sizeport); \
					  Outw(x, hwap->x_toport); \
					  Outw(y, hwap->y_toport); \
					  Outl((GRPHXCOORD << 16) | GRPHYCOORD, hwap->x_fromport); }

/*
 * Make sure that the screen is not dimmed. This is done by setting the turn
 * down screen counter in the shared memory page to the start value for the
 * blank counter.
 * 'smemp' is a pointer to the shared memory page.
 */
#define	Screen_on(smemp)	((smemp)->blnkcnt = (smemp)->blnkstr)

/*
 * Max and min macros.
 */
#define	Max(x,y)	(((x) >= (y)) ? (x) : (y))
#define	Min(x,y)	(((x) <= (y)) ? (x) : (y))

/*
 * Absolute value macro.
 */
#define	Abs(x)		(((x) >= 0) ? (x) : (-(x)))

/*
 * Division by 2 and 16.
 * WARNING: These macros should only be used with a positive argument (zero is
 * positive), otherwise the result may be incorrect.
 */
#define	Div2(x)		((x) >> 1)
#define	Div16(x)	((x) >> 4)

/*
 * Macro to multiply two short's giving a long as result.
 * This macro forces the Microsoft XENIX compiler to use the MULS instruction
 * of the M68K processor.
 */
#define	Muls(a, b)	((short)(a) * (short)(b))

/*
 * Given a coordinate x,y this macro returns the number of bytes from the
 * start of the picture memory.
 */
#define	Ctoaddr(x,y)	(((x) >> 3) + ((y) << 7))

/*
 * Given a coordinate x, this macro returns the bit in a byte corresponding
 * to the pixel.
 */
#define	Ctobit(x)	(1 << (7 - ((x) & 7)))
