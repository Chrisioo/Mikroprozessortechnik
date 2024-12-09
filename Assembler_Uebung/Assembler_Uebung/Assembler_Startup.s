;********************************************************************
;* htw saar - Fakultaet fuer Ingenieurwissenschaften				*
;* Labor fuer Eingebettete Systeme									*
;* Mikroprozessortechnik											*
;********************************************************************
;* Assembler_Startup.S: 											*
;* Programmrumpf fuer Assembler-Programme mit dem Keil				*
;* Entwicklungsprogramm uVision fuer ARM-Mikrocontroller			*
;********************************************************************
;* Aufgabe-Nr.:         	*	               						*
;*              			*						    			*
;********************************************************************
;* Gruppen-Nr.: 			*										*
;*              			*										*
;********************************************************************
;* Name / Matrikel-Nr.: 	*										*
;*							*										*
;*							*										*
;********************************************************************
;* Abgabedatum:         	*              							*
;*							*										*
;********************************************************************

;********************************************************************
;* Daten-Bereich bzw. Daten-Speicher				            	*
;********************************************************************
				AREA		Daten, DATA, READWRITE
Datenanfang
Quadrat			EQU			Datenanfang

;********************************************************************
;* Programm-Bereich bzw. Programm-Speicher							*
;********************************************************************
				AREA		Programm, CODE, READONLY
				ARM
Reset_Handler	MSR			CPSR_c, #0x10	; User Mode aktivieren

;********************************************************************
;* Hier das eigene (Haupt-)Programm einfuegen   					*
;********************************************************************

				LDR			R3,=Wert 		; Adresse des Werts laden
				LDR	   		R1,[R3]			; Wert laden
				MUL			R2,R1,R1		; Quadrat bilden
				LDR     	R3,=Quadrat		; Adresse des Quadrats laden
				STR			R2,[R3]			; Quadrat ablegen
				BL			Demo_UP
				
;********************************************************************
;* Ende des eigenen (Haupt-)Programms                               *
;********************************************************************
endlos			B			endlos

;********************************************************************
;* ab hier Unterprogramme                                           *
;********************************************************************
Demo_UP			MVN    		R4, #0
				BX   		LR

;********************************************************************
;* Konstanten im CODE-Bereich                                       *
;********************************************************************
Wert			DCD			10

;********************************************************************
;* Ende der Programm-Quelle                                         *
;********************************************************************
				END
