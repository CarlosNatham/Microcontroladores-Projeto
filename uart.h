#include <xc.h>
#include <stdio.h>
#pragma BOREN = OFF
#define _XTAL_FREQ 4000000

void sendChar(char ch) {
    TXREG = ch;
    while (!TXSTAbits.TRMT);
}

void UART_send_string(char* st_pt) {
    while (*st_pt) //if there is a char
        sendChar(*st_pt++); //process it as a byte data
}

char charAvailable(void) {
    return PIR1bits.RCIF;
}

char getChar(void) {
    while (!PIR1bits.RCIF);
    if (RCSTAbits.FERR) {
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
        return -1;
    } else return RCREG;
}

void initUART(void) {
    TRISCbits.RC6 = 1; // portas de TX e RX como entrada digital
    TRISCbits.RC7 = 1;
    SPBRG = 25; // ajuste do gerador de baud rate (9600 bps p/ fosc = 4MHz)
    BAUDCONbits.BRG16 = 0; // modo de 8 bits
    TXSTAbits.BRGH = 1;
    TXSTAbits.SYNC = 0; // modo assíncrono
    TXSTAbits.TXEN = 1; // habilita tx
    RCSTAbits.SPEN = 1; // serial habilitada
    RCSTAbits.CREN = 1; // modo continuo para recepcao   
}

void putch_uart(unsigned char byte) {
    TXREG = byte;
    while (!TXSTAbits.TRMT);
}
