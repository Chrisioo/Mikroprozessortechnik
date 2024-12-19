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
;* Gruppen-Nr.: 			* 3. Gruppe Freitag						*
;*              			*										*
;********************************************************************
;* Name / Matrikel-Nr.: 	* Christian Petry / 3847497				*
;*							* Xudong Zhang /						*
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
STR_1           EQU         Datenanfang + 0x100
STR_2           EQU         Datenanfang + 0x200
Stack           EQU         Datenanfang + 0x300
TopStack        EQU         Stack + 0x100
Datenende       EQU         TopStack               

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
                LDR         SP, =TopStack
                LDR         R0, =STR_1
                BL          AtouI
                BL          Berechnung
                LDR         R1, =STR_2
                BL          uItoA
				
;********************************************************************
;* Ende des eigenen (Haupt-)Programms                               *
;********************************************************************
endlos			B			endlos

;********************************************************************
;* ab hier Unterprogramme                                           *
;********************************************************************

;********************************************************************
;                                                                   *
;       AtouI - Konvertiert ASCII-Zeichen in Integer                *
;                                                                   *
;********************************************************************

AtouI
                STMFD      	SP!, {R1-R4, LR}
                MOV         R1, #0
                MOV         R2, #10

convert_loop_AtouI
                LDRB        R3, [R0], #1
                CMP         R3, #0
                BEQ         done_AtouI

                SUB         R3, R3, #'0'
                CMP         R3, #9
                BHI         error

                MOV         R4, R1
                MUL         R1, R4, R2
                ADD         R1, R1, R3

                B           convert_loop_AtouI

done_AtouI
                MOV         R0, R1
                LDMFD       SP!, {R1-R4, LR}
                BX          LR

error
                MOV         R0, #0
                LDMFD       SP!, {R1-R4, LR}
                BX          LR


;********************************************************************
;                                                                   *
;       Berechnung -> Y = ((2/5) * X)²                              *
;                                                                   *
;********************************************************************

Berechnung
                STMFD       SP!, {R1-R4, LR}

                LSL         R0, R0, #16
                ASR         R0, R0, #16

                CMP         R0, #0
                RSBMI       R0, R0, #0

                MOV         R1, R0
                MOV         R2, R1, LSL #1

                LDR         R1, =0xCCCCCCCD
                UMULL       R3, R4, R2, R1
                MOV         R2, R4, LSL #2

                MUL         R0, R2, R2
                LDMFD       SP!, {R1-R4, LR}
                BX          LR


;********************************************************************
;                                                                   *
;       uItoA - Konvertiert Integer in ASCII-Zeichen                *
;                                                                   *
;********************************************************************

uItoA
                STMFD       SP!, {R1-R7, LR}
                LDR         R3, [R0]
                MOV         R4, R3
                MOV         R5, R1
                MOV         R7, R5

convert_loop_uItoA
                LDR         R6, =0xCCCCCCCD
                UMULL       R1, R2, R4, R6
                MOV         R2, R2, LSR #3
                MOV         R3, #10
                MUL         R1, R2, R3
                SUB         R1, R4, R1
                ADD         R1, R1, #'0'
                STRB        R1, [R7], #1
                MOV         R4, R2
                CMP         R4, #0
                BNE         convert_loop_uItoA

                MOV         R3, #0
                STRB        R3, [R7]

reverse_loop
                CMP         R5, R7
                BHS         done_uItoA
                LDRB        R3, [R5]
                LDRB        R1, [R7]
                STRB        R1, [R5], #1
                STRB        R3, [R7], #-1
                B           reverse_loop

done_uItoA
                LDMFD       SP!, {R1-R7, LR}
                BX          LR

;********************************************************************
;* Konstanten im CODE-Bereich                                       *
;********************************************************************
;STR_1_Input     DCB         "65535", 0x00

;********************************************************************
;* Ende der Programm-Quelle                                         *
;********************************************************************
				END