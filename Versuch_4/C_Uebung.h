#ifndef C_UEBUNG_H
#define C_UEBUNG_H

#define PERIPHERIE_CLOCK 12500000
#define DATENBITS 8

volatile unsigned int running = 0; // 1: Timer läuft, 0: Timer gestoppt
volatile unsigned int total_seconds = 0;
volatile unsigned current_digit = 0;

/* BCD-Werte auf 7-Segment */
#define SEGMENT_0 0x00FC0000U // 0
#define SEGMENT_1 0x00180000U // 1
#define SEGMENT_2 0x016C0000U // 2
#define SEGMENT_3 0x013C0000U // 3
#define SEGMENT_4 0x01980000U // 4
#define SEGMENT_5 0x01B40000U // 5
#define SEGMENT_6 0x01F40000U // 6
#define SEGMENT_7 0x001C0000U // 7
#define SEGMENT_8 0x01FC0000U // 8
#define SEGMENT_9 0x01BC0000U // 9

#define BAUDRATE_0 110U
#define BAUDRATE_1 300U
#define BAUDRATE_2 600U
#define BAUDRATE_3 1200U
#define BAUDRATE_4 2400U
#define BAUDRATE_5 4800U
#define BAUDRATE_6 9600U
#define BAUDRATE_7 19200U
#define BAUDRATE_8 38400U
#define BAUDRATE_9 57600U

#define TOTAL_SECONDS_MAX 212400U

#define TIMER_PRESCALER 12500U
#define TIMER_RESET 0x02U
#define TIMER_INTERRUPT_RESET 0x03U
#define TIMER_MATCH_VALUE 1000U
#define TIMER_STOP 0x00U
#define TIMER_START 0x01U
#define TIMER_RESET 0x02U

#define PINSEL0_EINT2_MASK 0x80000000U // P0.15 als EINT2
#define EXTMODE_EINT2_EDGE 0x04U     // EINT2 als Flanken-Interrupt
#define VIC_VECT_CNTL0_EINT2 0x30U     // EINT2 als IRQ，aktiv für Kanal 16 (=EINT2)
#define VIC_INT_ENABLE_EINT2 0x10000U  // EINT2 aktivieren

#define SIEBEN_SEGMENT_MASKE 0x01FC0000U

#define PARITY_ODD 0x01U                   // Paritätsbit ungerade
#define PARITY_EVEN 0x00U                  // Paritätsbit gerade
#define PARITY_ALWAYS_ONE 0x02U            // Paritätsbit immer 1
#define PARITY_ALWAYS_ZERO 0x03U           // Paritätsbit immer 0

void initUart0(unsigned int baudRate, unsigned short dataBits, unsigned short stopBits, unsigned short parity, unsigned short parityMode);
void UART0_sendChar(char c);
void UART0_sendString(char *s);
char UART0_receiveChar(void);
void initTimer(void);
void timerISR(void) __irq;
void initExIn(void);
void toggleStopwatch(void) __irq;
void displayTime(void);
void resetStopwatch(void);
void startStopwatch(void);
void stopStopwatch(void);
void formatTime(char *buffer, int total_seconds);
void initSeg(void);
void updateSegmentDisplay(void);
#endif // C_UEBUNG_H
