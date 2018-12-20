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

const char* SECRET_PASSWORD = "123456789ABCDEF";

void display(uint8_t x)
{
    // Display 4-bit number on LEDs
    RA2 = (x >> 0) & 0x1;
    RA3 = (x >> 1) & 0x1;
    RA4 = (x >> 2) & 0x1;
    RB0 = (x >> 3) & 0x1;
}

void main(void)
{
    // Configure internal RC oscillator 8 MHz.
    OSCCON = 0b01110010;
    
    // Set ANSEL for analogue(1) or digital(0) I/O
    ANSEL = 0b0000000;
    
    // Set digital outputs for status LEDs
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA3 = 0;
    TRISAbits.TRISA4 = 0;
    TRISBbits.TRISB0 = 0;
    
    // Configure SPI as slave
    spi_init(0, 0);
    
    display(0); // Turn 4-bit number display off.
    uint8_t init = 0;
    uint8_t state = 0;
    while (1)
    {
        // Turn on the "everything is okay" LED.
        RA0 = 1;
        
        // Receive a byte from master. We have to send something, so send 0xFF.
        uint8_t rx = spi_transceive_byte(0xFF);

        // Sync up with wherever the master is in the password
        if (init == 0)
        {
            for (size_t k=0; k<strlen(SECRET_PASSWORD); k++)
                if (rx == SECRET_PASSWORD[k])
                {
                    state = k;
                    init = 1;
                }
        }
        else
        {
            uint8_t next_state = (state+1) % strlen(SECRET_PASSWORD);
            
            if (rx == SECRET_PASSWORD[next_state])
            {
                // Success! Received the expected character. Proceed to next.
                state = next_state;
                display(1+state); // +1 so at least 1 LED is lit.
            }
            else
            {
                // Error! Unexpected character received. Turn off all the lights
                // to signal error and wait a while so we see it.
                display(0);
                RA0 = 0;
                __delay_ms(100);
            }
        }
    }
}
