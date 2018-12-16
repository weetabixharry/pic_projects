#ifndef SPI_H
#define	SPI_H

#include <stdint.h>
#include <stddef.h>

void set_sspm(uint8_t x)
{
    SSPCONbits.SSPM3 = (x >> 3) & 0x1;
    SSPCONbits.SSPM2 = (x >> 2) & 0x1;
    SSPCONbits.SSPM1 = (x >> 1) & 0x1;
    SSPCONbits.SSPM0 = (x >> 0) & 0x1;
}

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
        //set_sspm(0b0000); // clock = OSC/4
        //set_sspm(0b0001); // clock = OSC/16
        set_sspm(0b0010); // clock = OSC/64
        //set_sspm(0b0011); // clock = TMR2 output/2
    }
    else
    {
        if (use_slave_select)
            set_sspm(0b0100);
        else
            set_sspm(0b0101);
    }
    
    SSPCONbits.SSPEN = 1; // SSPCON<5> = SSPEN. Enable Synchronous Serial Port
}

// -------------------------------------------------------------------------
// -- MCC doesn't support PIC16LF88, so the following functions were      --
// -- modified from MCC-generated code for some other part (PIC16LF1503). --
// -------------------------------------------------------------------------

#define SPI_DUMMY_DATA 0x0

uint8_t SPI_Exchange8bit(uint8_t data)
{
    // Clear the Write Collision flag, to allow writing
    SSPCONbits.WCOL = 0;
    
    // Load TX data into shift register
    SSPBUF = data;

    while(SSPSTATbits.BF == 0); // Wait until RX data shifts in
    
    // Always read the RX shift register to prevent overflow
    return SSPBUF;
}

uint8_t SPI_Exchange8bitBuffer(uint8_t *dataIn, uint8_t bufLen, uint8_t *dataOut)
{
    uint8_t bytesWritten = 0;

    if(bufLen != 0)
    {
        if(dataIn != NULL)
        {
            while(bytesWritten < bufLen)
            {
                if(dataOut == NULL)
                {
                    SPI_Exchange8bit(dataIn[bytesWritten]);
                }
                else
                {
                    dataOut[bytesWritten] = SPI_Exchange8bit(dataIn[bytesWritten]);
                }

                bytesWritten++;
            }
        }
        else
        {
            if(dataOut != NULL)
            {
                while(bytesWritten < bufLen )
                {
                    dataOut[bytesWritten] = SPI_Exchange8bit(SPI_DUMMY_DATA);

                    bytesWritten++;
                }
            }
        }
    }

    return bytesWritten;
}

uint8_t SPI_IsBufferFull()
{
    return SSPSTATbits.BF;
}

uint8_t SPI_HasWriteCollisionOccured()
{
    return SSPCONbits.WCOL;
}

void SPI_ClearWriteCollisionStatus()
{
    SSPCONbits.WCOL = 0;
}
#endif	/* SPI_H */
