/* kybrdcodes.h */

/*
 * This file contains all the command codes which the window handler sends to
 * the ABC99 keyboard.
 */

/* 1985-03-08, Peter Andersson, Luxor Datorer AB */



#define		K_BELL		0x07	/* produce an alarm signal */
#define		K_POSM		0x39	/* write data to mouse pos. reg. */
#define		K_PREM		0x3a	/* write dato to prescale reg. */
#define		K_LIMM		0x3b	/* write data to limit reg. */
#define		K_RDMS		0x3c	/* read data from mouse */
#define		K_OFINS		0x09	/* turn off INS key LED */
#define		K_ONINS		0x89	/* turn on INS key LED */
#define		K_OFALT		0x0a	/* turn off ALT key LED */
#define		K_ONALT		0x8a	/* turn on ALT key LED */
#define		K_ONLD1		0x00	/* light LED1 */
#define		K_OFLD1		0x80	/* turn off LED1 */
#define		K_ONLD2		0x10	/* light LED2 */
#define		K_OFLD2		0x90	/* turn off LED2 */
#define		K_ONLD3		0x20	/* light LED3 */
#define		K_OFLD3		0xa0	/* turn off LED3 */
#define		K_ONLD4		0x30	/* light LED4 */
#define		K_OFLD4		0xb0	/* turn off LED4 */
#define		K_ONLD5		0x40	/* light LED5 */
#define		K_OFLD5		0xc0	/* turn off LED5 */
#define		K_ONLD6		0x50	/* light LED6 */
#define		K_OFLD6		0xd0	/* turn off LED6 */
#define		K_ONLD7		0x60	/* light LED7 */
#define		K_OFLD7		0xe0	/* turn off LED7 */
#define		K_ONLD8		0x70	/* light LED8 */
#define		K_OFLD8		0xf0	/* turn off LED8 */
