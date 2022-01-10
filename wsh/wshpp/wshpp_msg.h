/* wshpp_msg.h */

/*
 * Messages and some other strings used in the window shell preprocessor - wshpp.
 */

/* 1985-02-28, Peter Andersson, Luxor Datorer AB */



char	*messages[] = {
	"data type missing\n",					/* M_DTM */
	"data-typ saknas\n",
	"file name missing\n",					/* M_FNM */
	"filnamn saknas\n",
	"illegal data type\n",					/* M_IDT */
	"felaktig data-typ\n",
	"failed to open input file\n",				/* M_FOIF */
	"kan ej |ppna in-filen\n",
	"Saved '",						/* M_SAVFIL */
	"Sparat '",
	"can't create ",					/* M_FTCF */
	"kan ej skapa ",
	"can't write ",						/* M_FTWF */
	"kan ej skriva ",
	", line ",						/* M_LINE */
	", rad ",
	"failed to allocate memory\n",				/* M_FAM */
	"misslyckades att allokera minne\n",
	"unexpected end of file",				/* M_UEOF */
	"ov{ntat filslut",
	"illegal syntax",					/* M_ILLSX */
	"felaktig syntax",
	"illegal or missing 'window' number",			/* M_IMWN */
	"'window'-nummer saknas eller {r felaktigt",
	"'illegal or missing 'icon' number",			/* M_IMIN */
	"'icon'-nummer saknas eller {r felaktigt",
	"illegal or missing 'header' number",			/* M_IMHN */
	"'header'-nummer saknas eller {r felaktigt",
	"illegal or missing 'flags' number",			/* M_IMFN */
	"'flags'-nummer saknas eller {r felaktigt",
	"illegal or missing 'substitute' number",		/* M_IMSKN */
	"'substitute'-nummer saknas eller {r felaktigt",
	"multiple 'substitute' sets",				/* M_MSSG */
	"flera 'substitute'-upps{ttningar",
	"illegal or missing 'string' number",			/* M_IMSN */
	"'string'-nummer saknas eller {r felaktigt",
	"illegal or missing 'directory' number",		/* M_IMDN */
	"'directory'-nummer saknas eller {r felaktigt",
	"illegal or missing 'command' number",			/* M_IMCN */
	"'command'-nummer saknas eller {r felaktigt",
	"illegal or missing 'pointer' number",			/* M_IMPN */
	"'pointer'-nummer saknas eller {r felaktigt",
	"illegal or missing 'background' number",		/* M_IMBGN */
	"'background'-nummer saknas eller {r felaktigt",
	"multiple 'background' patterns",			/* M_MBP */
	"flera 'background'-m|nster",
	"illegal or missing 'choice' number",			/* M_IMCHN */
	"'choice'-nummer saknas eller {r felaktigt",
	"multiple 'icons'",					/* M_MULIC */
	"flera 'icon'",
	"illegal or missing 'action' number",			/* M_IMAN */
	"'action'-nummer saknas eller {r felaktigt",
	"multiple 'actions'",					/* M_MULAC */
	"flera 'action'",
	"'init' number is not allowed",				/* M_INNA */
	"'init'-nummer {r ej till}tet",
	"multiple 'init' descriptions",				/* M_MID */
	"flera 'init'-beskrivningar",
	"multiple 'pointers'",					/* M_MPNT */
	"flera 'pointer'",
	"illegal or missing 'menu' number",			/* M_IMMN */
	"'menu'-nummer saknas eller {r felaktigt",
	"multiple 'menus'",					/* M_MULMU */
	"flera 'menu'",
	"illegal or missing 'action' number",			/* M_IMACTN */
	"'action'-nummer saknas eller {r felaktigt",
	"'close' number is not allowed",			/* M_CNNA */
	"'close'-nummer {r ej till}tet",
	"'restore' number is not allowed",			/* M_RNNA */
	"'restore'-nummer {r ej till}tet",
	"'inverse' number is not allowed",			/* M_INVNA */
	"'inverse'-nummer {r ej till}tet",
	"'normal' number is not allowed",			/* M_NRMNA */
	"'normal'-nummer {r ej till}tet",
	"'logout' number is not allowed",			/* M_LOUTNA */
	"'logout'-nummer {r ej till}tet",
	"illegal or missing 'terminal' number",			/* M_IMTERM */
	"'terminal'-nummer saknas eller {r felaktigt",
	"multiple 'windows'",					/* M_MULWIN */
	"flera 'window'",
	"multiple 'headers'",					/* M_MULHD */
	"flera 'header'",
	"multiple 'directories'",				/* M_MULDIR */
	"flera 'directory'",
	"multiple 'commands'",					/* M_MULCMD */
	"flera 'command'",
	"illegal option\n",					/* M_ILLOPT */
	"felaktig option\n",
	"'super' number is not allowed",			/* M_NSUPNO */
	"'super'-nummer {r ej till}tet",
	"illegal or missing 'pstring' number",			/* M_IMPSN */
	"'pstring'-nummer saknas eller {r felaktigt",
	"illegal or missing 'lstring' number",			/* M_IMLSN */
	"'lstring'-nummer saknas eller {r felaktigt",
	"' multiple defined\n",					/* M_MULDEF */
	"' multipelt definierad\n",
	"'top' number is not allowed",				/* M_TOPNA */
	"'top'-nummer {r ej till}tet",
	"' not defined\n",					/* M_NOTDEF */
	"' ej definierad\n",
	"'init' missing\n",					/* M_MINIT */
	"'init' saknas\n",
	"' refered in '",					/* M_REFIN */
	"' refererad i '",
	"' but not defined\n",					/* M_BNDEF */
	"' men ej definierad\n",
	"' not specified in '",					/* M_NSPEC */
	"' ej specificerad i '",
	"(warning) 'menu' not at the end of",			/* M_ACT1 */
	"(varning) 'menu' ej i slutet p}",
	"no 'menu' in",						/* M_ACT2 */
	"'menu' saknas i",
	"(warning) multiple 'menus' in",			/* M_ACT3 */
	"(varning) flera 'menu' i",
	"'turn' number is not allowed",				/* M_TURNNA */
	"'turn'-nummer {r ej till}tet",
	"illegal or missing 'zoomlist' number",			/* M_IMZN */
	"'zoomlist'-nummer saknas eller {r felaktigt",
	"multiple 'zoomlists'",					/* M_MULZOOM */
	"flera 'zoomlist'",
	"(warning) both 'choice' and 'action' in '",		/* M_BACMU */
	"(varning) b}de 'choice' och 'action' i '",
	"illegal or missing 'environ' number",			/* M_IMENVN */
	"'environ'-nummer saknas eller {r felaktigt",
	"illegal or missing 'penviron' number",			/* M_IMPENVN */
	"'penviron'-nummer saknas eller {r felaktigt",
	"illegal or missing 'lenviron' number",			/* M_IMLENVN */
	"'lenviron'-nummer saknas eller {r felaktigt",
	"'wait' number is not allowed",				/* M_NWTNO */
	"'wait'-nummer {r ej till}tet"
};



/*
 * List of the different data items that can be specified to be saved in the
 * command line.
 */

char	*argitems[] = {
	D_WINDOW,
	D_ICON,
	D_POINTER,
	D_HEADER,
	D_FLAGS,
	D_ZOOM,
	D_SUBST,
	D_BGROUND,
	NULL
};
