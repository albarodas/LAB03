/* 
 * File:   TMR0.c
 * Author: ALBA RODAS
 *
 * Created on 22 de julio de 2022, 08:20 AM
 */
#include <xc.h>
#include <stdint.h>
#include "TMR0.h"

void tmr0_init(uint16_t prescaler){
    OPTION_REGbits.T0CS = 0;    // TMR0 configurado como temporizador 
    if(prescaler != 0)
        OPTION_REGbits.PSA = 0; // Se asigna PS siempre que se ingrese un valor diferente de 0
    else if (prescaler == 0)    // Si ingresa a cero pasa a WDT
        OPTION_REGbits.PSA = 1;
    switch(prescaler){
        case 0:
            OPTION_REGbits.PS = 0b000; //PS WDT
            break;
        case 2:
            OPTION_REGbits.PS = 0b000; //PS 1:2
            break;
        case 4:
            OPTION_REGbits.PS = 0b001; //PS 1:4
            break;
        case 8:
            OPTION_REGbits.PS = 0b010; //PS 1:8
            break;
        case 16:
            OPTION_REGbits.PS = 0b011; //PS 1:16
            break;
        case 32:
            OPTION_REGbits.PS = 0b100;//PS 1:32
            break;
        case 64:
            OPTION_REGbits.PS = 0b101; //PS 1:64
            break;
        case 128:
            OPTION_REGbits.PS = 0b110; //PS 1:128
            break;
        case 256:
            OPTION_REGbits.PS = 0b111; //PS 1:256
            break;
        default:
            OPTION_REGbits.PS = 0b000; //PS WDT
            break;
    }
    INTCONbits.T0IE = 1; // Se habilitan Int. TMR0
    INTCONbits.T0IF = 0; // CLEAR TMR0 Flag
    return;
}

void tmr0_reload(void){
    TMR0 = _tmr0_value; // Se reasigna valor TMR0
    INTCONbits.T0IF = 0; // CLEAR TMR0 Flag
}