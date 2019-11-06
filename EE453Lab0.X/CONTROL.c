/*-----------------------------------------------------------------------
 * File:        CONTROL.c
 * Developers:	M. Batchelder
                S. Thornburg
   
   Processor:	PIC32MX79F512L
   Board:       Max32
   Compiler:	XC32
   IDE:         MPLAB-X
   Date:        October 18, 2019
   Status:      Adapted for EE 453L/553L Lab 0
            
   Description:
  		Generic Shell for Closed-Loop Digital Control
  		Timer 2 generates interrupts at 15000 Hz, or T = 66.7 us
  		The ISR outputs a value from a waveform lookup table
  			to the MCP4822 DAC on the SPI2 port
  		Illustrates how to
  		1.	set oscillator
  		2.	use SPI port
  		3.	use timer
  		4.	use interrupts
        
-----------------------------------------------------------------------*/

//-----------------------------------------------------------------------
// INCLUDES
//-----------------------------------------------------------------------
#include <xc.h>
#include <stdio.h>
#include <plib.h>
#include <stdint.h>
#include <math.h>
#include "INCLUDES.h"


#define Ts  (0.0000667)  // Control sample period in seconds
#define Fs  (1/Ts)       // Sample frequency in Hertz


// DDS table for generating a sine waveshape using DDS
// One cycle of sine tone, (256) 8-bit samples, in hex values
// Any desired waveform could be placed here
uint8_t sineTable[] = {
    0x80, 0x83, 0x86, 0x89, 0x8c, 0x8f, 0x92, 0x95, 0x98, 0x9b, 0x9e, 0xa2, 0xa5, 0xa7, 0xaa, 0xad,
    0xb0, 0xb3, 0xb6, 0xb9, 0xbc, 0xbe, 0xc1, 0xc4, 0xc6, 0xc9, 0xcb, 0xce, 0xd0, 0xd3, 0xd5, 0xd7,
    0xda, 0xdc, 0xde, 0xe0, 0xe2, 0xe4, 0xe6, 0xe8, 0xea, 0xeb, 0xed, 0xee, 0xf0, 0xf1, 0xf3, 0xf4,
    0xf5, 0xf6, 0xf8, 0xf9, 0xfa, 0xfa, 0xfb, 0xfc, 0xfd, 0xfd, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfd, 0xfd, 0xfc, 0xfb, 0xfa, 0xfa, 0xf9, 0xf8, 0xf6,
    0xf5, 0xf4, 0xf3, 0xf1, 0xf0, 0xee, 0xed, 0xeb, 0xea, 0xe8, 0xe6, 0xe4, 0xe2, 0xe0, 0xde, 0xdc,
    0xda, 0xd7, 0xd5, 0xd3, 0xd0, 0xce, 0xcb, 0xc9, 0xc6, 0xc4, 0xc1, 0xbe, 0xbc, 0xb9, 0xb6, 0xb3,
    0xb0, 0xad, 0xaa, 0xa7, 0xa5, 0xa2, 0x9e, 0x9b, 0x98, 0x95, 0x92, 0x8f, 0x8c, 0x89, 0x86, 0x83,
    0x80, 0x7c, 0x79, 0x76, 0x73, 0x70, 0x6d, 0x6a, 0x67, 0x64, 0x61, 0x5d, 0x5a, 0x58, 0x55, 0x52,
    0x4f, 0x4c, 0x49, 0x46, 0x43, 0x41, 0x3e, 0x3b, 0x39, 0x36, 0x34, 0x31, 0x2f, 0x2c, 0x2a, 0x28,
    0x25, 0x23, 0x21, 0x1f, 0x1d, 0x1b, 0x19, 0x17, 0x15, 0x14, 0x12, 0x11, 0x0f, 0x0e, 0x0c, 0x0b,
    0x0a, 0x09, 0x07, 0x06, 0x05, 0x05, 0x04, 0x03, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x02, 0x02, 0x03, 0x04, 0x05, 0x05, 0x06, 0x07, 0x09,
    0x0a, 0x0b, 0x0c, 0x0e, 0x0f, 0x11, 0x12, 0x14, 0x15, 0x17, 0x19, 0x1b, 0x1d, 0x1f, 0x21, 0x23,
    0x25, 0x28, 0x2a, 0x2c, 0x2f, 0x31, 0x34, 0x36, 0x39, 0x3b, 0x3e, 0x41, 0x43, 0x46, 0x49, 0x4c,
    0x4f, 0x52, 0x55, 0x58, 0x5a, 0x5d, 0x61, 0x64, 0x67, 0x6a, 0x6d, 0x70, 0x73, 0x76, 0x79, 0x7c
};


// ------------------------------------------------------------
// Timer2initialize
// ------------------------------------------------------------
// Set up Timer2 to generate interrupt signals at a rate of Fs = 1/Ts

void Timer2initialize(void) {
    T2CON = 0x8070; // 80 MHz = 12.5 ns per clock.  Prescale = divide by 256
    // T2 count period = (12.5 ns per clock)/256 = 3.2 us per count
    // Counts needed per desired T2 period = Ts/T2 - 1
    PR2 = Ts/0.0000032-1;
}

// ------------------------------------------------------------
// PortsInitialize
// ------------------------------------------------------------

void PortsInitialize(void) {
    //TRIS_LED5 = 0;  // output
}

// ------------------------------------------------------------
// InterruptInitialize
// ------------------------------------------------------------
// Initializes Timer2 interrupt

void InterruptInitialize(void) {
    mT2SetIntPriority(1);
    INTEnableSystemMultiVectoredInt();
    mT2IntEnable(1);
}

// ------------------------------------------------------------
// SystemInitialize
// ------------------------------------------------------------

void SystemInitialize(void) {
    PortsInitialize();
    SPI2initialize();
    Timer2initialize();
    InterruptInitialize();
    initU1(); // UART
    initADC(AINPUTS);
}



// ------------------------------------------------------------
// Global Variables
// ------------------------------------------------------------
// variables for DDS--used in this lab to create reference signal r(k)
uint16_t phaseAccumulator = 0; // Holds DDS phase
uint16_t phaseStep = 0;        // DDS tuning word
uint8_t waveTableIndex = 0;    // index for wave lookup (upper 8 bits of phaseAccumulator)
uint16_t sampleOutput = 0;     // oscillator sample output
uint8_t LEDout = 0;
int count = 0;

// variables for control law
int32_t rk = 0;   // current value of reference signal, r(k)
int32_t rHigh;    // high integer value of reference signal
int32_t yk = 0;   // current value of output read from ADC, y(k)
int32_t ek = 0;   // current value of error e(k)
int32_t ek1 = 0;  // past value of error e(k-1)
int32_t ek2 = 0;  // past value of error e(k-2)
// control signal is limited by unsigned 12-bit DAC
#define umin (0)    // min value of control signal
#define umax (4095) // max value of control signal
int32_t uk = 0;     // current value of control, u(k)
int32_t uk1 = 0;    // past value of control, u(k-1)
int32_t uk2 = 0;    // past value of control, u(k-2)
int32_t ukAWU = 0;  // anti-windup value of control
// Compensator coefficients (a0=1):
#define a1 (-1)
#define a2 (0)
#define b0 (1.187)
#define b1 (-1.435)
#define b2 (0.434)
// Compensator coefficients to use in case of integrator windup:
#define a1AWU (0)
#define a2AWU (0)
#define b0AWU (1.163)
#define b1AWU (-0.425)
#define b2AWU (0)

// ------------------------------------------------------------
// T2InterruptHandler
// ------------------------------------------------------------
// This program uses the interrupts generated by Timer2 to execute
// the control algorithm

void __ISR(_TIMER_2_VECTOR, ipl1AUTO) T2InterruptHandler(void) {
    // T2 handler code here

    // Read current value from 10-bit ADC and scale to be consistent with DAC.
    // We want a 1 Volt signal read from the ADC to create a 1 Volt output from
    // the DAC if we pass the value straight through.
    // The ADC uses a 3.3 V reference into a 10-bit binary representation,
    // so the scale factor is (3.3 V)/(2^10-1) = 0.003226 V/bit.
    // The DAC can produce a 4.096 V max output from a 12-bit binary number,
    // so its scale factor is (4.096 V)/(2^12-1) = 0.001000 V/bit.
    // To put the values from the ADC onto the same scale factor as the DAC,
    // multiply the readings by the ratio of the scale factors:
    // yk = valueADC * SFDAC / SFADC = valueADC * 0.00323 / 0.00100
    yk = readADC(1)*3.225;
    
    #define rAmp (2.0) // magnitude of reference wave in Volts
    #define vmax (4.096) // max MCP4822 output in Volts
    // Calculate sinusoidal reference signal via DDS
    // get 8-bit sample from wave table and left shift 4 bits to match
    // 12-bit fixed-point format for DAC
    // rk = (sineTable[waveTableIndex] << 4)*rAmp/vmax;
    
    // Alternative square wave reference signal
    // First half of wave has zero amplitude,
    // second half has amplitude rAmp.
    if(phaseAccumulator < 0x8000)
       rk = 0;
    else
        rk = rHigh;
    
    ek = rk - yk; // Calculate new current value of error e(k)
    
    // Control law
    // D(z) = U(z)/E(z) = (b0 + b1*z^-1 + b2*z^-2)/(1 + a1*z^-1 + a2*z^-2)
    // Only the b0 term is added here. The rest of the formula is calculated
    // for the next sample time below.
    uk += b0*ek;
    
    // Check for control saturation and fix it
    if(uk > umax || uk < umin)
        uk = ukAWU + b0AWU*ek; // use anti-windup compensator
    if(uk > umax)
        uk = umax;
    else if(uk < umin)
        uk = umin;

    // transmit value out the SPI port
    // writeDAC_SPI2(uint16_t val, uint8_t channel, uint8_t gain, uint8_t shutdown)
    writeDAC_SPI2(uk, 0, 0, 1);


    // Remainder of calculations were saved until after the DAC output to
    // minimize the time delay from reading y(k) to writing u(k)
    
    // Set up past values e and u to be used on next interrupt
    ek2 = ek1; // Shift last value of e(k-1) into e(k-2)
    ek1 = ek; // Shift last value of e(k) into e(k-1)
    uk2 = uk1; // Shift last value of u(k-1) into u(k-2)
    uk1 = uk; // Shift last value of u(k) into u(k-1)
    
    // Precalculate as much of the control signal for the next sample time as possible
    uk = -a1*uk1 - a2*uk2 + b1*ek1 + b2*ek2; // Precalculate uk
    ukAWU = -a1AWU*uk1 - a2AWU*uk2 + b1AWU*ek1 + b2AWU*ek2; // anti-windup precalc
    
    // Advance phase and lookup sample in wave table for next sample
    phaseAccumulator += phaseStep; // increment the phase accumulator
    waveTableIndex = phaseAccumulator >> 8; // use top 8 bits as wavetable index

    // clear the flag and exit
    mT2ClearIntFlag();

} // T2 Interrupt Handler



// ------------------------------------------------------------
// main
// ------------------------------------------------------------

main() {
    SystemInitialize();

    // Set up phaseStep for DDS output frequency
    // DDS waveshape table holds 256 values, and phaseAccumulator is 16 bits
    // so output frequency will be Fout = M*Fs/2^16, or M = Fout*2^16/Fs
    #define Fout (100) // Reference waveform frequency in Hz
    phaseStep = Fout*0x10000*Ts;
    rHigh = (rAmp*4095)/vmax; // Calc the integer value for the reference signal

    printf("Hello world.\n"); // Print a message to UART1

    while (1) // This loop runs except when the timer interrupt occurs
    {
        // Empty infinite loop allows interrupt handler to do all processing
    }

} // main
