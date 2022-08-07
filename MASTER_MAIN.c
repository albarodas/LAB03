/*
 * File:   MAIN_MASTER.c
 * Author: ALBA RODAS 
 * Created on 3 de agosto de 2022, 10:49 AM
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIGURATION WORDS: 1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIGURATION WORDS: 2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// ====================================================
// LIBRERIAS:
// ====================================================
// INCLUIMOS LIBRERIAS:
#include <xc.h>
#include <stdint.h>
#include <pic16f887.h>
#include "oscilador.h"
#include "SPI.h"
// ====================================================
// OSCILADOR:
// ====================================================
#define _XTAL_FREQ 1000000
// ====================================================
// VARIABLES:
// ====================================================
uint8_t variable_ADRESH = 0;
uint8_t variable_ADRESL = 0; 
uint8_t devolver = 0;
uint8_t lectura = 0;

// ====================================================
// PROTOTIPOS DE FUNCIONES:
// ====================================================
void setup(void); 
// ====================================================
// CICLO PRINCIPAL:
// ====================================================
void main(void) 
{
    setup();
    while(1)
    {
        RC7 = 0; 
        // ACTIVAMOS EL SS:
        __delay_ms(1); 
        //---------------------------------------------
        spiWrite(62); 
        // SLAVE AL ADRESH:
        if (devolver == 0)
        { // --> OBTENEMOS UNA LECTURA RANDOM, ENTONCES LA SALTAMOS Y DAMOS 1.
            lectura = spiRead(); 
            // DAMOS VALOR AL SLAVE:
            devolver = 1; 
            // LECTURA UTIL:
            __delay_ms(1); 
            // DESACTIVAMOS EL SS:
            RC7 = 0; 
        } else 
        {
            variable_ADRESL = spiRead(); //Se lee valor devuelto por Slave (ADRESL)
            __delay_ms(1); 
            // DESACTIVAMOS EL SS:
            RC7 = 0; 
        }
        __delay_ms(100);
        // ACTIVAMOS EL SLAVE SELECT:
        //---------------------------------------------
        RC7 = 0;
        // ENVIAMOS AL ADRESH:
        spiWrite(60);
        PORTD = 2;
        // LEER EL VALOR POR SLAVE --> ADRESH.
        variable_ADRESH = spiRead(); 
        __delay_ms(1); 
        //---------------------------------------------
        // ACTIVAMOS EL SLAVE SELECT:
        RC7 = 1; 
        //---------------------------------------------
        PORTA = variable_ADRESH; // --> PORTA
        PORTB = variable_ADRESL; // --> PORTB
        //---------------------------------------------
        __delay_ms(100);
    }
    return;
}
// ====================================================
// CONFIGURACIONES:
// ====================================================
void setup(void) {
    
    ANSEL = 0; // --> INPUTS/OUTPUTS DIGITALES
    ANSELH = 0;   
    // SALIDA:
    TRISA = 0; 
    PORTA = 0; 
    // OUTPUT:
    TRISB = 0; //PORTB como Output
    PORTB = 0;
    // OUTPUT PARA EL SLAVE SELECT:
    TRISCbits.TRISC7 = 0; 
    PORTCbits.RC7 = 1; 
    TRISD = 0;
    PORTD = 0;
    
    // CONFIGURACIONES EN .h
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE); 
    
    initOsc(1); 
    
    return;
}