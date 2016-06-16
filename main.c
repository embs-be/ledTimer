/*
 * File:   main.c
 * Author: slava
 *
 * Created on May 8, 2016, 7:17 PM
 */
#define _XTAL_FREQ 31000

// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Detect (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)

#include <xc.h>

typedef void (*Handler)(void);

void waitForKeyPress(void);
void waitForKeyRelease(void);

Handler handler = waitForKeyPress;
unsigned char keyPressed;

void ledOff(void) {
    SWDTEN = 0; // Disable WDT
    GPIO = 0x30; //LED off

    if (keyPressed) {
        waitForKeyRelease();
    }

    handler = waitForKeyPress;
}

unsigned char ledCount;
#define LED_CYCLES 7

void ledTimer(void) {
    if ((keyPressed) || (++ledCount > LED_CYCLES)) {
        ledOff();
    }
}

void keyReleased(void) {

    handler = ledTimer;

    // set WDT to wakeup from sleep
    WDTPS0 = 1;
    WDTPS1 = 1;
    WDTPS2 = 0;
    WDTPS3 = 1;
    SWDTEN = 1;

    ledCount = 0;
    keyPressed = 0;
}

void waitForKeyRelease(void) {
    unsigned char keyBuf;

    keyBuf = 0xff;

    while (keyBuf) {
        keyBuf <<= 1;
        if (GP2 == 0) keyBuf++;
        __delay_ms(10);
    }

}

void waitForKeyPress(void) {
    GPIO = 0; //LED on
    waitForKeyRelease();
    keyReleased();
}

void interrupt tc_int(void) { // interrupt function
    if (INTCONbits.INTF) { // if timer flag is set & interrupt enabled
        INTCONbits.INTF = 0; // clear the interrupt flag
        keyPressed = 1;
    }

}

void main(void) {
    CMCON0 = 7;
    /*
    ANS<3:0>: Analog Select bits
    Analog select between analog or digital function on pins AN<3:0>, respectively.
    1 = Analog input. Pin is assigned as analog input(1).
    0 = Digital I/O. Pin is assigned to port or special function.
     */
    ANSELbits.ANS = 0;
    /*
     ADON: ADC Enable bit
    1 = ADC is enabled
    0 = ADC is disabled and consumes no operating current
     */
    ADCON0bits.ADON = 0;

    OSCCONbits.IRCF0 = 0; //set OSCCON IRCF bits to select OSC frequency=31kHz
    OSCCONbits.IRCF1 = 0;
    OSCCONbits.IRCF2 = 0;

    GPIO = 0x30;
    TRISIO = 0; //all pins as Output PIN

    // input with pull up
    OPTION_REGbits.nGPPU = 0;
    TRISIO2 = 1;
    TRISIO3 = 1;
    WPU2 = 1;

    OPTION_REGbits.INTEDG = 0; // falling edge trigger the interrupt
    INTCONbits.INTF = 0; // clear the interrupt flag
    INTCONbits.INTE = 1; // enable the external interrupt
    INTCONbits.GIE = 1; // Global interrupt enable

    SWDTEN = 0; // Disable WDT

    while (1) {
        SLEEP();
        handler();
    }

    return;
}
