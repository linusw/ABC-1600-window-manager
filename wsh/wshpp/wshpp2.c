/* wshpp2.c */

/*
 * This file contains the code to decode the data for the different items in
 * the window shell preprocessor - wshpp.
 */

/* 1985-02-21, Peter Andersson, Luxor Datorer AB */


/*
 * Functions in this file:
 *	windecod()
 *	icondecod()
 *	headdecod()
 *	flgdecod()
 *	zoomdecod()
 *	subdecod()
 *	strdecod()
 *	pstrdecod()
 *	lstrdecod()
 *	dirdecod()
 *	envdecod()
 *	penvdecod()
 *	lenvdecod()
 *	cmddecod()
 *	pntdecod()
 *	bgdecod()
 *	chodecod()
 *	initdecod()
 *	actdecod()
 *	menudecod()
 *	termdecod()
 */


#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	"../h/w_language.h"
#include	"../h/w_wsh.h"
#include	"wshpp_defs.h"
#include	"wshpp_glbl.h"


/*
 * Global variables.
 */
extern	struct	globals	glbl;



/*
 * Decode data for a window (the windat structure).
 */

windecod()
{
	char			kwbuf[KW_LEN];	/* buffer to hold keyword */
	register	char	*cp;
	register	uflags	flag;		/* window flags */
	register	word	bflag;		/* border boxes flags */
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	windat	*wdp;		/* pointer to window data */
	char			*allocmem();

	glblp = &glbl;
	if (glblp->numflag == NO || glblp->inumber <= 0) {

		/*
		 * Illegal or missing window number.
		 */
		dsperr(M_IMWN);
		skipeoln();
		return;
	}

	/*
	 * Allocate memory for the windat structure.
	 */
	wdp = (struct windat *)allocmem(sizeof(struct windat));
	if (glblp->wdstart == NULL) {
		glblp->wdstart = wdp;
	}
	else {
		glblp->wdcur->wd_next = wdp;
	}
	glblp->wdcur = wdp;
	wdp->wd_no = glblp->inumber;
	flag = 0;
	bflag = 0;
	cp = &kwbuf[0];
	glblp->tmperr = NO;

	/*
	 * Now process the keywords one by one.
	 */
	while (glblp->tmperr == NO && kwcollect(cp) == OK) {
		if (kwcmp(cp, WP_XORIG) == HIT) {
			wdp->wd_win.wp_xorig = getval();
		}
		else if (kwcmp(cp, WL_XORIG) == HIT) {
			wdp->wd_win.wl_xorig = getval();
		}
		else if (kwcmp(cp, WP_YORIG) == HIT) {
			wdp->wd_win.wp_yorig = getval();
		}
		else if (kwcmp(cp, WL_YORIG) == HIT) {
			wdp->wd_win.wl_yorig = getval();
		}
		else if (kwcmp(cp, WP_XSIZE) == HIT) {
			wdp->wd_win.wp_xsize = getval();
		}
		else if (kwcmp(cp, WL_XSIZE) == HIT) {
			wdp->wd_win.wl_xsize = getval();
		}
		else if (kwcmp(cp, WP_YSIZE) == HIT) {
			wdp->wd_win.wp_ysize = getval();
		}
		else if (kwcmp(cp, WL_YSIZE) == HIT) {
			wdp->wd_win.wl_ysize = getval();
		}
		else if (kwcmp(cp, WP_VXORIG) == HIT) {
			wdp->wd_win.wp_vxorig = getval();
		}
		else if (kwcmp(cp, WL_VXORIG) == HIT) {
			wdp->wd_win.wl_vxorig = getval();
		}
		else if (kwcmp(cp, WP_VYORIG) == HIT) {
			wdp->wd_win.wp_vyorig = getval();
		}
		else if (kwcmp(cp, WL_VYORIG) == HIT) {
			wdp->wd_win.wl_vyorig = getval();
		}
		else if (kwcmp(cp, WP_VXSIZE) == HIT) {
			wdp->wd_win.wp_vxsize = getval();
		}
		else if (kwcmp(cp, WL_VXSIZE) == HIT) {
			wdp->wd_win.wl_vxsize = getval();
		}
		else if (kwcmp(cp, WP_VYSIZE) == HIT) {
			wdp->wd_win.wp_vysize = getval();
		}
		else if (kwcmp(cp, WL_VYSIZE) == HIT) {
			wdp->wd_win.wl_vysize = getval();
		}
		else if (kwcmp(cp, W_COLOR) == HIT) {
			wdp->wd_win.w_color = getval();
		}
		else if (kwcmp(cp, W_BORDER) == HIT) {
			wdp->wd_win.w_border = getval();
		}
		else if (kwcmp(cp, WP_FONT) == HIT) {
			wdp->wd_win.wp_font = getval();
		}
		else if (kwcmp(cp, WL_FONT) == HIT) {
			wdp->wd_win.wl_font = getval();
		}
		else if (kwcmp(cp, W_UBOXES) == HIT) {
			wdp->wd_win.w_uboxes = getval();
			bflag |= BX_USER;
		}
		else if (kwcmp(cp, W_TSIG) == HIT) {
			wdp->wd_win.w_tsig = getval();
		}
		else if (kwcmp(cp, W_NTSIG) == HIT) {
			wdp->wd_win.w_ntsig = getval();
		}
		else if (kwcmp(cp, W_RSIG) == HIT) {
			wdp->wd_win.w_rsig = getval();
		}
		else if (kwcmp(cp, W_CSIG) == HIT) {
			wdp->wd_win.w_csig = getval();
		}
		else if (kwcmp(cp, WB_CBOX) == HIT) {
			bflag |= BX_CLOS;
		}
		else if (kwcmp(cp, WB_SBOX) == HIT) {
			bflag |= BX_SIZE;
		}
		else if (kwcmp(cp, WB_MBOX) == HIT) {
			bflag |= BX_MOVE;
		}
		else if (kwcmp(cp, WB_ZBOX) == HIT) {
			bflag |= BX_ZOOM;
		}
		else if (kwcmp(cp, WB_HSCR) == HIT) {
			bflag |= BX_HSCR;
		}
		else if (kwcmp(cp, WB_VSCR) == HIT) {
			bflag |= BX_VSCR;
		}
		else if (kwcmp(cp, WB_AVIS) == HIT) {
			bflag |= BX_AVIS;
		}
		else if (kwcmp(cp, WB_BBOX) == HIT) {
			bflag |= BX_BLOW;
		}
		else if (kwcmp(cp, WB_HBOX) == HIT) {
			bflag |= BX_HELP;
		}
		else if (kwcmp(cp, WF_PMODE) == HIT) {
			flag |= PMODE;
		}
		else if (kwcmp(cp, WF_LMODE) == HIT) {
			flag |= LMODE;
		}
		else if (kwcmp(cp, WF_SAVTXT) == HIT) {
			flag |= SAVETEXT;
		}
		else if (kwcmp(cp, WF_SAVBM) == HIT) {
			flag |= SAVEBITMAP;
		}
		else if (kwcmp(cp, WF_LOCK) == HIT) {
			flag |= LOCK;
		}
		else if (kwcmp(cp, WF_NOOVER) == HIT) {
			flag |= NOOVER;
		}
		else if (kwcmp(cp, WF_NOCUR) == HIT) {
			flag |= NOCURSOR;
		}
		else if (kwcmp(cp, WF_NOMOVE) == HIT) {
			flag |= NOMOVE;
		}
		else if (kwcmp(cp, WF_ALLSCR) == HIT) {
			flag |= ALLSCR;
		}
		else if (kwcmp(cp, WF_SPECIAL) == HIT) {
			flag |= SPECIAL;
		}
		else if (kwcmp(cp, WF_KSCR) == HIT) {
			flag |= KEYSCROLL;
		}
		else if (kwcmp(cp, WF_WSCR) == HIT) {
			flag |= WRITSCROLL;
		}
		else if (kwcmp(cp, WF_ALTMPNT) == HIT) {
			flag |= ALTMPNT;
		}
		else if (kwcmp(cp, WF_REL) == HIT) {
			flag |= RELATIVE;
		}
		else if (kwcmp(cp, WF_NCPIN) == HIT) {
			flag |= NOCPIN;
		}
		else if (kwcmp(cp, WF_NCPOUT) == HIT) {
			flag |= NOCPOUT;
		}
		else if (kwcmp(cp, WF_TEXT) == HIT) {
			flag |= TXTSIZE;
		}
		else if (kwcmp(cp, WF_GROUP) == HIT) {
			flag |= WGROUP;
		}
		else if (kwcmp(cp, WF_RULC) == HIT) {
			flag |= REL_ULC;
		}
		else if (kwcmp(cp, WF_RURC) == HIT) {
			flag |= REL_URC;
		}
		else if (kwcmp(cp, WF_RLLC) == HIT) {
			flag |= REL_LLC;
		}
		else if (kwcmp(cp, WF_RLRC) == HIT) {
			flag |= REL_LRC;
		}
		else {
			glblp->tmperr = YES;	/* else error */
		}
	}

	wdp->wd_win.w_flags = flag;		/* save the flags */
	wdp->wd_win.w_boxes = bflag;

	/*
	 * Check if it is ok.
	 */
	ckifok();

	return;
}	/* end of windecod() */



/*
 * Decode data for an icon (the icondat structure).
 */

icondecod()
{
	char			kwbuf[KW_LEN];	/* buffer to hold keyword */
	register	char	*cp;
	register	word	flag;		/* icon flags */
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	icondat	*idp;		/* pointer to icon data */
	char			*allocmem();

	glblp = &glbl;
	if (glblp->numflag == NO || glblp->inumber <= 0) {

		/*
		 * Illegal or missing icon number.
		 */
		dsperr(M_IMIN);
		skipeoln();
		return;
	}

	/*
	 * Allocate memory for the icondat structure.
	 */
	idp = (struct icondat *)allocmem(sizeof(struct icondat));
	if (glblp->icstart == NULL) {
		glblp->icstart = idp;
	}
	else {
		glblp->iccur->ic_next = idp;
	}
	glblp->iccur = idp;
	idp->ic_no = glblp->inumber;	/* the icon number */

	flag = 0;
	cp = &kwbuf[0];
	glblp->tmperr = NO;

	/*
	 * Now process the keywords one by one.
	 */
	while (glblp->tmperr == NO && kwcollect(cp) == OK) {
		if (kwcmp(cp, IP_XORIG) == HIT) {
			idp->ic_icon.ip_xorig = getval();
		}
		else if (kwcmp(cp, IL_XORIG) == HIT) {
			idp->ic_icon.il_xorig = getval();
		}
		else if (kwcmp(cp, IP_YORIG) == HIT) {
			idp->ic_icon.ip_yorig = getval();
		}
		else if (kwcmp(cp, IL_YORIG) == HIT) {
			idp->ic_icon.il_yorig = getval();
		}
		else if (kwcmp(cp, IP_XSIZE) == HIT) {
			idp->ic_icon.ip_xsize = getval();
		}
		else if (kwcmp(cp, IL_XSIZE) == HIT) {
			idp->ic_icon.il_xsize = getval();
		}
		else if (kwcmp(cp, IP_YSIZE) == HIT) {
			idp->ic_icon.ip_ysize = getval();
		}
		else if (kwcmp(cp, IL_YSIZE) == HIT) {
			idp->ic_icon.il_ysize = getval();
		}
		else if (kwcmp(cp, I_CMDSEQ) == HIT) {
			getlstr(&idp->ic_icon.i_cmdseq[0], ICONSEQLEN);
		}
		else if (kwcmp(cp, IF_PMODE) == HIT) {
			flag |= I_PMODE;
		}
		else if (kwcmp(cp, IF_LMODE) == HIT) {
			flag |= I_LMODE;
		}
		else if (kwcmp(cp, IF_PRESS) == HIT) {
			flag |= I_PRESS;
		}
		else if (kwcmp(cp, IF_RLSE) == HIT) {
			flag |= I_RELEASE;
		}
		else if (kwcmp(cp, IF_INV) == HIT) {
			flag |= I_INVERT;
		}
		else if (kwcmp(cp, IF_ENTER) == HIT) {
			flag |= I_ENTER;
		}
		else if (kwcmp(cp, IF_LEAVE) == HIT) {
			flag |= I_LEAVE;
		}
		else if (kwcmp(cp, IF_REMOV) == HIT) {
			flag |= I_REMOVE;
		}
		else if (kwcmp(cp, IF_RQST) == HIT) {
			flag |= I_RQST;
		}
		else if (kwcmp(cp, IF_SETCHK) == HIT) {
			flag |= I_SETCHK;
		}
		else if (kwcmp(cp, IF_LZERO) == HIT) {
			flag |= I_LZERO;
		}
		else if (kwcmp(cp, IF_TEXT) == HIT) {
			flag |= I_TEXT;
		}
		else {
			glblp->tmperr = YES;	/* error */
		}
	}
	idp->ic_icon.i_flags = flag;		/* save the flags */

	/*
	 * Check if it is ok.
	 */
	ckifok();

	return;
}	/* end of icondecod() */



/*
 * Decode data for a header (the headdat structure).
 */

headdecod()
{
	char			kwbuf[KW_LEN];	/* buffer to hold keyword */
	register	char	*cp;
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	headdat	*hdp;		/* pointer to header data */
	char			*allocmem();

	glblp = &glbl;
	if (glblp->numflag == NO || glblp->inumber <= 0) {

		/*
		 * Illegal or missing header number.
		 */
		dsperr(M_IMHN);
		skipeoln();
		return;
	}

	/*
	 * Allocate memory for the headdat structure.
	 */
	hdp = (struct headdat *)allocmem(sizeof(struct headdat));
	if (glblp->hdstart == NULL) {
		glblp->hdstart = hdp;
	}
	else {
		glblp->hdcur->hd_next = hdp;
	}
	glblp->hdcur = hdp;
	hdp->hd_no = glblp->inumber;
	cp = &kwbuf[0];
	glblp->tmperr = NO;

	/*
	 * Now process the keywords one by one.
	 */
	while (glblp->tmperr == NO && kwcollect(cp) == OK) {
		if (kwcmp(cp, H_HDR) == HIT) {
			getlstr(&hdp->hd_head.h_hdr[0], HDRSIZE);
		}
		else if (kwcmp(cp, HF_INVH) == HIT) {
			hdp->hd_head.h_flags |= H_INVHD;
		}
		else if (kwcmp(cp, HF_INVT) == HIT) {
			hdp->hd_head.h_flags |= H_INVTOP;
		}
		else {
			glblp->tmperr = YES;		/* error */
		}
	}

	/*
	 * Check if it is ok.
	 */
	ckifok();

	return;
}	/* end of headdecod() */



/*
 * Decode data for new flags (the flagdat structure).
 */

flgdecod()
{
	char			kwbuf[KW_LEN];	/* buffer to hold keyword */
	register	char	*cp;
	register	uflags	flag;		/* new window flags */
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	flagdat	*fdp;		/* pointer to flags data */
	char			*allocmem();

	glblp = &glbl;
	if (glblp->numflag == NO || glblp->inumber <= 0) {

		/*
		 * Illegal or missing flags number.
		 */
		dsperr(M_IMFN);
		skipeoln();
		return;
	}

	/*
	 * Allocate memory for the flagdat structure.
	 */
	fdp = (struct flagdat *)allocmem(sizeof(struct flagdat));
	if (glblp->flgstart == NULL) {
		glblp->flgstart = fdp;
	}
	else {
		glblp->flgcur->fl_next = fdp;
	}
	glblp->flgcur = fdp;
	fdp->fl_no = glblp->inumber;		/* the flags number */
	flag = 0;
	cp = &kwbuf[0];
	glblp->tmperr = NO;

	/*
	 * Now process the keywords one by one.
	 */
	while (glblp->tmperr == NO && kwcollect(cp) == OK) {
		if (kwcmp(cp, FF_LOCK) == HIT) {
			flag |= LOCK;
		}
		else if (kwcmp(cp, FF_NOOVER) == HIT) {
			flag |= NOOVER;
		}
		else if (kwcmp(cp, FF_NOCUR) == HIT) {
			flag |= NOCURSOR;
		}
		else if (kwcmp(cp, FF_NOMOVE) == HIT) {
			flag |= NOMOVE;
		}
		else if (kwcmp(cp, FF_ALLSCR) == HIT) {
			flag |= ALLSCR;
		}
		else if (kwcmp(cp, FF_KSCR) == HIT) {
			flag |= KEYSCROLL;
		}
		else if (kwcmp(cp, FF_WSCR) == HIT) {
			flag |= WRITSCROLL;
		}
		else if (kwcmp(cp, FF_NCPIN) == HIT) {
			flag |= NOCPIN;
		}
		else if (kwcmp(cp, FF_NCPOUT) == HIT) {
			flag |= NOCPOUT;
		}
		else if (kwcmp(cp, FF_RULC) == HIT) {
			flag |= REL_ULC;
		}
		else if (kwcmp(cp, FF_RURC) == HIT) {
			flag |= REL_URC;
		}
		else if (kwcmp(cp, FF_RLLC) == HIT) {
			flag |= REL_LLC;
		}
		else if (kwcmp(cp, FF_RLRC) == HIT) {
			flag |= REL_LRC;
		}
		else {
			glblp->tmperr = YES;
		}
	}

	fdp->fl_flags.f_flags = flag;		/* save the flags */

	/*
	 * Check if it is ok.
	 */
	ckifok();

	return;
}	/* end of flgdecod() */



/*
 * Decode data for a zoom list.
 */

zoomdecod()
{
	char			kwbuf[KW_LEN];	/* buffer to hold keyword */
	register	char	*cp;
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	zoomdat	*zdp;		/* pointer to zoom list data */
	char			*allocmem();

	glblp = &glbl;
	if (glblp->numflag == NO || glblp->inumber <= 0) {

		/*
		 * Illegal or missing zoomlist number.
		 */
		dsperr(M_IMZN);
		skipeoln();
		return;
	}

	/*
	 * Allocate memory for the zoomdat structure.
	 */
	zdp = (struct zoomdat *)allocmem(sizeof(struct zoomdat));
	if (glblp->zoomstart == NULL) {
		glblp->zoomstart = zdp;
	}
	else {
		glblp->zoomcur->zm_next = zdp;
	}
	glblp->zoomcur = zdp;
	zdp->zm_no = glblp->inumber;
	cp = &kwbuf[0];
	glblp->tmperr = NO;

	/*
	 * Now process the keywords one by one.
	 */
	while (glblp->tmperr == NO && kwcollect(cp) == OK) {
		if (kwcmp(cp, ZP_LIST) == HIT) {
			getlstr(&zdp->zm_zoom.zp_list[0], ZOOMSIZE);
		}
		else if (kwcmp(cp, ZL_LIST) == HIT) {
			getlstr(&zdp->zm_zoom.zl_list[0], ZOOMSIZE);
		}
		else if (kwcmp(cp, ZF_PMODE) == HIT) {
			zdp->zm_zoom.z_flags |= Z_PMODE;
		}
		else if (kwcmp(cp, ZF_LMODE) == HIT) {
			zdp->zm_zoom.z_flags |= Z_LMODE;
		}
		else {
			glblp->tmperr = YES;		/* error */
		}
	}

	/*
	 * Check if it is ok.
	 */
	ckifok();

	return;
}	/* end of zoomdecod() */



/*
 * Decode data for the mouse substitute keys (the msimu structure).
 */

subdecod()
{
	char			kwbuf[KW_LEN];	/* buffer to hold keywords */
	register	char	*cp;
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	subdat	*sdp;		/* pointer to key data */
	char			*allocmem();

	glblp = &glbl;
	if (glblp->numflag == NO || glblp->inumber <= 0) {

		/*
		 * Illegal or missing number.
		 */
		dsperr(M_IMSKN);
		skipeoln();
		return;
	}

	/*
	 * Allocate memory for the substitute keys.
	 */
	sdp = (struct subdat *)allocmem(sizeof(struct subdat));
	if (glblp->substart == NULL) {
		glblp->substart = sdp;
	}
	else {
		glblp->subcur->sk_next = sdp;
	}
	glblp->subcur = sdp;
	sdp->sk_no = glblp->inumber;
	cp = &kwbuf[0];
	glblp->tmperr = NO;

	/*
	 * Now process the keywords one by one.
	 */
	while (glblp->tmperr == NO && kwcollect(cp) == OK) {
		if (kwcmp(cp, C_INITFLG) == HIT) {
			sdp->sk_subst.c_initflg = getval();
		}
		else if (kwcmp(cp, C_ONOFF) == HIT) {
			sdp->sk_subst.c_keys[S_ONOFF] = getval();
		}
		else if (kwcmp(cp, C_MPU) == HIT) {
			sdp->sk_subst.c_keys[S_MPU] = getval();
		}
		else if (kwcmp(cp, C_MPD) == HIT) {
			sdp->sk_subst.c_keys[S_MPD] = getval();
		}
		else if (kwcmp(cp, C_MPL) == HIT) {
			sdp->sk_subst.c_keys[S_MPL] = getval();
		}
		else if (kwcmp(cp, C_MPR) == HIT) {
			sdp->sk_subst.c_keys[S_MPR] = getval();
		}
		else if (kwcmp(cp, C_MPUL) == HIT) {
			sdp->sk_subst.c_keys[S_MPUL] = getval();
		}
		else if (kwcmp(cp, C_MPUR) == HIT) {
			sdp->sk_subst.c_keys[S_MPUR] = getval();
		}
		else if (kwcmp(cp, C_MPDL) == HIT) {
			sdp->sk_subst.c_keys[S_MPDL] = getval();
		}
		else if (kwcmp(cp, C_MPDR) == HIT) {
			sdp->sk_subst.c_keys[S_MPDR] = getval();
		}
		else if (kwcmp(cp, C_LMPU) == HIT) {
			sdp->sk_subst.c_keys[S_LMPU] = getval();
		}
		else if (kwcmp(cp, C_LMPD) == HIT) {
			sdp->sk_subst.c_keys[S_LMPD] = getval();
		}
		else if (kwcmp(cp, C_LMPL) == HIT) {
			sdp->sk_subst.c_keys[S_LMPL] = getval();
		}
		else if (kwcmp(cp, C_LMPR) == HIT) {
			sdp->sk_subst.c_keys[S_LMPR] = getval();
		}
		else if (kwcmp(cp, C_LMPUL) == HIT) {
			sdp->sk_subst.c_keys[S_LMPUL] = getval();
		}
		else if (kwcmp(cp, C_LMPUR) == HIT) {
			sdp->sk_subst.c_keys[S_LMPUR] = getval();
		}
		else if (kwcmp(cp, C_LMPDL) == HIT) {
			sdp->sk_subst.c_keys[S_LMPDL] = getval();
		}
		else if (kwcmp(cp, C_LMPDR) == HIT) {
			sdp->sk_subst.c_keys[S_LMPDR] = getval();
		}
		else if (kwcmp(cp, C_PCMD) == HIT) {
			sdp->sk_subst.c_keys[S_PCMD] = getval();
		}
		else if (kwcmp(cp, C_CHWIN) == HIT) {
			sdp->sk_subst.c_keys[S_CHWIN] = getval();
		}
		else if (kwcmp(cp, C_MCA) == HIT) {
			sdp->sk_subst.c_keys[S_MCA] = getval();
		}
		else if (kwcmp(cp, C_STEP) == HIT) {
			sdp->sk_subst.c_step = getval();
		}
		else if (kwcmp(cp, C_LSTEP) == HIT) {
			sdp->sk_subst.c_lstep = getval();
		}
		else {
			glblp->tmperr = YES;		/* error */
		}
	}

	/*
	 * Check if it is ok.
	 */
	ckifok();

	return;
}	/* end of subdecod() */



/*
 * Decode a string.
 */

strdecod()
{
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	strdat	*sdp;		/* pointer to string data */
	char			*allocmem();
	char			*getstr();

	glblp = &glbl;
	if (glblp->numflag == NO || glblp->inumber <= 0) {

		/*
		 * Illegal or missing string number.
		 */
		dsperr(M_IMSN);
		skipeoln();
		return;
	}

	/*
	 * Allocate memory for the string data structure.
	 */
	sdp = (struct strdat *)allocmem(sizeof(struct strdat));
	if (glblp->strstart == NULL) {
		glblp->strstart = sdp;
	}
	else {
		glblp->strcur->st_next = sdp;
	}
	glblp->strcur = sdp;
	sdp->st_no = glblp->inumber;		/* the string number */
	glblp->tmperr = NO;
	sdp->st_str = getstr();

	/*
	 * Check if it is ok.
	 */
	ckifok();

	return;
}	/* end of strdecod() */



/*
 * Decode a portrait mode string.
 */

pstrdecod()
{
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	strdat	*sdp;		/* pointer to string data */
	char			*allocmem();
	char			*getstr();

	glblp = &glbl;
	if (glblp->numflag == NO || glblp->inumber <= 0) {
		dsperr(M_IMPSN);
		skipeoln();
		return;
	}

	/*
	 * Allocate memory for the string data structure.
	 */
	sdp = (struct strdat *)allocmem(sizeof(struct strdat));
	if (glblp->pstrstart == NULL) {
		glblp->pstrstart = sdp;
	}
	else {
		glblp->pstrcur->st_next = sdp;
	}
	glblp->pstrcur = sdp;
	sdp->st_no = glblp->inumber;
	glblp->tmperr = NO;
	sdp->st_str = getstr();

	/*
	 * Check if everything is ok.
	 */
	ckifok();

	return;
}	/* end of pstrdecod() */



/*
 * Decode a landscape mode string.
 */

lstrdecod()
{
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	strdat	*sdp;		/* pointer to string data */
	char			*allocmem();
	char			*getstr();

	glblp = &glbl;
	if (glblp->numflag == NO || glblp->inumber <= 0) {
		dsperr(M_IMLSN);
		skipeoln();
		return;
	}

	/*
	 * Allocate memory for the string data structure.
	 */
	sdp = (struct strdat *)allocmem(sizeof(struct strdat));
	if (glblp->lstrstart == NULL) {
		glblp->lstrstart = sdp;
	}
	else {
		glblp->lstrcur->st_next = sdp;
	}
	glblp->lstrcur = sdp;
	sdp->st_no = glblp->inumber;
	glblp->tmperr = NO;
	sdp->st_str = getstr();

	/*
	 * Check if everything is ok.
	 */
	ckifok();

	return;
}	/* end of lstrdecod() */



/*
 * Decode data for a pathname.
 */

dirdecod()
{
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	strdat	*ddp;		/* pointer to pathname data structure */
	char			*allocmem();
	char			*getstr();

	glblp = &glbl;
	if (glblp->numflag == NO || glblp->inumber <= 0) {
		dsperr(M_IMDN);
		skipeoln();
		return;
	}

	/*
	 * Allocate memory for the pathname data structure.
	 */
	ddp = (struct strdat *)allocmem(sizeof(struct strdat));
	if (glblp->dirstart == NULL) {
		glblp->dirstart = ddp;
	}
	else {
		glblp->dircur->st_next = ddp;
	}
	glblp->dircur = ddp;
	ddp->st_no = glblp->inumber;		/* the directory number */
	glblp->tmperr = NO;
	ddp->st_str = getstr();

	/*
	 * Check if it is ok.
	 */
	ckifok();

	return;
}	/* end of dirdecod() */



/*
 * Decode a list of environment variables.
 */

envdecod()
{
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	arrdat	*envp;		/* pointer to environment data */
	char			*allocmem();
	char			**getmstr();

	glblp = &glbl;
	if (glblp->numflag == NO || glblp->inumber <= 0) {
		dsperr(M_IMENVN);
		skipeoln();
		return;
	}

	/*
	 * Allocate memory for the environment data structure.
	 */
	envp = (struct arrdat *)allocmem(sizeof(struct arrdat));
	if (glblp->envstart == NULL) {
		glblp->envstart = envp;
	}
	else {
		glblp->envcur->ca_next = envp;
	}
	glblp->envcur = envp;
	envp->ca_no = glblp->inumber;
	glblp->tmperr = NO;
	envp->ca_args = getmstr();

	/*
	 * Check if ok.
	 */
	ckifok();

	return;
}	/* end of envdecod() */



/*
 * Decode a list of portrait mode environment variables.
 */

penvdecod()
{
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	arrdat	*envp;		/* pointer to environment data */
	char			*allocmem();
	char			**getmstr();

	glblp = &glbl;
	if (glblp->numflag == NO || glblp->inumber <= 0) {
		dsperr(M_IMPENVN);
		skipeoln();
		return;
	}

	/*
	 * Allocate memory for the environment data structure.
	 */
	envp = (struct arrdat *)allocmem(sizeof(struct arrdat));
	if (glblp->penvstart == NULL) {
		glblp->penvstart = envp;
	}
	else {
		glblp->penvcur->ca_next = envp;
	}
	glblp->penvcur = envp;
	envp->ca_no = glblp->inumber;
	glblp->tmperr = NO;
	envp->ca_args = getmstr();

	/*
	 * Check if ok.
	 */
	ckifok();

	return;
}	/* end of penvdecod() */



/*
 * Decode a list of landscape mode environment variables.
 */

lenvdecod()
{
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	arrdat	*envp;		/* pointer to environment data */
	char			*allocmem();
	char			**getmstr();

	glblp = &glbl;
	if (glblp->numflag == NO || glblp->inumber <= 0) {
		dsperr(M_IMLENVN);
		skipeoln();
		return;
	}

	/*
	 * Allocate memory for the environment data structure.
	 */
	envp = (struct arrdat *)allocmem(sizeof(struct arrdat));
	if (glblp->lenvstart == NULL) {
		glblp->lenvstart = envp;
	}
	else {
		glblp->lenvcur->ca_next = envp;
	}
	glblp->lenvcur = envp;
	envp->ca_no = glblp->inumber;
	glblp->tmperr = NO;
	envp->ca_args = getmstr();

	/*
	 * Check if ok.
	 */
	ckifok();

	return;
}	/* end of lenvdecod() */



/*
 * Decode a command.
 */

cmddecod()
{
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	arrdat	*cdp;		/* pointer to command data */
	char			*allocmem();
	char			**getmstr();

	glblp = &glbl;
	if (glblp->numflag == NO || glblp->inumber <= 0) {
		dsperr(M_IMCN);
		skipeoln();
		return;
	}

	/*
	 * Allocate memory for the command data structure.
	 */
	cdp = (struct arrdat *)allocmem(sizeof(struct arrdat));
	if (glblp->cmdstart == NULL) {
		glblp->cmdstart = cdp;
	}
	else {
		glblp->cmdcur->ca_next = cdp;
	}
	glblp->cmdcur = cdp;
	cdp->ca_no = glblp->inumber;		/* the command number */
	glblp->tmperr = NO;
	cdp->ca_args = getmstr();

	/*
	 * Check if ok.
	 */
	ckifok();

	return;
}	/* end of cmddecod() */



/*
 * Decode mouse pointer data.
 */

pntdecod()
{
	char			kwbuf[KW_LEN];	/* buffer to hold keyword */
	register	char	*cp;
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	mpdat	*mdp;		/* pointer to mouse pointer data */
	char			*allocmem();
	long			getval();

	glblp = &glbl;
	if (glblp->numflag == NO || glblp->inumber <= 0) {
		dsperr(M_IMPN);
		skipeoln();
		return;
	}

	/*
	 * Allocate memory for the mpdat structure.
	 */
	mdp = (struct mpdat *)allocmem(sizeof(struct mpdat));
	if (glblp->mpstart == NULL) {
		glblp->mpstart = mdp;
	}
	else {
		glblp->mpcur->mp_next = mdp;
	}
	glblp->mpcur = mdp;
	mdp->mp_no = glblp->inumber;		/* the mouse pointer number */
	cp = &kwbuf[0];
	glblp->tmperr = NO;

	/*
	 * Now process the keywords one by one.
	 */
	while (glblp->tmperr == NO && kwcollect(cp) == OK) {
		if (kwcmp(cp, NP_XSIZE) == HIT) {
			mdp->mp_mp.np_xsize = getval();
		}
		else if (kwcmp(cp, NP_YSIZE) == HIT) {
			mdp->mp_mp.np_ysize = getval();
		}
		else if (kwcmp(cp, NP_XPNT) == HIT) {
			mdp->mp_mp.np_xpnt = getval();
		}
		else if (kwcmp(cp, NP_YPNT) == HIT) {
			mdp->mp_mp.np_ypnt = getval();
		}
		else if (kwcmp(cp, NP_AND) == HIT) {
			getmdword(&mdp->mp_mp.np_and[0], MPSIZE);
		}
		else if (kwcmp(cp, NP_OR) == HIT) {
			getmdword(&mdp->mp_mp.np_or[0], MPSIZE);
		}
		else {
			glblp->tmperr = YES;	/* error */
		}
	}

	/*
	 * Check if everything is ok.
	 */
	ckifok();

	return;
}	/* end of pntdecod() */



/*
 * Decode background pattern data.
 */

bgdecod()
{
	char			kwbuf[KW_LEN];	/* buffer to hold keyword */
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	bgdat	*bdp;		/* pointer to background data */
	char			*allocmem();

	glblp = &glbl;
	if (glblp->numflag == NO || glblp->inumber <= 0) {
		dsperr(M_IMBGN);
		skipeoln();
		return;
	}

	/*
	 * Allocate memory for the background pattern.
	 */
	bdp = (struct bgdat *)allocmem(sizeof(struct bgdat));
	if (glblp->bgstart == NULL) {
		glblp->bgstart = bdp;
	}
	else {
		glblp->bgcur->bg_next = bdp;
	}
	glblp->bgcur = bdp;
	bdp->bg_no = glblp->inumber;
	glblp->tmperr = NO;

	/*
	 * Now process the keywords one by one.
	 */
	while (glblp->tmperr == NO && kwcollect(&kwbuf[0]) == OK) {
		if (kwcmp(&kwbuf[0], CB_BITMAP) == HIT) {
			getmword(&bdp->bg_bgd.cb_bitmap[0], BGPSIZE);
		}
		else {
			glblp->tmperr = YES;	/* error */
		}
	}

	/*
	 * Check if everything went ok.
	 */
	ckifok();

	return;
}	/* end of bgdecod() */



/*
 * Decode choice data.
 */

chodecod()
{
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	chodat	*cdp;		/* pointer to choice data */
	register		act;		/* action code */
	char			*allocmem();

	glblp = &glbl;
	if (glblp->numflag == NO || glblp->inumber <= 0) {
		dsperr(M_IMCHN);
		goto error;
	}

	/*
	 * Allocate memory for the chodat structure.
	 */
	cdp = (struct chodat *)allocmem(sizeof(struct chodat));
	if (glblp->chostart == NULL) {
		glblp->chostart = cdp;
	}
	else {
		glblp->chocur->co_next = cdp;
	}
	glblp->chocur = cdp;
	cdp->co_no = glblp->inumber;		/* the choice number */

	/*
	 * Now process the choice data.
	 */
	while ((act = nextact()) != EOLN && act != EOF) {
		switch (act) {

		case ICON_TOK:			/* icon */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMIN);
				goto error;
			}
			if (cdp->co_icon != 0) {
				dsperr(M_MULIC);
				goto error;
			}
			cdp->co_icon = glblp->inumber;
			break;

		case ACT_TOK:			/* action */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMAN);
				goto error;
			}
			if (cdp->co_action != 0) {
				dsperr(M_MULAC);
				goto error;
			}
			cdp->co_action = glblp->inumber;
			break;

		default:			/* error */
			illsyntax();
			goto error;
		}
	}

	return;

error:
	skipeoln();
	return;
}	/* end of chodecod() */



/*
 * Decode initialization data.
 */

initdecod()
{
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	actlist	*alp;		/* pointer to action list */
	register		act;		/* action code */
	register		code;		/* code in action list */
	char			*allocmem();

	glblp = &glbl;
	if (glblp->numflag != NO) {
		dsperr(M_INNA);
		goto error;
	}
	if (glblp->init != NULL) {
		dsperr(M_MID);
		goto error;
	}
	glblp->init = (struct actlist *)allocmem(0);

	/*
	 * Now process the init data.
	 */
	while ((act = nextact()) != EOLN && act != EOF) {
		switch (act) {

		case INV_TOK:		/* inverse video */
			if (glblp->numflag != NO) {
				dsperr(M_INVNA);
				goto error;
			}
			code = AL_INVERSE;
			break;

		case NORM_TOK:		/* normal video */
			if (glblp->numflag != NO) {
				dsperr(M_NRMNA);
				goto error;
			}
			code = AL_NORMAL;
			break;

		case SUBST_TOK:		/* mouse substitute keys */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMSKN);
				goto error;
			}
			code = AL_SUBST;
			break;

		case PNT_TOK:		/* mouse pointer */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMPN);
				goto error;
			}
			code = AL_PNT;
			break;

		case BG_TOK:		/* background pattern */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMBGN);
				goto error;
			}
			code = AL_BG;
			break;

		case TERM_TOK:		/* terminal window */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMTERM);
				goto error;
			}
			code = AL_TERM;
			break;

		case MENU_TOK:		/* menu */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMMN);
				goto error;
			}
			code = AL_MENU;
			break;

		default:		/* error */
			illsyntax();
			goto error;
		}

		/*
		 * Allocate a new element in the action list.
		 */
		alp = (struct actlist *)allocmem(sizeof(struct actlist));
		alp->al_code = code;
		alp->al_no = glblp->inumber;
	}

	/*
	 * Allocate the terminating element in the action list.
	 */
	alp = (struct actlist *)allocmem(sizeof(struct actlist));
	/* alp->al_code = AL_END; */

	return;

error:
	skipeoln();
	return;
}	/* end of initdecod() */



/*
 * Decode an action list.
 */

actdecod()
{
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	actdat	*adp;		/* pointer to action data */
	register struct	actlist	*ldp;		/* pointer to action list */
	register		act;		/* action code */
	register		code;		/* code used in action list */
	char			*allocmem();

	glblp = &glbl;
	if (glblp->numflag == NO || glblp->inumber <= 0) {
		dsperr(M_IMACTN);
		goto error;
	}

	/*
	 * Allocate memory for the actdat structure.
	 */
	adp = (struct actdat *)allocmem(sizeof(struct actdat));
	if (glblp->actstart == NULL) {
		glblp->actstart = adp;
	}
	else {
		glblp->actcur->ac_next = adp;
	}
	glblp->actcur = adp;
	adp->ac_no = glblp->inumber;		/* action number */
	adp->ac_alist = (struct actlist *)allocmem(0);

	/*
	 * Now process the action data.
	 */
	while ((act = nextact()) != EOLN && act != EOF) {
		switch (act) {

		case CLOS_TOK:		/* close window */
			if (glblp->numflag != NO) {
				dsperr(M_CNNA);
				goto error;
			}
			code = AL_CLOSE;
			break;

		case FLG_TOK:		/* window flags */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMFN);
				goto error;
			}
			code = AL_FLAGS;
			break;

		case REST_TOK:		/* restore screen */
			if (glblp->numflag != NO) {
				dsperr(M_RNNA);
				goto error;
			}
			code = AL_RESTORE;
			break;

		case INV_TOK:		/* inverse video */
			if (glblp->numflag != NO) {
				dsperr(M_INVNA);
				goto error;
			}
			code = AL_INVERSE;
			break;

		case NORM_TOK:		/* normal video */
			if (glblp->numflag != NO) {
				dsperr(M_NRMNA);
				goto error;
			}
			code = AL_NORMAL;
			break;

		case TOP_TOK:		/* move to top level */
			if (glblp->numflag != NO) {
				dsperr(M_TOPNA);
				goto error;
			}
			code = AL_TOP;
			break;

		case TURN_TOK:		/* turn the screen */
			if (glblp->numflag != NO) {
				dsperr(M_TURNNA);
				goto error;
			}
			code = AL_TURN;
			break;

		case SUBST_TOK:
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMSKN);
				goto error;
			}
			code = AL_SUBST;
			break;

		case PNT_TOK:		/* set up new mouse pointer */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMPN);
				goto error;
			}
			code = AL_PNT;
			break;

		case BG_TOK:		/* alter background pattern */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMBGN);
				goto error;
			}
			code = AL_BG;
			break;

		case LOUT_TOK:		/* log out */
			if (glblp->numflag != NO) {
				dsperr(M_LOUTNA);
				goto error;
			}
			code = AL_LOGOUT;
			break;

		case TERM_TOK:		/* terminal window */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMTERM);
				goto error;
			}
			code = AL_TERM;
			break;

		case MENU_TOK:
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMMN);
				goto error;
			}
			code = AL_MENU;
			break;

		default:		/* error */
			illsyntax();
			goto error;
		}

		/*
		 * Allocate a new element in the action list.
		 */
		ldp = (struct actlist *)allocmem(sizeof(struct actlist));
		ldp->al_code = code;
		ldp->al_no = glblp->inumber;
	}

	/*
	 * Allocate the terminating element in the action list.
	 */
	ldp = (struct actlist *)allocmem(sizeof(struct actlist));
	/* ldp->al_code = AL_END; */

	return;

error:
	skipeoln();
	return;
}	/* end of actdecod() */



/*
 * Decode menu data.
 */

menudecod()
{
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	menudat	*mdp;		/* pointer to menu data */
	register struct	actlist	*ldp;		/* list of strings and choices */
	register		act;		/* action code */
	char			*allocmem();

	glblp = &glbl;
	if (glblp->numflag == NO || glblp->inumber <= 0) {
		dsperr(M_IMMN);
		goto error;
	}

	/*
	 * Allocate memory for the menudat structure.
	 */
	mdp = (struct menudat *)allocmem(sizeof(struct menudat));
	if (glblp->menustart == NULL) {
		glblp->menustart = mdp;
	}
	else {
		glblp->menucur->mu_next = mdp;
	}
	glblp->menucur = mdp;
	mdp->mu_no = glblp->inumber;		/* the menu number */
	mdp->mu_list = (struct actlist *)allocmem(0);

	/*
	 * Now process the menu data.
	 */
	while ((act = nextact()) != EOLN && act != EOF) {
		switch (act) {

		case WIN_TOK:		/* window */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMWN);
				goto error;
			}
			if (mdp->mu_win != 0) {
				dsperr(M_MULWIN);
				goto error;
			}
			mdp->mu_win = glblp->inumber;
			break;

		case HEAD_TOK:		/* window header */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMHN);
				goto error;
			}
			if (mdp->mu_hd != 0) {
				dsperr(M_MULHD);
				goto error;
			}
			mdp->mu_hd = glblp->inumber;
			break;

		case PNT_TOK:		/* mouse pointer */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMPN);
				goto error;
			}
			if (mdp->mu_pnt != 0) {
				dsperr(M_MPNT);
				goto error;
			}
			mdp->mu_pnt = glblp->inumber;
			break;

		case STR_TOK:		/* a string */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMSN);
				goto error;
			}

			/*
			 * Allocate a new element in the string and choice
			 * list.
			 */
			ldp = (struct actlist *)allocmem(sizeof(struct actlist));
			ldp->al_code = AL_STRING;
			ldp->al_no = glblp->inumber;
			break;

		case PSTR_TOK:		/* a portrait mode string */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMPSN);
				goto error;
			}

			/*
			 * Allocate a new element in the string and choice
			 * list.
			 */
			ldp = (struct actlist *)allocmem(sizeof(struct actlist));
			ldp->al_code = AL_PSTRING;
			ldp->al_no = glblp->inumber;
			break;

		case LSTR_TOK:		/* a landscape mode string */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMLSN);
				goto error;
			}

			/*
			 * Allocate a new element in the string and choice
			 * list.
			 */
			ldp = (struct actlist *)allocmem(sizeof(struct actlist));
			ldp->al_code = AL_LSTRING;
			ldp->al_no = glblp->inumber;
			break;

		case ACT_TOK:		/* action list */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMAN);
				goto error;
			}
			if (mdp->mu_act != 0) {
				dsperr(M_MULAC);
				goto error;
			}
			mdp->mu_act = glblp->inumber;
			break;

		case CHO_TOK:		/* a choice */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMCHN);
				goto error;
			}

			/*
			 * Allocate a new element in the string and choice
			 * list.
			 */
			ldp = (struct actlist *)allocmem(sizeof(struct actlist));
			ldp->al_code = AL_CHOICE;
			ldp->al_no = glblp->inumber;
			break;

		default:		/* error */
			illsyntax();
			goto error;
		}
	}

	/*
	 * Allocate the terminating element in the string and choice list.
	 */
	ldp = (struct actlist *)allocmem(sizeof(struct actlist));
	/* ldp->al_code = AL_END; */

	return;

error:
	skipeoln();
	return;
}	/* end of menudecod() */



/*
 * Decode terminal data.
 */

termdecod()
{
	register struct	globals	*glblp;		/* pointer to global variables */
	register struct	termdat	*tdp;		/* pointer to terminal data */
	register struct	actlist	*ldp;		/* string and icon list */
	register		act;		/* action code */
	char			*allocmem();

	glblp = &glbl;
	if (glblp->numflag == NO || glblp->inumber <= 0) {
		dsperr(M_IMTERM);
		goto error;
	}

	/*
	 * Allocate memory for the termdat structure.
	 */
	tdp = (struct termdat *)allocmem(sizeof(struct termdat));
	if (glblp->termstart == NULL) {
		glblp->termstart = tdp;
	}
	else {
		glblp->termcur->td_next = tdp;
	}
	glblp->termcur = tdp;
	tdp->td_no = glblp->inumber;	/* the terminal window number */
	tdp->td_list = (struct actlist *)allocmem(0);

	/*
	 * Now process the terminal data.
	 */
	while ((act = nextact()) != EOLN && act != EOF) {
		switch (act) {

		case WIN_TOK:		/* window */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMWN);
				goto error;
			}
			if (tdp->td_win != 0) {
				dsperr(M_MULWIN);
				goto error;
			}
			tdp->td_win = glblp->inumber;
			break;

		case HEAD_TOK:		/* window header */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMHN);
				goto error;
			}
			if (tdp->td_head != 0) {
				dsperr(M_MULHD);
				goto error;
			}
			tdp->td_head = glblp->inumber;
			break;

		case ZOOM_TOK:		/* zoom list */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMZN);
				goto error;
			}
			if (tdp->td_zoom != 0) {
				dsperr(M_MULZOOM);
				goto error;
			}
			tdp->td_zoom = glblp->inumber;
			break;

		case PNT_TOK:		/* mouse pointer */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMPN);
				goto error;
			}
			if (tdp->td_mp != 0) {
				dsperr(M_MPNT);
				goto error;
			}
			tdp->td_mp = glblp->inumber;
			break;

		case STR_TOK:		/* a string */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMSN);
				goto error;
			}

			/*
			 * Allocate a new element in the string, environment, and icon list.
			 */
			ldp = (struct actlist *)allocmem(sizeof(struct actlist));
			ldp->al_code = AL_STRING;
			ldp->al_no = glblp->inumber;
			break;

		case PSTR_TOK:		/* a portrait mode string */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMPSN);
				goto error;
			}

			/*
			 * Allocate a new element in the string, environment, and icon list.
			 */
			ldp = (struct actlist *)allocmem(sizeof(struct actlist));
			ldp->al_code = AL_PSTRING;
			ldp->al_no = glblp->inumber;
			break;

		case LSTR_TOK:		/* a landscape mode string */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMLSN);
				goto error;
			}

			/*
			 * Allocate a new element in the string, environment, and icon list.
			 */
			ldp = (struct actlist *)allocmem(sizeof(struct actlist));
			ldp->al_code = AL_LSTRING;
			ldp->al_no = glblp->inumber;
			break;

		case ICON_TOK:		/* an icon */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMIN);
				goto error;
			}

			/*
			 * Allocate a new element in the string, environment, and icon list.
			 */
			ldp = (struct actlist *)allocmem(sizeof(struct actlist));
			ldp->al_code = AL_ICON;
			ldp->al_no = glblp->inumber;
			break;

		case DIR_TOK:		/* a pathname */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMDN);
				goto error;
			}
			if (tdp->td_dir != 0) {
				dsperr(M_MULDIR);
				goto error;
			}
			tdp->td_dir = glblp->inumber;
			break;

		case ENV_TOK:		/* environment variables */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMENVN);
				goto error;
			}

			/*
			 * Allocate a new element in the list.
			 */
			ldp = (struct actlist *)allocmem(sizeof(struct actlist));
			ldp->al_code = AL_ENV;
			ldp->al_no = glblp->inumber;
			break;

		case PENV_TOK:		/* environment variables */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMPENVN);
				goto error;
			}

			/*
			 * Allocate a new element in the list.
			 */
			ldp = (struct actlist *)allocmem(sizeof(struct actlist));
			ldp->al_code = AL_PENV;
			ldp->al_no = glblp->inumber;
			break;

		case LENV_TOK:		/* environment variables */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMLENVN);
				goto error;
			}

			/*
			 * Allocate a new element in the list.
			 */
			ldp = (struct actlist *)allocmem(sizeof(struct actlist));
			ldp->al_code = AL_LENV;
			ldp->al_no = glblp->inumber;
			break;

		case SUP_TOK:		/* super channel */
			if (glblp->numflag != NO) {
				dsperr(M_NSUPNO);
				goto error;
			}
			tdp->td_flags |= TD_SUPER;
			break;

		case WAIT_TOK:		/* wait for the command to finish */
			if (glblp->numflag != NO) {
				dsperr(M_NWTNO);
				goto error;
			}
			tdp->td_flags |= TD_WAIT;
			break;

		case CMD_TOK:		/* the command */
			if (glblp->numflag == NO || glblp->inumber <= 0) {
				dsperr(M_IMCN);
				goto error;
			}
			if (tdp->td_cmd != 0) {
				dsperr(M_MULCMD);
				goto error;
			}
			tdp->td_cmd = glblp->inumber;
			break;

		default:
			illsyntax();
			goto error;
		}
	}

	/*
	 * Allocate the terminating element in the string and icon list.
	 */
	ldp = (struct actlist *)allocmem(sizeof(struct actlist));
	/* ldp->al_code = AL_END; */

	return;

error:
	skipeoln();
	return;
}	/* end of termdecod() */
