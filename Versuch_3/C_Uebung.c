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

#include "LPC21xx.h" /* LPC21xx Definitionen                     */

#define BAUDRATE 19200
#define DATENBITS 8
#define STOPBITS 2
#define PARITY 1
#define PERIPHERIE_CLOCK 12500000

void initUart1(int baudRate, short datenBits, short stopBits, short parity)
{

	unsigned int divisor;

	/* 1. Port-Pins für TxD und RxD konfigurieren */
	PINSEL0 |= 0x00050000; // P0.8 = TXD1, P0.9 = RXD1, to aktivieren UART1

	/* 2. Anzahl Datenbits, Stop-Bits und Parität in UxLCR einstellen dabei DLAB-Bit setzen */
	U1LCR = 0x80;					   /* DLAB = 1, um auf den Divisor zuzugreifen */
	U1LCR = ((datenBits - 5) & 0x03) | /* 5..8 Datenbits */
			((stopBits == 2) << 2) |   /* 1 oder 2 Stopbits */
			((parity != 0) << 3) |	   /* Paritaet: 0=keine, 1=gerade aktiviert */
			((parity == 2) << 4);	   /* Paritaetsmodus: 1 = ungerade, 2 = gerade */

	/* 3. Frequenzteiler für Baudrate berechnen. Low-Byte in UxDLL. High-Byte UxDLM. */
	divisor = PERIPHERIE_CLOCK / (16 * baudRate);
	U1DLM = (divisor >> 8) & 0xFF;
	U1DLL = divisor & 0xFF;

	/* 4. DLAB-Bit in UxLCR löschen */
	U1LCR &= ~0x80; // DLAB = 0

	/* 5. FIFO’s aktivieren und rücksetzen in UxFCR */
	U1FCR = 0x07; // FIFO aktivieren und RX- und TX-FIFOs loeschen

	/* 6. Wenn Interrupt ausgelöst werden soll, dann entsprechenden Interrupt in UxIER freigeben */
	// Beispiel: U1IER = 0x01; // RBR Interrupt freigeben
}

void UART1_sendChar(char c)
{
	/* Warten, bis Sendepuffer leer ist */
	while (!(U1LSR & 0x20))
		; // U1LSR: Line Status Register, 0x20: THRE (Transmitter Holding Register Empty) bit,
		  // 1: Transmitter Holding Register is empty, and the next character that is written to the THR will be transmitted out on the TXD pin.
		  // 0: Transmitter Holding Register is full. The THR is full and cannot accept any more data.
	/* Zeichen senden */
	U1THR = c;
}

void UART1_sendString(char *s)
{
	while (*s)
	{
		UART1_sendChar(*s);
		s++;
	}
}

char UART1_receiveChar()
{
	/* Warten, bis Empfangspuffer gefüllt ist */
	while (!(U1LSR & 0x01))
		;
	/* Zeichen empfangen */
	return U1RBR;
}

void UART1_sendHexDump(unsigned char *address)
{
	int i;
	char hexDigits[] = "0123456789ABCDEF";

	// send address
	for (i = 28; i >= 0; i -= 4)
	{
		UART1_sendChar(hexDigits[((unsigned int)address >> i) & 0xF]);
	}
	UART1_sendString(": ");

	// send 16 bytes of data
	for (i = 0; i < 16; i++)
	{
		unsigned char c = *address++;
		UART1_sendChar(hexDigits[(c >> 4) & 0xF]);
		UART1_sendChar(hexDigits[c & 0xF]);
		UART1_sendChar(' ');
	}

	UART1_sendString("\r\n");
}

int main(void)
{
	char input[9]; //  for 8 hex digits and a null terminator
	int index = 0;
	int i;
	unsigned int address;
	char hexDigits[] = "0123456789ABCDEF";

	/* Initialisierung */
	initUart1(BAUDRATE, DATENBITS, STOPBITS, PARITY);

	UART1_sendString("Ready to receive address:\r\n");

	/* Endlosschleife */
	while (1)
	{
		char receivedChar = UART1_receiveChar();

		UART1_sendChar(receivedChar); // echo the received character

		if (receivedChar == '\r') // CR (0x0D) stands for the end of input
		{
			input[index] = '\0'; // null-terminate the string
			address = 0;
			UART1_sendString("Received input: ");
			UART1_sendString(input);
			UART1_sendString("\r\n");

			for (i = 0; i < index; i++)
			{
				char c = input[i];
				address <<= 4;
				if (c >= '0' && c <= '9')
				{
					address |= (c - '0');
				}
				else if (c >= 'A' && c <= 'F')
				{
					address |= (c - 'A' + 10);
				}
				else if (c >= 'a' && c <= 'f')
				{
					address |= (c - 'a' + 10);
				}
			}

			// Debug: print the converted address
			UART1_sendString("Converted address: ");
			for (i = 28; i >= 0; i -= 4)
			{
				UART1_sendChar(hexDigits[(address >> i) & 0xF]);
			}
			UART1_sendString("\r\n");

			// check if the address is in the valid range
			if (address < 0x40000000 || address > 0x40007FFF)
			{
				UART1_sendString("Invalid address range.\r\n");
				index = 0; // reset index for next input
				continue;
			}

			UART1_sendHexDump((unsigned char *)address);
			index = 0; // reset index for next input
		}
		else if (index < 8)
		{ // make sure we don't overflow the input buffer
			input[index++] = receivedChar;
		}
	}
}