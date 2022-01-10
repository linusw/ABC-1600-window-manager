/*

	x35g.h

	definitions for character generator

	by Jan Tjernberg

*/




/*
*	font information structure 
*/


typedef struct { 
  unsigned short cg_magic;	/* type of generator (value: 0xffff)   */
  unsigned short cg_stat;	/* flags i e Portrait/Landscape        */
  unsigned short cg_fnumb;	/* the number of the generator         */
  unsigned short cg_fxsiz;	/* font sixe x, # of pixels            */
  unsigned short cg_fysiz;	/* font size y, # of pixels            */
  unsigned char  cg_firstc;	/* ascii # for first defined char      */
  unsigned char  cg_lastc;	/* ascii # for last defined char       */
  unsigned short cg_bwidth;	/* # of bytes per line in memory dump  */
  unsigned short cg_dumpl;	/* # of bytes in dump after this block */
  unsigned char pad[24];	/* pad block to 40 bytes. Future use   */
} cgenblk ;


/*
*	picture load/store structure
*/


typedef struct { 
  unsigned short magic;		/* type of generator (value: 0xfffe)   */
  unsigned short stat;		/* flags                               */
  unsigned short xaddr;		/* from/to x address (aligned pixel)   */
  unsigned short yaddr;		/* from/to y address (pixel)           */
  unsigned short bwidth;	/* # of bytes per line in memory dump  */
  unsigned short bheight;	/* # of byte lines in dump after this block */
  unsigned char pad[38];	/* pad block to 50 bytes. Future use   */
} pictblk ;



#ifdef NOT_USED
#define PPICTLD 	(('p'<<8)|2)
#define PPICTRD 	(('p'<<8)|3)
#endif
#define PFNKLD		(('p'<<8)|4)
#define PFNKRD		(('p'<<8)|5)
#define PTOKBD		(('p'<<8)|6)
#define PFLOAD		(('p'<<8)|7)
#define PFREMOVE	(('p'<<8)|8)


#ifdef NOT_USED
#define pread(bp,size)	dnix(F_IOCW,1,bp,size,PPICTRD,0,0,0)
#define pwrite(bp,size)	dnix(F_IOCW,1,bp,size,PPICTLD,0,0,0)
#endif

/* ptokbrd sends character pointed to by 'bp' to keyboard */

#define ptokbrd(fd, bp)		dnix(F_IOCW,fd,bp,1,PTOKBD,0,0,0)

/* the following calls are only for window handler      */
/* loading and removing of fonts are automatic in the   */
/* VT100 emulator for ABC1600                           */

#ifdef NOT_USED
#define pfload(fd,bp)	dnix(F_IOCW,fd,bp,1,PFLOAD,0,0,0)
#define pfrem(fd,bp)	dnix(F_IOCW,fd,bp,1,PFREMOVE,0,0,0)
#endif
