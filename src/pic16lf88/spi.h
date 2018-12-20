#ifndef SPI_H
#define	SPI_H

#include <xc.h>
#include <stdint.h>

void spi_init(uint8_t is_master, uint8_t use_slave_select)
{
    // -------------------
    // -- Configure I/O --
    // -------------------
    TRISB1 = 1; // SDI (pin 7) is input
    TRISB2 = 0; // SDO (pin 8) is output
    if (is_master)
    {
        TRISB4 = 0; // Master: SCK (pin 10) is output
        if (use_slave_select)
            TRISB5 = 0; // Master: Use RB5 (pin 11) as output for one !SS
    }
    else
    {
        TRISB4 = 1; // Slave: SCK (pin 10) is input
        if (use_slave_select)
            TRISB5 = 1; // Slave: !SS (pin 11) is input
    }
    
    // ----------------------------
    // -- Configure SSPSTAT<7:6> --
    // ----------------------------
    if (is_master)
    {
        // SSPSTAT<7> = SMP. Use default in master mode?
    }
    else
    {
        SSPSTATbits.SMP = 0; // SSPSTAT<7> = SMP. Must be cleared in slave mode.
    }
    // SSPSTAT<6> = CKE. Use default?
    
    // ---------------------------
    // -- Configure SSPCON<5:0> --
    // ---------------------------
    
    // SSPCON<4> = CKP. Use default?
    
    // SSPCON<3:0> = SSPM<3:0>
    if (is_master)
    {
        //SSPCONbits.SSPM = 0b0000; // clock = OSC/4
        //SSPCONbits.SSPM = 0b0001; // clock = OSC/16
        SSPCONbits.SSPM = 0b0010; // clock = OSC/64
        //SSPCONbits.SSPM = 0b0011; // clock = TMR2 output/2
    }
    else
    {
        if (use_slave_select)
            SSPCONbits.SSPM = 0b0100;
        else
            SSPCONbits.SSPM = 0b0101;
    }
    
    SSPCONbits.SSPEN = 1; // SSPCON<5> = SSPEN. Enable Synchronous Serial Port
}

// This function was copied from MCC-generated code
uint8_t spi_transceive_byte(uint8_t data)
{
    // Clear the Write Collision flag, to allow writing
    SSPCONbits.WCOL = 0;
    
    // Load TX data into shift register
    SSPBUF = data;

    while(SSPSTATbits.BF == 0); // Wait until RX data shifts in
    
    // Always read the RX shift register to prevent overflow
    return SSPBUF;
}

#endif	/* SPI_H */
