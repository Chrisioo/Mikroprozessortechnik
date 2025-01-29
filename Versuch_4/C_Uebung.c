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
/*  Aufgaben-Nr.:        *                                          */
/*                       *                                          */
/********************************************************************/
/*  Gruppen-Nr.: 	     *                                          */
/*                       *                                          */
/********************************************************************/
/*  Name / Matrikel-Nr.: *          Christian Petry / 3847497       */
/*                       *          Xudong Zhang / 5014211          */
/*                       *                                          */
/********************************************************************/
/* 	Abgabedatum:         *                 31.01.2025               */
/*                       *                                          */
/********************************************************************/

#include "LPC21xx.h" /* LPC21xx Definitionen */

#define PERIPHERIE_CLOCK 12500000
#define DATENBITS 8

volatile int running = 0; // 1: Timer läuft, 0: Timer gestoppt
volatile int hours = 0, minutes = 0, seconds = 0;

unsigned int readBCD()
{
	return (IOPIN0 >> 10) & 0x0F;
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
		return 57600; // default to 57600
	}
}

void initUart0()
{

	unsigned int divisor;
	short datenBits = DATENBITS;
	short stopBits;
	short parity;
	short parityMode;

	/* read the current state of the switches */
	int S1 = (IOPIN0 & (1 << 16)) ? 1 : 0; // P0.16
	int S2 = (IOPIN0 & (1 << 17)) ? 1 : 0; // P0.17
	int S3 = (IOPIN1 & (1 << 25)) ? 1 : 0; // P1.25

	/* set the stop bits and parity according to the switches */
	stopBits = (S3 == 0) ? 1 : 2;	// 0: 1 stop bit, 1: 2 stop bits
	parityMode = (S2 == 0) ? 0 : 1; // 0: odd, 1: even
	parity = (S1 == 0) ? 0 : 1;		// 0: no parity, 1: parity

	/* read the BCD switch */
	int bcdSwitch = readBCD();
	int baudRate = getBaudRate(bcdSwitch);

	/* 1. Port-Pins für TxD und RxD konfigurieren */
	PINSEL0 |= 0x05; // P0.0 = TXD0, P0.1 = RXD0, to aktivieren UART0

	/* 2. Anzahl Datenbits, Stop-Bits und Parität in UxLCR einstellen dabei DLAB-Bit setzen */
	U0LCR = 0x80;						/* DLAB = 1, um auf den Divisor zuzugreifen */
	U0LCR |= ((datenBits - 5) & 0x03) | /* 5..8 Datenbits */
			 ((stopBits == 2) << 2) |	/* 1 oder 2 Stopbits */
			 ((parity != 0) << 3) |		/* Paritaet: 0=keine, 1=gerade aktiviert */
			 ((parity == 1) << 4);		/* Paritaetsmodus: 00 = ungerade, 01 = gerade */

	/* 3. Frequenzteiler für Baudrate berechnen. Low-Byte in UxDLL. High-Byte UxDLM. */
	divisor = PERIPHERIE_CLOCK / (16 * baudRate);
	U0DLM = (divisor >> 8) & 0xFF;
	U0DLL = divisor & 0xFF;

	/* 4. DLAB-Bit in UxLCR löschen */
	U0LCR &= ~0x80; // DLAB = 0

	/* 5. FIFO’s aktivieren und rücksetzen in UxFCR */
	U0FCR = 0x07; // FIFO aktivieren und RX- und TX-FIFOs loeschen

	/* 6. Wenn Interrupt ausgelöst werden soll, dann entsprechenden Interrupt in UxIER freigeben */
	// Beispiel: U1IER = 0x01; // RBR Interrupt freigeben
}

void UART0_sendChar(char c)
{
	/* Warten, bis Sendepuffer leer ist */
	while (!(U0LSR & 0x20))
		; // U1LSR: Line Status Register, 0x20: THRE (Transmitter Holding Register Empty) bit,
		  // 1: THR is empty, and the next character that is written to the THR will be transmitted out on the TXD pin.
		  // 0: THR is full. The THR is full and cannot accept any more data.
	/* Zeichen senden */
	U0THR = c;
}

void UART0_sendString(char *s)
{

	while (*s) // while the string is not null-terminated
	{
		UART0_sendChar(*s);
		s++;
	}
}

char UART0_receiveChar()
{
	/* Warten, bis Empfangspuffer gefüllt ist */
	while (!(U0LSR & 0x01))
		;
	/* Zeichen empfangen */
	return U0RBR;
}

void initTimer()
{
	T0MR0 = PERIPHERIE_CLOCK;				// 1秒
	T0MCR = 3;								// Interrupt und Reset bei MR0
	T0TCR = 1;								// Timer starten
	VICVectAddr0 = (unsigned long)timerISR; // ISR-Adresse
	VICVectCntl0 = 0x20 | 4;				// Slot 0, Timer 0
	VICIntEnable = 1 << 4;					// Timer 0 Interrupt freigeben
}

void timerISR(void)
{
	if (running)
	{
		seconds++;
		if (seconds == 60)
		{
			seconds = 0;
			minutes++;
			if (minutes == 60)
			{
				minutes = 0;
				hours++;
				if (hours == 60)
				{
					hours = 0;
					running = 0;
					UART0_sendString("Stoppuhr erreicht 59:59:59 und wird angehalten.\n");
				}
			}
		}
	}
	T0IR = 1;		 // Interrupt-Flag löschen
	VICVectAddr = 0; // VIC-Adressregister löschen
}

void formatTime(char *buffer, int hours, int minutes, int seconds)
{
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

void displayTime(void)
{
	char buffer[9];
	formatTime(buffer, hours, minutes, seconds);
	UART0_sendString(buffer);
}

void resetStopwatch(void)
{
	hours = 0;
	minutes = 0;
	seconds = 0;
	UART0_sendString("Stoppuhr zurückgesetzt.\n");
}

void startStopwatch(void)
{
	running = 1;
	UART0_sendString("Stoppuhr gestartet.\n");
}

void stopStopwatch(void)
{
	running = 0;
	UART0_sendString("Stoppuhr angehalten.\n");
}