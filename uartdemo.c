/*
 * Weather Engineering Team
 * A program to simulate UART
 * between PIC on supernode and PI
 * 
 */


// FBS
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

// http://www.microchip.com/forums/m273860.aspx

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <p24F16KL401.h>
#include <string.h>
#include <uart.h>
#include <stdio.h>
//#include <delay.h>

#define _XTAL_FREQ  8000000

int flag = 0;


void main(void)
{

    U2MODEbits.UARTEN = 1;//enable UART
    U2MODEbits.USIDL = 0; // continue operation in idle mode
    U2MODEbits.STSEL = 0; // 1 stop bit
    U2MODEbits.PDSEL0 = 0;
    U2MODEbits.PDSEL1 = 0;// 8, N, 
    U2MODEbits.BRGH = 0;
    //U2MODEbits.BRGH = 1;

    U2MODEbits.RXINV = 0;// rx idle state = 1
    U2MODEbits.ABAUD = 0;// no autobaud
    U2MODEbits.LPBACK = 0; // loopback mode disabled
    U2MODEbits.WAKE = 0; // wakeup disable
    U2MODEbits.UEN0 = 0;
    U2MODEbits.UEN1 = 0;
    U2MODEbits.RTSMD = 1;// rts mode 
    U2MODEbits.IREN = 0;// irda disabled
    U2STAbits.ADDEN = 0; // address mode disabled
      
    U2STAbits.URXISEL0 = 1;
    U2STAbits.URXISEL1 = 1; // interrupt any data received
    
    U2STAbits.UTXEN = 1; // transmit enabled
    U2STAbits.UTXBRK = 0; // break disabled
    
    U2STAbits.UTXISEL0 = 1;
    U2STAbits.UTXISEL1 = 0; // interrupt when txregister becomes empty
     
    U2BRG = 51 ; // 3.340277778

    //commented out for transmitter card     
    // receive interrupt config
    //IFS1bits.U2RXIF = 0;
    //IEC1bits.U2RXIE = 0;//; // rx interrupt enabled
    
    
    //clear interrupt flags
    IFS0bits.INT0IF = 0;
    IFS1bits.INT2IF = 0;
    
    //interrupt occurs on posedge
    INTCON2bits.INT2EP = 0;
    INTCON2bits.INT0EP = 0;
    
    //enalbes interrupts, page 94 for setup
    IEC1bits.INT2IE = 1;
    IEC0bits.INT0IE = 1;


    //TRISB = 0b1111111111111111;
    //TRISBbits.TRISB9 = 0;
    int count = 0;
    uint32_t c = 0;
    uint8_t msg[17] = {'2',' ','6','5','2','D',' ','4','0','B','7',' ','0','2','9','E','U'};
    uint8_t m[24] = {'E','N','D',' ','7','F','F','F',' ','7','F','F','F',' ','7','F','F','F',' ','7','F','F','F','U'};
    uint8_t f=0;
   
    while(flag == 0);
    
    while (1)
    {
        
        while(msg[count] != '\0')
        {
        
            if (U2STAbits.UTXBF == 0)
                {    
                WriteUART2(msg[count]);
                count += 1;
                while(c++<1000);
                }
            //need to waste time because buffer doesn't
            
            c = 0;   
        }
        count = 0;
        
        if( msg[0] != '5') 
        {
            msg[0] += 1; // increment to next node
            f=1;
        }
        if(f==1)
        {
            f=0;
            msg[0] = '2';
            while(m[count] != 0x00)
            {
        
                if (U2STAbits.UTXBF == 0)
                    {    
                    WriteUART2(m[count]);
                    count += 1;
                    while(c++<1000);
                    }
                
            //need to waste time because buffer doesn't
                
                c = 0;
            }
            count = 0;
            while(c++<0x3FFFFF);
            c = 0;
        }
    
        
         

    }

}

void __attribute__((__interrupt__,no_auto_psv)) _INT0Interrupt(void)
{
    if(IFS0bits.INT0IF == 1)
    {
        flag = 1;
        //LATAbits.LATA2 = 0;//~(LATBbits.LATB8);
        IFS0bits.INT0IF = 0;
    }
    /*if(IFS1bits.INT2IF == 1)
    {
        LATAbits.LATA2 = 0;//~(LATBbits.LATB8);
        IFS1bits.INT2IF = 0;
    }*/
}

void __attribute__((__interrupt__,no_auto_psv)) _INT2Interrupt(void)
{
    if(IFS1bits.INT2IF == 1)
    {
        flag = 1;

        //LATAbits.LATA2 = 0;//~(LATBbits.LATB8);
        IFS1bits.INT2IF = 0;
    }
}
