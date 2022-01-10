/* w_const.h */

/*
 * Constant definitions for communication with the ABC-1600 Window Handler.
 */

/* 1985-07-23, Luxor Datorer AB */



#define		W_CONST


/*
 * The directory which the window handler is mounted on and pathname to use
 * to activate the window handler.
 */

#define		WMNTDIR		"/win"
#define		WACTIVATE	"/win/activate"



/*
 * Different size constants.
 */

#define		MAXWINS		16	/* max number of windows allowed    */
#define		MAXCOLS		256	/* max # of columns in a virtual
					   screen                           */
#define		TSTOPSIZE	8	/* size of array holding tab stops  */
#define		HDRSIZE		32	/* max size of window header        */
#define		HLPSIZE		16	/* max. length of help sequence     */
#define		MAXICONS	64	/* max number of icons
					   This limit only applies to the
					   first version of the window
					   handler (version 1.0)            */
#define		ICONSEQLEN	16	/* max length of icon string        */
#define		SUBSTKEYS	20	/* # of mouse substitute keys       */
#define		ZOOMSIZE	16	/* max # of fonts in a zoom list    */
#define		MPSIZE		32	/* max vertical size of mouse
					   pointer                          */
#define		BGPSIZE		16	/* vertical size of background
					   pattern                          */
#define		UBOXSIZE	16	/* # of elements in user box bitmap */



/*
 * Mnemonic constants.
 */

#define		OFF		0
#define		ON		1



/*
 * Return status returned by the new i/o control commands.
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
#define		WE_SPECIAL	10	/* the window can't change level
					   because it is a special one  */
#define		WE_NOMP		11	/* can't alter mouse pointer for
					   this window                  */
#define		WE_FATHER	12	/* no father window             */
#define		WE_ILMOD	13	/* illegal screen mode          */
#define		WE_TSAVE	14	/* text contents not saved      */
#define		WE_NOFONT	15	/* can't load specified font    */
#define		WE_SPACE	16	/* user buffer too small        */
#define		WE_OPEN		17	/* windows still open           */



/*
 * Different border types. The constants which looks like XXXXBORD where X
 * is either S or D stands for a combination of Single and Double lines
 * borders. The first letter is the left side, the second the right side, the
 * third the upper side, and the fourth the lower side.
 */

#define		NOBORDER	0	/* no border          */
#define		SLBORDER	1	/* single line border */
#define		DLBORDER	2	/* double line border */
#define		DSSSBORD	3
#define		SDSSBORD	4
#define		SSDSBORD	5
#define		SSSDBORD	6
#define		DDSSBORD	7
#define		DSDSBORD	8
#define		DSSDBORD	9
#define		SDDSBORD	10
#define		SDSDBORD	11
#define		SSDDBORD	12
#define		DDDSBORD	13
#define		DDSDBORD	14
#define		DSDDBORD	15
#define		SDDDBORD	16



/*
 * The width of the different borders (pixels).
 */

#define		P_NOBWIDTH	0	/* no border, portrait          */
#define		L_NOBWIDTH	0	/* landscape                    */
#define		P_SLBWIDTH	3	/* single line border, portrait */
#define		L_SLBWIDTH	3	/* landscape                    */
#define		P_DLBWIDTH	23	/* double line border, portrait */
#define		L_DLBWIDTH	23	/* landscape                    */



/*
 * Different background patterns.
 */

#define		BLACK		0	/* black background pattern */
#define		WHITE		1	/* white background pattern */



/*
 * Different screen modes.
 */

#define		M_PORT		0	/* portrait mode  */
#define		M_LAND		1	/* landscape mode */



/*
 * Status flags in the (long) status word for the window.
 */

#define	PMODE		((uflags)(1 << 0))	/* portrait mode                                */
#define	LMODE		((uflags)(1 << 1))	/* landscape mode                               */
#define	SAVETEXT	((uflags)(1 << 2))	/* save text contents flag                      */
#define	SAVEBITMAP	((uflags)(1 << 3))	/* save bitmap contents flag                    */
#define	OVERLAP		((uflags)(1 << 4))	/* window is overlapped flag                    */
#define	LOCK		((uflags)(1 << 5))	/* lock on highest level flag                   */
#define	NOOVER		((uflags)(1 << 6))	/* not allowed to overlap window                */
#define	NOCURSOR	((uflags)(1 << 7))	/* cursor not visible flag                      */
#define	NOMOVE		((uflags)(1 << 8))	/* window must not be moved or change size      */
#define	ALLSCR		((uflags)(1 << 9))	/* window must be whole virtual screen          */
#define	SPECIAL		((uflags)(1 << 10))	/* special window                               */
#define	KEYSCROLL	((uflags)(1 << 11))	/* make cursor visible when a key is pressed    */
#define	WRITSCROLL	((uflags)(1 << 12))	/* make cursor visible after writing something  */
#define	ALTMPNT		((uflags)(1 << 13))	/* possible to have an own mouse pointer        */
#define	RELATIVE	((uflags)(1 << 14))	/* add the window relative to another window    */
#define	NOCPIN		((uflags)(1 << 15))	/* not allowed to copy text to this window      */
#define	NOCPOUT		((uflags)(1 << 16))	/* not allowed to copy text from this window    */
#define	TXTSIZE		((uflags)(1 << 17))	/* sizes and window origin in term of chars     */
#define	WGROUP		((uflags)(1 << 18))	/* the window belongs to a window group         */
#define	REL_ULC		((uflags)(1 << 19))	/* relative upper left corner of father         */
#define	REL_URC		((uflags)(1 << 20))	/* relative upper right corner of father        */
#define	REL_LLC		((uflags)(1 << 21))	/* relative lower left corner of father         */
#define	REL_LRC		((uflags)(1 << 22))	/* relative lower right corner of father        */



/*
 * Flags indicating which boxes shall be present in the border.
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



/*
 * Flags for the window header.
 */

#define		H_INVHD		(1 << 0)	/* invert window header     */
#define		H_INVTOP	(1 << 1)	/* invert top window header */



/*
 * Flags used to indicate certain attributes, etc. of an icon.
 */

#define		I_PMODE		(1 << 0)	/* portrait mode           */
#define		I_LMODE		(1 << 1)	/* landscape mode          */
#define		I_INVERT	(1 << 2)	/* invert when pointed to  */
#define		I_PRESS		(1 << 3)	/* send when pressed       */
#define		I_RELEASE	(1 << 4)	/* send when release       */
#define		I_ENTER		(1 << 5)	/* send when enter area    */
#define		I_LEAVE		(1 << 6)	/* send when leave area    */
#define		I_RQST		(1 << 7)	/* send if any request     */
#define		I_REMOVE	(1 << 8)	/* remove after sending    */
#define		I_SETCHK	(1 << 9)	/* on set, check if inside */
#define		I_LZERO		(1 << 10)	/* send if level zero      */
#define		I_TEXT		(1 << 11)	/* coord. in text pos.     */



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
 * Flags used in the request to set up a zoom list.
 */

#define		Z_PMODE		(1 << 0)	/* portrait mode list given  */
#define		Z_LMODE		(1 << 1)	/* landscape mode list given */



/*
 * The initial terminal and tty driver parameters.
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
 * New i/o control commands implemented in the window handler.
 */

#define		WINCREAT	(('w' << 8) | 0)	/* create a window   */
#define		WINLEVEL	(('w' << 8) | 1)	/* exchange window   */
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
#define		WINCHMPNT	(('w' << 8) | 14)	/* alter mouse pntr  */
#define		WINCNT		(('w' << 8) | 15)	/* # of open wins    */
#define		WINRESTOR	(('w' << 8) | 16)	/* restore screen    */
#define		WINGETTXT	(('w' << 8) | 17)	/* get text contents */
#define		WINTEST		(('w' << 8) | 18)	/* window test       */
#define		WINSINIT	(('w' << 8) | 19)	/* set init values   */
#define		WINGINIT	(('w' << 8) | 20)	/* get init values   */
#define		WINZOOM		(('w' << 8) | 21)	/* set up zoom list  */
#define		WINTURN		(('w' << 8) | 22)	/* turn the screen   */
#define		WINMODE		(('w' << 8) | 23)	/* get screen mode   */
#define		WINNDCHR	(('w' << 8) | 24)	/* new default font  */
#define		WPICTRD		(('w' << 8) | 25)	/* read picture mem. */
#define		WINKYSIG	(('w' << 8) | 26)	/* keyboard signal   */
#define		WIN1LEV		(('w' << 8) | 27)	/* level 0 in group  */
#define		WIN1ALTER	(('w' << 8) | 28)	/* alter win in grp  */
#define		WINDFLSZ	(('w' << 8) | 29)	/* default win size  */
#define		WINUBOX		(('w' << 8) | 30)	/* add user box      */
#define		WINHELP		(('w' << 8) | 31)	/* new help sequence */

#define		PSPRAY		(('p' << 8) | 128)	/* set up spray mask */
