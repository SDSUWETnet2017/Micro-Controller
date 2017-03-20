
#include <p24F16KL401.h>
//#include "I2C1.h"
//#include <i2c.h>

#define DELAY 10000

#define _XTAL_FREQ 8000000

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
#pragma config POSCMD = XT              // Primary Oscillator Mode (XT oscillator mode selected)   1
#pragma config OSCIOFNC = ON            // CLKO Pin I/O Function (Port I/O enabled (CLKO disabled)) 
#pragma config POSCFREQ = HS            // Primary Oscillator Frequency Range (Primary Oscillator/External Clock frequency >8MHz)
#pragma config SOSCSEL = SOSCHP         // SOSC Power Selection Configuration bits (Secondary Oscillator configured for high-power operation)
#pragma config FCKSM = CSECME           // Clock Switching and Monitor Selection (Clock Switching and Fail-safe Clock Monitor Enabled)

// FWDT
#pragma config WDTPS = PS32768          // Watchdog Timer Postscale Select bits (1:32768)
#pragma config FWPSA = PR128            // WDT Prescaler bit (WDT prescaler ratio of 1:128)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bits (WDT disabled in hardware; SWDTEN bit disabled)  1
#pragma config WINDIS = ON              // Windowed Watchdog Timer Disable bit (Windowed WDT enabled)

// FPOR
#pragma config BOREN = BOR0             // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware; SBOREN bit disabled)  1
#pragma config PWRTEN = OFF             // Power-up Timer Enable (PWRT disabled)   1 
#pragma config I2C1SEL = PRI            // Alternate I2C1 Pin Mapping bit (Default SCL1/SDA1 Pins for I2C1)
#pragma config BORV = V18               // Brown-out Reset Voltage bits (Brown-out Reset at 1.8V)
#pragma config MCLRE = ON               // MCLR Pin Enable bit (RA5 input disabled; MCLR enabled)

// FICD
#pragma config ICS = PGx1       



void I2C_Master_Init(const unsigned long c)
{

  SSP1CON1 =0b0000000000101000; //SSPM mater node, no collision, no overflow,  Enables the serial port and configures the SDAx and SCLx pins as the serial port pins  
  SSP1CON2 =0b0000000000000000; 
  SSP1ADD = (_XTAL_FREQ/(4*c))-1; //Setting Clock Speed
  
  SSP1STAT =0b0000000000000000;
 
  
  TRISBbits.TRISB8 = 1;      //SCL. Set as input.
 
  TRISBbits.TRISB9 = 1;      //SDA. Set as input.
 
}


void I2C_Master_Wait()
{
  while ((SSP1STAT & 0x04) || (SSP1CON2 & 0x1F)); //Transmit is in progress
}


void I2C_Master_Start()
{
  I2C_Master_Wait();    
  SSP1CON2bits.SEN = 1;             //Initiate start condition
  
}


 void I2C_Master_RepeatedStart()
{
  I2C_Master_Wait();
  SSP1CON2bits.RSEN = 1;           //Initiate repeated start condition
}

void I2C_Master_Stop()
{
  I2C_Master_Wait();
  SSP1CON2bits.PEN = 1;           //Initiate stop condition
}

void I2C_Master_Write(unsigned d)
{
  I2C_Master_Wait();
  SSP1BUF = d;         //Write data to SSPBUF
}

unsigned short I2C_Master_Read(unsigned short a)
{
  unsigned char temp;
  
  I2C_Master_Wait();
  SSP1CON2bits.RCEN = 1; //enable receive mode for i2c
  I2C_Master_Wait();
  
  
  temp = SSP1BUF;      //Read data from SSPBUF
  I2C_Master_Wait();
  
  SSP1CON2bits.ACKDT = (a)?0:1;    //Acknowledge bit
  SSP1CON2bits.ACKEN = 1;          //Acknowledge sequence
  return temp;
}






void main()
{   
    
  unsigned char i;
  
  I2C_Master_Init(100000);      //Initialize I2C Master with 100KHz clock
  
  TRISAbits.TRISA4 = 0;                 //PORTD as output
  
  T1CON=0x8030;
  
    while(1)
    {
   /* I2C_Master_Start();         //Start condition   //I2C write
    I2C_Master_Wait();
    * 
    I2C_Master_Write(0x60);     //slave id + 0
    I2C_Master_Wait();
    * 
    I2C_Master_Write(0x00);    //register address
    I2C_Master_Wait();
    * 
     I2C_Master_Write(0xff);    //data 
     I2C_Master_Wait(); 
     
    I2C_Master_Stop();          //Stop condition
   */
    I2C_Master_Start();  //I2C read
    I2C_Master_Wait();
    
    I2C_Master_Write(0xD0);     //7 bit address + write
    I2C_Master_Wait();
    
    I2C_Master_Write(0x00);     // internal address
    I2C_Master_Wait();
    
    I2C_Master_RepeatedStart()     //repeated start
    I2C_Master_Wait();
    
    I2C_Master_Write(0xD1);     // 7 bit address + read
    I2C_Master_Wait();
    
    
    i = I2C_Master_Read(0); //
    
    I2C_Master_Wait();
    I2C_Master_Stop();
    
    
    
    while (TMR1<DELAY)
    {
        
    }
        
     
    }
  
}



