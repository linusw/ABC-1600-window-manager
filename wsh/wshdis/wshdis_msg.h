/* wshdis_msg.h */

/*
 * This file contains the different messages (in english and swedish) used in
 * the wsh disassembler - wshdis.
 */

/* 1985-02-26, Peter Andersson, Luxor Datorer AB */



char	*messages[] = {
	"multiple output files\n",			/* M_MULOUT */
	"flera ut-filer\n",
	"file name missing\n",				/* M_FNAMMIS */
	"filnamn saknas\n",
	"multiple input files\n",			/* M_MULIN */
	"flera in-filer\n",
	"failed to open input file\n",			/* M_FOIF */
	"misslyckades |ppna in-fil\n",
	"format of input file is not correct\n",	/* M_ILLFORM */
	"in-filens format {r felaktigt\n",
	"can't allocate memory\n",			/* M_NOMEM */
	"kan ej allokera minne\n",
	"failed to create output file\n",		/* M_FCOF */
	"misslyckades skapa ut-fil\n",
	"failed to write to output file\n",		/* M_FWOF */
	"misslyckades att skriva till ut-filen\n"
};
