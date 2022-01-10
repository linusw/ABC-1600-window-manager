/* gdriver.h */

/*
 * This file contains definitions and declarations that later on should be
 * replaced by standard header files.
 */

/* 1984-11-27, Peter Andersson, Luxor Datorer AB */



/*
 * Structure used to load and read the function keys.
 */
struct fnkst {
	union {
		struct {
			unsigned char	code;	/* keycode, usually >= 0x80            */
			unsigned char	len;	/* length of string                    */
		} st;
		short	space;			/* space left after loading (returned) */
	} u;
	unsigned char	string[60];		/* the function key string             */
};
