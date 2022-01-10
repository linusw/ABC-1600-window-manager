/* w_ex1.c */

/*
 * A WINDOW HANDLER EXAMPLE.
 * This program opens a small window in the middle of the screen, writes some
 * text in it, waits for the RETURN key to be pressed, and then exits.
 */

/* 1985-02-06, Peter Andersson, Luxor Datorer AB */


#include	<win/w_const.h>
#include	<win/w_types.h>
#include	<win/w_structs.h>
#include	<win/w_macros.h>

#define		STDOUT		1	/* standard output */
#define		STDERROUT	2	/* standard error output */


/*
 * Structure used to create the window.
 */

struct	winstruc win;



main()
{
	int		 fd;		/* window file descriptor */
	char		 *str;
	char		 c;

	/*
	 * Fill in the window data structure.
	 */
	win.wp_xorig = 300;
	win.wp_yorig = 450;
	win.wp_xsize = 100;
	win.wp_ysize = 100;
	win.wp_vxorig = 0;
	win.wp_vyorig = 0;
	win.wp_vxsize = 100;
	win.wp_vysize = 100;
	win.w_border = DLBORDER;
	win.w_color = WHITE;
	win.wp_font = 'A';
	win.w_boxes = BX_SIZE | BX_MOVE | BX_HSCR | BX_VSCR;
	win.w_flags = PMODE | SAVETEXT;

	/*
	 * Open a channel for the window.
	 */
	if ((fd = open("/win", 2)) == -1) {
		error("w_ex1: can't open window channel\n");
	}

	/*
	 * Create the window (on the screen).
	 */
	if (Wincreat(fd, &win) < 0 || win.w_rstat != W_OK) {
		error("w_ex1: can't create window\n");
	}

	/*
	 * Write some text in the window.
	 */
	str = "A simple\nexample..\n\nPress\nRETURN\nto exit";
	write(fd, str, strlen(str));

	/*
	 * Wait until the RETURN key is pressed.
	 */
	read(fd, &c, 1);

	/*
	 * Remove the window.
	 */
	close(fd);

	str = "That's all...\n";
	write(STDOUT, str, strlen(str));

	exit(0);
}	/* end of main() */



/*
 * Display an error message on the standard error output and then exit.
 */

error(s)
char	*s;
{
	write(STDERROUT, s, strlen(s));
	exit(1);
}	/* end of error() */
