/* defs.h */

/*
 * This file contains constants and data type definitions for the ABC1600
 * window handler.
 */

/* 1984-06-28, Peter Andersson, Luxor Datorer AB (at DIAB) */


/*
 * Different size constants.
 */
#define		MAXWINS		16	/* max number of windows allowed    */
#define		MAXWINSIZE	((unsigned pix_d) (~0) / 4)
					/* max size of virtual screen in
					   pixels, this size prevents over-
					   flow                             */
#define		MAXVALPARA	MAXWINSIZE
					/* max allowed value of parameter
					   in escape sequence               */
#define		MAXCOLS		256	/* max # of columns in a virtual
					   screen                           */
#define		TP_PER_EL	(8 * sizeof(t_stop))
					/* number of tab stop markers that
					   can be held in one element       */
#define		TSTOPSIZE	8	/* size of array holding tab stop
					   markers                          */
#define		HDRSIZE		32	/* max size of window header        */
#define		HLPSIZE		16	/* max length of help sequence      */
#define		MAXESCPARA	10	/* max # of parameters in escape
					   sequence                         */
#define		TTYHOG		256	/* size of raw character input queue
					   buffer for each window           */
#define		ICONSEQLEN	16	/* max length of icon string        */
#define		ICONCHUNK	32	/* number of icon data structures
					   to allocate space for every time
					   no more space exists             */
#define		ICONHYST	(ICONCHUNK / 2)
					/* number of free icon data structures
					   before releasing any memory      */
#define		CBUFSIZE	(RECTTMPMAX * sizeof(struct wpstruc))
					/* size of character copy buffer    */
#define		BITBUFSIZE	(RECTTMPMAX * sizeof(struct wpstruc) / sizeof(byte))
					/* size of bitmap copy buffer       */
#define		TCPBUFSIZE	CBUFSIZE
					/* size of text copy buffer when
					   reading text contents            */
#define		IBSIZE		128	/* size of console input buffer     */
#define		MAXLOC		8	/* max number of locate opens       */
#define		SUBSTKEYS	20	/* # of mouse substitute keys       */
#define		ZOOMSIZE	16	/* max # of fonts in a zoom list    */
#define		UBOXSIZE	16	/* # of elements in user box bitmap */
#define		BOXCOUNT	10	/* # of different boxes in the
					   border                           */
#define		RECTTOTMAX	(4 * MAXWINS + 4 * (MAXWINS - 2) + 2 + 1)
					/* max total # of rectangles, see
					   the end of this file             */
#define		RECTTMPMAX	(6 * (MAXWINS - 1) + 3)
					/* max # of rectangles in the
					   temporary area obtained when
					   dividing rectangles, see the
					   end of this file             */

/*
 * Preliminary new signal added to make it possible to be noticed when there
 * is something to read from the tty driver. It only works if both RAW and
 * CBREAK modes are set.
 * This signal is by default ignored. Setting it to SIG_DFL or SIG_IGN is
 * equivalent.
 */
#define		SIGCHAR		17

/*
 * Some mnemonic constants.
 */
#define		OK		0
#define		ERROR		1
#define		NO		0
#define		YES		1
#define		KEY		1
#define		MOUSE		2
#define		OFF		0
#define		ON		1
#define		RELEASED	0
#define		PRESSED		1
#define		NORMAL		0
#define		EXTENDED	1
#define		NEWREQ		0	/* new request  */
#define		OLDREQ		1	/* old request  */
#define		NULL		0	/* null pointer */

/*
 * Some lu's.
 */
#define		MNTDIRLU	0	/* lu returned when mounting            */
#define		CON_LU		1	/* lu returned when opening the console */

/*
 * Some extra constants used to communicate with the tty device driver through
 * the shared memory page (more constants can be found in gpage.h).
 * Values greater or equal than 80 hex can also be received. This indicates
 * that a key that has been set up as a substitute for the mouse has been
 * pressed. In this case the value is the code sent from the ABC-99 keyboard
 * for the pressed key.
 */
#define		M_ILLEGAL	8	/* illegal code */
#define		M_SKEY		0x80

/*
 * Directory to mount the window handler on.
 */
#define		FMNTDIR		"/win"
#define		MNTDIR		"win"
#define		MNTDIRLEN	3		/* length of above */

/*
 * Temporary file used for copying text between windows. The last character in
 * the name is the destination window number plus 'A'.
 */
#define		CPTMPNAME	"/tmp/wincpA"
#define		CPTMPLEN	11
#define		CPTMPPROT	(CM_WRW | CM_WRE)

/*
 * Different constants concerning characters.
 */
#define		CMASK		127		/* removes eight bit of character         */
#define		MAX7BCHR	127		/* high ASCII code of 7 bit character     */
#define		REVATTR		(1 << 7)	/* reverse attribute flag (in saved text) */
#define		ESC		'\33'		/* escape ASCII code                      */

/*
 * New i/o control requests implemented in the window handler.
 */
#define		WINCREAT	(('w' << 8) | 0)	/* create a window   */
#define		WINLEVEL	(('w' << 8) | 1)	/* move to level 0   */
#define		WINALTER	(('w' << 8) | 2)	/* alter a window    */
#define		WINSTAT		(('w' << 8) | 3)	/* get window status */
#define		WINHEADER	(('w' << 8) | 4)	/* insert header     */
#define		WINICON		(('w' << 8) | 5)	/* add icon          */
#define		RMICONS		(('w' << 8) | 6)	/* remove icons      */
#define		WINFLAGS	(('w' << 8) | 7)	/* alter flags       */
#define		WINMSUB		(('w' << 8) | 8)	/* mouse substitute  */
#define		WINCHBG		(('w' << 8) | 9)	/* alter background  */
#define		WINGETVIS	(('w' << 8) | 10)	/* get visible parts */
#define		WINIVIDEO	(('w' << 8) | 11)	/* inverse video     */
#define		WINNVIDEO	(('w' << 8) | 12)	/* non-inverse video */
#define		WINCURVIS	(('w' << 8) | 13)	/* make cur. visible */
#define		WINCHMPNT	(('w' << 8) | 14)	/* change mouse pntr */
#define		WINCNT		(('w' << 8) | 15)	/* # of open wins    */
#define		WINRESTOR	(('w' << 8) | 16)	/* restore screen    */
#define		WINGETTXT	(('w' << 8) | 17)	/* get text contents */
#define		WINTEST		(('w' << 8) | 18)	/* test if windows   */
#define		WINSINIT	(('w' << 8) | 19)	/* set init values   */
#define		WINGINIT	(('w' << 8) | 20)	/* get init values   */
#define		WINZOOM		(('w' << 8) | 21)	/* set up zoom list  */
#define		WINTURN		(('w' << 8) | 22)	/* turn screen       */
#define		WINMODE		(('w' << 8) | 23)	/* get screen mode   */
#define		WINNDCHR	(('w' << 8) | 24)	/* new default font  */
#define		WPICTRD		(('w' << 8) | 25)	/* read picture mem. */
#define		WINKYSIG	(('w' << 8) | 26)	/* keyboard signal   */
#define		WIN1LEV		(('w' << 8) | 27)	/* to top in group   */
#define		WIN1ALTER	(('w' << 8) | 28)	/* alter in group    */
#define		WINDFLSZ	(('w' << 8) | 29)	/* default win size  */
#define		WINUBOX		(('w' << 8) | 30)	/* add a user box    */
#define		WINHELP		(('w' << 8) | 31)	/* new help sequence */

#define		PSPRAY		(('p' << 8) | 128)	/* set up spray mask */

/*
 * Return status returned by the new i/o control requests.
 */
#define		W_OK		0	/* OK                           */
#define		WE_ILPARA	1	/* illegal parameter            */
#define		WE_LORO		2	/* locked or will be overlapped */
#define		WE_NOTCR	3	/* window not created yet       */
#define		WE_ALRCR	4	/* window already created       */
#define		WE_NOMEM	5	/* no memory left               */
#define		WE_NOICON	6	/* no space left for new icon   */
#define		WE_ONICON	7	/* on another icon              */
#define		WE_NOMOVE	8	/* window may not be moved      */
#define		WE_ALLSCR	9	/* whole virtual screen must be
					   visible                      */
#define		WE_SPECIAL	10	/* can't change level because the
					   window is a special window   */
#define		WE_NOMP		11	/* can't change mouse pointer for
					   this window                  */
#define		WE_FATHER	12	/* no father window             */
#define		WE_ILMOD	13	/* illegal screen mode          */
#define		WE_TSAVE	14	/* text contents not saved      */
#define		WE_NOFONT	15	/* can't load specified font    */
#define		WE_SPACE	16	/* user buffer too small        */
#define		WE_OPEN		17	/* windows still open           */

/*
 * Panic error codes written to the console by the window handler.
 */
#define		HE_NODIR	1	/* no directory to mount on      */
#define		HE_MOUNT	2	/* unable to mount               */
#define		HE_MAPIO	3	/* can't map in I/O pages        */
#define		HE_MAPGR	4	/* can't map in graphic memory   */
#define		HE_RQUE		5	/* can't read request queue      */
#define		HE_RHINFO	6	/* can't read handler info       */
#define		HE_WHINFO	7	/* can't write handler info      */
#define		HE_RRRQ		8	/* can't reread request queue    */
#define		HE_NOCON	9	/* no console                    */
#define		HE_CONOPN	10	/* cannot open the console       */
#define		HE_KYBRD	11	/* cannot set up the keyboard    */
#define		HE_WHP		12	/* cannot execute 'whp'          */
#define		HE_WHL		13	/* cannot execute 'whl'          */
#define		HE_WHGO		14	/* cannot execute 'whgo'         */
#define		HE_FATAL	15	/* fatal error in window handler */

/*
 * The number of different border types and some particular border types.
 */
#define		MAXBORDER	17
#define		NOBORDER	0	/* no border           */
#define		SLBORDER	1	/* single lines border */

/*
 * The width of the different border sides.
 */
#define		NOBWIDTH	0	/* no border          */
#define		SLBWIDTH	3	/* single line border */
#ifdef LANDSCAPE
#define		DLBWIDTH	23	/* double line border */
#else  PORTRAIT
#define		DLBWIDTH	23	/* double line border */
#endif LANDSCAPE / PORTRAIT

/*
 * Flags used to indicate which parts of the border that can hold a box.
 */
#define		BD_LEFT		(1 << 0)	/* left side          */
#define		BD_RIGHT	(1 << 1)	/* right side         */
#define		BD_UP		(1 << 2)	/* upper side         */
#define		BD_DOWN		(1 << 3)	/* lower side         */
#define		BD_ULC		(1 << 4)	/* upper left corner  */
#define		BD_URC		(1 << 5)	/* upper right corner */
#define		BD_DLC		(1 << 6)	/* lower left corner  */
#define		BD_DRC		(1 << 7)	/* lower right corner */

/*
 * Different constants concerning the headers.
 */
#define		HEADOFFS	4	/* displacement from upper side of border
					   to upper side of header (pixels) */

/*
 * The distance from the outer side of the border to the boxes in the border.
 */
#ifdef LANDSCAPE
#define		BOXOFFS		3
#else  PORTRAIT
#define		BOXOFFS		3
#endif LANDSCAPE / PORTRAIT

/*
 * The area occupied by each user box (including the distance between them).
 */
#define		UBOXTSIZ	20

/*
 * The distance from the outer side of the border to the visible indicators
 * in the border.
 */
#define		INDOFFS		(BOXOFFS + 1)

/*
 * Max vertical size of the mouse pointer (the horizontal size is 32).
 */
#define		MPSIZE		32

/*
 * Vertical size of the pattern used to build up the background (the horizontal
 * size is 16).
 */
#define		BGPSIZE		16

/*
 * Different background pattern codes. Note that BLACK and WHITE are used
 * as limits in tests.
 */
#define		BLACK		0	/* black background pattern     */
#define		WHITE		1	/* white background pattern     */
#define		PATTERN		2	/* draw background pattern code */

/*
 * Different screen modes.
 */
#define		M_PORT		0	/* portrait mode  */
#define		M_LAND		1	/* landscape mode */

/*
 * The initial setup of the mouse.
 */
#define		MXPOS		384	/* initial x position */
#define		MYPOS		512	/* initial y position */
#define		MXSCALE		1	/* x scale            */
#define		MYSCALE		1	/* y scale            */
#define		MXINCR		2	/* x increment        */
#define		MYINCR		2	/* y increment        */
#define		MXMINLIM	0	/* x min limit        */
#define		MXMAXLIM	767	/* x max limit        */
#define		MYMINLIM	0	/* y min limit        */
#define		MYMAXLIM	1023	/* y max limit        */

/*
 * Flags in the internal status word for each window.
 */
#define	ESCMODE		((dword)(1 << 0))	/* escape sequence under processing  */
#define	ESCTRLSEQ	((dword)(1 << 1))	/* control escape sequence           */
#define	ESCGRSEQ	((dword)(1 << 2))	/* private escape sequence           */
#define	ESCSEQ		((dword)(1 << 3))	/* escape sequence without parameter */
#define	SELCHSET	((dword)(1 << 4))	/* select character set escape seq.  */
#define	CURSAVED	((dword)(1 << 5))	/* cursor saved flag                 */
#define	CREATED		((dword)(1 << 6))	/* window is created                 */
#define	ENDLINE		((dword)(1 << 7))	/* written char at last pos. on line */
#define	WAITCLOS	((dword)(1 << 8))	/* window waiting to be closed       */
#define	TTSTOP		((dword)(1 << 9))	/* output stopped by CTRL-S          */
#define	MOUSCHNG	((dword)(1 << 10))	/* the mouse position has changed    */
#define	MOUSBCHNG	((dword)(1 << 11))	/* a mouse button has changed        */
#define	MOUSREP		((dword)(1 << 12))	/* report mouse when next change     */
#define	MOUSBREP	((dword)(1 << 13))	/* report next button change         */
#define	CPTMPFILE	((dword)(1 << 14))	/* text copy buffer file present     */
#define	GRAPHIC		((dword)(1 << 15))	/* text is considered as graphics    */
#define	INVHEAD		((dword)(1 << 16))	/* invert header                     */
#define	INVTOP		((dword)(1 << 17))	/* invert top window header          */
#define	FATHER		((dword)(1 << 18))	/* a father window                   */
#define	MPOVER		((dword)(1 << 19))	/* overlapped by mouse pointer       */

/*
 * Status flags in the external (long) status word for the window.
 */
#define	PMODE		((uflags)(1 << 0))	/* portrait mode                             */
#define	LMODE		((uflags)(1 << 1))	/* landscape mode                            */
#define	SAVETEXT	((uflags)(1 << 2))	/* save text contents flag                   */
#define	SAVEBITMAP	((uflags)(1 << 3))	/* save bitmap contents flag (future use)    */
#define	OVERLAP		((uflags)(1 << 4))	/* window is overlapped flag                 */
#define	LOCK		((uflags)(1 << 5))	/* locked on highest level flag              */
#define	NOOVER		((uflags)(1 << 6))	/* not allowed to overlap window             */
#define	NOCURSOR	((uflags)(1 << 7))	/* cursor not visible (off) flag             */
#define	NOMOVE		((uflags)(1 << 8))	/* window may not be moved or change size    */
#define	ALLSCR		((uflags)(1 << 9))	/* window must be whole virtual screen       */
#define	SPECIAL		((uflags)(1 << 10))	/* special window                            */
#define	KEYSCROLL	((uflags)(1 << 11))	/* make cursor visible when key pressed      */
#define	WRITSCROLL	((uflags)(1 << 12))	/* make cursor visible after write request   */
#define	ALTMPNT		((uflags)(1 << 13))	/* window has a mouse pointer on its own     */
#define	RELATIVE	((uflags)(1 << 14))	/* add the window relative to another window */
#define	NOCPIN		((uflags)(1 << 15))	/* not allowed to copy text to this window   */
#define	NOCPOUT		((uflags)(1 << 16))	/* not allowed to copy text from this window */
#define	TXTSIZE		((uflags)(1 << 17))	/* sizes and window origin in term of chars  */
#define	WGROUP		((uflags)(1 << 18))	/* window belongs to a window group          */
#define	REL_ULC		((uflags)(1 << 19))	/* relative upper left corner of father      */
#define	REL_URC		((uflags)(1 << 20))	/* relative upper right corner of father     */
#define	REL_LLC		((uflags)(1 << 21))	/* relative lower left corner of father      */
#define	REL_LRC		((uflags)(1 << 22))	/* relative lower right corner of father     */

/*
 * Flags indicating which boxes shall be present in the border. The last two
 * are only used internally.
 */
#define		BX_HSCR		(1 << 0)	/* horizontal scroll         */
#define		BX_VSCR		(1 << 1)	/* vertical scroll           */
#define		BX_CLOS		(1 << 2)	/* close box                 */
#define		BX_SIZE		(1 << 3)	/* size box                  */
#define		BX_MOVE		(1 << 4)	/* move box                  */
#define		BX_ZOOM		(1 << 5)	/* zoom box                  */
#define		BX_AVIS		(1 << 6)	/* no scroll if all visible  */
#define		BX_BLOW		(1 << 7)	/* blow up box               */
#define		BX_HELP		(1 << 8)	/* help box                  */
#define		BX_USER		(1 << 9)	/* max # of user boxes given */
#define		BXI_HIND	(1 << 10)	/* horizontal indicator vis. */
#define		BXI_VIND	(1 << 11)	/* vertical indicator vis.   */

/*
 * Action codes telling what to do when a box in the border has been used.
 */
#define		BA_CLOS		0	/* close box        */
#define		BA_MOVE		1	/* move box         */
#define		BA_SIZE		2	/* size box         */
#define		BA_ZOOM		3	/* zoom box         */
#define		BA_BLOW		4	/* blow up box      */
#define		BA_HELP		5	/* help box         */
#define		BA_USCR		6	/* scroll up box    */
#define		BA_DSCR		7	/* scroll down box  */
#define		BA_LSCR		8	/* scroll left box  */
#define		BA_RSCR		9	/* scroll right box */

/*
 * Flags to describe the type of icon etc.
 */
#define		I_PMODE		(1 << 0)	/* portrait mode           */
#define		I_LMODE		(1 << 1)	/* landscape mode          */
#define		I_INVERT	(1 << 2)	/* invert when pointing to */
#define		I_PRESS		(1 << 3)	/* send when pressed       */
#define		I_RELEASE	(1 << 4)	/* send when released      */
#define		I_ENTER		(1 << 5)	/* send when enter area    */
#define		I_LEAVE		(1 << 6)	/* send when leave area    */
#define		I_RQST		(1 << 7)	/* send if any request     */
#define		I_REMOVE	(1 << 8)	/* remove after sending    */
#define		I_SETCHK	(1 << 9)	/* on set, check if inside */
#define		I_LZERO		(1 << 10)	/* send if level zero      */
#define		I_TEXT		(1 << 11)	/* coord. in text pos.     */

/*
 * Flags for the window header.
 */
#define		H_INVHD		(1 << 0)	/* invert window header     */
#define		H_INVTOP	(1 << 1)	/* invert top window header */

/*
 * Index to the different keys in the substit structure.
 */
#define		S_ONOFF		0	/* toggle substitute keys on/off  */
#define		S_MPU		1	/* move pointer up                */
#define		S_MPD		2	/* move pointer down              */
#define		S_MPL		3	/* move pointer left              */
#define		S_MPR		4	/* move pointer right             */
#define		S_MPUL		5	/* move pointer up - left         */
#define		S_MPUR		6	/* move pointer up - right        */
#define		S_MPDL		7	/* move pointer down - left       */
#define		S_MPDR		8	/* move pointer down - right      */
#define		S_LMPU		9	/* move pointer long up           */
#define		S_LMPD		10	/* move pointer long down         */
#define		S_LMPL		11	/* move pointer long left         */
#define		S_LMPR		12	/* move pointer long right        */
#define		S_LMPUL		13	/* move pointer long up - left    */
#define		S_LMPUR		14	/* move pointer long up - right   */
#define		S_LMPDL		15	/* move pointer long down - left  */
#define		S_LMPDR		16	/* move pointer long down - right */
#define		S_PCMD		17	/* point command key              */
#define		S_CHWIN		18	/* change window key              */
#define		S_MCA		19	/* mark copy area key             */

/*
 * The initial terminal flags (struct wininit).
 */
#define		TD_NL		(1 << 0)	/* do CR-LF on '\n'          */
#define		TD_WRAP		(1 << 1)	/* force CR-LF at right marg */
#define		TD_ORIGIN	(1 << 2)	/* origin mode               */
#define		TD_USCORE	(1 << 3)	/* underscore attribute      */
#define		TD_REVERSE	(1 << 4)	/* reverse attribute         */
#define		TD_SCREEN	(1 << 5)	/* screen mode               */
#define		TD_CUNDER	(1 << 6)	/* underline cursor          */
#define		TD_NONBLNK	(1 << 7)	/* non-blinking cursor       */
#define		TD_PHASE	(1 << 8)	/* phased pattern mode       */
#define		TD_NOSCR	(1 << 9)	/* no scroll (page) mode     */

/*
 * Flags in the zoom list set up request.
 */
#define		Z_PMODE		(1 << 0)	/* portrait mode fonts given  */
#define		Z_LMODE		(1 << 1)	/* landscape mode fonts given */

/*
 * Flags used to keep track of the status of the left and middle mouse buttons
 * or the corresponding keyboard key.
 * Also a flag indicating if it has been computed which windows are overlapped
 * by the mouse pointer.
 */
#define		PD_KYBRD	(1 << 0)	/* keyboard key flag        */
#define		PD_LPRESS	(1 << 1)	/* left button is pressed   */
#define		PD_MPRESS	(1 << 2)	/* middle button is pressed */
#define		PD_LSKIP	(1 << 3)	/* skip next left release   */
#define		PD_MSKIP	(1 << 4)	/* skip next middle release */
#define		PD_OLAP		(1 << 5)	/* overlap computed or not  */

/*
 * Flags in the 'fi_flag' member of the font information structure 'finfo'.
 */
#define		NOFONT		(1 << 0)	/* font not set up or active */

/*
 * Flags indicating if the border is included in a rectangle.
 */
#define		B_LEFT		(1 << 0)	/* border on the left side  */
#define		B_RIGHT		(1 << 1)	/* border on the right side */
#define		B_UP		(1 << 2)	/* border on the upper side */
#define		B_LOW		(1 << 3)	/* border on the lower side */
#define		B_MASK		(B_LEFT | B_RIGHT | B_UP | B_LOW)
#define		B_NOTEMPTY	(1 << 4)	/* rectangle not empty when
						   border removed flag      */

/*
 * Some new data types.
 */
typedef		 short	pix_d;		/* distance in pixel units        */
typedef		 short	cur_d;		/* distance in cursor positions   */
typedef		 char	sint;		/* small integer                  */
typedef	unsigned char	byte;		/* a byte                         */
typedef unsigned short	word;		/* a word (2 bytes)               */
typedef unsigned long	dword;		/* a double word (4 bytes)        */
typedef		 dword	uflags;		/* type of user flags             */
typedef	unsigned long	t_stop;		/* to hold tab stop marks         */


/*
 * An explanation of the choice of RECTTOTMAX and RECTTMPMAX:
 *
 * Both these values are the worst cases found so far.
 * The worst case for RECTTOTMAX is when all windows are located in a cross-
 * like manner and all the corners are in the background. In this case the
 * background is divided into 4*MAXWINS rectangles, all the windows except
 * the two topmost ones are divided into 4 rectangles, the window on level
 * one is divided into 2 rectangles, and the level zero window is divided
 * into 1 rectangle (the window itself). This makes a total of
 * 4*MAXWINS + 4*(MAXWINS - 2) + 2 + 1 rectangles.
 * The worst case for RECTTMPMAX is when (MAXWINS - 1) windows are put as
 * described above and the last window is located so it crosses all the
 * rectangle borders on one side of the crosslike formation. In the background
 * the crosslike formation gives 4*(MAXWINS - 1) rectangles. The last window
 * divides completely 2*(MAXWINS - 1) - 1 into two new rectangles each, and
 * lastly the four corners in the last window gives at most 4 new rectangles.
 * This gives 4*(MAXWINS - 1) + 2*(MAXWINS - 1) - 1 + 4 = 6*(MAXWINS - 1) + 3
 * rectangles. Note that this is the count when the rectangles has not been
 * concatenated after the last window has been added.
 */
