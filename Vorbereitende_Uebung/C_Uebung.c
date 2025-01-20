/********************************************************************/
/*  Hochschule fuer Technik und Wirtschaft                          */
/*  Fakult�t fuer Ingenieurwissenschaften                           */
/*  Labor fuer Eingebettete Systeme                                 */
/*  Mikroprozessortechnik                                           */
/********************************************************************/
/*                                                                  */
/*  C_�bung.C:                                                      */
/*	  Programmrumpf fuer C-Programme mit dem Keil                   */
/*    Entwicklungsprogramm uVision fuer ARM-Mikrocontroller         */
/*                                                                  */
/********************************************************************/
/*  Aufgaben-Nr.:        *                                          */
/*                       *                                          */
/********************************************************************/
/*  Gruppen-Nr.: 	       *                                          */
/*                       *                                          */
/********************************************************************/
/*  Name / Matrikel-Nr.: *                                          */
/*                       *                                          */
/*                       *                                          */
/********************************************************************/
/* 	Abgabedatum:         *                                          */
/*                       *                                          */
/********************************************************************/

#include <LPC21xx.H>		/* LPC21xx Definitionen                     */

#define BAUDRATE 			4800
#define DATENBITS 			8
#define STOPBITS 			1
#define PARITY 				0
#define PERIPHERIE_CLOCK 	15000000

/**
 * Initialisierung der seriellen Schnittstelle UART0
 */
void initUart0 (int baudRate, short datenBits, short stopBits, short parity) {	
	unsigned int divisor;

	/* Pinsel UART0 RXD0 und TXD0 auf P0.0 und P0.1 setzen */
	PINSEL0 |= 0x00000005;

	/* UART0 initialisieren */
	U0LCR = 0x80;	/* DLAB = 1, um auf den Divisor zuzugreifen */

	/* Baudrate einstellen */
	divisor = PERIPHERIE_CLOCK / (16 * baudRate);
	U0DLM = (divisor >> 8) & 0xFF;
	U0DLL = divisor & 0xFF;

	/* 8 Datenbits, 1 Stopbit, keine Paritaet einstellen */
	U0CLR = ((datenBits - 5) & 0x03) |			/* 5..8 Datenbits */
			((stopBits == 2) << 2) |			/* 1 oder 2 Stopbits */
			((parity != 0) << 3) |				/* Paritaet: 0=keine, 1=gerade aktiviert */
			((parity == 2) << 4);				/* Paritaetsmodus: 1 = ungerade, 2 = gerade */

	/* FIFO aktivieren und RX- und TX-FIFOs loeschen */
	U0FCR = 0x07;
}

void UART0_sendChar(char c) {
	/* Warten, bis Sendepuffer leer ist */
	while (!(U0LSR & 0x20));
	
	/* Zeichen senden */
	U0THR = c;
}

char UART0_receiveChar() {
	/* Warten, bis Zeichen empfangen wurde */
	while (!(U0LSR & 0x01));
	
	/* Zeichen empfangen */
	return U0RBR;
}

int main (void)  
{
	/* Initialisierung */
	initUart0(BAUDRATE, DATENBITS, STOPBITS, PARITY);

	/* Ausgabe Chars '0' bis '9' */
	for (char c = '0'; c <= '9'; c++) {
		UART0_sendChar(c);
	}

 	/* Endlosschleife */	
 	while (1)  {
		char receivedChar = UART0_receiveChar();
		UART0_sendChar(receivedChar);
	}
}
