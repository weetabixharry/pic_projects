// CONFIG1
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTRC oscillator; port I/O function on both RA6/OSC2/CLKO pin and RA7/OSC1/CLKI pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON      // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable bit (RB3/PGM pin has PGM function, Low-Voltage Programming enabled)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB0      // CCP1 Pin Selection bit (CCP1 function on RB0)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// CONFIG2
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode enabled)

#include <xc.h>
#include <string.h>
#define _XTAL_FREQ 8000000

#include "pic16lf88/spi.h"

const char* MASTER_MAGIC = "Hello from the master!";
const char* SLAVE_MAGIC = "Greetings, master. agkasglaskgnaklsgnalksgn";

void main(void)
{
    // Configure internal RC oscillator 8 MHz.
    OSCCON  = 0b01110010;
    
    // Set ANSEL for analogue(1) or digital(0) I/O
    ANSEL = 0b0000000;
    
    // Set digital outputs for LEDs
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA3 = 0;
    TRISAbits.TRISA4 = 0;
    TRISBbits.TRISB0 = 0;
    
    // Configure SPI as slave with slave select enabled
    spi_init(0, 0);
    
    RA0 = 0;
    while (1)
    {
        RA2 = 0;
        RA3 = 0;
        RA4 = 0;
        RB0 = 0;

        // SPI handshake
        uint8_t state = 0;
        while (state < strlen(MASTER_MAGIC))
        {
            // Receive byte from master. Send zeros.
            uint8_t rx = spi_transceive_byte(0);
            if (rx == MASTER_MAGIC[state])
                state++;
            else
                state = 0; // Unexpected rx. Reset state.

            // Output state onto LEDs
            RB0 = (state >= strlen(MASTER_MAGIC));
            RA4 = (state >= 2);
            RA3 = (state >= 1);
        }

        // Acknowledge that we saw the master's magic number
        for (uint8_t i=0; i<strlen(SLAVE_MAGIC); i++)
            spi_transceive_byte(SLAVE_MAGIC[i]);

        RA2 = 1;
        // Take commands from master
        uint8_t rx = spi_transceive_byte(0);

        if (rx == 0 || rx == 1)
            RA0 = rx;
    }
}
