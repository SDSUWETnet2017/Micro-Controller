  #include <p24F16KL401.h>
  #include <xc.h>
  #include <stdint.h>
  #include <stdio.h>
  #include <uart.h>
  #include <string.h>
  #include <stdbool.h>

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

//****************************************************************************
uint16_t ANGLE;  
unsigned short i;
unsigned short j;
uint16_t AVERAGE_ANGLE;
int array_size = 100;

int counter = 0; //Counts to 100 for while-loop (Average_wind function)
int adder = 0;
int send_flag = 0; //Flag is high in Timer2 and sends output
char string[8];
//****************************************************************************


    
    void I2C_Master_Init(const unsigned long c)
    {

      SSP1CON1 =0b0000000000101000; //SSPM mater node, no collision, no overflow,  Enables the serial port and configures the SDAx and SCLx pins as the serial port pins  
      SSP1CON2 =0b0000000000000000; 
      SSP1ADD = (_XTAL_FREQ/(4*c))-1; //Setting Clock Speed

      SSP1STAT =0b0000000000000000;


      TRISBbits.TRISB8 = 1;      //SCL. Set as input.

      TRISBbits.TRISB9 = 1;      //SDA. Set as input.

      TRISBbits.TRISB7 = 0; //UART1 transmit the value (RB0)
      
      /////////////////////////////timer
    T2CONbits.TMR2ON = 1; //Turn on Timer 2
    T2CONbits.T2CKPS0 = 1; //Set prescaler to 16
    T2CONbits.T2CKPS1 = 1;
    PR2 = 50000;   //(2/Fosc)*post*pre_(PR2+1) = Time   (2/8M)*16*(50000+1) = 0.2 sec
    
    INTCON1bits.NSTDIS = 1; //Disable nested interrupts
    INTCON2bits.INT0EP = 0; //interrupt 0 on posedge
    IFS0bits.T2IF = 0; //Clear Timer2 interrupt flag
    IEC0bits.T2IE = 1; //Enable Timer2 interrupt

    
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
    
    ////******************///****************////
 
    }

    //************************************* single function to form complex one
    void I2C_Master_Wait()
    {
      while ((SSP1STAT & 0x04) || (SSP1CON2 & 0x1F)); //Transmit is in progress
    }


    void I2C_Master_Start()
    {
       I2C_Master_Wait();
      SSP1CON2bits.SEN = 1;             //Initiate start condition
        I2C_Master_Wait();
    }


     void I2C_Master_RepeatedStart()
    {
      I2C_Master_Wait();
      SSP1CON2bits.RSEN = 1;           //Initiate repeated start condition
      I2C_Master_Wait();
    }

    void I2C_Master_Stop()
    {
      I2C_Master_Wait();
      SSP1CON2bits.PEN = 1;           //Initiate stop condition
      I2C_Master_Wait();
    }

    void I2C_Master_Write(unsigned d) //single write singe
    {
      I2C_Master_Wait();
      SSP1BUF = d;         //Write data to SSPBUF

    }
    //**************************************************

   uint16_t send_uart(uint16_t ang) //UART
    {
    
   int count = 0;
   
    while (count < 6)
        {    
        float t= (float) ang;
        float T= t/100;
            sprintf(string, "\r\n%f", T);
            WriteUART1(string[count]);
            count++;
        } 
       count = 0;
        
    }    


    //********************************************used for write and read

    void Write8(unsigned add,unsigned reg,unsigned val) //slave address, register, val (three write)) + start +stop condition
    {
          I2C_Master_Start();

         I2C_Master_Write(add);     //slave id + 0

        I2C_Master_Write(reg);    //register address

         I2C_Master_Write(val);    //data 

         I2C_Master_Stop();
    }

    void writeParam(unsigned  p, unsigned  v) //write parameter
    {
        Write8(0xc0,0x17, v);
        Write8(0xc0,0x18, p | 0xA0);

    }

    /*void Read(unsigned q,unsigned w,unsigned e) //don`t use 
    {

        I2C_Master_Write(q); //7 bits + write
        I2C_Master_Write(w);
        I2C_Master_RepeatedStart();
        I2C_Master_Write(e); // 7 bits +read (1)

    }*/



    unsigned short ReadValue(unsigned q,unsigned w, unsigned e, unsigned short a)
    {
      unsigned short temp;

        I2C_Master_Write(q); //7 bits + write
        I2C_Master_Write(w);
        I2C_Master_RepeatedStart();
        I2C_Master_Write(e); // 7 bits +read (1)

      I2C_Master_Wait();
      SSP1CON2bits.RCEN = 1; //enable receive mode for i2c
      I2C_Master_Wait();


      temp = SSP1BUF;      //Read data from SSPBUF
      I2C_Master_Wait();

      SSP1CON2bits.ACKDT = (a)?0:1;    //Acknowledge bit
      SSP1CON2bits.ACKEN = 1;          //Acknowledge sequence
      return temp;
    }


    void reset() {
      Write8(0xc0,0x08, 0);
      Write8(0xc0,0x09, 0);
      Write8(0xc0,0x04, 0);
      Write8(0xc0,0x05, 0);
      Write8(0xc0,0x06, 0);
      Write8(0xc0,0x03, 0);
      Write8(0xc0,0x21, 0xFF);

      Write8(0xc0,0x18, 0x01);
      I2C_Master_Wait();
      Write8(0xc0,0x07, 0x17);

      I2C_Master_Wait();
    }

    void initializationUV()
    {
        //enable UVindex measurement coefficients
      Write8(0xc0,0x13, 0x29);
      Write8(0xc0,0x14, 0x89);
      Write8(0xc0,0x15, 0x02);
      Write8(0xc0,0x16, 0x00);

     // enable UV sensor
      writeParam(0x01, 0xb1);

      /****************************** Prox Sense 1 */

      // program LED current
      Write8(0xc0,0x0F, 0x03); // 20mA for LED 1 only
      writeParam(0x07, 0x03);
      // prox sensor #1 uses LED #1
      writeParam(0x02, 0x01);
      // fastest clocks, clock div 1
      writeParam(0x0B, 0);
      // take 511 clocks to measure
      writeParam(0x0A, 0x70);
      // in prox mode, high range
      writeParam(0x0C, 0x20|0x04);

      writeParam(0x0E, 0x00);  
      // fastest clocks, clock div 1
      writeParam(0x1E, 0);
      // take 511 clocks to measure
      writeParam(0x1D, 0x70);
      // in high range mode
      writeParam(0x1F, 0x20);



      // fastest clocks, clock div 1
      writeParam(0x11, 0);
      // take 511 clocks to measure
      writeParam(0x10, 0x70);
      // in high range mode (not normal signal)
      writeParam(0x12, 0x20);


      // measurement rate for auto
      Write8(0xc0,0x08, 0xFF); // 255 * 31.25uS = 8ms

      // auto run
      Write8(0xc0,0x18, 0x0F);

    }
    
    void main()
    {   

      unsigned short i; //store lower byte

     unsigned short j; //store higher byte 
     
     unsigned short value;
     
     I2C_Master_Init(100000);      //Initialize I2C Master with 100KHz clock

     reset(); //reset SI1145, uv light
 
     initializationUV(); //initialization for UV sensor
     
     
      TRISAbits.TRISA4 = 0;    //no use
 
      T1CON=0x8030; //timer

      

        while(1)
        {

         I2C_Master_Start();

        j=ReadValue(0xc0,0x2d,0xc1,0); //change to 2d

        I2C_Master_Stop();
      
        I2C_Master_Start();

        i=ReadValue(0xc0,0x2C,0xc1,0);

        I2C_Master_Stop(); 
        
        value =j*250+i;
      
        if(send_flag == 1)
   {
        ANGLE = value;
        
        send_uart(ANGLE);
   }    
    
           
     
        
       

        //  Write8(0xc0,0x00,0b01100110);

        while (TMR1<DELAY) //delay function
        {


        }


        }

    }

    void __attribute__((__interrupt__,no_auto_psv)) _T2Interrupt(void)
{
    
    
    if(counter>=25000) //Delay transmitting output if PR2 is not long enough //30000
        {
            send_flag = 1; //Set flag high to output data
            
            IFS0bits.T2IF = 0; //Empty flag to exit Timer2
            
            counter = 0;
        }
    else
        counter++;
   
}
