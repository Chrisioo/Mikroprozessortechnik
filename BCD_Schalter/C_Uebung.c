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

unsigned char sevenSegmentLookupTable[10] = {
	0x3F, // 0
	0x06, // 1
	0x5B, // 2
	0x4F, // 3
	0x66, // 4
	0x6D, // 5
	0x7D, // 6
	0x07, // 7
	0x7F, // 8
	0x6F  // 9
};

void initLED (void);
void init7Segment (void);
void initTimer (void);
void initInterrupt (void);
int readSwitchPosition (void);
int readBCDSwitchPosition (void);
void interruptServiceRoutine (void);
void timerInterrupt (void);

int main (void)  
{
	/* Initialisierung */
	initLED();
	init7Segment();
	initTimer();
	initInterrupt();
	
 	/* Endlosschleife */	
 	while (1)  
	{
		int switchPosition = readBCDSwitchPosition();					// Read switch position
		IO1CLR = 0x00FF0000; 											// Clear LED bits
		IO1SET = (switchPosition << 16); 								// Set LED bits according to switch position
		IO1CLR = 0x000000FF; 											// Clear 7-segment display bits
		IO1SET = sevenSegmentLookupTable[switchPosition]; 				// Set 7-segment display bits
	}
}

int readSwitchPosition (void)
{
  	return (IO0PIN & 0x00000001);										// Read switch position from P0.0
																		// Return 1 if switch is pressed, 0 otherwise
}

int readBCDSwitchPosition (void)
{
  	return ((IO0PIN & 0x0000000E) >> 1);								// Read BCD switch position from P0.1 to P0.3
																		// Return switch position as integer			
}

void initLED (void)
{
  	IO1DIR |= 0x00FF0000;												// Set P1.16 to P1.23 as output					
}

void init7Segment (void)
{
  	IO1DIR |= 0x000000FF;												// Set P1.0 to P1.7 as output		
}

void initTimer (void)
{
	T0TCR = 0x02; 														// Reset Timer
	T0PR = 0x00; 														// Prescaler value
	T0MR0 = 0x00000000; 												// Match Register 0
	T0MCR = 0x03; 														// Match Control Register
	T0TCR = 0x01; 														// Enable Timer
}

void initInterrupt (void)
{
	VICIntSelect = 0x00000000; 											// IRQ
	VICIntEnable = 0x00000010; 											// Enable Timer0 Interrupt
	VICVectCntl0 = 0x20 | 4; 											// Enable Timer0 IRQ Slot
	VICVectAddr0 = 0x00; 												// Set Interrupt Vector Address
}

void interruptServiceRoutine (void)
{
	T0IR = 0x01;
	T0TCR = 0x00;
	T0MR0 = 0x00000000;
	T0TCR = 0x01;
}

void timerInterrupt (void)
{
  T0MR0 = 0x00000000;
  T0TCR = 0x01;
}
