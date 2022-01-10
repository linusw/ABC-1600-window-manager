/* termio.c */

/*
 * This command reads the tty parameter setting by using the ioctl command
 * TCGETA.
 */

/* 1985-01-31, Peter Andersson, Luxor Datorer AB */


#include	<sys/types.h>
#include	<sys/termio.h>
#include	<sys/ioctl.h>

#define		STDIN		0
#define		STDOUT		1
#define		STDERROUT	2


struct	termio	t;

main(argc, argv)
int	argc;
char	**argv;
{
	register	iflag;
	register	oflag;
	register	cflag;
	register	lflag;
	register char	*s;

	if (argc == 1) {
		if (ioctl(STDIN, TCGETA, &t) == -1) {
			write(STDERROUT,  "termio: failed to read parameters\n", 34);
			exit(1);
		}

		/*
		 * Display the states of all the flags.
		 * Start with the input mode flags.
		 */
		iflag = t.c_iflag;

		s = "-ignbrk\t";
		if (iflag & IGNBRK) {
			s++;
		}
		output(s);

		s = "-brkint\t";
		if (iflag & BRKINT) {
			s++;
		}
		output(s);

		s = "-ignpar\t";
		if (iflag & IGNPAR) {
			s++;
		}
		output(s);

		s = "-parmrk\t";
		if (iflag & PARMRK) {
			s++;
		}
		output(s);

		s = "-inpck\t";
		if (iflag & INPCK) {
			s++;
		}
		output(s);

		s = "-istrip\t";
		if (iflag & ISTRIP) {
			s++;
		}
		output(s);

		s = "-inlcr\t";
		if (iflag & INLCR) {
			s++;
		}
		output(s);

		s = "-igncr\n";
		if (iflag & IGNCR) {
			s++;
		}
		output(s);

		s = "-icrnl\t";
		if (iflag & ICRNL) {
			s++;
		}
		output(s);

		s = "-iuclc\t";
		if (iflag & IUCLC) {
			s++;
		}
		output(s);

		s = "-ixon\t";
		if (iflag & IXON) {
			s++;
		}
		output(s);

		s = "-ixany\t";
		if (iflag & IXANY) {
			s++;
		}
		output(s);

		s = "-ixoff\n\n";
		if (iflag & IXOFF) {
			s++;
		}
		output(s);

		/*
		 * The output mode flags.
		 */
		oflag = t.c_lflag;

		s = "-opost\t";
		if (oflag & OPOST) {
			s++;
		}
		output(s);

		s = "-olcuc\t";
		if (oflag & OLCUC) {
			s++;
		}
		output(s);

		s = "-onlcr\t";
		if (oflag & ONLCR) {
			s++;
		}
		output(s);

		s = "-ocrnl\t";
		if (oflag & OCRNL) {
			s++;
		}
		output(s);

		s = "-onocr\t";
		if (oflag & ONOCR) {
			s++;
		}
		output(s);

		s = "-onlret\t";
		if (oflag & ONLRET) {
			s++;
		}
		output(s);
		s = "-ofill\t";
		if (oflag & OFILL) {
			s++;
		}
		output(s);

		s = "-ofdel\n";
		if (oflag & OFDEL) {
			s++;
		}
		output(s);

		if (oflag & NL1) {
			s = "nl0\t";
		}
		else {
			s = "nl1\t";
		}
		output(s);

		if ((oflag & CRDLY) == CR3) {
			s = "cr3\t";
		}
		else if (oflag & CR2) {
			s = "cr2\t";
		}
		else if (oflag & CR1) {
			s = "cr1\t";
		}
		else {
			s = "cr0\t";
		}
		output(s);

		if ((oflag & TABDLY) == TAB3) {
			s = "tab3\t";
		}
		else if (oflag & TAB2) {
			s = "tab2\t";
		}
		else if (oflag & TAB1) {
			s = "tab1\t";
		}
		else {
			s = "tab0\t";
		}
		output(s);

		if (oflag & BS1) {
			s = "bs1\t";
		}
		else {
			s = "bs0\t";
		}
		output(s);

		if (oflag & VT1) {
			s = "vt1\t";
		}
		else {
			s = "vt0\t";
		}
		output(s);

		if (oflag & FF1) {
			s = "ff1\n\n";
		}
		else {
			s = "ff0\n\n";
		}
		output(s);

		/*
		 * The hardware control flags.
		 */
		cflag = t.c_cflag;

		switch (cflag & CBAUD) {

		case B0:
			s = "b0\t";
			break;

		case B50:
			s = "b50\t";
			break;

		case B75:
			s = "b75\t";
			break;

		case B110:
			s = "b110\t";
			break;

		case B134:
			s = "b134\t";
			break;

		case B150:
			s = "b150\t";
			break;

		case B200:
			s = "b200\t";
			break;

		case B300:
			s = "b300\t";
			break;

		case B600:
			s = "b600\t";
			break;

		case B1200:
			s = "b1200\t";
			break;

		case B1800:
			s = "b1800\t";
			break;

		case B2400:
			s = "b2400\t";
			break;

		case B4800:
			s = "b4800\t";
			break;

		case B9600:
			s = "b9600\t";
			break;

		case EXTA:
			s = "exta\t";
			break;

		case EXTB:
			s = "extb\t";
			break;
		}
		output(s);

		if ((cflag & CSIZE) == CS8) {
			s = "cs8\t";
		}
		else if (cflag & CS7) {
			s = "cs7\t";
		}
		else if (cflag & CS6) {
			s = "cs6\t";
		}
		else {
			s = "cs5\t";
		}
		output(s);

		s = "-cstopb\t";
		if (cflag & CSTOPB) {
			s++;
		}
		output(s);

		s = "-cread\t";
		if (cflag & CREAD) {
			s++;
		}
		output(s);

		s = "-parenb\t";
		if (cflag & PARENB) {
			s++;
		}
		output(s);

		s = "-parodd\t";
		if (cflag & PARODD) {
			s++;
		}
		output(s);

		s = "-hupcl\t";
		if (cflag & HUPCL) {
			s++;
		}
		output(s);

		s = "-clocal\n\n";
		if (cflag & CLOCAL) {
			s++;
		}
		output(s);

		/*
		 * The line discipline modes.
		 */
		lflag = t.c_lflag;

		s = "-isig\t";
		if (lflag & ISIG) {
			s++;
		}
		output(s);

		s = "-icanon\t";
		if (lflag & ICANON) {
			s++;
		}
		output(s);

		s = "-xcase\t";
		if (lflag & XCASE) {
			s++;
		}
		output(s);

		s = "-echo\t";
		if (lflag & ECHO) {
			s++;
		}
		output(s);

		s = "-echoe\t";
		if (lflag & ECHOE) {
			s++;
		}
		output(s);

		s = "-echok\t";
		if (lflag & ECHOK) {
			s++;
		}
		output(s);

		s = "-echonl\t";
		if (lflag & ECHONL) {
			s++;
		}
		output(s);

		s = "-noflsh\n";
		if (lflag & NOFLSH) {
			s++;
		}
		output(s);

		s = "-xclude\n\n";
		if (lflag & XCLUDE) {
			s++;
		}
		output(s);

		/*
		 * The control characters.
		 */
		outschr("INTR =\t", t.c_ccs[VINTR]);
		outschr("QUIT =\t", t.c_ccs[VQUIT]);
		outschr("ERASE =\t", t.c_ccs[VERASE]);
		outschr("KILL =\t", t.c_ccs[VKILL]);
		outschr("EOF =\t", t.c_ccs[VEOF]);
		outschr("EOL =\t", t.c_ccs[VEOL]);
	}

	exit(0);
}	/* end of main() */



/*
 * Function to output a string followed by a character inside apostrophes.
 * If the character is a control character, it is displayed as '^A' (for
 * ASCII code 1. Otherwise if the character is not printable, it is displayed
 * in octal.
 */

outschr(s, c)
char	*s;	/* string to output */
int	c;	/* ASCII code for character to display */
{
	char	ch;
	char	tmpbuf[4];
	char	*tmpcp;

	c &= 0377;
	write(STDOUT, s, strlen(s));
	if (c <= '~') {
		write(STDOUT, "'", 1);
		if (c < ' ') {
			write(STDOUT, "^", 1);
			c += '@';
		}
		ch = c;
		write(STDOUT, &ch, 1);
		write(STDOUT, "'", 1);
	}
	else {
		tmpcp = &tmpbuf[3];
		do {
			*tmpcp-- = c % 8 + '0';
		} while ((c /= 8) > 0);
		*tmpcp = '0';
		write(STDOUT, tmpcp, &tmpbuf[4] - tmpcp);
	}
	write(STDOUT, "\n", 1);

	return;
}	/* end of outschr() */



/*
 * Output a string to the standard output.
 */

output(s)
char	*s;
{
	write(STDOUT, s, strlen(s));

	return;
}	/* end of output() */
