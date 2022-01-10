/* relative.c */

/*
 * This program tests the ability to add windows relative to others.
 */

#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>

#define		STDIN	0
#define		STDOUT	1

struct	winstruc	win, win2;
struct	flgstruc	flg;


main()
{
	register	i;
	register	lu;
	register char	*str;

	str = "\033[?35h";
	write(STDOUT, str, strlen(str));

	if (Winstat(STDIN, &win) < 0 || win.w_rstat != W_OK) {
		exit(1);
	}
	flg.f_flags = win.w_flags | WGROUP;
	if (Winflags(STDIN, &flg) < 0 || flg.f_rstat != W_OK) {
		exit(1);
	}
	win.wp_xsize = win.wp_vxsize = win.wl_xsize = win.wl_vxsize = 50;
	win.wp_ysize = win.wp_vysize = win.wl_ysize = win.wl_vysize = 50;
	win.wp_vxorig = win.wl_vxorig = 0;
	win.wp_vyorig = win.wl_vyorig = 0;
	win.w_border = SLBORDER;
	win.w_color = WHITE;
	win.wp_font = win.wl_font = 'A';
	win.w_flags = PMODE | LMODE | RELATIVE | NOCURSOR;

	str = "\033[?35l";
	for (i = 0 ; i < 400 ; i += 10) {
		win2 = win;
		win2.wp_xorig = win2.wl_xorig = i;
		win2.wp_yorig = win2.wl_yorig = 0;
		if ((lu = open(WMNTDIR, 2)) < 0) {
			write(STDOUT, str, strlen(str));
			exit(1);
		}
		if (Wincreat(lu, &win2) < 0 || win2.w_rstat != W_OK) {
			write(STDOUT, str, strlen(str));
			exit(1);
		}
		close(lu);
	}

	for (i = 0 ; i < 400 ; i += 10) {
		win2 = win;
		win2.wp_xorig = win2.wl_xorig = 0;
		win2.wp_yorig = win2.wl_yorig = i;
		if ((lu = open(WMNTDIR, 2)) < 0) {
			write(STDOUT, str, strlen(str));
			exit(1);
		}
		if (Wincreat(lu, &win2) < 0 || win2.w_rstat != W_OK) {
			write(STDOUT, str, strlen(str));
			exit(1);
		}
		close(lu);
	}

	for (i = 0 ; i < 400 ; i += 10) {
		win2 = win;
		win2.wp_xorig = win2.wl_xorig = i;
		win2.wp_yorig = win2.wl_yorig = i;
		if ((lu = open(WMNTDIR, 2)) < 0) {
			write(STDOUT, str, strlen(str));
			exit(1);
		}
		if (Wincreat(lu, &win2) < 0 || win2.w_rstat != W_OK) {
			write(STDOUT, str, strlen(str));
			exit(1);
		}
		close(lu);
	}

	for (i = 0 ; i < 400 ; i += 10) {
		win2 = win;
		win2.wp_xorig = win2.wl_xorig = i;
		win2.wp_yorig = win2.wl_yorig = 0;
		if ((lu = open(WMNTDIR, 2)) < 0) {
			write(STDOUT, str, strlen(str));
			exit(1);
		}
		if (Wincreat(lu, &win2) < 0 || win2.w_rstat != W_OK) {
			write(STDOUT, str, strlen(str));
			exit(1);
		}
		close(lu);
	}

	for (i = 0 ; i < 400 ; i += 10) {
		win2 = win;
		win2.wp_xorig = win2.wl_xorig = 0;
		win2.wp_yorig = win2.wl_yorig = i;
		if ((lu = open(WMNTDIR, 2)) < 0) {
			write(STDOUT, str, strlen(str));
			exit(1);
		}
		if (Wincreat(lu, &win2) < 0 || win2.w_rstat != W_OK) {
			write(STDOUT, str, strlen(str));
			exit(1);
		}
		close(lu);
	}

	for (i = 0 ; i < 400 ; i += 10) {
		win2 = win;
		win2.wp_xorig = win2.wl_xorig = i;
		win2.wp_yorig = win2.wl_yorig = i;
		if ((lu = open(WMNTDIR, 2)) < 0) {
			write(STDOUT, str, strlen(str));
			exit(1);
		}
		if (Wincreat(lu, &win2) < 0 || win2.w_rstat != W_OK) {
			write(STDOUT, str, strlen(str));
			exit(1);
		}
		close(lu);
	}

	str = "Thats all ............";
	write(STDOUT, str, strlen(str));
	sleep(5);
	str = "\033[?35l";
	write(STDOUT, str, strlen(str));

	exit(0);
}
