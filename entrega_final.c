// Henrique Gaspar Monteiro 15459073
// Julia de Oliveira Giglio 15465167

// LCD ligado ao PORTD
// RS -> RD2
// EN -> RD3
// D4 -> RD4
// D5 -> RD5
// D6 -> RD6
// D7 -> RD7
//Devido a versão do simullide, precisamos trocar o Vref externo de 1V para Vref interno de 5V
//110°C = 1V

sbit LCD_RS at LATD2_bit;
sbit LCD_EN at LATD3_bit;
sbit LCD_D4 at LATD4_bit;
sbit LCD_D5 at LATD5_bit;
sbit LCD_D6 at LATD6_bit;
sbit LCD_D7 at LATD7_bit;

sbit LCD_RS_Direction at TRISD2_bit;
sbit LCD_EN_Direction at TRISD3_bit;
sbit LCD_D4_Direction at TRISD4_bit;
sbit LCD_D5_Direction at TRISD5_bit;
sbit LCD_D6_Direction at TRISD6_bit;
sbit LCD_D7_Direction at TRISD7_bit;



unsigned short flag_botao1 = 0;
unsigned short flag_botao2 = 0;
unsigned short longa = 60;
unsigned short curta = 40;
unsigned short sistema_ativo = 0;  // 0=parado, 1=longa rodando, 2=curta rodando

unsigned int temp_decimos = 0;     // temperatura em decimos de grau (ex: 253 = 25.3 C)

char bufNum[3];    // buffer para exibir numeros de 2 digitos no LCD
char bufTemp[10];  // buffer para exibir temperatura no LCD


void formataNumero(unsigned short valor, char *buf);
void formataTemperatura(unsigned int decimos, char *buf);
void atualizaDisplay();
unsigned int leTemperatura();


//interrupção
void interrupt() {

    // TMR0: estouro a cada 1s - longa
    if (INTCON.TMR0IF) {
        TMR0H = 0xF8;
        TMR0L = 0x5F;
        INTCON.TMR0IF = 0;
        if (sistema_ativo == 1 && longa > 0) {
            longa--;
            if (longa == 0) sistema_ativo = 0; // para ao zerar
        }
    }

    // TMR1: estouro a cada 250ms - curta 40 ticks = 10s
    if (PIR1.TMR1IF) {
        TMR1H = 0xC2;
        TMR1L = 0xF7;
        PIR1.TMR1IF = 0;
        if (sistema_ativo == 2 && curta > 0) {
            curta--;
            if (curta == 0) sistema_ativo = 0; // para ao zerar
        }
    }

    // Botao 1: inicia contagem longa (INT0)
    if (INTCON.INT0IF) {
        INTCON.INT0IF = 0;
        flag_botao1 = 1;
    }

    // Botao 2: inicia contagem curta (INT1)
    if (INTCON3.INT1IF) {
        INTCON3.INT1IF = 0;
        flag_botao2 = 1;
    }
}


void main() {
    TRISA = 0xFF;

    // Configura PORTD como saida (LCD)
    TRISD = 0x00;

    // Botao 1 e 2 como entrada
    TRISB.B0 = 1;
    TRISB.B1 = 1;

    // LED de alerta como saida
    TRISC.B0 = 0;
    LATC.B0  = 0;

    // Vref interno: VDD=5V como Vref+, VSS=GND como Vref- bug da versão do simullide
    // ADCON1 = 0x0E:
    //   VCFG1=0, VCFG0=0 -> Vref interno (VDD e VSS)
    //   PCFG=1110 -> AN0 analogico, demais digitais
    ADCON1 = 0x0E;
    ADCON2 = 0xBE;
    ADCON0 = 0x01;
    Delay_ms(10); // aguarda estabilizacao

    // Inicializa LCD
    Lcd_Init();
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);

    //interrupcoes externas
    INTCON2.INTEDG0 = 1;  // INT0: borda de subida
    INTCON2.INTEDG1 = 1;  // INT1: borda de subida

    INTCON.INT0IF  = 0;   // limpa flags antes de habilitar
    INTCON3.INT1IF = 0;

    INTCON.INT0IE  = 1;   // habilita INT0 (Botao 1)
    INTCON3.INT1IE = 1;   // habilita INT1 (Botao 2)

    // Configuracao TMR0 (1s, prescaler 1:256, 16 bits)
    T0CON = 0x07;
    TMR0H = 0xF8;
    TMR0L = 0x5F;
    INTCON.TMR0IF = 0;
    INTCON.TMR0IE = 1;
    T0CON.TMR0ON  = 1;

    // Configuracao TMR1 (250ms, prescaler 1:8, 16 bits)
    T1CON = 0x30;
    TMR1H = 0xC2;
    TMR1L = 0xF7;
    PIR1.TMR1IF  = 0;
    PIE1.TMR1IE  = 1;
    T1CON.TMR1ON = 1;

    INTCON.PEIE = 1;
    INTCON.GIE  = 1;

    // mensagem inicial
    Lcd_Out(1, 1, "Forno Industrial");
    Lcd_Out(2, 1, "B1=Longa B2=Curt");
    Delay_ms(1500);
    Lcd_Cmd(_LCD_CLEAR);

    while (1) {

        // Botao 1: inicia contagem longa
        if (flag_botao1) {
            flag_botao1 = 0;
            INTCON.GIE = 0;
            Delay_ms(50);
            if (PORTB.B0 == 1) {
                longa = 60;
                sistema_ativo = 1;
            }
            INTCON.INT0IF = 0;
            INTCON.GIE    = 1;
        }

        // Botao 2: inicia contagem curta
        if (flag_botao2) {
            flag_botao2 = 0;
            INTCON.GIE = 0;
            Delay_ms(50);
            if (PORTB.B1 == 1) {
                curta = 40;
                sistema_ativo = 2;
            }
            INTCON3.INT1IF = 0;
            INTCON.GIE     = 1;
        }

        // le e converte temperatura
        temp_decimos = leTemperatura();

        // LED acende se temperatura > 50 C
        if (temp_decimos > 500) {
            LATC.B0 = 1;
        } else {
            LATC.B0 = 0;
        }

        atualizaDisplay();
        Delay_ms(100);
    }
}

unsigned int leTemperatura() {
    unsigned int raw;
    unsigned int resultado;

    ADCON0 = 0x01;         // canal AN0, ADC ligado
    Delay_ms(5);           // tempo de aquisicao generoso para SimulIDE
    ADCON0.GO_DONE = 1;    // inicia conversao
    while (ADCON0.GO_DONE); // aguarda fim (bit zera automaticamente)

    raw = ((unsigned int)(ADRESH & 0x03) << 8) | ADRESL;  // 10 bits

    // Com Vref=5V: raw=204 corresponde a 1V = 100.0 C
    resultado = (unsigned int)((unsigned long)raw * 1000UL / 204UL);
    if (resultado > 1000) resultado = 1000;  // limita a 100.0 C
    return resultado;
}


// formata valor de 2 digitos em string
void formataNumero(unsigned short valor, char *buf) {
    buf[0] = (valor / 10) + '0';
    buf[1] = (valor % 10) + '0';
    buf[2] = '\0';
}


// Formata temperatura em decimos para string
void formataTemperatura(unsigned int decimos, char *buf) {
    unsigned char centenas, dezenas, unidade;
    centenas = decimos / 100;
    dezenas  = (decimos % 100) / 10;
    unidade  = decimos % 10;

    if (centenas > 0) {
        buf[0] = '0' + centenas;
        buf[1] = '0' + dezenas;
        buf[2] = '.';
        buf[3] = '0' + unidade;
        buf[4] = ' ';
        buf[5] = 'C';
        buf[6] = '\0';
    } else {
        buf[0] = '0' + dezenas;
        buf[1] = '.';
        buf[2] = '0' + unidade;
        buf[3] = ' ';
        buf[4] = 'C';
        buf[5] = ' ';
        buf[6] = '\0';
    }
}


// Atualiza o display LCD com temperatura (linha 1) e tempo/status (linha 2)
void atualizaDisplay() {
    formataTemperatura(temp_decimos, bufTemp);
    Lcd_Out(1, 1, "Temp: ");
    Lcd_Out(1, 7, bufTemp);

    if (sistema_ativo == 1) {
        formataNumero(longa, bufNum);
        Lcd_Out(2, 1, "Longa: ");
        Lcd_Out(2, 8, bufNum);
        Lcd_Out(2, 10, "s        ");
    } else if (sistema_ativo == 2) {
        formataNumero(curta / 4, bufNum);  // converte ticks para segundos
        Lcd_Out(2, 1, "Curta: ");
        Lcd_Out(2, 8, bufNum);
        Lcd_Out(2, 10, "s        ");
    } else {
        Lcd_Out(2, 1, "B1=Longa B2=Curt");
    }
}