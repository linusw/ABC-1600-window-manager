/*---------------------------------------------------------------------------
 *
 *
 *      gpage.h
 *
 *      This file contains definitions for memory page used by X-35 terminal
 *      driver and window handler for communication
 *
 *	Jan Tjernberg, Dataindustrier DIAB AB
 *
 *	1985-02-07
 *
 *---------------------------------------------------------------------------
 */

#define		MAXFONTCNT	26	/* size of array of font info structures */

/* bits in status word in font info structure 'fontinf' */

#define		FONTACTIVE	1	/* font initialized */
#define		FONTLAND	2	/* font for landscape mode */


/*
*	structure to store info about loaded font
*/

typedef struct {
  unsigned short stat;		/* status flag bits                    */
  unsigned char fbzx;		/* font box size in x direction        */
  unsigned char fbzy;		/* font box size in y direction        */
  unsigned char firstc;		/* ascii # for first defined char      */
  unsigned char lastc;		/* ascii # for last defined char       */
  short bwidth;			/* # of bytes width of font definition */
  short bheight;		/* # of pixel lines of font definition */
  short start;			/* line # in PICTMEM, x is always 96   */
  unsigned char basl;		/* line from fontbox bottom to align   */
  unsigned char lastf;		/* link to previously used font        */
} fontinf;


#define FNKEYBUFSIZE    1024
#define NFNKEYS		110

/*---------------------------------------------------------------------------
 * 
 * flag bits in 'curstat'
 * 
 * used by terminal emulator to syncronize output and handle cursor
 *
 *---------------------------------------------------------------------------
 */

#define CSTEADY		(0x01)  /* not blinking cursor          */
#define CUNDERL		(0x02)	/* underline cursor             */
#define COFF		(0x04)	/* no cursor at all             */
#define COFFWR		(0x08)	/* set when in x35write routine */
#define COFFCH		(0x10)	/* set when in tty_cho routine  */
#define MSON		(0x20)  /* when set poll mouse for pos  */
#define MSAUTO		(0x40)  /* when set mouse in auto mode  */
#define COFFTO		(0x80)  /* set when in timeout routine  */


struct mousestr {
    short x;
    short y;
    char remkey;
    char lastkst;
    short haircx;	/* haircross x position */
    short haircy;	/* haircross y position */
};

/*---------------------------------------------------------------------------
 * struct whcur is used by window handler to generate blinking cursor
 *
 * when the terminal emulator cursor blink timeout is handled 
 * the WHCURON bit in 'flgs' is tested
 * if it is set and the WHNOACT bit is not set
 * the area at 'xpos', 'ypos' is inverted
 *
 *---------------------------------------------------------------------------
 */

#define WHCURON	   (0x01)     /* when set window handler wants cursor
			         blink */
#define WHNOACT	   (0x02)     /* when set window handler is working, no
				 action should be done at cursor timeout */

#define WHCPRES	   (0x04)     /* when set the cursor is present on the 
				 screen, this bit is inverted at every
				 cursor blink. */

struct whcur {
    short xsize;
    short ysize;
    short xpos;
    short ypos;
    short flgs;
};

/*---------------------------------------------------------------------------
 * struct ledst is used for output of bytes to keyboard
 *
 * at every cursor timeout one character will be output,
 * to ensure sufficient delay between transmissions.
 *
 *---------------------------------------------------------------------------
 */

struct ledst {
    char f;	   /* index into buf for first char */
    char l;	   /* index into buf for last char  */
    char cnt;	   /* number of characters in buf   */
    char buf[10];  /* circular buffer area          */
};


struct iogrst {
    unsigned char mflag;	/* mouse action code, movement or keys */
    char mactive;		/* set to 1 when window handler active */
    short mlocx;		/* mouse position x                    */
    short mlocy;		/* mouse position y                    */
    unsigned char mtbcnt;       /* number of reserved keycodes in table*/
    unsigned char mtbstrt[31];  /* table of reserved keycodes          */
    char userm[128];		/* reserved for use by window handler  */
    fontinf fonttab[MAXFONTCNT];/* information about loaded fonts      */
    struct mousestr mouse;      /* mouse status and position           */
    struct whcur whc;	        /* blinking cursor for window handler  */
    struct ledst led;		/* handling of output to keyboard      */
    short fnkeyoff[NFNKEYS];    /* offsets for function key strings    */
    unsigned char fnkeyslen[NFNKEYS]; /* length of the string          */
    short fnkbcnt;		/* space used in string buffer         */
    char kbdstate;	        /* status during mouse sequences       */
    char curcnt;		/* counter cursor blink                */
    short blnkcnt;		/* counter turn down screen            */
    short blnkstr;		/* start value for blank counter       */
    unsigned char curstat;	/* status bits for cursor              */
    short ffreline;		/* free line for font load             */
    char fnkeybuf[FNKEYBUFSIZE]; /* common buffer for function key strings
				    */
    
};


#define iogrp (*((struct iogrst*)(IOBPAGEA)))



#define REPCHANGE    0	  /* driver should report mouse changes */
#define MOUSEMVD     1	  /* mouse has been moved               */
#define LEMKEYPR     2	  /* left mouse key pressed		*/
#define LEMKEYRE     3	  /* left mouse key released            */
#define MIMKEYPR     4	  /* middle mouse key pressed           */
#define MIMKEYRE     5	  /* middle mouse key released          */
#define RIMKEYPR     6	  /* right mouse key pressed            */
#define RIMKEYRE     7    /* right mouse key released           */
