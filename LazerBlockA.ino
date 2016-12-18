
// _______                         _________________                         _________________
//        |_______________________|                 |_______________________|                 |_______
//            _________________                         _________________                         ____
// __________|                 |_______________________|                 |_______________________|            
//        x--x  
//           y-----------------------------------------y
//


//#define  MaksimalusPeriodas    32000   // =16MHz*2000us  y--y    DEBUG
#define  MaksimalusPeriodas    1600    // =16MHz*100us  y--y    Tikras

#define  MinimalusPeriodas     1120    // =16MHz*70us   y--y    Tikras

//#define MaksimalusPersijungimoPeriodas 6400  // =16MHz*400us    x--x    DEBUG
#define MaksimalusPersijungimoPeriodas  160     // =16MHz*5us    x--x    Tikras 

#define MinimalusPersijungimoPeriodas  80     // =16MHz*5us    x--x    Tikras 


//#define LazerioPWMPeriodas  1     // =255/16MHz =  16us  
//#define LazerioPWMPeriodas  2     // =255*8/16MHz =  127us
//#define LazerioPWMPeriodas  3     // =255*32/16MHz =  510us
#define LazerioPWMPeriodas  4     // =255*64/16MHz =  1020us


#define  PeriodoPotenciometroPin  0

#define InputMaksimalusLygisPasiektasPin 2  //P4
#define InputMinimalusLygisPasiektasPin  3  //P5
#define InputBlokoBusenaAktyviPin  4 
#define InputLazerisAktyvusPin  5  
 

#define OutputTranzistorAPin 9    // Nekeiciami
#define OutputTranzistorBPin 10   // Nekeiciami
#define OutputLazerTranzistorPin 11   // Nekeiciami

#define OutputBusenaBaterijaTusciaPin  6  // Indikuoja Generatorius busena ijungta
#define OutputBusenaBaterijaPilnaPin  7  // Indikuoja Generatorius busena isjungta
#define OutputBlokoBusenaAktyviPin  13  // Indikuoja Blokas ijungtas


volatile int adcReading;
volatile boolean adcDone;
boolean adcStarted;

volatile unsigned long DarbinisPeriodas;
volatile unsigned long TeigiamasOCR1B;
volatile unsigned long NeigiamasOCR1A;


volatile unsigned long LazerioVeikimoPeriodas = 64000;    
volatile unsigned long LazerioOCR2A=127;  // =Puse Laiko


//volatile bool BlokoBusenaAktyvi=false;
//volatile bool InternalBusenaReady=false;

 
enum IBaterijosBusenos: byte
{
  Tuscia,
  Pilna
};

 
enum IMasinosBusenos: byte
{
  Inicijuojama,
  Kraunam, 
  Isjungta,
  IsKraunam    
};
 


enum IEvent: byte
{
  Startas,
  BaterijaTuscia,
  BaterijaPilna,
  ChargingPeriodsChanged,
  ExtraExternalInputs2Changed,
  LazerChargeTimePassed
};


volatile IBaterijosBusenos DabartineBaterijosBusena=IBaterijosBusenos::Tuscia;
volatile IMasinosBusenos DabartineMashinosBusena=IMasinosBusenos::Inicijuojama;


/**************************************************************************************************************/
/********************INTERUPTS*********************************************************************************/
/**************************************************************************************************************/
//Greitas INTERUPTAS ant InputMinimalusLygisPasiektas
void InputMaksimalusLygisPasiektas()
{ 
  BaterijaStateMashine(IEvent::BaterijaPilna);    
  StateMashine(IEvent::BaterijaPilna);  
}

//Greitas INTERUPTAS ant InputMaksimalusLygisPasiektas
void InputMinimalusLygisPasiektas()
{
  BaterijaStateMashine(IEvent::BaterijaTuscia);    
  StateMashine(IEvent::BaterijaTuscia);
}

   
void IsjungtiBaterijosKrovima(void)
{
  
  //if ( PWMBusena!=IPWMBusena::IsKrauti) return;   
  //PWMBusena=IPWMBusena::IsKraunam;
  
  TCCR1B &= 0B11111000 ;  //atjungiam Timerio iejimo signala
  //TCNT1H = 0; //Isvalom Timeri
  //TCNT1L=0;   //Isvalom Timeri
  
  //pinMode(OutputTranzistorAPin, OUTPUT); 
  //digitalWrite (OutputTranzistorAPin, LOW); //Isjungiam Isejimo Pinus
  
  //pinMode(OutputTranzistorBPin, OUTPUT); 
  //digitalWrite (OutputTranzistorBPin, LOW); //Isjungiam Isejimo Pinus
  
  TCCR1B=0;   //uzseinam Timeri i Normalia busena.  
  TCCR1A = _BV(COM1B1) | _BV(COM1A1)   ;   //seting Timer1 Pins 9 ir 10 ; Clear on Outout Compare
  
  TCCR1C=_BV(FOC1A)|_BV(FOC1B);      // Force Outout Compare

  //TCCR1B=_BV(WGM13)  ;  //Seting Timer Back
  //TCCR1A = _BV(COM1B1) | _BV(COM1A1) | _BV(COM1A0)  ;   //seting Timer1 Pins 9 ir 10 ; A-tranzas startuoja Uzssidares, B-Atsidares

  DabartineMashinosBusena=IMasinosBusenos::Isjungta;
  Serial.println("DabartineMashinosBusena=Isjungta");
  
}


void IjungtiBaterijosKrovima(void)
{
    
      //Reseting Timer1
      TCNT1H = 0; // set timer1 high byte to 0
      TCNT1L = 0; // set timer1 low byte to 0
      
      //Setting Timers mode
      TCCR1A = _BV(COM1B1) | _BV(COM1A1) | _BV(COM1A0)  ;   //seting Timer1 Pins 9 ir 10 ; A-tranzas startuoja Uzssidares, B-Atsidares
      TCCR1B=_BV(WGM13)  ;            //  PWM, Phase and Frequency Correct with ICR1   NO CLOCK Source.(TIMERIS ISJUNGAS)


      //TIFR1=_BV(TOV1);    // cleaning  Overflow Interrupt; DEBUG
      //TIMSK1=  _BV(TOIE1); // enabling Overflow Interrupt; DEBUG
 
    
    ICR1  = DarbinisPeriodas ;// compare match register
    OCR1A = NeigiamasOCR1A;   // turi dviguva buferi
    OCR1B = TeigiamasOCR1B;   // turi dviguva buferi

    TCCR1B |= (1<<CS12)|(1<<CS10) ;  // JUNGIAM timeri prie /1024 Prescalerio  (DEBUG)
    //TCCR1B |=           (1<<CS10) ;  //JUNGIAM timeri Tiesiai prie prie 16Mhz

    DabartineMashinosBusena=IMasinosBusenos::Kraunam;
    Serial.println("DabartineMashinosBusena=Kraunam");
  
    

  Serial.print ("Paskutine ADC reiksme:");    
  Serial.println (adcReading);    
  
  Serial.print ("DarbinisPeriodas:");    
  Serial.println (DarbinisPeriodas);   

  Serial.print ("A tranzistoriaus frontas:");    
  Serial.println (NeigiamasOCR1A);   

  Serial.print ("B tranzistoriaus frontas:");    
  Serial.println (TeigiamasOCR1B);   

  Serial.println ("-----------------");   
  
}

void IjungtiIskrovima( )
{
 ///////////SETTING Timer1////////////////////////
      TCNT1H = 0; // set timer1 high byte to 0
      TCNT1L = 0; // set timer1 low byte to 0
      
      //Setting Timers mode
      TCCR1A = 0  ;   //seting Timer1 Pins 9 ir 10 ; A-tranzas startuoja Uzssidares, B-Atsidares
      TCCR1B=_BV(WGM12)|_BV(WGM13)  ;            // CTC and Frequency Correct with ICR1   NO CLOCK Source.(TIMERIS ISJUNGAS)
      //TCCR1B=_BV(WGM13)  ;
      //TCCR1B=0  ;            //NORMAL   NO CLOCK Source.(TIMERIS ISJUNGAS)

      OCR1A  = 0;//LazerioVeikimoPeriodas ;// compare match register
      ICR1  = LazerioVeikimoPeriodas ;// compare match register

      TIFR1=0;    // cleaning Interrupts; 
      TIMSK1=  _BV(ICIE1);//|_BV(TOIE1)|_BV(OCIE1A); // enabling Overflow Interrupt; 
 
 

      TCCR1B |= (1<<CS12)|(1<<CS10) ;  // JUNGIAM timeri prie /1024 Prescalerio  (DEBUG)
     //TCCR1B |=           (1<<CS10) ;  //JUNGIAM timeri Tiesiai prie prie 16Mhz

///////////SETTING Timer2////////////////////////


      TCNT2 = 0; // set timer2 byte to 0      
            
      //Setting Timers2 mode
      TCCR2A = _BV(COM2A0)|_BV(COM2A1)|_BV(WGM21)|_BV( WGM20)  ;   //seting Timer2 Pins 11 ; Set OC2A on Compare Match, clear OC2A at BOTTOM  ; Fast PWM
      TCCR2B=LazerioPWMPeriodas  ;  // Seting prescaler
      //TCCR2B=0;
  
      DabartineMashinosBusena=IMasinosBusenos::IsKraunam;
      Serial.println("DabartineMashinosBusena=IsKraunam");
 
}



void IsjungtiIskrovima( )
{
 ///////////SETTING Timer1////////////////////////
      TCNT1H = 0; // set timer1 high byte to 0
      TCNT1L = 0; // set timer1 low byte to 0
      
      //Setting Timers mode      
      TCCR1B=0  ;            //NORMAL   NO CLOCK Source.(TIMERIS ISJUNGAS)

      TIMSK1=  0; //disablinginterupts;
      TIFR1=0;    // cleaning  Interrupts; 

///////////SETTING Timer2////////////////////////
      
      TCCR2B=0; //disconectint source
  
      DabartineMashinosBusena=IMasinosBusenos::Inicijuojama;
      Serial.println("DabartineMashinosBusena=Inicijuojama");
 
}



void BaterijaStateMashine(IEvent event)
{
     //Serial.print("DabartineBaterijosBusena Event:");
     //Serial.println(event);
   
    switch(event)
    {
      case IEvent::BaterijaPilna:
          digitalWrite (OutputBusenaBaterijaTusciaPin, LOW);
          digitalWrite (OutputBusenaBaterijaPilnaPin, HIGH);
          DabartineBaterijosBusena=IBaterijosBusenos::Pilna;
          Serial.println("DabartineBaterijosBusena=Pilna");
        break;
      case IEvent::BaterijaTuscia:
        digitalWrite (OutputBusenaBaterijaTusciaPin, HIGH);
        digitalWrite (OutputBusenaBaterijaPilnaPin, LOW);
        DabartineBaterijosBusena=IBaterijosBusenos::Tuscia;
        Serial.println("DabartineBaterijosBusena=Tuscia");       
      break;
    }
}

 
void StateMashine(IEvent event)
{
   Serial.print("StateMashine Event:");
   Serial.println(event);
   
  
  switch(DabartineMashinosBusena)
  {
    case IMasinosBusenos::Inicijuojama: 
      if (digitalRead(InputBlokoBusenaAktyviPin)==1 &&  DabartineBaterijosBusena==IBaterijosBusenos::Tuscia  )  
      {  
         IjungtiBaterijosKrovima();
         break;
      }   
      IsjungtiBaterijosKrovima();
      break;
    case IMasinosBusenos::Kraunam: 
      if (digitalRead(InputBlokoBusenaAktyviPin)==0 || DabartineBaterijosBusena==IBaterijosBusenos::Pilna)  
      {  
        IsjungtiBaterijosKrovima();
        if (digitalRead(InputLazerisAktyvusPin)==1 &&  DabartineBaterijosBusena==IBaterijosBusenos::Pilna )  
        {
          IjungtiIskrovima();
        }        
        break;
      }
    break;
    case IMasinosBusenos::Isjungta: 
      if (digitalRead(InputBlokoBusenaAktyviPin)==1 &&  DabartineBaterijosBusena==IBaterijosBusenos::Tuscia  )  
      {  
         IjungtiBaterijosKrovima();
         break;
      }
      if (digitalRead(InputLazerisAktyvusPin)==1 &&  DabartineBaterijosBusena==IBaterijosBusenos::Pilna )  
      {  
         IjungtiIskrovima();
         break;
      }    
      
    break;
    case IMasinosBusenos::IsKraunam: 
    if(event==IEvent::LazerChargeTimePassed)
    {
      IsjungtiIskrovima();
    }
    break;     
  }
 
 
}

//Timer/Counter1 Capture Event
ISR(TIMER1_CAPT_vect) 
{
    StateMashine(IEvent::LazerChargeTimePassed);
}



//Timer/Counter1 Compare Match B
ISR(TIMER1_COMPB_vect) 
{
    Serial.println("Timer1 Compare Match B");
}



//Timer/Counter1 Compare Match A
ISR(TIMER1_COMPA_vect) 
{
    Serial.println("Timer1 Compare Match A");
}


//Timer1 Overflow event 
ISR(TIMER1_OVF_vect) 
{
    Serial.println("Timer1 Overflow");
}

 
//Extra external inputs INTERUPTAS
ISR(PCINT2_vect) 
{
  
  StateMashine(IEvent::ExtraExternalInputs2Changed);    
 


  
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
      
    pinMode(InputMaksimalusLygisPasiektasPin, INPUT);
    pinMode(InputMinimalusLygisPasiektasPin, INPUT);

 
    if(digitalRead(InputMaksimalusLygisPasiektasPin)==0)
    {
      BaterijaStateMashine(IEvent::BaterijaPilna);
    }
    else
    {
      BaterijaStateMashine(IEvent::BaterijaTuscia);
    }

        
    attachInterrupt (digitalPinToInterrupt (InputMaksimalusLygisPasiektasPin), InputMaksimalusLygisPasiektas, FALLING  ); 
    attachInterrupt (digitalPinToInterrupt (InputMinimalusLygisPasiektasPin), InputMinimalusLygisPasiektas, FALLING  ); 
}

void SetupExtraExternalInterrupts()
{  
  digitalWrite(InputBlokoBusenaAktyviPin, HIGH);   // Configure internal pull-up resistor
  pinMode(InputBlokoBusenaAktyviPin, INPUT);    // Pin A2 is input to which a switch is connected
  
  digitalWrite(InputLazerisAktyvusPin, HIGH);   // Configure internal pull-up resistor
  pinMode(InputLazerisAktyvusPin, INPUT);    // Pin A2 is input to which a switch is connected
  
  PCIFR = 0;                // cleaning External Interrupt register
  PCICR = bit (PCIE2);      // Enable PCINT1 interrupt
  PCMSK2 =  bit (PCINT20)|bit (PCINT21);  // Enabling interupt on PCINT20 = digital Pin 4 , PCINT21 = digital Pin 5

}

void SetupOutputPins(void)
{
   digitalWrite (OutputBlokoBusenaAktyviPin, DabartineMashinosBusena==IMasinosBusenos::Kraunam);
   pinMode(OutputBlokoBusenaAktyviPin, OUTPUT);

   digitalWrite (OutputTranzistorAPin, LOW);
   digitalWrite (OutputTranzistorBPin, LOW);
   digitalWrite (OutputLazerTranzistorPin, LOW);

   

   pinMode(OutputTranzistorAPin, OUTPUT);
   pinMode(OutputTranzistorBPin, OUTPUT);
   pinMode (OutputLazerTranzistorPin, OUTPUT);
   
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
      
      Serial.println ("Setup Finish-----------------");
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
 
    if(DabartineMashinosBusena==IMasinosBusenos::Inicijuojama) 
    {
      StateMashine(IEvent::ChargingPeriodsChanged);    
    }
 
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
      //delay(1000);
    }    
}

void loop()
{
   TikrintiADC();
}






















