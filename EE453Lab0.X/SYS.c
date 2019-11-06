// SYS.c
#include "SYS.h"
#include "ADC.h"
// Initialize the system and required peripherals.

//---------------------------------------------------------------------
// PRAGMA
//---------------------------------------------------------------------
// 80 MHz main clock and 40 MHz Peripheral clock
#pragma config POSCMOD=HS, FNOSC=PRIPLL
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_20, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_1, FWDTEN=OFF, CP=OFF, BWP=OFF


void SysInitialize(void) {
    /* Enable optimal performance                       */
    SYSTEMConfigPerformance(BSP_CLK_FREQ);
    mOSCSetPBDIV(0); /* Use 1:1 CPU Core:Peripheral clocks               */

#if JTAG_ENABLE
    DDPCONbits.JTAGEN = 1; /* Maintain the port pins for JTAG use              */
#else
    DDPCONbits.JTAGEN = 0; /* Free the JTAG port pins for use as GPIO          */
#endif


} // SysInitialize