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
#include "Header.h"

void initUart1(unsigned int baudRate, unsigned short datenBits, unsigned short stopBits, unsigned short parity, unsigned short parityMode) {

	unsigned int divisor;

	/* 1. Port-Pins für TxD und RxD konfigurieren */
	PINSEL0 |= 0x00050000; 	// P0.8 = TXD1, P0.9 = RXD1, um UART1 zu nutzen
						    
	/* 2. Anzahl Datenbits, Stop-Bits und Parität in UxLCR einstellen dabei DLAB-Bit setzen */
	U1LCR = 0x80;						/* DLAB = 1, um auf den Divisor zuzugreifen */
	U1LCR |= ((datenBits - 5) & 0x03) | /* 5..8 Datenbits */
			 ((stopBits == 2) << 2) |	/* 1 oder 2 Stopbits */
			 ((parity != 0) << 3) |	/* Paritaetsmodus: 0 = kein Paritaetsbit, 1 = Paritaetsbit */
			 (parityMode<< 4);		/* Paritaet: 0 = ungerade, 1 = gerade */

	/* 3. Frequenzteiler für Baudrate berechnen. Low-Byte in UxDLL. High-Byte UxDLM. */
	divisor = PERIPHERIE_CLOCK / (16 * baudRate);
	U1DLM = divisor / 256;
	U1DLL = divisor % 256;

	/* 4. DLAB-Bit in UxLCR löschen */
	U1LCR &= ~0x80; // DLAB = 0

	/* 5. FIFO’s aktivieren und rücksetzen in UxFCR */
	U1FCR = 0x07; // FIFO aktivieren und RX- und TX-FIFOs loeschen

	/* 6. Wenn Interrupt ausgelöst werden soll, dann entsprechenden Interrupt in UxIER freigeben */
	// Beispiel: U1IER = 0x01; // RBR Interrupt freigeben
}

// Sendet ein Zeichen über UART1
void UART1_sendChar(char c) {
	/* Warten, bis Sendepuffer leer ist */
	while (!(U1LSR & 0x20))
		; // U1LSR: Line Status Register, 0x20: THRE (Transmitter Holding Register Empty) bit,
		  // 1: THR is empty, and the next character that is written to the THR will be transmitted out on the TXD pin.
		  // 0: THR is full. The THR is full and cannot accept any more data.
	/* Zeichen senden */
	U1THR = c;
}

// Sendet einen String über UART1
void UART1_sendString(char *s)
{

	while (*s) { // solange das aktuelle Zeichen nicht das Nullzeichen ist
		UART1_sendChar(*s);	// sende das aktuelle Zeichen
		s++;				// gehe zum nächsten Zeichen
	}
}

// Empfaengt ein Zeichen über UART1
char UART1_receiveChar() {
	/* Warten, bis Empfangspuffer gefüllt ist */
	while (!(U1LSR & 0x01))
		;
	/* Zeichen empfangen */
	return U1RBR;			// Zurueckgeben des empfangenen Zeichens
}

// Sendet einen Hex-Dump über UART1
void UART1_sendHexDump(unsigned char *address) {
	int i;
	char hexDigits[] = "0123456789ABCDEF"; // used as a lookup table for converting a nibble to a hex digit

	/* send address */
	for (i = 28; i >= 0; i -= 4) { // Loop through the 32-bit address in 4-bit nibbles
		UART1_sendChar(hexDigits[((unsigned int)address >> i) & 0xF]);	// send the hex digit corresponding to the current nibble
	}
	UART1_sendString(": "); // send a colon and a space

	/* send 16 bytes of data */
	for (i = 0; i < 16; i++) { // Loop through the 16 bytes of data
		unsigned char c = *address++;				// get the current byte of data
		UART1_sendChar(hexDigits[(c >> 4) & 0xF]);	// send the high nibble
		UART1_sendChar(hexDigits[c & 0xF]);			// send the low nibble
		UART1_sendChar(' ');						// send a space		
	}

	UART1_sendString("\r\n");						// send a newline
}

int main(void) {
	char input[9]; // initialize an input buffer with 8 characters and a null terminator
	short index = 0;	// initialize the index of the input buffer
	short i;
	unsigned int address;
	char hexDigits[] = "0123456789ABCDEF"; // used as a lookup table for converting a nibble to a hex digit

	/* Initialisierung des UART1 */
	initUart1(BAUDRATE, DATENBITS, STOPBITS, PARITY, PARITY_EVEN);

	UART1_sendString("0123456789\r\n");		// Test string
	UART1_sendString("Ready to receive address:\r\n"); // Print a message to indicate that the program is ready to receive an address

	/* Endlosschleife */
	while (1) {
		char receivedChar = UART1_receiveChar();	// receive a character from UART1

		UART1_sendChar(receivedChar); // echo the received character

		if (receivedChar == '\r') {  // CR (0x0D) stands for the end of input
			input[index] = '\0'; // null-terminate the string
			address = 0;

			/* Debug: print the received input */
			UART1_sendString("Received input: ");
			UART1_sendString(input);
			UART1_sendString("\r\n");

			for (i = 0; i < index; i++) {
				char c = input[i];
				address <<= 4;
				if (c >= '0' && c <= '9') {
					address |= (c - '0');
				} else if (c >= 'A' && c <= 'F') {
					address |= (c - 'A' + 10);
				} else if (c >= 'a' && c <= 'f') {
					address |= (c - 'a' + 10);
				}
			}

			/* Debug: print the converted address */
			UART1_sendString("Converted address: ");
			for (i = 28; i >= 0; i -= 4) {
				UART1_sendChar(hexDigits[(address >> i) & 0xF]);
			}
			UART1_sendString("\r\n");

			/* check if the address is in the valid range */
			if (address < ADRESS_MIN || address > ADRESS_MAX) {
				UART1_sendString("Invalid address range.\r\n");
				index = 0; // reset index for next input
				continue;
			}

			UART1_sendHexDump((unsigned char *)address);
			index = 0; // reset index for next input
		} else if (index < 8) { /* make sure we don't overflow the input buffer */
			input[index++] = receivedChar;
		}
	}
}
