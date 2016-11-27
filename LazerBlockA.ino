const byte adcPin = 0;
volatile int adcReading;
volatile boolean adcDone;
boolean adcStarted;

void setup()
{
 
          //noInterrupts();           // disable all interrupts


// use only one of the following 3 lines
//GTCCR = (1<<TSM)|(1<<PSRASY)|(1<<PSRSYNC); // halt all timers
//GTCCR = (1<<TSM)|(1<<PSRASY)|(0<<PSRSYNC); // halt timer2 only
GTCCR = (1<<TSM)|(0<<PSRASY)|(1<<PSRSYNC); // halt timer0 and timer1 only

// place all timer setup code here
// do not do any timer setup before this section

// set all timers to the same value Syncronyzing
TCNT0 = 0; // set timer0 to 0
TCNT1H = 0; // set timer1 high byte to 0
TCNT1L = 0; // set timer1 low byte to 0


//Setting Timers mode

          pinMode(9, OUTPUT);
          pinMode(10, OUTPUT);
          TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(COM1B0)  ;  
          TCCR1B=_BV(WGM13) |_BV(CS12);       //  PWM, Phase and Frequency Correct with TOP ICR1 +   clkI/O/256 (From prescaler)
          
          ICR1=10000 ;        // compare match register 16MHz/256/2Hz

          OCR1A = 5000;
          OCR1B = 5000;

          

GTCCR = 0; // release all timers

//setting ADC
ADCSRA =  bit (ADEN);                      // turn ADC on
ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);   // Seting Prescaler for  ADC clock --- >128
ADMUX  =  bit (REFS0) | (adcPin & 0x07);    // AVcc and select input port


  
//Interrupts();


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
  }  // end of ADC_vect
  
  

void loop()
{
   
 // if last reading finished, process it
  if (adcDone)
    {
    adcStarted = false;

    // do something with the reading, for example, print it
    Serial.println (adcReading);
    delay (500);

    adcDone = false;
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


















