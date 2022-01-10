/* w_types.h */

/*
 * This file contains declarations of some new data types.
 * Used when communicating with the ABC1600 window handler.
 */

/* 1985-07-23, Luxor Datorer AB */



#define		W_TYPES


typedef		 short	pix_d;		/* distance in pixel units      */
typedef		 short	cur_d;		/* distance in cursor positions */
typedef		 char	sint;		/* small integer                */
typedef	unsigned char	byte;		/* a byte                       */
typedef unsigned short	word;		/* a word (2 bytes)             */
typedef unsigned long	dword;		/* a double word (4 bytes)      */
typedef		 dword	uflags;		/* user flags type              */
typedef	unsigned long	t_stop;		/* to hold tab stop marks       */
