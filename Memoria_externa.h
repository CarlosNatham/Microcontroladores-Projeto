#include <xc.h>
#pragma config MCLRE = ON, WDT = OFF, OSC = HS
#define _XTAL_FREQ 8000000
unsigned char ENDH = 0b01010000;
unsigned char TEMP;
//*******************************************************************

/*fun��o que escreve o valor no endere�o de mem�ria recebido  como par�metro
o valor escrito � retornado. Caso ocorra um erro na transmiss�o � retornado o valor -1.*/
char ESCRITA_24C08(unsigned char ENDH, unsigned char ENDL, char DADO) {
    char x; //declara��o de vari�vel local 
    x = I2C_LIVRE(); //chamada � fun��o com retorno de valor. Verifica se barramento est� livre
    if (x == 0) //se barramento ocupado, aborta transmiss�o e retorna
    {
        I2C_STOP(); //gera bit STOP
        return -1; //erro na transmiss�o
    } else //barramento livre
    {
        I2C_START(); //barramento livre, gera condi��o START
        TEMP = ENDH << 1; //rotaciona END_I2C para a esquerda (insere bit R_W para escrita) 
        I2C_TRANSMITE(TEMP); //transmite endere�o alto
        if (!I2C_TESTA_ACK()) //se erro na transmiss�o, aborta transmiss�o
        {
            I2C_STOP(); //gera bit STOP
            return -1; //erro na transmiss�o
        }
        I2C_TRANSMITE(ENDL); //transmite endere�o baixo
        if (!I2C_TESTA_ACK()) //se erro na transmiss�o, aborta transmiss�o
        {
            I2C_STOP(); //gera bit STOP
            return -1; //erro na transmiss�o
        }
        I2C_TRANSMITE(DADO); //transmite dado
        if (!I2C_TESTA_ACK()) //se erro na transmiss�o, aborta transmiss�o
        {
            I2C_STOP(); //gera bit STOP
            return -1; //erro na transmiss�o
        }
        I2C_STOP(); //gera bit STOP
        return DADO; //transmiss�o feita com sucesso
    }
}//*********************************************************************

/*fun��o que l� o valor no endere�o de mem�ria recebido  como par�metro.
o valor lido � retornado. Caso ocorra um erro na transmiss�o � retornado o valor -1.*/
char LEITURA_24C08(unsigned char _ENDH, unsigned char _ENDL) {
    char x; //declara��o de vari�vel local 
    unsigned char DADO_I2C; //declara��o de vari�vel local 
    x = I2C_LIVRE(); //chamada � fun��o com retorno de valor. Verifica se barramento est� livre
    if (x == 0) //se barramento ocupado, aborta transmiss�o e retorna
    {
        I2C_STOP(); //gera bit STOP
        return -1; //erro na transmiss�o
    } else //barramento livre
    {
        I2C_START(); //barramento livre, gera condi��o START
        TEMP = _ENDH << 1; //rotaciona END_I2C para a esquerda 
        I2C_TRANSMITE(TEMP); //transmite endere�o
        if (!I2C_TESTA_ACK()) //se erro na transmiss�o, aborta transmiss�o
        {
            I2C_STOP(); //gera bit STOP
            return -1; //erro na transmiss�o
        }
        I2C_TRANSMITE(_ENDL); //transmite endere�o baixo
        if (!I2C_TESTA_ACK()) //se erro na transmiss�o, aborta transmiss�o
        {
            I2C_STOP(); //gera bit STOP
            return -1; //erro na transmiss�o
        }
        I2C_RESTART();
        TEMP |= 0b00000001; //insere bit R_W para leitura
        I2C_TRANSMITE(TEMP); //transmite endere�o
        if (!I2C_TESTA_ACK()) //se erro na transmiss�o, aborta transmiss�o
        {
            I2C_STOP(); //gera bit STOP
            return -1; //erro na transmiss�o
        }
        DADO_I2C = I2C_RECEBE(); //recebe dado
        I2C_NACK(); //gera bit NACK
        I2C_STOP(); //gera bit STOP
        return DADO_I2C; //transmiss�o feita com sucesso
    }
}//*********************************************************************

/*esta fun��o detecta se o ciclo de escrita na EEPROM terminou.Caso a 
escrita tenha terminado ela retornar� 0, caso contrario retornar� -1*/
char Detecta_fim_escrita(void) {
    char x; //declara��o de vari�vel local 
    unsigned char DADO_I2C; //declara��o de vari�vel local 
    x = I2C_LIVRE(); //chamada � fun��o com retorno de valor. Verifica se barramento est� livre
    if (x == 0) //se barramento ocupado, aborta transmiss�o e retorna
    {
        I2C_STOP(); //gera bit STOP
        return -1; //barramento ocupado
    } else //barramento livre
    {
        I2C_START(); //barramento livre, gera condi��o START
        TEMP = ENDH << 1; //rotaciona END_I2C para a esquerda 
        I2C_TRANSMITE(TEMP); //transmite endere�o
        if (!I2C_TESTA_ACK()) //se erro na transmiss�o, aborta transmiss�o
        {
            I2C_STOP(); //gera bit STOP
            return -1; //ciclode esrita n�o terminou
        }
        I2C_STOP();
        return 0; //ciclo de escrita terminou
    }
    //***********************************************************************
}