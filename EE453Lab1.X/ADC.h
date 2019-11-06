/* 
 * File:   ADC.h
 * Author: mbatchel
 *
 * Created on January 20, 2016, 8:10 PM
 */

#ifndef ADC_H
#define	ADC_H

#include <xc.h>
#include <plib.h>

// ------------------------------------------------------------
// ADC Defines
// ------------------------------------------------------------
#define POT     0               // 10k potentiometer on AN0 input
#define POT_CH (POT)
#define AINPUTS 0xfffc // A0 (B0), A1 (B1) are analog rest (B2-15) are digital
#define SAMC 2	// Auto-sample time bits 1-31 Tad
#define ADCS 2	// ADC Conversion Clock select bits Tad = 2*(ADCS+1)*Tpb
//	Ex:  ADCS = 2, SAMC = 2,Tpb = 25 ns (Fpb = 40 MHz)
//	Tad = Tpb*2*(2+1) = 25*6 = 150 ns
//	Auto-sample time = 2*Tad = 300 ns
//	12 Tad required for conversion = 150 * 12 = 1.8 us
//	total ADC time = 12*150 + 300 = 2.1 us
#define AD1CON3_BITS ((SAMC<<8)|ADCS)


// ------------------------------------------------------------
// ADC Function Prototypes
// ------------------------------------------------------------
void initADC(int amask);
int readADC(int ch);

#endif  // ADC_H
