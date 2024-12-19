;********************************************************************
;* htw saar - Fakultaet fuer Ingenieurwissenschaften				*
;* Labor fuer Eingebettete Systeme									*
;* Mikroprozessortechnik											*
;********************************************************************
;* Assembler_Startup.S: 											*
;* Programmrumpf fuer Assembler-Programme mit dem Keil				*
;* Entwicklungsprogramm uVision fuer ARM-Mikrocontroller			*
;********************************************************************
;* Aufgabe-Nr.:         	* 1	               						*
;*              			*						    			*
;********************************************************************
;* Gruppen-Nr.: 			* 3. Gruppe Freitag 1+2 Stunde			*
;*              			*										*
;********************************************************************
;* Name / Matrikel-Nr.: 	* Christian Petry / 3847497				*
;*							* Xudong Zhang / 5014211				*
;*							*										*
;********************************************************************
;* Abgabedatum:         	* 20.12.2024            				*
;*							*										*
;********************************************************************

;********************************************************************
;* Daten-Bereich bzw. Daten-Speicher				            	*
;********************************************************************
				AREA		Daten, DATA, READWRITE
Datenanfang
STR_1           EQU         Datenanfang + 0x100                     ; Adresse für ersten String STR_1, Input
STR_2           EQU         Datenanfang + 0x200                     ; Adresse für zweiten String STR_2, Ausgabe
Stack           EQU         Datenanfang + 0x300                     ; Stack
TopStack        EQU         Stack + 0x100                           ; Obergrenze des Stacks, 256 Byte reserviert                                 

;********************************************************************
;* Programm-Bereich bzw. Programm-Speicher							*
;********************************************************************
				AREA		Programm, CODE, READONLY
				ARM
Reset_Handler	MSR			CPSR_c, #0x10	; User Mode aktivieren

;********************************************************************
;* Hier das eigene (Haupt-)Programm einfuegen   					*
;********************************************************************

main
                LDR         SP, =TopStack                           ; Stack initialisieren  
                LDR         R0, =STR_1                              ; Adresse des Strings STR_1 in R0 laden        
                BL          AtouI                                   ; Branch with Link zu AtouI
                BL          Berechnung                              ; Branch with Link zu Berechnung  
                LDR         R1, =STR_2                              ; Adresse des Strings STR_2 in R1 laden           
                BL          uItoA                                   ; Branch with Link zu uItoA
				
;********************************************************************
;* Ende des eigenen (Haupt-)Programms                               *
;********************************************************************
endlos			B			endlos                                  ; Endlosschleife              

;********************************************************************
;* ab hier Unterprogramme                                           *
;********************************************************************

;********************************************************************
;                                                                   *
;           AtouI - Konvertiert ASCII-Zeichen in Integer            *
;                                                                   *
;********************************************************************

AtouI
                STMFD      	SP!, {R1-R4, LR}                        ; Speichern von Registern R1-R4 und und Rücksprungadresse auf Stack
                MOV         R1, #0                                  ; Initialisierung von Ergebnis R1 = 0
                MOV         R2, #10                                 ; Initialisierung von Multiplikator R2 = 10            

convert_loop_AtouI
                LDRB        R3, [R0], #1                            ; Laden von aktuellem Zeichen in R3 und Inkrementierung von R0
                CMP         R3, #0x00                               ; Vergleiche R3 mit '\0'
                BEQ         done_AtouI                              ; Wenn R3 = 0, dann gehe zu done_AtouI        

                SUB         R3, R3, #'0'                            ; Konvertierung von ASCII-Zeichen in Integer
                CMP         R3, #9                                  ; Prüfen, ob R3 Zahl zwischen 0 und 9 ist
                BHI         error                                   ; Wenn R3 > 9, dann gehe zu error

                MOV         R4, R1                                  ; Initialisierung von R4 mit R1
                MUL         R1, R4, R2                              ; Berechnung von R4 * R2, speichern in R1
                ADD         R1, R1, R3                              ; Berechnung von R1 + R3, speichern in R1

                B           convert_loop_AtouI                      ; Gehe zu convert_loop_AtouI

done_AtouI
                MOV         R0, R1                                  ; Speichern von Ergebnis in R0
                LDMFD       SP!, {R1-R4, LR}                        ; Laden von Registern R1-R4 und Rücksprungadresse vom Stack
                BX          LR                                      ; Sprung zur Rücksprungadresse

error
                MOV         R0, #0                                  ; R0 = 0
                LDMFD       SP!, {R1-R4, LR}                        ; Laden von Registern R1-R4 und Rücksprungadresse vom Stack
                BX          LR                                      ; Sprung zur Rücksprungadresse


;********************************************************************
;                                                                   *
;                   Berechnung -> Y = ((2/5) * X)²                  *
;                                                                   *
;********************************************************************

Berechnung
                STMFD       SP!, {R1-R4, LR}                        ; Speichern von Registern R1-R4 und und Rücksprungadresse auf Stack

                LSL         R0, R0, #16                             ; Verschieben von R0 um 16 Bit nach links, auffüllen mit Nullen
                ASR         R0, R0, #16                             ; Verschieben von R0 um 16 Bit nach rechts, auffüllen mit Vorzeichenbit

                CMP         R0, #0                                  ; Vergleiche R0 mit 0
                RSBMI       R0, R0, #0                              ; Falls R0 < 0, dann R0 = -R0 (also positiv)

                MOV         R1, R0                                  ; R1 = Eingabewert X
                MOV         R2, R1, LSL #1                          ; R2 = 2 * X

                LDR         R1, =0xCCCCCCCD                         ; Laden von 0xCCCCCCCD in R1, Magic Number für Division durch 10
                UMULL       R3, R4, R2, R1                          ; Speichern von R2 / 10 in R3 und R4
                MOV         R2, R4, LSL #2                          ; Teile Ergebnis in R4 durch 4, speichern in R2

                MUL         R0, R2, R2                              ; Berechnung von R2 * R2, speichern in R0             
                LDMFD       SP!, {R1-R4, LR}                        ; Laden von Registern R1-R4 und Rücksprungadresse vom Stack
                BX          LR                                      ; Sprung zur Rücksprungadresse


;********************************************************************
;                                                                   *
;           uItoA - Konvertiert Integer in ASCII-Zeichen            *
;                                                                   *
;********************************************************************

uItoA
                STMFD       SP!, {R1-R7, LR}                        ; Speichern von Registern R1-R7 und und Rücksprungadresse auf Stack
                LDR         R3, [R0]                                ; Laden der Adresse von R0 in R3
                MOV         R4, R3                                  ; Laden von R3 in R4
                MOV         R5, R1                                  ; Laden von R1 in R5
                MOV         R7, R5                                  ; Laden von R5 in R7

convert_loop_uItoA
                LDR         R6, =0xCCCCCCCD                         ; Laden von 0xCCCCCCCD in R6, Magic Number für Division durch 10
                UMULL       R1, R2, R4, R6                          ; Speichern von R4 / 10 in R1 und R2
                MOV         R2, R2, LSR #3                          ; Teile Ergebnis in R2 durch 8
                MOV         R3, #10                                 ; R3 = 10
                MUL         R1, R2, R3                              ; Berechnung von R2 * 10, speichern in R1
                SUB         R1, R4, R1                              ; Berechnung von R4 - R1, speichern in R1
                ADD         R1, R1, #'0'                            ; R1 = R1 + '0'
                STRB        R1, [R7], #1                            ; Speichern der Adresse von R7 in R1 und Inkrementierung von R7
                MOV         R4, R2                                  ; Laden von R2 in R4
                CMP         R4, #0                                  ; Vergleiche R4 mit 0
                BNE         convert_loop_uItoA                      ; Wenn R4 != 0, dann gehe zu convert_loop_uItoA

                MOV         R3, #0                                  ; Ansonsten R3 = 0
                STRB        R3, [R7]                                ; Speichern der Adresse von R7 in R3

reverse_loop
                CMP         R5, R7                                  ; Vergleiche R5 mit R7
                BHS         done_uItoA                              ; Wenn R5 >= R7, dann gehe zu done_uItoA
                LDRB        R3, [R5]                                ; Laden der Adresse von R5 in R3
                LDRB        R1, [R7]                                ; Laden der Adresse von R7 in R1
                STRB        R1, [R5], #1                            ; Speichern der Adresse von R5 in R1 und Inkrementierung von R5
                STRB        R3, [R7], #-1                           ; Speichern der Adresse von R7 in R3 und Dekrementierung von R7
                B           reverse_loop                            ; Gehe zu reverse_loop

done_uItoA
                LDMFD       SP!, {R1-R7, LR}                        ; Laden von Registern R1-R7 und Rücksprungadresse vom Stack
                BX          LR                                      ; Sprung zur Rücksprungadresse

;********************************************************************
;* Konstanten im CODE-Bereich                                       *
;********************************************************************
;STR_1_Input     DCB         "65535", 0x00

;********************************************************************
;* Ende der Programm-Quelle                                         *
;********************************************************************
                ALIGN
				END