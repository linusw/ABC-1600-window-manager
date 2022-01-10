/* wshpp1.c */

/*
 * This file contains the source code for the window shell preprocessor -
 * wshpp.
 */

/* 1985-02-18, Peter Andersson, Luxor Datorer AB */

/*
 * Functions in this file:
 *	main()
 *	errexit()
 *	uneof()
 *	illsyntax()
 *	dsperr()
 *	errname()
 *	msgname()
 *	multerr()
 *	itemerr()
 *	errmsg()
 *	notdefmsg()
 *	nspecmsg()
 *	actwarn()
 *	writestr()
 *	allocmem()
 *	inttoasc()
 *	asctoint()
 *	nextitem()
 *	nextact()
 *	nexttoken()
 *	kwcollect()
 *	kwcmp()
 *	getval()
 *	getdec()
 *	getoct()
 *	gethex()
 *	getlstr()
 *	esccnv()
 *	ckifok()
 *	getstr()
 *	getmstr()
 *	getmword()
 *	getmdword()
 */



#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	"../h/w_language.h"
#include	"../h/w_wsh.h"
#include	"wshpp_defs.h"
#include	"wshpp_glbl.h"
#include	"wshpp_msg.h"


/*
 * Global variables.
 */

struct	globals	glbl;


main(argc, argv)
int	argc;		/* argument count */
char	**argv;		/* argument list */
{
	register	char	*s;		/* various uses */
	register	char	**argp;		/* to adjust cmd arg array */
	register struct	globals	*glblp;		/* global variables */
	register		item;		/* type of item */
	register	long	diff;		/* to adjust pointers */
	char			*allocmem();	/* memory allocator */
	char			*getenv();

	glblp = &glbl;

	/*
	 * Initialize all global variables.
	 * Here it is used that all the global variables are initialized to
	 * zeros.
	 */
	/* glblp->inputfd = STDIN; */
	/* glblp->infile = NULL; */
	/* glblp->outfile = NULL; */
	/* glblp->nopt = NO; */
	/* glblp->xstart = NULL; */
	/* glblp->errflag = NO; */
	/* glblp->wdstart = NULL; */
	/* glblp->wdcur = NULL; */
	/* glblp->hdstart = NULL; */
	/* glblp->hdcur = NULL; */
	/* glblp->icstart = NULL; */
	/* glblp->iccur = NULL; */
	/* glblp->flgstart = NULL; */
	/* glblp->flgcur = NULL; */
	/* glblp->zoomstart = NULL; */
	/* glblp->zoomcur = NULL; */
	/* glblp->mpstart = NULL; */
	/* glblp->mpcur = NULL; */
	/* glblp->substart = NULL; */
	/* glblp->subcur = NULL; */
	/* glblp->bgstart = NULL; */
	/* glblp->bgcur = NULL; */
	/* glblp->strstart = NULL; */
	/* glblp->strcur = NULL; */
	/* glblp->pstrstart = NULL; */
	/* glblp->pstrcur = NULL; */
	/* glblp->lstrstart = NULL; */
	/* glblp->lstrcur = NULL; */
	/* glblp->dirstart = NULL; */
	/* glblp->dircur = NULL; */
	/* glblp->envstart = NULL; */
	/* glblp->envcur = NULL; */
	/* glblp->penvstart = NULL; */
	/* glblp->penvcur = NULL; */
	/* glblp->lenvstart = NULL; */
	/* glblp->lenvcur = NULL; */
	/* glblp->cmdstart = NULL; */
	/* glblp->cmdcur = NULL; */
	/* glblp->init = NULL; */
	/* glblp->menustart = NULL; */
	/* glblp->menucur = NULL; */
	/* glblp->chostart = NULL; */
	/* glblp->chocur = NULL; */
	/* glblp->actstart = NULL; */
	/* glblp->actcur = NULL; */
	/* glblp->termstart = NULL; */
	/* glblp->termcur = NULL; */
	/* glblp->filehead = NULL; */
	/* glblp->endmem = NULL; */
	/* glblp->single = NO; */
	glblp->cmdname = *argv;
	glblp->lineno = 1;

	/*
	 * Determine which language to use.
	 */
	if ((s = getenv("LANGUAGE")) != NULL && strcmp(s, "swedish") == 0) {
		glblp->language = SWEDISH;
	}
	else {
		glblp->language = ENGLISH;
	}

	/*
	 * Decode the arguments.
	 */
	if (argc > 1) {
		scanargs(argv);
	}
	if (glblp->outfile == NULL && glblp->xstart == NULL) {
		glblp->outfile = DFLTFILE;	/* use the default output file */
	}

	/*
	 * Open the input file.
	 */
	if (glblp->infile != NULL) {
		if ((glblp->inputfd = open(glblp->infile, 0)) == -1) {
			errexit(M_FOIF);	/* failed to open input file */
		}
	}

	/*
	 * First allocate space for the file header structure.
	 */
	glblp->filehead = (struct wshhead *)allocmem(sizeof(struct wshhead));
	glblp->filehead->wh_magic = MAGIC;

	/*
	 * Process the input file.
	 */
	while ((item = nextitem()) != EOF) {
		switch (item) {
		case WIN_TOK:		/* window data */
			windecod();
			break;

		case ICON_TOK:		/* icon data */
			icondecod();
			break;

		case STR_TOK:		/* a string */
			strdecod();
			break;

		case PSTR_TOK:		/* a portrait mode string */
			pstrdecod();
			break;

		case LSTR_TOK:		/* a landscape mode string */
			lstrdecod();
			break;

		case PNT_TOK:		/* mouse pointer data */
			pntdecod();
			break;

		case HEAD_TOK:		/* window header data */
			headdecod();
			break;

		case DIR_TOK:		/* a pathname */
			dirdecod();
			break;

		case ENV_TOK:		/* environment variables */
			envdecod();
			break;

		case PENV_TOK:		/* portrait mode environment variables */
			penvdecod();
			break;

		case LENV_TOK:		/* landscape mode environment variables */
			lenvdecod();
			break;

		case CMD_TOK:		/* a command */
			cmddecod();
			break;

		case FLG_TOK:		/* flags data */
			flgdecod();
			break;

		case ZOOM_TOK:		/* zoom list data */
			zoomdecod();
			break;

		case SUBST_TOK:		/* mouse substitute keys */
			subdecod();
			break;

		case BG_TOK:		/* background data */
			bgdecod();
			break;

		case TERM_TOK:		/* a terminal window description */
			termdecod();
			break;

		case MENU_TOK:		/* a menu description */
			menudecod();
			break;

		case CHO_TOK:		/* a choice description */
			chodecod();
			break;

		case ACT_TOK:		/* an action description */
			actdecod();
			break;

		case INIT_TOK:		/* actions on initializations */
			initdecod();
			break;

		default:		/* error */
			illsyntax();
			skipeoln();
			break;
		}
	}

	/*
	 * Check that there are not any multiple defined things.
	 */
	if (glblp->errflag == NO) {
		multchk();
	}
	if (glblp->errflag != NO) {
		exit(1);
	}

	/*
	 * Output single structures.
	 */
	if (glblp->xstart != NULL && glblp->nopt == NO) {
		xoutput(glblp->xstart);
	}

	/*
	 * Check that all refered things does exist.
	 */
	if (glblp->outfile != NULL) {
		referchk();
	}
	if (glblp->errflag != NO) {
		exit(1);
	}
	if (glblp->nopt != NO) {
		exit(0);
	}

	if (glblp->outfile != NULL) {

		/*
		 * Adjust all pointers so null will be the start of the file.
		 */
		diff = (long)glblp->filehead;
		if (glblp->wdstart != NULL) {
			(char *)glblp->filehead->wh_wd = (char *)glblp->wdstart - diff;
			while ((s = (char *)glblp->wdstart->wd_next) != NULL) {
				(char *)glblp->wdstart->wd_next -= diff;
				glblp->wdstart = (struct windat *)s;
			}
		}
		if (glblp->hdstart != NULL) {
			(char *)glblp->filehead->wh_hd = (char *)glblp->hdstart - diff;
			while ((s = (char *)glblp->hdstart->hd_next) != NULL) {
				(char *)glblp->hdstart->hd_next -= diff;
				glblp->hdstart = (struct headdat *)s;
			}
		}
		if (glblp->icstart != NULL) {
			(char *)glblp->filehead->wh_ic = (char *)glblp->icstart - diff;
			while ((s = (char *)glblp->icstart->ic_next) != NULL) {
				(char *)glblp->icstart->ic_next -= diff;
				glblp->icstart = (struct icondat *)s;
			}
		}
		if (glblp->flgstart != NULL) {
			(char *)glblp->filehead->wh_flg = (char *)glblp->flgstart - diff;
			while ((s = (char *)glblp->flgstart->fl_next) != NULL) {
				(char *)glblp->flgstart->fl_next -= diff;
				glblp->flgstart = (struct flagdat *)s;
			}
		}
		if (glblp->zoomstart != NULL) {
			(char *)glblp->filehead->wh_zoom = (char *)glblp->zoomstart - diff;
			while ((s = (char *)glblp->zoomstart->zm_next) != NULL) {
				(char *)glblp->zoomstart->zm_next -= diff;
				glblp->zoomstart = (struct zoomdat *)s;
			}
		}
		if (glblp->mpstart != NULL) {
			(char *)glblp->filehead->wh_mp = (char *)glblp->mpstart - diff;
			while ((s = (char *)glblp->mpstart->mp_next) != NULL) {
				(char *)glblp->mpstart->mp_next -= diff;
				glblp->mpstart = (struct mpdat *)s;
			}
		}
		if (glblp->substart != NULL) {
			(char *)glblp->filehead->wh_sub = (char *)glblp->substart - diff;
			while ((s = (char *)glblp->substart->sk_next) != NULL) {
				(char *)glblp->substart->sk_next -= diff;
				glblp->substart = (struct subdat *)s;
			}
		}
		if (glblp->bgstart != NULL) {
			(char *)glblp->filehead->wh_bg = (char *)glblp->bgstart - diff;
			while ((s = (char *)glblp->bgstart->bg_next) != NULL) {
				(char *)glblp->bgstart->bg_next -= diff;
				glblp->bgstart = (struct bgdat *)s;
			}
		}
		if (glblp->strstart != NULL) {
			(char *)glblp->filehead->wh_str = (char *)glblp->strstart - diff;
			while ((s = (char *)glblp->strstart->st_next) != NULL) {
				(char *)glblp->strstart->st_next -= diff;
				(char *)glblp->strstart->st_str -= diff;
				glblp->strstart = (struct strdat *)s;
			}
			(char *)glblp->strstart->st_str -= diff;
		}
		if (glblp->pstrstart != NULL) {
			(char *)glblp->filehead->wh_pstr = (char *)glblp->pstrstart - diff;
			while ((s = (char *)glblp->pstrstart->st_next) != NULL) {
				(char *)glblp->pstrstart->st_next -= diff;
				(char *)glblp->pstrstart->st_str -= diff;
				glblp->pstrstart = (struct strdat *)s;
			}
			(char *)glblp->pstrstart->st_str -= diff;
		}
		if (glblp->lstrstart != NULL) {
			(char *)glblp->filehead->wh_lstr = (char *)glblp->lstrstart - diff;
			while ((s = (char *)glblp->lstrstart->st_next) != NULL) {
				(char *)glblp->lstrstart->st_next -= diff;
				(char *)glblp->lstrstart->st_str -= diff;
				glblp->lstrstart = (struct strdat *)s;
			}
			(char *)glblp->lstrstart->st_str -= diff;
		}
		if (glblp->dirstart != NULL) {
			(char *)glblp->filehead->wh_dir = (char *)glblp->dirstart - diff;
			while ((s = (char *)glblp->dirstart->st_next) != NULL) {
				(char *)glblp->dirstart->st_next -= diff;
				(char *)glblp->dirstart->st_str -= diff;
				glblp->dirstart = (struct strdat *)s;
			}
			(char *)glblp->dirstart->st_str -= diff;
		}
		if (glblp->envstart != NULL) {
			(char *)glblp->filehead->wh_env = (char *)glblp->envstart - diff;
			while ((s = (char *)glblp->envstart->ca_next) != NULL) {
				(char *)glblp->envstart->ca_next -= diff;
				argp = glblp->envstart->ca_args;
				while (*argp != NULL) {
					*argp++ -= diff;
				}
				(char *)glblp->envstart->ca_args -= diff;
				glblp->envstart = (struct arrdat *)s;
			}
			argp = glblp->envstart->ca_args;
			while (*argp != NULL) {
				*argp++ -= diff;
			}
			(char *)glblp->envstart->ca_args -= diff;
		}
		if (glblp->penvstart != NULL) {
			(char *)glblp->filehead->wh_penv = (char *)glblp->penvstart - diff;
			while ((s = (char *)glblp->penvstart->ca_next) != NULL) {
				(char *)glblp->penvstart->ca_next -= diff;
				argp = glblp->penvstart->ca_args;
				while (*argp != NULL) {
					*argp++ -= diff;
				}
				(char *)glblp->penvstart->ca_args -= diff;
				glblp->penvstart = (struct arrdat *)s;
			}
			argp = glblp->penvstart->ca_args;
			while (*argp != NULL) {
				*argp++ -= diff;
			}
			(char *)glblp->penvstart->ca_args -= diff;
		}
		if (glblp->lenvstart != NULL) {
			(char *)glblp->filehead->wh_lenv = (char *)glblp->lenvstart - diff;
			while ((s = (char *)glblp->lenvstart->ca_next) != NULL) {
				(char *)glblp->lenvstart->ca_next -= diff;
				argp = glblp->lenvstart->ca_args;
				while (*argp != NULL) {
					*argp++ -= diff;
				}
				(char *)glblp->lenvstart->ca_args -= diff;
				glblp->lenvstart = (struct arrdat *)s;
			}
			argp = glblp->lenvstart->ca_args;
			while (*argp != NULL) {
				*argp++ -= diff;
			}
			(char *)glblp->lenvstart->ca_args -= diff;
		}
		if (glblp->cmdstart != NULL) {
			(char *)glblp->filehead->wh_cmd = (char *)glblp->cmdstart - diff;
			while ((s = (char *)glblp->cmdstart->ca_next) != NULL) {
				(char *)glblp->cmdstart->ca_next -= diff;
				argp = glblp->cmdstart->ca_args;
				while (*argp != NULL) {
					*argp++ -= diff;
				}
				(char *)glblp->cmdstart->ca_args -= diff;
				glblp->cmdstart = (struct arrdat *)s;
			}
			argp = glblp->cmdstart->ca_args;
			while (*argp != NULL) {
				*argp++ -= diff;
			}
			(char *)glblp->cmdstart->ca_args -= diff;
		}
		if (glblp->menustart != NULL) {
			(char *)glblp->filehead->wh_menu = (char *)glblp->menustart - diff;
			while ((s = (char *)glblp->menustart->mu_next) != NULL) {
				(char *)glblp->menustart->mu_next -= diff;
				(char *)glblp->menustart->mu_list -= diff;
				glblp->menustart = (struct menudat *)s;
			}
			(char *)glblp->menustart->mu_list -= diff;
		}
		if (glblp->chostart != NULL) {
			(char *)glblp->filehead->wh_cho = (char *)glblp->chostart - diff;
			while ((s = (char *)glblp->chostart->co_next) != NULL) {
				(char *)glblp->chostart->co_next -= diff;
				glblp->chostart = (struct chodat *)s;
			}
		}
		if (glblp->actstart != NULL) {
			(char *)glblp->filehead->wh_act = (char *)glblp->actstart - diff;
			while ((s = (char *)glblp->actstart->ac_next) != NULL) {
				(char *)glblp->actstart->ac_next -= diff;
				(char *)glblp->actstart->ac_alist -= diff;
				glblp->actstart = (struct actdat *)s;
			}
			(char *)glblp->actstart->ac_alist -= diff;
		}
		if (glblp->termstart != NULL) {
			(char *)glblp->filehead->wh_term = (char *)glblp->termstart - diff;
			while ((s = (char *)glblp->termstart->td_next) != NULL) {
				(char *)glblp->termstart->td_next -= diff;
				(char *)glblp->termstart->td_list -= diff;
				glblp->termstart = (struct termdat *)s;
			}
			(char *)glblp->termstart->td_list -= diff;
		}
		(char *)glblp->filehead->wh_ini = (char *)glblp->init - diff;

		/*
		 * Finish by writing the data to the output file.
		 */
		diff = glblp->endmem - (char *)glblp->filehead;
		glblp->filehead->wh_size = diff;
		if (strcmp(glblp->outfile, "-") == 0) {
			item = STDOUT;		/* send to standard output */
		}
		else if ((item = creat(glblp->outfile, PERMISSION)) == -1) {
			errname(M_FTCF, glblp->outfile);	/* failed to create */
		}
		if (item >= 0) {
			if (write(item, (char *)glblp->filehead, diff) != diff) {
				if (item != STDOUT) {
					close(item);
					unlink(glblp->outfile);
				}
				errname(M_FTWF, glblp->outfile);	/* failed to write */
			}
			else if (item != STDOUT) {
				msgname(M_SAVFIL, glblp->outfile);	/* info that file saved */
			}
		}
	}	/* if (glblp->outfile != NULL) */

	exit(0);
}	/* end of main() */



/*
 * Display an error on the standard error output and then exit with false
 * status.
 */

errexit(code)
int	code;		/* code for the error message to display */
{
	if (glbl.cmdname != NULL) {
		writestr(glbl.cmdname);
		writestr(": ");
	}
	writestr(messages[2*code+glbl.language]);

	exit(1);
}	/* end of errexit() */



/*
 * Display an error message concerning an unexpected end of file.
 */

uneof()
{
	dsperr(M_UEOF);

	return;
}	/* end of uneof() */



/*
 * Display an error message concerning illegal syntax in the input file.
 */

illsyntax()
{
	dsperr(M_ILLSX);

	return;
}	/* end of illsyntax() */



/*
 * Display a string indicating an error in the input file preceded by the
 * name of the preprocessor and followed by the line number where the error
 * occured.
 */

dsperr(code)
int	code;		/* code for the error message to display */
{
	char		*inttoasc();

	if (glbl.infile != NULL) {
		writestr(glbl.infile);
		writestr(": ");
	}
	writestr(messages[2*code+glbl.language]);
	writestr(messages[2*M_LINE+glbl.language]);
	writestr(inttoasc(glbl.lineno));
	writestr("\n");
	glbl.errflag = YES;		/* set error flag */

	return;
}	/* end of dsperr() */



/*
 * Display the name of the preprocessor followed by a message and a filename
 * on the standard error output.
 */

errname(code, name)
int	code;		/* code of the error message */
char	*name;		/* the filename */
{
	if (glbl.cmdname != NULL) {
		writestr(glbl.cmdname);
		writestr(": ");
	}
	writestr(messages[2*code+glbl.language]);
	writestr(name);
	writestr("\n");

	return;
}	/* end of errname() */



/*
 * Display a message followed by a filename on the standard error output.
 */

msgname(code, name)
int	code;		/* code of the message */
char	*name;		/* the filename */
{
	char	*cp;

	writestr(messages[2*code+glbl.language]);
	writestr(name);
	writestr("'\n");

	return;
}	/* end of msgname() */



/*
 * Display a message indicating that an item has been multiple defined.
 * Also set the global error flag - errflag.
 */

multerr(name, n)
char	*name;		/* name of the item */
int	n;		/* number of the item */
{
	itemerr(name, n, M_MULDEF);
	glbl.errflag = YES;

	return;
}	/* end of multerr() */



/*
 * Display an error message on the standard error output consisting of the
 * input file name followed by an item and an error message. The number
 * of the item is not displayed if n is zero.
 */

itemerr(name, n, code)
char	*name;		/* name of the item */
int	n;		/* number of the item */
int	code;		/* code of the error message */
{
	char	*inttoasc();

	if (glbl.infile != NULL) {
		writestr(glbl.infile);
		writestr(": ");
	}
	writestr("'");
	writestr(name);
	if (n != 0) {
		writestr(inttoasc(n));
	}
	writestr(messages[2*code+glbl.language]);

	return;
}	/* end of itemerr() */



/*
 * Display the input file name followed by an error message. Also set the
 * global error flag - errflag.
 */

errmsg(code)
int	code;		/* code of the message to display */
{
	if (glbl.infile != NULL) {
		writestr(glbl.infile);
		writestr(": ");
	}
	writestr(messages[2*code+glbl.language]);
	glbl.errflag = YES;

	return;
}	/* end of errmsg() */



/*
 * Display the input file name followed by a message saying that an item
 * is refered in another item but not defined. Also set the global error
 * flag - errflag.
 */

notdefmsg(name1, n1, name2, n2)
char	*name1;		/* name of refered item */
int	n1;		/* number of refered item */
char	*name2;		/* name of item where refered */
int	n2;		/* number of above */
{
	char	*inttoasc();

	if (glbl.infile != NULL) {
		writestr(glbl.infile);
		writestr(": ");
	}
	writestr("'");
	writestr(name1);
	if (n1 != 0) {
		writestr(inttoasc(n1));
	}
	writestr(messages[2*M_REFIN+glbl.language]);
	writestr(name2);
	if (n2 != 0) {
		writestr(inttoasc(n2));
	}
	writestr(messages[2*M_BNDEF+glbl.language]);
	glbl.errflag = YES;

	return;
}	/* end of notdefmsg() */



/*
 * Display an error message indicating that a certain item is not present
 * in another item. Also set the global error flag - errflag.
 */

nspecmsg(name1, name2, n2)
char	*name1;		/* name of missing item */
char	*name2;		/* name of item where it is missing */
int	n2;		/* number of above */
{
	char	*inttoasc();

	if (glbl.infile != NULL) {
		writestr(glbl.infile);
		writestr(": ");
	}
	writestr("'");
	writestr(name1);
	writestr(messages[2*M_NSPEC+glbl.language]);
	writestr(name2);
	if (n2 != 0) {
		writestr(inttoasc(n2));
	}
	writestr("'\n");
	glbl.errflag = YES;

	return;
}	/* end of nspecmsg() */



/*
 * Display a warning message indicating that something is wrong in an
 * action or init description. The global error flag is NOT set.
 */

actwarn(code, n)
int	code;		/* code of the message */
int	n;		/* number of the action or 0 if init */
{
	char	*inttoasc();

	if (glbl.infile != NULL) {
		writestr(glbl.infile);
		writestr(": ");
	}
	writestr(messages[2*code+glbl.language]);
	writestr(" '");
	if (n == 0) {
		writestr(D_INIT);
	}
	else {
		writestr(D_ACTION);
		writestr(inttoasc(n));
	}
	writestr("'\n");

	return;
}	/* end of actwarn() */



/*
 * Write a null-terminated string to the standard error output.
 */

writestr(s)
char	*s;		/* string to write */
{
	write(STDERROUT, s, strlen(s));

	return;
}	/* end of writestr() */



/*
 * Allocate memory. This routine always allocates an even number of bytes and
 * the allocated memory area is cleared with zeros.
 * Note that it is allowed to allocate zero bytes.
 */

char *
allocmem(size)
register	size;		/* number of bytes to allocate */
{
	register char	*p;			/* pointer */
	register	cnt;			/* counter */
	static	 char	*curalloc = NULL;	/* next free byte */
	static	 char	*endalloc = NULL;	/* next byte to allocate
						   from the operating system */
	char		*sbrk();

	size++;		/* make size even */
	size >>= 1;
	size <<= 1;
	while (curalloc + size > endalloc) {
		if ((int)(p = sbrk(2048)) == -1) {
			errexit(M_FAM);
		}
		if (curalloc == NULL) {
			curalloc = p;		/* first time */
		}
		cnt = 2048;
		while (cnt-- != 0) {
			*p++ = 0;
		}
		endalloc = p;
	}
	p = curalloc;
	curalloc += size;
	glbl.endmem = curalloc;

	return(p);
}	/* end of allocmem() */



/*
 * Convert a positive binary number to a decimal ASCII string. Returns a
 * pointer to the string.
 */

char *
inttoasc(n)
register	n;		/* number to convert */
{
	static	 char	cnvbuf[16];	/* conversion buffer */
	register char	*cnvp;		/* pointer into cnvbuf[] */

	cnvp = &cnvbuf[15];
	*cnvp = '\0';
	do {
		*--cnvp = n % 10 + '0';
	} while ((n /= 10) > 0);

	return(cnvp);
}	/* end of inttoasc() */



/*
 * Convert an unsigned decimal ASCII string from the input file to binary.
 * The value is returned.
 */

unsigned long
asctoint()
{
	register	n;		/* absolute value of the number */
	register	c;		/* read character */

	n = 0;
	while ((c = creadchar()) >= '0' && c <= '9') {
		n = 10 * n + c - '0';
	}
	backchar(c);

	return(n);
}	/* end of asctoint() */



/*
 * Find the next item to decode from the input file. A code corresponding to
 * the item, ERROR, or EOF is returned.
 * If EOF is returned, an appropriate error message has already been displayed.
 */

int
nextitem()
{
	register	c;	/* read character */
	register	item;	/* item code */

	/*
	 * Skip leading blanks.
	 */
	while ((c = creadchar()) == ' ' || c == '\t' || c == '\n')
		;
	backchar(c);

	item = nexttoken();
	if (item >= WIN_TOK && item <= TERM_TOK) {

		/*
		 * Skip beyond the ':'.
		 */
		while ((c = creadchar()) == ' ' || c == '\t')
			;
		if (c == EOF) {
			uneof();
			return(EOF);
		}
		if (c != ':') {
			backchar(c);
			return(ERROR);
		}
	}

	return(item);
}	/* end of nextitem() */



/*
 * Find the next action in an item from the input file. Otherwise similar to
 * nextitem(), but when EOLN is reached EOLN is returned.
 */

int
nextact()
{
	register	c;		/* read character */

	/*
	 * Skip leading blanks and colons.
	 */
	while ((c = creadchar()) == ' ' || c == '\t' || c == ':')
		;
	backchar(c);
	if (c == '\n') {
		return(EOLN);
	}
	if (c == EOF) {
		uneof();
		return(EOF);
	}

	return(nexttoken());
}	/* end of nextact() */



/*
 * Find the next token in the input file. A code corresponding to the token,
 * ERROR, or EOF is returned.
 * If EOF is returned, an appropriate error message has already been displayed.
 */

int
nexttoken()
{
	static	char	*tokennames[] = {	/* the names of the different tokens */
		D_WINDOW,
		D_ICON,
		D_STRING,
		D_PSTRING,
		D_LSTRING,
		D_POINTER,
		D_HEADER,
		D_DIR,
		D_ENVIRON,
		D_PENVIRON,
		D_LENVIRON,
		D_CMD,
		D_FLAGS,
		D_ZOOM,
		D_SUBST,
		D_BGROUND,
		D_INIT,
		D_MENU,
		D_CHOICE,
		D_ACTION,
		D_TERM,
		A_CLOSE,
		A_RESTORE,
		A_INVERSE,
		A_NORMAL,
		A_LOGOUT,
		A_SUPER,
		A_TOP,
		A_TURN,
		A_WAIT,
		NULL
	};

	char		collect[MAXITEMLEN+1];	/* to collect from file */
	register	c;			/* read character */
	register	i;			/* index */
	unsigned long	asctoint();

	/*
	 * Collect an item name.
	 */
	i = 0;
	while (i < MAXITEMLEN && (c = creadchar()) >= 'a' && c <= 'z') {
		collect[i++] = c;
	}
	collect[i] = '\0';
	if (c == EOF) {
		if (i > 0) {
			uneof();	/* unexpected end of file */
		}
		return(EOF);
	}
	backchar(c);

	/*
	 * Determine which item it is.
	 */
	i = 0;
	while (strcmp(&collect[0], tokennames[i]) != 0) {
		if (tokennames[++i] == NULL) {
			return(ERROR);
		}
	}

	/*
	 * Get the number of the item (if any).
	 */
	glbl.numflag = NO;
	if (c >= '0' && c <= '9') {
		glbl.inumber = (int)asctoint();
		glbl.numflag = YES;
	}

	return(i);
}	/* end of nexttoken() */



/*
 * Collect a keyword (consisting of the characters a - z) of length KW_LEN.
 * Returns ERROR if error, otherwise OK.
 * Note that ERROR doesn't automatically mean an error, this also happens when
 * the end of the line is reached.
 */

int
kwcollect(cbuf)
register char	*cbuf;		/* buffer to receive the keyword */
{
	register	c;	/* read character */
	register	i;	/* counter */

	while ((c = creadchar()) == ' ' || c == '\t' || c == ':')
		;
	i = KW_LEN;
	backchar(c);
	while (i-- != 0 && (c = creadchar()) >= 'a' && c <= 'z') {
		*cbuf++ = c;
	}
	if (i >= 0) {
		backchar(c);
		return(ERROR);
	}

	return(OK);
}	/* end of kwcollect() */



/*
 * Compare two strings of length KW_LEN. Returns HIT if they are equal,
 * else NOTHIT.
 */

int
kwcmp(s1, s2)
register char	*s1;
register char	*s2;
{
	register	i;	/* counter */

	i = KW_LEN;
	while (i-- != 0) {
		if (*s1++ != *s2++) {
			return(NOTHIT);
		}
	}

	return(HIT);
}	/* end of kwcmp() */



/*
 * Get a value corresponding to a keyword from the input file. Except for
 * blanks, it must start with a number sign and the a signed decimal number,
 * an octal number, or a hexadecimal number. The value is returned.
 * If an error occurs, the global flag tmperr will be set to YES and the
 * returned value is of no significance.
 */

long
getval()
{
	register	c;		/* read character */
	register long	val;		/* value */
	long		getdec();
	unsigned long	getoct();
	unsigned long	gethex();

	/*
	 * Skip leading blanks.
	 */
	while ((c = creadchar()) == ' ' || c == '\t')
		;

	/*
	 * Check that the next character is a number sign.
	 */
	if (c != '#') {
		backchar(c);
		glbl.tmperr = YES;
		return(0);
	}

	/*
	 * Skip blanks again.
	 */
	while ((c = creadchar()) == ' ' || c == '\t')
		;
	backchar(c);

	/*
	 * Now determine the type of the number and convert it to binary.
	 */
	if ((c >= '1' && c <= '9') || c == '+' || c == '-') {
		val = getdec();		/* decimal number */
	}
	else if (c != '0') {
		glbl.tmperr = YES;	/* error */
		return(0);
	}
	else {
		c = creadchar();
		c = creadchar();
		backchar(c);
		if (c >= '0' && c <= '9') {
			val = (long)getoct();		/* octal number */
		}
		else if (c != 'x' && c != 'X') {
			return(0);		/* it was a '0' */
		}
		else {
			c = creadchar();
			val = (long)gethex();		/* hexadecimal number */
		}
	}

	return(val);
}	/* end of getval() */



/*
 * Get a signed decimal number from the input file. The global flag tmperr is
 * set to YES if an error occurs.
 * The value is returned.
 */

long
getdec()
{
	register long	n;		/* value */
	register	sign;		/* sign flag */
	register	c;		/* read character */

	n = 0;
	sign = 1;
	c = creadchar();
	if (c == '-') {
		sign = -1;
	}
	else if (c != '+') {
		backchar(c);
	}
	c = creadchar();
	if (c < '0' || c > '9') {
		glbl.tmperr = YES;
	}
	else {
		do {
			n = 10 * n + c - '0';
		} while ((c = creadchar()) >= '0' && c <= '9');
	}
	backchar(c);

	return(sign * n);
}	/* end of getdec() */



/*
 * Get an octal number from the input file. The global flag tmperr is set to
 * YES if an error occurs.
 * The value is returned.
 */

unsigned long
getoct()
{
	register unsigned long	n;		/* value */
	register		c;		/* read character */

	n = 0;
	if ((c = creadchar()) < '0' || c > '9') {
		glbl.tmperr = YES;		/* error */
	}
	else {
		do {
			n = 8 * n + c - '0';
		} while ((c = creadchar()) >= '0' && c <= '9');
	}
	backchar(c);

	return(n);
}	/* end of getoct() */



/*
 * Get a hexadecimal number from the input file. The leading '0x' or '0X' is
 * supposed to have been skipped.
 * The global flag tmperr is set to YES if an error occurs.
 * The value is returned.
 */

unsigned long
gethex()
{
	register unsigned long	n;		/* value */
	register		c;		/* read character */
	register		c1;		/* read character */

	n = 0;
	c1 = c = creadchar();
	if (c >= 'A' && c <= 'F') {
		c += 'a' - 'A';
	}
	if ((c < '0' || c > '9') && (c < 'a' || c > 'f')) {
		glbl.tmperr = YES;		/* error */
	}
	else {
		do {
			if (c >= 'a') {
				c -= 'a' - 10;
			}
			else {
				c -= '0';
			}
			n = 16 * n + c;
			c1 = c = creadchar();
			if (c >= 'A' && c <= 'F') {
				c += 'a' - 'A';
			}
		} while ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'));
	}
	backchar(c1);

	return(n);
}	/* end of gethex() */



/*
 * Get a string of limited length from the input file.
 * If the string is of maximal length, no terminating null character is
 * inserted.
 * If an error occurs, the global flag tmperr will be set to YES.
 */

getlstr(cp, lim)
register char	*cp;		/* area to copy string to */
register	lim;		/* limiting length of string */
{
	register	c;	/* read character */

	/*
	 * Skip leading blanks.
	 */
	while ((c = creadchar()) == ' ' || c == '\t')
		;

	/*
	 * Check that the next character is the '=' character.
	 */
	if (c != '=') {
		backchar(c);
		glbl.tmperr = YES;
		return;
	}

	/*
	 * Now collect the string.
	 */
	while (lim-- > 0 && (c = creadchar()) != EOF && c != ':' && c != '\n') {
		if (c == '\\') {
			c = esccnv();
		}
		*cp++ = c;
	}
	if (lim >= 0) {
		backchar(c);
	}

	return;
}	/* end of getlstr() */



/*
 * Convert an escaped character (or an octal number) to the corresponding
 * character code. The escape character '\' is supposed to has been skipped.
 * The character code is returned.
 */

int
esccnv()
{
	register	c;	/* read character */
	register	n;	/* returned code */
	register	i;	/* counter */

	n = c = creadchar();
	if (c >= '0' && c <= '9') {

		/*
		 * It is an octal number.
		 */
		n = 0;
		i = 3;		/* max number of digits */
		do {
			n = 8 * n + c - '0';
		} while ((c = creadchar()) >= '0' && c <= '9' && --i > 0);
		backchar(c);
	}
	else {
		switch (c) {

		case 'n':		/* newline */
			n = '\n';
			break;

		case 't':		/* tab */
			n = '\t';
			break;

		case 'b':		/* backspace */
			n = '\b';
			break;

		case 'r':		/* carriage return */
			n = '\r';
			break;

		case 'f':		/* formfeed */
			n = '\f';
			break;

		case 'e':		/* escape */
			n = '\033';
			break;
		}
	}

	return(n);
}	/* end of esccnv() */



/*
 * Check if everything is ok when an item has been processed.
 */

ckifok()
{
	int	c;		/* read character */

	c = creadchar();
	backchar(c);
	if (c == EOF) {
		uneof();	/* unexpected end of file */
	}
	else if (glbl.tmperr != NO || c != '\n') {
		illsyntax();
		skipeoln();
	}

	return;
}	/* end of ckifok() */



/*
 * Collect a string from the input file until the end of the current line.
 * A pointer to the string is returned and the string will be terminated
 * by a null character.
 */

char *
getstr()
{
	register char	*start;		/* start of string */
	register char	*cp;
	register	cnt;		/* allocated memory left */
	register	c;		/* read character */
	char		*allocmem();

	cp = start = allocmem(2);
	cnt = 2;

	while ((c = creadchar()) != EOF && c != '\n') {
		if (c == '\\') {
			c = esccnv();	/* convert escaped character */
		}
		if (cnt-- == 0) {
			cp = allocmem(2);	/* allocate more memory */
			cnt = 1;
		}
		*cp++ = c;
	}
	backchar(c);
	if (cnt == 0) {
		cp = allocmem(2);
	}

	/*
	 * The memory allocation routine clears the allocated memory area, so
	 * here we already have a null terminating byte.
	 */

	return(start);
}	/* end of getstr() */



/*
 * Collect one or more strings separated by non-escaped commas from the input
 * file. Proceed until the end of the line is reached.
 * A pointer to an array of character pointers to the strings is returned.
 * The last element in the array is a null pointer.
 */

char **
getmstr()
{
	register char	*cp;
	register char	*cplast;	/* start of last string */
	register char	*start;		/* start of first string */
	register char	**arrayp;	/* start of pointer array */
	register	cnt;		/* allocated memory left */
	register	c;		/* read character */
	char		*allocmem();

	cp = start = allocmem(2);
	cnt = 2;

	do {
		cplast = cp;
		while ((c = creadchar()) != EOF && c != ',' && c != '\n') {
			if (c == '\\') {
				c = esccnv();	/* convert escaped character */
			}
			if (cnt-- == 0) {
				cp = allocmem(2);
				cnt = 1;
			}
			*cp++ = c;
		}
		if (cnt-- == 0) {
			cp = allocmem(2);
			cnt = 1;
		}

		/*
		 * Insert a null character at the end of the string.
		 */
		cp++;
	} while (c != EOF && c != '\n');
	backchar(c);

	/*
	 * Now construct the pointer array.
	 */
	arrayp = (char **)allocmem(0);
	do {
		*((char **)allocmem(sizeof(char *))) = start;
		while (*start++ != '\0')
			;
	} while (start <= cplast);

	/*
	 * Insert the terminating null pointer.
	 */
	cp = allocmem(sizeof(char *));

	return(arrayp);
}	/* end of getmstr() */



/*
 * Get an array of values of word type from the input file. The elements are
 * separated by commas and continuing until end of line or a ':' occurs.
 */

getmword(wp, lim)
register word	*wp;		/* pointer to array to fill */
register	lim;		/* max # of elements in array */
{
	register	c;		/* read character */
	unsigned long	asctoint();
	unsigned long	getoct();
	unsigned long	gethex();

	/*
	 * Skip leading blanks.
	 */
	while ((c = creadchar()) == ' ' || c == '\t')
		;

	/*
	 * Check that the next character is a number sign.
	 */
	if (c != '#') {
		backchar(c);
		glbl.tmperr = YES;
		return;
	}

	do {
		if (lim-- == 0) {
			glbl.tmperr = YES;	/* error, to many elements */
			return;
		}

		/*
		 * Skip blanks.
		 */
		while ((c = creadchar()) == ' ' || c == '\t')
			;
		backchar(c);

		if (c == ',' || c == ':' || c == '\n') {
			wp++;	/* empty field, i.e. value is zero */
		}
		else if (c >= '1' && c <= '9') {
			*wp++ = (word)asctoint();	/* decimal number */
		}
		else if (c != '0') {
			glbl.tmperr = YES;		/* error */
			return;
		}
		else {
			c = creadchar();
			c = creadchar();
			backchar(c);
			if (c >= '0' && c <= '9') {
				*wp++ = (word)getoct();	/* octal number */
			}
			else if (c != 'x' && c != 'X') {
				wp++;		/* it was a single '0' */
			}
			else {
				c = creadchar();
				*wp++ = (word)gethex();	/* hexadecimal number */
			}
		}

		/*
		 * Skip blanks again.
		 */
		while ((c = creadchar()) == ' ' || c == '\t')
			;

	} while (c == ',');
	backchar(c);

	return;
}	/* end of getmword() */



/*
 * Get an array of values of dword type from the input file. The elements are
 * separated by commas and continuing until end of line or a ':' occurs.
 */

getmdword(dwp, lim)
register dword	*dwp;		/* pointer to array to fill */
register	lim;		/* max # of elements in array */
{
	register	c;		/* read character */
	unsigned long	asctoint();
	unsigned long	getoct();
	unsigned long	gethex();

	/*
	 * Skip leading blanks.
	 */
	while ((c = creadchar()) == ' ' || c == '\t')
		;

	/*
	 * Check that the next character is a number sign.
	 */
	if (c != '#') {
		backchar(c);
		glbl.tmperr = YES;
		return;
	}

	do {
		if (lim-- == 0) {
			glbl.tmperr = YES;	/* error, to many elements */
			return;
		}

		/*
		 * Skip blanks.
		 */
		while ((c = creadchar()) == ' ' || c == '\t')
			;
		backchar(c);
		if (c == ',' || c == ':' || c == '\n') {
			dwp++;		/* empty field, i.e. zero value */
		}
		else if (c >= '1' && c <= '9') {
			*dwp++ = (dword)asctoint();	/* decimal number */
		}
		else if (c != '0') {
			glbl.tmperr = YES;		/* error */
			return;
		}
		else {
			c = creadchar();
			c = creadchar();
			backchar(c);
			if (c >= '0' && c <= '9') {
				*dwp++ = (dword)getoct();	/* octal number */
			}
			else if (c != 'x' && c != 'X') {
				dwp++;		/* it was a single '0' */
			}
			else {
				c = creadchar();
				*dwp++ = (dword)gethex();	/* hexadecimal number */
			}
		}

		/*
		 * Skip blanks again.
		 */
		while ((c = creadchar()) == ' ' || c == '\t')
			;

	} while (c == ',');
	backchar(c);

	return;
}	/* end of getmdword() */
