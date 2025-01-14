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

unsigned int ledState = 0x01;
int direction = 1;
int ledCounter = 0;

void timerISR(void);

void initLED (void) {
	IODIR0 |= 0x00FF0000;		
	IOCLR1 = 0x00FF0000;
}

void init7Segment (void) {
	IODIR0 |= 0x007C0000;		
	IOCLR0 = 0x007C0000;
}

void initBCDSwitch (void) {
	IODIR0 &= ~(0x00033C00);
}

void initTimer (void) {
	T0TCR = 0x02; 
	T0PR = 12499;
	T0MR0 = 500;
	T0MCR = 0x03;
	T0TCR = 0x01;

	VICIntSelect &= ~(1 << 4);
	VICIntEnable |= (1 << 4);
	VICVectAddr4 = (unsigned int)timerISR;
}

unsigned int readBCDSwitchPosition (void) {
	unsigned int bcdValue = (IOPIN0 & 0x00033C00) >> 10;
	return bcdValue;
}

void display7Segment (unsigned int number) {
	IOCLR0 = 0x007C0000;
	IOSET0 = sevenSegmentLookupTable[number] << 18;
}

void displayLED (unsigned int number) {
	IOCLR1 = 0x00FF0000;
	IOSET1 = number << 16;
}

__irq void timerISR (void) {
	if ((IOPIN0 & (1 << 17)) != 0) {
		ledCounter = (ledCounter + 1) % 9;
		displayLED((1 << ledCounter) - 1);
	}

	T0IR = 0x01;
	VICVectAddr = 0x00;
}

int main (void)  
{
	/* Initialisierung */
	unsigned int bcdValue;

	initLED();
	init7Segment();
	initBCDSwitch();
	initTimer();
	
 	/* Endlosschleife */	
 	while (1)  
	{
		if ((IOPIN0 & (1 << 16)) != 0) {
			bcdValue = readBCDSwitchPosition();
			display7Segment(bcdValue);
		} else {
			IOCLR0 = 0x007C0000;
		}

		if ((IOPIN0 & (1 << 17)) == 0) {
			IOCLR1 = 0x00FF0000;
		}
	}
	return 0;
}
