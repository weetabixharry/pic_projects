// CONFIG1
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTRC oscillator; port I/O function on both RA6/OSC2/CLKO pin and RA7/OSC1/CLKI pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is digital I/O, MCLR internally tied to VDD)
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
#define _XTAL_FREQ 8000000

#include "pic16lf88/spi.h"

void main(void)
{
    // Configure internal RC oscillator 8 MHz.
    OSCCON  = 0b01110010;
    
    // Set ANSEL for analogue(1) or digital(0) I/O
    ANSEL = 0b0000000;
    
    // Set RA2 as input for switch
    TRISAbits.TRISA2 = 1;
    
    // Set RA0 as output for LED
    TRISAbits.TRISA0 = 0;
    
    // Configure SPI
    spi_init(1, 0);                  
    
    while (1)
    {
        if (RA2)
        {
            RA0 = 1;
        }
        else
        {
            RA0 = 0;
        }
        
        // Send switch reading to slave
        SPI_Exchange8bit(RA2);
    }
    
}
