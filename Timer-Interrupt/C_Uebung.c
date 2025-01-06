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
void updateLED (void);
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
	PINSEL2 = 0x00000000;		
	IODIR0 = 0x00FF0000;		
}

void updateLED (void) {
	IOCLR0 = 0x00FF0000;		
	IOSET0 = (1 << (currentLED + 16));

	if (direction == 1) {
		if (currentLED < 7) {
			currentLED++;
		} else {
			direction = 0;
		}
	} else {
		if (currentLED > 0) {
			currentLED--;
		} else {
			direction = 1;
		}
	}
}

void TIMER0_IRQHandler(void) {
	if (T0IR & 0x01) {
		T0IR = 0x01;
		updateLED();
	}
	VICVectAddr = 0;
}

void initTimer (void) {
	T0TCR = 0x02;			
	T0PR = 12499;		
	T0MR0 = 1000;		
	T0MCR = 0x03;			
	T0TCR = 0x01;			
	VICIntSelect &= ~0x10;
	VICVectAddr4 = (unsigned int)TIMER0_IRQHandler;
	VICVectCntl4 = 0x20 | 4;
	VICIntEnable = 0x10;
}
