
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
        
#define  MaksimalusLazerioIskrovimoPeriodas     160000     // =16MHz*10000us
#define  MinimalusLazerioIskrovimoPeriodas       80000     // =16MHz*5000us 
                                               

 

//#define LazerioPWMPeriodas  1     // =255/16MHz =  16us  
//#define LazerioPWMPeriodas  2     // =255*8/16MHz =  127us
//#define LazerioPWMPeriodas  3     // =255*32/16MHz =  510us
#define LazerioPWMPeriodas  4     // =255*64/16MHz =  1020us


#define  PeriodoPotenciometroPin  0
#define  LazerioIskrovimoIlgisPotenciometroPin  1
#define  LazerioIskrovimoPWMPotenciometroPin  2

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


volatile int PaskutineADCReiksme[3]={-1, -1, -1};
volatile byte DabarAdcSkaitomasPin=-1;
//volatile boolean adcDone=false;
volatile boolean adcStarted;


volatile unsigned long DarbinisPeriodas;
volatile unsigned long TeigiamasOCR1B;
volatile unsigned long NeigiamasOCR1A;


volatile unsigned long LazerioIskrovimoIlgis = 64000;    
volatile  byte LazerioIskrovimoPWM=127;  // =Puse Laiko


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
  CikloPradzia,
  Kraunam, 
  Isjungta,
  IsKraunam    
};
 


enum IEvent: byte
{
  BaterijaTuscia,
  BaterijaPilna,
  ChargingPeriodsChanged,
  ExtraExternalInputs2Changed,
  TIMER0TimePassed
};


volatile IBaterijosBusenos DabartineBaterijosBusena=IBaterijosBusenos::Tuscia;
volatile IMasinosBusenos DabartineMashinosBusena=IMasinosBusenos::Inicijuojama;

unsigned long Timer0PasedTicks=0; 
unsigned long Timer0StopAtTick=0; 

 

/**************************************************************************************************************/
/********************DEBUG*********************************************************************************/
/**************************************************************************************************************/

void PrintTimer1( )
{
    //Serial.println(coment);
    unsigned int zTCNT1 = TCNT1;
    unsigned int zTCCR1A = TCCR1A;
    unsigned int zTCCR1B = TCCR1B;
    unsigned int zOCR1A = OCR1A;
    unsigned int zOCR1B = OCR1B;
    unsigned int zICR1 = ICR1;
 

    //Serial.print("### counter1=");
    //Serial.print(zTCNT1);    
    
    //Serial.print(" TCCR1A=");
    //Serial.print(zTCCR1A);    
    
    
    //Serial.print(" TCCR1B=");
    //Serial.print(zTCCR1B);    
    
    
    //Serial.print(" OCR1A=");
    //Serial.print(zOCR1A);    
    
    
    //Serial.print(" OCR1B=");
    //Serial.print(zOCR1B);    
    
    
    //Serial.print(" ICR1=");
    //Serial.println(zICR1);    
    
}

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
          
  TCCR1B=0;   //uzseinam Timeri i Normalia busena.  
  TCCR1A = _BV(COM1B1) | _BV(COM1A1)   ;   //seting Timer1 Pins 9 ir 10 ; Clear on Outout Compare  
  TCCR1C=_BV(FOC1A)|_BV(FOC1B);      // Force Outout Compare
  TCCR1A=0;
  TIFR1=0;    // cleaning Interrupts;   
  TCCR1B=_BV(CS10);

  
  DabartineMashinosBusena=IMasinosBusenos::Isjungta;
  //Serial.println("DabartineMashinosBusena=Isjungta");
  
}


void IjungtiBaterijosKrovima(void)
{
     
    //Setting Timers mode
    TCCR1A = _BV(COM1B1) | _BV(COM1A1) | _BV(COM1A0)  ;   //seting Timer1 Pins 9 ir 10 ; A-tranzas startuoja Uzssidares, B-Atsidares
    //TCCR1A=0;
    TCCR1B=_BV(WGM13) ;            //  PWM, Phase and Frequency Correct with ICR1   NO CLOCK Source.(TIMERIS ISJUNGAS)
    //TCCR1B=0;
    
    TCNT1=0;  //  set timer1 to 0
    
    ICR1  = DarbinisPeriodas ;// compare match register
    OCR1A = NeigiamasOCR1A;   // turi dviguva buferi
    OCR1B = TeigiamasOCR1B;   // turi dviguva buferi
    
    //TCCR1B |= _BV(CS12)|_BV(CS10) ;  // JUNGIAM timeri prie /1024 Prescalerio  (DEBUG)
    TCCR1B |=           _BV(CS10) ;  //JUNGIAM timeri Tiesiai prie prie 16Mhz
    

 

    DabartineMashinosBusena=IMasinosBusenos::Kraunam;
    //Serial.println("DabartineMashinosBusena=Kraunam");
    DabartineBaterijosBusena=IBaterijosBusenos::Pilna;  Timer0For(LazerioIskrovimoIlgis);   // DEBUG
    
}


void IjungtiIskrovima( )
{  
 ///////////SETTING Timer0////////////////////////
      Timer0For(LazerioIskrovimoIlgis);
///////////SETTING Timer2////////////////////////

      TCNT2 = 0; // set timer2 byte to 0                  
      //Setting Timers2 mode
      TCCR2A = _BV(COM2A0)|_BV(COM2A1)|_BV(WGM21)|_BV( WGM20)  ;   //seting Timer2 Pins 11 ; Set OC2A on Compare Match, clear OC2A at BOTTOM  ; Fast PWM
      TCCR2B=LazerioPWMPeriodas  ;  // Seting prescaler /starting timer2
      //TCCR2B=0;
  
      DabartineMashinosBusena=IMasinosBusenos::IsKraunam;
      //Serial.println("DabartineMashinosBusena=IsKraunam***********************************");
 
      //PrintTimer1();
 
}



void IsjungtiIskrovima( )
{
    ///////////SETTING Timer2////////////////////////
      
      TCCR2B=0;   //uzseinam Timeri i Normalia busena.  
      TCCR2A =  _BV(COM2A1)   ;   //seting Timer1 Pins 11 ; Clear on Outout Compare  
      TCCR2B=_BV(FOC2A);      // Force Outout Compare
  
      DabartineMashinosBusena=IMasinosBusenos::CikloPradzia;
      //Serial.println("DabartineMashinosBusena=CikloPradzia");
  
      DabartineBaterijosBusena=IBaterijosBusenos::Tuscia ; StateMashine(IEvent::TIMER0TimePassed); //DEBUG
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
          //Serial.println("DabartineBaterijosBusena=Pilna");
        break;
      case IEvent::BaterijaTuscia:
        digitalWrite (OutputBusenaBaterijaTusciaPin, HIGH);
        digitalWrite (OutputBusenaBaterijaPilnaPin, LOW);
        DabartineBaterijosBusena=IBaterijosBusenos::Tuscia;
        //Serial.println("DabartineBaterijosBusena=Tuscia");       
      break;
    }
}

 
void StateMashine(IEvent event)
{
   //Serial.print("StateMashine Event:");
   //Serial.println(event);
   
  
  switch(DabartineMashinosBusena)
  {
    case IMasinosBusenos::Inicijuojama: 
    case IMasinosBusenos::CikloPradzia:     
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
    if(event==IEvent::TIMER0TimePassed)
    {
      IsjungtiIskrovima();
    }
    break;     
  }
 
 
}


void Timer0For(unsigned long  ticks)
{
    //Serial.print("will wait for tiks");
    //Serial.println(ticks);

    
    if  (ticks>255)
    {
      OCR0A = 255;
      Timer0StopAtTick=ticks;
    }
    else
    {
       OCR0A=ticks;      
       Timer0StopAtTick=1;       
    }    
    Timer0PasedTicks=0;
    TCCR0A = 1;      
    TCCR0B = 1;   
          
    TCNT0  = 0;  //  set timer1 to 0
    TIFR0=0;    // cleaning Interrupts; 
    TIMSK0=  _BV(TOIE0); // enabling Overflow Interrupt;       
}

//Timer0 Overflow event 
ISR(TIMER0_OVF_vect) 
{
    //TIMSK0= 0;
    //StateMashine(IEvent::TIMER0TimePassed);            
    Timer0PasedTicks=Timer0PasedTicks+OCR0A+1;    
    //Serial.println("a");    
    if(Timer0PasedTicks>=Timer0StopAtTick)
    {
      //Serial.println(Timer0PasedTicks);    
      TIMSK0= 0;
      StateMashine(IEvent::TIMER0TimePassed);
    }
}
 
//Extra external inputs INTERUPTAS
ISR(PCINT2_vect) 
{
  StateMashine(IEvent::ExtraExternalInputs2Changed);      
}

// ADC Nuskaitymas Ivyko INTERUPTAS
ISR (ADC_vect)
{
  PaskutineADCReiksme[DabarAdcSkaitomasPin] = ADC;
  adcStarted = false;
} 

/**************************************************************************************************************/
/********************Setupai***********************************************************************************/
/**************************************************************************************************************/
 
 

void SetupADCInterupts()
{
    ADCSRA =  bit (ADEN);                      // turn ADC on
    ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);   // Seting Prescaler for  ADC clock --- >128
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
    
    
      SetupADCInterupts();
      SetupFastExternalInterrupts();
      SetupExtraExternalInterrupts(); 
      SetupOutputPins(); 
      
      //Serial.println ("Setup Finish-----------------");
    


    // cleaning interupts
     TIMSK0=0; TIFR0=0;
     TIMSK1=0; TIFR1=0;    
     TIMSK2=0; TIFR2=0;

}
 
/**************************************************************************************************************/
/********************Pagrindinis Ciklas************************************************************************/
/**************************************************************************************************************/

void SkaiciuokPeriodus()
{
    //unsigned long GalimasReguliavimoDiapazonas= MaksimalusPeriodas-MinimalusPeriodas;
    //unsigned long TikrasPeriodas= GalimasReguliavimoDiapazonas*adcReading/1023+MinimalusPeriodas;

      unsigned long  TikrasPeriodas = map(PaskutineADCReiksme[PeriodoPotenciometroPin], 0, 1023, MinimalusPeriodas, MaksimalusPeriodas);

    
    DarbinisPeriodas = TikrasPeriodas/2 ;

    //unsigned long GalimasPersijungimoPeriodoReguliavimoDiapazonas= MaksimalusPersijungimoPeriodas-MinimalusPersijungimoPeriodas;
    //unsigned long PersijungimoPeriodas = GalimasPersijungimoPeriodoReguliavimoDiapazonas*adcReading/1023 + MinimalusPersijungimoPeriodas;

    unsigned long PersijungimoPeriodas= map(PaskutineADCReiksme[PeriodoPotenciometroPin], 0, 1023, MinimalusPersijungimoPeriodas, MaksimalusPersijungimoPeriodas);
        
    TeigiamasOCR1B=(DarbinisPeriodas-PersijungimoPeriodas)/2;
    NeigiamasOCR1A=TeigiamasOCR1B+PersijungimoPeriodas;

    //Serial.println("SkaiciuokPeriodus done");        

    //Serial.print ("Paskutine ADC reiksme:");    
    //Serial.println (PaskutineADCReiksme[PeriodoPotenciometroPin]);    
    
    //Serial.print ("DarbinisPeriodas:");    
    //Serial.println (DarbinisPeriodas);   
  
    //Serial.print ("A tranzistoriaus frontas:");    
    //Serial.println (NeigiamasOCR1A);   
  
    //Serial.print ("B tranzistoriaus frontas:");    
    //Serial.println (TeigiamasOCR1B);   
  
    //Serial.println ("-----------------");  
    
}

void SkaiciuokLazerioIskrovimoIlgi()
{
   LazerioIskrovimoIlgis = map(PaskutineADCReiksme[LazerioIskrovimoIlgisPotenciometroPin], 0, 1023, MinimalusLazerioIskrovimoPeriodas, MaksimalusLazerioIskrovimoPeriodas);       
   //Serial.println("SkaiciuokLazerioIskrovimoIlgi done");        

   //Serial.print ("Paskutine ADC reiksme:");    
   //Serial.println (PaskutineADCReiksme[LazerioIskrovimoIlgisPotenciometroPin]); 
    
   //Serial.print("LazerioIskrovimoIlgis=");        
   //Serial.println(LazerioIskrovimoIlgis);     
   
   //Serial.println ("-----------------");        
}

void SkaiciuokLazerioIskrovimoPWM()
{
   byte _lazerioIskrovimoPWM = map(PaskutineADCReiksme[LazerioIskrovimoPWMPotenciometroPin], 0, 1023, 0, 255);
   LazerioIskrovimoPWM=_lazerioIskrovimoPWM;
   
  //Serial.println("SkaiciuokLazerioIskrovimoPWM done");        

  //Serial.print ("Paskutine ADC reiksme:");    
  //Serial.println (PaskutineADCReiksme[LazerioIskrovimoPWMPotenciometroPin]); 
  
  //Serial.print("LazerioIskrovimoPWM=");        
  //Serial.println(LazerioIskrovimoPWM);        

  //Serial.println ("-----------------");  
  
}


void StartADCReading(byte PotenciometroPin)
{
  //noInterrupts();
    adcStarted = true;      
    DabarAdcSkaitomasPin=PotenciometroPin;
    ADMUX  =  bit (REFS0) | (PotenciometroPin & 0x07);    // AVcc and select input port    
    ADCSRA |= bit (ADSC) | bit (ADIE);    
  //interrupts();  
}

void TikrintiADC(void)
{
    // if last reading finished, process it
//    if (adcDone)
//    {      
//      SkaiciuokPeriodus();
//      adcStarted = false;
//      adcDone = false;
//   }
    
    // Pradek Nauja ADC Skaityma;
    if (!adcStarted)
    {
      switch(DabarAdcSkaitomasPin)
      {
        case PeriodoPotenciometroPin:
          if(DabartineMashinosBusena==IMasinosBusenos::Inicijuojama) 
          {
              SkaiciuokPeriodus();
          }          
          StartADCReading(LazerioIskrovimoIlgisPotenciometroPin);
        break;
        case LazerioIskrovimoIlgisPotenciometroPin:
          if(DabartineMashinosBusena==IMasinosBusenos::Inicijuojama) 
          { 
              SkaiciuokLazerioIskrovimoIlgi();
          }        
          StartADCReading(LazerioIskrovimoPWMPotenciometroPin);
        break;        
        case LazerioIskrovimoPWMPotenciometroPin:
          if(DabartineMashinosBusena==IMasinosBusenos::Inicijuojama) 
          {     
            SkaiciuokLazerioIskrovimoPWM();
            StateMashine(IEvent::ChargingPeriodsChanged);    
          }
          StartADCReading(PeriodoPotenciometroPin);
        break;        
        default:
          StartADCReading(PeriodoPotenciometroPin);
        break;        
      }
 
      
    }    
}



void loop()
{
   TikrintiADC();
}






















