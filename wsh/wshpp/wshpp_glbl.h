/* wshpp_glbl.h */

/*
 * This file contains a structure which contains most of the global variables
 * used in the window shell preprocessor - wshpp.
 */

/* 1985-02-20, Peter Andersson, Luxor Datorer AB */



struct	globals {
	char		  *cmdname;	/* name of this command */
	char		  *outfile;	/* output file */
	char		  *infile;	/* input file */
	short		  nopt;		/* -n option present flag */
	struct	xdata	  *xstart;	/* pointer to list of single structures to write out */
	int		  inputfd;	/* file descriptor for input file */
	int		  errflag;	/* error has occured flag */
	int		  tmperr;	/* temporary error flag */
	short		  numflag;	/* item number given flag */
	short		  inumber;	/* item number */
	char		  *endmem;	/* pointer to end of allocated memory */
	long		  lineno;	/* number of currently processed line */
	long		  charno;	/* number of the currently processed character */
	int		  language;	/* language flag */
	struct	windat	  *wdstart;	/* pointer to first window structure */
	struct	windat	  *wdcur;	/* pointer to current window structure */
	struct	headdat	  *hdstart;	/* pointer to first header structure */
	struct	headdat	  *hdcur;	/* pointer to current header structure */
	struct	icondat	  *icstart;	/* pointer to first icon structure */
	struct	icondat	  *iccur;	/* pointer to current icon structure */
	struct	flagdat	  *flgstart;	/* pointer to first flag structure */
	struct	flagdat	  *flgcur;	/* pointer to current flag structure */
	struct	zoomdat	  *zoomstart;	/* pointer to first zoom list structure */
	struct	zoomdat	  *zoomcur;	/* pointer to current zoom list structure */
	struct	mpdat	  *mpstart;	/* pointer to first mouse pointer structure */
	struct	mpdat	  *mpcur;	/* pointer to current mouse pointer structure */
	struct	subdat	  *substart;	/* start of substitute keys list */
	struct	subdat	  *subcur;	/* current substitute key data */
	struct	bgdat	  *bgstart;	/* start of background patterns */
	struct	bgdat	  *bgcur;	/* current background data */
	struct	strdat	  *strstart;	/* pointer to first string */
	struct	strdat	  *strcur;	/* pointer to current string */
	struct	strdat	  *pstrstart;	/* pointer to first portrait string */
	struct	strdat	  *pstrcur;	/* pointer to current portrait string */
	struct	strdat	  *lstrstart;	/* pointer to first landscape string */
	struct	strdat	  *lstrcur;	/* pointer to current landscape string */
	struct	strdat	  *dirstart;	/* pointer to first pathname */
	struct	strdat	  *dircur;	/* pointer to current pathname */
	struct	arrdat	  *envstart;	/* pointer to first environment variable */
	struct	arrdat	  *envcur;	/* pointer to current environment variable */
	struct	arrdat	  *penvstart;	/* pointer to first portrait environment */
	struct	arrdat	  *penvcur;	/* pointer to current portrait environment */
	struct	arrdat	  *lenvstart;	/* pointer to first landscape environment */
	struct	arrdat	  *lenvcur;	/* pointer to current landscape environment */
	struct	arrdat	  *cmdstart;	/* pointer to first command */
	struct	arrdat	  *cmdcur;	/* pointer to current command */
	struct	actlist	  *init;	/* pointer to actions on init */
	struct	menudat	  *menustart;	/* pointer to start of menu data's */
	struct	menudat	  *menucur;	/* pointer to current menu data */
	struct	chodat	  *chostart;	/* pointer to start of choices */
	struct	chodat	  *chocur;	/* pointer to current choice */
	struct	actdat	  *actstart;	/* pointer to start of actions */
	struct	actdat	  *actcur;	/* pointer to current action */
	struct	termdat	  *termstart;	/* pointer to start of terminals */
	struct	termdat	  *termcur;	/* pointer to current terminal */
	struct	wshhead	  *filehead;	/* pointer to file header */
};
