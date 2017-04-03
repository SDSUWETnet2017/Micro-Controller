/* 
 * File:   Anemometer.c
 * Author: e217
 *
 * Created on March 19, 2017, 8:11 PM
 */

#pragma config BWRP = OFF               // Boot Segment Write Protect (Disabled)
#pragma config BSS = OFF                // Boot segment Protect (No boot flash segment)

// FGS
#pragma config GWRP = OFF               // General Segment Flash Write Protect (General segment may be written)
#pragma config GSS0 = OFF               // General Segment Code Protect (No Protection)

// FOSCSEL
#pragma config FNOSC = FRCPLL           // Oscillator Select (Fast RC Oscillator with Postscaler and PLL (FRCDIV+PLL))
#pragma config SOSCSRC = ANA            // SOSC Source Type (Analog Mode for use with crystal)
#pragma config LPRCSEL = HP             // LPRC Power and Accuracy (High Power/High Accuracy)
#pragma config IESO = ON                // Internal External Switch Over bit (Internal External Switchover mode enabled (Two-speed Start-up enabled))

// FOSC
#pragma config POSCMD = NONE            // Primary Oscillator Mode (Primary oscillator disabled)
#pragma config OSCIOFNC = ON            // CLKO Pin I/O Function (Port I/O enabled (CLKO disabled))
#pragma config POSCFREQ = HS            // Primary Oscillator Frequency Range (Primary Oscillator/External Clock frequency >8MHz)
#pragma config SOSCSEL = SOSCHP         // SOSC Power Selection Configuration bits (Secondary Oscillator configured for high-power operation)
#pragma config FCKSM = CSECME           // Clock Switching and Monitor Selection (Clock Switching and Fail-safe Clock Monitor Enabled)

// FWDT
#pragma config WDTPS = PS32768          // Watchdog Timer Postscale Select bits (1:32768)
#pragma config FWPSA = PR128            // WDT Prescaler bit (WDT prescaler ratio of 1:128)
#pragma config FWDTEN = SWON            // Watchdog Timer Enable bits (WDT controlled with SWDTEN bit setting)
#pragma config WINDIS = OFF             // Windowed Watchdog Timer Disable bit (Standard WDT selected (windowed WDT disabled))

// FPOR
#pragma config BOREN = BOR3             // Brown-out Reset Enable bits (Enabled in hardware; SBOREN bit disabled)
#pragma config PWRTEN = ON              // Power-up Timer Enable (PWRT enabled)
#pragma config I2C1SEL = PRI            // Alternate I2C1 Pin Mapping bit (Default SCL1/SDA1 Pins for I2C1)
#pragma config BORV = V18               // Brown-out Reset Voltage bits (Brown-out Reset at 1.8V)
#pragma config MCLRE = ON               // MCLR Pin Enable bit (RA5 input disabled; MCLR enabled)

// FICD
#pragma config ICS = PGx1               // ICD Pin Placement Select (EMUC/EMUD share PGC1/PGD1)
//#pragma config LVP = OFF

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <p24F16KL401.h>
#include "uart.h"

#define PI 3.14159
#define ANEMOMETER_CIRC 0.354331*PI //inches //get new anemometer circ
#define _XTAL_FREQ 8000000

float MPH_Calculate(float period);

volatile float old_pulse = 0;
volatile int overflow = 0;
volatile int calculate = 0;
volatile float period = 0;

void main(void)
{
    TRISBbits.TRISB9 = 1;
    TRISBbits.TRISB8 = 0;
    LATBbits.LATB8 = 0;
    TRISBbits.TRISB7 = 0;
  
    // overflow every 1s // might change
    T3CONbits.TMR3CS = 0b01; //fosc/2
    T3CONbits.T3CKPS = 0b11; //Set prescaler to 8 ~4s
    T3CONbits.TMR3ON = 1; //Turn on Timer 3
    TMR3 = 0;
    
    CCP3CON = 0b0000000000000110; //capture every 4th edge
    CCPTMRS0bits.C3TSEL0 = 1;
    
    INTCON1bits.NSTDIS = 1; //Disable nested interrupts
    IPC6bits.CCP3IP = 0b111; //highest priority will change
    IFS1bits.CCP3IF = 0; //clear flag
    IEC1bits.CCP3IE = 1; //enable interrupt
    IFS0bits.T3IF = 0;
   // IEC0bits.T3IE = 1;
   // IFS0bits.U1TXIF = 0;
    //IEC0bits.U1TXIE = 1;
    
    U1MODEbits.UARTEN = 1;//enable UART
    U1MODEbits.USIDL = 0; // continue operation in idle mode
    U1MODEbits.STSEL = 0; // 1 stop bit
    U1MODEbits.PDSEL0 = 0;
    U1MODEbits.PDSEL1 = 0;// 8, N, 
    U1MODEbits.BRGH = 0;
    //U1MODEbits.BRGH = 1;

    U1MODEbits.RXINV = 0;// rx idle state = 1
    U1MODEbits.ABAUD = 0;// no autobaud
    U1MODEbits.LPBACK = 0; // loopback mode disabled
    U1MODEbits.WAKE = 0; // wakeup disable
    U1MODEbits.UEN0 = 0;
    U1MODEbits.UEN1 = 0;
    U1MODEbits.RTSMD = 1;// rts mode 
    U1MODEbits.IREN = 0;// irda disabled
    U1STAbits.ADDEN = 0; // address mode disabled
      
    U1STAbits.URXISEL0 = 1;
    U1STAbits.URXISEL1 = 1; // interrupt any data received
    
    U1STAbits.UTXEN = 1; // transmit enabled
    U1STAbits.UTXBRK = 0; // break disabled
    
   // U1STAbits.UTXISEL0 = 1;
   // U1STAbits.UTXISEL1 = 0; // interrupt when txregister becomes empty
     
    U1BRG = 51 ; // 3.340277778
    unsigned int MPH_COUNT = 0;
    unsigned int MPH = 0;
    unsigned int MPH_AVG = 0;
    unsigned int GUST = 0;
    unsigned int GUST_COUNT = 0;
    int count = 0;
    char string[6];
    while(1)
    {
       if(IFS0bits.T3IF == 1)
        {
            overflow++;
            IFS0bits.T3IF = 0;
        }
       if(calculate = 1)
       {
            MPH = MPH_Calculate(period);
            if(MPH > 18 )
            {
                GUST += MPH;
                GUST_COUNT++;
            }
            else
            {
                MPH_AVG += MPH;
                MPH_COUNT++; //add code before sending UART to take average MPH_AVG /= MPH_COUNT;
            }
            calculate = 0;
       }
        if(MPH_COUNT > 0)
        {
       while (count < 6)
        {    
           MPH_AVG = MPH_AVG/MPH_COUNT;
            sprintf(string, "%d", MPH_AVG);
            WriteUART1(string[count]);
            count++;
        } 
       count = 0;
        }
    }
}
float MPH_Calculate(float period)
{
    float frequency = 0;
    float RPM = 0;
    frequency = 1/period;
        //might be able to do calculations outside interrupt if latency is issue
      //  MPH = (3600 * frequency * ANEMOMETER_CIRC)/(5280*12); //conversion
    RPM = 60 * frequency;
    RPM = RPM/12;
    RPM = RPM * 60;
    RPM = RPM * 1.11;
    return RPM/5280; //conversion
}
void __attribute__((__interrupt__,no_auto_psv)) _CCP3Interrupt(void)
{
    unsigned int current_pulse = 0;
    float time;
    if (IFS1bits.CCP3IF == 1)
    {
        current_pulse = (CCPR3H << 8)|CCPR3L;
        time = 0.000002 * current_pulse;
        if(overflow > 0)
        {
            time = 0.131072*overflow - time; //check for rollover
        }
       
        period = (float)(time - old_pulse) / 4; //divide by 4

        old_pulse = time;
        calculate = 1;
        IFS1bits.CCP3IF = 0;
    }   
}
