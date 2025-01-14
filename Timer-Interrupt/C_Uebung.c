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

volatile unsigned int direction = 1;
volatile unsigned int currentLED = 0;

void initLED (void);
void initTimer (void);

int main (void)  
{
	/* Initialisierung */
	initLED();
	initTimer();
 	/* Endlosschleife */	
 	while (1)  
	{
		// Leeres Hauptprogramm
	}
}

void initLED (void) {
	IODIR1 |= 0x00FF0000;								// P1.16 to P1.23 as output
	IOCLR1 = 0x00FF0000;								// Turn off all LEDs
}

__irq void timerISR(void) {
	IOCLR1 = 0x00FF0000; 								// Turn off all LEDs
    IOSET1 = currentLED << 16; 							// Set LEDs

    if (direction == 1) {
        currentLED <<= 1;
        if (currentLED == 0x80) direction = -1; 		// Change direction
    } else {
        currentLED >>= 1;
        if (currentLED == 0x01) direction = 1;			// Change direction
    }

    T0IR = 0x01;          								// Clear interrupt flag
    VICVectAddr = 0x00;   								// End of Interrupt
}

void initTimer (void) {
	T0TCR = 0x02;										// Reset Timer
	T0PR = 12499;										// Prescaler
	T0MR0 = 1000;										// Match Register 0, set to 1000ms = 1s
	T0MCR = 0x03;										// Interrupt and Reset on MR0
	T0TCR = 0x01;										// Start Timer

	VICIntSelect &= ~(1 << 4);							// Timer0 selected as IRQ
	VICVectAddr4 = (unsigned int)timerISR;				// Set interrupt vector 
	VICIntEnable |= (1 << 4);							// Enable Timer0 Interrupt
}
