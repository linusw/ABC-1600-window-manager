/* eglobals.h */

/*
 * This file contains external global declarations for the ABC1600 window
 * handler.
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
extern	char	*mem_high;

/*
 * Pointer to memory page shared with tty device driver.
 */
extern	struct	iogrst	*cmpgp;

/*
 * This structure is used to hold data for the font used for window headers.
 */
extern	struct	finfo	hdf_data;

/*
 * Flag indicating if there are characters to read from the console or a change
 * to the mouse has occured.
 * This flag is set by the signal routine consig().
 */
extern	sint	keyin;

/*
 * Number of windows currently created.
 */
extern	int	nwin;

/*
 * Number of non-special windows (always less than or equal to 'nwin').
 */
extern	int	nnswin;

/*
 * Pattern used to draw contours on the background.
 */
extern	sint	bg_cont;

/*
 * Flag indicating if global inverse video or not.
 */
extern	sint	inversevideo;

/*
 * Pointer to structures holding the internal status for all the windows.
 * If a pointer is NULL, the window number is not used
 */
extern	struct	winint	*wstatp[];

/*
 * Structures holding the information about which parts of the windows that
 * are visible. The background is also stored here.
 * The array wpp[] is used as pointers into this array.
 */
extern	struct	wpstruc	wp[];

/*
 * Pointers for each window into the wp[] array. The window number shall be
 * used as index into this array.
 */
extern	struct	wpstruc	*wpp[];

/*
 * Total number of rectangles which the background and the windows are divided
 * into.
 */
extern	int	rtotal;

/*
 * Number of rectangles that each window is divided into. The window number
 * shall be used as index into this array.
 */
extern	int	nwpart[];

/*
 * Temporary area used by e.g. alter1win(). Enough space is reserved to divide
 * a rectangle by overlaying it with one window.
 */
extern	struct	wpstruc	wpt1r[];

/*
 * Array used to map the level number to the window number, i.e. with the
 * level number as index the window number is obtained. The background is
 * always at the lowest level and has window number MAXWINS.
 */
extern	int	ltow[];

/*
 * Array used to map the window number to the level number, i.e. with the
 * window number as index the level is obtained. The background is always
 * window MAXWINS and it is always at a lower level than all other windows.
 */
extern	int	wtol[];

/*
 * Array containing the width and some flags concerning the different border
 * types.
 */
extern	struct	borddst	borddata[];

/*
 * Array containing data for the different boxes in the border.
 */
extern	struct	boxactd	boxdata[];

/*
 * Default (initial) values for the graphic pattern tables.
 */
extern	word	msk1[];		/* horizontal masks */

extern	word	msk2[];		/* vertical masks   */

extern	byte	shft[];		/* shift counts     */

extern	byte	op[];		/* operations       */

/*
 * Default bit pattern used by the spray escape sequence.
 */
extern	struct	sprayst	spraym;

/*
 * Structure containing hardware addresses.
 */
extern	struct	hwastruc	hwa;

/*
 * Structure containing data about the current position of the mouse pointer.
 */
extern	struct	pntstruc	pntdata;

/*
 * Structure containing the form and other data of the global mouse pointer
 * currently used.
 */
extern	struct	mpstruc	mpdat;

/*
 * Pointer to data for the different icons.
 */
extern		int	n_icons;	/* # of icons currently set up */
extern		int	tot_icons;	/* total # of allocated icons  */
extern	struct	icondat	*iconsave;	/* should equal 'mem_high' if
					   no bugs                     */

/*
 * Flags and data indicating an inverted icon.
 */
extern	struct	invsave	invdata;

/*
 * Data to keep track of the state of the text cursor.
 */
extern struct	cursave	curdata;

/*
 * Structure containing the mouse substitute keys.
 */
extern	struct	substit	subkys;

/*
 * Buffer used when serving VT100 escape reports. Note that the size of the
 * buffer may have to be increased if support for a new report is added.
 */
extern	char	repvt100[];

/*
 * Buffer used when serving private escape reports. Note that the size of the
 * buffer may have to be increased if support for a new report is added.
 */
extern	char	repprivate[];

/*
 * Buffer used to copy data to and from the user's buffer.
 * Also used as temporary area.
 */
extern	union	buffer	ubuf;

/*
 * Structure used to save the tty status before setting raw, cbreak, no echo mode.
 */
extern	struct	sgttyb	ttysave;
extern	short		ttysflg;	/* tty status saved flag */

/*
 * Structure used to hold the default terminal and tty parameters used when a
 * new window is created. The tty parameters are obtained when the window
 * handler is started by a TCGETA ioctl command to the console.
 */
extern	struct	wininit	ttydflt;

/*
 * Buffer to receive characters from the console.
 */
extern	unsigned char	inbuf[];

/*
 * Flags for logical unit numbers returned when locate requests are received.
 * If a flag is non-zero, the lu is occupied and it is the index plus
 * (MAXWINS + 1).
 */
extern	sint	locflg[];

/*
 * The name of the window handler.
 */
extern	char	*hndname;

/*
 * The name of the temporary files used for text copying between windows.
 */
extern	char	cptmpname[];
