/* 
 * File:   UART1.h
 * Author: mbatchel
 *
 * Created on January 20, 2016, 8:18 PM
 */

#ifndef UART1_H
#define	UART1_H

#include <xc.h>
#include <plib.h>
#include "SYS.h"

// ------------------------------------------------------------
// UART1 Defines
// ------------------------------------------------------------
#define U_TX 0x0400
#define U_RX 0x1000
#define U_ENABLE 0x8008 // enable, BREGH = 1, 1 stop, no parity
#define BAUD_RATE 115200
// 115,200 Baud Rate = FPB / 4(U1BRG + 1))  FPB = freq of peripheral clock
#define BRATE ((PB_CLK_FREQ/(4*BAUD_RATE))-1)
// if BREGH = 0 then use 16 in place of 4 in above formulas

// ------------------------------------------------------------
// UART1 Function Prototypes
// ------------------------------------------------------------
void initU1(void);
void putU1(char c);
char getU1(void);
char *getsU1(char *s, int len);
void _mon_putc(char c);
int _mon_getc(int canblock);
void putsU1(char *s);

#endif	// UART1_H
