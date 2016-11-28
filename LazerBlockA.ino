
// _______                         _________________                         _________________
//        |_______________________|                 |_______________________|                 |_______
//            _________________                         _________________                         ____
// __________|                 |_______________________|                 |_______________________|            
//        x--x  
//           y-----------------------------------------y
//


#define  MaksimalusPeriodas    10000    // =16MH*100us  y--y    DEBUG
#define  MinimalusPeriodas     5000    // =16MH*70us   y--y    DEBUG
#define  PersijungimoPeriodas  2000    // =16MH*5us    x--x    DEBUG


//#define  MaksimalusPeriodas    1600    // =16MH*100us  y--y    Tikras
//#define  MinimalusPeriodas     1120    // =16MH*70us   y--y    Tikras
//#define  PersijungimoPeriodas    80    // =16MH*5us    x--x    Tikras 
#define  PeriodoPotenciometroPin  0

#define  TranzistorAPin 9    // Nekeiciami
#define  TranzistorBPin 10   // Nekeiciami



volatile int adcReading;
volatile boolean adcDone;
boolean adcStarted;

unsigned long DarbinisPeriodas;
unsigned long TeigiamasOCR1B;
unsigned long NeigiamasOCR1A;



void RestartTranzistors(void)//unsigned long Period, unsigned long TeigiamasOCR1B, unsigned long NeigiamasOCR1A)
{    
      digitalWrite (TranzistorAPin, LOW);
      digitalWrite (TranzistorBPin, LOW);
      
      pinMode(TranzistorAPin, OUTPUT);
      pinMode(TranzistorBPin, OUTPUT);
      
      //halting timer0 and timer1 only
      GTCCR = (1<<TSM)|(0<<PSRASY)|(1<<PSRSYNC); 

      //Reseting Timer1
      TCNT1H = 0; // set timer1 high byte to 0
      TCNT1L = 0; // set timer1 low byte to 0
      
      //Setting Timers mode
      TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(COM1B0)  ;  
      //TCCR1B=_BV(WGM13) |_BV(CS10);       //  PWM, Phase and Frequency Correct with TOP ICR1 +   clkI/O/   (  NO prescaler)
      TCCR1B=_BV(WGM13) |_BV(CS12)|_BV(CS10);       //  PWM, Phase and Frequency Correct with TOP ICR1 +   clkI/O/1024 (From prescaler)
      
      ICR1=DarbinisPeriodas ;        // compare match register 16MHz/256/2Hz
      
      OCR1A = NeigiamasOCR1A;
      OCR1B = TeigiamasOCR1B;
           
      
      GTCCR = 0; // release all timers
  
}

void setup()
{
 
    //noInterrupts();           // disable all interrupts
    //setting ADC
    ADCSRA =  bit (ADEN);                      // turn ADC on
    ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);   // Seting Prescaler for  ADC clock --- >128
    ADMUX  =  bit (REFS0) | (PeriodoPotenciometroPin & 0x07);    // AVcc and select input port
    //interrupts();

    //DEBUGING
    Serial.begin(9600);
    delay(1000);
    Serial.println ("Test setup"); 
}


// ADC complete ISR
ISR (ADC_vect)
{
  adcReading = ADC;
  adcDone = true;  
} 

void loop()
{
   
 // if last reading finished, process it
  if (adcDone)
    {      
      Serial.print ("adcReading:");    
      Serial.println (adcReading);    

      unsigned long GalimasReguliavimas= MaksimalusPeriodas-MinimalusPeriodas;

      unsigned long TikrasPeriodas= GalimasReguliavimas*adcReading/1023+MinimalusPeriodas;
      Serial.print ("TikrasPeriodas:");    
      Serial.println (TikrasPeriodas);   

  
      DarbinisPeriodas = TikrasPeriodas/2 ;




      TeigiamasOCR1B=DarbinisPeriodas/2-PersijungimoPeriodas/2;
      NeigiamasOCR1A=TeigiamasOCR1B+PersijungimoPeriodas;


      Serial.print ("DarbinisPeriodas:");    
      Serial.println (DarbinisPeriodas);   

      Serial.print ("NeigiamasOCR1A:");    
      Serial.println (NeigiamasOCR1A);   

      Serial.print ("TeigiamasOCR1B:");    
      Serial.println (TeigiamasOCR1B);   

      Serial.println ("-----------------");   

      

      RestartTranzistors();//DarbinisPeriodas, TeigiamasOCR1B, NeigiamasOCR1A);

      adcStarted = false;
      adcDone = false;
      delay (5000);
    }
    
  // if we aren't taking a reading, start a new one
  if (!adcStarted)
    {
    adcStarted = true;
    // start the conversion
    ADCSRA |= bit (ADSC) | bit (ADIE);
    }    
  
  // do other stuff here
}


















