/* w_language.h */

/*
 * This file contains definitions of strings used in a language to describe
 * windows, etc.
 */

/* 1985-02-04, Peter Andersson, Luxor Datorer AB */



/*
 * Keywords used to describe a window (the winstruc structure).
 */

#define		WP_XORIG	"pxor"		/* wp_xorig */
#define		WL_XORIG	"lxor"		/* wl_xorig */
#define		WP_YORIG	"pyor"		/* wp_yorig */
#define		WL_YORIG	"lyor"		/* wl_yorig */
#define		WP_XSIZE	"pxsi"		/* wp_xsize */
#define		WL_XSIZE	"lxsi"		/* wl_xsize */
#define		WP_YSIZE	"pysi"		/* wp_ysize */
#define		WL_YSIZE	"lysi"		/* wl_ysize */
#define		WP_VXORIG	"pvxo"		/* wp_vxorig */
#define		WL_VXORIG	"lvxo"		/* wl_vxorig */
#define		WP_VYORIG	"pvyo"		/* wp_vyorig */
#define		WL_VYORIG	"lvyo"		/* wl_vyorig */
#define		WP_VXSIZE	"pvxs"		/* wp_vxsize */
#define		WL_VXSIZE	"lvxs"		/* wl_vxsize */
#define		WP_VYSIZE	"pvys"		/* wp_vysize */
#define		WL_VYSIZE	"lvys"		/* wl_vysize */
#define		W_COLOR		"colr"		/* w_color */
#define		W_BORDER	"bord"		/* w_border */
#define		WP_FONT		"pfnt"		/* wp_font */
#define		WL_FONT		"lfnt"		/* wl_font */
#define		W_UBOXES	"usrb"		/* w_uboxes */
#define		W_TSIG		"tsig"		/* w_tsig */
#define		W_NTSIG		"nsig"		/* w_ntsig */
#define		W_RSIG		"rsig"		/* w_rsig */
#define		W_CSIG		"csig"		/* w_csig */



/*
 * Flags in the w_boxes member of the winstruc structure.
 */

#define		WB_HSCR		"hscr"		/* BX_HSCR */
#define		WB_VSCR		"vscr"		/* BX_VSCR */
#define		WB_CBOX		"cbox"		/* BX_CLOS */
#define		WB_SBOX		"sbox"		/* BX_SIZE */
#define		WB_MBOX		"mbox"		/* BX_MOVE */
#define		WB_ZBOX		"zbox"		/* BX_ZOOM */
#define		WB_AVIS		"avis"		/* BX_AVIS */
#define		WB_BBOX		"bbox"		/* BX_BLOW */
#define		WB_HBOX		"hbox"		/* BX_HELP */



/*
 * Flags in the w_flags member of the winstruc structure.
 */

#define		WF_PMODE	"pmod"		/* PMODE */
#define		WF_LMODE	"lmod"		/* LMODE */
#define		WF_SAVTXT	"stxt"		/* SAVETEXT */
#define		WF_SAVBM	"sbmp"		/* SAVEBITMAP */
#define		WF_LOCK		"lock"		/* LOCK */
#define		WF_NOOVER	"novr"		/* NOOVER */
#define		WF_NOCUR	"ncur"		/* NOCURSOR */
#define		WF_NOMOVE	"nmov"		/* NOMOVE */
#define		WF_ALLSCR	"alls"		/* ALLSCR */
#define		WF_SPECIAL	"spec"		/* SPECIAL */
#define		WF_KSCR		"kscr"		/* KEYSCROLL */
#define		WF_WSCR		"wscr"		/* WRITSCROLL */
#define		WF_ALTMPNT	"amsp"		/* ALTMPNT */
#define		WF_REL		"rltv"		/* RELATIVE */
#define		WF_NCPIN	"ncpi"		/* NOCPIN */
#define		WF_NCPOUT	"ncpo"		/* NOCPOUT */
#define		WF_TEXT		"text"		/* TXTSIZE */
#define		WF_GROUP	"wgrp"		/* WGROUP */
#define		WF_RULC		"rulc"		/* REL_ULC */
#define		WF_RURC		"rurc"		/* REL_URC */
#define		WF_RLLC		"rllc"		/* REL_LLC */
#define		WF_RLRC		"rlrc"		/* REL_LRC */



/*
 * Keywords used to describe a window header (the headstruc structure).
 */

#define		H_HDR		"head"		/* h_hdr */



/*
 * Flags in the h_flags member of the headstruc structure.
 */

#define		HF_INVH		"invh"		/* H_INVHD */
#define		HF_INVT		"invt"		/* H_INVTOP */



/*
 * Keywords used to describe an icon (the winicon structure).
 */

#define		IP_XORIG	"pxor"		/* ip_xorig */
#define		IL_XORIG	"lxor"		/* il_xorig */
#define		IP_YORIG	"pyor"		/* ip_yorig */
#define		IL_YORIG	"lyor"		/* il_yorig */
#define		IP_XSIZE	"pxsi"		/* ip_xsize */
#define		IL_XSIZE	"lxsi"		/* il_xsize */
#define		IP_YSIZE	"pysi"		/* ip_ysize */
#define		IL_YSIZE	"lysi"		/* il_ysize */
#define		I_CMDSEQ	"cseq"		/* i_cmdseq */


/*
 * Flags in the i_flags member of the winicon structure.
 */

#define		IF_PMODE	"pmod"		/* I_PMODE */
#define		IF_LMODE	"lmod"		/* I_LMODE */
#define		IF_PRESS	"pres"		/* I_PRESS */
#define		IF_RLSE		"rlse"		/* I_RELEASE */
#define		IF_INV		"inve"		/* I_INVERT */
#define		IF_ENTER	"entr"		/* I_ENTER */
#define		IF_LEAVE	"leav"		/* I_LEAVE */
#define		IF_REMOV	"rmov"		/* I_REMOVE */
#define		IF_RQST		"rqst"		/* I_RQST */
#define		IF_SETCHK	"schk"		/* I_SETCHK */
#define		IF_LZERO	"lzer"		/* I_LZERO */
#define		IF_TEXT		"text"		/* I_TEXT */



/*
 * Flags in the f_flags member of the flgstruc structure.
 */

#define		FF_LOCK		WF_LOCK		/* LOCK */
#define		FF_NOOVER	WF_NOOVER	/* NOOVER */
#define		FF_NOCUR	WF_NOCUR	/* NOCURSOR */
#define		FF_NOMOVE	WF_NOMOVE	/* NOMOVE */
#define		FF_ALLSCR	WF_ALLSCR	/* ALLSCR */
#define		FF_KSCR		WF_KSCR		/* KEYSCROLL */
#define		FF_WSCR		WF_WSCR		/* WRITSCROLL */
#define		FF_NCPIN	WF_NCPIN	/* NOCPIN */
#define		FF_NCPOUT	WF_NCPOUT	/* NOCPOUT */
#define		FF_RULC		WF_RULC		/* REL_ULC */
#define		FF_RURC		WF_RURC		/* REL_URC */
#define		FF_RLLC		WF_RLLC		/* REL_LLC */
#define		FF_RLRC		WF_RLRC		/* REL_LRC */



/*
 * Keywords used to describe a mouse pointer (the npstruc structure).
 */

#define		NP_XSIZE	"xsiz"		/* np_xsize */
#define		NP_YSIZE	"ysiz"		/* np_ysize */
#define		NP_XPNT		"xpnt"		/* np_xpnt */
#define		NP_YPNT		"ypnt"		/* np_ypnt */
#define		NP_AND		"andm"		/* np_and[] */
#define		NP_OR		"orma"		/* np_or[] */



/*
 * Keywords used to describe the keys used to simulate the mouse
 * (the msimu structure).
 */

#define		C_INITFLG	"init"		/* c_initflg */
#define		C_ONOFF		"onof"		/* c_onoff */
#define		C_MPU		"mpup"		/* c_mpu */
#define		C_MPD		"mpdo"		/* c_mpd */
#define		C_MPL		"mple"		/* c_mpl */
#define		C_MPR		"mpri"		/* c_mpr */
#define		C_MPUL		"mpul"		/* c_mpul */
#define		C_MPUR		"mpur"		/* c_mpur */
#define		C_MPDL		"mpdl"		/* c_mpdl */
#define		C_MPDR		"mpdr"		/* c_mpdr */
#define		C_LMPU		"lpup"		/* c_lmpu */
#define		C_LMPD		"lpdo"		/* c_lmpd */
#define		C_LMPL		"lple"		/* c_lmpl */
#define		C_LMPR		"lpri"		/* c_lmpr */
#define		C_LMPUL		"lpul"		/* c_lmpul */
#define		C_LMPUR		"lpur"		/* c_lmpur */
#define		C_LMPDL		"lpdl"		/* c_lmpdl */
#define		C_LMPDR		"lpdr"		/* c_lmpdr */
#define		C_PCMD		"pcmd"		/* c_pcmd */
#define		C_CHWIN		"cwin"		/* c_chwin */
#define		C_MCA		"mtxt"		/* c_mca */
#define		C_STEP		"step"		/* c_step */
#define		C_LSTEP		"lstp"		/* c_lstep */



/*
 * Keywords used to describe a new background pattern (the chbgstruc
 * structure).
 */

#define		CB_BITMAP	"bmap"		/* cb_bitmap */



/*
 * Keywords to describe a zoom list.
 */

#define		ZP_LIST		"plst"		/* zp_list[] */
#define		ZL_LIST		"llst"		/* zl_list[] */



/*
 * Flags in the z_flags member of the zoomlst structure.
 */

#define		ZF_PMODE	"pmod"		/* Z_PMODE */
#define		ZF_LMODE	"lmod"		/* Z_LMODE */



/*
 * Names of different main data items.
 */

#define		D_WINDOW	"window"	/* window data */
#define		D_ICON		"icon"		/* icon data */
#define		D_STRING	"string"	/* string */
#define		D_PSTRING	"pstring"	/* portrait mode string */
#define		D_LSTRING	"lstring"	/* landscape mode string */
#define		D_POINTER	"pointer"	/* mouse pointer data */
#define		D_HEADER	"header"	/* window header data */
#define		D_DIR		"directory"	/* directory pathname */
#define		D_CMD		"command"	/* command, file and args */
#define		D_FLAGS		"flags"		/* window flags */
#define		D_SUBST		"substitute"	/* mouse substitute keys */
#define		D_BGROUND	"background"	/* background data */
#define		D_INIT		"init"		/* on init */
#define		D_MENU		"menu"		/* menu window description */
#define		D_CHOICE	"choice"	/* icon and action */
#define		D_ACTION	"action"	/* action */
#define		D_TERM		"terminal"	/* description of a terminal */
#define		D_ZOOM		"zoomlist"	/* description of a zoom list */
#define		D_ENVIRON	"environ"	/* environment variables */
#define		D_PENVIRON	"penviron"	/* portrait environment variables */
#define		D_LENVIRON	"lenviron"	/* landscape environment variables */



/*
 * Names of different special actions.
 */

#define		A_CLOSE		"close"		/* close menu window */
#define		A_RESTORE	"restore"	/* restore screen */
#define		A_INVERSE	"inverse"	/* inverse video */
#define		A_NORMAL	"normal"	/* normal video */
#define		A_LOGOUT	"logout"	/* log out */
#define		A_SUPER		"super"		/* super channel */
#define		A_TOP		"top"		/* move to top level */
#define		A_TURN		"turn"		/* turn the screen */
#define		A_WAIT		"wait"		/* wait for process to finish */
