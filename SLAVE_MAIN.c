/* 
 * File:   SLAVE_MAIN.c
 * Author: ALBA RODAS
 *
 * Created on 22 de julio de 2022, 08:20 AM
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

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// ====================================================
// LIBRERIAS:
// ====================================================
// INCLUIMOS LIBRERIAS:
#include <xc.h>
#include <stdint.h>
#include <pic16f887.h>
#include "SPI.h"
#include "oscilador.h"
#include "ADC.h"

// ====================================================
// CONSTANTES:
// ====================================================
#define _XTAL_FREQ 1000000 

// ====================================================
// VARIABLES:
// ====================================================
uint8_t value = 0;
uint8_t value_ADRESH = 8;
uint8_t value_ADRESL = 0;
// ====================================================
// PROTOTIPOS DE FUNCIONES:
// ====================================================
void __interrupt() SPI_slave(void){
    if(PIR1bits.ADIF){                      
        if(ADCON0bits.CHS == 0)
        { 
            value_ADRESH = (adc_read()>>2) & 255; 
            value_ADRESL = adc_read()& 3; 
        }
        PIR1bits.ADIF = 0; 
    }
    if (PIR1bits.SSPIF){
        value = spiRead();
        if (value == 62){
            spiWrite(value_ADRESH); 
            // > ENVIAR AL ADRESH
        } else if (value == 60) {
            spiWrite(value_ADRESL); 
            // < AL ADRESL
        }
        PIR1bits.SSPIF = 0;
    }
    return;
}
// ====================================================
// CICLO PRINCIPAL:
// ====================================================
void main(void) 
{
    
    ANSEL = 0x01; 
    ANSELH = 0; 
    
    TRISAbits.TRISA0 = 1; 
    // INPUT PARA EL SLAVE SELECT:
    TRISAbits.TRISA5 = 1; 
    // PARA MODIFICACIONES: ADC.h, ADC.c
    adc_init(0, 0, 0); 
    // CONFIGURACIONES PARA EL SPI:
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    
    INTCONbits.GIE = 1;  // GLOBALES
    PIR1bits.SSPIF = 0;  // SPI --> BANDERA.
    PIE1bits.SSPIE = 1;  
    INTCONbits.PEIE = 1; 
    
    while(1)
    {
        // CONFIGURAMOS EL ADC, CANAL 0:
        adc_start(0);
    }
    return;
}