/********************************************************************/
/*  Hochschule fuer Technik und Wirtschaft                          */
/*  Fakultaet fuer Ingenieurwissenschaften                          */
/*  Labor fuer Eingebettete Systeme                                 */
/*  Mikroprozessortechnik                                           */
/********************************************************************/
/*                                                                  */
/*  C_Uebung.C:                                                     */
/*	  Programmrumpf fuer C-Programme mit dem Keil                   */
/*    Entwicklungsprogramm uVision fuer ARM-Mikrocontroller         */
/*                                                                  */
/********************************************************************/
/*  Aufgaben-Nr.:        * 2                                        */
/*                       *                                          */
/********************************************************************/
/*  Gruppen-Nr.: 	     * 3. Gruppe Freitag 1 + 2 Stunde           */
/*                       *                                          */
/********************************************************************/
/*  Name / Matrikel-Nr.: * Christian Petry / 3847497                */
/*                       * Xudong Zhang / 5014211                   */
/*                       *                                          */
/********************************************************************/
/* 	Abgabedatum:         *                                          */
/*                       *                                          */
/********************************************************************/

#include <LPC21xx.H>		/* LPC21xx Definitionen                     */

const unsigned int sevenSegmentLookupTable[10] = {
    0x3F, // 0: 0011 1111
    0x06, // 1: 0000 0110
    0x5B, // 2: 0101 1011
    0x4F, // 3: 0100 1111
    0x66, // 4: 0110 0110
    0x6D, // 5: 0110 1101
    0x7D, // 6: 0111 1101
    0x07, // 7: 0000 0111
    0x7F, // 8: 0111 1111
    0x6F  // 9: 0110 1111
};

void initLED (void);
void init7Segment (void);
void initBCDSwitch (void);
unsigned int readBCDSwitchPosition (void);
void display7Segment (unsigned int number);
void displayLED (unsigned int number);

int main (void) {
	/* Initialisierung */
	initLED();
	init7Segment();
	initBCDSwitch();
 	/* Endlosschleife */	
 	while (1)  
	{
		unsigned int number = readBCDSwitchPosition();
		if (number < 10) {
			display7Segment(number);
			displayLED(number);
		}
	}
}

void initLED (void) {
	PINSEL2 = 0x00000000; 							// set P1.16 to P1.31 as GPIO
	IODIR1 = 0x00FF0000; 							// set P1.16 to P1.23 as output
}

void init7Segment (void) {
	PINSEL1 &= ~0x003FC000; 						// set P0.0 to P0.15 as GPIO
	IODIR0 |= 0x007C0000; 							// set P0.18 to P0.24 as output
}

void initBCDSwitch (void) {
	PINSEL0 &= ~0x000FF000; 						// set P0.10 to P0.13 as GPIO
	IODIR0 &= ~0x00003C00; 							// set P0.10 to P0.13 as input
}

unsigned int readBCDSwitchPosition (void) {
	return (IOPIN0 >> 10) & 0x0F;					// read P0.10 to P0.13
}

void display7Segment (unsigned int number) {
	IOCLR0 = 0x007C0000; 							// clear P0.18 to P0.24
	IOSET0 = sevenSegmentLookupTable[number] << 18; // set P0.18 to P0.24
}

void displayLED (unsigned int number) {
	IOCLR1 = 0x000F0000; 							// clear P1.16 to P1.23
	IOSET1 = (number << 16) & 0x000F0000; 			// set P1.16 to P1.23
}
