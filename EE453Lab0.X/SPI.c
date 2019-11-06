// SPI.c

#include "SPI.h"

// ------------------------------------------------------------
// SPI2initialize
// ------------------------------------------------------------

/*
            |__|--|__|--|__|--|__
   data read here ^     ^     ^
            |xxxxx|yyyyy|zzzzz|
            ^     ^     ^
        data changes here
        With CKP=0, CKE=1  SPI Mode 0,0 is implemented which the MCP4822
             is designed for.
        SPI Mode	CKP,CKE
        0,0			0,1 <- MCP4822 operating mode  
        0,1			0,0
        1,0			1,1
        1,1			1,0
 */
void SPI2initialize(void) {
    TRIS_DAC_SELECT = 0; // make SPI select pin output
    DAC_SELECT = 1; // de-select the DAC
    SPI2CON = SPI_CONF; // select mode and enable SPI2
    SPI2BRG = SPI_BAUD; // select clock speed
}
/*
 bit 15 A/B: DACA or DACB Selection bit
1 = Write to DACB
0 = Write to DACA
bit 14 ? Don?t Care
bit 13 GA: Output Gain Selection bit
1 = 1x (VOUT = VREF * D/4096)
0 = 2x (VOUT = 2 * VREF * D/4096), where internal VREF = 2.048V.
bit 12 SHDN: Output Shutdown Control bit
1 = Active mode operation. VOUT is available.
0 = Shutdown the selected DAC channel. Analog output is not available at the channel that was shut down.
bit 11-0 D11:D0: DAC Input Data bits. Bit x is ignored. 
 */
// ------------------------------------------------------------
// writeSPI2		send data to MCP 4822 DAC bits 12-15 command, bits 0-11 data
// ------------------------------------------------------------

int writeDAC_SPI2(uint16_t val, uint8_t channel, uint8_t gain, uint8_t shutdown) {
    DAC_SELECT = 0; // select the DAC
    val = (val & 0x0fff) | // Add command bits
            ((channel & 0x01) << CH) |
            ((gain & 0x01) << GAIN) |
            ((shutdown & 0x01) << SHUTDOWN);

    SPI2BUF = val; // write to buffer for TX
    while (SPI2STATbits.SPIBUSY); // wait for transfer complete
    DAC_SELECT = 1; // deselect to complete command
    return SPI2BUF; // data sheet says always read
    // receiver buffer since full duplex
} // writeDAC_SPI2