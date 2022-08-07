/* 
 * File:   TMR0.c
 * Author: ALBA RODAS
 *
 * Created on 22 de julio de 2022, 08:20 AM
 */

#ifndef TMR0_H
#define	TMR0_H

#include <xc.h>
#include <stdint.h>

#define _tmr0_value 207 // 1MHz

void tmr0_init(uint16_t prescaler);
void tmr0_reload(void);


#endif

