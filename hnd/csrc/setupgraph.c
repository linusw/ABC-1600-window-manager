/* setupgraph.c */

/*
 * This file contains things to initialize graphics for the ABC1600 window
 * handler.
 */

/* 1985-06-04, Peter Andersson, Luxor Datorer AB */

/*
 * Functions in this file:
 *	setupgr()
 */


#include	"../hsrc/defs.h"	/* definitions               */
#include	"../hsrc/structs.h"	/* structure declarations    */
#include	"../hsrc/eglobals.h"	/* external global variables */
#include	"../hsrc/graphconst.h"
#include	"../hsrc/macros.h"



/*
 * Data to set up the different boxes used in the borders.
 */
#ifdef LANDSCAPE

/*
 * Size box data.
 */
static	word	sizbd[] = {
	0xffff,
	0x8101,
	0x8101,
	0x8101,

	0x8101,
	0x8101,
	0x8101,
	0x8101,

	0x81ff,
	0x8201,
	0x8401,
	0x8801,

	0x9001,
	0xa001,
	0xc001,
	0xffff
};

/*
 * Move box data.
 */
static	word	movbd[] = {
	0xffff,
	0x8001,
	0xbfe1,
	0xa021,

	0xa021,
	0xa7fd,
	0xa405,
	0xa405,

	0xa405,
	0xa405,
	0xbc05,
	0x8405,

	0x8405,
	0x87fd,
	0x8001,
	0xffff
};

/*
 * Help box data.
 */
static	word	hlpbd[] = {
	0xffff,
	0x8001,
	0x8001,
	0x8031,

	0x8039,
	0x801d,
	0x800d,
	0xb60d,

	0xb70d,
	0x838d,
	0x81dd,
	0x80f9,

	0x8071,
	0x8001,
	0x8001,
	0xffff
};
#else  PORTRAIT

/*
 * Size box data.
 */
static	word	sizbd[] = {
	0xffff,
	0x8081,
	0x8081,
	0x8081,

	0x8081,
	0x8081,
	0x8081,
	0x8081,

	0xff81,
	0x8041,
	0x8021,
	0x8011,

	0x8009,
	0x8005,
	0x8003,
	0xffff
};

/*
 * Move box data.
 */
static	word	movbd[] = {
	0xffff,
	0x8001,
	0x87fd,
	0x8405,

	0x8405,
	0xbc05,
	0xa405,
	0xa405,

	0xa405,
	0xa405,
	0xa7fd,
	0xa021,

	0xa021,
	0xbfe1,
	0x8001,
	0xffff
};

/*
 * Help box data.
 */
static	word	hlpbd[] = {
	0xffff,
	0x8001,
	0x87e1,
	0x8ff1,

	0x9c39,
	0x9819,
	0x8039,
	0x8071,

	0x80e1,
	0x81c1,
	0x8181,
	0x8001,

	0x8181,
	0x8181,
	0x8001,
	0xffff
};
#endif LANDSCAPE / PORTRAIT

/*
 * Close box data.
 */
static	word	closbd[] = {
	0xffff,
	0x8001,
	0x8001,
	0x9ff9,

	0x9009,
	0x9429,
	0x9249,
	0x9189,

	0x9189,
	0x9249,
	0x9429,
	0x9009,

	0x9ff9,
	0x8001,
	0x8001,
	0xffff
};

/*
 * Blow up box data.
 */
static	word	blwbd[] = {
	0xffff,
	0x8001,
	0x8001,
	0x8181,

	0x8241,
	0x8421,
	0x8811,
	0x9009,

	0x9009,
	0x8811,
	0x8421,
	0x8241,

	0x8181,
	0x8001,
	0x8001,
	0xffff
};

/*
 * Scroll down box data in portrait and scroll right box data in landscape.
 * The data for the scroll up/left box is obtained if this array is read
 * backwards.
 */
static	word	scrdb[] = {
	0xffff,
	0x8001,
	0x8001,
	0x83c1,

	0x8241,
	0x8241,
	0x8241,
	0x8241,

	0x8241,
	0x8241,
	0x8241,
	0x8241,

	0x8241,
	0x8241,
	0x8241,
	0x9e79,

	0x9009,
	0x8811,
	0x8421,
	0x8241,

	0x8181,
	0x8001,
	0x8001,
	0xffff
};

/*
 * Zoom box data.
 */
#ifdef LANDSCAPE
static	word	zoombd[] = {
	0xffff,
	0x8001,
	0x8181,
	0x83c1,

	0x8241,
	0x8661,
	0x8421,
	0x8c31,

	0x8811,
	0x8ff1,
	0x8001,
	0x8ff1,

	0x8421,
	0x83c1,
	0x8001,
	0xffff
};
#else  PORTRAIT
static	word	zoombd[] = {
	0xffff,
	0x8001,
	0x8001,
	0x8001,

	0x81d1,
	0x8759,
	0x9c55,
	0xb055,

	0xb055,
	0x9c55,
	0x8759,
	0x81d1,

	0x8001,
	0x8001,
	0x8001,
	0xffff
};
#endif LANDSCAPE / PORTRAIT

/*
 * Scroll right box data in landscape and scroll up data in portrait.
 */
static	dword	scrrb[] = {
	0xffffff00,
	0x80000100,
	0x80000100,
	0x80018100,

	0x80014100,
	0x80012100,
	0x9fff1100,
	0x90000900,

	0x90000900,
	0x9fff1100,
	0x80012100,
	0x80014100,

	0x80018100,
	0x80000100,
	0x80000100,
	0xffffff00
};

/*
 * Scroll left box data in portrait and scroll down box data in landscape.
 */
static	dword	scrlb[] = {
	0xffffff00,
	0x80000100,
	0x80000100,
	0x81800100,

	0x82800100,
	0x84800100,
	0x88fff900,
	0x90000900,

	0x90000900,
	0x88fff900,
	0x84800100,
	0x82800100,

	0x81800100,
	0x80000100,
	0x80000100,
	0xffffff00
};

/*
 * Vertical visible part indicator data.
 */
#ifdef LANDSCAPE
static	word	vind[] = {
	0x0200,
	0x0200,
	0x0700,
	0x0700,

	0x0f80,
	0x0f80,
	0x1fc0,
	0x1c40,

	0x31e0,
	0x27e0,
	0x71f0,
	0x7c70,

	0xfff8,
	0xfff8
};
#else  PORTRAIT
static	word	vind[] = {
	0x000c,
	0x003c,
	0x00fc,
	0x03fc,

	0x0eec,
	0x3eec,
	0xfe4c,
	0x3f5c,

	0x0f1c,
	0x03bc,
	0x00fc,
	0x003c,

	0x000c
};
#endif LANDSCAPE / PORTRAIT

/*
 * Horizontal visible part indicator data.
 */
#ifdef LANDSCAPE
static	word	hind[] = {
	0xc000,
	0xf000,
	0xfc00,
	0xff00,

	0x81c0,
	0xf7f0,
	0xf7fc,
	0xf7f0,

	0x81c0,
	0xff00,
	0xfc00,
	0xf000,

	0xc000
};
#else  PORTRAIT
static	word	hind[] = {
	0x0200,
	0x0200,
	0x0700,
	0x0700,

	0x0f80,
	0x0f80,
	0x1fc0,
	0x1740,

	0x3760,
	0x3060,
	0x7770,
	0x7770,

	0xf778,
	0xfff8
};
#endif LANDSCAPE / PORTRAIT


/*
 * This function sets up areas in the video memory which will be used for
 * different graphic purposes.
 */

void
setupgr()
{
	register	i;	/* counter                   */
	register	grey;	/* "grey" bit pattern        */
	register char	*paddr;	/* pointer to graphic memory */
	register char	*x1;	/* address in video memory   */
	register char	*x2;
	register char	*x3;
	register dword	x4;
	register dword	x5;
	register dword	x6;
	register dword	x7;
	char		*x8;

	paddr = (char *)hwa.pictmemst;
	Waits();
	Outw(~0, paddr + (FILLYCOORD * PICTSIZE + FILLXCOORD) / 8);	/* filled word */
	Outw(0, paddr + (CLRYCOORD * PICTSIZE + CLRXCOORD) / 8);	/* cleared word */

#ifdef LANDSCAPE
	grey = ~0xaaaaaaaa;	/* fill area to be used to draw the */
#else  PORTRAIT
	grey = 0xaaaaaaaa;	/* background */
#endif LANDSCAPE / PORTRAIT
	i = BGPYSIZE;
	x1 = paddr + (BGPYCOORD * PICTSIZE + BGPXCOORD) / 8;
	do {
		grey = ~grey;
		Outl(grey, x1);
		x1 += PICTSIZE / 8;
	} while (--i != 0);

	/*
	 * Set up the different boxes to use in the border of the windows.
	 */
	x1 = paddr + (SIZBYCOORD * PICTSIZE + SIZBXCOORD) / 8;
	x2 = paddr + (CLOSBYCOORD * PICTSIZE + CLOSBXCOORD) / 8;
#ifdef LANDSCAPE
	x3 = paddr + (SCRUYCOORD * PICTSIZE + SCRUXCOORD) / 8;
	x4 = (dword)paddr + (SCRDYCOORD * PICTSIZE + SCRDXCOORD) / 8;
#else  PORTRAIT
	x3 = paddr + (SCRRYCOORD * PICTSIZE + SCRRXCOORD) / 8;
	x4 = (dword)paddr + (SCRLYCOORD * PICTSIZE + SCRLXCOORD) / 8;
#endif LANDSCAPE / PORTRAIT
	x5 = (dword)paddr + (MOVBYCOORD * PICTSIZE + MOVBXCOORD) / 8;
	x6 = (dword)paddr + (ZOOMBYCOORD * PICTSIZE + ZOOMBXCOORD) / 8;
	x7 = (dword)paddr + (BLWBYCOORD * PICTSIZE + BLWBXCOORD) / 8;
	x8 = paddr + (HLPBYCOORD * PICTSIZE + HLPBXCOORD) / 8;
	for (i = 0 ; i < SBOXSIZE ; i++) {
		Outw(sizbd[i], x1);		/* size box */
		Outw(closbd[i], x2);		/* close box */
		Outl(scrrb[i], x3);		/* scroll right/up box */
		Outl(scrlb[i], x4);		/* scroll left/down box */
		Outw(movbd[i], x5);		/* move box */
		Outw(zoombd[i], x6);		/* zoom box */
		Outw(blwbd[i], x7);		/* blow up box */
		Outw(hlpbd[i], x8);		/* help box */
		x1 += PICTSIZE / 8;
		x2 += PICTSIZE / 8;
		x3 += PICTSIZE / 8;
		x4 += PICTSIZE / 8;
		x5 += PICTSIZE / 8;
		x6 += PICTSIZE / 8;
		x7 += PICTSIZE / 8;
		x8 += PICTSIZE / 8;
	}

#ifdef LANDSCAPE
	x1 = paddr + (SCRRYCOORD * PICTSIZE + SCRRXCOORD) / 8;
	x2 = paddr + (SCRLYCOORD * PICTSIZE + SCRLXCOORD) / 8;
#else  PORTRAIT
	x1 = paddr + (SCRDYCOORD * PICTSIZE + SCRDXCOORD) / 8;
	x2 = paddr + (SCRUYCOORD * PICTSIZE + SCRUXCOORD) / 8;
#endif LANDSCAPE / PORTRAIT
	for (i = 0 ; i < LBOXSIZE ; i++) {
		Outw(scrdb[i], x1);			/* scroll down/right box */
		Outw(scrdb[LBOXSIZE-1-i], x2);		/* scroll up/left box */
		x1 += PICTSIZE / 8;
		x2 += PICTSIZE / 8;
	}

#ifdef LANDSCAPE
	x1 = paddr + (HINDYCOORD * PICTSIZE + HINDXCOORD) / 8;
#else  PORTRAIT
	x1 = paddr + (VINDYCOORD * PICTSIZE + VINDXCOORD) / 8;
#endif LANDSCAPE / PORTRAIT
	for (i = 0 ; i < WINDSIZE ; i++) {
#ifdef LANDSCAPE
		Outw(hind[i], x1);	/* horizontal visible part indicator */
#else  PORTRAIT
		Outw(vind[i], x1);	/* vertical visible part indicator */
#endif LANDSCAPE / PORTRAIT
		x1 += PICTSIZE / 8;
	}

#ifdef LANDSCAPE
	x1 = paddr + (VINDYCOORD * PICTSIZE + VINDXCOORD) / 8;
#else  PORTRAIT
	x1 = paddr + (HINDYCOORD * PICTSIZE + HINDXCOORD) / 8;
#endif LANDSCAPE / PORTRAIT
	x2 = paddr + (CINDYCOORD * PICTSIZE + CINDXCOORD) / 8;
	for (i = 0 ; i < PINDSIZE ; i++) {
#ifdef LANDSCAPE
		Outw(vind[i], x1);	/* vertical visible part indicator */
#else  PORTRAIT
		Outw(hind[i], x1);	/* horizontal visible part indicator */
#endif LANDSCAPE / PORTRAIT
		Outw(0, x2);
		x1 += PICTSIZE / 8;
		x2 += PICTSIZE / 8;
	}

	return;
}	/* end of setupgr() */
