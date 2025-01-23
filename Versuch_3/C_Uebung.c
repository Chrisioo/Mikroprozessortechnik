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

#include <LPC21xx.H> /* LPC21xx Definitionen                     */

#define BAUDRATE 19200
#define DATENBITS 8
#define STOPBITS 2
#define PARITY 1
#define PERIPHERIE_CLOCK 12500000

void initUart0(int baudRate, short datenBits, short stopBits, short parity)
{

	unsigned int divisor;

	/* 1. Port-Pins für TxD und RxD konfigurieren */
	PINSEL0 |= 0x00050000; // P0.8 = TXD1, P0.9 = RXD1

	/* 2. Anzahl Datenbits, Stop-Bits und Parität in UxLCR einstellen dabei DLAB-Bit setzen */
	U1LCR = 0x80;					   /* DLAB = 1, um auf den Divisor zuzugreifen */
	U0LCR = ((datenBits - 5) & 0x03) | /* 5..8 Datenbits */
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

void UART0_sendChar(char c)
{
	/* Warten, bis Sendepuffer leer ist */
	while (!(U0LSR & 0x20))
		;
	/* Zeichen senden */
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

char UART0_receiveChar()
{
	/* Warten, bis Empfangspuffer gef�llt ist */
	while (!(U0LSR & 0x01))
		;
	/* Zeichen empfangen */
	return U0RBR;
}
void UART0_sendHex(unsigned char value)
{
	char hexDigits[] = "0123456789ABCDEF";
	UART0_sendChar(hexDigits[value >> 4]);	 // send high 4 bits
	UART0_sendChar(hexDigits[value & 0x0F]); // send low 4 bits
}
void UART0_sendHexDump(unsigned char *addr)
{
	int i;

	// send address
	for (i = 28; i >= 0; i -= 4)
	{
		UART0_sendChar("0123456789ABCDEF"[((unsigned int)addr >> i) & 0xF]);
	}
	UART0_sendString(": ");

	// send 16 bytes of hex data
	for (i = 0; i < 16; i++)
	{
		UART0_sendHex(addr[i]);
		UART0_sendChar(' ');
	}

	UART0_sendString("\r\n\r\n");
}

int main(void)
{
	char input[9]; //  for 8 hex digits and a null terminator
	int index = 0;
	int i;
	unsigned int address;
	/* Initialisierung */
	initUart0(BAUDRATE, DATENBITS, STOPBITS, PARITY);

	/* Endlosschleife */
	while (1)
	{
		char receivedChar = UART0_receiveChar();

		if (receivedChar == '\r')
		{						 // CR (0x0D) stands for the end of input
			input[index] = '\0'; // null-terminate the string
			address = 0;
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
			UART0_sendHexDump((unsigned char *)address);
			index = 0; // reset index for next input
		}
		else if (index < 8)
		{ // make sure we don't overflow the input buffer
			input[index++] = receivedChar;
		}
	}
}
