/********************************************************************/
/*  Hochschule fuer Technik und Wirtschaft                          */
/*  Fakultät fuer Ingenieurwissenschaften                           */
/*  Labor fuer Eingebettete Systeme                                 */
/*  Mikroprozessortechnik                                           */
/********************************************************************/
/*                                                                  */
/*  C_Uebung.C:                                                      */
/*    Programmrumpf fuer C-Programme mit dem Keil                   */
/*    Entwicklungsprogramm uVision fuer ARM-Mikrocontroller         */
/*                                                                  */
/********************************************************************/
/*  Aufgaben-Nr.:        *                                          */
/*                       *                                          */
/********************************************************************/
/*  Name / Matrikel-Nr.: *      Christian Petry / 3847497         	*/
/*                       *      Xudong Zhang / 5014211         		*/
/*                       *                                          */
/********************************************************************/
/*  Abgabedatum:         *      17.01.2025               			*/
/*                       *                                          */
/********************************************************************/
#include <LPC21xx.H>
#include "Header.h"

/** Lookup-Table fuer BCD-Werte auf 7-Segment-Anzeige
Segment a = Bit 18
Segment b = Bit 19
Segment c = Bit 20
Segment d = Bit 21
Segment e = Bit 22
Segment f = Bit 23
Segment g = Bit 24*/
static const unsigned int bcdLookupTable[10] = {
    SEGMENT_0,
    SEGMENT_1,
    SEGMENT_2,
    SEGMENT_3,
    SEGMENT_4,
    SEGMENT_5,
    SEGMENT_6,
    SEGMENT_7,
    SEGMENT_8,
    SEGMENT_9
};


	
// Funktionsprototypen
void initLED(void);        							   		// LED-Initialisierung
void init7Segment(void);                                 	// 7-Segment-Anzeige-Initialisierung
void update7Segment(unsigned int value); 			        // 7-Segment-Anzeige aktualisieren
void updateLED(unsigned int pattern);              			// LEDs aktualisieren
unsigned int readBCDInput(void);                    		// Eingabe einlesen
unsigned int readSwitchState(void);                         // Schalter-Status einlesen
void initTimer(void);                      				  	// Timer initialisieren
void T0isr(void) __irq;                    				  	// Timer-Interrupt-Service-Routine

// LED-Ausgänge initialisieren
void initLED(void) {
    IODIR1 = LED_MASKE;                      				// P1.16-P1.23 als Ausgang definieren
    IOCLR1 = LED_MASKE;                      			  	// Alle LEDs initial ausschalten
}

// 7-Segment-Ausgänge initialisieren
void init7Segment(void) {
    IODIR0 = SIEBEN_SEGMENT_MASKE;                			// P0.18-P0.24 als Ausgang definieren
    IOCLR0 = SIEBEN_SEGMENT_MASKE;                  		// BCD-Anzeige initial löschen
}

// 7-Segment-Anzeige aktualisieren
void update7Segment(unsigned int value) {
    if (value > BCD_MAXIMUM) {								// Check ob Wert in darsellbarem Bereich
		value = BCD_MAXIMUM; 								// Wenn nicht, Wert auf Maximum setzen
	}
    IOCLR0 = SIEBEN_SEGMENT_MASKE;                  		// Alte Anzeige löschen
    IOSET0 = bcdLookupTable[value];             			// Neuen Wert setzen
}

// LED-Muster aktualisieren
void updateLED(unsigned int ledMuster) {
    IOCLR1 = LED_MASKE;      								// Alle LEDs ausschalten
    IOSET1 = (ledMuster << 16); 							// Neues Muster auf P1.16-P1.23 setzen
}

// Eingabewert von BCD-Schalter einlesen
unsigned int readBCDInput(void) {
    return (IOPIN0 >> 10) & 0xF;  				        	// Bits 10-13 ausmaskieren
}

// Eingabe von Kipp-Schalter einlesen
unsigned int readSwitchState(void) {
    return (IOPIN0 & KIPPSCHALTER_MASKE) >> 16;  			// Bits 16-17 ausmaskieren
}

// Timer-Interrupt-Service-Routine
void T0isr(void) __irq {
	unsigned int switchState = readSwitchState();           // Schalter-Status einlesen, in switchState speichern
	static unsigned int ledMuster = 0;             			// Aktuelles LED-Muster
															// LED-Muster bei aktivem Schalter aktualisieren
    if (switchState & 0x2) {								// Check ob Schalter aktiv
        ledMuster = (ledMuster << 1) + 1;   				// Wenn ja, Muster um 1 nach links schieben und 1 hinzufügen	 																				   
        if (ledMuster > LED_MAXIMUM) {						// Check, ob Muster größer als LED_MAXIMUM
            ledMuster = 0;									// Wenn ja, Muster auf 0 setzen, erneuter Durchlauf
        }													
        updateLED(ledMuster);								// Darstellung des Musters auf LED-Anzeige	
    } else {
        updateLED(0);										// Wenn Schalter nicht aktiv, alle LEDs ausschalten
    }                                            
    T0IR = 0x01;											// Interrupt-Flag löschen
    VICVectAddr = 0x00;										// ISR beenden
}

// Timer initialisieren
void initTimer(void) {
    // Timer-Konfiguration
    T0PR = TAKTFREQUENZ - 1;    							// Prescaler für 1 kHz (1 MS pro Tick) 
															// Timer zählt alle 12500 Takte einmal
    T0MR0 = MATCH_WERT;                                   	// Match-Wert für 0,5 Sekunden ausgelöst
	
															// Match Control Register:
															// Bit 0: Aktiviert Interrupt für M0
															// Bit 1: Setzt Timer Zähler nach Match zurück
    T0MCR = 0x03;                                  			// Interrupt und Reset bei Match
    T0TCR = 0x01;                                  			// Timer starten

    // Interrupt-Konfiguration
    VICVectAddr4 = (unsigned long)T0isr;           			// Speichern Adresse für ISR Kanal 0, 
															// T0isr als ISR für Timer 0 registriert
    VICVectCntl4 = 0x24;                           			// Kanal 4 aktivieren - Bit 5
															// Quelle festlegen -> 4 für Timer 0
    VICIntEnable = 0x10;                           			// Timer 0 Interrupt aktivieren
															// Jedes Bit : entsprechender Kanal
}

int main(void) {
    unsigned int bcdInput = 0;
	unsigned int switchState = readSwitchState();
    
	// Hardware initialisieren
    initLED();
    init7Segment();
    initTimer();

    while (1) {
		// Schalter-Status aktualisieren
		switchState = readSwitchState();	
		// BCD-Anzeige aktualisieren
        if (switchState & 0x1) {
            bcdInput = readBCDInput();
            update7Segment(bcdInput);
        } else {
            IOCLR0 = SIEBEN_SEGMENT_MASKE; 			        	 // BCD-Anzeige löschen
        }
    }
}
