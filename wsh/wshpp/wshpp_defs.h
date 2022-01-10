/* wshpp_defs.h */

/*
 * This file contains constant definitions and structure declarations for the
 * window shell preprocessor - wshpp.
 */

/* 1985-02-20, Peter Andersson, Luxor Datorer AB */



/*
 * Structure used to store which single data items shall be extracted and
 * written to file.
 */

struct	xdata {
	char	*x_fname;	/* pointer to file name */
	short	x_no;		/* the number of the item */
	short	x_type;		/* the type of item */
};



#define		DFLTFILE	".window"	/* default name of data file */
#define		PERMISSION	0644		/* protection for data file */
#define		CMASK		127		/* to mask out seven bit character */
#define		COMMENT		'#'		/* character starting a comment line */
#define		MAXITEMLEN	16		/* max length of an item name */
#define		KW_LEN		4		/* length of a keyword */



/*
 * Some mnemonic constants.
 */

#define		NULL		0		/* null pointer */
#define		STDIN		0		/* standard input */
#define		STDOUT		1		/* standard output */
#define		STDERROUT	2		/* standard error output */
#define		NO		0
#define		YES		1
#define		HIT		0
#define		NOTHIT		1
#define		ENGLISH		0
#define		SWEDISH		1
#define		OK		0
#define		EOF		(-1)
#define		ERROR		(-2)
#define		EOLN		(-3)



/*
 * Codes of different items which can be specified in the command line.
 */

#define		ARG_WIN		0		/* window data */
#define		ARG_ICON	1		/* icon data */
#define		ARG_PNT		2		/* mouse pointer data */
#define		ARG_HEAD	3		/* header data */
#define		ARG_FLG		4		/* flags data */
#define		ARG_ZOOM	5		/* zoom list data */
#define		ARG_SUBST	6		/* substitute keys data */
#define		ARG_BG		7		/* background data */



/*
 * Codes of different tokens.
 */

#define		WIN_TOK		0		/* window data */
#define		ICON_TOK	1		/* icon data */
#define		STR_TOK		2		/* a string */
#define		PSTR_TOK	3		/* a portrait string */
#define		LSTR_TOK	4		/* a landscape string */
#define		PNT_TOK		5		/* mouse pointer data */
#define		HEAD_TOK	6		/* window header data */
#define		DIR_TOK		7		/* a pathname */
#define		ENV_TOK		8		/* environment variables */
#define		PENV_TOK	9		/* portrait environment variables */
#define		LENV_TOK	10		/* landscape environment variables */
#define		CMD_TOK		11		/* a command */
#define		FLG_TOK		12		/* flags data */
#define		ZOOM_TOK	13		/* zoom list data */
#define		SUBST_TOK	14		/* mouse substitute keys */
#define		BG_TOK		15		/* background data */
#define		INIT_TOK	16		/* initialization data */
#define		MENU_TOK	17		/* menu data */
#define		CHO_TOK		18		/* a choice description */
#define		ACT_TOK		19		/* an action description */
#define		TERM_TOK	20		/* terminal data */
#define		CLOS_TOK	21		/* close window */
#define		REST_TOK	22		/* restore screen */
#define		INV_TOK		23		/* inverse video */
#define		NORM_TOK	24		/* normal video */
#define		LOUT_TOK	25		/* log out */
#define		SUP_TOK		26		/* super channel */
#define		TOP_TOK		27		/* move to top level */
#define		TURN_TOK	28		/* turn the screen */
#define		WAIT_TOK	29		/* wait for cmd to finish */



/*
 * Codes (i.e. indices) for the different messages.
 */

#define		M_DTM		0
#define		M_FNM		1
#define		M_IDT		2
#define		M_FOIF		3
#define		M_SAVFIL	4
#define		M_FTCF		5
#define		M_FTWF		6
#define		M_LINE		7
#define		M_FAM		8
#define		M_UEOF		9
#define		M_ILLSX		10
#define		M_IMWN		11
#define		M_IMIN		12
#define		M_IMHN		13
#define		M_IMFN		14
#define		M_IMSKN		15
#define		M_MSSG		16
#define		M_IMSN		17
#define		M_IMDN		18
#define		M_IMCN		19
#define		M_IMPN		20
#define		M_IMBGN		21
#define		M_MBP		22
#define		M_IMCHN		23
#define		M_MULIC		24
#define		M_IMAN		25
#define		M_MULAC		26
#define		M_INNA		27
#define		M_MID		28
#define		M_MPNT		29
#define		M_IMMN		30
#define		M_MULMU		31
#define		M_IMACTN	32
#define		M_CNNA		33
#define		M_RNNA		34
#define		M_INVNA		35
#define		M_NRMNA		36
#define		M_LOUTNA	37
#define		M_IMTERM	38
#define		M_MULWIN	39
#define		M_MULHD		40
#define		M_MULDIR	41
#define		M_MULCMD	42
#define		M_ILLOPT	43
#define		M_NSUPNO	44
#define		M_IMPSN		45
#define		M_IMLSN		46
#define		M_MULDEF	47
#define		M_TOPNA		48
#define		M_NOTDEF	49
#define		M_MINIT		50
#define		M_REFIN		51
#define		M_BNDEF		52
#define		M_NSPEC		53
#define		M_ACT1		54
#define		M_ACT2		55
#define		M_ACT3		56
#define		M_TURNNA	57
#define		M_IMZN		58
#define		M_MULZOOM	59
#define		M_BACMU		60
#define		M_IMENVN	61
#define		M_IMPENVN	62
#define		M_IMLENVN	63
#define		M_NWTNO		64
