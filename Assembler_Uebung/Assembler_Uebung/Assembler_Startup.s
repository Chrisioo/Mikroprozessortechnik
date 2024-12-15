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
STR_1			DCB			"65535", 0
STR_2 			SPACE		16

;********************************************************************
;* Programm-Bereich bzw. Programm-Speicher							*
;********************************************************************
				AREA		Programm, CODE, READONLY
				ARM
Reset_Handler	MSR			CPSR_c, #0x10	; User Mode aktivieren

;********************************************************************
;* Hier das eigene (Haupt-)Programm einfuegen   					*
;********************************************************************
; Hauptprogramm
; R0 = Adresse STR_1
;AtouI - Konvertiert ASCII-Zeichen in Integer, X in R0
; Berechnung - Berechnet Wert Y in R0
; uItoA - Konvertiert Integer in ASCII-Zeichen & speichert in STR_2, Y in R1

                ENTRY				   		; Programmstart
				LDR         R0, =STR_1     	; Schreiben der Adresse von STR_1 in R0
                BL          AtouI          	; Aufruf von AtouI

                BL          Berechnung     	; Aufruf von Berechnung

                LDR         R1, =STR_2     	; Schreiben der Adresse von STR_2 in R1
                BL          uItoA          	; Aufruf von uItoA

				
;********************************************************************
;* Ende des eigenen (Haupt-)Programms                               *
;********************************************************************
endlos			B			endlos

;********************************************************************
;* ab hier Unterprogramme                                           *
;********************************************************************


;********************************************************************
;* Konstanten im CODE-Bereich                                       *
;********************************************************************
Wert			DCD			10

;********************************************************************
;* Ende der Programm-Quelle                                         *
;********************************************************************
				END