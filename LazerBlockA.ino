
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
#define InputBlokoBusenaAktyviPin  4  

#define OutputTranzistorAPin 9    // Nekeiciami
#define OutputTranzistorBPin 10   // Nekeiciami
#define OutputBusenaKraunamPin  11  // Indikuoja Generatorius busena ijungta
#define OutputBusenaIskraunamPin  12  // Indikuoja Generatorius busena isjungta
#define OutputBlokoBusenaAktyviPin  13  // Indikuoja Blokas ijungtas


volatile int adcReading;
volatile boolean adcDone;
boolean adcStarted;

volatile unsigned long DarbinisPeriodas;
volatile unsigned long TeigiamasOCR1B;
volatile unsigned long NeigiamasOCR1A;

//volatile bool BlokoBusenaAktyvi=false;
//volatile bool InternalBusenaReady=false;

 
enum IPWMBusena: byte
{
  Kraunam,
  Iskraunam
};

 
enum IBlokoBusena: byte
{
  Inicijuojama,
  Isjungta,
  Ijungta  
};

volatile IPWMBusena PWMBusena=IPWMBusena::Iskraunam;
volatile IBlokoBusena BlokoBusena=IBlokoBusena::Inicijuojama;



/**************************************************************************************************************/
/********************INTERUPTS*********************************************************************************/
/**************************************************************************************************************/
//Greitas INTERUPTAS ant InputMinimalusLygisPasiektas
void InputMaksimalusLygisPasiektas()
{   
  IsjungtiTranzistoriuPWM();
  
  PWMBusena=IPWMBusena::Iskraunam;
  digitalWrite (OutputBusenaKraunamPin, LOW);
  digitalWrite (OutputBusenaIskraunamPin, HIGH);
}

//Greitas INTERUPTAS ant InputMaksimalusLygisPasiektas
void InputMinimalusLygisPasiektas()
{
  PWMBusena=IPWMBusena::Kraunam;
  digitalWrite (OutputBusenaKraunamPin, HIGH);
  digitalWrite (OutputBusenaIskraunamPin, LOW);
    
  IjungtiTranzistoriuPWM();
}

void IsjungtiTranzistoriuPWM(void)
{
  TCCR1B &= 0B11111000 ;  //atjungiam Timerio iejimo signala
  TCNT1H = 0; //Isvalom Timeri
  TCNT1L=0;   //Isvalom Timeri

  digitalWrite (OutputTranzistorAPin, LOW); //Isjungiam Isejimo Pinus
  digitalWrite (OutputTranzistorBPin, LOW); //Isjungiam Isejimo Pinus
}


void IjungtiTranzistoriuPWM(void)
{
    if ( BlokoBusena!=IBlokoBusena::Ijungta) return;
    if ( PWMBusena!=IPWMBusena::Kraunam) return;
    
  
    ICR1  = DarbinisPeriodas ;// compare match register
    OCR1A = NeigiamasOCR1A;   // turi dviguva buferi
    OCR1B = TeigiamasOCR1B;   // turi dviguva buferi

  
  TCCR1B |= (1<<CS12)|(1<<CS10) ;  // JUNGIAM timeri prie /1024 Prescalerio  (DEBUG)
  //TCCR1B |=           (1<<CS10) ;  //JUNGIAM timeri Tiesiai opie prie 16Mhz


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
   if (digitalRead(InputBlokoBusenaAktyviPin)==1)  
   {
      BlokoBusena=IBlokoBusena::Ijungta;
      IjungtiTranzistoriuPWM();
   }
   else
   {
      BlokoBusena=IBlokoBusena::Isjungta;      
      IsjungtiTranzistoriuPWM();
   }      
   
   digitalWrite (OutputBlokoBusenaAktyviPin, BlokoBusena==IBlokoBusena::Ijungta);

   Serial.println("PCINT2_vect");
    
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
    ADCSRA =  bit (ADEN);                      // turn ADC on
    ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);   // Seting Prescaler for  ADC clock --- >128
    ADMUX  =  bit (REFS0) | (PeriodoPotenciometroPin & 0x07);    // AVcc and select input port  
}

void SetupFastExternalInterrupts(void)
{
    digitalWrite (InputMaksimalusLygisPasiektasPin, HIGH);
    digitalWrite (InputMinimalusLygisPasiektasPin, HIGH); 
      
    pinMode(InputMaksimalusLygisPasiektasPin, INPUT_PULLUP);
    pinMode(InputMinimalusLygisPasiektasPin, INPUT_PULLUP);

        
    attachInterrupt (digitalPinToInterrupt (InputMaksimalusLygisPasiektasPin), InputMaksimalusLygisPasiektas, FALLING ); 
    attachInterrupt (digitalPinToInterrupt (InputMinimalusLygisPasiektasPin), InputMinimalusLygisPasiektas, FALLING ); 
}

void SetupExtraExternalInterrupts()
{  
  digitalWrite(InputBlokoBusenaAktyviPin, HIGH);   // Configure internal pull-up resistor
  pinMode(InputBlokoBusenaAktyviPin, INPUT_PULLUP);    // Pin A2 is input to which a switch is connected
  
  PCIFR = 0;                // cleaning External Interrupt register
  PCICR = bit (PCIE2);      // Enable PCINT1 interrupt
  PCMSK2 =  bit (PCINT20);  // Enabling interupt on PCINT20 = digital Pin 4

}

void SetupOutputPins(void)
{
   pinMode(OutputBlokoBusenaAktyviPin, OUTPUT);
   digitalWrite (OutputBlokoBusenaAktyviPin, BlokoBusena==IBlokoBusena::Ijungta);
}

void setup()
{
    //DEBUGING
    Serial.begin(9600);
    
    noInterrupts();
      SetupADCInterupts();
      SetupFastExternalInterrupts();
      SetupExtraExternalInterrupts(); 
      SetupOutputPins(); 
      SetupTranzistoriuPWM();
    interrupts();  
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
    unsigned long PersijungimoPeriodas = GalimasPersijungimoPeriodoReguliavimoDiapazonas*adcReading/1023 + MinimalusPersijungimoPeriodas;

    
    
    TeigiamasOCR1B=DarbinisPeriodas/2-PersijungimoPeriodas/2;
    NeigiamasOCR1A=TeigiamasOCR1B+PersijungimoPeriodas;
    BlokoBusena=IBlokoBusena::Isjungta;
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






















