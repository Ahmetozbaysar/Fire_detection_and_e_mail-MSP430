#include <msp430.h>

#define ENABLE_CONFIG_WIFI  1

void Wifi(void);
void WiFiSendTCP();
void putc(const unsigned c);
void TX(const char *s);
void crlf(void);
void uart_putch(unsigned char c);
char uart_getch(void);
char alinan;

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO+++++
    DCOCTL = CALDCO_1MHZ;
    if (CALBC1_1MHZ == 0xFF)                // If calibration constant erased
        {
            while(1);                           // do not load, trap CPU!!
        }

       #if defined(ENABLE_CONFIG_WIFI)
        Wifi();             // Configure the ESP8266 WiFi settings
       #endif



    P1DIR |= BIT4  ;
    P1DIR &=~ BIT5;
    P1OUT &=~ BIT4;
    //P1REN= 0x00;
    P1REN |= BIT5;                   // Enable internal pull-up/down resistors
    P1OUT |= BIT5;
    P1IE |= BIT5;                    
    P1IES |= BIT5;                  
    P1IFG &= ~BIT5;

    P2DIR |= BIT2 + BIT4;
    P2OUT |= BIT4 ;
    P2OUT &=~ BIT2 ;

    P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD

    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 104;                            // 1MHz 9600
    UCA0BR1 = 0;                              // 1MHz 9600
    UCA0MCTL = UCBRS0;                        
    UCA0CTL1 &= ~UCSWRST;                     
    IE2 |= UCA0RXIE;                          
    //WiFiSendTCP();

    __bis_SR_register(LPM0_bits + GIE);
    while(1);

}

#pragma vector=PORT1_VECTOR
__interrupt void P1_ISR(void)
{
    Wifi();
    P2OUT |= BIT2;
    WiFiSendTCP();
    P2OUT &= ~ BIT4 ;
    P1OUT |= BIT4;
    __delay_cycles(2000000);
    P2OUT &= ~ BIT2;
    P2OUT |= BIT4 ;
    P1OUT &= ~ BIT4;

    P1IFG = 0x00 ;
}



void Wifi(void)
{
    TX("AT+RST\r\n");
    __delay_cycles(2000000);

    TX("AT+CWMODE_DEF=1\r\n");
    __delay_cycles(3000000);
    TX("AT+CWJAP_DEF=\"iPhone\",\"ahmet4455\"\r\n");
    __delay_cycles(6000000);
    while (!(IFG2&UCA0TXIFG)); UCA0TXBUF = '"';                 
            TX("AT+CWMODE_DEF=1");
            crlf();
    while (!(IFG2&UCA0TXIFG)); UCA0TXBUF = '"';                 
            TX("AT+CWJAP_DEF=\"iPhone\",\"ahmet4455\"");*/
            crlf();
    TX("AT+CIPMUX=1\r\n");
    __delay_cycles(3000000);
    TX("AT+CIPSERVER=1,80\r\n");
    __delay_cycles(3000000);
    while (!(IFG2&UCA0TXIFG)); UCA0TXBUF = '"';                  
            TX("AT+CIPMUX=1");
            crlf();
            __delay_cycles(3000000);
            while (!(IFG2&UCA0TXIFG)); UCA0TXBUF = '"';
            __delay_cycles(3000000);
            TX("AT+CIPSERVER=1,80");
            crlf();
}

void WiFiSendTCP()
{
    
    TX("AT+CIPSTART=");
        //while (!(IFG2&UCA0TXIFG)); UCA0TXBUF = '"';                  
        TX("0");
        //while (!(IFG2&UCA0TXIFG)); UCA0TXBUF = '"';
        TX(",");
        while (!(IFG2&UCA0TXIFG)); UCA0TXBUF = '"';                  
        TX("TCP");
        while (!(IFG2&UCA0TXIFG)); UCA0TXBUF = '"';
        TX(",");
        while (!(IFG2&UCA0TXIFG)); UCA0TXBUF = '"';
        TX("52.85.7.98");
        while (!(IFG2&UCA0TXIFG)); UCA0TXBUF = '"';
        TX(",");
        TX("80");
        crlf();
        __delay_cycles(3000000);
        TX("AT+CIPSEND=0,84");
        crlf();
        __delay_cycles(3000000);
        TX("GET /trigger/fire/with/key/cImsJ1WC9mY_ByFimSONs3 HTTP/1.1");
        crlf();
        TX("Host:maker.ifttt.com\r\n");
        crlf();
        __delay_cycles(3000000);
        //TX("AT+RST\r\n");
        //crlf();

}

void putc(const unsigned c)
{
    while (!(IFG2&UCA0TXIFG)); // USCI_A0 TX buffer ready?
    UCA0TXBUF = c;
}

void TX(const char *s)
{
    while(*s) putc(*s++);
}
void crlf(void)
{
    TX("\r\n");
}
