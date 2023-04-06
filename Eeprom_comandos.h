#include <xc.h>

void EEPROM_Write(unsigned char data, unsigned char address) {
    char INTSTATUS = INTCONbits.GIE;

    while (EECON1bits.WR); // Garante que nenhuma grava��o esteja em curso 

    EEADR = address; //Endere�o de grava��o
    EEDATA = data; //Dado a ser gravado na EEPROM
    EECON1bits.EEPGD = 0; //Sele��o da mem�ria EEPROM
    EECON1bits.CFGS = 0; //Acessar a mem�ria EEPROM
    EECON1bits.WREN = 1; //Liberar ciclo de escrita
    INTCONbits.GIE = 0; //Desativa momentaneamente a int. de perifericos
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1; //Iniciar grava��o
    INTCONbits.GIE = INTSTATUS; //Retorna a estado original da Int de perifericos
    EECON1bits.WREN = 0; //Liberar ciclo de escrita
}

unsigned char EEPROM_Read(unsigned char address) {
    char teste;
    while (EECON1bits.WR); // Garante que nenhuma grava��o esteja em curso 

    EEADR = address; //Carrega endere�o
    EECON1bits.EEPGD = 0; //Sele��o da mem�ria EEPROM
    EECON1bits.CFGS = 0; //Acessar a mem�ria EEPROM
    EECON1bits.RD = 1; //Solicita leitura da mem�ria
    teste = EEDATA;
    return teste; //Retorna dado lido
}