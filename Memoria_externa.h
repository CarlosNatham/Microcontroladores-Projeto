#include <xc.h>
#pragma config MCLRE = ON, WDT = OFF, OSC = HS
#define _XTAL_FREQ 8000000
unsigned char ENDH = 0b01010000;
unsigned char TEMP;
//*******************************************************************

/*função que escreve o valor no endereço de memória recebido  como parâmetro
o valor escrito é retornado. Caso ocorra um erro na transmissão é retornado o valor -1.*/
char ESCRITA_24C08(unsigned char ENDH, unsigned char ENDL, char DADO) {
    char x; //declaração de variável local 
    x = I2C_LIVRE(); //chamada à função com retorno de valor. Verifica se barramento está livre
    if (x == 0) //se barramento ocupado, aborta transmissão e retorna
    {
        I2C_STOP(); //gera bit STOP
        return -1; //erro na transmissão
    } else //barramento livre
    {
        I2C_START(); //barramento livre, gera condição START
        TEMP = ENDH << 1; //rotaciona END_I2C para a esquerda (insere bit R_W para escrita) 
        I2C_TRANSMITE(TEMP); //transmite endereço alto
        if (!I2C_TESTA_ACK()) //se erro na transmissão, aborta transmissão
        {
            I2C_STOP(); //gera bit STOP
            return -1; //erro na transmissão
        }
        I2C_TRANSMITE(ENDL); //transmite endereço baixo
        if (!I2C_TESTA_ACK()) //se erro na transmissão, aborta transmissão
        {
            I2C_STOP(); //gera bit STOP
            return -1; //erro na transmissão
        }
        I2C_TRANSMITE(DADO); //transmite dado
        if (!I2C_TESTA_ACK()) //se erro na transmissão, aborta transmissão
        {
            I2C_STOP(); //gera bit STOP
            return -1; //erro na transmissão
        }
        I2C_STOP(); //gera bit STOP
        return DADO; //transmissão feita com sucesso
    }
}//*********************************************************************

/*função que lê o valor no endereço de memória recebido  como parâmetro.
o valor lido é retornado. Caso ocorra um erro na transmissão é retornado o valor -1.*/
char LEITURA_24C08(unsigned char _ENDH, unsigned char _ENDL) {
    char x; //declaração de variável local 
    unsigned char DADO_I2C; //declaração de variável local 
    x = I2C_LIVRE(); //chamada à função com retorno de valor. Verifica se barramento está livre
    if (x == 0) //se barramento ocupado, aborta transmissão e retorna
    {
        I2C_STOP(); //gera bit STOP
        return -1; //erro na transmissão
    } else //barramento livre
    {
        I2C_START(); //barramento livre, gera condição START
        TEMP = _ENDH << 1; //rotaciona END_I2C para a esquerda 
        I2C_TRANSMITE(TEMP); //transmite endereço
        if (!I2C_TESTA_ACK()) //se erro na transmissão, aborta transmissão
        {
            I2C_STOP(); //gera bit STOP
            return -1; //erro na transmissão
        }
        I2C_TRANSMITE(_ENDL); //transmite endereço baixo
        if (!I2C_TESTA_ACK()) //se erro na transmissão, aborta transmissão
        {
            I2C_STOP(); //gera bit STOP
            return -1; //erro na transmissão
        }
        I2C_RESTART();
        TEMP |= 0b00000001; //insere bit R_W para leitura
        I2C_TRANSMITE(TEMP); //transmite endereço
        if (!I2C_TESTA_ACK()) //se erro na transmissão, aborta transmissão
        {
            I2C_STOP(); //gera bit STOP
            return -1; //erro na transmissão
        }
        DADO_I2C = I2C_RECEBE(); //recebe dado
        I2C_NACK(); //gera bit NACK
        I2C_STOP(); //gera bit STOP
        return DADO_I2C; //transmissão feita com sucesso
    }
}//*********************************************************************

/*esta função detecta se o ciclo de escrita na EEPROM terminou.Caso a 
escrita tenha terminado ela retornará 0, caso contrario retornará -1*/
char Detecta_fim_escrita(void) {
    char x; //declaração de variável local 
    unsigned char DADO_I2C; //declaração de variável local 
    x = I2C_LIVRE(); //chamada à função com retorno de valor. Verifica se barramento está livre
    if (x == 0) //se barramento ocupado, aborta transmissão e retorna
    {
        I2C_STOP(); //gera bit STOP
        return -1; //barramento ocupado
    } else //barramento livre
    {
        I2C_START(); //barramento livre, gera condição START
        TEMP = ENDH << 1; //rotaciona END_I2C para a esquerda 
        I2C_TRANSMITE(TEMP); //transmite endereço
        if (!I2C_TESTA_ACK()) //se erro na transmissão, aborta transmissão
        {
            I2C_STOP(); //gera bit STOP
            return -1; //ciclode esrita não terminou
        }
        I2C_STOP();
        return 0; //ciclo de escrita terminou
    }
    //***********************************************************************
}