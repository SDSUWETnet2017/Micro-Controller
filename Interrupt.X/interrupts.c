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

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <p24F16KL401.h>

void main(void)
{
    INTCON1bits.NSTDIS = 1; //disable nested interrupts
    
    //clear interrupt flags
    IFS0bits.INT0IF = 0;
    IFS1bits.INT2IF = 0;
    
    //interrupt occurs on posedge
    INTCON2bits.INT2EP = 0;
    INTCON2bits.INT0EP = 0;
    
    //enalbes interrupts, page 94 for setup
    IEC1bits.INT2IE = 1;
    IEC0bits.INT0IE = 1;
    
    TRISA = 0xFFFF;
    TRISAbits.TRISA2 = 0;
    LATAbits.LATA2 = 1;
    
    
    while(1)
    {
        
    }
  
}







//void interrupt inter(void)
//{
//    if(IFS0bits.INT0IF || IFS1bits.INT2IF)
//    {
//        LATBbits.LATB8 = ~(LATBbits.LATB8);
///    }
//}

void __attribute__((__interrupt__,no_auto_psv)) _INT0Interrupt(void)
{
    if(IFS0bits.INT0IF == 1)
    {
        LATAbits.LATA2 = 0;//~(LATBbits.LATB8);
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
        LATAbits.LATA2 = 0;//~(LATBbits.LATB8);
        IFS1bits.INT2IF = 0;
    }
}