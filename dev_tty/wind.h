/*

	wind.h

	definitions for character generator

	by Jan Tjernberg

*/


#include "../h/mac.h"
#include "../h/graphadr.h" 


#define XMASK 0x03ff0000	/* mask for x field in size, to and from */
#define YMASK 0x0fff		/* mask for y field in size, to and from */
#define UdLAND 0x0400		/* down in x dir, up in y dir */
#define UdPORT 0x0c00		/* up in both x and y dir */
#define DORI	0x00		/* down in both x and y dir */
#define DOLE	0x0800		/* up in x dir, down in y dir */
#define UPRI	0x0400		/* down in x dir, up in y dir */
#define UPLE	0x0c00		/* up in both directions */

#define VT	'\013'		/* vertical tab */
#define FF	'\014'		/* form feed */
#define ESC	'\033'		/* escape */

#define UCHAR unsigned char
#define outl(val,addr) *((unsigned long*)(addr))=((unsigned long)(val))
#define outw(val,addr) *((unsigned short*)(addr))=((unsigned short)(val))

/* Window definition stored in 'window' structure is
   in 'portrait mode' coord. independently of portrait/landscape mode
   x starts at upper left corner with 0 and goes horizontaly
   to the right to 767/1023.
   y starts at upper left corner with 0 and goes down to 1023/767. */
/*
  NOTE: pos and fsize should only be used by bottom level routines
  which have to know if in portrait or landscape mode */

STRUCT BEGIN
  unsigned status;  /* flags for window                                      */
  short minx;    /* the the lowest pixel IN the window                       */
  short maxx;    /* the highest pixel IN the window                          */
  short rmarg;   /* right margin, last x to-coord where full width font fits */
  short miny;    /* the lowest pixel IN the window                           */
  short maxy;    /* the highest pixel IN the window                          */
  short tmarg;   /* top margin                                               */
  short bmarg;   /* bottom marg, last y to-coord where full hight font fits  */
  short lmarg;   /* left margin, first pixel where font is printed           */
  short sminy;   /* scroll top margins                                       */
  short sbmarg;  /* scroll bottom margin                                     */
  short xsize;   /* the width of the window in # of pixels                   */
  short ysize;   /* the hight of the window in # of pixels                   */
  short xcur;    /* the cursor position in x                                 */
  short ycur;    /* the cursor position in y                                 */
  short xgrc;    /* graphic cursor x                                         */
  short ygrc;    /* graphic cursor y                                         */
  short xfont;   /* font width in x                                          */
  short yfont;   /* font width in y                                          */
  unsigned long pos;   /* the current position of pixel cursor               */
		       /* NOTE: this is in mover coordinates                 */
		       /* bit 32      25       16     11      0              */
		       /*             !  x coor !     ! y coor!              */
  unsigned long fsize; /* font size in pixels , format as above              */
		       /* NOTE: also in mover coordinates                    */
  unsigned char fnum;  /* number of font used in this region                 */
  unsigned short pstat; /* push cursor save status word                       */
  short fontmxcnt;     /* max # of chars in 1 line for this font, width      */
  unsigned long tabstop[7]; /* array defining tab positions                  */
  unsigned long scpos;	    /* save cursor position                          */
END window ;




#define FROMSP outl(*(w->fbas+' '),FROM);
#define PORTRAIT w->fsize&0x08000000
#define move(fromx,fromy,tox,toy,sizex,sizey,flg) outw((sizex&0x3ff)|(flg),SIZEH);\
	outw(sizey,SIZEL);\
	outw(tox,TOADH);\
	outw(toy,TOADL);\
	outw(fromx,FROMH);\
	outw(fromy,FROML);

#define UPDCUR IF PORTRAIT THEN\
		w->xcur=(w->pos)>>16;\
		w->ycur=w->pos&YMASK;\
	       ELSE\
		w->xcur=w->pos&YMASK;\
		w->ycur=SCHIGHT-(w->pos>>16);\
	      FI

#define UPDPOS IF PORTRAIT THEN\
		w->pos=((unsigned long)w->xcur<<16)+w->ycur;\
	       ELSE\
		w->pos=((unsigned long)(SCHIGHT-w->ycur)<<16)+w->xcur;\
	       FI

#define WAITL IF INTWIND THEN waitint(); ELSE WHILE BUSYMOV DO WATCHDOG OD FI
#define WAITS WHILE BUSYMOV DO  OD
#define CLEARNOW SETHOLD; outl(CLRWAD,FROM); WAITS; RELHOLD;

/* bit definition used for status word in window structure */

#define NLFLG	(0x0001)	/* indicates do CR LF on '\n'               */
#define WRAPFLG (0x0002)	/* indicates forced new line at right marg  */
#define WINTFLG (0x0004)	/* indicates that interrupts are useful due
			   to the size of the window
			*/
#define ESQMODE (0x0008)	/* escape/control sequence under processing */
#define CTRLSEQ (0x0010)	/* control sequence                         */
#define ESQSEQ  (0x0020)	/* escape sequence                          */
#define SCROLLREG (0x0040)	/* scroll region set                        */
#define CURSAVED  (0x0080)	/* cursor saved                             */
#define GRSEQ     (0x0100)	/* graphic sequence                         */
#define INESCPARA (0x0200)	/* extracting parameters                    */
#define ORIGINFL  (0x0400)   	/* origin mode flag                         */
#define UNDERSCORE (0x0800) 	/* underscore mode                          */
#define REGACTIVE  (0x1000)	/* region initialized                       */
#define SELCHSET   (0x2000)	/* select new character set                 */
#define CURSORON   (0x4000)	/* cursor is on                             */
#define LASTCOL    (0x8000)	/* character printed in last col, wrap ?    */
#define TEKMOD     (0x10000)    /* Tektronix 4014 mode                      */
#define HCROSS     (0x20000)	/* hair cross on                            */
#define HCOFF      (0x40000)    /* hair cross temporary off (during write)  */
#define NODIM      (0x80000)    /* screen is never dimmed                   */

/* bits in status byte from mover, read from MSTATUS port           */

#define POLASTAT  2	/* bit in MSTATUS from position indicator LAND/port */
#define BUSYFLG 128     /* bit 7 is mover busy flag                         */

/* bits in status word in font info structure 'fontinf'                     */

#define FONTACTIVE 1	/* font initialized                                 */
#define FONTLAND   2    /* font for landscape mode                          */



#define SCHIGHT 767     /* hight of screen in landscape mode                */
#define CLEVREL  2	/* position on character level, relative            */
#define CLEVABS  3	/* position on character level, absolute            */
#define PLEVREL  4	/* position on pixel level, relative                */
#define PLEVABS  5	/* position on pixel level, absolute                */
#define MAXFONTCNT 26	/* size of array of font information structures     */
#define MAXNOREG   1	/* size of array of region info structs (window)    */
#define PMEMSIZE 131072	/* picture memory size in bytes                     */

#define min(a,b) ((a)<(b)?(a):(b))
#define ENDESQSEQ(ptr) ((c= *(ptr))>='\060' && c<='\176')
#define ENDCTRSEQ(ptr) ((c= *(ptr))>='\100' && c<='\176')
#define INTERMED(c)    (c>='\040' && c<='\04f')
#define portrait size&0x8000000	/* use movedir x flag as indicator */
#define nliscrlf (w->status&NLFLG) /* mapping of '\n' to '\r''\n' */
#define WRAPMODE (w->status&WRAPFLG) /* force CR LF at right marg */
#define INTWIND  (w->status&WINTFLG) /* use wait on interrupt */
#define BUSYMOV  ((*(char*)MSTATUS)&BUSYFLG) /* test mover busy flg */
#define SETHOLD  WAITS; grfstat|=HOLDFRY+HOLDFRX; outb(MFLAG,grfstat);
#define RELHOLD  WAITS; grfstat&=~(HOLDFRY+HOLDFRX); outb(MFLAG,grfstat);




#define SETGRMOD  *((unsigned short*)(PICTMEMST+96))=0xffff; \
		  *((unsigned short*)(PICTMEMST+224))=0;\
		  WAITS; SETHOLD; outw(UdPORT,SIZEH); outw(0,SIZEL);\
                  grmp=GRAFXAD+15;\
		  *((short*)GRAFWAD)=mask;

#define RESGRMOD WAITS; RELHOLD


/*
*	font information structure 
*/


STRUCT BEGIN 
  unsigned short cg_magic;	/* type of generator (value: 0xffff)   */
  unsigned short cg_stat;	/* flags i e Portrait/Landscape        */
  unsigned short cg_fnumb;	/* the number of the generator         */
  unsigned short cg_fxsiz;	/* font sixe x, # of pixels            */
  unsigned short cg_fysiz;	/* font size y, # of pixels            */
  unsigned char  cg_firstc;	/* ascii # for first defined char      */
  unsigned char  cg_lastc;	/* ascii # for last defined char       */
  unsigned short cg_bwidth;	/* # of bytes per line in memory dump  */
  unsigned short cg_dumpl;	/* # of bytes in dump after this block */
  unsigned char  cg_baslin;	/* base line for change font alignment */
  unsigned char pad[22];	/* pad block to 40 bytes. Future use   */
END cgenblk ;


/*
*	picture load/store structure
*/


STRUCT BEGIN 
  unsigned short magic;		/* type of generator (value: 0xfffe)   */
  unsigned short stat;		/* flags                               */
  unsigned short xaddr;		/* from/to x address (aligned pixel)   */
  unsigned short yaddr;		/* from/to y address (pixel)           */
  unsigned short bwidth;	/* # of bytes per line in memory dump  */
  unsigned short bheight;	/* # of bytes in dump after this block */
  unsigned char pad[38];	/* pad block to 50 bytes. Future use   */
END pictblk ;


/*
*	structure to store info about loaded font
*/

STRUCT BEGIN
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
END fontinf;  


/*---------------------------------------------------------------------------
 * structure for loading / reading of user defined keyboard keys
 *---------------------------------------------------------------------------
 */

struct fnkst {
    union {
        struct {
	    unsigned char code;	   /* keycode, usually > 0x80		*/
	    unsigned char len;	   /* length of string			*/
	} st;
	short space;		   /* left space after loading (returned)
				      */
    } u;
    unsigned char string[60];	   /* the functionkey string		*/
};


