/* wsh_msg.h */

/*
 * Error messages and other strings used in the window shell - wsh.
 */

/* 1985-03-19, Peter Andersson, Luxor Datorer AB */



char	*messages[] = {
	"cannot access the window handler\n",		/* M_ACCESS */
	"kan ej anropa f|nster-hanteraren\n",
	"failed to open data file\n",			/* M_FOIF */
	"misslyckades |ppna data-fil\n",
	"cannot allocate memory\n",			/* M_NOMEM */
	"kan ej allokera minne\n",
	"illegal format of data file\n",		/* M_ILLFORM */
	"felaktigt format p} data-fil\n",
	"cannot activate the window handler\n",		/* M_CANTINIT */
	"kan ej aktivera f|nster-hanteraren\n"
};
