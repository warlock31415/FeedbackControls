// File: UART1.c

#include "UART1.h"

//----- Initialize UART1
void initU1(void) {
    U1BRG = BRATE; // initialize the baud rate generator work as OpenUART1 does
    U1MODE = U_ENABLE;
    U1STA = U_TX | U_RX;
} // initU1

void putU1(char c) {
    while (U1STAbits.UTXBF); // wait while TX buffer is full
    U1TXREG = c;
} // putU1

//----- Get character from UART1

char getU1(void) {
    while (!U1STAbits.URXDA); // wait for a new character to arrive
    return U1RXREG; // read the character from the receive buffer
} // getU1


// ----- Get String from U1.
//       From Programming 32-Bit Microcontrollers in C: Exploring the PIC32,
//	 By Lucio Di Jasio, Newnes Publishing.

char *getsU1(char *s, int len) {
#define BACKSPACE 0x08
    char *p = s; // copy the buffer pointer
    do {
        *s = getU1(); // wait for a new character
        putU1(*s); // echo character

        if ((*s == BACKSPACE) && (s > p)) {
            putU1(' '); // overwrite the last character
            putU1(BACKSPACE);
            len++;
            s--; // back the pointer
            continue;
        }
        if (*s == '\n') // line feed, ignore it
            continue;
        if (*s == '\r') // end of line, end loop
            break;
        s++; // increment buffer pointer
        len--;
    } while (len > 1); // until buffer full

    *s = '\0'; // null terminate the string

    return p; // return buffer pointer
} // getsU1


//Compiler helper function that allows the use of printf
void _mon_putc(char c) {
    putU1(c);
} // _mon_putc

// ----- Helper function for scanf   Not used in this program since not using scanf
int _mon_getc(int canblock) {
    return getU1();
} // _mon_getc


//----- Output zero terminated string to UART1
void putsU1(char *s) {
    while (*s) // loop until *s = \0, end of string
        putU1(*s++);
} // putsU1
