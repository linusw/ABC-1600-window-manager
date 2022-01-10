/* w_structs.h */

/*
 * This file contains structures used to communicate with the ABC1600
 * window handler.
 */

/* 1985-07-23, Luxor Datorer AB */



#define		W_STRUCTS

#ifndef		W_CONST
#include	<win/w_const.h>
#endif
#ifndef		W_TYPES
#include	<win/w_types.h>
#endif



/*
 * The structure used for opening a window, receiving the window status, etc.
 */

struct	winstruc
{
	pix_d	wp_xorig;	/* x coordinate of lower left corner of the
				   viritual screen, pixels                  */
	pix_d	wl_xorig;	/* dito, landscape mode                     */
	pix_d	wp_yorig;	/* y coordinate                             */
	pix_d	wl_yorig;	/* dito, landscape mode                     */
	pix_d	wp_xsize;	/* width of the viritual screen, pixels     */
	pix_d	wl_xsize;	/* dito, landscape mode                     */
	pix_d	wp_ysize;	/* height of the viritual screen, pixels    */
	pix_d	wl_ysize;	/* dito, landscape mode                     */
	pix_d	wp_vxorig;	/* x coordinate of lower left corner of the
				   window (not including the border)        */
	pix_d	wl_vxorig;	/* dito, landscape mode                     */
	pix_d	wp_vyorig;	/* y coordinate                             */
	pix_d	wl_vyorig;	/* dito, landscape mode                     */
	pix_d	wp_vxsize;	/* width of the window (not including the
				   border)                                  */
	pix_d	wl_vxsize;	/* dito, landscape mode                     */
	pix_d	wp_vysize;	/* hight of the window                      */
	pix_d	wl_vysize;	/* dito, landscape mode                     */
	short	w_color;	/* background colour                        */
	sint	w_border;	/* border type                              */
	char	wp_font;	/* initial font number, portrait mode       */
	char	wl_font;	/* dito, landscape mode                     */
	sint	w_curfont;	/* number of currently used font            */
	sint	w_level;	/* window level (0 - MAXWINS-1)             */
	sint	w_uboxes;	/* max. number of user defined boxes        */
	cur_d	w_xcur;		/* normal cursor position (greater than
				   zero), x coordinate                      */
	cur_d	w_ycur;		/* normal cursor position (greater than
				   zero), y coordinate                      */
	pix_d	w_xgcur;	/* graphic cursor position, x coordinate    */
	pix_d	w_ygcur;	/* graphic cursor position, y coordinate    */
	sint	w_tsig;		/* signal to send when becomming top window */
	sint	w_ntsig;	/* signal to send when leaving top          */
	sint	w_rsig;		/* signal to send when window change        */
	sint	w_csig;		/* signal to send when close box            */
	word	w_boxes;	/* boxes in the border                      */
	uflags	w_flags;	/* flags for the window                     */
	sint	w_rstat;	/* return status                            */
	union
	{
		long	w_xxx;	/* future use                               */
	} w_pad;
};



/*
 * Structure used when adding a header to a window.
 */

struct	headstruc
{
	char	h_hdr[HDRSIZE];		/* window header string */
	word	h_flags;		/* header flags         */
	union
	{
		long	h_xxx;		/* future use           */
	} h_pad;
};



/*
 * Structure used to change active window.
 */

struct	winlevel
{
	sint	l_rstat;	/* return status */
	union
	{
		long	l_xxx;	/* future use    */
	} l_pad;
};



/*
 * Structure used when adding a new icon.
 */

struct	winicon
{
	pix_d	ip_xorig;		/* lower left corner of icon */
	pix_d	il_xorig;		/* dito, landscape mode      */
	pix_d	ip_yorig;		/* relative virtual screen   */
	pix_d	il_yorig;		/* dito, landscape mode      */
	pix_d	ip_xsize;		/* size of icon              */
	pix_d	il_xsize;		/* dito, landscape mode      */
	pix_d	ip_ysize;
	pix_d	il_ysize;
	char	i_cmdseq[ICONSEQLEN];	/* sequence to be sent       */
	word	i_flags;		/* flags                     */
	sint	i_rstat;		/* return status             */
	union
	{
		long	i_xxx;		/* future use                */
	} i_pad;
};



/*
 * Structure used when altering the flags for a window.
 */

struct	flgstruc
{
	uflags	f_flags;	/* the new flags */
	sint	f_rstat;	/* return status */
	union
	{
		long	f_xxx;	/* future use    */
	} f_pad;
};



/*
 * Structure used to set up mouse substitute keys.
 */

struct	substit
{
	sint		c_initflg;	/* enabled on set up flag */
	unsigned char	c_keys[SUBSTKEYS];	/* the key codes  */
	unsigned char	c_step;		/* step when normal move  */
	unsigned char	c_lstep;	/* step when long move    */
	union
	{
		long	c_xxx;
	} c_pad;
};



/*
 * Structure used to alter the background pattern.
 */

struct	chbgstruc
{
	word	cb_bitmap[BGPSIZE];
	union
	{
		long	cb_xxx;		/* future use */
	} cb_pad;
};



/*
 * Structure containing data about one rectangle in a window.
 */

struct	rectdes
{
	pix_d	r_xorig;	/* x coordinate of lower left corner (relative
				   the origin of the virtual screen)        */
	pix_d	r_yorig;	/* y coordinate of the lower left corner    */
	pix_d	r_xsize;	/* size of rectangle, x                     */
	pix_d	r_ysize;	/* size of rectangle, y                     */
};



/*
 * Parameter structure used to get the visible parts of a window or the
 * background.
 */

struct	visdes
{
	short		v_nrect;		/* number of rectangles */
	sint		v_rstat;		/* return status        */
	union
	{
		long	v_xxx;			/* future use           */
	} v_pad;
};



/*
 * Structure used to alter the mouse pointer.
 */

struct	npstruc
{
	pix_d	np_xsize;	/* horizontal size of mouse pointer   */
	pix_d	np_ysize;	/* vertical size of mouse pointer     */
	pix_d	np_xpnt;	/* "pointer" coordinate of pointer, x */
	pix_d	np_ypnt;	/* "pointer" coordinate of pointer, y */
	dword	np_and[MPSIZE];	/* and mask                           */
	dword	np_or[MPSIZE];	/* or mask                            */
	byte	np_flags;	/* mouse pointer flags                */
	sint	np_rstat;	/* return status                      */
	union
	{
		long	np_xxx;	/* future use                         */
	} np_pad;
};



/*
 * Structure used to return the number of open windows and the number of
 * created windows.
 */

struct	nwstruc
{
	short	nw_open;	/* number of opened windows  */
	short	nw_created;	/* number of created windows */
	union
	{
		long	nw_xxx;	/* future use                */
	} nw_pad;
};



/*
 * Structure used when reading the text contents of a virtual screen.
 */

struct	txtstruc
{
	cur_d	tx_row;		/* first row         */
	cur_d	tx_col;		/* first column      */
	cur_d	tx_rcnt;	/* number of rows    */
	cur_d	tx_ccnt;	/* number of columns */
	sint	tx_rstat;	/* return status     */
	union
	{
		long	tx_xxx;	/* future use        */
	} tx_pad;
};



/*
 * Structure used to set up a new zoom list.
 */

struct zoomlst
{
	char	zp_list[ZOOMSIZE];	/* portrait mode list  */
	char	zl_list[ZOOMSIZE];	/* landscape mode list */
	byte	z_flags;		/* flags               */
	sint	z_rstat;		/* return status       */
	union
	{
		long	z_xxx;
	} z_pad;
};



/*
 * Structure used to change to a new default font.
 */

struct	dfltchr
{
	char	dcp_font;	/* new default font, portrait mode        */
	char	dcl_font;	/* new default font, landscape mode       */
	cur_d	dcp_x;		/* x middle character position, portrait  */
	cur_d	dcl_x;		/* x middle character position, landscape */
	cur_d	dcp_y;		/* y middle character position, portrait  */
	cur_d	dcl_y;		/* y middle character position, landscape */
	byte	dc_flags;	/* flags                                  */
	sint	dc_rstat;	/* return status                          */
	union
	{
		long	dc_xxx;
	} dc_pad;
};



/*
 * Structure used to turn the screen or get the current screen mode.
 */

struct	modstruc
{
	sint	m_mode;		/* the current screen mode */
	sint	m_rstat;	/* return status           */
	union
	{
		long	m_xxx;
	} m_pad;
};



/*
 * Structure used to set up and read the initial terminal and tty driver
 * parameters.
 */

struct	wininit
{
	t_stop		td_tbstop[TSTOPSIZE];	/* tab stops             */
	word		td_term;		/* terminal flags        */
	struct					/* same as struct termio */
	{
		unsigned short	c_iflag;
		unsigned short	c_oflag;
		unsigned short	c_cflag;
		unsigned short	c_lflag;
		char		c_line;
		unsigned char	c_ccs[8];
	} td_driver;				/* tty driver parameters */
	union
	{
		long	td_xxx;			/* future use            */
	} td_pad;
};



/*
 * Structure used to read the picture memory for a window or the whole screen.
 */

struct	wpictblk
{
	pix_d	p_xaddr;	/* from x pixel address     */
	pix_d	p_yaddr;	/* from y pixel address     */
	pix_d	p_width;	/* width of area in pixels  */
	pix_d	p_height;	/* height of area in pixels */
	union
	{
		long	p_xxx;
	} p_pad;
};



/*
 * Structure used to set up a new spray mask.
 */

struct	sprayst
{
	dword	sp_mask[8*sizeof(dword)];
};



/*
 * Structure to set up a signal to be sent when there is something to read
 * from the keyboard buffer.
 */

struct	kysigst
{
	sint	ks_sig;		/* signal to send             */
	byte	ks_flags;	/* flags (not currently used) */
	sint	ks_rstat;	/* return status              */
	union
	{
		long	ks_xxx;
	} ks_pad;
};



/*
 * Structure used to set up a new user defined box.
 */

struct	userbox
{
	word	bx_bmap[UBOXSIZE];	/* bitmap for box             */
	short	bx_sig;			/* signal to be sent          */
	byte	bx_flags;		/* flags (not currently used) */
	sint	bx_rstat;		/* return status              */
	union
	{
		long	bx_xxx;
	} bx_pad;
};



/*
 * Structure used to set up a new help sequence.
 */

struct	helpst
{
	char	hlp_seq[HLPSIZE];	/* help sequence              */
	word	hlp_flags;		/* flags (not currently used) */
	union
	{
		long	hlp_xxx;
	} hlp_pad;
};
