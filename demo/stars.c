/* star.c */

/*
 * This program rotates a star using the mover escape sequence.
 */


#include	<sgtty.h>

struct	sgttyb	tty;	/* to set raw mode, no echo */


main()
{
	register char	*str;
	register	loop;
	register	i;

	/*
	 * Set raw mode, no echo.
	 */
	tty.sg_flags = RAW;
	ioctl(0, TIOCSETP, &tty);

	/*
	 * Star one.
	 */
	str = "\33:150;100m\33:115;115;;1d\33:100;150;;1d\33:85;115;;1d\33:50;100;;1d";
	write(1, str, strlen(str));
	str = "\33:85;85;;1d\33:100;50;;1d\33:115;85;;1d\33:150;100;;1d";
	write(1, str, strlen(str));

	/*
	 * Star two.
	 */
	str = "\33:274;108m\33:237;117;;1d\33:217;149;;1d\33:208;112;;1d\33:176;92;;1d";
	write(1, str, strlen(str));
	str = "\33:213;83;;1d\33:233;51;;1d\33:242;88;;1d\33:274;108;;1d";
	write(1, str, strlen(str));

	/*
	 * Star three.
	 */
	str = "\33:396;117m\33:359;119;;1d\33:333;146;;1d\33:331;109;;1d\33:304;83;;1d";
	write(1, str, strlen(str));
	str = "\33:341;81;;1d\33:367;54;;1d\33:369;91;;1d\33:396;117;;1d";
	write(1, str, strlen(str));

	/*
	 * Star four.
	 */
	str = "\33:143;250m\33:105;245;;1d\33:75;268;;1d\33:80;230;;1d\33:57;200;;1d";
	write(1, str, strlen(str));
	str = "\33:95;205;;1d\33:125;182;;1d\33:120;220;;1d\33:143;250;;1d";
	write(1, str, strlen(str));

	/*
	 * Star five.
	 */
	str = "\33:263;257m\33:227;246;;1d\33:193;263;;1d\33:204;227;;1d\33:187;193;;1d";
	write(1, str, strlen(str));
	str = "\33:223;204;;1d\33:257;187;;1d\33:246;223;;1d\33:263;257;;1d";
	write(1, str, strlen(str));

	/*
	 * Star six.
	 */
	str = "\33:382;263m\33:348;246;;1d\33:312;257;;1d\33:329;223;;1d\33:318;187;;1d";
	write(1, str, strlen(str));
	str = "\33:352;204;;1d\33:388;193;;1d\33:371;227;;1d\33:382;263;;1d";
	write(1, str, strlen(str));

	/*
	 * Star seven.
	 */
	str = "\33:125;393m\33:94;370;;1d\33:57;375;;1d\33:80;344;;1d\33:75;307;;1d";
	write(1, str, strlen(str));
	str = "\33:106;330;;1d\33:143;325;;1d\33:120;356;;1d\33:125;393;;1d";
	write(1, str, strlen(str));

	/*
	 * Star eight.
	 */
	str = "\33:242;396m\33:216;369;;1d\33:178;367;;1d\33:206;341;;1d\33:208;304;;1d";
	write(1, str, strlen(str));
	str = "\33:234;331;;1d\33:272;333;;1d\33:244;359;;1d\33:242;396;;1d";
	write(1, str, strlen(str));

	/*
	 * Star nine.
	 */
	str = "\33:358;399m\33:338;368;;1d\33:301;358;;1d\33:332;338;;1d\33:342;301;;1d";
	write(1, str, strlen(str));
	str = "\33:362;332;;1d\33:399;342;;1d\33:368;362;;1d\33:358;399;;1d";
	write(1, str, strlen(str));

	/*
	 * Move the different stars to the same area to make it rotate.
	 */
	for (loop = 0 ; loop < 5 ; loop++) {
		for (i = 0 ; i < 10 ; i++) {
			str = "\33:50;50;500;125;100;100;0r";
			write(1, str, strlen(str));
			str = "\33:175;50;500;125;100;100;0r";
			write(1, str, strlen(str));
			str = "\33:300;50;500;125;100;100;0r";
			write(1, str, strlen(str));
			str = "\33:50;175;500;125;100;100;0r";
			write(1, str, strlen(str));
			str = "\33:175;175;500;125;100;100;0r";
			write(1, str, strlen(str));
			str = "\33:300;175;500;125;100;100;0r";
			write(1, str, strlen(str));
			str = "\33:50;300;500;125;100;100;0r";
			write(1, str, strlen(str));
			str = "\33:175;300;500;125;100;100;0r";
			write(1, str, strlen(str));
			str = "\33:300;300;500;125;100;100;0r";
			write(1, str, strlen(str));
		}
		sleep(5);
	}

	exit(0);
}
