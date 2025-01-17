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
#define SEGMENT_0                   0x00FC0000      // 0
#define SEGMENT_1                   0x00180000      // 1
#define SEGMENT_2                   0x016C0000      // 2
#define SEGMENT_3                   0x013C0000      // 3
#define SEGMENT_4                   0x01980000      // 4
#define SEGMENT_5                   0x01B40000      // 5
#define SEGMENT_6                   0x01F40000      // 6
#define SEGMENT_7                   0x001C0000      // 7
#define SEGMENT_8                   0x01FC0000      // 8
#define SEGMENT_9                   0x01BC0000      // 9

#endif
