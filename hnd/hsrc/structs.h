/* structs.h */

/*
 * This file contains structure declarations for the ABC1600 window handler.
 */

/* 1984-06-28, Peter Andersson, Luxor Datorer AB (at DIAB) */

/*
 *  ***  The defs.h file must be included before this file  ***
 */

#include	"../hsrc/terminal.h"
#include	"../hsrc/gdriver.h"
#include	"../../gdrv/gpage.h"



/*
 * The structure used to create a window, receive the window status, etc.
 */
struct	winstruc {
	pix_d	wp_xorig;	/* x coordinate of lower left corner of the
				   virtual screen, pixels                   */
	pix_d	wl_xorig;	/* dito, landscape mode                     */
	pix_d	wp_yorig;	/* y coordinate                             */
	pix_d	wl_yorig;	/* dito, landscape mode                     */
	pix_d	wp_xsize;	/* width of the virtual screen, pixels      */
	pix_d	wl_xsize;	/* dito, landscape mode                     */
	pix_d	wp_ysize;	/* height of the virtual screen, pixels     */
	pix_d	wl_ysize;	/* dito, landscape mode                     */
	pix_d	wp_vxorig;	/* x coordinate of lower left corner of the
				   window (not including the border)        */
	pix_d	wl_vxorig;	/* dito, landscape mode                     */
	pix_d	wp_vyorig;	/* y coordinate                             */
	pix_d	wl_vyorig;	/* dito, landscape mode                     */
	pix_d	wp_vxsize;	/* width of the window (not including the
				   border)                                  */
	pix_d	wl_vxsize;	/* dito, landscape mode                     */
	pix_d	wp_vysize;	/* height of the window                     */
	pix_d	wl_vysize;	/* dito, landscape mode                     */
	short	w_color;	/* background colour                        */
	sint	w_border;	/* border type                              */
	char	wp_font;	/* default font, portrait mode              */
	char	wl_font;	/* default font, landscape mode             */
	char	w_curfont;	/* current font                             */
	sint	w_level;	/* window level (0 - MAXWINS-1)             */
	sint	w_uboxes;	/* max. number of user defined boxes        */
	cur_d	w_xcur;		/* text cursor position (greater than
				   zero), x coordinate                      */
	cur_d	w_ycur;		/* text cursor position (greater than
				   zero), y coordinate                      */
	pix_d	w_xgcur;	/* graphic cursor position, x coordinate    */
	pix_d	w_ygcur;	/* graphic cursor position, y coordinate    */
	sint	w_tsig;		/* signal to send when becomming top window */
	sint	w_ntsig;	/* signal to send when leaving top          */
	sint	w_rsig;		/* signal to send when window change        */
	sint	w_csig;		/* signal to send when close box            */
	word	w_boxes;	/* boxes in the border                      */
	uflags	w_flags;	/* flags for the window                     */
	sint	w_rstat;	/* return status (for calls to the handler) */
	union {
		long	w_xxx;	/* future use                               */
	} w_pad;
};

/*
 * Structure describing the raw input queue (used by the tty device driver
 * emulator).
 */
struct	w_que {
	short		c_cc;		/* character count               */
	unsigned char	*c_cb;		/* start of buffer               */
	unsigned char	*c_cf;		/* pointer to first character    */
	unsigned char	*c_cl;		/* pointer beyond last character */
};

/*
 * Structure describing the output character queue (used by the tty device
 * driver emulator).
 */

struct	w_oque {
	short		c_cc;		/* character count          */
	unsigned char	*c_cb;		/* start of buffer          */
	unsigned char	*c_cn;		/* next character in buffer */
};

/*
 * This is a modified version of the 'tty' structure used in the standard tty
 * device driver.
 * The two extra elements in the t_ccs[] array is used to hold the version 7
 * mode EOF and EOL characters.
 */
struct	w_tty {
	struct	w_que	t_inq;		/* input queue description            */
	struct	w_oque	t_outq;		/* output queue description           */
	short		t_delct;	/* # of delimiters in raw input queue */
	unsigned short	t_iflag;	/* input modes                        */
	unsigned short	t_oflag;	/* output modes                       */
	unsigned short	t_cflag;	/* control modes                      */
	unsigned short	t_lflag;	/* line discipline modes              */
	unsigned char	t_ccs[NCC+2];	/* control characters                 */
	char		t_ispeed;	/* input speed, for v7 compatibility  */
	char		t_ospeed;	/* output speed, for v7 compatibility */
};

/*
 * Structure containing data which describes the current format of the mouse
 * pointer.
 */
struct	mpstruc {
	pix_d	mp_xsize;	/* x size of mouse pointer               */
	pix_d	mp_ysize;	/* y size of mouse pointer               */
	pix_d	mp_xpnt;	/* x pointing pixel in the mouse pointer */
	pix_d	mp_ypnt;	/* y pointing pixel                      */
	dword	mp_and[MPSIZE];	/* and mask to construct mouse pointer   */
	dword	mp_or[MPSIZE];	/* or mask to construct mouse pointer    */
};

/*
 * Structure to hold information about waiting requests.
 * Not served requests are held in a linked list (separate lists for read and
 * write requests) by using the handler information in the requests. Only the
 * first member of this structure is used and it points to the next request
 * in the queue.
 */
struct	rqque	{
	short	q_next;		/* next request in queue (request #) */
	short	q_last;		/* last request in queue (request #) */
	short	q_cnt;		/* number of waiting requests        */
};

/*
 * Structure containing information about a font.
 * The fi_used member of this structure contains flags for each font indicating
 * if the font has been used in the window or not. As it is of type long, it
 * sets a limit on the number of fonts which can be used (bit 0 indicates if
 * font A has been used etc.).
 */
struct	finfo {
	byte		fi_flag;	/* flags about the font, i.e. font
					   active or not                    */
	sint		fi_cur;		/* current font number - 'A'        */
	sint		fi_xsize;	/* x size of font box               */
	sint		fi_ysize;	/* y size of font box               */
	sint		fi_base;	/* baseline of font matrix          */
	sint		fi_chb;		/* blocking factor for the font     */
	unsigned char	fi_cfirst;	/* code of first char in font       */
	unsigned char	fi_clast;	/* code of last char in font        */
	pix_d		fi_start;	/* start pixel line of loaded font  */
	long		fi_used;	/* flags indicating which fonts are
					   used                             */
	dword		fi_size;	/* size of box in mover coordinates */
	dword		*fi_fbas;	/* pointer to "start" of font table */
	dword		fi_cgtab[MAX7BCHR+1-' '];
					/* table with pointers to the font
					   boxes                            */
};

/*
 * Structure used to hold a spray mask.
 */
struct	sprayst {
	dword	sp_mask[8*sizeof(dword)];
};

/*
 * Internal status structure for each window.
 */
struct	winint {
	pix_d	x_orig;		/* x coordinate of upper left corner of the
				   virtual screen, pixels                   */
	pix_d	y_orig;		/* y coordinate                             */
	pix_d	x_size;		/* width of the virtual screen, pixels      */
	pix_d	y_size;		/* height of the virtual screen, pixels     */
	pix_d	vx_orig;	/* x coordinate of upper left corner of the
				   window (not including the border)        */
	pix_d	vy_orig;	/* y coordinate                             */
	pix_d	vx_size;	/* width of the window (not including the
				   border)                                  */
	pix_d	vy_size;	/* height of the window                     */
	pix_d	bx_size;	/* x size of window including border        */
	pix_d	by_size;	/* y size of window including border        */
	short	color;		/* background colour                        */
	sint	dflxsz;		/* x size of the default font               */
	sint	dflysz;		/* y size of the default font               */
	char	font;		/* default font (A - Z)                     */
	sint	border;		/* border type                              */
	word	bordflags;	/* flags indicating which sides and corners
				   in the border can hold a box             */
	cur_d	x_cur;		/* text cursor position (greater than
				   zero), x coordinate                      */
	cur_d	y_cur;		/* text cursor position (greater than
				   zero), y coordinate                      */
	pix_d	x_coffs;	/* offset from left margin for text cursor  */
	pix_d	y_coffs;	/* offset from upper margin for text cursor */
	pix_d	x_gcur;		/* graphic cursor position, x coordinate    */
	pix_d	y_gcur;		/* graphic cursor position, y coordinate    */
	pix_d	x_gorig;	/* graphic origin, x                        */
	pix_d	y_gorig;	/* graphic origin, y                        */
	pix_d	abx_orig;	/* abs. upper left corner including border  */
	pix_d	aby_orig;	/* as above, but y coordinate               */
	pix_d	abx_lrc;	/* abs. lower right corner including border */
	pix_d	aby_lrc;	/* as above, but y coordinate               */
	pix_d	avx_orig;	/* abs. upper left corner excluding border  */
	pix_d	avy_orig;	/* as above, but y coordinate               */
	pix_d	avx_lrc;	/* abs. lower right corner excluding border */
	pix_d	avy_lrc;	/* as above, but y coordinate               */

	/* default window size and location when the blow up box is used    */

	struct {
		pix_d	dx_orig;	/* origin of virtual screen         */
		pix_d	dy_orig;
		pix_d	dvx_orig;	/* origin of window                 */
		pix_d	dvy_orig;
		pix_d	dvx_size;	/* size of window                   */
		pix_d	dvy_size;
	} dblow;

	/* the size and location when the window was last blown up          */

	struct {
		pix_d	ox_orig;	/* origin of virtual screen         */
		pix_d	oy_orig;
		pix_d	ovx_orig;	/* origin of window                 */
		pix_d	ovy_orig;
		pix_d	ovx_size;	/* size of window                   */
		pix_d	ovy_size;
	} oblow;

	cur_d	rm_cur;		/* rightmost character cursor position      */
	cur_d	bm_cur;		/* bottommost character cursor position     */
	cur_d	umr_cur;	/* upper margin in scroll region            */
	cur_d	bmr_cur;	/* lower margin in scroll region            */
	cur_d	columns;	/* original number of columns in virtual
				   screen                                   */
	cur_d	rows;		/* original number of rows in virtual
				   screen                                   */

	cur_d	cl_vis;		/* leftmost character coordinate that may be
				   visible in the window                    */
	cur_d	cr_vis;		/* rightmost character                      */
	cur_d	cu_vis;		/* uppermost character                      */
	cur_d	cd_vis;		/* lowermost character                      */

	t_stop	tabstop[TSTOPSIZE];
				/* tab stops                                */

	unsigned char hdr[HDRSIZE];
				/* window header                            */
	sint	hdrlen;		/* length of window header string           */

	sint	zoomindex;	/* index into zoom list                     */
	char	zoomlist[ZOOMSIZE+2];
				/* the zoom list (terminated by null)       */
	pix_d	escpara[MAXESCPARA];
				/* parameter stack for escape sequences     */
	sint	nescpara;	/* # of parameters in escape sequences      */
	sint	helplen;	/* length of help sequence                  */
	char	help[HLPSIZE];	/* help sequence                            */
	/* the graphic pattern tables for the window */

	word	msk1[16];	/* horizontal masks                         */
	word	msk2[16];	/* vertical masks                           */
	byte	shft[16];	/* shift counts                             */
	byte	op[16];		/* operations                               */
	struct	sprayst	spraym;
				/* spray mask                               */

	word	boxes;		/* boxes in the border                      */
	word	bpresent;	/* boxes currently present in the border    */
	struct	uboxst	*uboxp;	/* pointer to user defined boxes            */
	sint	uboxcnt;	/* number of user boxes set up              */
	sint	uboxmax;	/* max. number of user boxes allowed        */
	sint	ubxcrrnt;	/* number of user boxes currently shown     */
	dword	status;		/* status flags for the window              */
	uflags	estatus;	/* external status flags for window         */
	word	termpara;	/* flags for the terminal emulator (VT100)  */

	/* structure to remember things for the push/pop cursor escape seq. */

	struct {
		cur_d	sx_cur;		/* x text cursor position           */
		cur_d	sy_cur;		/* y text cursor position           */
		pix_d	sx_gcur;	/* x graphic cursor position        */
		pix_d	sy_gcur;	/* y graphic cursor position        */
		pix_d	sx_gorig;	/* x graphic origin                 */
		pix_d	sy_gorig;	/* y graphic origin                 */
		pix_d	sx_coffs;	/* x text cursor offset             */
		pix_d	sy_coffs;	/* y text cursor offset             */
		cur_d	srm_cur;	/* right margin                     */
		cur_d	sbm_cur;	/* bottom margin                    */
		cur_d	sumr_cur;	/* upper margin scrolling region    */
		cur_d	sbmr_cur;	/* bottom margin scrolling region   */
		dword	sstatus;	/* to save the GRAPHIC flag         */
		word	stermpara;	/* reverse and underscore flags     */
		sint	scurfnt;	/* current font                     */
	} pshc;

	byte	statflg;	/* status flags to write to mover status port
				   for this window                          */
	byte	chstatflg;	/* status flags to use when displaying
				   characters in a window (not in the border)
				   the complement flag is altered by the
				   character attribute escape sequence      */
	struct	w_tty	ttyd;	/* tty emulator structure                   */
	unsigned char	**savc;	/* pointer to the array containing pointers
				   to the different lines in the window text
				   save area                                */
	struct	mpstruc	*mppnt;	/* pointer to mouse pointer data            */
	long	alloccnt;	/* # of bytes allocated for the text save area
				   keyboard buffers, etc. for this window   */
	struct	rqque	nsrque;	/* queue for not served read requests       */
	struct	rqque	nswque;	/* queue for not served write requests      */
	long		cplen;	/* length of temporary text copy file       */
	long		cpseek;	/* current position in text copy file       */
	short		pgid;	/* negative process group id.               */
	sint		tsig;	/* signal to send when becomming top window */
	sint		ntsig;	/* signal to send when leaving top          */
	sint		rsig;	/* signal to send when window change        */
	sint		csig;	/* signal to send when close box            */
	sint		ksig;	/* signal to send when something to read
				   from the keyboard                        */
	word		ledflg;	/* flags indicating which of the LED's for
				   this window that are on, bits 1 to 8
				   corresponds to LED's 1 to 8 (bit 0 is not
				   used) 9 and 10 is the INS and ALT keys   */
	struct	finfo	fdata;	/* data about the current font              */
};

/*
 * Structure to hold information about one sub-rectangle in a window.
 */
struct	wpstruc {
	pix_d	xl_p;		/* left x coordinate (side), pixels  */
	pix_d	xr_p;		/* right x coordinate (side), pixels */
	pix_d	yu_p;		/* upper y coordinate (side), pixels */
	pix_d	yl_p;		/* lower y coordinate (side), pixels */
	pix_d	xlv_p;		/* left side excluding border        */
	pix_d	xrv_p;		/* right side excluding border       */
	pix_d	yuv_p;		/* upper side excluding border       */
	pix_d	ylv_p;		/* lower side excluding border       */
	word	bflags;		/* border included flags             */
};

/*
 * Structure used to hold the different icons set up.
 */
struct	icondat {
	pix_d		id_xorig;		/* upper left corner relative */
	pix_d		id_yorig;		/* virtual screen             */
	pix_d		id_xsize;		/* size of icon               */
	pix_d		id_ysize;
	word		id_flags;		/* flags                      */
	unsigned char	id_cmdseq[ICONSEQLEN];	/* sequence to be sent        */
	sint		id_len;			/* length of sequence         */
	sint		id_win;			/* window number              */
};

/*
 * Structure used to internally store the user boxes.
 */
struct	uboxst {
	word	ub_bmap[UBOXSIZE];	/* bitmap for box */
	short	ub_sig;			/* signal to send */
};

/*
 * Structure used when adding a header to a window.
 */
struct	headstruc {
	char	h_hdr[HDRSIZE];	/* window header string */
	word	h_flags;	/* window header flags  */
	union {
		long	h_xxx;	/* future use           */
	} h_pad;
};

/*
 * Structure used to change active window.
 */
struct	winlevel {
	sint	l_rstat;	/* return status */
	union {
		long	l_xxx;	/* future use    */
	} l_pad;
};

/*
 * Structure used when adding a new icon.
 */
struct	winicon	{
	pix_d	ip_xorig;		/* lower left corner of icon */
	pix_d	il_xorig;		/* dito, landscape mode      */
	pix_d	ip_yorig;
	pix_d	il_yorig;
	pix_d	ip_xsize;		/* size of icon              */
	pix_d	il_xsize;		/* dito, landscape mode      */
	pix_d	ip_ysize;
	pix_d	il_ysize;
	char	i_cmdseq[ICONSEQLEN];	/* sequence to be sent       */
	word	i_flags;		/* flags                     */
	sint	i_rstat;		/* return status             */
	union {
		long	i_xxx;		/* future use                */
	} i_pad;
};

/*
 * Structure used when altering the flags for a window.
 */
struct	flgstruc {
	uflags	f_flags;	/* the new flags */
	sint	f_rstat;	/* return status */
	union {
		long	f_xxx;	/* future use    */
	} f_pad;
};

/*
 * Structure used to set up mouse substitute keys.
 */
struct	substit {
	sint		c_initflg;	/* simu keys initially on/off */
	unsigned char	c_keys[SUBSTKEYS];	/* the keys           */
	unsigned char	c_step;		/* step when normal move      */
	unsigned char	c_lstep;	/* step when long move        */
	union {
		long	c_xxx;
	} c_pad;
};

/*
 * Structure used when altering the background pattern.
 */
struct	chbgstruc {
	word	cb_bitmap[BGPSIZE];
	union {
		long	cb_xxx;		/* future use */
	} cb_pad;
};

/*
 * Structure containing data about one rectangle visible in a window.
 */
struct	rectdes {
	pix_d	r_xorig;	/* x coordinate of lower left corner (relative
				   the origin of the virtual screen)        */
	pix_d	r_yorig;	/* y coordinate of lower left corner        */
	pix_d	r_xsize;	/* size of rectangle, x                     */
	pix_d	r_ysize;	/* size of rectangle, y                     */
};

/*
 * Parameter structure used when serving a request asking for the visible
 * area of a window.
 */
struct	visdes {
	short	v_nrect;		/* number of rectangles */
	sint	v_rstat;		/* return status        */
	union {
		long	v_xxx;		/* future use           */
	} v_pad;
};

/*
 * Structure used to change the mouse pointer.
 */
struct	npstruc {
	pix_d	np_xsize;		/* x size of mouse pointer      */
	pix_d	np_ysize;		/* y size                       */
	pix_d	np_xpnt;		/* the pointing part of the pnt */
	pix_d	np_ypnt;
	dword	np_and[MPSIZE];		/* and mask                     */
	dword	np_or[MPSIZE];		/* or mask                      */
	byte	np_flags;		/* mouse pointer flags          */
	sint	np_rstat;		/* return status                */
	union {
		long	np_xxx;		/* future use                   */
	} np_pad;
};

/*
 * Structure used to return the number of open windows and the number of
 * created windows.
 */
struct	nwstruc {
	short	nw_open;	/* number of opened windows  */
	short	nw_created;	/* number of created windows */
	union {
		long	nw_xxx;	/* future use                */
	} nw_pad;
};

/*
 * Structure used when reading the text contents of a virtual screen.
 */
struct	txtstruc {
	cur_d	tx_row;		/* first row         */
	cur_d	tx_col;		/* first column      */
	cur_d	tx_rcnt;	/* number of rows    */
	cur_d	tx_ccnt;	/* number of columns */
	sint	tx_rstat;	/* return status     */
	union {
		long	tx_xxx;	/* future use        */
	} tx_pad;
};

/*
 * Structure used to set up a new zoom list.
 */
struct	zoomlst {
	char	zp_list[ZOOMSIZE];	/* portrait mode list  */
	char	zl_list[ZOOMSIZE];	/* landscape mode list */
	byte	z_flags;		/* flags               */
	sint	z_rstat;		/* return status       */
	union {
		long	z_xxx;
	} z_pad;
};

/*
 * Structure used when change to a new default font.
 */
struct	dfltchr {
	char	dcp_font;	/* new default font, portrait mode        */
	char	dcl_font;	/* new default font, landscape mode       */
	cur_d	dcp_x;		/* x middle character position, portrait  */
	cur_d	dcl_x;		/* x middle character position, landscape */
	cur_d	dcp_y;		/* y middle character position, portrait  */
	cur_d	dcl_y;		/* y middle character position, landscape */
	byte	dc_flags;	/* flags                                  */
	sint	dc_rstat;	/* return status                          */
	union {
		long	dc_xxx;
	} dc_pad;
};

/*
 * Structure used to turn the screen or get the current screen mode.
 */
struct	modstruc {
	sint	m_mode;		/* screen mode   */
	sint	m_rstat;	/* return status */
	union {
		long	m_xxx;
	} m_pad;
};

/*
 * Structure containing initial terminal and tty driver parameters.
 */
struct	wininit {
	t_stop		td_tbstop[TSTOPSIZE];	/* tab stops             */
	word		td_term;		/* terminal flags        */
	struct	termio	td_driver;		/* tty driver parameters */
	union {
		long	td_xxx;			/* future use            */
	} td_pad;
};

/*
 * Structure used to read picture memory.
 */
struct	wpictblk {
	pix_d	p_xaddr;	/* x pixel of lower left corner */
	pix_d	p_yaddr;	/* y pixel of lower left corner */
	pix_d	p_width;	/* width of area in pixels      */
	pix_d	p_height;	/* height of area in pixels     */
	union {
		long	p_xxx;
	} p_pad;
};

/*
 * Structure used to set up a signal to be sent when there is something to
 * read from the keyboard buffer.
 */
struct	kysigst {
	sint	ks_sig;			/* signal to send */
	byte	ks_flags;		/* flags          */
	sint	ks_rstat;		/* return status  */
	union {
		long	ks_xxx;
	} ks_pad;
};

/*
 * Structure used to set up a new user box.
 */
struct	userbox {
	word	bx_bmap[UBOXSIZE];	/* bitmap for box             */
	short	bx_sig;			/* signal to be sent          */
	byte	bx_flags;		/* flags (not currently used) */
	sint	bx_rstat;		/* return status              */
	union {
		long	bx_xxx;
	} bx_pad;
};

/*
 * Structure used to set up a new help sequence.
 */

struct	helpst {
	char	hlp_seq[HLPSIZE];	/* help sequence              */
	word	hlp_flags;		/* flags (not currently used) */
	union {
		long	hlp_xxx;
	} hlp_pad;
};

/*
 * Structure containing data describing the current location, etc. of the
 * mouse pointer.
 */
struct	pntstruc {
	short		pd_onoff;	/* on/off flag                      */
	pix_d		pd_xmpos;	/* x mover coordinate               */
	pix_d		pd_ympos;	/* y mover coordinate               */
	pix_d		pd_xkpos;	/* x keyboard coordinate            */
	pix_d		pd_ykpos;	/* y keyboard coordinate            */
	pix_d		pd_x;		/* upper left corner, x coordinate  */
	pix_d		pd_y;		/* upper left corner, y coordinate  */
	pix_d		pd_xsize;	/* x size                           */
	pix_d		pd_ysize;	/* y size                           */
	pix_d		pd_xoffs;	/* offset from left side            */
	pix_d		pd_yoffs;	/* offset from upper side           */
	struct	mpstruc	*pd_mpp;	/* pointer to current mouse pointer */
	word		pd_flags;	/* different flags                  */
};

/*
 * Flags and data indicating an inverted icon.
 */
struct invsave {
	sint	inv_flg;	/* an icon inverted or not flag              */
	int	inv_wn;		/* window where inverted                     */
	pix_d	inv_left;	/* left side of inverted icon (mover coord.) */
	pix_d	inv_right;	/* right side of inverted icon               */
	pix_d	inv_up;		/* upper side of inverted icon               */
	pix_d	inv_low;	/* lower side of inverted icon               */
};

/*
 * Data to keep track of the text cursor.
 */
struct	cursave {
	short	cs_bcnt;	/* calls to offblnk() count   */
	short	cs_ccnt;	/* calls to cursoff() count   */
	sint	cs_curvis;	/* cursor visible or not flag */
};

/*
 * Structure containing data of one border type.
 */
struct	borddst {
	sint	bd_left;	/* width of left border                    */
	sint	bd_right;	/* width of right border                   */
	sint	bd_up;		/* width of upper border                   */
	sint	bd_down;	/* width of lower border                   */
	word	bd_flags;	/* flags indicating which sides or corners
				   of the border can hold boxes            */
};

/*
 * Structure containing data of one box in the border.
 */
struct	boxactd {
	word	ba_flags;	/* flag to be set if box is present */
	pix_d	ba_xsrc;	/* x coord. of source bitmap        */
	pix_d	ba_ysrc;	/* y coord. of source bitmap        */
	pix_d	ba_xsiz;	/* x size minus 1                   */
	pix_d	ba_ysiz;	/* y size minus 1                   */
	pix_d	ba_xoffs;	/* offset from border of border, x  */
	pix_d	ba_yoffs;	/* offset from border of border, y  */
	short	ba_act;		/* action code (when used)          */
};

/*
 * Structure containing hardware addresses.
 */
struct	hwastruc {
	dword	x_sizeport;	/* mover size port         */
	dword	y_sizeport;
	dword	x_toport;	/* mover to port           */
	dword	y_toport;
	dword	x_fromport;	/* mover from port         */
	dword	y_fromport;
	dword	mstatusport;	/* mover status port       */
	dword	mflagport;	/* mover flag port         */
	dword	pictmemst;	/* start of picture memory */
	dword	grphaddr;	/* word used for graphics  */
};

/*
 * Structure used when mapping in graphic and I/O memory.
 */
struct	mapseg {
	char	*lad;		/* start of segment mapped in */
	int	size;		/* size of segment            */
};

/*
 * Buffer used to copy data to and from the user's buffer.
 * Also used as temporary area.
 * NOTE: Be carefull so the use of the wpt[] array does not collide with the
 *   use of any of the other members.
 * NOTE 2: It is not absolutely sure that the size of wpt[] is big enough.
 *   Has to be investigated (it depends somewhat on the method used in
 *   wsplit()).
 */
union	buffer {
	char			cbuf[CBUFSIZE];
					/* to copy chars from and to user    */
	struct	winstruc	wbuf;	/* window create and alter structure */
	struct	winlevel	levbuf;	/* change level structure            */
	struct	headstruc	hbuf;	/* insert header structure           */
	struct	sgttyb		pbuf;	/* basic parameters structure        */
	struct	tchars		scbuf;	/* special characters structure      */
	struct	termio		tmiobuf;/* vIII tty parameter structure      */
	struct	winicon		icbuf;	/* to add a new icon                 */
	struct	flgstruc	fbuf;	/* to alter the flags for a window   */
	struct	chbgstruc	cbbuf;	/* to alter the background pattern   */
	struct	rectdes		rectbuf[4*MAXWINS];
					/* to report visible parts           */
	struct	npstruc		npbuf;	/* to alter mouse pointer            */
	struct	nwstruc		nwbuf;	/* to return # of open windows       */
	struct	fnkst		fnkbuf;	/* structure used to pass on load and
					   read function keys requests       */
	struct	wpictblk	pblbuf;	/* to read picture memory            */
	struct	txtstruc	txtbuf;	/* to copy text contents             */
	struct	modstruc	modbuf;	/* to turn the screen or get the
					   current screen mode               */
#ifdef LANDSCAPE
	struct	sprayst		sprbuf;	/* used to load spray mask           */
#endif LANDSCAPE
	struct	kysigst		ksbuf;	/* used to set up keboard signal     */
	struct	userbox		ubbuf;	/* to add a user defined box         */
	struct	helpst		hlpbuf;	/* to set up a new help sequence     */
	char			tcpbuf[TCPBUFSIZE];
					/* text copy buffer (by WINGETTXT)   */
	byte			bitmap[BITBUFSIZE];
					/* to copy the contents of the
					   picture memory to the user's
					   buffer                            */
	struct	wpstruc		wpt[RECTTMPMAX];
					/* temporary area used to construct the
					   sub-rectangles for a window. This
					   area is big enough to divide a
					   window at level MAXWINS. Therefore
					   this area can also be used to divide
					   the background into rectangles.   */
};
