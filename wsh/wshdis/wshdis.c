/* wshdis.c */

/*
 * This file contains source code for the window shell disassemble command -
 * wshdis.
 */

/* 1985-02-26, Peter Andersson, Luxor Datorer AB */

/*
 * Functions in this file:
 *	main()
 *	errexit()
 *	outlstr()
 *	outitem()
 *	outval()
 *	outflag()
 *	outhexval()
 *	itemout()
 *	escout()
 *	snumout()
 *	hexout()
 *	twonl()
 *	cnvwin()
 *	cnvicon()
 *	cnvhead()
 *	cnvflags()
 *	cnvsubst()
 *	cnvstr()
 *	cnvpstr()
 *	cnvlstr()
 *	cnvdir()
 *	strdircommon()
 *	cnvenv()
 *	cnvpenv()
 *	cnvlenv()
 *	cnvcmd()
 *	cnvcmdcommon()
 *	cnvmp()
 *	cnvbg()
 *	cnvchoic()
 *	cnvinit()
 *	cnvact()
 *	cnvmenu()
 *	cnvterm()
 */



#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	"../h/w_language.h"
#include	"../h/w_wsh.h"
#include	"wshdis_defs.h"
#include	"wshdis_glbl.h"
#include	"wshdis_msg.h"



/*
 * Global variables.
 */
struct	globals	glbl;



/*
 * The main function.
 */

main(argc, argv)
int	argc;		/* argument count */
char	**argv;		/* argument list */
{
	register struct	globals	*glblp;		/* pointer to global variables */
	register	char	*s;		/* temporary */
	register struct	wshhead	*fh;		/* pointer to file header */
	register		infd;		/* file descriptor of input file */
	register	long	diff;		/* value to adjust pointers with */
	char			*sbrk();
	char			*getenv();

	glblp = &glbl;
	glblp->cmdname = *argv;

	/*
	 * Determine which language to use.
	 */
	if ((s = getenv("LANGUAGE")) != NULL && strcmp(s, "swedish") == 0) {
		glblp->language = SWEDISH;
	}
	else {
		glblp->language = ENGLISH;
	}

	/* glblp->infile = NULL; */
	/* glblp->outfile = NULL; */
	/* glblp->linlen = 0; */

	/*
	 * Decode the arguments.
	 */
	if (argc > 1) {
		while ((s = *++argv) != NULL) {
			if (strcmp(s, "-o") == 0) {
				if (glblp->outfile != NULL) {
					errexit(M_MULOUT);
				}
				if ((glblp->outfile = *++argv) == NULL) {
					errexit(M_FNAMMIS);
				}
			}
			else if (glblp->infile != NULL) {
				errexit(M_MULIN);
			}
			else {
				glblp->infile = s;
			}
		}
	}
	if (glblp->infile == NULL) {
		glblp->infile = DFLTFILE;	/* default input file */
	}
	if ((infd = open(glblp->infile, 0)) == -1) {
		errexit(M_FOIF);	/* can't open input file */
	}

	/*
	 * Allocate memory for the file header structure.
	 */
	if ((int)(fh = (struct wshhead *)sbrk(sizeof(struct wshhead))) == -1) {
		errexit(M_NOMEM);	/* can't allocate memory */
	}
	if (read(infd, (char *)fh, sizeof(struct wshhead)) != sizeof(struct wshhead) ||
	    fh->wh_magic != MAGIC || fh->wh_size < sizeof(struct wshhead)) {
		errexit(M_ILLFORM);	/* illegal format of input file */
	}

	/*
	 * Allocate memory for the rest of the file.
	 */
	diff = fh->wh_size - sizeof(struct wshhead) + 2;
	if ((int)sbrk(diff) == -1) {
		errexit(M_NOMEM);
	}
	if (read(infd, (char *)fh + sizeof(struct wshhead), diff) != diff - 2) {
		errexit(M_ILLFORM);
	}

	/*
	 * Create the output file.
	 */
	if (glblp->outfile == NULL) {
		glblp->outfd = STDOUT;
	}
	else if ((glblp->outfd = creat(glblp->outfile, PERMISSION)) == -1) {
		errexit(M_FCOF);
	}

	/*
	 * Now convert the input file to a text file.
	 */
	diff = glblp->offset = (long)fh;

	/*
	 * Convert the window descriptions.
	 */
	s = (char *)fh->wh_wd;
	while (s != NULL) {
		s += diff;
		cnvwin((struct windat *)s);
		s = (char *)((struct windat *)s)->wd_next;
	}

	/*
	 * Convert the header descriptions.
	 */
	s = (char *)fh->wh_hd;
	while (s != NULL) {
		s += diff;
		cnvhead((struct headdat *)s);
		s = (char *)((struct headdat *)s)->hd_next;
	}

	/*
	 * Convert the icons.
	 */
	s = (char *)fh->wh_ic;
	while (s != NULL) {
		s += diff;
		cnvicon((struct icondat *)s);
		s = (char *)((struct icondat *)s)->ic_next;
	}

	/*
	 * Convert the window flags.
	 */
	s = (char *)fh->wh_flg;
	while (s != NULL) {
		s += diff;
		cnvflags((struct flagdat *)s);
		s = (char *)((struct flagdat *)s)->fl_next;
	}

	/*
	 * Convert the zoom list.
	 */
	s = (char *)fh->wh_zoom;
	while (s != NULL) {
		s += diff;
		cnvzoom((struct zoomdat *)s);
		s = (char *)((struct zoomdat *)s)->zm_next;
	}

	/*
	 * Convert the mouse pointers.
	 */
	s = (char *)fh->wh_mp;
	while (s != NULL) {
		s += diff;
		cnvmp((struct mpdat *)s);
		s = (char *)((struct mpdat *)s)->mp_next;
	}

	/*
	 * Convert the mouse substitute keys.
	 */
	s = (char *)fh->wh_sub;
	while (s != NULL) {
		s += diff;
		cnvsubst((struct subdat *)s);
		s = (char *)((struct subdat *)s)->sk_next;
	}

	/*
	 * Convert the background pattern.
	 */
	s = (char *)fh->wh_bg;
	while (s != NULL) {
		s += diff;
		cnvsubst((struct bgdat *)s);
		s = (char *)((struct bgdat *)s)->bg_next;
	}

	/*
	 * Convert the strings.
	 */
	s = (char *)fh->wh_str;
	while (s != NULL) {
		s += diff;
		cnvstr((struct strdat *)s);
		s = (char *)((struct strdat *)s)->st_next;
	}

	/*
	 * Convert the portrait mode strings.
	 */
	s = (char *)fh->wh_pstr;
	while (s != NULL) {
		s += diff;
		cnvpstr((struct strdat *)s);
		s = (char *)((struct strdat *)s)->st_next;
	}

	/*
	 * Convert the landscape mode strings.
	 */
	s = (char *)fh->wh_lstr;
	while (s != NULL) {
		s += diff;
		cnvlstr((struct strdat *)s);
		s = (char *)((struct strdat *)s)->st_next;
	}

	/*
	 * Convert the pathnames.
	 */
	s = (char *)fh->wh_dir;
	while (s != NULL) {
		s += diff;
		cnvdir((struct strdat *)s);
		s = (char *)((struct strdat *)s)->st_next;
	}

	/*
	 * Convert the environment variables.
	 */
	s = (char *)fh->wh_env;
	while (s != NULL) {
		s += diff;
		cnvenv((struct arrdat *)s);
		s = (char *)((struct arrdat *)s)->ca_next;
	}

	/*
	 * Convert the portrait mode environment variables.
	 */
	s = (char *)fh->wh_penv;
	while (s != NULL) {
		s += diff;
		cnvpenv((struct arrdat *)s);
		s = (char *)((struct arrdat *)s)->ca_next;
	}

	/*
	 * Convert the landscape mode environment variables.
	 */
	s = (char *)fh->wh_lenv;
	while (s != NULL) {
		s += diff;
		cnvlenv((struct arrdat *)s);
		s = (char *)((struct arrdat *)s)->ca_next;
	}

	/*
	 * Convert the commands.
	 */
	s = (char *)fh->wh_cmd;
	while (s != NULL) {
		s += diff;
		cnvcmd((struct arrdat *)s);
		s = (char *)((struct arrdat *)s)->ca_next;
	}

	/*
	 * Convert the menus.
	 */
	s = (char *)fh->wh_menu;
	while (s != NULL) {
		s += diff;
		cnvmenu((struct menudat *)s);
		s = (char *)((struct menudat *)s)->mu_next;
	}

	/*
	 * Convert the choices.
	 */
	s = (char *)fh->wh_cho;
	while (s != NULL) {
		s += diff;
		cnvchoic((struct chodat *)s);
		s = (char *)((struct chodat *)s)->co_next;
	}

	/*
	 * Convert the actions.
	 */
	s = (char *)fh->wh_act;
	while (s != NULL) {
		s += diff;
		cnvact((struct actdat *)s);
		s = (char *)((struct actdat *)s)->ac_next;
	}

	/*
	 * Convert the terminal windows.
	 */
	s = (char *)fh->wh_term;
	while (s != NULL) {
		s += diff;
		cnvterm((struct termdat *)s);
		s = (char *)((struct termdat *)s)->td_next;
	}

	/*
	 * Convert the initialization data.
	 */
	if ((s = (char *)fh->wh_ini) != NULL) {
		cnvinit((struct actlist *)(s + diff));
	}

	/*
	 * Make sure that the file output buffer is emptied.
	 */
	emptybuf();

	exit(0);
}	/* end of main() */



/*
 * Display ann error on the standard error output and then exit with false
 * status.
 */

errexit(code)
int	code;		/* code of message to display */
{
	char	*s;

	if (glbl.cmdname != NULL) {
		write(STDERR, glbl.cmdname, strlen(glbl.cmdname));
		write(STDERR, ": ", 2);
	}
	s = messages[2*code+glbl.language];
	write(STDERR, s, strlen(s));

	exit(1);
}	/* end of errexit() */



/*
 * Output a colon, a keyword, an equal character, and a string of a given
 * maximum length. If more than 70 characters minus the length of the string
 * has been outputted to the current line, first insert a newline.
 */

outlstr(kw, s, maxlen)
char	*kw;		/* pointer to keyword */
char	*s;		/* pointer to string */
int	maxlen;		/* max length of string */
{
	register char	*cp;	/* temporary */
	register	tmp;	/* temporary */
	register	len;	/* length of string when outputted */
	register	c;	/* a character in the string */

	tmp = maxlen;
	cp = s;
	len = 0;
	while (tmp-- > 0 && (c = *cp++ & EIGHTMASK) != '\0') {
		len++;
		if (c < ' ' || c > '~') {
			len += 3;
		}
		else if (c == '\\' || c == ':') {
			len++;
		}
	}
	if (glbl.linlen + len > 70 && glbl.linlen > 0) {
		strout("\\\n");
	}
	charout(':');
	strout(kw);
	charout('=');

	/*
	 * Now output the string.
	 */
	tmp = maxlen;
	cp = s;
	while (tmp-- > 0 && (c = *cp++ & EIGHTMASK) != '\0') {
		if (glbl.linlen > 70) {
			strout("\\\n");
		}
		if (c == '\\' || c == ':' || c < ' ' || c > '~') {
			escout(c);
		}
		else {
			charout(c);
		}
	}

	return;
}	/* end of outlstr() */



/*
 * Output a colon, a item name, and an optional number. If more than 60
 * characters has been output to the current line, first insert a newline.
 */

outitem(s, n)
char	*s;		/* item name */
int	n;		/* number (only displayed if non-zero) */
{
	if (glbl.linlen > 60) {
		strout("\\\n");
	}
	charout(':');
	strout(s);
	if (n != 0) {
		snumout(n);
	}

	return;
}	/* end of outitem() */



/*
 * Output a colon, a keyword, a number sign, and a value. If more than 60
 * characters has been output to the current line, first insert a newline.
 */

outval(s, n)
char	*s;		/* keyword */
int	n;		/* value */
{
	if (glbl.linlen > 60) {
		strout("\\\n");
	}
	charout(':');
	strout(s);
	charout('#');
	snumout(n);

	return;
}	/* end of outval() */



/*
 * Output a colon and a flag. If more than 65 characters has been output to the
 * current line, first insert a newline.
 */

outflag(s)
char	*s;		/* flag to output */
{
	if (glbl.linlen > 65) {
		strout("\\\n");
	}
	charout(':');
	strout(s);

	return;
}	/* end of outflag() */



/*
 * Output a colon, a keyword, a number sign, and a hexadecimal value. If more
 * than 60 characters has been output to the current line, first insert a
 * newline.
 */

outhexval(s, n)
char		*s;		/* keyword to output */
unsigned long	n;		/* number to output */
{
	if (glbl.linlen > 60) {
		strout("\\\n");
	}
	charout(':');
	strout(s);
	charout('#');
	hexout(n);

	return;
}	/* end of outhexval() */



/*
 * Output an item name immediately followed by a number.
 */

itemout(s, n)
char	*s;		/* item name */
int	n;		/* number */
{
	strout(s);
	snumout(n);

	return;
}	/* end of itemout() */



/*
 * Output a character which has to be escaped by a backslash.
 */

escout(c)
register	c;		/* character */
{
	charout('\\');
	switch (c) {

	case '\n':		/* newline */
		c = 'n';
		break;

	case '\t':		/* tab */
		c = 't';
		break;

	case '\b':		/* backspace */
		c = 'b';
		break;

	case '\r':		/* carriage return */
		c = 'r';
		break;

	case '\f':		/* formfeed */
		c = 'f';
		break;

	case '\033':		/* escape */
		c = 'e';
		break;

	case '\\':
	case ':':
	case ',':
		break;

	default:		/* convert to an octal number */
		charout(c % 8 + '0');
		c /= 8;
		charout(c % 8 + '0');
		c /= 8;
		c += '0';
		break;
	}
	charout(c);

	return;
}	/* end of escout() */



/*
 * Output a signed decimal number.
 */

snumout(n)
register	n;		/* number to output */
{
	char		buf[16];	/* conversion buffer */
	register char	*cp;		/* pointer into buf[] */

	if (n < 0) {
		charout('-');
		n = -n;
	}
	cp = &buf[15];
	*cp = '\0';
	do {
		*--cp = n % 10 + '0';
	} while ((n /= 10) > 0);
	strout(cp);

	return;
}	/* end of snumout() */



/*
 * Output a hexadecimal number.
 */

hexout(n)
register unsigned long	n;	/* number to output */
{
	char		buf[16];	/* conversion buffer */
	register char	*cp;		/* pointer into buf[] */
	register	tmp;		/* temporary */

	strout("0x");
	cp = &buf[15];
	*cp = '\0';
	do {
		tmp = n % 16;
		if (tmp < 10) {
			tmp += '0';
		}
		else {
			tmp += 'a' - 10;
		}
		*--cp = tmp;
	} while ((n /= 16) > 0);
	strout(cp);

	return;
}	/* end of hexout() */



/*
 * Output two newlines to the output file.
 */

twonl()
{
	charout('\n');
	charout('\n');

	return;
}	/* end of twonl() */



/*
 * Convert a windat structure to text.
 */

cnvwin(wdp)
register struct	windat	*wdp;		/* pointer to window data */
{
	register uflags	flag;		/* window flags */
	register word	bflag;		/* border boxes flags */

	flag = wdp->wd_win.w_flags;
	bflag = wdp->wd_win.w_boxes;

	itemout(D_WINDOW, wdp->wd_no);
	outval(WP_XORIG, wdp->wd_win.wp_xorig);
	outval(WL_XORIG, wdp->wd_win.wl_xorig);
	outval(WP_YORIG, wdp->wd_win.wp_yorig);
	outval(WL_YORIG, wdp->wd_win.wl_yorig);
	outval(WP_XSIZE, wdp->wd_win.wp_xsize);
	outval(WL_XSIZE, wdp->wd_win.wl_xsize);
	outval(WP_YSIZE, wdp->wd_win.wp_ysize);
	outval(WL_YSIZE, wdp->wd_win.wl_ysize);
	outval(WP_VXORIG, wdp->wd_win.wp_vxorig);
	outval(WL_VXORIG, wdp->wd_win.wl_vxorig);
	outval(WP_VYORIG, wdp->wd_win.wp_vyorig);
	outval(WL_VYORIG, wdp->wd_win.wl_vyorig);
	outval(WP_VXSIZE, wdp->wd_win.wp_vxsize);
	outval(WL_VXSIZE, wdp->wd_win.wl_vxsize);
	outval(WP_VYSIZE, wdp->wd_win.wp_vysize);
	outval(WL_VYSIZE, wdp->wd_win.wl_vysize);
	outval(W_COLOR, wdp->wd_win.w_color);
	outval(W_BORDER, wdp->wd_win.w_border);
	outval(WP_FONT, wdp->wd_win.wp_font);
	outval(WL_FONT, wdp->wd_win.wl_font);
	if (bflag & BX_USER) {
		outval(W_UBOXES, wdp->wd_win.w_uboxes);
	}
	outval(W_TSIG, wdp->wd_win.w_tsig);
	outval(W_NTSIG, wdp->wd_win.w_ntsig);
	outval(W_RSIG, wdp->wd_win.w_rsig);
	outval(W_CSIG, wdp->wd_win.w_csig);

	if (bflag & BX_CLOS) {
		outflag(WB_CBOX);
	}
	if (bflag & BX_SIZE) {
		outflag(WB_SBOX);
	}
	if (bflag & BX_MOVE) {
		outflag(WB_MBOX);
	}
	if (bflag & BX_ZOOM) {
		outflag(WB_ZBOX);
	}
	if (bflag & BX_HSCR) {
		outflag(WB_HSCR);
	}
	if (bflag & BX_VSCR) {
		outflag(WB_VSCR);
	}
	if (bflag & BX_AVIS) {
		outflag(WB_AVIS);
	}
	if (bflag & BX_BLOW) {
		outflag(WB_BBOX);
	}
	if (bflag & BX_HELP) {
		outflag(WB_HBOX);
	}
	if (flag & PMODE) {
		outflag(WF_PMODE);
	}
	if (flag & LMODE) {
		outflag(WF_LMODE);
	}
	if (flag & SAVETEXT) {
		outflag(WF_SAVTXT);
	}
	if (flag & SAVEBITMAP) {
		outflag(WF_SAVBM);
	}
	if (flag & LOCK) {
		outflag(WF_LOCK);
	}
	if (flag & NOOVER) {
		outflag(WF_NOOVER);
	}
	if (flag & NOCURSOR) {
		outflag(WF_NOCUR);
	}
	if (flag & NOMOVE) {
		outflag(WF_NOMOVE);
	}
	if (flag & ALLSCR) {
		outflag(WF_ALLSCR);
	}
	if (flag & SPECIAL) {
		outflag(WF_SPECIAL);
	}
	if (flag & KEYSCROLL) {
		outflag(WF_KSCR);
	}
	if (flag & WRITSCROLL) {
		outflag(WF_WSCR);
	}
	if (flag & ALTMPNT) {
		outflag(WF_ALTMPNT);
	}
	if (flag & RELATIVE) {
		outflag(WF_REL);
	}
	if (flag & NOCPIN) {
		outflag(WF_NCPIN);
	}
	if (flag & NOCPOUT) {
		outflag(WF_NCPOUT);
	}
	if (flag & TXTSIZE) {
		outflag(WF_TEXT);
	}
	if (flag & WGROUP) {
		outflag(WF_GROUP);
	}
	if (flag & REL_ULC) {
		outflag(WF_RULC);
	}
	if (flag & REL_URC) {
		outflag(WF_RURC);
	}
	if (flag & REL_LLC) {
		outflag(WF_RLLC);
	}
	if (flag & REL_LRC) {
		outflag(WF_RLRC);
	}
	twonl();

	return;
}	/* end of cnvwin() */



/*
 * Convert an icondat structure to text.
 */

cnvicon(idp)
register struct	icondat	*idp;		/* pointer to icon data */
{
	register word	flag;		/* icon flags */

	itemout(D_ICON, idp->ic_no);
	outval(IP_XORIG, idp->ic_icon.ip_xorig);
	outval(IL_XORIG, idp->ic_icon.il_xorig);
	outval(IP_YORIG, idp->ic_icon.ip_yorig);
	outval(IL_YORIG, idp->ic_icon.il_yorig);
	outval(IP_XSIZE, idp->ic_icon.ip_xsize);
	outval(IL_XSIZE, idp->ic_icon.il_xsize);
	outval(IP_YSIZE, idp->ic_icon.ip_ysize);
	outval(IL_YSIZE, idp->ic_icon.il_ysize);
	outlstr(I_CMDSEQ, &idp->ic_icon.i_cmdseq[0], ICONSEQLEN);
	flag = idp->ic_icon.i_flags;
	if (flag & I_PMODE) {
		outflag(IF_PMODE);
	}
	if (flag & I_LMODE) {
		outflag(IF_LMODE);
	}
	if (flag & I_PRESS) {
		outflag(IF_PRESS);
	}
	if (flag & I_RELEASE) {
		outflag(IF_RLSE);
	}
	if (flag & I_INVERT) {
		outflag(IF_INV);
	}
	if (flag & I_ENTER) {
		outflag(IF_ENTER);
	}
	if (flag & I_LEAVE) {
		outflag(IF_LEAVE);
	}
	if (flag & I_REMOVE) {
		outflag(IF_REMOV);
	}
	if (flag & I_RQST) {
		outflag(IF_RQST);
	}
	if (flag & I_SETCHK) {
		outflag(IF_SETCHK);
	}
	if (flag & I_LZERO) {
		outflag(IF_LZERO);
	}
	if (flag & I_TEXT) {
		outflag(IF_TEXT);
	}
	twonl();

	return;
}	/* end of cnvicon() */



/*
 * Convert a headdat structure to text.
 */

cnvhead(hdp)
register struct	headdat	*hdp;		/* pointer to header data */
{
	itemout(D_HEADER, hdp->hd_no);
	outlstr(H_HDR, &hdp->hd_head.h_hdr[0], HDRSIZE);
	if (hdp->hd_head.h_flags & H_INVHD) {
		outflag(HF_INVH);
	}
	if (hdp->hd_head.h_flags & H_INVTOP) {
		outflag(HF_INVT);
	}
	twonl();

	return;
}	/* end of cnvhead() */



/*
 * Convert a flagdat structure to text.
 */

cnvflags(fdp)
struct	flagdat	*fdp;
{
	register uflags	flag;		/* window flags */

	itemout(D_FLAGS, fdp->fl_no);
	flag = fdp->fl_flags.f_flags;
	if (flag & LOCK) {
		outflag(FF_LOCK);
	}
	if (flag & NOOVER) {
		outflag(FF_NOOVER);
	}
	if (flag & NOCURSOR) {
		outflag(FF_NOCUR);
	}
	if (flag & NOMOVE) {
		outflag(FF_NOMOVE);
	}
	if (flag & ALLSCR) {
		outflag(FF_ALLSCR);
	}
	if (flag & KEYSCROLL) {
		outflag(FF_KSCR);
	}
	if (flag & WRITSCROLL) {
		outflag(FF_WSCR);
	}
	if (flag & NOCPIN) {
		outflag(FF_NCPIN);
	}
	if (flag & NOCPOUT) {
		outflag(FF_NCPOUT);
	}
	if (flag & REL_ULC) {
		outflag(FF_RULC);
	}
	if (flag & REL_URC) {
		outflag(FF_RURC);
	}
	if (flag & REL_LLC) {
		outflag(FF_RLLC);
	}
	if (flag & REL_LRC) {
		outflag(FF_RLRC);
	}
	twonl();

	return;
}	/* end of cnvflags() */



/*
 * Convert a zoomdat structure to text.
 */

cnvzoom(zdp)
register struct	zoomdat	*zdp;		/* pointer to zoom list data */
{
	itemout(D_ZOOM, zdp->zm_no);
	outlstr(ZP_LIST, &zdp->zm_zoom.zp_list[0], ZOOMSIZE);
	outlstr(ZL_LIST, &zdp->zm_zoom.zl_list[0], ZOOMSIZE);
	if (zdp->zm_zoom.z_flags & Z_PMODE) {
		outflag(ZF_PMODE);
	}
	if (zdp->zm_zoom.z_flags & Z_LMODE) {
		outflag(ZF_LMODE);
	}
	twonl();

	return;
}	/* end of cnvzoom() */



/*
 * Convert the mouse substitute keys to text.
 */

cnvsubst(sdp)
register struct	subdat	*sdp;	/* pointer to mouse key data */
{
	itemout(D_SUBST, sdp->sk_no);
	outval(C_INITFLG, sdp->sk_subst.c_initflg);
	outhexval(C_ONOFF, (unsigned long)(sdp->sk_subst.c_keys[S_ONOFF] & EIGHTMASK));
	outhexval(C_MPU, (unsigned long)(sdp->sk_subst.c_keys[S_MPU] & EIGHTMASK));
	outhexval(C_MPD, (unsigned long)(sdp->sk_subst.c_keys[S_MPD] & EIGHTMASK));
	outhexval(C_MPL, (unsigned long)(sdp->sk_subst.c_keys[S_MPL] & EIGHTMASK));
	outhexval(C_MPR, (unsigned long)(sdp->sk_subst.c_keys[S_MPR] & EIGHTMASK));
	outhexval(C_MPUL, (unsigned long)(sdp->sk_subst.c_keys[S_MPUL] & EIGHTMASK));
	outhexval(C_MPUR, (unsigned long)(sdp->sk_subst.c_keys[S_MPUR] & EIGHTMASK));
	outhexval(C_MPDL, (unsigned long)(sdp->sk_subst.c_keys[S_MPDL] & EIGHTMASK));
	outhexval(C_MPDR, (unsigned long)(sdp->sk_subst.c_keys[S_MPDR] & EIGHTMASK));
	outhexval(C_LMPU, (unsigned long)(sdp->sk_subst.c_keys[S_LMPU] & EIGHTMASK));
	outhexval(C_LMPD, (unsigned long)(sdp->sk_subst.c_keys[S_LMPD] & EIGHTMASK));
	outhexval(C_LMPL, (unsigned long)(sdp->sk_subst.c_keys[S_LMPL] & EIGHTMASK));
	outhexval(C_LMPR, (unsigned long)(sdp->sk_subst.c_keys[S_LMPR] & EIGHTMASK));
	outhexval(C_LMPUL, (unsigned long)(sdp->sk_subst.c_keys[S_LMPUL] & EIGHTMASK));
	outhexval(C_LMPUR, (unsigned long)(sdp->sk_subst.c_keys[S_LMPUR] & EIGHTMASK));
	outhexval(C_LMPDL, (unsigned long)(sdp->sk_subst.c_keys[S_LMPDL] & EIGHTMASK));
	outhexval(C_LMPDR, (unsigned long)(sdp->sk_subst.c_keys[S_LMPDR] & EIGHTMASK));
	outhexval(C_PCMD, (unsigned long)(sdp->sk_subst.c_keys[S_PCMD] & EIGHTMASK));
	outhexval(C_CHWIN, (unsigned long)(sdp->sk_subst.c_keys[S_CHWIN] & EIGHTMASK));
	outhexval(C_MCA, (unsigned long)(sdp->sk_subst.c_keys[S_MCA] & EIGHTMASK));
	outval(C_STEP, sdp->sk_subst.c_step & EIGHTMASK);
	outval(C_LSTEP, sdp->sk_subst.c_lstep & EIGHTMASK);
	twonl();

	return;
}	/* end of cnvsubst() */



/*
 * Convert a strdat structure to text.
 */

cnvstr(sdp)
struct	strdat	*sdp;		/* pointer to string data */
{
	itemout(D_STRING, sdp->st_no);
	strdircommon(sdp->st_str);

	return;
}	/* end of cnvstr() */



/*
 * Convert a portrait strdat structure to text.
 */

cnvpstr(sdp)
struct	strdat	*sdp;		/* pointer to string data */
{
	itemout(D_PSTRING, sdp->st_no);
	strdircommon(sdp->st_str);

	return;
}	/* end of cnvpstr() */



/*
 * Convert a landscape strdat structure to text.
 */

cnvlstr(sdp)
struct	strdat	*sdp;		/* pointer to string data */
{
	itemout(D_LSTRING, sdp->st_no);
	strdircommon(sdp->st_str);

	return;
}	/* end of cnvlstr() */



/*
 * Convert a directory data structure to text.
 */

cnvdir(ddp)
struct	strdat	*ddp;		/* pointer to pathname data */
{
	itemout(D_DIR, ddp->st_no);
	strdircommon(ddp->st_str);

	return;
}	/* end of cnvdir() */



/*
 * Common code used by cnvstr(), cnvpstr(), cnvlstr(), and cnvdir().
 */

strdircommon(s)
register char	*s;	/* string to output */
{
	register	c;	/* character */

	charout(':');
	if (s != NULL) {
		s += glbl.offset;
		while ((c = *s++ & EIGHTMASK) != '\0') {
			if (glbl.linlen > 70) {
				strout("\\\n");
			}
			if (c == '\\' || c < ' ' || c > '~') {
				escout(c);
			}
			else {
				charout(c);
			}
		}
	}
	twonl();

	return;
}	/* end of strdircommon() */



/*
 * Convert environment variables to text.
 */

cnvenv(envp)
struct	arrdat	*envp;		/* pointer to environment data */
{
	itemout(D_ENVIRON, envp->ca_no);
	envcmdcommon(envp->ca_args);

	return;
}	/* end of cnvenv() */



/*
 * Convert portrait mode environment variables to text.
 */

cnvpenv(envp)
struct	arrdat	*envp;		/* pointer to environment data */
{
	itemout(D_PENVIRON, envp->ca_no);
	envcmdcommon(envp->ca_args);

	return;
}	/* end of cnvenv() */



/*
 * Convert landscape mode environment variables to text.
 */

cnvlenv(envp)
struct	arrdat	*envp;		/* pointer to environment data */
{
	itemout(D_LENVIRON, envp->ca_no);
	envcmdcommon(envp->ca_args);

	return;
}	/* end of cnvenv() */



/*
 * Convert a command to text.
 */

cnvcmd(cdp)
struct	arrdat	*cdp;		/* pointer to command data */
{
	itemout(D_CMD, cdp->ca_no);
	envcmdcommon(cdp->ca_args);

	return;
}	/* end of cnvcmd() */



/*
 * Common code used by cnvenv(), cnvpenv(), cnvlenv(), and cnvcmd().
 */

envcmdcommon(cpp)
register char	**cpp;		/* pointer argument array */
{
	register char	*cp;		/* pointer into argument */
	register	c;		/* character */

	charout(':');
	if (cpp != NULL) {
		(char *)cpp += glbl.offset;
		while (*cpp != NULL) {
			cp = *cpp++ + glbl.offset;
			while ((c = *cp++ & EIGHTMASK) != '\0') {
				if (glbl.linlen > 70) {
					strout("\\\n");
				}
				if (c == '\\' || c == ',' || c < ' ' || c > '~') {
					escout(c);
				}
				else {
					charout(c);
				}
			}
			if (*cpp != NULL) {
				charout(',');
			}
		}
	}
	twonl();

	return;
}	/* end of envcmdcommon() */



/*
 * Convert a mpdat structure to text.
 */

cnvmp(mdp)
register struct	mpdat	*mdp;	/* pointer to mouse pointer data */
{
	register	i;		/* loop index */

	itemout(D_POINTER, mdp->mp_no);
	outval(NP_XSIZE, mdp->mp_mp.np_xsize);
	outval(NP_YSIZE, mdp->mp_mp.np_ysize);
	outval(NP_XPNT, mdp->mp_mp.np_xpnt);
	outval(NP_YPNT, mdp->mp_mp.np_ypnt);
	outhexval(NP_AND, (unsigned long)mdp->mp_mp.np_and[0]);
	for (i = 1 ; i < MPSIZE ; i++) {
		charout(',');
		if (glbl.linlen > 60) {
			strout("\\\n");
		}
		hexout((unsigned long)mdp->mp_mp.np_and[i]);
	}
	outhexval(NP_OR, (unsigned long)mdp->mp_mp.np_or[0]);
	for (i = 1 ; i < MPSIZE ; i++) {
		charout(',');
		if (glbl.linlen > 60) {
			strout("\\\n");
		}
		hexout((unsigned long)mdp->mp_mp.np_or[i]);
	}
	twonl();

	return;
}	/* end of cnvmp() */



/*
 * Convert a chbgstruc structure to text.
 */

cnvbg(bdp)
register struct	bgdat	*bdp;		/* pointer to background data */
{
	register	i;		/* loop index */

	itemout(D_BGROUND, bdp->bg_no);
	outhexval(CB_BITMAP, (unsigned long)bdp->bg_bgd.cb_bitmap[0]);
	for (i = 1 ; i < BGPSIZE ; i++) {
		charout(',');
		if (glbl.linlen > 60) {
			strout("\\\n");
		}
		hexout((unsigned long)bdp->bg_bgd.cb_bitmap[i]);
	}
	twonl();

	return;
}	/* end of cnvbg() */



/*
 * Convert a chodat structure to text.
 */

cnvchoic(cdp)
register struct	chodat	*cdp;	/* pointer to choice data */
{
	itemout(D_CHOICE, cdp->co_no);
	if (cdp->co_icon != 0) {
		outitem(D_ICON, cdp->co_icon);
	}
	if (cdp->co_action != 0) {
		outitem(D_ACTION, cdp->co_action);
	}
	twonl();

	return;
}	/* end of cnvchoic() */



/*
 * Convert an inidat structure to text.
 */

cnvinit(alp)
register struct	actlist	*alp;
{
	strout(D_INIT);
	while (alp->al_code != AL_END) {
		switch (alp->al_code) {

		case AL_SUBST:
			outitem(D_SUBST, alp->al_no);
			break;

		case AL_PNT:
			outitem(D_POINTER, alp->al_no);
			break;

		case AL_BG:
			outitem(D_BGROUND, alp->al_no);
			break;

		case AL_INVERSE:
			outitem(A_INVERSE, 0);
			break;

		case AL_NORMAL:
			outitem(A_NORMAL, 0);
			break;

		case AL_TERM:
			outitem(D_TERM, alp->al_no);
			break;

		case AL_MENU:
			outitem(D_MENU, alp->al_no);
			break;
		}
		alp++;		/* next action in list */
	}
	twonl();

	return;
}	/* end of cnvinit() */



/*
 * Convert an actdat structure to text.
 */

cnvact(adp)
struct	actdat	*adp;		/* pointer to action data */
{
	register struct	actlist	*ldp;	/* pointer to an action list */

	itemout(D_ACTION, adp->ac_no);
	ldp = adp->ac_alist;
	if (ldp != NULL) {
		(char *)ldp += glbl.offset;
		while (ldp->al_code != AL_END) {
			switch (ldp->al_code) {

			case AL_CLOSE:
				outitem(A_CLOSE, 0);
				break;

			case AL_FLAGS:
				outitem(D_FLAGS, ldp->al_no);
				break;

			case AL_SUBST:
				outitem(D_SUBST, ldp->al_no);
				break;

			case AL_BG:
				outitem(D_BGROUND, ldp->al_no);
				break;

			case AL_PNT:
				outitem(D_POINTER, ldp->al_no);
				break;

			case AL_RESTORE:
				outitem(A_RESTORE, 0);
				break;

			case AL_INVERSE:
				outitem(A_INVERSE, 0);
				break;

			case AL_TOP:
				outitem(A_TOP, 0);
				break;

			case AL_TURN:
				outitem(A_TURN, 0);
				break;

			case AL_NORMAL:
				outitem(A_NORMAL, 0);
				break;

			case AL_LOGOUT:
				outitem(A_LOGOUT, 0);
				break;

			case AL_TERM:
				outitem(D_TERM, ldp->al_no);
				break;

			case AL_MENU:
				outitem(D_MENU, ldp->al_no);
				break;
			}
			ldp++;		/* next action in list */
		}
	}
	twonl();

	return;
}	/* end of cnvact() */



/*
 * Convert a menudat structure to text.
 */

cnvmenu(mdp)
register struct	menudat	*mdp;	/* pointer to menu data */
{
	register struct	actlist	*ldp;	/* pointer to list of strings and choices */
	register	char	*s;	/* item name */

	itemout(D_MENU, mdp->mu_no);
	if (mdp->mu_win != 0) {
		outitem(D_WINDOW, mdp->mu_win);
	}
	if (mdp->mu_hd != 0) {
		outitem(D_HEADER, mdp->mu_hd);
	}
	if (mdp->mu_pnt != 0) {
		outitem(D_POINTER, mdp->mu_pnt);
	}
	if (mdp->mu_act != 0) {
		outitem(D_ACTION, mdp->mu_act);
	}
	ldp = mdp->mu_list;
	if (ldp != NULL) {
		(char *)ldp += glbl.offset;
		while (ldp->al_code != AL_END) {

			switch (ldp->al_code) {

			case AL_STRING:
				s = D_STRING;
				break;

			case AL_PSTRING:
				s = D_PSTRING;
				break;

			case AL_LSTRING:
				s = D_LSTRING;
				break;

			case AL_CHOICE:
				s = D_CHOICE;
				break;
			}

			outitem(s, (ldp++)->al_no);
		}
	}

	twonl();

	return;
}	/* end of cnvmenu() */



/*
 * Convert a termdat structure to text.
 */

cnvterm(tdp)
register struct	termdat	*tdp;	/* pointer to terminal window data */
{
	register struct	actlist	*ldp;	/* pointer to strings and icons */
	register	char	*s;	/* item name */

	itemout(D_TERM, tdp->td_no);
	if (tdp->td_win != 0) {
		outitem(D_WINDOW, tdp->td_win);
	}
	if (tdp->td_head != 0) {
		outitem(D_HEADER, tdp->td_head);
	}
	if (tdp->td_zoom != 0) {
		outitem(D_ZOOM, tdp->td_zoom);
	}
	if (tdp->td_mp != 0) {
		outitem(D_POINTER, tdp->td_mp);
	}
	ldp = tdp->td_list;
	if (ldp != NULL) {
		(char *)ldp += glbl.offset;
		while (ldp->al_code != AL_END) {
			switch (ldp->al_code) {

			case AL_STRING:
				s = D_STRING;
				break;

			case AL_PSTRING:
				s = D_PSTRING;
				break;

			case AL_LSTRING:
				s = D_LSTRING;
				break;

			case AL_ICON:
				s = D_ICON;
				break;

			case AL_ENV:
				s = D_ENVIRON;
				break;

			case AL_PENV:
				s = D_PENVIRON;
				break;

			case AL_LENV:
				s = D_LENVIRON;
				break;
			}

			outitem(s, (ldp++)->al_no);
		}
	}

	if (tdp->td_dir != 0) {
		outitem(D_DIR, tdp->td_dir);
	}
	if (tdp->td_flags & TD_SUPER) {
		outitem(A_SUPER, 0);
	}
	if (tdp->td_flags & TD_WAIT) {
		outitem(A_WAIT, 0);
	}
	if (tdp->td_cmd != 0) {
		outitem(D_CMD, tdp->td_cmd);
	}
	twonl();

	return;
}	/* end of cnvterm() */
