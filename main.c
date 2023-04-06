#include <xc.h>
#include <stdio.h>
#include "display.h"
#include "Eeprom_comandos.h"
#include "I2C_Master.h"
#include "uart.h"
#define r0 PORTBbits.RB0
#define r1 PORTBbits.RB1
#define r2 PORTBbits.RB2
#define r3 PORTBbits.RB3
#define r4 PORTBbits.RB4
#define r5 PORTBbits.RB5
#define r6 PORTBbits.RB6
#define button PORTBbits.RB7
#define Led_Vermelho PORTCbits.RC1
#define Led_Verde PORTCbits.RC0
#define Led_Amarelo PORTCbits.RC2
#define Alarme PORTCbits.RC5
#pragma config MCLRE = ON, WDT = OFF, OSC = HS
#define _XTAL_FREQ 8000000

char ESCRITA_24C08(unsigned char ENDH, unsigned char ENDL, char DADO);
char LEITURA_24C08(unsigned char _ENDH, unsigned char _ENDL);
char Detecta_fim_escrita(void);

unsigned char ENDH = 0b01010000;
unsigned char TEMP;
char a, b, c, d;
int pos;

char read_Register(char ADDR) {
    return I2C_LEITURA(0x68, ADDR);
}

//Recebe um vetor de char de tamanho 9 e modifica ele com o horário formatado em "hh:mm:ss"

void read_Hour(char *hour) {
    char vet[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

    int j = 0;
    for (int i = 2; i >= 0; i--) {
        hour[j] = vet[read_Register(i) >> 4];
        j++;
        hour[j] = vet[read_Register(i)&0x0F];
        j++;
        if (i > 0)
            hour[j] = ':';
        j++;
    }
}

//Recebe um vetor de char de tamanho 9 e modifica ele com a data formatada em "dd/mm/aa"

void read_Date(char *date) {
    char vet[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

    int j = 0;
    for (int i = 4; i <= 6; i++) {
        date[j] = vet[read_Register(i) >> 4];
        j++;
        date[j] = vet[read_Register(i)&0x0F];
        j++;
        if (i < 6)
            date[j] = '/';
        j++;
    }
}

char verifica_linha_col() { //função para verificar qual foi a linha e coluna que foi para zero
    r0 = 0; //r0 em zero, significa que a primeira linha teve algum botão pressionado
    r1 = 1;
    r2 = 1;
    r3 = 1;
    //então é verificado em qual coluna recebeu o valor zero tambem para indeficar o botão pressionado
    if (r4 == 0) {
        __delay_ms(100);
        while (r4 == 0);
        return '*';
    }
    if (r5 == 0) {
        __delay_ms(100);
        while (r5 == 0);
        return '0';
    }
    if (r6 == 0) {
        __delay_ms(100);
        while (r6 == 0);
        return '#';
    }

    r0 = 1;
    r1 = 0; //r1 em zero, significa que a segunda linha teve algum botão pressionado
    r2 = 1;
    r3 = 1;
    //então é verificado em qual coluna recebeu o valor zero tambem para indeficar o botão pressionado
    if (r4 == 0) {
        __delay_ms(100);
        while (r4 == 0);
        return '7';
    }
    if (r5 == 0) {
        __delay_ms(100);
        while (r5 == 0);
        return '8';
    }
    if (r6 == 0) {
        __delay_ms(100);
        while (r6 == 0);
        return '9';
    }

    r0 = 1;
    r1 = 1;
    r2 = 0; //r2 em zero, significa que a terceira linha teve algum botão pressionado
    r3 = 1;
    //então é verificado em qual coluna recebeu o valor zero tambem para indeficar o botão pressionado
    if (r4 == 0) {
        __delay_ms(100);
        while (r4 == 0);
        return '4';
    }
    if (r5 == 0) {
        __delay_ms(100);
        while (r5 == 0);
        return '5';
    }
    if (r6 == 0) {
        __delay_ms(100);
        while (r6 == 0);
        return '6';
    }

    r0 = 1;
    r1 = 1;
    r2 = 1;
    r3 = 0; //r3 em zero, significa que a quarta linha teve algum botão pressionado
    //então é verificado em qual coluna recebeu o valor zero tambem para indeficar o botão pressionado
    if (r4 == 0) {
        __delay_ms(100);
        while (r4 == 0);
        return '1';
    }
    if (r5 == 0) {
        __delay_ms(100);
        while (r5 == 0);
        return '2';
    }
    if (r6 == 0) {
        __delay_ms(100);
        while (r6 == 0);
        return '3';
    }

    return 'n'; //se caso não foi identificado nenhuma tecla, retorna n para continuar esperando 
}

char pega_tecla() { //função para retornar a tecla informada
    char tecla = 'n'; //define uma varivel tecla ja valendo n, que siginifica que não foi digitado nada
    while (tecla == 'n') //enquanto a tecla não for digitada, ele fica verificando
        tecla = verifica_linha_col();
    return tecla; //assim que for digitada, ele retorna a tecla
}

char verifica_senha(char vet[]) {
    //função para verificar todas as senhas gravadas e identificar se é igual a senha digitada
    a = LEITURA_24C08(ENDH, 0); //pega as 4 primeiras posições da memoria
    b = LEITURA_24C08(ENDH, 1);
    c = LEITURA_24C08(ENDH, 2);
    d = LEITURA_24C08(ENDH, 3);
    if (vet[0] == a && vet[1] == b && vet[2] == c && vet[3] == d) { //verifica se é igual ao senha digitada pelo usuario
        pos = 0; //se for igual, guarda a primeira posição da senha
        return 'i'; //e retorna o caracter que identifica que as senhas são iguais
    }
    //caso ele não consiga nas primeiras 4 posições, ele fica verificando todas as demais, de 4 em 4
    a = LEITURA_24C08(ENDH, 4); //pega mais 4 primeiras posições da memoria
    b = LEITURA_24C08(ENDH, 5);
    c = LEITURA_24C08(ENDH, 6);
    d = LEITURA_24C08(ENDH, 7);
    if (vet[0] == a && vet[1] == b && vet[2] == c && vet[3] == d) { //verifica se é igual ao senha digitada pelo usuario
        pos = 4; //se for igual, guarda a primeira posição da senha
        return 'i'; //e retorna o caracter que identifica que as senhas são iguais
    }

    a = LEITURA_24C08(ENDH, 8); //pega mais 4 primeiras posições da memoria
    b = LEITURA_24C08(ENDH, 9);
    c = LEITURA_24C08(ENDH, 10);
    d = LEITURA_24C08(ENDH, 11);
    if (vet[0] == a && vet[1] == b && vet[2] == c && vet[3] == d) { //verifica se é igual ao senha digitada pelo usuario
        pos = 8; //se for igual, guarda a primeira posição da senha
        return 'i'; //e retorna o caracter que identifica que as senhas são iguais
    }

    a = LEITURA_24C08(ENDH, 12); //pega mais 4 primeiras posições da memoria
    b = LEITURA_24C08(ENDH, 13);
    c = LEITURA_24C08(ENDH, 14);
    d = LEITURA_24C08(ENDH, 15);
    if (vet[0] == a && vet[1] == b && vet[2] == c && vet[3] == d) { //verifica se é igual ao senha digitada pelo usuario
        pos = 12; //se for igual, guarda a primeira posição da senha
        return 'i'; //e retorna o caracter que identifica que as senhas são iguais
    }

    //caso não entre em nenhuma condição, quer dizer que a senha digitada é diferente das gravadas
    //então retornamos um caracter diferente para identificar que não foi localizado nada
    return 'd';
}

void zera_tudo() { //função usada para zerar tudo na memoria caso precise
    for (int i = 0; i < 260; i++) {
        ESCRITA_24C08(ENDH, i, 0xFF);
        __delay_ms(10);
    }
    EEPROM_Write(0, 0);
    EEPROM_Write(0, 1);
    EEPROM_Write(20, 2);
}

void inicia_log(int cont){
    char aux2;
    for(int i = 20; i < cont; i++){
        aux2 = LEITURA_24C08(ENDH, i);
        if(aux2 == 'C'){
           UART_send_string("Cadastro Realizado as: "); 
        }else if(aux2 == 'L'){
           UART_send_string("Login Realizado as: "); 
        }else if(aux2 == 'E'){
           UART_send_string("Exclusao Realizada as: "); 
        }else if(aux2 == 0){
            UART_send_string(" ");
        }else if(aux2 == '|'){
            sendChar(0x0D);
        }else{
            sendChar(aux2);
        }
    }
}

void main(void) {
    ADCON1 = 0x0F; //portas como digitais
    TRISBbits.RB0 = 0; //RB0 saida
    TRISBbits.RB1 = 0; //RB1 saida
    TRISBbits.RB2 = 0; //RB2 saida
    TRISBbits.RB3 = 0; //RB3 saida
    TRISBbits.RB4 = 1; //RB4 entrada
    TRISBbits.RB5 = 1; //RB5 entrada
    TRISBbits.RB6 = 1; //RB6 entrada
    TRISBbits.RB7 = 1; //RB7 entrada
    INTCON2bits.RBPU = 0; //ativa os resistores internos de pull up
    TRISCbits.RC0 = 0; //RC0 saida
    TRISCbits.RC1 = 0; //RC1 saida
    TRISCbits.RC2 = 0; //RC2 saida
    TRISCbits.RC3 = 1; //RC3 entrada
    TRISCbits.RC4 = 1; //RC4 entrada
    TRISCbits.RC5 = 0; //RC3 saida
    SSPCON1 = 0b00101000; //configura dispositivo como mestre
    //Habilita porta serial e ativa pinos SDA E SCL <5>
    //ativa modo mestre <3:0>
    SSPADD = 19; //bit rate de 100kbps a Fosc = 8MHz
    SSPSTAT = 0b10000000; //controle de inclinação desativado <7>
    //níveis de entrada conforme especificação I2C <6>
    initLCD(); //inicia display
    initUART(); //inicia o terminal virtual
    //variaveis de controle para senhas e demais funcionalidades
    char admin[4] = {'0', '0', '0', '0'};
    char tecla;
    char senha_cadastro[4] = {'a', 'a', 'a', 'a'};
    char senha_login[4] = {'a', 'a', 'a', 'a'};
    char senha_excluir[4] = {'a', 'a', 'a', 'a'};
    char senha_admin[4] = {'a', 'a', 'a', 'a'};
    int cont1 = 0, cont2 = 0, cont3 = 0, invasao = 0, qtdsenhas = 0, posex = 0;
    char aux, resp;
    char hour[9], date[9], log[20];

    while (1) {
        //zera_tudo();
        qtdsenhas = EEPROM_Read(1); //ja inicia salvando a quantidade de senhas ja gravadas em uma variavel
        cont1 = EEPROM_Read(0); //e a posição que parou a ultima senha em outra variavel
        cont3 = EEPROM_Read(2);
        if(!button){
            inicia_log(cont3);
        }
        //zera tudo e exibe o menu principal
        Alarme = 0;
        Led_Verde = 0;
        Led_Vermelho = 0;
        Led_Amarelo = 0;
        sendCMD(D_CLEAR);
        gotoXY(0, 0);
        printf("Escolher Opcao:");
        gotoXY(0, 1);
        printf("1-Cadastrar");
        gotoXY(0, 2);
        printf("2-Acessar");
        gotoXY(0, 3);
        printf("3-Excluir");
        tecla = pega_tecla();
        switch (tecla) {
            case '1'://se tecla for 1, vai para a opção de cadastro de senha
                sendCMD(D_CLEAR);
                gotoXY(2, 1);
                printf("Digite sua senha");
                gotoXY(8, 2);
                //aguarda o usuario digitar a senha
                while (senha_cadastro[3] == 'a') {
                    tecla = pega_tecla();
                    printf("%c", tecla);
                    senha_cadastro[cont2] = tecla;
                    cont2++;
                }
                __delay_ms(1000);
                resp = verifica_senha(senha_cadastro); // verifica se ela ja existe

                if (senha_cadastro[0] == admin[0] && senha_cadastro[1] == admin[1] && senha_cadastro[2] == admin[2] && senha_cadastro[3] == admin[3]) {
                    sendCMD(D_CLEAR); //verifica se a senha digitada não é igual a admin
                    gotoXY(4, 1);
                    printf("Essa Senha");
                    gotoXY(4, 2);
                    printf("Ja Existe");
                    Led_Vermelho = 1; //acende led vermelho
                    for (int j = 0; j < 4; j++) {
                        senha_cadastro[j] = 'a';
                    }
                    cont2 = 0;
                    __delay_ms(1000);
                    break;
                }

                if (qtdsenhas < 4 && resp == 'd') { //se caso não exister e tiver menos que 4 senhas cadastradas
                    read_Date(date);
                    read_Hour(hour);
                    log[0] = 'C';
                    for (int i = 1; i < 10; i++) {
                        log[i] = date[i-1];
                        log[i + 9] = hour[i-1];
                    }
                    log[19] = '|';
                    for (int i = 0; i < 4; i++) { //grava a senha na memoria
                        ESCRITA_24C08(ENDH, cont1, senha_cadastro[i]);
                        cont1++;
                        __delay_ms(10);
                    }
                    for (int i = 0; i < 20; i++) { //grava a senha na memoria
                        ESCRITA_24C08(ENDH, cont3, log[i]);
                        cont3++;
                        __delay_ms(10);
                    }
                    qtdsenhas++; //incrementa a quantidade de senhas e grava na memoria interna essas 2 variaveis para controle
                    EEPROM_Write(cont1, 0);
                    EEPROM_Write(qtdsenhas, 1);
                    EEPROM_Write(cont3, 2);
                    sendCMD(D_CLEAR);
                    gotoXY(2, 1);
                    printf("Senha cadastrada");
                    Led_Verde = 1; //acende led verde de confirmação
                    for (int j = 0; j < 4; j++) {//zera vetor auxiliar
                        senha_cadastro[j] = 'a';
                    }
                    cont2 = 0;
                    __delay_ms(1000);
                    break;
                } else if (resp == 'i' && qtdsenhas < 3) { //se for igual a uma senha ja existente, entra nessa condição
                    sendCMD(D_CLEAR); //exibe que a senha ja existe e não deixa cadastrar
                    gotoXY(4, 1);
                    printf("Essa Senha");
                    gotoXY(4, 2);
                    printf("Ja Existe");
                    Led_Vermelho = 1; //acende led vermelho
                    for (int j = 0; j < 4; j++) {
                        senha_cadastro[j] = 'a';
                    }
                    cont2 = 0;
                    __delay_ms(1000);
                    break;
                } else if (qtdsenhas == 4) { //se caso ja tiver 4 senhas, entra nessa condição
                    sendCMD(D_CLEAR); //informa que ja atingiu o limite de senhas e não permite continuar
                    gotoXY(1, 1);
                    printf("O limite de senhas");
                    gotoXY(1, 2);
                    printf("Ja foi excedido");
                    Led_Vermelho = 1; //acende led vermelho
                    for (int j = 0; j < 4; j++) {
                        senha_cadastro[j] = 'a';
                    }
                    cont2 = 0;
                    __delay_ms(1000);
                    break;
                }

            case '2'://caso seja opção 2, vai para a parte de login
                sendCMD(D_CLEAR);
                gotoXY(2, 1);
                printf("Informe sua senha");
                gotoXY(8, 2);
                //aguarda usuario informar o login
                while (senha_login[3] == 'a') {
                    tecla = pega_tecla();
                    printf("%c", tecla);
                    senha_login[cont2] = tecla;
                    cont2++;
                }
                __delay_ms(1000);
                sendCMD(D_CLEAR);
                resp = verifica_senha(senha_login); //verifica se senha ja existe

                if (resp == 'i') {//se ela ja existir, confirma o acesso
                    read_Date(date);
                    read_Hour(hour);
                    log[0] = 'L';
                    for (int i = 1; i < 10; i++) {
                        log[i] = date[i-1];
                        log[i + 9] = hour[i-1];
                    }
                    log[19] = '|';
                    for (int i = 0; i < 20; i++) { //grava a senha na memoria
                        ESCRITA_24C08(ENDH, cont3, log[i]);
                        cont3++;
                        __delay_ms(10);
                    }
                    EEPROM_Write(cont3, 2);
                    gotoXY(2, 1);
                    printf("Acesso confirmado");
                    Led_Verde = 1; //acende led verde
                    cont2 = 0; //zera as variaveis de controle
                    invasao = 0;
                    for (int j = 0; j < 4; j++) {
                        senha_login[j] = 'a';
                    }
                    __delay_ms(1000);
                    break;
                } else if (resp == 'd') { //se ela não existir na memoria   
                    gotoXY(2, 1); //informa que a senha não existe e não permite acessar
                    printf("Senha Incorreta");
                    Led_Vermelho = 1; //acende led vermelho
                    cont2 = 0;
                    invasao++; //incremente variavel que controla invasão
                    for (int j = 0; j < 4; j++) {
                        senha_login[j] = 'a';
                    }
                    __delay_ms(1000);
                    if (invasao == 3) {//se forem 3 tentativas erradas, entra nessa condição
                        gotoXY(2, 0); // que avisa que atingiu o limite de login errados
                        printf("Detectado Varias");
                        gotoXY(1, 1);
                        printf("Tentativas Erradas");
                        gotoXY(1, 2);
                        printf("De Acessar ao Cofre");
                        gotoXY(2, 3);
                        printf("Alerta Ligado!!!");
                        invasao = 0;
                        Alarme = 1; //alarme é disparado
                        Led_Vermelho = 1;
                        __delay_ms(2000);

                        sendCMD(D_CLEAR);
                        gotoXY(1, 0);
                        printf("Informe senha admin");
                        gotoXY(1, 1);
                        printf("Para Liberar");
                        gotoXY(8, 2);
                        //solicita senha admin para poder liberar o cofre
                        while (senha_admin[3] == 'a') {
                            tecla = pega_tecla();
                            printf("%c", tecla);
                            senha_admin[cont2] = tecla;
                            cont2++;
                        }
                        cont2 = 0;

                        if (senha_admin[0] == admin[0] && senha_admin[1] == admin[1] && senha_admin[2] == admin[2] && senha_admin[3] == admin[3]) {
                            sendCMD(D_CLEAR); //se for a senha admin, ele libera o cofre e desativa o alarme
                            gotoXY(2, 1);
                            printf("Cofre Liberado");
                            Led_Verde = 1;
                            Alarme = 0;
                            Led_Vermelho = 0;
                            for (int j = 0; j < 4; j++) {
                                senha_admin[j] = 'a';
                            }
                            __delay_ms(1000);
                        }
                    }
                    break;
                }

            case '3'://caso seja opção 3, vai para a parte de exclusão
                sendCMD(D_CLEAR);
                gotoXY(1, 0);
                printf("Informe senha admin");
                gotoXY(1, 1);
                printf("Para Confirmar");
                gotoXY(8, 2);
                //solicita senha admin para continuar
                while (senha_admin[3] == 'a') {
                    tecla = pega_tecla();
                    printf("%c", tecla);
                    senha_admin[cont2] = tecla;
                    cont2++;
                }
                cont2 = 0;
                //verifica se a senha admin esta certa
                if (senha_admin[0] == admin[0] && senha_admin[1] == admin[1] && senha_admin[2] == admin[2] && senha_admin[3] == admin[3]) {
                    sendCMD(D_CLEAR); //se estiver, ele permite continuar
                    gotoXY(2, 1);
                    printf("Senha Correta");
                    Led_Verde = 1;
                    __delay_ms(1000);
                } else {//caso não, ele exibe que esta errada e não deixa continuar
                    sendCMD(D_CLEAR);
                    gotoXY(2, 1);
                    printf("Senha Incorreta");
                    Led_Vermelho = 1;
                    __delay_ms(1000);
                    for (int j = 0; j < 4; j++) {
                        senha_admin[j] = 'a';
                    }
                    break;
                }

                sendCMD(D_CLEAR);
                Led_Vermelho = 0;
                Led_Verde = 0;
                gotoXY(1, 0);
                printf("Informe sua senha");
                gotoXY(1, 1);
                printf("Para Exclusao");
                gotoXY(8, 2);
                //agora espera digitar a senha que deseja excluir
                while (senha_excluir[3] == 'a') {
                    tecla = pega_tecla();
                    printf("%c", tecla);
                    senha_excluir[cont2] = tecla;
                    cont2++;
                }
                __delay_ms(1000);
                resp = verifica_senha(senha_excluir); //verifica se a senha existe

                if (resp == 'i') {//se existir, entra nessa condição
                    read_Date(date);
                    read_Hour(hour);
                    log[0] = 'E';
                    for (int i = 1; i < 10; i++) {
                        log[i] = date[i-1];
                        log[i + 9] = hour[i-1];
                    }
                    log[19] = '|';
                    for (int i = 0; i < 20; i++) { //grava a senha na memoria
                        ESCRITA_24C08(ENDH, cont3, log[i]);
                        cont3++;
                        __delay_ms(10);
                    }
                    EEPROM_Write(cont3, 2);
                    for (int i = pos; i < pos + 4; i++) {//zera as 4 posições dessa senha
                        ESCRITA_24C08(ENDH, i, 0xFF);
                        __delay_ms(10);
                    }
                    posex = pos;
                    for (int i = pos + 4; i < 20; i++) {//e depois joga as demais posições que estão a frente
                        aux = LEITURA_24C08(ENDH, i); //da senha excluida para trás
                        ESCRITA_24C08(ENDH, posex, aux);
                        posex++;
                        __delay_ms(10);
                    }
                    sendCMD(D_CLEAR);
                    gotoXY(1, 1);
                    printf("Exclusao confirmada");
                    Led_Amarelo = 1; //acende led amarelo
                    cont2 = 0;
                    posex = 0;
                    cont1 = cont1 - 4; //diminiu em 4 a variavel que guarda posição da memoria
                    qtdsenhas--; //diminiu a quantidade de senhas totais
                    EEPROM_Write(cont1, 0); //salva na memoria interna para controle
                    EEPROM_Write(qtdsenhas, 1);
                    for (int j = 0; j < 4; j++) {
                        senha_excluir[j] = 'a';
                        senha_admin[j] = 'a';
                    }
                    __delay_ms(1000);
                } else if (resp == 'd') {//se caso a senha não existir
                    sendCMD(D_CLEAR); //entra nessa opção e avisa que a senha não existe
                    gotoXY(1, 1);
                    printf("Esta Senha");
                    gotoXY(1, 2);
                    printf("Nao Existe");
                    Led_Vermelho = 1;
                    cont2 = 0;
                    posex = 0;
                    for (int j = 0; j < 4; j++) {
                        senha_excluir[j] = 'a';
                        senha_admin[j] = 'a';
                    }
                    __delay_ms(1000);
                }
                break;
        }
    }
    return;
}

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