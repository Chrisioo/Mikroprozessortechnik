#ifndef HEADER_H
#define HEADER_H

/* Masken */
#define SIEBEN_SEGMENT_MASKE 		0x00FC0000		// P0.18 - P0.23, Bits 7-Segment-Anzeige
#define LED_MASKE 					0x00FF0000		// P1.16 - P1.23, Bits LED
#define KIPPSCHALTER_MASKE          0x00030000		// P0.16 - P0.17, Bits Kippschalter
#define BCD_DREHSCHALTER_MASKE      0x00003C00      // P0.10 - P0.13, Bits BCD-Drehschalter


/* Maximalwerte */
#define BCD_MAXIMUM				    9				// Maximale gebrauchter Wert fuer BCD
#define LED_MAXIMUM				    0xFF			// Maximaler auf LED darstellbarer Wert

/* Prescaler Wert */
#define TAKTFREQUENZ				12500			// Taktfrequenz der Peripherie in kHz
#define MATCH_WERT                  500             // Match-Wert für 0.5 Sekunden

/* BCD-Werte auf 7-Segment */
#define SEGMENT_A                   0x00040000		// P0.18
#define SEGMENT_B                   0x00080000		// P0.19
#define SEGMENT_C                   0x00100000		// P0.20
#define SEGMENT_D                   0x00200000		// P0.21
#define SEGMENT_E                   0x00400000		// P0.22
#define SEGMENT_F                   0x00800000		// P0.23
#define SEGMENT_G                   0x01000000		// P0.24

#endif
