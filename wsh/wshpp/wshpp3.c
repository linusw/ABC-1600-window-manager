/* wshpp3.c */

/*
 * This file contains source code for the window shell preprocessor - wshpp.
 */

/* 1985-03-04, Peter Andersson, Luxor Datorer AB */


/*
 * Functions in this file:
 *	scanargs()
 *	getdtype()
 *	xoutput()
 *	multchk()
 *	winfind()
 *	headfind()
 *	iconfind()
 *	flagfind()
 *	zoomfind()
 *	pntfind()
 *	subfind()
 *	bgfind()
 *	strfind()
 *	dirfind()
 *	cmdfind()
 *	menufind()
 *	chofind()
 *	actfind()
 *	termfind()
 *	referchk()
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
extern	char		*argitems[];
extern	char		*messages[];



/*
 * Scan and convert the command arguments.
 */

scanargs(cpp)
register char	**cpp;		/* pointer to command argument list */
{
	register struct	globals	*glblp;		/* pointer to global variables */
	register	char	*s;		/* pointer to a command argument */
	register struct	xdata	*xdp;		/* pointer to output files data */
	char			*allocmem();

	glblp = &glbl;
	while ((s = *++cpp) != NULL) {
		if (*s != '-') {
			glblp->infile = s;
		}
		else {
			while (*++s != '\0') {
				if (*s == 'n') {
					glblp->nopt = YES;
				}
				else if (*s == 'o' && *(s + 1) == '\0') {
					if ((glblp->outfile = *++cpp) == NULL) {
						errexit(M_FNM);	/* output file name missing */
					}
				}
				else if (*s != 'x' || *(s + 1) != '\0') {
					errexit(M_ILLOPT);	/* illegal option */
				}
				else {
					if (*++cpp == NULL) {
						errexit(M_DTM);	/* data type missing */
					}
					xdp = (struct xdata *)allocmem(sizeof(struct xdata));
					if (glblp->xstart == NULL) {
						glblp->xstart = xdp;	/* first item */
					}
					if (getdtype(*cpp, xdp) != OK) {
						errexit(M_IDT);	/* illegal data type */
					}
					if ((xdp->x_fname = *++cpp) == NULL) {
						errexit(M_FNM);	/* file name missing */
					}
				}
			}
		}
	}

	if (xdp != NULL) {

		/*
		 * Allocate space for the terminating element.
		 */
		xdp = (struct xdata *)allocmem(sizeof(struct xdata));
		/* xdp->f_name = NULL; */
	}

	return;
}	/* end of scanargs() */



/*
 * Get a datatype specified in the command line. Returns OK if it is a valid
 * data type, else ERROR.
 */

int
getdtype(s, xdp)
char			*s;		/* command arg to investigate */
register struct	xdata	*xdp;		/* to obtain result */
{
	register char	*cp;
	register	i;		/* index */
	register	len;		/* length of item name */

	i = 0;
	while ((cp = argitems[i]) != NULL) {
		len = strlen(cp);
		if (strncmp(s, cp, len) == 0) {
			cp = s + len;
			xdp->x_type = i;
			if (*cp < '0' || *cp > '9') {
				break;			/* error */
			}
			else {
				while (*cp >= '0' && *cp <= '9') {
					xdp->x_no = 10 * xdp->x_no + *cp++ - '0';
				}
				if (*cp == '\0' && xdp->x_no > 0) {
					return(OK);
				}
				else {
					break;		/* error */
				}
			}
		}

		i++;			/* try next name */
	}

	return(ERROR);
}	/* end of getdtype() */



/*
 * Output all the single structures specified in the command line.
 */

xoutput(xdp)
register struct	xdata	*xdp;		/* data of items to output */
{
	register struct	globals	*glblp;		/* pointer to global variables */
	register	char	*s;		/* pointer to various things */
	register		size;		/* size of item to output */
	register		fd;		/* file descriptor */
	struct		windat	*winfind();
	struct		icondat	*iconfind();
	struct		mpdat	*pntfind();
	struct		headdat	*headfind();
	struct		flagdat	*flagfind();
	struct		subdat	*subfind();
	struct		bgdat	*bgfind();
	struct		zoomdat	*zoomfind();

	glblp = &glbl;

	while (xdp->x_fname != NULL) {
		s = NULL;

		switch (xdp->x_type) {

		case ARG_WIN:		/* a window data structure */
			(struct windat *)s = winfind(xdp->x_no);
			if (s != NULL) {
				(struct winstruc *)s = &((struct windat *)s)->wd_win;
				size = sizeof(struct winstruc);
			}
			break;

		case ARG_ICON:		/* an icon data structure */
			(struct icondat *)s = iconfind(xdp->x_no);
			if (s != NULL) {
				(struct winicon *)s = &((struct icondat *)s)->ic_icon;
				size = sizeof(struct winicon);
			}
			break;

		case ARG_PNT:		/* a mouse pointer data structure */
			(struct mpdat *)s = pntfind(xdp->x_no);
			if (s != NULL) {
				(struct npstruc *)s = &((struct mpdat *)s)->mp_mp;
				size = sizeof(struct npstruc);
			}
			break;

		case ARG_HEAD:
			(struct headdat *)s = headfind(xdp->x_no);
			if (s != NULL) {
				(struct headstruc *)s = &((struct headdat *)s)->hd_head;
				size = sizeof(struct headstruc);
			}
			break;

		case ARG_FLG:		/* flags data structure */
			(struct flagdat *)s = flagfind(xdp->x_no);
			if (s != NULL) {
				(struct flgstruc *)s = &((struct flagdat *)s)->fl_flags;
				size = sizeof(struct flgstruc);
			}
			break;

		case ARG_ZOOM:		/* zoom list data structure */
			(struct zoomdat *)s = zoomfind(xdp->x_no);
			if (s != NULL) {
				(struct zoomlst *)s = &((struct zoomdat *)s)->zm_zoom;
				size = sizeof(struct zoomlst);
			}
			break;

		case ARG_SUBST:		/* substitute keys data structure */
			(struct subdat *)s = subfind(xdp->x_no);
			if (s != NULL) {
				(struct substit *)s = &((struct subdat *)s)->sk_subst;
				size = sizeof(struct substit);
			}
			break;

		case ARG_BG:		/* background data structure */
			(struct bgdat *)s = bgfind(xdp->x_no);
			if (s != NULL) {
				(struct chbgstruc *)s = &((struct bgdat *)s)->bg_bgd;
				size = sizeof(struct chbgstruc);
			}
			break;
		}

		/*
		 * Save the structure if it was present.
		 */
		if (s == NULL) {
			itemerr(argitems[xdp->x_type], xdp->x_no, M_NOTDEF);
		}
		else {
			if (strcmp(xdp->x_fname, "-") == 0) {
				fd = STDOUT;
			}
			else if ((fd = creat(xdp->x_fname, PERMISSION)) == -1) {
				errname(M_FTCF, xdp->x_fname);	/* failed to create */
			}
			if (fd >= 0) {
				if (write(fd, s, size) != size) {
					if (fd != STDOUT) {
						close(fd);	/* failed to write */
						unlink(xdp->x_fname);
					}
					errname(M_FTWF, xdp->x_fname);
				}
				else if (fd != STDOUT) {
					msgname(M_SAVFIL, xdp->x_fname);
				}
			}
		}

		/*
		 * The next structure to write out.
		 */
		xdp++;
	}

	return;
}	/* end of xoutput() */



/*
 * Check that there are not any multiple defined things.
 */

multchk()
{
	register struct	globals	*glblp;		/* pointer to global variables */
	register	char	*s1;		/* pointer to various things */
	register	char	*s2;		/* pointer to various things */

	glblp = &glbl;

	/*
	 * Check the windows.
	 */
	(struct windat *)s1 = glblp->wdstart;
	while (s1 != NULL) {
		s2 = s1;
		while (((struct windat *)s2 = ((struct windat *)s2)->wd_next) != NULL) {
			if (((struct windat *)s1)->wd_no == ((struct windat *)s2)->wd_no) {
				multerr(D_WINDOW, ((struct windat *)s1)->wd_no);
				break;
			}
		}
		(struct windat *)s1 = ((struct windat *)s1)->wd_next;
	}

	/*
	 * Check the window headers.
	 */
	(struct headdat *)s1 = glblp->hdstart;
	while (s1 != NULL) {
		s2 = s1;
		while (((struct headdat *)s2 = ((struct headdat *)s2)->hd_next) != NULL) {
			if (((struct headdat *)s1)->hd_no == ((struct headdat *)s2)->hd_no) {
				multerr(D_HEADER, ((struct headdat *)s1)->hd_no);
				break;
			}
		}
		(struct headdat *)s1 = ((struct headdat *)s1)->hd_next;
	}

	/*
	 * Check the icons.
	 */
	(struct icondat *)s1 = glblp->icstart;
	while (s1 != NULL) {
		s2 = s1;
		while (((struct icondat *)s2 = ((struct icondat *)s2)->ic_next) != NULL) {
			if (((struct icondat *)s1)->ic_no == ((struct icondat *)s2)->ic_no) {
				multerr(D_ICON, ((struct icondat *)s1)->ic_no);
				break;
			}
		}
		(struct icondat *)s1 = ((struct icondat *)s1)->ic_next;
	}

	/*
	 * Check the flags.
	 */
	(struct flagdat *)s1 = glblp->flgstart;
	while (s1 != NULL) {
		s2 = s1;
		while (((struct flagdat *)s2 = ((struct flagdat *)s2)->fl_next) != NULL) {
			if (((struct flagdat *)s1)->fl_no == ((struct flagdat *)s2)->fl_no) {
				multerr(D_FLAGS, ((struct flagdat *)s1)->fl_no);
				break;
			}
		}
		(struct flagdat *)s1 = ((struct flagdat *)s1)->fl_next;
	}

	/*
	 * Check the zoom lists.
	 */
	(struct zoomdat *)s1 = glblp->zoomstart;
	while (s1 != NULL) {
		s2 = s1;
		while (((struct zoomdat *)s2 = ((struct zoomdat*)s2)->zm_next) != NULL) {
			if (((struct zoomdat *)s1)->zm_no == ((struct zoomdat *)s2)->zm_no) {
				multerr(D_ZOOM, ((struct zoomdat *)s1)->zm_no);
				break;
			}
		}
		(struct zoomdat *)s1 = ((struct zoomdat *)s1)->zm_next;
	}

	/*
	 * Check the mouse pointers.
	 */
	(struct mpdat *)s1 = glblp->mpstart;
	while (s1 != NULL) {
		s2 = s1;
		while (((struct mpdat *)s2 = ((struct mpdat *)s2)->mp_next) != NULL) {
			if (((struct mpdat *)s1)->mp_no == ((struct mpdat *)s2)->mp_no) {
				multerr(D_POINTER, ((struct mpdat *)s1)->mp_no);
				break;
			}
		}
		(struct mpdat *)s1 = ((struct mpdat *)s1)->mp_next;
	}

	/*
	 * Check the mouse substitute keys.
	 */
	(struct subdat *)s1 = glblp->substart;
	while (s1 != NULL) {
		s2 = s1;
		while (((struct subdat *)s2 = ((struct subdat *)s2)->sk_next) != NULL) {
			if (((struct subdat *)s1)->sk_no == ((struct subdat *)s2)->sk_no) {
				multerr(D_SUBST, ((struct subdat *)s1)->sk_no);
				break;
			}
		}
		(struct subdat *)s1 = ((struct subdat *)s1)->sk_next;
	}

	/*
	 * Check the background patterns.
	 */
	(struct bgdat *)s1 = glblp->bgstart;
	while (s1 != NULL) {
		s2 = s1;
		while (((struct bgdat *)s2 = ((struct bgdat *)s2)->bg_next) != NULL) {
			if (((struct bgdat *)s1)->bg_no == ((struct bgdat *)s2)->bg_no) {
				multerr(D_BGROUND, ((struct bgdat *)s1)->bg_no);
				break;
			}
		}
		(struct bgdat *)s1 = ((struct bgdat *)s1)->bg_next;
	}

	/*
	 * Check the strings.
	 */
	(struct strdat *)s1 = glblp->strstart;
	while (s1 != NULL) {
		s2 = s1;
		while (((struct strdat *)s2 = ((struct strdat *)s2)->st_next) != NULL) {
			if (((struct strdat *)s1)->st_no == ((struct strdat *)s2)->st_no) {
				multerr(D_STRING, ((struct strdat *)s1)->st_no);
				break;
			}
		}
		(struct strdat *)s1 = ((struct strdat *)s1)->st_next;
	}

	/*
	 * Check the portrait strings.
	 */
	(struct strdat *)s1 = glblp->pstrstart;
	while (s1 != NULL) {
		s2 = s1;
		while (((struct strdat *)s2 = ((struct strdat *)s2)->st_next) != NULL) {
			if (((struct strdat *)s1)->st_no == ((struct strdat *)s2)->st_no) {
				multerr(D_PSTRING, ((struct strdat *)s1)->st_no);
				break;
			}
		}
		(struct strdat *)s1 = ((struct strdat *)s1)->st_next;
	}

	/*
	 * The landscape strings.
	 */
	(struct strdat *)s1 = glblp->lstrstart;
	while (s1 != NULL) {
		s2 = s1;
		while (((struct strdat *)s2 = ((struct strdat *)s2)->st_next) != NULL) {
			if (((struct strdat *)s1)->st_no == ((struct strdat *)s2)->st_no) {
				multerr(D_LSTRING, ((struct strdat *)s1)->st_no);
				break;
			}
		}
		(struct strdat *)s1 = ((struct strdat *)s1)->st_next;
	}

	/*
	 * Check the directories.
	 */
	(struct strdat *)s1 = glblp->dirstart;
	while (s1 != NULL) {
		s2 = s1;
		while (((struct strdat *)s2 = ((struct strdat *)s2)->st_next) != NULL) {
			if (((struct strdat *)s1)->st_no == ((struct strdat *)s2)->st_no) {
				multerr(D_DIR, ((struct strdat *)s1)->st_no);
				break;
			}
		}
		(struct strdat *)s1 = ((struct strdat *)s1)->st_next;
	}

	/*
	 * Check the environments.
	 */
	(struct arrdat *)s1 = glblp->envstart;
	while (s1 != NULL) {
		s2 = s1;
		while (((struct arrdat *)s2 = ((struct arrdat *)s2)->ca_next) != NULL) {
			if (((struct arrdat *)s1)->ca_no == ((struct arrdat *)s2)->ca_no) {
				multerr(D_ENVIRON, ((struct arrdat *)s1)->ca_no);
				break;
			}
		}
		(struct arrdat *)s1 = ((struct arrdat *)s1)->ca_next;
	}

	/*
	 * Check the portrait mode environments.
	 */
	(struct arrdat *)s1 = glblp->penvstart;
	while (s1 != NULL) {
		s2 = s1;
		while (((struct arrdat *)s2 = ((struct arrdat *)s2)->ca_next) != NULL) {
			if (((struct arrdat *)s1)->ca_no == ((struct arrdat *)s2)->ca_no) {
				multerr(D_PENVIRON, ((struct arrdat *)s1)->ca_no);
				break;
			}
		}
		(struct arrdat *)s1 = ((struct arrdat *)s1)->ca_next;
	}

	/*
	 * Check the landscape mode environments.
	 */
	(struct arrdat *)s1 = glblp->lenvstart;
	while (s1 != NULL) {
		s2 = s1;
		while (((struct arrdat *)s2 = ((struct arrdat *)s2)->ca_next) != NULL) {
			if (((struct arrdat *)s1)->ca_no == ((struct arrdat *)s2)->ca_no) {
				multerr(D_LENVIRON, ((struct arrdat *)s1)->ca_no);
				break;
			}
		}
		(struct arrdat *)s1 = ((struct arrdat *)s1)->ca_next;
	}

	/*
	 * Check the commands.
	 */
	(struct arrdat *)s1 = glblp->cmdstart;
	while (s1 != NULL) {
		s2 = s1;
		while (((struct arrdat *)s2 = ((struct arrdat *)s2)->ca_next) != NULL) {
			if (((struct arrdat *)s1)->ca_no == ((struct arrdat *)s2)->ca_no) {
				multerr(D_CMD, ((struct arrdat *)s1)->ca_no);
				break;
			}
		}
		(struct arrdat *)s1 = ((struct arrdat *)s1)->ca_next;
	}

	/*
	 * Check the menu's.
	 */
	(struct menudat *)s1 = glblp->menustart;
	while (s1 != NULL) {
		s2 = s1;
		while (((struct menudat *)s2 = ((struct menudat *)s2)->mu_next) != NULL) {
			if (((struct menudat *)s1)->mu_no == ((struct menudat *)s2)->mu_no) {
				multerr(D_MENU, ((struct menudat *)s1)->mu_no);
				break;
			}
		}
		(struct menudat *)s1 = ((struct menudat *)s1)->mu_next;
	}

	/*
	 * Check the choices.
	 */
	(struct chodat *)s1 = glblp->chostart;
	while (s1 != NULL) {
		s2 = s1;
		while (((struct chodat *)s2 = ((struct chodat *)s2)->co_next) != NULL) {
			if (((struct chodat *)s1)->co_no == ((struct chodat *)s2)->co_no) {
				multerr(D_CHOICE, ((struct chodat *)s1)->co_no);
				break;
			}
		}
		(struct chodat *)s1 = ((struct chodat *)s1)->co_next;
	}

	/*
	 * Check the actions.
	 */
	(struct actdat *)s1 = glblp->actstart;
	while (s1 != NULL) {
		s2 = s1;
		while (((struct actdat *)s2 = ((struct actdat *)s2)->ac_next) != NULL) {
			if (((struct actdat *)s1)->ac_no == ((struct actdat *)s2)->ac_no) {
				multerr(D_ACTION, ((struct actdat *)s1)->ac_no);
				break;
			}
		}
		(struct actdat *)s1 = ((struct actdat *)s1)->ac_next;
	}

	/*
	 * Check the terminal windows.
	 */
	(struct termdat *)s1 = glblp->termstart;
	while (s1 != NULL) {
		s2 = s1;
		while (((struct termdat *)s2 = ((struct termdat *)s2)->td_next) != NULL) {
			if (((struct termdat *)s1)->td_no == ((struct termdat *)s2)->td_no) {
				multerr(D_TERM, ((struct termdat *)s1)->td_no);
				break;
			}
		}
		(struct termdat *)s1 = ((struct termdat *)s1)->td_next;
	}

	return;
}	/* end of multchk() */



/*
 * Find a window data structure, given the number. Returns a pointer to the
 * structure or NULL if the structure is not present.
 */

struct windat *
winfind(n)
int	n;		/* number of window to find */
{
	register struct	windat	*wdp;	/* pointer to window data */

	wdp = glbl.wdstart;
	while (wdp != NULL) {
		if (wdp->wd_no == n) {
			break;		/* found window */
		}
		wdp = wdp->wd_next;
	}

	return(wdp);
}	/* end of winfind() */



/*
 * Find a header data structure given the number. Returns a pointer to the
 * structure or NULL if the structure is not present.
 */

struct headdat *
headfind(n)
int	n;
{
	register struct	headdat	*hdp;

	hdp = glbl.hdstart;
	while (hdp != NULL) {
		if (hdp->hd_no == n) {
			break;
		}
		hdp = hdp->hd_next;
	}

	return(hdp);
}	/* end of headfind() */



/*
 * Find an icon data structure given the number. Returns a pointer to the
 * structure or NULL if the structure is not present.
 */

struct icondat *
iconfind(n)
int	n;
{
	register struct	icondat	*idp;

	idp = glbl.icstart;
	while (idp != NULL) {
		if (idp->ic_no == n) {
			break;
		}
		idp = idp->ic_next;
	}

	return(idp);
}	/* end of iconfind() */



/*
 * Find a flags data structure given the number. Returns a pointer to the
 * structure or NULL if the structure is not present.
 */

struct flagdat *
flagfind(n)
int	n;
{
	register struct	flagdat	*fdp;

	fdp = glbl.flgstart;
	while (fdp != NULL) {
		if (fdp->fl_no == n) {
			break;
		}
		fdp = fdp->fl_next;
	}

	return(fdp);
}	/* end of flagfind() */



/*
 * Find a zoom list data structure.
 */

struct zoomdat *
zoomfind(n)
int	n;
{
	register struct	zoomdat	*zdp;

	zdp = glbl.zoomstart;
	while (zdp != NULL) {
		if (zdp->zm_no == n) {
			break;
		}
		zdp = zdp->zm_next;
	}

	return(zdp);
}	/* end of zoomfind() */



/*
 * Find a pointer data structure.
 */

struct mpdat *
pntfind(n)
int	n;
{
	register struct	mpdat	*pdp;

	pdp = glbl.mpstart;
	while (pdp != NULL) {
		if (pdp->mp_no == n) {
			break;
		}
		pdp = pdp->mp_next;
	}

	return(pdp);
}	/* pntfind() */



/*
 * Find a mouse substitute data structure.
 */

struct subdat *
subfind(n)
int	n;
{
	register struct	subdat	*sdp;

	sdp = glbl.substart;
	while (sdp != NULL) {
		if (sdp->sk_no == n) {
			break;
		}
		sdp = sdp->sk_next;
	}

	return(sdp);
}	/* end of subfind() */



/*
 * Find a background pattern data structure.
 */

struct bgdat *
bgfind(n)
int	n;
{
	register struct	bgdat	*bdp;

	bdp = glbl.bgstart;
	while (bdp != NULL) {
		if (bdp->bg_no == n) {
			break;
		}
		bdp = bdp->bg_next;
	}

	return(bdp);
}	/* end of bgfind() */



/*
 * Find a string data structure.
 */

struct strdat *
strfind(n, sdp)
int			n;	/* the number of the string */
register struct	strdat	*sdp;	/* pointer to the start of the linked list */
{
	while (sdp != NULL) {
		if (sdp->st_no == n) {
			break;
		}
		sdp = sdp->st_next;
	}

	return(sdp);
}	/* end of strfind() */



/*
 * Find a directory data structure.
 */

struct strdat *
dirfind(n)
int	n;
{
	register struct	strdat	*ddp;

	ddp = glbl.dirstart;
	while (ddp != NULL) {
		if (ddp->st_no == n) {
			break;
		}
		ddp = ddp->st_next;
	}

	return(ddp);
}	/* end of dirfind() */



/*
 * Find a command data structure.
 */

struct arrdat *
cmdfind(n, cdp)
int			n;
register struct	arrdat	*cdp;
{
	while (cdp != NULL) {
		if (cdp->ca_no == n) {
			break;
		}
		cdp = cdp->ca_next;
	}

	return(cdp);
}	/* end of cmdfind() */



/*
 * Find a menu data structure.
 */

struct menudat *
menufind(n)
int	n;
{
	register struct	menudat	*mdp;

	mdp = glbl.menustart;
	while (mdp != NULL) {
		if (mdp->mu_no == n) {
			break;
		}
		mdp = mdp->mu_next;
	}

	return(mdp);
}	/* end of menufind() */



/*
 * Find a choice data structure.
 */

struct chodat *
chofind(n)
int	n;
{
	register struct	chodat	*cdp;

	cdp = glbl.chostart;
	while (cdp != NULL) {
		if (cdp->co_no == n) {
			break;
		}
		cdp = cdp->co_next;
	}

	return(cdp);
}	/* end of chofind() */



/*
 * Find an action data structure.
 */

struct actdat *
actfind(n)
int	n;
{
	register struct	actdat	*adp;

	adp = glbl.actstart;
	while (adp != NULL) {
		if (adp->ac_no == n) {
			break;
		}
		adp = adp->ac_next;
	}

	return(adp);
}	/* end of actfind() */



/*
 * Find a terminal data structure.
 */

struct termdat *
termfind(n)
int	n;
{
	register struct	termdat	*tdp;

	tdp = glbl.termstart;
	while (tdp != NULL) {
		if (tdp->td_no == n) {
			break;
		}
		tdp = tdp->td_next;
	}

	return(tdp);
}	/* end of termfind() */



/*
 * Check that all necessary and refered items does exist.
 * If not, the global error flag - errflag - is set.
 */

referchk()
{
	register struct	globals	*glblp;		/* pointer to global variables */
	register	char	*p;		/* pointer to various things */
	register struct	actlist	*ldp;		/* pointer to list with actions */
	register		n;		/* item number */
	register		m;		/* item number */
	register		cnt;		/* to count choices */
	register		menus;		/* to count menus */
	struct		termdat	*termfind();
	struct		flagdat	*flagfind();
	struct		arrdat	*cmdfind();
	struct		strdat	*dirfind();
	struct		mpdat	*pntfind();
	struct		menudat	*menufind();
	struct		windat	*winfind();
	struct		strdat	*strfind();
	struct		chodat	*chofind();
	struct		icondat	*iconfind();
	struct		actdat	*actfind();
	struct		headdat	*headfind();
	struct		subdat	*subfind();
	struct		bgdat	*bgfind();
	struct		zoomdat	*zoomfind();

	glblp = &glbl;

	/*
	 * Check the init data.
	 */
	if ((ldp = glblp->init) == NULL) {
		errmsg(M_MINIT);
	}
	else {

		/*
		 * Check the initialization data list.
		 */
		menus = 0;
		while (ldp->al_code != AL_END) {
			n = ldp->al_no;
			switch (ldp->al_code) {

			case AL_SUBST:
				if (subfind(n) == NULL) {
					notdefmsg(D_SUBST, n, D_INIT, 0);
				}
				break;

			case AL_PNT:
				if (pntfind(n) == NULL) {
					notdefmsg(D_POINTER, n, D_INIT, 0);
				}
				break;

			case AL_BG:
				if (bgfind(n) == NULL) {
					notdefmsg(D_BGROUND, n, D_INIT, 0);
				}
				break;

			case AL_TERM:
				if (termfind(n) == NULL) {
					notdefmsg(D_TERM, n, D_INIT, 0);
				}
				break;

			case AL_MENU:
				menus++;
				if (menufind(n) == NULL) {
					notdefmsg(D_MENU, n, D_INIT, 0);
				}
				if ((ldp + 1)->al_code != AL_END) {
					actwarn(M_ACT1, 0);
				}
				break;
			}

			ldp++;		/* next element in list */
		}

		if (menus == 0) {
			actwarn(M_ACT2, 0);
			glblp->errflag = YES;
		}
		else if (menus > 1) {
			actwarn(M_ACT3, 0);
		}
	}

	/*
	 * Check all the menu descriptions.
	 */
	p = (char *)glblp->menustart;
	while (p != NULL) {
		m = ((struct menudat *)p)->mu_no;
		if ((n = ((struct menudat *)p)->mu_win) == 0) {
			nspecmsg(D_WINDOW, D_MENU, m);
		}
		else if (winfind(n) == NULL) {
			notdefmsg(D_WINDOW, n, D_MENU, m);
		}
		if ((n = ((struct menudat *)p)->mu_hd) != 0 && headfind(n) == NULL) {
			notdefmsg(D_HEADER, n, D_MENU, m);
		}
		if ((n = ((struct menudat *)p)->mu_pnt) != 0 && pntfind(n) == NULL) {
			notdefmsg(D_POINTER, n, D_MENU, m);
		}
		if ((n = ((struct menudat *)p)->mu_act) != 0 && actfind(n) == NULL) {
			notdefmsg(D_ACTION, n, D_MENU, m);
		}
		cnt = 0;
		ldp = ((struct menudat *)p)->mu_list;
		while (ldp->al_code != AL_END) {
			n = ldp->al_no;
			switch (ldp->al_code) {

			case AL_STRING:
				if (strfind(n, glblp->strstart) == NULL) {
					notdefmsg(D_STRING, n, D_MENU, m);
				}
				break;

			case AL_PSTRING:
				if (strfind(n, glblp->pstrstart) == NULL) {
					notdefmsg(D_PSTRING, n, D_MENU, m);
				}
				break;

			case AL_LSTRING:
				if (strfind(n, glblp->lstrstart) == NULL) {
					notdefmsg(D_LSTRING, n, D_MENU, m);
				}
				break;

			case AL_CHOICE:
				if (chofind(n) == NULL) {
					notdefmsg(D_CHOICE, n, D_MENU, m);
				}
				cnt++;
				break;
			}

			ldp++;		/* next element in list */
		}

		if (cnt == 0 && ((struct menudat *)p)->mu_act == 0) {
			nspecmsg(D_CHOICE, D_MENU, m);	/* no choice */
		}
		else if (cnt != 0 && ((struct menudat *)p)->mu_act != 0) {
			if (glblp->infile != NULL) {
				writestr(glblp->infile);
				writestr(": ");
			}
			writestr(messages[2*M_BACMU+glblp->language]);
			writestr(D_MENU);
			writestr(inttoasc(m));
			writestr("'\n");
		}
		p = (char *)((struct menudat *)p)->mu_next;
	}

	/*
	 * Check all the choice descriptions.
	 */
	p = (char *)glblp->chostart;
	while (p != NULL) {
		m = ((struct chodat *)p)->co_no;
		if ((n = ((struct chodat *)p)->co_icon) == 0) {
			nspecmsg(D_ICON, D_CHOICE, m);
		}
		else if (iconfind(n) == NULL) {
			notdefmsg(D_ICON, n, D_CHOICE, m);
		}
		if ((n = ((struct chodat *)p)->co_action) == 0) {
			nspecmsg(D_ACTION, D_CHOICE, m);
		}
		else if (actfind(n) == NULL) {
			notdefmsg(D_ACTION, n, D_CHOICE, m);
		}
		p = (char *)((struct chodat *)p)->co_next;
	}

	/*
	 * Check all the terminals.
	 */
	p = (char *)glblp->termstart;
	while (p != NULL) {
		m = ((struct termdat *)p)->td_no;
		if ((n = ((struct termdat *)p)->td_win) != 0 && winfind(n) == NULL) {
			notdefmsg(D_WINDOW, n, D_TERM, m);
		}
		if ((n = ((struct termdat *)p)->td_head) != 0 && headfind(n) == NULL) {
			notdefmsg(D_HEADER, n, D_TERM, m);
		}
		if ((n = ((struct termdat *)p)->td_zoom) != 0 && zoomfind(n) == NULL) {
			notdefmsg(D_ZOOM, n, D_TERM, m);
		}
		if ((n = ((struct termdat *)p)->td_mp) != 0 && pntfind(n) == NULL) {
			notdefmsg(D_POINTER, n, D_TERM, m);
		}
		ldp = ((struct termdat *)p)->td_list;

		while (ldp->al_code != AL_END) {
			n = ldp->al_no;
			switch (ldp->al_code) {

			case AL_STRING:
				if (strfind(n, glblp->strstart) == NULL) {
					notdefmsg(D_STRING, n, D_TERM, m);
				}
				break;

			case AL_PSTRING:
				if (strfind(n, glblp->pstrstart) == NULL) {
					notdefmsg(D_PSTRING, n, D_TERM, m);
				}
				break;

			case AL_LSTRING:
				if (strfind(n, glblp->lstrstart) == NULL) {
					notdefmsg(D_LSTRING, n, D_TERM, m);
				}
				break;

			case AL_ICON:
				if (iconfind(n) == NULL) {
					notdefmsg(D_ICON, n, D_TERM, m);
				}
				break;

			case AL_ENV:
				if (cmdfind(n, glblp->envstart) == NULL) {
					notdefmsg(D_ENVIRON, n, D_TERM, m);
				}
				break;

			case AL_PENV:
				if (cmdfind(n, glblp->penvstart) == NULL) {
					notdefmsg(D_PENVIRON, n, D_TERM, m);
				}
				break;

			case AL_LENV:
				if (cmdfind(n, glblp->lenvstart) == NULL) {
					notdefmsg(D_LENVIRON, n, D_TERM, m);
				}
				break;
			}

			ldp++;		/* next element in list */
		}

		if ((n = ((struct termdat *)p)->td_dir) != 0 && dirfind(n) == NULL) {
			notdefmsg(D_DIR, n, D_TERM, m);
		}
		if ((n = ((struct termdat *)p)->td_cmd) == 0) {
			nspecmsg(D_CMD, D_TERM, m);
		}
		else if (cmdfind(n, glblp->cmdstart) == NULL) {
			notdefmsg(D_CMD, n, D_TERM, m);
		}

		p = (char *)((struct termdat *)p)->td_next;
	}

	/*
	 * Check all the actions.
	 */
	p = (char *)glblp->actstart;
	while (p != NULL) {
		menus = 0;
		m = ((struct actdat *)p)->ac_no;
		ldp = ((struct actdat *)p)->ac_alist;
		while (ldp->al_code != AL_END) {
			n = ldp->al_no;
			switch (ldp->al_code) {

			case AL_FLAGS:
				if (flagfind(n) == NULL) {
					notdefmsg(D_FLAGS, n, D_ACTION, m);
				}
				break;

			case AL_SUBST:
				if (subfind(n) == NULL) {
					notdefmsg(D_SUBST, n, D_ACTION, m);
				}
				break;

			case AL_BG:
				if (bgfind(n) == NULL) {
					notdefmsg(D_BGROUND, n, D_ACTION, m);
				}
				break;

			case AL_PNT:
				if (pntfind(n) == NULL) {
					notdefmsg(D_POINTER, n, D_ACTION, m);
				}
				break;

			case AL_TERM:
				if (termfind(n) == NULL) {
					notdefmsg(D_TERM, n, D_ACTION, m);
				}
				break;

			case AL_MENU:
				menus++;
				if (menufind(n) == NULL) {
					notdefmsg(D_MENU, n, D_ACTION, m);
				}
				if ((ldp + 1)->al_code != AL_END) {
					actwarn(M_ACT1, m);
				}
				break;
			}

			ldp++;		/* next element in list */
		}

		if (menus == 0) {
			actwarn(M_ACT2, m);
			glblp->errflag = YES;
		}
		else if (menus > 1) {
			actwarn(M_ACT3, m);
		}

		p = (char *)((struct actdat *)p)->ac_next;
	}

	return;
}	/* end of referchk() */
