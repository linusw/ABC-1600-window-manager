/* charcodes.h */

/*
 * This file contains defines for character codes and escape sequences.
 */

/* 1985-07-22, Peter Andersson, Luxor Datorer AB */



/*
 * Different control character codes.
 */
#define		BELL		'\07'		/* bell                    */
#define		VT		'\13'		/* vertical tab ASCII code */
#define		FF		'\14'		/* form feed ASCII code    */
#define		CAN		'\30'
#define		SUB		'\32'

/*
 * Characters ending escape sequences starting with CSI 'ESC ['.
 */
#define		TBC	'g'		/* tabulation clear          */
#define		STBM	'r'		/* set top and bottom margin */
#define		CUU	'A'		/* cursor up                 */
#define		CUD	'B'		/* cursor down               */
#define		CUF	'C'		/* cursor forward            */
#define		CUB	'D'		/* cursor backward           */
#define		CUP	'H'		/* cursor position           */
#define		CUP1	'f'		/* identical to CUP          */
#define		ED	'J'		/* erase in display          */
#define		EIL	'K'		/* erase in line             */
#define		CAT	'm'		/* character attributes      */
#define		SM	'h'		/* set mode                  */
#define		RM	'l'		/* reset mode                */
#define		DSR	'n'		/* device status report      */
#define		CPR	'R'		/* cursor position report    */
#ifdef LANDSCAPE
#define		SMR	"\033[?Ln"	/* screen mode report        */
#else  PORTRAIT
#define		SMR	"\033[?Pn"	/* screen mode report        */
#endif LANDSCAPE / PORTRAIT
#define		SMRLEN	5		/* length of above           */
#define		DECLL	'q'		/* load LED's                */
#define		BTAB	'Z'		/* tab backward (Twist)      */

/*
 * Characters ending ABC1600 private escape sequences (CSI 'ESC :').
 */
#define		DRAWLINE	'd'	/* draw line                      */
#define		INVLINE		'i'	/* invert line                    */
#define		MOVGRPHCUR	'm'	/* move graphic cursor            */
#define		FILLAREA	'f'	/* fill area                      */
#define		PAINTAREA	'F'	/* paint area                     */
#define		DRAWARC		'a'	/* draw arc                       */
#define		INVARC		'I'	/* draw inverse arc               */
#define		DRAWPOINT	'p'	/* draw point                     */
#define		DEFPATT		'R'	/* define pattern                 */
#define		MOVAREA		'r'	/* move area                      */
#define		DSR_ABC1600	'n'	/* device status report           */
#define		CPR_ABC1600	'R'	/* graphic cursor position report */
#define		PTG		'H'	/* put text cursor at graphic cur */
#define		MCNTRL		'M'	/* mouse control                  */
#define		GRORG		'O'	/* set graphic origin             */
#define		CLRALL		'J'	/* clear everything, home all
					   cursors and reset GRAPHIC flag
					   if current font is the same as
					   the default font               */
#define		MPR		'P'	/* mouse position report          */
#define		REPSFS		'W'	/* report virtual screen size and
					   current font                   */
#define		ABCLL		'q'	/* private load LED's             */
#define		FILLCIRC	'c'	/* fill circle                    */
#define		SM_ABC1600	'h'	/* private set mode               */
#define		RM_ABC1600	'l'	/* private reset mode             */
#define		SPRAY		's'	/* spray with pattern             */

/*
 * Characters ending escape sequences with CSI 'ESC'.
 */
#define		IND	'D'	/* index                       */
#define		NEL	'E'	/* next line                   */
#define		HTS	'H'	/* horizontal tabulation set   */
#define		RI	'M'	/* reverse index               */
#define		SC	'7'	/* save cursor                 */
#define		RC	'8'	/* restore cursor              */
#define		RIS	'c'	/* reset to initial state      */

/*
 * Different modes.
 */
#define		LNM	20			/* line feed newline mode             */
#define		DECSCNM	(10*('?'-'0')+5)	/* screen mode                        */
#define		DECOM	(10*('?'-'0')+6)	/* origin mode                        */
#define		DECAWM	(10*('?'-'0')+7)	/* auto wrap mode                     */
#define		TWPSMOD	(100*('?'-'0')+32)	/* page/scroll mode (Twist)           */
#define		TWCTYP	(100*('?'-'0')+33)	/* cursor type (Twist)                */
#define		TWCBLNK	(100*('?'-'0')+34)	/* blink/non-blink cursor (Twist)     */
#define		TWCVIS	(100*('?'-'0')+35)	/* visible/not visible cursor (Twist) */
#define		PHASEMOD	2		/* phased pattern mode (private)      */

/*
 * Different report requests.
 */
#define		TWSMOD	(10*('?'-'0')+1)	/* Twist screen mode report */
