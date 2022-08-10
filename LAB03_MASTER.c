/* 
 * File:   MASTER_MAIN.c
 * Author: ALBA RODAS
 *
 * Created on 22 de julio de 2022, 08:22 AM
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
=================================================================================*/
#include <xc.h>
#include "SPI.h"        // DADA POR EL CATEDRATICO.
#include "LCD.h"        // DADA POR EL CATEDRATICO.
#include "TMR0.h"
#include "Oscilador.h"
// LIBRERIAS YA DADAS POR CONFIG.
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
//-------------------------------------------------------------------------------
//===============================================================================
// VARIABLES:
//===============================================================================
//-------------------------------------------------------------------------------
// VARIABLE PARA CONTADORES:
uint8_t COUNTER = 0;
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// GUARDAMOS VALORES:
uint16_t VALUE;
//------------------------------------------------------------------------------
// VARIABLE PARA MAPEO DE VOLTAJE --> TOMAMOS EN CUENTA QUE NO SON VALORES ENTEROS:
char VOLTAJE_01;
char VOLTAJE_02;
// VARIABLE PARA MAPEO DE VOLTAJE:
float VOLTAJE_01_MAPPING = 0;
float VOLTAJE_02_MAPPING = 0;
//-------------------------------------------------------------------------------
// VARIABLES PARA MAPEO DE VOLTAJE:
char VOLTAJE_L01[5];
char VOLTAJE_L02[5];
//------------------------------------------------------------------------------
/*===============================================================================
 * PROTOTIPO DE FUNCIONES:
================================================================================*/
void setup(void);
//===============================================================================
/*===============================================================================
 * CONFIGURACION DE ENTRADAS Y SALIDAS:
================================================================================*/
void setup(void)
{
    // DEFINIMOS ENTRADAS PARA LOS POTENCIOMETROS (RA0, RA1).
    ANSEL = 0;
    ANSELH = 0;               
    //--------------------------------------------------------------------------
    // COMUNICAMOS AL MASTER CON EL SLAVE POR RA7:
    TRISAbits.TRISA7 = 0;  
    //--------------------------------------------------------------------------
    // DEFINIMOS LOS PUERTOS COMO ENTRADAS/SALIDAS:
    TRISB = 0;                
    TRISD = 0;                
    TRISE = 0;
    //--------------------------------------------------------------------------
    // DECLARAMOS COMO ENTRADAS/SALIDAS A PINES DE COMUNICACIÓN SPI:
    // SDA --> INPUT:
    TRISCbits.TRISC4 = 1;    
    // SCK --> OUTPUT:
    TRISCbits.TRISC3 = 0;    
    // SD0 --> SALIDA:
    TRISCbits.TRISC5 = 0;     
    //--------------------------------------------------------------------------
    // LIMPIAMOS LOS PUERTOS PARA EVITAR VALORES RANDOM.
    PORTA = 0;                
    PORTB = 0;                
    PORTC = 0;                
    PORTD = 0;                
    PORTE = 0;               
    //--------------------------------------------------------------------------
    // DEFINIMOS EL VALOR DEL OSCILADOR --> 1MHz.
    initOsc(2);               
    // FRECUENCIA --> 1MHZ --> MEJOR FUNCIONAMIENTO DE LA LCD
    //**************************************************************
    //SSPM = 0000 (SPI Master mode, clock = FOSC/4), SSPEN = 1 (SCK, SDO, SDI y SS); SPM = 1; CKP = 0 - Reloj inactivo en 0; CKE = 1
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE); 
}
/*===============================================================================
 * CICLO PRINCIPAL:
================================================================================*/
int main(void) 
{ 
    // "MENCIONAMOS" LA FUNCION DE CONFIGURACIÓN DE ENTRADAS Y SALIDAS:
    setup();    
    // INCLUIMOS LIBRERIA/FUNCION DE INICIALIZACIÓN DE LCD:
    Lcd_Init();
    // LIMPIAMOS Y LUEGO INICIALIZAMOS:
    Lcd_Clear();
    
    while(1)
    {
        // DEFINIMOS QUE:
        // MIENTRAS HAYA COMUNICACIÓN ENTRE MAESTRO-ESCLAVO:
        TRISAbits.TRISA7 = 0;     //RA7 salida Comunicación con Esclavo
        // CHECK EL BUFFER:
        spiWrite(0);
        // LEEMOS EL ESTADO DEL VOLTAJE EN POT#1:
        VOLTAJE_01 = spiRead();
        // DAMOS UN DELAY --> ES POSIBLE QUE SE VEA REFLEJADO EN EL ARMADO FISICO.
        __delay_ms(1);
        
        // SPI FUNCION --> 1 --> CHECK EL .h, .c
        spiWrite(1);
        // LEEMOS EL ESTADO DEL VOLTAJE EN POT#2:
        VOLTAJE_02 = spiRead();
        // DAMOS UN DELAY --> ES POSIBLE QUE SE VEA REFLEJADO EN EL ARMADO FISICO.
         __delay_ms(1);
        // UPDATE COMUNICACION CON EL SLAVE:
        TRISAbits.TRISA7 = 1;     
        // DAMOS UN DELAY --> ES POSIBLE QUE SE VEA REFLEJADO EN EL ARMADO FISICO.
         __delay_ms(1);
     
        //--------------------------------------------------------------------------
        // DEFINIMOS "ESCALA";
        VOLTAJE_01_MAPPING = (VOLTAJE_01*0.01961);
        VOLTAJE_02_MAPPING = (VOLTAJE_02*0.01961);
        //--------------------------------------------------------------------------
        // CONFIGURACION EXTRAÍDA DE LAB02 --> CURSORES, TITLES, ETC.
        // UTILIZAMOS COMANDOS DE LAS LIBRERIAS DADAS DEL LDC:
        // DEFINIMOS POSICIÓN DE CURSOR:
        Lcd_Set_Cursor(1,1);
        // DEFINIMOS ENCABEZADO: P1 --> POTENCIOMETRO #1.
        Lcd_Write_String("V1:");
        // FILA NO.2, COLUMINA 1.
        Lcd_Set_Cursor(2,1);               
        // MOSTRAMOS VALOR EN STRING, DECIMALES IMPORTANTE:
        sprintf(VOLTAJE_L01,"%.2fV", VOLTAJE_01_MAPPING);    
        // ESCRIBIMOS LOS STRING RECIBIDOS DEL PROCESO:
        Lcd_Write_String(VOLTAJE_L01);              //String obtenidos
        // DAMOS UN DELAY PARA QUE CARGUE TODO --> ME FALLÓ SIN ESTO:
        __delay_ms(1000);
        
        //--------------------------------------------------------------------------
        // LIMPIAMOS DATA:
        Lcd_Clear();
        //--------------------------------------------------------------------------
        
        // DEFINIMOS POSICIÓN DE CURSOR:
        Lcd_Set_Cursor(1,1);
        // DEFINIMOS ENCABEZADO: P2 --> POTENCIOMETRO #2.
        Lcd_Write_String("V2:");
        // FILA NO.2, COLUMINA 1.
        Lcd_Set_Cursor(2,1);                
        // MOSTRAMOS VALOR EN STRING:
        sprintf(VOLTAJE_L01," %.2fV ", VOLTAJE_02_MAPPING);
        // MOSTRAMOS VALOR ACTUALIZADO DE VOLTAJE:
        Lcd_Write_String(VOLTAJE_L01);
        // DAMOS UN DELAY PARA QUE CARGUE TODO --> ME FALLÓ SIN ESTO:
        __delay_ms(1000);
        
        //--------------------------------------------------------------------------
        // LIMPIAMOS DATA:
        Lcd_Clear();
        //-------------------------------------------------------------------------- 
    }
    return(1);
}
