/* wsh.c */

/*
 * This file contains the source code for the window shell - wsh.
 */

/*
 * Functions in this file:
 *	main()
 *	redraw()
 *	resig()
 *	winloop()
 *	terminal()
 *	getcmd()
 *	dspstrings()
 *	errexit()
 *	ewritstr()
 *	winfind()
 *	winsetup()
 *	headsetup()
 *	iconsetup()
 *	flagsetup()
 *	zoomsetup()
 *	mpsetup()
 *	subsetup()
 *	bgsetup()
 *	strout()
 *	dirchng()
 *	execcmd()
 *	menufind()
 *	chofind()
 *	actfind()
 *	termfind()
 *	envsetup()
 *	insenv()
 *	eqcompar()
 *	iorqst()
 *	iocmd()
 */



#include	<stdio.h>
#undef		NULL
#include	<sgtty.h>
#undef		FIOCLEX
#undef		FIONCLEX
#include	<fcntl.h>
#include	<signal.h>
#include	<dnix/fcodes.h>
#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>
#include	"../h/w_wsh.h"
#include	"wsh_defs.h"
#include	"wsh_glbl.h"
#include	"wsh_msg.h"



/*
 * Global variables.
 */

struct	globals	glbl;



/*
 * The main function.
 */

main(argc, argv)
int	argc;		/* number of arguments */
char	**argv;		/* pointer to arguments */
{
	register struct	globals	*glblp;		/* pointer to global variables */
	register	char	*p;		/* pointer to various things */
	register	char	**pp;		/* to adjust pointer arrays */
	register struct	wshhead	*fhp;		/* pointer to file header */
	char			*cp;
	register	long	diff;		/* offset when adjusting pointers */
	register		i;		/* various uses */
	register		fd;		/* file descriptor */
	struct	modstruc	mode;		/* to get the initial screen mode */
	char			*getenv();
	char			*sbrk();
	int			redraw();
	char			*ttyname();

	glblp = &glbl;
	pp = argv;
	glblp->cmdname = *pp;		/* point to command name */

	/*
	 * Determine which language to use.
	 */
	/* glblp->language = ENGLISH; */
	if ((p = getenv("LANGUAGE")) != NULL && strcmp(p, "swedish") == 0) {
		glblp->language = SWEDISH;
	}

	/*
	 * Decode possible arguments.
	 */
	p = DFLTFILE;
	/* glblp->n_opt = NO; */
	fd = NO;
	while (*++pp != NULL) {
		if (strcmp(*pp, "-n") == 0) {
			glblp->n_opt = YES;
		}
		else {
			fd = YES;
			p = *pp;
		}
	}

	/*
	 * If the standard input isn't the console or the window handler is
	 * active (tested through the standard input), execute a shell instead.
	 */
	if (glblp->n_opt == NO && ((cp = ttyname(STDIN)) == NULL ||
	    strcmp(cp, "/dev/console") != 0 || iocmd(STDIN, NULL, 0, WINTEST) >= 0)) {
		execl("/bin/sh", "sh");
	}

	/*
	 * Change process group to avoid that the window handler sends signals
	 * to other processes.
	 */
	setpgrp();

	/*
	 * Open the data file.
	 */
	if ((i = open(p, 0)) < 0) {
		if (fd != NO || (i = open(GLBLFILE, 0)) < 0) {
			errexit(M_FOIF);	/* failed to open data file */
		}
	}

	/*
	 * Allocate memory for the file header data structure.
	 */
	if ((int)(glblp->fh = fhp = (struct wshhead *)sbrk(sizeof(struct wshhead))) < 0) {
		errexit(M_NOMEM);	/* cannot allocate memory */
	}

	/*
	 * Read the file header.
	 */
	if (read(i, (char *)fhp, sizeof(struct wshhead)) != sizeof(struct wshhead)
	    || fhp->wh_magic != MAGIC) {
		errexit(M_ILLFORM);	/* illegal format */
	}

	/*
	 * Allocate memory for the rest of the file.
	 */
	diff = fhp->wh_size - sizeof(struct wshhead) + 2;
	if ((int)sbrk(diff) < 0) {
		errexit(M_NOMEM);	/* cannot allocate memory */
	}

	/*
	 * Read the rest of the data file.
	 */
	if (read(i, (char *)(fhp + 1), diff) != diff - 2) {
		errexit(M_ILLFORM);	/* illegal format of data file */
	}
	close(i);			/* close the data file */

	/*
	 * Adjust all pointers in the read data.
	 */
	diff = (long)fhp;		/* add this value to all pointers */

	/*
	 * Adjust the pointers in the window data list.
	 */
	if (fhp->wh_wd != NULL) {
		p = (char *)fhp->wh_wd += diff;
		while (((struct windat *)p)->wd_next != NULL) {
			((struct windat *)p)->wd_fd = -1;
			p = (char *)((struct windat *)p)->wd_next += diff;
		}
		((struct windat *)p)->wd_fd = -1;
	}

	/*
	 * Adjust the pointers in the window header list.
	 */
	if (fhp->wh_hd != NULL) {
		p = (char *)fhp->wh_hd += diff;
		while (((struct headdat *)p)->hd_next != NULL) {
			p = (char *)((struct headdat *)p)->hd_next += diff;
		}
	}

	/*
	 * Adjust the pointers in the icon list.
	 */
	if (fhp->wh_ic != NULL) {
		p = (char *)fhp->wh_ic += diff;
		while (((struct icondat *)p)->ic_next != NULL) {
			p = (char *)((struct icondat *)p)->ic_next += diff;
		}
	}

	/*
	 * Adjust the pointers in the window flags list.
	 */
	if (fhp->wh_flg != NULL) {
		p = (char *)fhp->wh_flg += diff;
		while (((struct flagdat *)p)->fl_next != NULL) {
			p = (char *)((struct flagdat *)p)->fl_next += diff;
		}
	}

	/*
	 * Adjust the pointers in the zoomlist list.
	 */
	if (fhp->wh_zoom != NULL) {
		p = (char *)fhp->wh_zoom += diff;
		while (((struct zoomdat *)p)->zm_next != NULL) {
			p = (char *)((struct zoomdat *)p)->zm_next += diff;
		}
	}

	/*
	 * Adjust the pointers in the mouse pointer list.
	 */
	if (fhp->wh_mp != NULL) {
		p = (char *)fhp->wh_mp += diff;
		while (((struct mpdat *)p)->mp_next != NULL) {
			p = (char *)((struct mpdat *)p)->mp_next += diff;
		}
	}

	/*
	 * Adjust the pointers in the mouse substitute key list.
	 */
	if (fhp->wh_sub != NULL) {
		p = (char *)fhp->wh_sub += diff;
		while (((struct subdat *)p)->sk_next != NULL) {
			p = (char *)((struct subdat *)p)->sk_next += diff;
		}
	}

	/*
	 * Adjust the pointers in the background pattern list.
	 */
	if (fhp->wh_bg != NULL) {
		p = (char *)fhp->wh_bg += diff;
		while (((struct bgdat *)p)->bg_next != NULL) {
			p = (char *)((struct bgdat *)p)->bg_next += diff;
		}
	}

	/*
	 * Adjust the pointers in the string data list.
	 */
	if (fhp->wh_str != NULL) {
		p = (char *)fhp->wh_str += diff;
		while (((struct strdat *)p)->st_next != NULL) {
			((struct strdat *)p)->st_str += diff;
			p = (char *)((struct strdat *)p)->st_next += diff;
		}
		((struct strdat *)p)->st_str += diff;
	}

	/*
	 * Adjust pointers in the portrait string list.
	 */
	if (fhp->wh_pstr != NULL) {
		p = (char *)fhp->wh_pstr += diff;
		while (((struct strdat *)p)->st_next != NULL) {
			((struct strdat *)p)->st_str += diff;
			p = (char *)((struct strdat *)p)->st_next += diff;
		}
		((struct strdat *)p)->st_str += diff;
	}

	/*
	 * Adjust the pointers in the landscape string list.
	 */
	if (fhp->wh_lstr != NULL) {
		p = (char *)fhp->wh_lstr += diff;
		while (((struct strdat *)p)->st_next != NULL) {
			((struct strdat *)p)->st_str += diff;
			p = (char *)((struct strdat *)p)->st_next += diff;
		}
		((struct strdat *)p)->st_str += diff;
	}

	/*
	 * Adjust pointers in the pathname data list.
	 */
	if (fhp->wh_dir != NULL) {
		p = (char *)fhp->wh_dir += diff;
		while (((struct strdat *)p)->st_next != NULL) {
			((struct strdat *)p)->st_str += diff;
			p = (char *)((struct strdat *)p)->st_next += diff;
		}
		((struct strdat *)p)->st_str += diff;
	}

	/*
	 * Adjust pointers in the environment data list.
	 */
	if (fhp->wh_env != NULL) {
		p = (char *)fhp->wh_env += diff;
		while (((struct arrdat *)p)->ca_next != NULL) {
			pp = (char **)((char *)((struct arrdat *)p)->ca_args += diff);
			while (*pp != NULL) {
				*pp++ += diff;
			}
			p = (char *)((struct arrdat *)p)->ca_next += diff;
		}
		pp = (char **)((char *)((struct arrdat *)p)->ca_args += diff);
		while (*pp != NULL) {
			*pp++ += diff;
		}
	}

	/*
	 * Adjust pointers in the portrait mode environment data list.
	 */
	if (fhp->wh_penv != NULL) {
		p = (char *)fhp->wh_penv += diff;
		while (((struct arrdat *)p)->ca_next != NULL) {
			pp = (char **)((char *)((struct arrdat *)p)->ca_args += diff);
			while (*pp != NULL) {
				*pp++ += diff;
			}
			p = (char *)((struct arrdat *)p)->ca_next += diff;
		}
		pp = (char **)((char *)((struct arrdat *)p)->ca_args += diff);
		while (*pp != NULL) {
			*pp++ += diff;
		}
	}

	/*
	 * Adjust pointers in the landscape mode environment data list.
	 */
	if (fhp->wh_lenv != NULL) {
		p = (char *)fhp->wh_lenv += diff;
		while (((struct arrdat *)p)->ca_next != NULL) {
			pp = (char **)((char *)((struct arrdat *)p)->ca_args += diff);
			while (*pp != NULL) {
				*pp++ += diff;
			}
			p = (char *)((struct arrdat *)p)->ca_next += diff;
		}
		pp = (char **)((char *)((struct arrdat *)p)->ca_args += diff);
		while (*pp != NULL) {
			*pp++ += diff;
		}
	}

	/*
	 * Adjust pointers in the command data list.
	 */
	if (fhp->wh_cmd != NULL) {
		p = (char *)fhp->wh_cmd += diff;
		while (((struct arrdat *)p)->ca_next != NULL) {
			pp = (char **)((char *)((struct arrdat *)p)->ca_args += diff);
			while (*pp != NULL) {
				*pp++ += diff;
			}
			p = (char *)((struct arrdat *)p)->ca_next += diff;
		}
		pp = (char **)((char *)((struct arrdat *)p)->ca_args += diff);
		while (*pp != NULL) {
			*pp++ += diff;
		}
	}

	/*
	 * Adjust pointers in the menu list.
	 */
	if (fhp->wh_menu != NULL) {
		p = (char *)fhp->wh_menu += diff;
		while (((struct menudat *)p)->mu_next != NULL) {
			(char *)((struct menudat *)p)->mu_list += diff;
			p = (char *)((struct menudat *)p)->mu_next += diff;
		}
		(char *)((struct menudat *)p)->mu_list += diff;
	}

	/*
	 * Adjust pointers in choice list.
	 */
	if (fhp->wh_cho != NULL) {
		p = (char *)fhp->wh_cho += diff;
		while (((struct chodat *)p)->co_next != NULL) {
			p = (char *)((struct chodat *)p)->co_next += diff;
		}
	}

	/*
	 * Adjust pointers in the action list.
	 */
	if (fhp->wh_act != NULL) {
		p = (char *)fhp->wh_act += diff;
		while (((struct actdat *)p)->ac_next != NULL) {
			(char *)((struct actdat *)p)->ac_alist += diff;
			p = (char *)((struct actdat *)p)->ac_next += diff;
		}
		(char *)((struct actdat *)p)->ac_alist += diff;
	}

	/*
	 * Adjust pointers in the terminal list.
	 */
	if (fhp->wh_term != NULL) {
		p = (char *)fhp->wh_term += diff;
		while (((struct termdat *)p)->td_next != NULL) {
			(char *)((struct termdat *)p)->td_list += diff;
			p = (char *)((struct termdat *)p)->td_next += diff;
		}
		(char *)((struct termdat *)p)->td_list += diff;
	}

	/*
	 * Adjust the pointer to the init data structure.
	 */
	if (fhp->wh_ini != NULL) {
		(char *)fhp->wh_ini += diff;

		/*
		 * Activate the window handler if no '-n' option was given.
		 */
		if (glblp->n_opt == NO) {
			if ((i = open(WACTIVATE, 2)) < 0) {
				errexit(M_CANTINIT);	/* cannot activate window handler */
			}

			/*
			 * Make sure that the file descriptor of the "super"
			 * channel is SUPERCH.
			 */
			if (i != SUPERCH) {
				dup2(i, SUPERCH);
				close(i);
			}
		}

		/*
		 * Find out the current screen mode.
		 */
		if (iocmd(SUPERCH, (char *)(&mode), sizeof(struct modstruc), WINMODE) < 0) {
			errexit(M_ACCESS);
		}
		glblp->screenmode = mode.m_mode;

		/*
		 * Set up so we can redraw menu windows when necessary.
		 */
		signal(REDRAWSIG, redraw);

		/*
		 * Do the rest.
		 */
		winloop();
	}

	/*
	 * No init data structure was given or the data file was found to be
	 * of an illegal format in winloop().
	 */
	errexit(M_ILLFORM);
}	/* end of main() */



/*
 * Routine to automatically update the contents of the menu windows when they
 * have changed in some way.
 */

redraw()
{
	register struct	windat	*wdp;		/* pointer to window data */
	register struct	menudat	*mdp;		/* pointer to menu data */
	struct		menudat	*menufind();
	int			resig();

	signal(REDRAWSIG, resig);
	do {
		glbl.sigflag = NO;
		wdp = glbl.fh->wh_wd;
		while (wdp != NULL) {
			if (wdp->wd_fd >= 0 && (wdp->wd_win.w_flags & SAVETEXT) == 0) {
				if ((mdp = menufind(wdp->wd_menu)) != NULL) {
					dspstrings(wdp->wd_fd, mdp->mu_list);
				}
			}
			wdp = wdp->wd_next;
		}
	} while (glbl.sigflag != NO);
	signal(REDRAWSIG, redraw);

	return;
}	/* end of redraw() */



/*
 * Function used to trap signal during the rewriting of windows.
 */

resig()
{
	signal(REDRAWSIG, resig);
	glbl.sigflag = YES;

	return;
}	/* end of resig() */



/*
 * Open windows, execute programs, etc. as described by the data file.
 */

winloop()
{
	register struct	menudat	  *mdp;		/* pointer to menu data */
	register struct	windat	  *wdp;		/* pointer to window data */
	register struct	chodat	  *cdp;		/* pointer to a choice */
	register struct	actlist	  *alp;		/* pointer to alist of actions */
	register		  menu;		/* number of the start menu */
	register		  oldmenu;	/* the previous menu, used for
						   sequrity */
	register		  fd;		/* file descriptor */
	register		  cnt;		/* counter */
	register		  wincnt;	/* number of open menu windows */
	struct	nwstruc		  nwin;		/* to get number of open windows */
	struct	winlevel	  level;	/* to move window to top */
	struct	modstruc	  turn;		/* to turn the screen */
	static	struct	sgttyb	  tty;		/* to set raw mode no echo */
	static	struct	headstruc emptyhead;	/* to remove a window header */
	char			  *iorqst();
	struct		menudat	  *menufind();
	struct		windat	  *winfind();
	struct		chodat	  *chofind();
	struct		actdat	  *actfind();
	struct		actlist	  *getcmd();

	/*
	 * Restart from here when the screen is turned.
	 */
restart:

	/*
	 * Execute all the actions in the initialization action list.
	 */
	alp = glbl.fh->wh_ini;
	while (alp->al_code != AL_END) {
		menu = alp->al_no;

		switch (alp->al_code) {

		case AL_INVERSE:	/* inverse video */
			iocmd(SUPERCH, NULL, 0, WINIVIDEO);
			break;

		case AL_NORMAL:		/* normal video */
			iocmd(SUPERCH, NULL, 0, WINNVIDEO);
			break;

		case AL_TERM:		/* open a terminal window */
			terminal(menu);
			break;

		case AL_MENU:		/* go to a menu */
			goto endinit;

		case AL_SUBST:		/* set up substitute keys */
			if (subsetup(menu) != OK) {
				return;		/* error */
			}
			break;

		case AL_BG:		/* set up background pattern */
			bgsetup(menu);
			break;

		case AL_PNT:		/* set up mouse pointer */
			mpsetup(SUPERCH, menu);
			break;
		}

		/*
		 * The next element in the list.
		 */
		alp++;
	}

	/*
	 * Now the initialization is finished, do the rest.
	 */
endinit:
	oldmenu = 0;
	wincnt = 0;

	for ( ; ; ) {
		if ((mdp = menufind(menu)) == NULL) {
			break;		/* illegal format of data file */
		}
		if ((wdp = winfind(mdp->mu_win)) == NULL) {
			break;		/* illegal format of data file */
		}
		fd = wdp->wd_fd;
		if (fd < 0) {

			/*
			 * The window is not present on the screen.
			 */
			if ((fd = open(WMNTDIR, 2)) < 0 || winsetup(fd, mdp->mu_win, MENU) != OK) {
				menu = oldmenu;		/* we failed, go back to */
				continue;		/* the previous menu */
			}

			/*
			 * Set the window to no echo and not canonical mode.
			 */
			tty.sg_flags = RAW;
			ioctl(fd, TIOCSETP, &tty);

			/*
			 * Save the file descriptor and set the menu number
			 * to an impossible value.
			 */
			wdp->wd_menu = 0;
			wdp->wd_fd = fd;

			/*
			 * One more menu window.
			 */
			wincnt++;
		}
		else if (wdp->wd_menu != menu) {

			/*
			 * Remove the old window header and the icons.
			 */
			iorqst(fd, (char *)(&emptyhead), sizeof(struct headstruc), WINHEADER);
			iocmd(fd, NULL, 0, RMICONS);
		}

		/*
		 * Update the window if it is a new menu.
		 */
		if (wdp->wd_menu != menu) {
			wdp->wd_menu = menu;
			if (mdp->mu_hd != 0) {
				headsetup(fd, mdp->mu_hd);
			}
			if (mdp->mu_pnt != 0) {
				mpsetup(fd, mdp->mu_pnt);
			}

			/*
			 * Display all the strings.
			 */
			alp = mdp->mu_list;
			dspstrings(fd, alp);
			if (mdp->mu_act == 0) {

				/*
				 * Set up all the icons.
				 */
				cnt = 0;
				while (alp->al_code != AL_END) {
					if (alp->al_code == AL_CHOICE) {
						cnt++;
						if ((cdp = chofind(alp->al_no)) != NULL) {
							iconsetup(fd, cdp->co_icon, cnt);
						}
					}
					alp++;
				}
			}
		}	/* if (wdp->wd_menu != menu) */

		if (mdp->mu_act != 0) {

			/*
			 * Execute an action list directly.
			 */
			(struct actdat *)alp = actfind(mdp->mu_act);
			if (alp != NULL) {
				alp = ((struct actdat *)alp)->ac_alist;
			}
		}
		else {

			/*
			 * Wait until we receive a command through an icon.
			 */
			alp = getcmd(fd, mdp->mu_list);
		}

		if (alp == NULL) {

			/*
			 * Illegal format of data file.
			 */
			break;
		}

		/*
		 * Execute all the actions in the action list.
		 */
		while (alp->al_code != AL_END) {
			cnt = alp->al_no;

			switch (alp->al_code) {

			case AL_CLOSE:		/* close the menu window */
				if (fd >= 0) {
					close(fd);
					wdp->wd_fd = fd = -1;
					wincnt--;
				}
				break;

			case AL_FLAGS:		/* set up new flags for the menu window */
				if (fd >= 0) {
					flagsetup(fd, cnt);
				}
				break;

			case AL_RESTORE:	/* restore the screen */
				iocmd(SUPERCH, NULL, 0, WINRESTOR);
				break;

			case AL_INVERSE:	/* inverse video */
				iocmd(SUPERCH, NULL, 0, WINIVIDEO);
				break;

			case AL_NORMAL:		/* normal video */
				iocmd(SUPERCH, NULL, 0, WINNVIDEO);
				break;

			case AL_LOGOUT:		/* log out */
				if (glbl.n_opt != NO) {
					exit(0);	/* '-n' option ==> just exit */
				}
				if (iocmd(SUPERCH, (char *)(&nwin), sizeof(struct nwstruc), WINCNT) >= 0
				    && nwin.nw_created == wincnt) {

					/*
					 * The window handler will send a hang
					 * up signal, so ignore it.
					 */
					signal(SIGHUP, SIG_IGN);
					close(SUPERCH);	/* terminate the window handler */
					sleep(2);	/* give it time to finish */
					exit(0);
				}
				break;

			case AL_TERM:		/* open a terminal */
				terminal(cnt);
				break;

			case AL_MENU:		/* go to another menu */
				oldmenu = menu;
				menu = cnt;
				goto cont;

			case AL_TOP:		/* move menu window to top */
				if (fd >= 0) {
					iocmd(fd, (char *)(&level), sizeof(struct winlevel), WINLEVEL);
				}
				break;

			case AL_TURN:		/* turn the screen */
				if (iocmd(SUPERCH, (char *)(&nwin), sizeof(struct nwstruc), WINCNT) >= 0
				    && nwin.nw_open == wincnt) {

					/*
					 * Close all the currently open menu
					 * windows.
					 */
					wdp = glbl.fh->wh_wd;
					while (wdp != NULL) {
						if (wdp->wd_fd >= 0) {
							close(wdp->wd_fd);
							wdp->wd_fd = -1;
						}
						wdp = wdp->wd_next;
					}

					/*
					 * Now turn the screen.
					 */
					if (iocmd(SUPERCH, (char *)(&turn), sizeof(struct modstruc), WINTURN)
					    >= 0 && turn.m_rstat == W_OK) {
						glbl.screenmode = turn.m_mode;
					}
					goto restart;		/* start over */
				}
				break;

			case AL_SUBST:		/* set up new substitute keys */
				subsetup(cnt);
				break;

			case AL_BG:		/* set up a new background */
				bgsetup(cnt);
				break;

			case AL_PNT:		/* global mouse pointer */
				mpsetup(SUPERCH, cnt);
				break;
			}

			/*
			 * Next item in action list.
			 */
			alp++;
		}

cont:		;
	}	/* for ( ; ; ) */

	return;
}	/* end of winloop() */



/*
 * Open a terminal window and execute a program in it.
 */

terminal(no)
int	no;		/* number of the terminal */
{
	register struct	termdat	*tdp;	/* pointer to terminal data */
	register struct	windat	*wdp;	/* pointer to window data */
	register struct	actlist	*alp;	/* pointer to list with strings and icons */
	register	char	*cp;	/* pointer to '/win' or '/dev/null' */
	register		fd;	/* file descriptor */
	int			r;	/* return status from wait() */
	register		i;	/* loop index */
	struct		termdat	*termfind();
	struct		windat	*winfind();
	char			*inttoasc();
	char			**envsetup();

	if ((tdp = termfind(no)) == NULL) {
		return;
	}
	if ((wdp = winfind(tdp->td_win)) == NULL) {
		cp = "/dev/null";
	}
	else {
		cp = WMNTDIR;
	}

	/*
	 * Let the program fly.
	 */
	if ((i = fork()) != 0) {
		while ((fd = wait(&r)) != i && fd >= 0)
			;
		return;
	}

	/*
	 * Change the process group and do a second fork if we shall not wait
	 * for the command to finish.
	 */
	if (setpgrp() < 0 || ((tdp->td_flags & TD_WAIT) == 0 && fork() != 0)) {
		exit(0);
	}

	/*
	 * Set up all signals to default.
	 */
	for (i = 1 ; i < NSIG ; i++) {
		signal(i, SIG_DFL);
	}

	/*
	 * Ignore the signals used for special purposes if they are set up.
	 */
	if (wdp != NULL) {
		if (wdp->wd_win.w_rsig > 0 && wdp->wd_win.w_rsig < NSIG) {
			signal(wdp->wd_win.w_rsig, SIG_IGN);
		}
		if (wdp->wd_win.w_tsig > 0 && wdp->wd_win.w_tsig < NSIG) {
			signal(wdp->wd_win.w_tsig, SIG_IGN);
		}
		if (wdp->wd_win.w_ntsig > 0 && wdp->wd_win.w_ntsig < NSIG) {
			signal(wdp->wd_win.w_ntsig, SIG_IGN);
		}
		if (wdp->wd_win.w_csig > 0 && wdp->wd_win.w_csig < NSIG) {
			signal(wdp->wd_win.w_csig, SIG_IGN);
		}
	}

	/*
	 * Close all channels.
	 */
	for (i = 0 ; i < _NFILE ; i++) {
		if (i != SUPERCH || (tdp->td_flags & TD_SUPER) == 0) {
			close(i);
		}
	}

	/*
	 * Open a channel for the window or /dev/null.
	 */
	if (open(cp, 2) != 0 ||

	/*
	 * Set up a new controlling terminal.
	 */
	    fcntl(0, F_SETCT, 0) < 0 ||

	/*
	 * Duplicate the standard output and the standard error output.
	 */
	    dup(0) != 1 || dup(0) != 2 ||

	/*
	 * Create the window if appropriate.
	 */
	    (wdp != NULL && winsetup(0, tdp->td_win, TERMINAL) != OK)) {

		/*
		 * We failed.
		 */
		exit(1);
	}

	/*
	 * Set up the window header.
	 */
	if (tdp->td_head != 0) {
		headsetup(0, tdp->td_head);
	}

	/*
	 * Set up the zoom list.
	 */
	if (tdp->td_zoom != 0) {
		zoomsetup(0, tdp->td_zoom);
	}

	/*
	 * Set up a new mouse pointer.
	 */
	if (tdp->td_mp != 0) {
		mpsetup(0, tdp->td_mp);
	}

	alp = tdp->td_list;

	/*
	 * Set up the environment.
	 */
	(char **)cp = envsetup(alp);

	if (alp != NULL) {

		/*
		 * Write the specified strings in the window.
		 */
		dspstrings(0, alp);

		/*
		 * Set up all icons.
		 */
		while (alp->al_code != AL_END) {
			if (alp->al_code == AL_ICON) {
				iconsetup(0, alp->al_no, 0);
			}
			alp++;
		}
	}

	/*
	 * Change directory and execute the command.
	 */
	if (tdp->td_dir != 0 && dirchng(tdp->td_dir) != OK) {
		exit(1);
	}
	if (tdp->td_cmd != 0) {
		execcmd(tdp->td_cmd, (char **)cp);
	}
	exit(1);
}	/* end of terminal() */



/*
 * Get a command through an icon.
 * This function returns a pointer to the action list corresponding to the
 * chosen icon.
 */

struct actlist *
getcmd(fd, alp)
int		fd;	/* file descriptor for the window */
struct	actlist	*alp;	/* pointer to list containing choices (among other things) */
{
	char			c;	/* 1-character string */
	register		a;
	register struct	actlist	*alp2;	/* pointer to action list */
	register struct	chodat	*cdp;	/* pointer to choice data */
	register struct	actdat	*adp;	/* pointer to action data */
	struct		chodat	*chofind();
	struct		actdat	*actfind();

restart:
	for ( ; ; ) {
		if (read(fd, &c, 1) != 1) {
			continue;
		}
		if (c & SET_EIGHT_BIT) {
			a = (c & SEVEN_BITS);
			if (a > 0) {
				alp2 = alp;
				for ( ; ; ) {
					if (alp2->al_code == AL_END) {
						goto restart;
					}
					if (alp2->al_code == AL_CHOICE) {
						if (--a == 0) {
							break;
						}
					}
					alp2++;
				}
				if ((cdp = chofind(alp2->al_no)) == NULL ||
				    (adp = actfind(cdp->co_action)) == NULL) {
					continue;
				}
				return(adp->ac_alist);
			}
		}
	}
}	/* end of getcmd() */



/*
 * Display all strings in an action list.
 */

dspstrings(fd, alp)
register		fd;		/* file descriptor for window */
register struct	actlist	*alp;		/* pointer to action list */
{
	register struct	globals	*glblp;	/* pointer to global variables */

	glblp = &glbl;
	while (alp->al_code != AL_END) {

		switch (alp->al_code) {

		case AL_PSTRING:	/* portrait mode string */
			if (glblp->screenmode == M_PORT) {
				strout(fd, alp->al_no, glblp->fh->wh_pstr);
			}
			break;

		case AL_LSTRING:	/* landscape mode string */
			if (glblp->screenmode != M_PORT) {
				strout(fd, alp->al_no, glblp->fh->wh_lstr);
			}
			break;

		case AL_STRING:
			strout(fd, alp->al_no, glblp->fh->wh_str);
			break;
		}
		alp++;
	}

	return;
}	/* end of dspstrings() */



/*
 * First close the "super" channel then display an error on the standard error
 * output, and then exit with false status.
 */

errexit(code)
int	code;		/* code of the error message */
{
	/*
	 * The window handler will send a hangup signal, so ignore it.
	 */
	signal(SIGHUP, SIG_IGN);
	close(SUPERCH);

	/*
	 * Wait for a while so the window handler can finish.
	 */
	sleep(3);

	if (glbl.cmdname != NULL) {
		ewritstr(glbl.cmdname);
		ewritstr(": ");
	}
	ewritstr(messages[2*code+glbl.language]);

	exit(1);
}	/* end of errexit() */



/*
 * Write a message to the standard error output.
 */

ewritstr(s)
char	*s;		/* message to write */
{
	write(STDERR, s, strlen(s));

	return;
}	/* end of ewritstr() */



/*
 * Return a pointer to the window data structure indicated by the number.
 * Return NULL if we can not find it.
 */

struct windat *
winfind(no)
int	no;		/* number of window to find */
{
	register struct	windat	*wdp;

	wdp = glbl.fh->wh_wd;
	while (wdp != NULL) {
		if (wdp->wd_no == no) {
			break;
		}
		wdp = wdp->wd_next;
	}

	return(wdp);
}	/* end of winfind() */



/*
 * Create the window indicated by the number on the given channel.
 * If 'flag' is MENU, the redraw signal is handled in a special way.
 * Returns ERROR if we fail, else OK.
 */

int
winsetup(fd, no, flag)
int	fd;		/* file descriptor for the window */
int	no;		/* the number of the window data structure */
int	flag;		/* how to handle the redraw signal */
{
	register struct	windat	 *wdp;
	register struct	winstruc *wsp;
	char			 *iorqst();

	wdp = glbl.fh->wh_wd;
	while (wdp != NULL) {
		if (wdp->wd_no == no) {
			if (flag == MENU) {
				wdp->wd_win.w_tsig = 0;
				wdp->wd_win.w_ntsig = 0;
				wdp->wd_win.w_boxes &= ~BX_CLOS;
				if (wdp->wd_win.w_flags & SAVETEXT) {
					wdp->wd_win.w_rsig = 0;
				}
				else {
					wdp->wd_win.w_rsig = REDRAWSIG;
				}
			}
			if ((wsp = (struct winstruc *)iorqst(fd, (char *)(&wdp->wd_win),
			    sizeof(struct winstruc), WINCREAT)) == NULL || wsp->w_rstat != W_OK) {
				break;		/* error */
			}
			return(OK);
		}
		wdp = wdp->wd_next;
	}

	return(ERROR);
}	/* end of winsetup() */



/*
 * Write the window header indicated by the number.
 */

headsetup(fd, no)
int	fd;		/* file descriptor for the window */
int	no;		/* the number of the header */
{
	register struct	headdat	*hdp;
	char			*iorqst();

	hdp = glbl.fh->wh_hd;
	while (hdp != NULL) {
		if (hdp->hd_no == no) {
			iorqst(fd, (char *)(&hdp->hd_head), sizeof(struct headstruc), WINHEADER);
			break;
		}
		hdp = hdp->hd_next;
	}

	return;
}	/* end of headsetup() */



/*
 * Set up the icon indicated by the number. If code is non-zero it is
 * inserted in the icon string with the high bit set. Otherwise the
 * icon string is left unaffected.
 */

iconsetup(fd, no, code)
int	fd;		/* file descriptor for the window */
int	no;		/* number of the icon */
int	code;		/* code to insert in icon string */
{
	register struct	icondat	*idp;
	char			*iorqst();

	idp = glbl.fh->wh_ic;
	while (idp != NULL) {
		if (idp->ic_no == no) {
			if (code != 0) {
				idp->ic_icon.i_cmdseq[0] = code | SET_EIGHT_BIT;
				idp->ic_icon.i_cmdseq[1] = '\0';
			}
			iorqst(fd, (char *)(&idp->ic_icon), sizeof(struct winicon), WINICON);
			break;
		}
		idp = idp->ic_next;
	}

	return;
}	/* end of iconsetup() */



/*
 * Change the window flags to the ones indicated by the number.
 */

flagsetup(fd, no)
int	fd;		/* file descriptor for the window */
int	no;		/* number of the flags */
{
	register struct	flagdat	*fdp;
	char			*iorqst();

	fdp = glbl.fh->wh_flg;
	while (fdp != NULL) {
		if (fdp->fl_no == no) {
			iorqst(fd, (char *)(&fdp->fl_flags), sizeof(struct flgstruc), WINFLAGS);
			break;
		}
		fdp = fdp->fl_next;
	}

	return;
}	/* end of flagsetup() */



/*
 * Set up the zoom list indicated by the number.
 */

zoomsetup(fd, no)
int	fd;		/* file descriptor for the window */
int	no;		/* number of the zoom list */
{
	register struct	zoomdat	*zdp;
	char			*iorqst();

	zdp = glbl.fh->wh_zoom;
	while (zdp != NULL) {
		if (zdp->zm_no == no) {
			iorqst(fd, (char *)(&zdp->zm_zoom), sizeof(struct zoomlst), WINZOOM);
			break;
		}
		zdp = zdp->zm_next;
	}

	return;
}	/* end of zoomsetup() */



/*
 * Set up the mouse pointer indicated by the number.
 */

mpsetup(fd, no)
int	fd;		/* file descriptor for the window or the "super" channel */
int	no;		/* number of the mouse pointer */
{
	register struct	mpdat	*mdp;
	char			*iorqst();

	mdp = glbl.fh->wh_mp;
	while (mdp != NULL) {
		if (mdp->mp_no == no) {
			iorqst(fd, (char *)(&mdp->mp_mp), sizeof(struct npstruc), WINCHMPNT);
			break;
		}
		mdp = mdp->mp_next;
	}

	return;
}	/* end of mpsetup() */



/*
 * Set up the mouse substitute keys indicated by the number.
 * Returns ERROR if we fail, otherwise OK.
 */

int
subsetup(no)
int	no;		/* number of the substitute keys set */
{
	register struct	subdat	*skp;
	char			*iorqst();

	skp = glbl.fh->wh_sub;
	while (skp != NULL) {
		if (skp->sk_no == no) {
			if (iorqst(SUPERCH, (char *)(&skp->sk_subst),
			    sizeof(struct substit), WINMSUB) == NULL) {
				break;		/* error */
			}
			return(OK);
		}
		skp = skp->sk_next;
	}

	return(ERROR);
}	/* end of subsetup() */



/*
 * Set up the background pattern indicated by the number.
 */

bgsetup(no)
int	no;		/* number of the background pattern */
{
	register struct	bgdat	*bdp;
	char			*iorqst();

	bdp = glbl.fh->wh_bg;
	while (bdp != NULL) {
		if (bdp->bg_no == no) {
			iorqst(SUPERCH, (char *)(&bdp->bg_bgd), sizeof(struct chbgstruc), WINCHBG);
			break;
		}
		bdp = bdp->bg_next;
	}

	return;
}	/* end of bgsetup() */



/*
 * Write the string indicated by the number to a window.
 * The check if the string shall be written (relevant for portrait and
 * landscape mode strings) must be made on a higher level.
 */

strout(fd, no, sdp)
int			fd;	/* file descriptor of the window */
int			no;	/* number of the string */
register struct	strdat	*sdp;	/* pointer to the string data list */
{
	while (sdp != NULL) {
		if (sdp->st_no == no) {
			write(fd, sdp->st_str, strlen(sdp->st_str));
			break;
		}
		sdp = sdp->st_next;
	}

	return;
}	/* end of strout() */



/*
 * Change to the directory indicated by the number.
 * Returns ERROR if we fail, else OK.
 */

int
dirchng(no)
int	no;		/* number of the directory */
{
	register struct	strdat *ddp;

	ddp = glbl.fh->wh_dir;
	while (ddp != NULL) {
		if (ddp->st_no == no) {
			if (chdir(ddp->st_str) < 0) {
				break;		/* error */
			}
			return(OK);
		}
		ddp = ddp->st_next;
	}

	return(ERROR);
}	/* end of dirchng() */



/*
 * Execute the command indicated by the number.
 * If we fail, exit with false status.
 */

execcmd(no, envp)
int	no;		/* number of the command */
char	**envp;		/* pointer to environment */
{
	register struct	arrdat	*cdp;
	extern		char	**environ;

	cdp = glbl.fh->wh_cmd;
	while (cdp != NULL) {
		if (cdp->ca_no == no) {
			environ = envp;
			execvp(*(cdp->ca_args), cdp->ca_args + 1);
			break;
		}
		cdp = cdp->ca_next;
	}

	exit(1);	/* we failed to execute */
}	/* end of execcmd() */



/*
 * Return a pointer to the menu data structure indicated by the number.
 * Return NULL if we cannot find it.
 */

struct menudat *
menufind(no)
int	no;		/* number of the menu */
{
	register struct	menudat	*mdp;

	mdp = glbl.fh->wh_menu;
	while (mdp != NULL) {
		if (mdp->mu_no == no) {
			break;		/* found the menu */
		}
		mdp = mdp->mu_next;
	}

	return(mdp);
}	/* end of menufind() */



/*
 * Find a choice data structure.
 */

struct chodat *
chofind(no)
int	no;		/* number of the choice */
{
	register struct chodat	*cdp;

	cdp = glbl.fh->wh_cho;
	while (cdp != NULL) {
		if (cdp->co_no == no) {
			break;		/* found the choice */
		}
		cdp = cdp->co_next;
	}

	return(cdp);
}	/* end of chofind() */



/*
 * Find an action data structure.
 */

struct actdat *
actfind(no)
int	no;		/* the number of the action */
{
	register struct	actdat	*adp;

	adp = glbl.fh->wh_act;
	while (adp != NULL) {
		if (adp->ac_no == no) {
			break;		/* found the action */
		}
		adp = adp->ac_next;
	}

	return(adp);
}	/* end of actfind() */



/*
 * Find a terminal data structure.
 */

struct termdat *
termfind(no)
int	no;		/* number of the terminal */
{
	register struct	termdat	*tdp;

	tdp = glbl.fh->wh_term;
	while (tdp != NULL) {
		if (tdp->td_no == no) {
			break;		/* found the terminal */
		}
		tdp = tdp->td_next;
	}

	return(tdp);
}	/* end of termfind() */



/*
 * Set up the environment.
 */

char **
envsetup(alp)
register struct	actlist	*alp;	/* pointer to action list */
{
	register char	**pp1;
	register char	**pp2;
	register char	**pp3;
	extern	 char	**environ;	/* pointer to old environment */
	register	cnt;		/* to keep track of allocated environment */
	char		*sbrk();

	/*
	 * First find out the number of variables in the environment.
	 */
	pp1 = pp2 = environ;
	cnt = 0;
	do {
		cnt++;
	} while (*pp1++ != NULL);

	/*
	 * Allocate memory to hold the new environment.
	 */
	if ((int)(pp1 = pp3 = (char **)sbrk((cnt + 20) * sizeof(char *))) < 0) {
		return(pp2);
	}

	/*
	 * Copy the environment pointers to the new area.
	 */
	while ((*pp1++ = *pp2++) != NULL)
		;

	/*
	 * Now replace or add the new variables.
	 */
	cnt = 20;
	while (alp->al_code != AL_END) {

		switch (alp->al_code) {

		case AL_ENV:
			cnt = insenv(alp->al_no, pp3, cnt, glbl.fh->wh_env);
			break;

		case AL_PENV:
			if (glbl.screenmode == M_PORT) {
				cnt = insenv(alp->al_no, pp3, cnt, glbl.fh->wh_penv);
			}
			break;

		case AL_LENV:
			if (glbl.screenmode != M_PORT) {
				cnt = insenv(alp->al_no, pp3, cnt, glbl.fh->wh_lenv);
			}
			break;
		}

		if (cnt < 0) {

			/*
			 * Out of memory, use the original environment instead.
			 */
			return(environ);
		}

		/*
		 * Next element in action list.
		 */
		alp++;
	}

	return(pp3);
}	/* end of envsetup() */



/*
 * Insert all the variables in an array into the new environment.
 */

int
insenv(no, envp, cnt, elstp)
int			no;	/* the number of the variables array */
char			**envp;	/* pointer to environment to modify */
register		cnt;	/* number of elements left in array */
register struct	arrdat	*elstp;	/* pointer to linked list of var. arrays */
{
	register char	**cpp1;		/* pointer into list of arrays to search */
	register char	**cpp2;		/* pointer into new environment */
	register char	*cp;		/* temporary */
	char		*sbrk();

	/*
	 * First find the environment data structure indicated by 'no'.
	 */
	while (elstp != NULL) {
		if (elstp->ca_no == no) {

			/*
			 * Found it.
			 */
			cpp1 = elstp->ca_args;
			while (*cpp1 != NULL) {
				cpp2 = envp;
				while ((cp = *cpp2) != NULL) {
					if (eqcompar(*cpp1, *cpp2) == 0) {
						break;		/* hit */
					}
					cpp2++;
				}

				/*
				 * Add or insert the new variable.
				 */
				*cpp2 = *cpp1;
				if (cp == NULL) {
					if (--cnt <= 0) {

						/*
						 * We must allocate more memory for
						 * the array.
						 */
						if ((int)sbrk(20 * sizeof(char *)) < 0) {
							return(-1);
						}
						cnt = 20 - 1;
					}
					*(cpp2 + 1) = NULL;
				}
				cpp1++;
			}
			break;
		}
		elstp = elstp->ca_next;
	}

	return(cnt);
}	/* end of insenv() */



/*
 * Check if two strings are equal up to and including an equal sign.
 */

int
eqcompar(cp1, cp2)
register char	*cp1;		/* pointer to strings to compare */
register char	*cp2;
{
	while (*cp1 != '\0' && *cp2 != '\0' && *cp1 == *cp2 && *cp1 != '=') {
		cp1++;
		cp2++;
	}

	return(*cp1 - *cp2);
}	/* end of eqcompar() */



/*
 * Copy the contents pointed to by 'p' and of size 'size' to a temporary area.
 * Then execute the specified I/O control request with the file descriptor
 * given.
 * This function returns a pointer to the copied data or NULL if the request
 * failed.
 * Note: The returned pointer is guaranteed to be properly aligned, i.e. at
 * an even address.
 */

char *
iorqst(fd, p, size, cmd)
int	fd;		/* file descriptor of the window or "super" channel */
char	*p;		/* pointer to data structure */
int	size;		/* size of data structure */
int	cmd;		/* I/O control command to execute */
{
	static	union {
		short		  u1[1];
		struct	flgstruc  u2;
		struct	chbgstruc u3;
		struct	winstruc  u4;
		struct	winicon	  u5;
		struct	npstruc	  u6;
		struct	headstruc u7;
		struct	substit	  u8;
		struct	zoomlst	  u9;
	} buf;
	register char	*p1;		/* used for copying */
	register char	*p2;		/* used for copying */
	register	cnt;		/* counter */

	p1 = p;
	p2 = (char *)(&buf.u1[0]);
	cnt = size;
	while (--cnt >= 0) {
		*p2++ = *p1++;
	}
	if (iocmd(fd, (char *)(&buf.u1[0]), size, cmd) < 0) {
		return(NULL);		/* we failed */
	}

	return((char *)(&buf.u1[0]));
}	/* end of iorqst() */



/*
 * Execute the specified I/O control command. This function's only purpose is
 * to save space.
 */

int
iocmd(fd, p, size, cmd)
int	fd;		/* file descriptor of window or "super" channel */
char	*p;		/* pointer to structure */
int	size;		/* size of structure */
int	cmd;		/* I/O control command to execute */
{
	return(dnix(F_IOCR, fd, p, size, cmd, 0, 0, 0));
}	/* end of iocmd() */
