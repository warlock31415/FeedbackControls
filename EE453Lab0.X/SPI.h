// SPI.h
#ifndef SPI_H
#define SPI_H

#include <xc.h>
#include <stdio.h>
#include <plib.h>
#include <stdint.h>

// ------------------------------------------------------------
// SPI Function Prototypes
// ------------------------------------------------------------
void SPI2initialize(void);
int writeDAC_SPI2(uint16_t val, uint8_t channel, uint8_t gain, uint8_t shutdown);

// ------------------------------------------------------------
// SPI Interface
// ------------------------------------------------------------
// DAC interfaces to microcontroller with the Serial Peripheral Interface bus
// SS   Slave Select (chip select) DAC pin 2 PIC32 RG9 / MAX32 pin 53 
// SCLK  Serial Clock              DAC pin 3 PIC32 RG6 / MAX32 pin 52
// MOSI  Master Out Slave In       DAC pin 4 PIC32 RG8 / MAX32 pin 51
// MISO  Master In Slave Out       not used  PIC32 RG7 / MAX32 pin 50

// ------------------------------------------------------------
// DAC defines
// ------------------------------------------------------------
// I/O definitions
#define DAC_SELECT    	_RG9    // select line for DAC
#define TRIS_DAC_SELECT   	_TRISG9 // tris control for DAC select pin
#define SPI_CONF    0x8520           // CKE=0, CKP=0 0x8520             // SPI on, 16-bit master,CKE=1,CKP=0
#define SPI_BAUD    15               // clock divider Fpb/(2 * (SPI_BAUD+1)) = 2.5 MHz, for Fpb = 80 MHz


// ------------------------------------------------------------
// DAC control bits
// ------------------------------------------------------------
#define CH	  15        //  bit 15:  0 select channel A
                        //           1 select channel B
                        //  bit 14:  not used, don?t care
#define GAIN     13     //  bit 13:  0 set gain 2X, output between 0 volts and 4.096 volts
                        //           1 set gain 1X, output between 0 volts and 2.048 volts
#define SHUTDOWN 12     //  bit 12:  0 shutdown the selected channel
                        //           1 selected channel set active
#define VAL_MASK 0x0fff //  bits 11-0:  12 bit value to be converted to analog

#endif // SPI_H