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

#include <LPC21xx.H>		/* LPC21xx Definitionen */

#define SEVEN_SEGMENT_MASK 			0x00FC0000		// P0.18 - P0.23
#define LED_MASK 					0x00FF0000		// P1.16 - P1.23
#define BCD_SWITCH_POSITION			0x00030000		// P0.16 - P0.17	
#define BCD_MAX_VALUE				9				// Maximaler Wert fuer BCD
#define LED_MAX_VALUE				0xFF			// Maximaler auf LED darstellbarer Wert

/**
 * Lookup-Table fuer BCD-Werte
 * Segment a = Bit 18
 * Segment b = Bit 19
 * Segment c = Bit 20
 * Segment d = Bit 21
 * Segment e = Bit 22
 * Segment f = Bit 23
 * Segment g = Bit 24
 */

static const unsigned int bcdLookupTable[10] = {
    0x00FC0000,		// 0, Segmente a bis f
	0x00180000,		// 1, Segmente b-c
	0x016C0000,		// 2, Segmente a-b-d-e-g
	0x013C0000,		// 3, Segmente a-b-c-d-g
	0x01980000,		// 4, Segmente b-c-f-g
	0x01B40000,		// 5, Segmente a-c-d-f-g
	0x01F40000,		// 6, Segmente a-c-d-e-f-g
	0x001C0000,		// 7, Segmente a-b-c
	0x01FC0000,		// 8, Segmente a-b-c-d-e-f-g
	0x01BC0000		// 9, Segmente a-b-c-d-f-g
};

volatile unsigned int ledPattern = 0;
volatile unsigned int bcdSwitchPosition = 0;

// Funktionsprototypen
void initLED (void);							// Initialisierung der LED
void initBCDSwitch (void);						// Initialisierung des BCD-Schalters
void initTimer (void);							// Initialisierung des Timers
void updateBCD (unsigned int bcdValue);			// Aktualisierung des BCD
void updateLED (unsigned int ledValue);			// Aktualisierung der LED
unsigned int readBCDSwitchValue (void);			// Einlesen des BCD-Schalters
void readBCDSwitchPosition (void);				// Einlesen der BCD-Schalterposition
void T0isr (void) __irq;						// Timer-Interrupt-Service-Routine

// Initialisierung der LED
void initLED (void) {
	IODIR1 = LED_MASK;							// P1.16 - P1.23 als Ausgang setzen, IODIR1 = GPIO Port 1 Direction Control Register
	IOCLR1 = LED_MASK;							// Alle LEDs ausschalten, IOCLR1 = GPIO Port 1 Clear Register
}

// Initialisierung des BCD-Schalters
void initBCDSwitch (void) {
	IODIR0 = SEVEN_SEGMENT_MASK;				// P0.18 - P0.23 als Ausgang setzen, IODIR0 = GPIO Port 0 Direction Control Register
	IOCLR0 = SEVEN_SEGMENT_MASK;				// Alle Segmente ausschalten, IOCLR0 = GPIO Port 0 Clear Register
}

// Initialisierung des Timers
void initTimer (void) {
	T0PR = 12499;								// Prescaler fuer 1kHz, T0PR = Timer 0 Prescale Register
	T0MR0 = 500;								// Match-Register auf 500 setzen, T0MR0 = Timer 0 Match Register 0
	T0MCR = 0x00000003;							// Timer-Interrupt bei Match-Register 0, T0MCR = Timer 0 Match Control Register
	T0TCR = 0x00000001;							// Timer starten, T0TCR = Timer 0 Control Register

	VICVectAddr0 = (unsigned long) T0isr;		// Timer-Interrupt-Service-Routine setzen, VICVectAddr0 = Vector Address Register
	VICVectCntl0 = 0x00000024;					// Kanal 4 fuer Timer-Interrupt setzen, VICVectCntl0 = Vector Control Register
	VICIntEnable = 0x00000010;					// Timer-Interrupt aktivieren, VICIntEnable = Interrupt Enable Register
}

// Aktualisierung des BCD
void updateBCD (unsigned int bcdValue) {
	if (bcdValue > BCD_MAX_VALUE) {				// Check, ob BCD-Wert groesser als maximaler Wert
		bcdValue = BCD_MAX_VALUE;				// Wenn ja, dann maximalen Wert setzen
	}
	IOCLR0 = SEVEN_SEGMENT_MASK;				// Alle Segmente ausschalten, IOCLR0 = GPIO Port 0 Clear Register
	IOSET0 = bcdLookupTable[bcdValue];			// BCD-Wert setzen, wird durch Lookup-Table bestimmt, IOSET0 = GPIO Port 0 Set Register
}

// Aktualisierung der LED
void updateLED (unsigned int ledValue) {
	IOCLR1 = LED_MASK;							// Alle LEDs ausschalten, IOCLR1 = GPIO Port 1 Clear Register
	IOSET1 = ledValue << 16;					// LED-Wert setzen, wird durch Bitshift bestimmt, IOSET1 = GPIO Port 1 Set Register
}

unsigned int readBCDSwitchValue (void) {
	return (IOPIN0 >> 16) & 0x0000000F;			// BCD-Schalterwert einlesen, IOPIN0 = GPIO Port 0 Pin Value Register
												// Bitshift um 16, um nur die Bits 16-19 zu betrachten
}

void readBCDSwitchPosition (void) {
	bcdSwitchPosition = (IOPIN0 & BCD_SWITCH_POSITION) >> 16;	// BCD-Schalterposition einlesen, IOPIN0 = GPIO Port 0 Pin Value Register
																// Bitwise AND mit BCD_SWITCH_POSITION
																// Logical Shift um 16, um nur die Bits 16-17 zu betrachten

}

void T0isr (void) __irq {
	if (bcdSwitchPosition & 0x00000002) {		// Check, ob BCD-Schalterposition gesetzt ist, Bitwise AND mit 0x2
		ledPattern = (ledPattern << 1) | 1;		// LED-Pattern um 1 nach links schieben und 1 hinzufuegen

		if (ledPattern > LED_MAX_VALUE) {		// Check, ob LED-Pattern groesser als maximaler Wert
			ledPattern = 0;						// Wenn ja, dann auf 0 setzen
		}

		updateLED(ledPattern);					// LED-Pattern setzen	
	} else {
		updateLED(0);							// LED ausschalten, falls BCD-Schalterposition nicht gesetzt ist
	}
	T0IR = 0x01;								// Interrupt-Flag zuruecksetzen, T0IR = Timer 0 Interrupt Register
	VICVectAddr = 0x00000000;					// Interrupt bestaetigen, VICVectAddr = Vector Address Register
}


int main (void)  
{
	/* Initialisierung */
	unsigned int bcdInput = 0;					// BCD-Eingabe

	initLED();									// Initialisierung der LED
	initBCDSwitch();							// Initialisierung des BCD-Schalters
	initTimer();								// Initialisierung des Timers
	
 	/* Endlosschleife */	
 	while (1)  
	{
		readBCDSwitchPosition();				// BCD-Schalterposition einlesen

		if (bcdSwitchPosition & 0x00000001) {	// Check, ob BCD-Schalterposition gesetzt ist
			bcdInput = readBCDSwitchValue();	// BCD-Wert einlesen
			updateBCD(bcdInput);				// BCD-Wert setzen
		} else {								
			IOCLR0 = SEVEN_SEGMENT_MASK;		// BCD ausschalten, falls BCD-Schalterposition nicht gesetzt ist
												// IOCLR0 = GPIO Port 0 Clear Register
		}
	}
}
