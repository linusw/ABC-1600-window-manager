/* wshdis_defs.h */

/*
 * This file contains constant definitions used in the wsh disassembler -
 * wshdis.
 */

/* 1985-02-26, Peter Andersson, Luxor Datorer AB */



#define		ENGLISH		0		/* english mode */
#define		SWEDISH		1		/* swedish mode */
#define		NULL		0		/* null pointer */
#define		DFLTFILE	".window"	/* default input file */
#define		STDOUT		1		/* standard output */
#define		STDERR		2		/* standard error output */
#define		PERMISSION	0644		/* permission of output file */
#define		EIGHTMASK	255		/* to mask out lower 8 bits */



/*
 * Codes (i.e. indices) for the different messages.
 */

#define		M_MULOUT	0
#define		M_FNAMMIS	1
#define		M_MULIN		2
#define		M_FOIF		3
#define		M_ILLFORM	4
#define		M_NOMEM		5
#define		M_FCOF		6
#define		M_FWOF		7
