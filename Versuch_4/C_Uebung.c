/********************************************************************/
/*  Hochschule fuer Technik und Wirtschaft                          */
/*  Fakultät fuer Ingenieurwissenschaften                           */
/*  Labor fuer Eingebettete Systeme                                 */
/*  Mikroprozessortechnik                                           */
/********************************************************************/
/*                                                                  */
/*  C_Übung.C:                                                      */
/*	  Programmrumpf fuer C-Programme mit dem Keil                   */
/*    Entwicklungsprogramm uVision fuer ARM-Mikrocontroller         */
/*                                                                  */
/********************************************************************/
/*  Aufgaben-Nr.:        * 4                                        */
/*                       *                                          */
/********************************************************************/
/*  Gruppen-Nr.: 	     *  Gruppe 3 Freitag 1+2 Stunde				*/
/*                       *                                          */
/********************************************************************/
/*  Name / Matrikel-Nr.: *          Christian Petry / 3847497       */
/*                       *          Xudong Zhang / 5014211          */
/*                       *                                          */
/********************************************************************/
/* 	Abgabedatum:         *                 31.01.2025               */
/*                       *                                          */
/********************************************************************/

#include "LPC21xx.h"  /* LPC21xx Definitionen */
#include "C_Uebung.h" /* eigene Definitionen */

// Lookup-Table für die 7-Segment-Anzeige
const unsigned int bcdLookupTable[10] = {
	SEGMENT_0, SEGMENT_1, SEGMENT_2, SEGMENT_3, SEGMENT_4,
	SEGMENT_5, SEGMENT_6, SEGMENT_7, SEGMENT_8, SEGMENT_9};

const unsigned int baudrateLookupTable[10] = {
	BAUDRATE_0, BAUDRATE_1, BAUDRATE_2, BAUDRATE_3, BAUDRATE_4,
	BAUDRATE_5, BAUDRATE_6, BAUDRATE_7, BAUDRATE_8, BAUDRATE_9};

// Methode zum Auslesen des BCD-Schalters
unsigned int readBCD(void)
{
	return (IOPIN0 >> 10) & 0x0F; // P0.10 - P0.13 BCDSWITCH
}

// Methode zum Bestimmen der Baudrate anhand des BCD-Schalters
int getBaudRate(int bcdSwitch)
{
	return baudrateLookupTable[bcdSwitch];
}

/**
 * Initialisierung der UART0
 * @param baudRate verwendete Baudrate, Bits/Sekunde
 * @param dataBits Anzahl der Datenbits
 * @param stopBits Anzahl der Stopbits
 * @param parity Paritätsbit aktivieren
 * @param parityMode Paritätsmodus: 0 = ungerade, 1 = gerade
 */
void initUart0(unsigned int baudRate, unsigned short dataBits, unsigned short stopBits, unsigned short parity, unsigned short parityMode)
{
	unsigned int divisor;

	/* 1. Port-Pins für TxD und RxD konfigurieren */
	PINSEL0 |= 0x05; // P0.0 = TXD0, P0.1 = RXD0, to aktivieren UART0

	/* 2. Anzahl Datenbits, Stop-Bits und Parität in UxLCR einstellen dabei DLAB-Bit setzen */
	U0LCR = 0x80;						/* DLAB = 1, um auf den Divisor zuzugreifen */
	U0LCR |= ((dataBits - 5) & 0x03) | /* 5..8 Datenbits */
			 ((stopBits == 2) << 2) |	/* 1 oder 2 Stopbits */
			 (parity << 3) |			/* Paritaet: 0=keine, 1=aktiviert */
			 (parityMode << 4);			/* Paritaetsmodus: 00 = ungerade, 01 = gerade */

	/* 3. Frequenzteiler für Baudrate berechnen. Low-Byte in UxDLL. High-Byte UxDLM. */
	divisor = PERIPHERIE_CLOCK / (16 * baudRate);

	U0DLM = divisor / 256;
	U0DLL = divisor % 256;

	/* 4. DLAB-Bit in UxLCR löschen */
	U0LCR &= ~0x80; // DLAB = 0

	/* 5. FIFO’s aktivieren und rücksetzen in UxFCR */
	U0FCR = 0x07; // FIFO aktivieren und RX- und TX-FIFOs loeschen

	/* 6. Wenn Interrupt ausgelöst werden soll, dann entsprechenden Interrupt in UxIER freigeben */
	// Beispiel: U0IER = 0x01; // RBR Interrupt freigeben
}

// Methode zum Auslesen des Schalters S1
int readSwitchS1 (void) {
	return (IOPIN0 & (1 << 16)) ? 1 : 0; // P0.16
}

// Methode zum Auslesen des Schalters S2
int readSwitchS2 (void) {
	return (IOPIN0 & (1 << 17)) ? 1 : 0; // P0.17
}

// Methode zum Auslesen des Schalters S3
int readSwitchS3 (void) {
	return (IOPIN1 & (1 << 25)) ? 1 : 0; // P1.25
}

// Methode zum Senden eines Zeichens über UART0
void UART0_sendChar(char c)
{
	while (!(U0LSR & 0x20))		// Warten, bis Sendepuffer leer ist
		;
	U0THR = c;					// Zeichen senden
}

// Methode zum Senden eines Strings über UART0
void UART0_sendString(char *s)
{
	while (*s)					// solange das aktuelle Zeichen nicht das Nullzeichen ist
	{
		UART0_sendChar(*s);		// sende das aktuelle Zeichen
		s++;					// gehe zum nächsten Zeichen
	}
	UART0_sendChar('\r');		// Wenn String zu Ende, dann Zeilenumbruch
}

// Methode zum Empfangen eines Zeichens über UART0
char UART0_receiveChar(void)
{
	while (!(U0LSR & 0x01))		// Warten, bis Empfangspuffer gefüllt ist
		;						
	return U0RBR;				// Zeichen empfangen
}

// Methode zum Initialisieren des Timers
void initTimer(void)
{

	T0PR = TIMER_PRESCALER;		   // Prescaler für den Timer, 12500 = 1ms
	T0TCR = TIMER_RESET;		   // Timer zurücksetzen
	T0MCR = TIMER_INTERRUPT_RESET; // Interrupt und Reset bei Match
	T0MR0 = TIMER_MATCH_VALUE;	   // 1s
	T0TCR = TIMER_STOP;			   // Timer anhalten

	/* Timer 0 Interrupt aktivieren */
	VICVectAddr4 = (unsigned long)timerISR;
	VICVectCntl4 = 0x24;
	VICIntEnable |= 0x10;
}

/* Timer-Interrupt-Service-Routine */
void timerISR(void) __irq
{
	if (running)					// Wenn die Stoppuhr läuft
	{
		total_seconds++;			// Sekunden erhöhen
		if (total_seconds == TOTAL_SECONDS_MAX)	// Wenn 59:59:59 erreicht ist
		{
			total_seconds = 0;			// Stoppuhr zurücksetzen
			running = 0;				// Stoppuhr anhalten
			UART0_sendString("Stoppuhr erreicht 59:59:59 und wird angehalten.\n");
		}
	}
	updateSegmentDisplay();				// 7-Segment-Anzeige aktualisieren
	T0IR = 1;		 // Interrupt-Flag löschen
	VICVectAddr = 0; // VIC-Adressregister löschen
}

/* Initialisierung des externen Interrupts */
void initExIn(void)
{
	PINSEL0 |= PINSEL0_EINT2_MASK;				   // P0.15 als EINT2
	EXTMODE |= EXTMODE_EINT2_EDGE;				   // EINT2 als Flanken-Interrupt
	EXTPOLAR |= EXTMODE_EINT2_EDGE;
	VICVectCntl0 = VIC_VECT_CNTL0_EINT2;		   // EINT2 als IRQ，aktiv für Kanal 16 (=EINT2)
	VICVectAddr0 = (unsigned long)toggleStopwatch; // Adresse der ISR
	VICIntEnable = VIC_INT_ENABLE_EINT2;		   // EINT2 aktivieren
}

// Methode zum Umschalten der Stoppuhr
void toggleStopwatch(void) __irq
{
	if (running)				// Wenn die Stoppuhr läuft
	{
		stopStopwatch();		// Stoppuhr anhalten
	}
	else
	{
		startStopwatch();		// Ansonsten Stoppuhr starten
	}
	EXTINT = 0x4;	 // Interrupt-Flag löschen
	VICVectAddr = 0; // VIC-Adressregister löschen
}

// Methode zum Anzeigen der Zeit
void displayTime(void)
{
	char buffer[9];				// Puffer für die Zeit initialisieren
	formatTime(buffer, total_seconds);	// Zeit formatieren
	UART0_sendString(buffer);			// Zeit über UART0 senden
}

// Methode zum Zurücksetzen der Stoppuhr
void resetStopwatch(void)
{
	total_seconds = 0;			// Sekunden zurücksetzen
	UART0_sendString("Stoppuhr zurueckgesetzt.\n");
}

// Methode zum Starten der Stoppuhr
void startStopwatch(void)
{
	running = 1;				// Stoppuhr läuft
	T0TCR = TIMER_START;		// Timer starten
	UART0_sendString("Stoppuhr gestartet.\n");
}

// Methode zum Anhalten der Stoppuhr
void stopStopwatch(void)
{
	running = 0;				// Stoppuhr anhalten
	T0TCR = TIMER_STOP;			// Timer anhalten
	UART0_sendString("Stoppuhr angehalten.\n");
}

// Methode zum Formatieren der Zeit
void formatTime(char *buffer, int total_seconds)
{
	int hours = total_seconds / 3600;
	int minutes = (total_seconds % 3600) / 60;
	int seconds = total_seconds % 60;

	buffer[0] = '0' + (hours / 10);
	buffer[1] = '0' + (hours % 10);
	buffer[2] = ':';
	buffer[3] = '0' + (minutes / 10);
	buffer[4] = '0' + (minutes % 10);
	buffer[5] = ':';
	buffer[6] = '0' + (seconds / 10);
	buffer[7] = '0' + (seconds % 10);
	buffer[8] = '\n';
	buffer[9] = '\0';
}

// Siebensegmentanzeige initialisieren
void initSeg(void)
{
	IODIR0 = SIEBEN_SEGMENT_MASKE; // P0.18 - P0.24 als Ausgang
	IOCLR0 = SIEBEN_SEGMENT_MASKE; // Alle Ausgänge auf 0 setzen, alle Segmente aus
	IOSET0 = bcdLookupTable[0];	   // Anzeige auf 0 setzen
}

// Siebensegmentanzeige aktualisieren
void updateSegmentDisplay(void)
{
	IOCLR0 = SIEBEN_SEGMENT_MASKE;			  // Alle Ausgänge auf 0 setzen
	IOSET0 = bcdLookupTable[current_digit];	  // Ausgabe des aktuellen Digits
	current_digit = (current_digit + 1) % 10; // Nächstes Digit
}

void showMenu (void) {
	UART0_sendString("Stopp-Uhr\n");
	UART0_sendString("Start und Anhalten durch Druecken der Interrupt-Taste\n");
	UART0_sendString("s, S - Start und Anhalten\n");
	UART0_sendString("a, A - Anzeige des Standes\n");
	UART0_sendString("r, R - Ruecksetzen der Stoppuhr\n");
}

int main(void)
{
	char input;

	initUart0(getBaudRate(readBCD()), DATENBITS, readSwitchS3(), readSwitchS1(), readSwitchS2()); 
	initTimer();
	initExIn();
	initSeg();
	showMenu();
	

	while (1)
	{
		input = UART0_receiveChar();

		switch (input)
		{
		case 's':
		case 'S':
			if (running)
			{
				stopStopwatch();
			}
			else
			{
				startStopwatch();
			}
			break;
		case 'a':
		case 'A':
			displayTime();
			break;
		case 'r':
		case 'R':
			if (!running)
			{
				resetStopwatch();
			}
			else
			{
				UART0_sendString("Fehler: Stoppuhr läuft noch!\n");
			}
			break;
		default:
			showMenu();
			break;
		}
	}
}
