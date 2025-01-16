#ifndef HEADER_H
#define HEADER_H

/* Masken */
#define SEVEN_SEGMENT_MASK 			0x00FC0000		// P0.18 - P0.23, Bits 7-Segment-Anzeige
#define LED_MASK 					0x00FF0000		// P1.16 - P1.23, Bits LED

/* BCD-Position */
#define BCD_SWITCH_POSITION			0x00030000		// P0.16 - P0.17

/* Maximalwerte */
#define BCD_MAX_VALUE				9				// Maximale gebrauchter Wert fuer BCD
#define LED_MAX_VALUE				0xFF			// Maximaler auf LED darstellbarer Wert

/* Prescaler Wert */
#define PRESCALER_VALUE				12499			// Prescaler fuer 1kHz

/* BCD-Werte auf 7-Segment */
#define SEGMENT_A                    0x00040000		// P0.18
#define SEGMENT_B                    0x00080000		// P0.19
#define SEGMENT_C                    0x00100000		// P0.20
#define SEGMENT_D                    0x00200000		// P0.21
#define SEGMENT_E                    0x00400000		// P0.22
#define SEGMENT_F                    0x00800000		// P0.23
#define SEGMENT_G                    0x01000000		// P0.24

#endif
