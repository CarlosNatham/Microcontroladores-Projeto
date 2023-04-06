/**************************
 * Definições relativas   *
 * a porta associada ao   *
 * display LCD            *
 **************************/
#define _XTAL_FREQ 4000000
#define DELAY_US        __delay_us(500)   // duração do pulso E
#define PORT_LCD        PORTD
#define TRIS_PORT_LCD   TRISD

/*************************
 * Comandos do display   *
 *************************/
#define D_CLEAR            0x01
#define D_HOME             0x02
#define D_ENTRY_MODE       0x06    // incremento da posição do cursor
#define D_CONTROL          0x0E    // display ON, cursor ON, blink OFF
#define D_CURSOR_SHIFT     0X18    // cursor shift ON
#define D_FUNCTION         0x28    // interface 4 bits, 2 linhas, fonte 5x8

/*********************************
 * Estrutura usada para acessar  *
 * o display                     *
 *********************************/
struct tDisplayPort {
    char RS : 1;
    char R_W : 1;
    char E : 1;
    char lixo : 1;
    char NData : 4;
};
struct tDisplayPort * pLCD;

//gera um pulso positivo no sinal enable do display

void pulseE() {
    DELAY_US;
    pLCD->E = 1;
    DELAY_US;
    pLCD->E = 0;
}
//leituras consecutivas ate que o busyflag esteja desligado

void waitIdle() {
    char aux = 0xFF;
    TRIS_PORT_LCD = 0xF0;
    pLCD->RS = 0;
    pLCD->R_W = 1; // operação de leitura

    while (aux & 0x80) {
        pLCD->E = 1;
        DELAY_US;

        pLCD->E = 0;
        DELAY_US;

        aux = (pLCD->NData) << 4;
        pLCD->E = 1;
        DELAY_US;
        pLCD->E = 0;
        DELAY_US;
        aux = aux | (pLCD->NData);
    }
    pLCD->R_W = 0;
    TRIS_PORT_LCD = 0x00;
}

// envia 1 byte de comando para o display. primeiro envia o nibble mais significativo dps o menos

void sendCMD(char dado) {
    pLCD ->RS = 0;
    pLCD ->R_W = 0; //indica operação da escrita
    pLCD ->NData = dado >> 4; //nibble mais significativo primeiro
    pulseE();
    //    waitIdle();
    pLCD -> RS = 0;
    pLCD->NData = dado & 0x0F; // nibble menos significativo
    pulseE();
    waitIdle();
}
// envia 1 byte de dado para o display

void sendData(char dado) {
    pLCD ->RS = 1;
    pLCD ->R_W = 0; //indica operação da escrita
    pLCD ->NData = dado >> 4; //nibble mais significativo primeiro
    pulseE();
    //    waitIdle();
    pLCD -> RS = 1;
    pLCD->NData = dado & 0x0F; // nibble menos significativo
    pulseE();
    waitIdle();
}
// inicia lcd

void initLCD() {
    TRIS_PORT_LCD = 0; // configura porta do display como saida

    pLCD = (struct tDisplayPort *) &PORT_LCD; // carrega o ponteiro da estrutura de manipulação do display com endereço da porta
    pLCD ->E = 0; // inicia linhas de controle
    pLCD ->RS = 0;
    pLCD ->R_W = 0;
    TRIS_PORT_LCD = 0; // configura port de controle como saida
    // envia comandos de inicialização do display
    pLCD->NData = 2; // configura interface com sendo de 4 bits
    pulseE();
    waitIdle();
    sendCMD(D_FUNCTION); // interface dados 4 bits/ 2 linhas/caracteres 5x8
    sendCMD(D_ENTRY_MODE); // incremento da posição do cursor
    sendCMD(D_CONTROL); // display ON, cursor ON, blink OFF
    sendCMD(D_CURSOR_SHIFT); // cursor shift ON
    sendCMD(D_CLEAR);
}


// posiciona cursor. Canto superior esquerdo tem coordenada x,y= 0,0

void gotoXY(unsigned char x, unsigned char y) {
    unsigned char temp;
    if (y == 0) //verifica se é primeira linha
    {
        sendCMD(0x80 + x);
    } else if (y == 1) //verifica se é segunda linha
    {
        sendCMD(0xC0 + x);
    } else if (y == 2) { //verifica se é terceira linha
        sendCMD(0x94 + x);
    } else if (y == 3) { //verifica se é quarta linha
        sendCMD(0xD4 + x);
    }
}

// função que vai redirecionar a saída do printf

void putch(char c) {
    sendData(c);
}