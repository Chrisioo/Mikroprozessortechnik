#ifndef HEADER_H
#define HEADER_H

#define BAUDRATE 19200UL                    // Vorgegebene Baudrate, Anzahl übertragener Bits pro Sekunde
#define DATENBITS 8UL                       // Vorgegebene Anzahl an Datenbits
#define STOPBITS 2UL                        // Vorgegebene Anzahl an Stopbits
#define PARITYMODE 0x01UL                   // Vorgegebene Paritätsbits
#define PARITY 0x00UL                       // Vorgegebene Parität
#define PERIPHERIE_CLOCK 12500000UL         // Vorgegebene Peripherie-Taktfrequenz, 1/4 der CPU-Frequenz

#define PARITY_ODD 0x01UL                   // Paritätsbit ungerade
#define PARITY_EVEN 0x00UL                  // Paritätsbit gerade
#define PARITY_ALWAYS_ONE 0x02UL            // Paritätsbit immer 1
#define PARITY_ALWAYS_ZERO 0x03UL           // Paritätsbit immer 0

#define ADRESS_MIN 0x40000000UL             // Vorgegebene Mindestadresse
#define ADRESS_MAX 0x40007FFFUL             // Vorgegebene Maximaladresse

#endif
