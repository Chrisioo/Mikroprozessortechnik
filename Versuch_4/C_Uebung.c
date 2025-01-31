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

const unsigned int bcdLookupTable[10] = {
	SEGMENT_0, SEGMENT_1, SEGMENT_2, SEGMENT_3, SEGMENT_4,
	SEGMENT_5, SEGMENT_6, SEGMENT_7, SEGMENT_8, SEGMENT_9};

unsigned int readBCD(void)
{
	return (IOPIN0 >> 10) & 0x0F; // P0.10 - P0.13 BCDSWITCH
}

int getBaudRate(int bcdSwitch)
{
	switch (bcdSwitch)
	{
	case 0:
		return 110;
	case 1:
		return 300;
	case 2:
		return 600;
	case 3:
		return 1200;
	case 4:
		return 2400;
	case 5:
		return 4800;
	case 6:
		return 9600;
	case 7:
		return 19200;
	case 8:
		return 38400;
	case 9:
		return 57600;
	default:
		return 57600;
	}
}

void initUart0(unsigned int baudRate, unsigned short dataBits, unsigned short stopBits, unsigned short parity, unsigned short parityMode)
{
	unsigned int divisor;
	int bcdSwitch;

	/* read the current state of the switches */
	int S1 = (IOPIN0 & (1 << 16)) ? 1 : 0; // P0.16
	int S2 = (IOPIN0 & (1 << 17)) ? 1 : 0; // P0.17
	int S3 = (IOPIN1 & (1 << 25)) ? 1 : 0; // P1.25

	/* set the stop bits and parity according to the switches */
	stopBits = (S3 == 0) ? 1 : 2;	// 0: 1 stop bit, 1: 2 stop bits
	parityMode = (S2 == 0) ? 0 : 1; // 0: odd, 1: even
	parity = (S1 == 0) ? 0 : 1;		// 0: no parity, 1: parity

	/* read the BCD switch */
	bcdSwitch = readBCD();
	baudRate = getBaudRate(bcdSwitch);

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

int readSwitchS1 (void) {
	return (IOPIN0 & (1 << 16)) ? 1 : 0; // P0.16
}

int readSwitchS2 (void) {
	return (IOPIN0 & (1 << 17)) ? 1 : 0; // P0.17
}

int readSwitchS3 (void) {
	return (IOPIN1 & (1 << 25)) ? 1 : 0; // P1.25
}

void UART0_sendChar(char c)
{
	while (!(U0LSR & 0x20))
		;
	U0THR = c;
}

void UART0_sendString(char *s)
{
	while (*s)
	{
		UART0_sendChar(*s);
		s++;
	}
}

char UART0_receiveChar(void)
{
	while (!(U0LSR & 0x01))
		;
	return U0RBR;
}

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
void timerISR(void)
{
	if (running)
	{
		total_seconds++;
		if (total_seconds == 212400)
		{
			total_seconds = 0;
			running = 0;
			UART0_sendString("Stoppuhr erreicht 59:59:59 und wird angehalten.\n");
		}
	}
	updateSegmentDisplay();
	T0IR = 1;		 // Interrupt-Flag löschen
	VICVectAddr = 0; // VIC-Adressregister löschen
}

/* Initialisierung des externen Interrupts */
void initExIn(void)
{
	PINSEL0 |= PINSEL0_EINT2_MASK;				   // P0.15 als EINT2
	EXTMODE |= EXTMODE_EINT2_EDGE;				   // EINT2 als Flanken-Interrupt
	VICVectCntl0 = VIC_VECT_CNTL0_EINT2;		   // EINT2 als IRQ，aktiv für Kanal 16 (=EINT2)
	VICVectAddr0 = (unsigned long)toggleStopwatch; // Adresse der ISR
	VICIntEnable = VIC_INT_ENABLE_EINT2;		   // EINT2 aktivieren
}

void toggleStopwatch(void)
{
	if (running)
	{
		stopStopwatch();
	}
	else
	{
		startStopwatch();
	}
	EXTINT = 0x4;	 // Interrupt-Flag löschen
	VICVectAddr = 0; // VIC-Adressregister löschen
}

void displayTime(void)
{
	char buffer[9];
	formatTime(buffer, total_seconds);
	UART0_sendString(buffer);
}

void resetStopwatch(void)
{
	total_seconds = 0;
	UART0_sendString("Stoppuhr zurückgesetzt.\n");
}

void startStopwatch(void)
{
	running = 1;
	T0TCR = TIMER_START;
	UART0_sendString("Stoppuhr gestartet.\n");
}

void stopStopwatch(void)
{
	running = 0;
	T0TCR = TIMER_STOP;
	UART0_sendString("Stoppuhr angehalten.\n");
}

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

int main(void)
{
	char input;

	initUart0(getBaudRate(readBCD()), DATENBITS, readSwitchS3(), readSwitchS1(), readSwitchS2()); 
	initTimer();
	initExIn();
	initSeg();

	UART0_sendString("Stopp-Uhr\n");
	UART0_sendString("Start und Anhalten durch Drücken der Interrupt-Taste\n");
	UART0_sendString("s, S - Start und Anhalten\n");
	UART0_sendString("a, A - Anzeige des Standes\n");
	UART0_sendString("r, R - Rücksetzen der Stoppuhr\n");

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
			UART0_sendString("Falsche Eingabe!\n");
			UART0_sendString("s, S - Start und Anhalten\n");
			UART0_sendString("a, A - Anzeige des Standes\n");
			UART0_sendString("r, R - Rücksetzen der Stoppuhr\n");
			break;
		}
	}
}