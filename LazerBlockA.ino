
// _______                         _________________                         _________________
//        |_______________________|                 |_______________________|                 |_______
//            _________________                         _________________                         ____
// __________|                 |_______________________|                 |_______________________|            
//        x--x  
//           y-----------------------------------------y
//


#define  MaksimalusPeriodas    64000   // =16MHz*4000us  y--y    DEBUG
//#define  MaksimalusPeriodas    1600    // =16MHz*100us  y--y    Tikras

#define  MinimalusPeriodas     1120    // =16MHz*70us   y--y    Tikras

#define MaksimalusPersijungimoPeriodas 12800  // =16MHz*800us    x--x    DEBUG
//#define MaksimalusPersijungimoPeriodas  160     // =16MHz*5us    x--x    Tikras 

#define MinimalusPersijungimoPeriodas  80     // =16MHz*5us    x--x    Tikras 

#define  PeriodoPotenciometroPin  A0

#define InputMaksimalusLygisPasiektasPin 2  //P4
#define InputMinimalusLygisPasiektasPin  3  //P5
#define InputMazgasAEnablePin  4  

#define OutputTranzistorAPin 9    // Nekeiciami
#define OutputTranzistorBPin 10   // Nekeiciami
#define OutputBusenaKraunamPin  11  // Indikuoja Ar Generatorius ijungtas
#define OutputBusenaIskraunamPin  12  // Indikuoja Ar Generatorius ijungtas
#define OutputMazgasAktyvusPin  13  // Indikuoja Ar Generatorius ijungtas


volatile int adcReading;
volatile boolean adcDone;
boolean adcStarted;

volatile unsigned long DarbinisPeriodas;
volatile unsigned long TeigiamasOCR1B;
volatile unsigned long NeigiamasOCR1A;


/**************************************************************************************************************/
/********************INTERUPTS*********************************************************************************/
/**************************************************************************************************************/
//Greitas INTERUPTAS ant InputMinimalusLygisPasiektas
void IsjungtiTranzistoriuPWM(void)
{
  TCCR1B &= 0B11111000 ;
  TCNT1H = 0; 
  TCNT1L=0;
  digitalWrite (OutputTranzistorAPin, LOW);
  digitalWrite (OutputTranzistorBPin, LOW);
    
  Serial.println ("Ijungiam PWM");    
  
}

//Greitas INTERUPTAS ant InputMaksimalusLygisPasiektas
void IjungtiTranzistoriuPWM(void)
{
    ICR1  = DarbinisPeriodas ;// compare match register
    OCR1A = NeigiamasOCR1A;   // turi dviguva buferi
    OCR1B = TeigiamasOCR1B;   // turi dviguva buferi

  
  TCCR1B |= (1<<CS12)|(1<<CS10) ;  // IJUNGIAM /1024 Prescaleri  (DEBUG)
  //TCCR1B |=           (1<<CS10) ;  // IJUNGIAM /1 Prescaleri(Tiesiai opie prie 16Mhz)


  Serial.println ("Ijungiam PWM");    

  Serial.print ("Paskutine ADC reiksme:");    
  Serial.println (adcReading);    
  
  Serial.print ("DarbinisPeriodas:");    
  Serial.println (DarbinisPeriodas);   

  Serial.print ("A tranzistoriaus frontas:");    
  Serial.println (NeigiamasOCR1A);   

  Serial.print ("A tranzistoriaus frontas:");    
  Serial.println (TeigiamasOCR1B);   

  Serial.println ("-----------------");   



  
}
 
//Extra external inputs INTERUPTAS
ISR(PCINT2_vect) 
{
  Serial.println("PCINT2_vect");
   if (digitalRead(InputMazgasAEnablePin)==1)  
   {
      Serial.println("D4=1");
   }
   else
   {
       Serial.println("D4=0");
   }   
   
    digitalWrite (OutputMazgasAktyvusPin, digitalRead(InputMazgasAEnablePin)==1);
}

// ADC Nuskaitymas Ivyko INTERUPTAS
ISR (ADC_vect)
{
  adcReading = ADC;
  adcDone = true;  
} 

/**************************************************************************************************************/
/********************Setupai***********************************************************************************/
/**************************************************************************************************************/
 
void SetupTranzistoriuPWM(void)
{
      digitalWrite (OutputTranzistorAPin, LOW);
      digitalWrite (OutputTranzistorBPin, LOW);

      pinMode(OutputTranzistorAPin, OUTPUT);
      pinMode(OutputTranzistorBPin, OUTPUT);

      //Reseting Timer1
      TCNT1H = 0; // set timer1 high byte to 0
      TCNT1L = 0; // set timer1 low byte to 0
      
      //Setting Timers mode
      TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(COM1B0)  ;   //seting Timer1 Pins 9 ir 10
      TCCR1B=_BV(WGM13)  ;            //  PWM, Phase and Frequency Correct with ICR1   NO CLOCK Source.(TIMERIS ISJUNGAS)
      

}

void SetupADCInterupts(void)
{
    noInterrupts();
    ADCSRA =  bit (ADEN);                      // turn ADC on
    ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);   // Seting Prescaler for  ADC clock --- >128
    ADMUX  =  bit (REFS0) | (PeriodoPotenciometroPin & 0x07);    // AVcc and select input port  
    interrupts();  
}

void SetupFastExternalInterrupts(void)
{
    digitalWrite (InputMaksimalusLygisPasiektasPin, HIGH);
    digitalWrite (InputMinimalusLygisPasiektasPin, HIGH); 
      
    pinMode(InputMaksimalusLygisPasiektasPin, INPUT_PULLUP);
    pinMode(InputMinimalusLygisPasiektasPin, INPUT_PULLUP);

        
    attachInterrupt (digitalPinToInterrupt (InputMaksimalusLygisPasiektasPin), IsjungtiTranzistoriuPWM, FALLING ); 
    attachInterrupt (digitalPinToInterrupt (InputMinimalusLygisPasiektasPin), IjungtiTranzistoriuPWM, FALLING ); 
}

void SetupExtraExternalInterrupts()
{  
  noInterrupts();    // switch interrupts off while messing with their settings       
  
  digitalWrite(InputMazgasAEnablePin, HIGH);   // Configure internal pull-up resistor
  pinMode(InputMazgasAEnablePin, INPUT_PULLUP);    // Pin A2 is input to which a switch is connected
  
  PCIFR = 0;                // cleaning External Interrupt register
  PCICR = bit (PCIE2);      // Enable PCINT1 interrupt
  PCMSK2 =  bit (PCINT20);  // Enabling interupt on PCINT20 = digital Pin 4
  
  interrupts();  
}

void SetupOutputPins(void)
{
   pinMode(OutputMazgasAktyvusPin, OUTPUT);
}

void setup()
{
    //DEBUGING
    Serial.begin(9600);
    
    SetupADCInterupts();
    SetupFastExternalInterrupts();
    SetupExtraExternalInterrupts(); 
    SetupOutputPins(); 
    SetupTranzistoriuPWM();
    
    //RestartTranzistors();    
}
 
/**************************************************************************************************************/
/********************Pagrindinis Ciklas************************************************************************/
/**************************************************************************************************************/

void SkaiciuokPeriodus()
{
  noInterrupts();
    unsigned long GalimasReguliavimoDiapazonas= MaksimalusPeriodas-MinimalusPeriodas;
    unsigned long TikrasPeriodas= GalimasReguliavimoDiapazonas*adcReading/1023+MinimalusPeriodas;
    DarbinisPeriodas = TikrasPeriodas/2 ;

    unsigned long GalimasPersijungimoPeriodoReguliavimoDiapazonas= MaksimalusPersijungimoPeriodas-MinimalusPersijungimoPeriodas;
    unsigned long PersijungimoPeriodas= GalimasPersijungimoPeriodoReguliavimoDiapazonas*adcReading/1023+MinimalusPersijungimoPeriodas;

    
    
    TeigiamasOCR1B=DarbinisPeriodas/2-PersijungimoPeriodas/2;
    NeigiamasOCR1A=TeigiamasOCR1B+PersijungimoPeriodas;
  interrupts();  
}

void TikrintiADC(void)
{
    // if last reading finished, process it
    if (adcDone)
    {      
      SkaiciuokPeriodus();
      adcStarted = false;
      adcDone = false;
    }
    
    // Pradek Nauja ADC Skaityma;
    if (!adcStarted)
    {
      adcStarted = true;
      // start the conversion
      ADCSRA |= bit (ADSC) | bit (ADIE);
    }    
}

void loop()
{
   TikrintiADC();
}






















