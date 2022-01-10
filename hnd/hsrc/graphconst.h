/* graphconst.h */

/*
 * This file contains address constants and other constants used to access
 * the ABC1600 graphic card.
 * Also addresses to the memory page shared with the tty device driver are
 * included.
 */

/* 1984-07-10, Peter Andersson, Luxor Datorer AB (at DIAB) */

/*
 * Offsets from the start of the mapped in I/O memory to some ports on the
 * graphic card.
 * Note: This memory is mapped in together with the shared memory.
 */

#ifdef NS32
#define		SIZEOFFS	0x0800		/* size port         */
#define		TOOFFS		0x0804		/* to port           */
#define		FROMOFFS	0x0900		/* from port         */
#define		MSTATUSOFFS	0x0800		/* mover status port */
#define		MFLAGOFFS	0x0a03		/* mover flags port  */
#else  M68K
#define		SIZEOFFS	0x7800		/* size port         */
#define		TOOFFS		0x7804		/* to port           */
#define		FROMOFFS	0x7900		/* from port         */
#define		MSTATUSOFFS	0x7800		/* mover status port */
#define		MFLAGOFFS	0x7a03		/* mover flags port  */
#endif NS32 / M68K

/*
 * Offset to start of the memory page shared with the tty device driver.
 * Note: This memory is mapped in together with the I/O memory.
 */

#define		PAGEOFFS	0x4000	/* offset from returned address  */


/*
 * Different sizes.
 */
#define		PICTSIZE	1024		/* size of picture memory in
						   pixels                    */
#define		SCXSIZE		768		/* width of screen in portrait
						   mode                      */
#define		SCYSIZE		1024		/* height of screen in portrait
						   mode                      */
#define		BIG		2000		/* number larger than any screen
						   coordinate                */
#define		SMALL		(-1)		/* number smaller than any
						   screen coordinate         */

/*
 * Bits within the flag byte written to MFLAGPORT port.
 */

#define		LAPOFL		1	/* landscape/portrait flag, affects the behaviour of
					   the mover counters                                 */
#define		BLANKFL		2	/* screen blank when this flag set, i.e. video off    */
#define		PIXPOLFL	4	/* pixel polarity flag, true inverse video            */
#define		FRAMEFL		8	/* frame flag                                         */
#define		HOLDYFL		16	/* hold from y counter flag                           */
#define		HOLDXFL		32	/* hold from x counter flag                           */
#define		COMPMFL		64	/* complement mover flag (inverts)                    */
#define		SETFL		128	/* high: set/reset mode, low: replace mode
					   set mode if WRMASKPORT=ffff, reset if WRMASKPORT=0 */

/*
 * Default value to write to mover flag port.
 */
#define		GSTATFLG	0

/*
 * Masks for the mover status port.
 */

#define		POLAFLG		2	/* portrait/landscape flag */
#define		BUSYFLG		128	/* mover busy flag         */

/*
 * Sizes of boxes and indicators in the window borders.
 */

#define		SBOXSIZE	16	/* short side of a box                 */
#define		LBOXSIZE	24	/* long side of a box                  */
#define		WINDSIZE	13	/* width of visible indicator          */
#define		PINDSIZE	14	/* size in pointing direction of above */

/*
 * Addresses within picture memory used for special purposes.
 */

#define		FILLXCOORD	768	/* this word shall be filled */
#define		FILLYCOORD	0

#define		CLRXCOORD	768	/* this word shall be cleared  */
#define		CLRYCOORD	1

#define		GRPHXCOORD	768	/* word used for mask for graphics */
#define		GRPHYCOORD	2

#ifdef LANDSCAPE
#define		SETUPXCOORD	992	/* area used to set up character block */
#define		SETUPYCOORD	0	/* also used when constructing the mouse pointer */
#else  PORTRAIT
#define		SETUPXCOORD	768	/* area used to set up character block */
#define		SETUPYCOORD	992	/* also used when constructing the mouse pointer */
#endif LANDSCAPE / PORTRAIT

#define		XPAINTBUF	768	/* area used to construct the scan   */
#define		YPAINTBUF	992	/* buffer used by the paint routines */
					/* (3 pixel rows are used)           */

#define		XSPRAYCOORD	768	/* area used by the spray escape seq */
#define		YSPRAYCOORD	992	/* 32 (y) times 64 (x) pixels used   */

#define		BGPXCOORD	880	/* area to hold current background pattern */
#define		BGPYCOORD	0
#define		BGPXSIZE	32
#define		BGPYSIZE	32

#define		SIZBXCOORD	784	/* size box */
#define		SIZBYCOORD	0

#define		CLOSBXCOORD	784	/* close box */
#define		CLOSBYCOORD	16

#define		MOVBXCOORD	864	/* move box */
#define		MOVBYCOORD	0

#define		ZOOMBXCOORD	944	/* zoom box */
#define		ZOOMBYCOORD	0

#define		BLWBXCOORD	944	/* blow up box */
#define		BLWBYCOORD	16

#define		HLPBXCOORD	960	/* help box */
#define		HLPBYCOORD	0

#ifdef LANDSCAPE
#define		SCRDXCOORD	832	/* scroll down box */
#define		SCRDYCOORD	0
#else  PORTRAIT
#define		SCRDXCOORD	800	/* scroll down box */
#define		SCRDYCOORD	0
#endif LANDSCAPE / PORTRAIT

#ifdef LANDSCAPE
#define		SCRUXCOORD	832	/* scroll up box */
#define		SCRUYCOORD	16
#else  PORTRAIT
#define		SCRUXCOORD	816	/* scroll up box */
#define		SCRUYCOORD	0
#endif LANDSCAPE / PORTRAIT

#ifdef LANDSCAPE
#define		SCRLXCOORD	816	/* scroll left box */
#define		SCRLYCOORD	0
#else  PORTRAIT
#define		SCRLXCOORD	832	/* scroll left box */
#define		SCRLYCOORD	0
#endif LANDSCAPE / PORTRAIT

#ifdef LANDSCAPE
#define		SCRRXCOORD	800	/* scroll right box */
#define		SCRRYCOORD	0
#else  PORTRAIT
#define		SCRRXCOORD	832	/* scroll right box */
#define		SCRRYCOORD	16
#endif LANDSCAPE / PORTRAIT

#define		SCRBVIS		(2 * LBOXSIZE + 5)
					/* scroll boxes visible if the window
					   size is at least this */

#ifdef LANDSCAPE
#define		VINDXCOORD	864	/* vertical visible indicator */
#define		VINDYCOORD	16
#else  PORTRAIT
#define		VINDXCOORD	768	/* vertical visible indicator */
#define		VINDYCOORD	17
#endif LANDSCAPE / PORTRAIT

#ifdef LANDSCAPE
#define		HINDXCOORD	768	/* horizontal visible indicator */
#define		HINDYCOORD	17
#else  PORTRAIT
#define		HINDXCOORD	864	/* horizontal visible indicator */
#define		HINDYCOORD	16
#endif LANDSCAPE / PORTRAIT

#define		CINDXCOORD	768	/* to remove visible indicators */
#define		CINDYCOORD	3

#define		INDVIS		(SCRBVIS + WINDSIZE)
					/* visible indicator visible if the
					    window size is at least this */

#define		MOUSXCOORD	912	/* to save contents under mouse pointer */
#define		MOUSYCOORD	0

/*
 * Constants used to choose how the mover counters shall count.
 */

#define		UX_UY		0x0c00		/* up in both x and y    */
#define		UX_DY		0x0800		/* up in x and down in y */
#define		DX_UY		0x0400		/* down in x and up in y */
#define		DX_DY		0x0000		/* down in both x and y  */
