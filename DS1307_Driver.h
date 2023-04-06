#include <xc.h>
#include "I2C_Master.h"
#define _XTAL_FREQ 16000000

//Variavel global que ira armazenar o endereço do dispositivo escravo
unsigned char Addr = 0b01010000;
int x = 20;

//Escreve a data no formato: dd/mm/aa
/*
 *endereço dia semana : 03h
 *endereço dia : 04h 
 *endereço mes : 05h
 *endereço ano : 06h 
 */
void write_Date(char day, char date, char month, char year){ 		
    ESCRITA_24C08(Addr,x,day);
    x++;
    ESCRITA_24C08(Addr,x,date);
    x++;
    ESCRITA_24C08(Addr,x,month);
    x++;
    ESCRITA_24C08(Addr,x,year);
    x++;
}

//Escreve a hora no formato: hh:mm:ss
/*
 *endereço segundo : 00h
 *endereço minuto : 01h
 *endereço hora : 02h 
 * 
 * BIT 0:3 -> unidade
 * BIT 4:6 -> dezena
 * 
 */
void write_Hour(char hour, char minute, char second){
    ESCRITA_24C08(Addr,x,hour);
    x++;
    ESCRITA_24C08(Addr,x,minute);
    x++;
    ESCRITA_24C08(Addr,x,second);
    x++;
}

 /*
 *endereço segundo : 00h
 *endereço minuto : 01h
 *endereço hora : 02h
 *endereço dia semana : 03h
 *endereço dia : 04h 
 *endereço mes : 05h
 *endereço ano : 06h
 */
char read_Register(char ADDR){
    return LEITURA_24C08(Addr,ADDR);
}

//Recebe um vetor de char de tamanho 9 e modifica ele com o horário formatado em "hh:mm:ss"
void read_Hour(char *hour)
{
    char vet[] = {'0','1','2','3','4','5','6','7','8','9'};
    
    int j = 0;
    for(int i = 2;i >= 0; i--)
    {
        hour[j] = vet[read_Register(i)>>4];
        j++;
        hour[j] = vet[read_Register(i)&0x0F];
        j++;
        if(i > 0)
            hour[j] = ':';
        j++;
    }
}

//Recebe um vetor de char de tamanho 9 e modifica ele com a data formatada em "dd/mm/aa"
void read_Date(char *date){
    char vet[] = {'0','1','2','3','4','5','6','7','8','9'};
    
    int j = 0;
    for(int i = 4;i <= 6; i++)
    {
        date[j] = vet[read_Register(i)>>4];
        j++;
        date[j] = vet[read_Register(i)&0x0F];
        j++;
        if(i < 6)
            date[j] = '/';
        j++;
    }
}
  