/* w_term.c */

/*
 * This function opens and creates a window running the program specified.
 * The window will have the status of a terminal with the standard input,
 * output, and error output as the window.
 * A specified header will be inserted in the window header if the pointer
 * to this structure isn't NULL.
 * Before the program is executed, the current directory will be changed to
 * the one specified (if the pointer isn't NULL).
 * This function fork's and does not wait for the process to terminate.
 * Before the execution of the program, all files will be closed, except
 * those with file descriptors 0, 1, and 2.
 * Nothing are done with the set up of the signals, so signals already
 * ignored will remain so when the program is executed.
 * No errors are returned, instead, in case of an error, the child process
 * will terminate.
 * If the execution of the program fails, an error message is displayed in the
 * new window (no message is displayed if the pointer to the message is null).
 */

/* 1985-01-23, Peter Andersson, Luxor Datorer AB */


#include	<stdio.h>
#include	<fcntl.h>
#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>


w_term(wdp, hdp, dir, name, argv, errp)
struct	winstruc  *wdp;		/* pointer to window data                */
struct	headstruc *hdp;		/* pointer to window header data or NULL */
char		  *dir;		/* directory to change to or NULL        */
char		  *name;	/* name of the file to execute           */
char		  **argv;	/* program arguments                     */
char		  *errp;	/* error message to display if the
				   execution fails                       */
{
	register	pid;	/* process id.     */
	register	s;	/* returned status */
	int		r;

	/*
	 * Let the program fly. Note that two fork's are necessary to avoid
	 * zoombies, i.e. processes not waited for. In this way the init
	 * process will wait for the double forked process.
	 */
	if ((pid = fork()) != 0) {
		while ((s = wait(&r)) != pid && s != -1)
			;
		return;
	}
	if (fork() != 0) {
		exit(0);
	}

	/*
	 * Close all open files.
	 */
	for (s = 0 ; s < _NFILE ; s++) {
		close(s);
	}

	/*
	 * Set up new process group (equal to the process id).
	 */
	if (setpgrp() < 0 ||

	/*
	 * Open a channel for the new window.
	 */
	    open(WMNTDIR, 2) != 0 ||

	/*
	 * Set up a new controlling terminal.
	 */
	    fcntl(0, F_SETCT, 0) < 0 ||

	/*
	 * Do two dup's to create the standard output and error output.
	 */
	    dup(0) != 1 || dup(0) != 2 ||

	/*
	 * Create the window.
	 */
	    Wincreat(0, wdp) < 0 || wdp->w_rstat != W_OK ||

	/*
	 * Insert possible header in the window.
	 */
	    (hdp != NULL && Winheader(0, hdp) < 0) ||

	/*
	 * Possibly change to another directory.
	 */
	    (dir != NULL && chdir(dir) < 0)) {
		_exit(1);
	}

	/*
	 * Execute the desired program.
	 */
	execv(name, argv);
	if (errp != NULL) {
		write(0, errp, strlen(errp));
		sleep(5);
	}
	_exit(1);
}
