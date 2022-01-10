/* w_wsh.h */

/*
 * This file contains constant definitions and structures used by the window
 * shell - wsh, and the window shell preprocessor - wshpp.
 */

/* 1984-02-14, Peter Andersson, Luxor Datorer AB */



#define		MAGIC		(short)0xffef	/* magic number of data file */



/*
 * Codes of different actions in an action list.
 */
#define		AL_END		0		/* end of list */
#define		AL_CLOSE	1		/* close window */
#define		AL_FLAGS	2		/* new window flags */
#define		AL_RESTORE	3		/* restore screen */
#define		AL_INVERSE	4		/* inverse video */
#define		AL_NORMAL	5		/* normal video */
#define		AL_LOGOUT	6		/* log out */
#define		AL_TERM		7		/* open terminal window */
#define		AL_MENU		8		/* next menu */
#define		AL_TOP		9		/* move window to top level */
#define		AL_TURN		10		/* turn the screen */
#define		AL_SUBST	11		/* alter mouse substitute keys */
#define		AL_BG		12		/* alter background pattern */
#define		AL_STRING	13		/* string */
#define		AL_PSTRING	14		/* portrait mode string */
#define		AL_LSTRING	15		/* landscape mode string */
#define		AL_CHOICE	16		/* a choice */
#define		AL_ICON		17		/* an icon */
#define		AL_PNT		18		/* a mouse pointer */
#define		AL_ENV		19		/* environment variables */
#define		AL_PENV		20		/* portrait environment variables */
#define		AL_LENV		21		/* landscape environment variables */



/*
 * Structures used to hold a linked list of window data.
 */

struct	windat {
	short		 wd_no;		/* number */
	struct	windat	 *wd_next;	/* pointer to next window data */
	short		 wd_fd;		/* file descriptor for the window */
	short		 wd_menu;	/* the number of the menu */
	struct	winstruc wd_win;	/* data to create the window */
};



/*
 * Structures used to hold a linked list of window header data.
 */

struct	headdat {
	short		  hd_no;	/* number */
	struct	headdat	  *hd_next;	/* pointer to next header data */
	struct	headstruc hd_head;	/* header data */
};



/*
 * Structures used to hold a linked list of icon data.
 */

struct	icondat {
	short		ic_no;		/* number */
	struct	icondat	*ic_next;	/* pointer to next icon data */
	struct	winicon	ic_icon;	/* icon data */
};



/*
 * Structures used to hold a linked list of flags data.
 */

struct	flagdat {
	short		 fl_no;		/* number */
	struct	flagdat	 *fl_next;	/* pointer to next flags data */
	struct	flgstruc fl_flags;	/* flags data */
};



/*
 * Structure used to hold a linked list of zoom data.
 */

struct	zoomdat {
	short		zm_no;		/* number */
	struct	zoomdat	*zm_next;	/* pointer to next zoom data */
	struct	zoomlst	zm_zoom;	/* zoom list data */
};



/*
 * Structures used to hold a linked list of mouse pointer data.
 */

struct	mpdat {
	short		mp_no;		/* number */
	struct	mpdat	*mp_next;	/* pointer to next pointer data */
	struct	npstruc	mp_mp;		/* mouse pointer data */
};



/*
 * Structures used to hold a linked list of mouse substitute keys.
 */

struct	subdat {
	short		sk_no;		/* number */
	struct	subdat	*sk_next;	/* pointer to next in list */
	struct	substit	sk_subst;	/* mouse substitute keys data */
};



/*
 * Structures used to hold a linked list of background pattern data.
 */

struct	bgdat {
	short		  bg_no;	/* number */
	struct	bgdat	  *bg_next;	/* pointer to next in list */
	struct	chbgstruc bg_bgd;	/* background data */
};



/*
 * Structure to hold a linked list of initial driver and terminal parameters.
 * This is not used at the moment (maybee in the future).
 */

struct	winidat {
	short		id_no;		/* number */
	struct	winidat	*id_next;	/* pointer to next in list */
	struct	wininit	id_inid;	/* initial data structure */
};



/*
 * Structure used to hold a linked list of strings or file pathnames.
 */

struct	strdat {
	short		st_no;		/* number */
	struct	strdat	*st_next;	/* pointer to next string data */
	char		*st_str;	/* pointer to the string */
};



/*
 * Structures used to hold a linked list of commands or environments.
 */

struct	arrdat {
	short		ca_no;		/* number */
	struct	arrdat	*ca_next;	/* pointer to next array data */
	char		**ca_args;	/* pointer to arguments */
};



/*
 * An action list consists of an array of this structure.
 */

struct	actlist {
	short	al_code;	/* the code of the action */
	short	al_no;		/* the number (if needed) */
};



/*
 * Structures to hold a linked list with menus.
 */

struct	menudat {
	short		mu_no;		/* number */
	struct	menudat	*mu_next;	/* pointer to next menu */
	short		mu_win;		/* the number of the window */
	short		mu_hd;		/* the number of the header */
	short		mu_pnt;		/* the number of the mouse pointer */
	short		mu_act;		/* action list */
	struct	actlist	*mu_list;	/* list of strings and choices */
};



/*
 * Structures to hold a linked list of choices.
 */

struct	chodat {
	short		co_no;		/* number */
	struct	chodat	*co_next;	/* pointer to next choice */
	short		co_icon;	/* the number of the icon */
	short		co_action;	/* the number of the action */
};



/*
 * Structures to hold a linked list of actions.
 */

struct	actdat {
	short		ac_no;		/* number */
	struct	actdat	*ac_next;	/* pointer to next action */
	struct	actlist	*ac_alist;	/* pointer to action list */
};



/*
 * Structures used to hold a linked list of terminal data.
 */

struct	termdat {
	short		td_no;		/* number */
	struct	termdat	*td_next;	/* pointer to next terminal data */
	short		td_win;		/* the number of the window */
	short		td_head;	/* the number of the header */
	short		td_zoom;	/* the number of the zoom list */
	short		td_mp;		/* the number of the mouse pointer */
	struct	actlist	*td_list;	/* list of strings and icons */
	short		td_dir;		/* the number of the pathname */
	short		td_flags;	/* flags */
	short		td_cmd;		/* the number of the command */
};



/*
 * Flags in the termdat structure.
 */

#define		TD_SUPER	(1 << 0)	/* super channel open */
#define		TD_WAIT		(1 << 1)	/* wait for process to finish */



/*
 * File header structure for the file describing how the window shell shall
 * act. This structure must be at the start of the file.
 */

struct	wshhead {
	short		wh_magic;	/* magic number = 0xffef */
	long		wh_size;	/* size of the file */
	struct	windat	*wh_wd;		/* start of list of windows */
	struct	headdat	*wh_hd;		/* start of list of headers */
	struct	icondat	*wh_ic;		/* start of list of icons */
	struct	flagdat	*wh_flg;	/* start of list of flags */
	struct	zoomdat	*wh_zoom;	/* start of list of zoom lists */
	struct	mpdat	*wh_mp;		/* start of list of mouse pointers */
	struct	subdat	*wh_sub;	/* list of mouse substitute keys */
	struct	bgdat	*wh_bg;		/* list of background patterns */
	struct	winidat	*wh_id;		/* list of initial driver and term. para */
	struct	strdat	*wh_str;	/* start of list of string data */
	struct	strdat	*wh_pstr;	/* start of portrait strings */
	struct	strdat	*wh_lstr;	/* start of landscape strings */
	struct	strdat	*wh_dir;	/* start of list of pathname data */
	struct	arrdat	*wh_env;	/* start of list of environment */
	struct	arrdat	*wh_penv;	/* portrait mode environment list */
	struct	arrdat	*wh_lenv;	/* landscape mode environment list */
	struct	arrdat	*wh_cmd;	/* start of list of command data */
	struct	menudat	*wh_menu;	/* start of list of menu data */
	struct	chodat	*wh_cho;	/* start of list of choices */
	struct	actdat	*wh_act;	/* start of list of actions */
	struct	termdat	*wh_term;	/* start of list of terminal data */
	char		*wh_pad1;	/* reserved for future use */
	char		*wh_pad2;
	char		*wh_pad3;
	char		*wh_pad4;
	char		*wh_pad5;
	struct	actlist	*wh_ini;	/* actions on initialization */
};
