/* globals.h */

/*
 * This file contains global declarations for the ABC1600 window handler.
 */

/* 1984-06-28, Peter Andersson, Luxor Datorer AB (at DIAB) */

/*
 * The files defs.h and structs.h must be included before this file.
 */


/*
 * The address to the end of allocated memory for window status structures,
 * text save areas, etc. This address should equal the address in 'iconsave'
 * if no bugs.
 */
char	*mem_high;

/*
 * Pointer to memory page shared with tty device driver.
 */
struct	iogrst	*cmpgp = NULL;

/*
 * This structure is used to hold data for the font used for window headers.
 */
struct	finfo	hdf_data;

/*
 * Flag indicating if there are characters to read from the console or a change
 * to the mouse has occured.
 * This flag is set by the signal routine consig().
 */
sint	keyin = NO;

/*
 * Number of windows currently created.
 */
int	nwin = 0;

/*
 * Number of non-special windows (always less than or equal to 'nwin').
 */
int	nnswin = 0;

/*
 * Pattern used to draw contours on the background.
 */
sint	bg_cont;

/*
 * Flag indicating if global inverse video or not.
 */
sint	inversevideo = NO;

/*
 * Pointers to structures holding the internal status for all the windows.
 * If a pointer is NULL, the window number is not used.
 */
struct	winint	*wstatp[MAXWINS];

/*
 * Structures holding the information about which parts of the windows that
 * are visible. Here space for MAXWINS+1 windows is reserved. The extra
 * window is the background. The array wpp[] is used as pointers into this
 * array.
 * NOTE: IT IS NOT ABSOLUTELY SURE THAT THIS IS THE MAXIMAL AMOUNT OF
 * RECTANGLES THAT CAN BE OBTAINED. HAS TO BE INVESTIGATED!!
 */
struct	wpstruc	wp[RECTTOTMAX];

/*
 * Pointers for each window into the wp[] array. The extra window is the
 * background. The window number shall be used as index into this array.
 */
struct	wpstruc	*wpp[MAXWINS+1];

/*
 * The total number of rectangles which the background and the windows are
 * divided into.
 */
int	rtotal;

/*
 * Number of rectangles that each window is divided into. The extra entry is
 * for the background. The window number shall be used as index into this
 * array.
 */
int	nwpart[MAXWINS+1];

/*
 * Temporary area used by e.g. alter1win(). Enough space to divide a rectangle
 * by overlaying it with one window.
 */
struct	wpstruc	wpt1r[4];

/*
 * Array used to map the level number to the window number, i.e. with the
 * level number as index the window number is obtained. The background is
 * always at the lowest level and has window number MAXWINS.
 */
int	ltow[MAXWINS+1];

/*
 * Array used to map the window number to the level number, i.e. with the
 * window number as index the level is obtained.
 * The background is always window MAXWINS and it is always at a lower level
 * than all other windows.
 */
int	wtol[MAXWINS+1];

/*
 * Array containing the width and some flags concerning the different border
 * types.
 */
struct	borddst	borddata[] = {
	{
		NOBWIDTH,
		NOBWIDTH,
		NOBWIDTH,
		NOBWIDTH,
		0
	},
	{
		SLBWIDTH,
		SLBWIDTH,
		SLBWIDTH,
		SLBWIDTH,
		0
	},
	{
		DLBWIDTH,
		DLBWIDTH,
		DLBWIDTH,
		DLBWIDTH,
		BD_LEFT | BD_RIGHT | BD_UP | BD_DOWN | BD_ULC | BD_URC | BD_DLC | BD_DRC
	},
	{
		DLBWIDTH,
		SLBWIDTH,
		SLBWIDTH,
		SLBWIDTH,
		BD_LEFT | BD_ULC | BD_DLC
	},
	{
		SLBWIDTH,
		DLBWIDTH,
		SLBWIDTH,
		SLBWIDTH,
		BD_RIGHT | BD_URC | BD_DRC
	},
	{
		SLBWIDTH,
		SLBWIDTH,
		DLBWIDTH,
		SLBWIDTH,
		BD_UP | BD_ULC | BD_URC
	},
	{
		SLBWIDTH,
		SLBWIDTH,
		SLBWIDTH,
		DLBWIDTH,
		BD_DOWN | BD_DLC | BD_DRC
	},
	{
		DLBWIDTH,
		DLBWIDTH,
		SLBWIDTH,
		SLBWIDTH,
		BD_LEFT | BD_RIGHT | BD_ULC | BD_URC | BD_DLC | BD_DRC
	},
	{
		DLBWIDTH,
		SLBWIDTH,
		DLBWIDTH,
		SLBWIDTH,
		BD_LEFT | BD_UP | BD_ULC | BD_URC | BD_DLC
	},
	{
		DLBWIDTH,
		SLBWIDTH,
		SLBWIDTH,
		DLBWIDTH,
		BD_LEFT | BD_DOWN | BD_ULC | BD_DLC | BD_DRC
	},
	{
		SLBWIDTH,
		DLBWIDTH,
		DLBWIDTH,
		SLBWIDTH,
		BD_RIGHT | BD_UP | BD_ULC | BD_URC | BD_DRC
	},
	{
		SLBWIDTH,
		DLBWIDTH,
		SLBWIDTH,
		DLBWIDTH,
		BD_RIGHT | BD_DOWN | BD_URC | BD_DLC | BD_DRC
	},
	{
		SLBWIDTH,
		SLBWIDTH,
		DLBWIDTH,
		DLBWIDTH,
		BD_UP | BD_DOWN | BD_ULC | BD_URC | BD_DLC | BD_DRC
	},
	{
		DLBWIDTH,
		DLBWIDTH,
		DLBWIDTH,
		SLBWIDTH,
		BD_LEFT | BD_RIGHT | BD_UP | BD_ULC | BD_URC | BD_DLC | BD_DRC
	},
	{
		DLBWIDTH,
		DLBWIDTH,
		SLBWIDTH,
		DLBWIDTH,
		BD_LEFT | BD_RIGHT | BD_DOWN | BD_ULC | BD_URC | BD_DLC | BD_DRC
	},
	{
		DLBWIDTH,
		SLBWIDTH,
		DLBWIDTH,
		DLBWIDTH,
		BD_LEFT | BD_UP | BD_DOWN | BD_ULC | BD_URC | BD_DLC | BD_DRC
	},
	{
		SLBWIDTH,
		DLBWIDTH,
		DLBWIDTH,
		DLBWIDTH,
		BD_RIGHT | BD_UP | BD_DOWN | BD_ULC | BD_URC | BD_DLC | BD_DRC
	}
};

/*
 * Array containing data for the different boxes in the border.
 */
#ifdef LANDSCAPE
struct	boxactd	boxdata[] = {
	{
		BX_ZOOM,		/* zoom box */
		ZOOMBXCOORD,
		ZOOMBYCOORD,
		SBOXSIZE - 1,
		SBOXSIZE - 1,
		BOXOFFS,
		BOXOFFS,
		BA_ZOOM
	},
	{
		BX_CLOS,		/* close box */
		CLOSBXCOORD,
		CLOSBYCOORD,
		SBOXSIZE - 1,
		SBOXSIZE - 1,
		-(BOXOFFS + SBOXSIZE - 1),
		BOXOFFS,
		BA_CLOS
	},
	{
		BX_MOVE,		/* move box */
		MOVBXCOORD,
		MOVBYCOORD,
		SBOXSIZE - 1,
		SBOXSIZE - 1,
		-(BOXOFFS + SBOXSIZE - 1),
		-(BOXOFFS + SBOXSIZE - 1),
		BA_MOVE
	},
	{
		BX_SIZE,		/* size box */
		SIZBXCOORD,
		SIZBYCOORD,
		SBOXSIZE - 1,
		SBOXSIZE - 1,
		BOXOFFS,
		-(BOXOFFS + SBOXSIZE - 1),
		BA_SIZE
	},
	{
		BX_VSCR,		/* scroll up box */
		SCRUXCOORD,
		SCRUYCOORD,
		LBOXSIZE - 1,
		SBOXSIZE - 1,
		-(DLBWIDTH + LBOXSIZE - 1),
		-(BOXOFFS + SBOXSIZE - 1),
		BA_USCR
	},
	{
		BX_VSCR,		/* scroll down box */
		SCRDXCOORD,
		SCRDYCOORD,
		LBOXSIZE - 1,
		SBOXSIZE - 1,
		DLBWIDTH,
		-(BOXOFFS + SBOXSIZE - 1),
		BA_DSCR
	},
	{
		BX_HSCR,		/* scroll left box */
		SCRLXCOORD,
		SCRLYCOORD,
		SBOXSIZE - 1,
		LBOXSIZE - 1,
		BOXOFFS,
		DLBWIDTH,
		BA_LSCR
	},
	{
		BX_HSCR,		/* scroll right box */
		SCRRXCOORD,
		SCRRYCOORD,
		SBOXSIZE - 1,
		LBOXSIZE - 1,
		BOXOFFS,
		-(DLBWIDTH + LBOXSIZE - 1),
		BA_RSCR
	},
	{
		BX_BLOW,		/* blow up box */
		BLWBXCOORD,
		BLWBYCOORD,
		SBOXSIZE - 1,
		SBOXSIZE - 1,
		-(BOXOFFS + SBOXSIZE - 1),
		-(DLBWIDTH + SBOXSIZE - 1),
		BA_BLOW
	},
	{
		BX_HELP,		/* help box */
		HLPBXCOORD,
		HLPBYCOORD,
		SBOXSIZE - 1,
		SBOXSIZE - 1,
		-(BOXOFFS + SBOXSIZE - 1),
		DLBWIDTH,
		BA_HELP
	}
};
#else  PORTRAIT
struct	boxactd	boxdata[] = {
	{
		BX_ZOOM,		/* zoom box */
		ZOOMBXCOORD,
		ZOOMBYCOORD,
		SBOXSIZE - 1,
		SBOXSIZE - 1,
		BOXOFFS,
		-(BOXOFFS + SBOXSIZE - 1),
		BA_ZOOM
	},
	{
		BX_CLOS,		/* close box */
		CLOSBXCOORD,
		CLOSBYCOORD,
		SBOXSIZE - 1,
		SBOXSIZE - 1,
		BOXOFFS,
		BOXOFFS,
		BA_CLOS
	},
	{
		BX_MOVE,		/* move box */
		MOVBXCOORD,
		MOVBYCOORD,
		SBOXSIZE - 1,
		SBOXSIZE - 1,
		-(BOXOFFS + SBOXSIZE - 1),
		BOXOFFS,
		BA_MOVE
	},
	{
		BX_SIZE,		/* size box */
		SIZBXCOORD,
		SIZBYCOORD,
		SBOXSIZE - 1,
		SBOXSIZE - 1,
		-(BOXOFFS + SBOXSIZE - 1),
		-(BOXOFFS + SBOXSIZE - 1),
		BA_SIZE
	},
	{
		BX_VSCR,		/* scroll up box */
		SCRUXCOORD,
		SCRUYCOORD,
		SBOXSIZE - 1,
		LBOXSIZE - 1,
		-(BOXOFFS + SBOXSIZE - 1),
		DLBWIDTH,
		BA_USCR
	},
	{
		BX_VSCR,		/* scroll down box */
		SCRDXCOORD,
		SCRDYCOORD,
		SBOXSIZE - 1,
		LBOXSIZE - 1,
		-(BOXOFFS + SBOXSIZE - 1),
		-(DLBWIDTH + LBOXSIZE - 1),
		BA_DSCR
	},
	{
		BX_HSCR,		/* scroll left box */
		SCRLXCOORD,
		SCRLYCOORD,
		LBOXSIZE - 1,
		SBOXSIZE - 1,
		DLBWIDTH,
		-(BOXOFFS + SBOXSIZE - 1),
		BA_LSCR
	},
	{
		BX_HSCR,		/* scroll right box */
		SCRRXCOORD,
		SCRRYCOORD,
		LBOXSIZE - 1,
		SBOXSIZE - 1,
		-(DLBWIDTH + LBOXSIZE - 1),
		-(BOXOFFS + SBOXSIZE - 1),
		BA_RSCR
	},
	{
		BX_BLOW,		/* blow up box */
		BLWBXCOORD,
		BLWBYCOORD,
		SBOXSIZE - 1,
		SBOXSIZE - 1,
		-(DLBWIDTH + SBOXSIZE - 1),
		BOXOFFS,
		BA_BLOW
	},
	{
		BX_HELP,		/* help box */
		HLPBXCOORD,
		HLPBYCOORD,
		SBOXSIZE - 1,
		SBOXSIZE - 1,
		DLBWIDTH,
		BOXOFFS,
		BA_HELP
	}
};
#endif LANDSCAPE / PORTRAIT

/*
 * Default (initial) values for the graphic pattern tables.
 */
word	msk1[] = {		/* horizontal masks */
			0xffff,
			0xaaaa,
			0xff00,
			0x0001,

			0x5050,
			0x5050,
			0x0001,
			0x0101,

			0x1111,
			0x6666,
			0x6666,
			0xf7f7,

			0xf7f7,
			0x7070,
			0x7070,
			0xff00
};

word	msk2[] = {		/* vertical masks */
			0xffff,
			0xffff,
			0xff00,
			0x8888,

			0xf0f0,
			0x6666,
			0xffff,
			0xffff,

			0xffff,
			0xf0f0,
			0xf0f0,
			0xff00,

			0xff00,
			0xff00,
			0xf0f0,
			0x1111
};

byte	shft[] = {		/* shift counts */
			0,
			1,
			0,
			3,

			2,
			14,
			1,
			4,

			2,
			2,
			1,
			1,

			15,
			1,
			1,
			0
};

byte	op[] = {		/* operations */
			0,	/* clear line, rotate msk1 shft bits */
			1,	/* set line, rotate msk1 shft bits   */
			2,	/* use msk1 complemented, rotate     */
			3,	/* leave line as it is, rotate       */

			4,	/* clear line, no rotate             */
			5,	/* set line, no rotate               */
			6,	/* use msk1 complemented, no rotate  */
			7,	/* leave line as it is, no rotate    */

			0,
			0,
			7,
			6,

			5,
			4,
			2,
			0
};

/*
 * Default bit pattern used by the spray escape sequence.
 */
#ifdef LANDSCAPE
struct	sprayst	spraym = {
	0x00008000, 0x00022000, 0x00900000, 0x00008900,
	0x04210040, 0x01000000, 0x08081040, 0x01420208,
	0x28004040, 0x00000204, 0x24201020, 0x02024000,
	0x00000842, 0x10080208, 0x80004080, 0x14811004,
	0x00100411, 0x04840800, 0x40108000, 0x04004282,
	0x00008000, 0x12082400, 0x40400048, 0x00001000,
	0x04000100, 0x10820000, 0x04000000, 0x00102440,
	0x00840000, 0x00000000, 0x00100500, 0x00004000
};
#else  PORTRAIT
struct	sprayst	spraym = {
	0x00008000, 0x00081000, 0x00410000, 0x01040200,
	0x00008000, 0x00200000, 0x0a880210, 0x00021000,
	0x10000082, 0x01441000, 0x00008412, 0x10084000,
	0x02210100, 0x40000410, 0x00921001, 0x90002800,
	0x08010000, 0x41100040, 0x00004008, 0x02040400,
	0x2000a012, 0x08200000, 0x01000200, 0x20014048,
	0x05000000, 0x00100400, 0x082150a0, 0x02800000,
	0x00050440, 0x00a00000, 0x00002200, 0x00020000
};
#endif LANDSCAPE / PORTRAIT

/*
 * Structure containing hardware addresses.
 */
struct	hwastruc	hwa;

/*
 * Structure containing data about the current position of the mouse pointer.
 */
struct	pntstruc pntdata = {
	OFF,
	MXPOS,
	SCYSIZE - MYPOS,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	NULL,
	0
};

/*
 * Structure containing the form and other data of the global mouse pointer
 * currently used.
 */

#ifdef LANDSCAPE
/* A mouse pointer pointing in the up direction */

struct	mpstruc	mpdat = {
	24 - 1, 17 - 1,
	22, 8,
	{
	  0xffffffff, 0xfe7fffff, 0xfe1fffff, 0xfe07ffff,
	  0xfe01ffff, 0xff007fff, 0xff001fff, 0x800007ff,
	  0x800001ff, 0x800007ff, 0xff001fff, 0xff007fff,
	  0xfe01ffff, 0xfe07ffff, 0xfe1fffff, 0xfe7fffff,
	  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
	},
	{
	  0x03c00000, 0x02700000, 0x021c0000, 0x02070000,
	  0x0201c000, 0x03007000, 0xff001c00, 0x80000700,
	  0x80000100, 0x80000700, 0xff001c00, 0x03007000,
	  0x0201c000, 0x02070000, 0x021c0000, 0x02700000,
	  0x03c00000, 0x00000000, 0x00000000, 0x00000000,
	  0x00000000, 0x00000000, 0x00000000, 0x00000000,
	  0x00000000, 0x00000000, 0x00000000, 0x00000000,
	  0x00000000, 0x00000000, 0x00000000, 0x00000000
	}
};
#else  PORTRAIT

/* 45 degrees pointer, pointing in an up-left direction */
/*
struct	mpstruc	mpdat = {
	18 - 1, 18 - 1,
	1, 1,
	{
	  0xffffffff, 0x801fffff, 0x801fffff, 0x83ffffff,
	  0x83ffffff, 0x81ffffff, 0x98ffffff, 0x9c7fffff,
	  0x9e3fffff, 0x9f1fffff, 0x9f8fffff, 0xffc7ffff,
	  0xffe3ffff, 0xfff1ffff, 0xfff8ffff, 0xfffc7fff,
	  0xfffeffff, 0xffffffff, 0xffffffff, 0xffffffff,
	  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
	},
	{
	  0xfff00000, 0x80100000, 0x80100000, 0x83f00000,
	  0x82000000, 0x81000000, 0x98800000, 0x94400000,
	  0x92200000, 0x91100000, 0x90880000, 0xf0440000,
	  0x00220000, 0x00110000, 0x00088000, 0x00044000,
	  0x00028000, 0x00010000, 0x00000000, 0x00000000,
	  0x00000000, 0x00000000, 0x00000000, 0x00000000,
	  0x00000000, 0x00000000, 0x00000000, 0x00000000,
	  0x00000000, 0x00000000, 0x00000000, 0x00000000
	}
};
*/

/* A Smalltalk inspired mouse pointer */
/*
struct	mpstruc	mpdat = {
	18 - 1, 18 - 1,
	1, 1,
	{
	  0xffffffff, 0xbfffffff, 0x9fffffff, 0x8fffffff,
	  0x87ffffff, 0x83ffffff, 0x81ffffff, 0x80ffffff,
	  0x87ffffff, 0x93ffffff, 0xb3ffffff, 0xf9ffffff,
	  0xf9ffffff, 0xfcffffff, 0xfcffffff, 0xfe7fffff,
	  0xfe7fffff, 0xffffffff, 0xffffffff, 0xffffffff,
	  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
	},
	{
	  0xc0000000, 0xa0000000, 0x90000000, 0x88000000,
	  0x84000000, 0x82000000, 0x81000000, 0x80800000,
	  0x87000000, 0x92000000, 0xb2000000, 0x09000000,
	  0x09000000, 0x04800000, 0x04800000, 0x02400000,
	  0x02400000, 0x01800000, 0x00000000, 0x00000000,
	  0x00000000, 0x00000000, 0x00000000, 0x00000000,
	  0x00000000, 0x00000000, 0x00000000, 0x00000000,
	  0x00000000, 0x00000000, 0x00000000, 0x00000000
	}
};
*/

/* A mouse pointer pointing in the up direction */

struct	mpstruc	mpdat = {
	17 - 1, 24 - 1,
	8, 1,
	{
	  0xffffffff, 0xff7fffff, 0xff7fffff, 0xfe3fffff,
	  0xfe3fffff, 0xfc1fffff, 0xfc1fffff, 0xf80fffff,
	  0xf80fffff, 0xf007ffff, 0xf007ffff, 0xe003ffff,
	  0xe003ffff, 0xc001ffff, 0xc001ffff, 0x8000ffff,
	  0x8630ffff, 0xfe3fffff, 0xfe3fffff, 0xfe3fffff,
	  0xfe3fffff, 0xfe3fffff, 0xfe3fffff, 0xffffffff,
	  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
	},
	{
	  0x01c00000, 0x01400000, 0x03600000, 0x02200000,
	  0x06300000, 0x04100000, 0x0c180000, 0x08080000,
	  0x180c0000, 0x10040000, 0x30060000, 0x20020000,
	  0x60030000, 0x40010000, 0xc0018000, 0x80008000,
	  0x86308000, 0xfe3f8000, 0x02200000, 0x02200000,
	  0x02200000, 0x02200000, 0x02200000, 0x03e00000,
	  0x00000000, 0x00000000, 0x00000000, 0x00000000,
	  0x00000000, 0x00000000, 0x00000000, 0x00000000
	}
};
#endif LANDSCAPE / PORTRAIT



/*
 * Pointer to data for the different icons.
 */
int		n_icons = 0;		/* # of icons currently set up */
int		tot_icons = 0;		/* total # of allocated icons  */
struct	icondat	*iconsave;		/* should equal to 'mem_high'
					   if no bugs                  */



/*
 * Flags and data indicating an inverted icon.
 */
struct	invsave	invdata;

/*
 * Data to keep track of the state of the text cursor.
 */
struct	cursave	curdata;

/*
 * Structure containing the mouse substitute keys.
 */
struct	substit	subkys;

/*
 * Buffer used when serving VT100 escape reports. Note that the size of the
 * buffer may have to be increased if support for a new report is added.
 */
char	repvt100[14] = { ESC, '[' };

/*
 * Buffer used when serving private escape reports. Note that the size of the
 * buffer may have to be increased if support for a new report is added.
 */
char	repprivate[32] = { ESC, ':' };

/*
 * Buffer used to copy data to and from the user's buffer.
 * Also used as temporary area.
 */
union	buffer	ubuf;

/*
 * Structure used to save the tty status before setting raw, cbreak, no echo
 * mode for the console.
 */
struct	sgttyb	ttysave;
short		ttysflg = NO;		/* tty status saved flag */

/*
 * Structure used to hold the default terminal and tty parameters used when a
 * new window is created. The tty parameters are obtained when the window
 * handler is started by a TCGETA ioctl command to the console.
 */
struct	wininit	ttydflt = {
	{
		(1 << 24) | (1 << 16) | (1 << 8) | (1 << 0),
		(1 << 24) | (1 << 16) | (1 << 8) | (1 << 0),
		(1 << 24) | (1 << 16) | (1 << 8) | (1 << 0),
		(1 << 24) | (1 << 16) | (1 << 8) | (1 << 0),
		(1 << 24) | (1 << 16) | (1 << 8) | (1 << 0),
		(1 << 24) | (1 << 16) | (1 << 8) | (1 << 0),
		(1 << 24) | (1 << 16) | (1 << 8) | (1 << 0),
		(1 << 24) | (1 << 16) | (1 << 8) | (1 << 0)
	},
	TD_WRAP
};

/*
 * Buffer to receive characters from the console.
 */
unsigned char	inbuf[IBSIZE];

/*
 * Flags for logical unit numbers returned when locate requests are received.
 * If a flag is non-zero, the lu is occupied and it is the index plus
 * (MAXWINS + 1).
 */
sint	locflg[MAXLOC];

/*
 * The name of the window handler.
 */
char	*hndname;

/*
 * The name of the temporary file used for text copying between windows.
 */
char	cptmpname[] = CPTMPNAME;
