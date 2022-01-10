/* dnixmacros.h */

/*
 * This file contains macros which are shorthands for the dnix() system call.
 */

/* 1984-08-09, Peter Andersson, Luxor Datorer AB (at DIAB) */



/*
 * Mount handler.
 * name = name of directory to mount on (full pathname without a leading '/').
 * len = length of 'name'.
 */

#define	Mount(name, len)	dnix(F_MOUNT, RDIRLU, name, len, 0, 0, 0, 0)


/*
 * Wait for a request to come in (don't use nowait).
 * rlu = lu obtained on mount.
 * bp = pointer to structure to receive request.
 * bc = size of request structure.
 */

#define	Waitreq(rlu, bp, bc)	dnix(F_READ, rlu, bp, bc, 0, 0, 0, 0)


/*
 * Terminate a request.
 * rn = request number of request to terminate.
 * par1 = returned parameter to process making the request.
 * par2 = parameter to kernel.
 */

#define	Termreq(rn, par1, par2)		(void)dnix(F_TERMIN, rn, par1, par2)

/*
 * Read from user buffer.
 * rn = request number.
 * bp = buffer to read into.
 * bc = number of bytes to read.
 * offs = offset from start of the user's buffer.
 */

#define	Readubuf(rn, bp, bc, offs)	dnix(F_UREAD, rn, bp, bc, offs, 0, 0, 0)


/*
 * Read from user buffer with nowait (type 2 request). Otherwise as above.
 */

#define	Rubnwait(rn, bp, bc, offs)	dnix(F_T2URD, rn, bp, bc, offs)


/*
 * Write to user buffer.
 * rn = request number.
 * bp = pointer to buffer to write from.
 * bc = number of bytes to write.
 * offs = offset from start of the user's buffer.
 */

#define	Writubuf(rn, bp, bc, offs)	dnix(F_UWRITE, rn, bp, bc, offs, 0, 0, 0)


/*
 * Write to user buffer with nowait (type 2 request). Otherwise as above.
 */

#define	Wubnwait(rn, bp, bc, offs)	dnix(F_T2UWR, rn, bp, bc, offs)


/*
 * Segment processing. I.e. map in or out graphic or I/O memory.
 * bp = pointer to appropriate structure.
 * bc = size of the structure.
 * par = parameter describing the action.
 */

#define	Segproc(bp, bc, par)	dnix(F_SEGP, bp, bc, par, 0)


/*
 * Macro to read handler information in request.
 * rn = request number.
 * bp = pointer to appropriate structure.
 * bc = size of the structure.
 */

#define	Readhinfo(rn, bp, bc)	dnix(F_RDREQ, rn, bp, 0, bc)


/*
 * Macro to write handler information to request.
 * rn = request number.
 * bp = pointer to appropriate structure.
 * bc = size of the structure.
 */

#define	Writhinfo(rn, bp, bc)	dnix(F_WHREQ, rn, bp, bc)


/*
 * Macro to reread request and handler information from request.
 * rn = request number.
 * bp = pointer to appropriate structure.
 * bc = size of the structure.
 */

#define	Rereadrh(rn, bp, rbc, hbc)	dnix(F_RDREQ, rn, bp, rbc, hbc)


/*
 * Macro for exit. It saves space to use the DNIX instead of the UNIX version.
 * status = exit status.
 */

#ifdef UNIXREQ
#define	Exit(status)	exit(status)
#else
#define	Exit(status)	(void)dnix(F_EXIT, status)
#endif


/*
 * Macro for I/O control read.
 * lu = logical unit number for the file.
 * subfc = subfunction.
 * bp = pointer to where read data shall be put.
 * bc = size of data.
 */

#define	Ioctlr(lu, subfc, bp, bc)	dnix(F_IOCR, lu, bp, bc, subfc, 0, 0, 0)


/*
 * Macro to get or release memory. It saves space to use the DNIX instead of
 * the UNIX version.
 * incr = size to increment or decrement with.
 */

#ifdef UNIXREQ
#define	Sbrk(incr)	sbrk(incr)
#else
#define	Sbrk(incr)	dnix(F_BREAK, incr)
#endif


/*
 * Macro to get the process group id. (as the returned value) for the open
 * request whose request number is given.
 * Note that this must be done before the open request is terminated.
 * rn = request number.
 */

#define	Getpgid(rn)	dnix(F_GROI, rn, 0)


/*
 * Send a signal to a whole process group.
 * pgid = negative process group id.
 * sig = signal.
 */

#define	Sigpgrp(pgid, sig)	(void)kill(pgid, sig)


/*
 * Execute a program. It saves space to use this request instead of a UNIX
 * exec request.
 * fnp = file name pointer (full pathname without a leading '/').
 * fnc = file name count.
 * argp = pointer to argument list.
 */

#define	Exec(fnp, fnc, argp)	(void)dnix(F_EXEC, RDIRLU, fnp, fnc, argp, NULL);
