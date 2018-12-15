#ifndef UART_H
#define	UART_H

void uart_init(unsigned long baud_rate)
{
    // Setting I/O pins for UART
    TRISB5 = 0; // TX Pin set as output
    TRISB2 = 1; // RX Pin set as input
    
    // Initialise SPBRG and BRGH to set baud rate
    SPBRG = (((unsigned long)(_XTAL_FREQ)/16UL) / baud_rate) - 1UL;
    BRGH  = 1;  // for high baud_rate
    
    // Enable asynchronous serial port
    SYNC  = 0;    // Asynchronous
    SPEN  = 1;    // Enable serial port pins

    // Set enables
    TXEN  = 1;    // enable transmission
    CREN  = 1;    // enable reception
    
    // Set 8-bit mode
    TX9   = 0;    // 8-bit TX mode
    RX9   = 0;    // 8-bit RX mode
}

void uart_send_char(char c)  
{
    while (!TXIF); // Wait until TX buffer is ready
    
    // Write TX buffer
    TXREG = c;
}

char UART_get_char()   
{
    // If error, then reset
    if (OERR)
    {
        CREN = 0;
        CREN = 1;
    }
    
    while (!RCIF); // Wait until RX buffer is ready
    
    // Read RX buffer
    return RCREG;
}

void uart_send_string(const char* s)
{
    while (*s)
        uart_send_char(*s++);
}

#endif	/* UART_H */
