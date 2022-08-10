/* 
 * File:   SLAVE_MAIN.c
 * Author: ALBA RODAS
 *
 * Created on 22 de julio de 2022, 08:20 AM
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
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

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
//-------------------------------------------------------------------------------
// DECLARAMOS FREQ. DE OSCILADOR:
#define _XTAL_FREQ 1000000
/*===============================================================================
 * INCLUIMOS LAS LIBRERIAS ".h" A UTILIZAR:
==============================================================  */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "SPI.h"    // DADA POR EL CATEDRATICO.
#include "Oscilador.h"
#include "ADC.h"    
//-------------------------------------------------------------------------------
//===============================================================================
// VARIABLES:
//===============================================================================
//-------------------------------------------------------------------------------
// VARIABLE PARA CONTADORES:
uint8_t COUNTER;
//-------------------------------------------------------------------------------
// VARIABLES PARA MAPEO DE VOLTAJE:
char VOLTAJE_01;
char VOLTAJE_02;
/*===============================================================================
 * PROTOTIPO DE FUNCIONES:
================================================================================*/
void setup(void);
/*===============================================================================
 * INTERRUPCIONES:
================================================================================*/
void __interrupt() isr (void){
    if(PIR1bits.SSPIF)
    {         
        // VERIFICAMOS LA BANDERA DEL --> SSPIF  
        COUNTER = spiRead();
        // SI EL CONTADOR = 0, THEN EXCRIBIMOS EN VARIABLE DE VOLTAJE, POT#1.
        if(COUNTER == 0)
            spiWrite(VOLTAJE_01);
        // DE LO CONTRARIO, ESCRIBIMOS EN LA VARIABLE DEL VOLTAJE PARA EL POT#2.
        else if(COUNTER == 1)
            spiWrite(VOLTAJE_02);
        // LIMPIAMOS LA BANDERA ACTIVADA INICIALMENTE.
        PIR1bits.SSPIF = 0;    
    } 
    // VERIFICAMOS LA BANDERA DEL --> ADIF  
    if(PIR1bits.ADIF)
    {
        // SI EL CANAL CERO ESTÁ SIENDO USADO, GUARDAMOS LO QUE TENGAMOS EN LA
        // VARIABLE DEL POT#1 EN EL ADRESH.
        if(ADCON0bits.CHS == 0)
            VOLTAJE_01 = ADRESH;
        else
            // DE LO CONTRARIO, GUARDAMOS LO DEL  POT#2.
            VOLTAJE_02 =  ADRESH;
        // LLAMAMOS A LA FUNCIÓN DE LECTURA DEL ADC.
        ADCRead();
    }
    return;
}
/*===============================================================================
 * CONFIGURACION DE ENTRADAS Y SALIDAS:
================================================================================*/
void setup(void)
{
    // DEFINIMOS ENTRADAS PARA LOS POTENCIOMETROS (RA0, RA1).
    ANSEL = 0b00000101;
    ANSELH = 0;               // INPUTS/OUTPUTS DIGITALES.
    //****************************************************************
//--------------------------------------------------------------------------
    // ACTIVAMOS EL RA0 --> POT#1, RA1 --> POT#2, RA5 --> SLAVE SELECT.
    TRISA = 0b00100101;
    // SDI y SCK COMO INPUTS, SD0 COMO OUTPUT.
    TRISC = 0b00011000; 
    //****************************************************************
//--------------------------------------------------------------------------
    // LIMPIAMOS LOS PUERTOS "A" y "C" PARA EVITAR VALORES RANDOM.
    PORTA = 0;                
    PORTC = 0;                
    // DEFINIMOS EL VALOR DEL OSCILADOR --> 1MHz.
    initOsc(2);        
    // FRECUENCIA --> 1MHZ --> MEJOR FUNCIONAMIENTO DE LA LCD. 
    //SELECCIONAMOS UN FOSC/2 Y EL Vref EN CERO.
    initADC(0,0,0);           
    // REALIZAMOS CONFIGURACIONES EN SPI,h, SPI.C:
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    
    //****************************************************************
    //--------------------------------------------------------------------------
    // CONFIGURAMOS LAS INTERRUPCIONES:
     // GLOBALES --> ON
    INTCONbits.GIE = 1; 
     // PERIFERICOS --> ON
    INTCONbits.PEIE = 1;   
    // CLEAR AL SPI
    PIR1bits.SSPIF = 0;     
     // INT. SPI --> ON
    PIE1bits.SSPIE = 1;    
}
/*===============================================================================
 * CICLO PRINCIPAL:
================================================================================*/
int main(void) 
{ 
    // MENCIONAMOS A LAS ENTRADAS Y SALIDAS, MANDANDO A LLAMAR A LO DEFINIDO ANTERIORMENTE:
    setup();   
    // MANTENEMOS ENLOOPADO, EL FUNCIONAMIENTO DEL ADC:
    while(1){
        startADC(0, 1);
    }
    return(1);
}
