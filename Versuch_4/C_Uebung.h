#ifndef C_UEBUNG_H
#define C_UEBUNG_H

#define PERIPHERIE_CLOCK 12500000
#define DATENBITS 8

volatile unsigned int running = 0; // 1: Timer läuft, 0: Timer gestoppt
volatile unsigned int total_seconds = 0;
volatile unsigned current_digit = 0;

/* BCD-Werte auf 7-Segment */
#define SEGMENT_0 0x00FC0000 // 0
#define SEGMENT_1 0x00180000 // 1
#define SEGMENT_2 0x016C0000 // 2
#define SEGMENT_3 0x013C0000 // 3
#define SEGMENT_4 0x01980000 // 4
#define SEGMENT_5 0x01B40000 // 5
#define SEGMENT_6 0x01F40000 // 6
#define SEGMENT_7 0x001C0000 // 7
#define SEGMENT_8 0x01FC0000 // 8
#define SEGMENT_9 0x01BC0000 // 9

#define TIMER_PRESCALER 12500
#define TIMER_RESET 0x02
#define TIMER_INTERRUPT_RESET 0x03
#define TIMER_MATCH_VALUE 1000
#define TIMER_STOP 0x00
#define TIMER_START 0x01
#define TIMER_RESET 0x02

#define PINSEL0_EINT2_MASK 0x80000000 // P0.15 als EINT2
#define EXTMODE_EINT2_EDGE 0x04       // EINT2 als Flanken-Interrupt
#define VIC_VECT_CNTL0_EINT2 0x30     // EINT2 als IRQ，aktiv für Kanal 16 (=EINT2)
#define VIC_INT_ENABLE_EINT2 0x10000  // EINT2 aktivieren

#define SIEBEN_SEGMENT_MASKE 0x01FC0000

#define PARITY_ODD 0x01UL                   // Paritätsbit ungerade
#define PARITY_EVEN 0x00UL                  // Paritätsbit gerade
#define PARITY_ALWAYS_ONE 0x02UL            // Paritätsbit immer 1
#define PARITY_ALWAYS_ZERO 0x03UL           // Paritätsbit immer 0

void initUart0(void);
void UART0_sendChar(char c);
void UART0_sendString(char *s);
char UART0_receiveChar(void);
void initTimer(void);
void timerISR(void);
void initExIn(void);
void toggleStopwatch(void);
void displayTime(void);
void resetStopwatch(void);
void startStopwatch(void);
void stopStopwatch(void);
void formatTime(char *buffer, int total_seconds);
void initSeg(void);
void updateSegmentDisplay(void);
#endif // C_UEBUNG_H