/* 
 * File:   Oscilador.c
 * Author: ALBA RODAS
 *
 * Created on 22 de julio de 2022, 08:20 AM
 */

#include <stdint.h>
#include "oscilador.h"

void initOsc(uint8_t freq){
    switch(freq){
        case 1:
            OSCCONbits.IRCF = 0b100; // FOSC = 1 MHZ
            break;
        case 2:
            OSCCONbits.IRCF = 0b101; // FOSC = 2 MHz
            break;
        case 4:
            OSCCONbits.IRCF = 0b110; // FOSC = 4 MHz
            break;
        case 8:
            OSCCONbits.IRCF = 0b111; // FOSC = 8 MHz
            break;
        default: //Default en caso el usuario indique valor no definido
            OSCCONbits.IRCF = 0b110; // FOSC = 4 MHz
            break;
    }
    OSCCONbits.SCS = 1;
}

    