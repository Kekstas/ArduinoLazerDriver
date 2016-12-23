void PrintTimer1( )
{
    //Serial.println(coment);
    unsigned int zTCNT1 = TCNT1;
    unsigned int zTCCR1A = TCCR1A;
    unsigned int zTCCR1B = TCCR1B;
    unsigned int zOCR1A = OCR1A;
    unsigned int zOCR1B = OCR1B;
    unsigned int zICR1 = ICR1;
 

    Serial.print("### counter1=");
    Serial.print(zTCNT1);    
    
    Serial.print(" TCCR1A=");
    Serial.print(zTCCR1A);    
    
    
    Serial.print(" TCCR1B=");
    Serial.print(zTCCR1B);    
    
    
    Serial.print(" OCR1A=");
    Serial.print(zOCR1A);    
    
    
    Serial.print(" OCR1B=");
    Serial.print(zOCR1B);    
    
    
    Serial.print(" ICR1=");
    Serial.println(zICR1);    
    
}
void SetupOutputPins(void)
{   
   pinMode(13, OUTPUT);

   digitalWrite (9, LOW);
   digitalWrite (10, LOW);
   digitalWrite (11, LOW);
   

   pinMode(9, OUTPUT);
   pinMode(10, OUTPUT);
   pinMode (11, OUTPUT);
   
}


void setup() 
{
    interrupts();  
      //DEBUGING
    Serial.begin(9600);

SetupOutputPins();
    
    // cleaning interupts
     TIMSK0=0; TIFR0=0;
     TIMSK1=0; TIFR1=0;    
     TIMSK2=0; TIFR2=0;

      Serial.println("Timerio Busena Pries:");
      PrintTimer1();
 ///////////SETTING Timer1////////////////////////
      GTCCR = (1<<TSM);//|(0<<PSRASY)|(1<<PSRSYNC);// halt timer0 and timer1 ; clears prescalers       
      
      //Setting Timers mode
      TCCR1A=0;
      //TCCR1B |= _BV(WGM12)|   _BV(WGM13)|       _BV(CS12)|_BV(CS10) ;  //  CTC mode,   ICR1 as top   ;     JUNGIAM timeri prie /1024 Prescalerio  (DEBUG)
      TCCR1B = _BV(WGM13)|       _BV(CS12)|_BV(CS10) ;  //  PWM, Phase and Frequency Correct mode,   ICR1 as top   ;     JUNGIAM timeri prie /1024 Prescalerio  (DEBUG)

      OCR1A  = 2000;
      OCR1B  = 0;
      ICR1  = 14468;

      //TCNT1=65534;  //  set timer1 to 0
      TCNT1=0;  //  set timer1 to 0

      GTCCR = (1<<TSM)|(0<<PSRASY)|(1<<PSRSYNC);// halt timer0 and timer1 ; clears prescalers       
      
      TIFR1=0;    // cleaning Interrupts; 
      TIMSK1=  _BV(ICIE1)|_BV(TOIE1)|_BV(OCIE1A); // enabling Overflow Interrupt; 
 
     GTCCR = 0; // release all timers
     PrintTimer1();
///////////SETTING Timer2////////////////////////


      TCNT2 = 0; // set timer2 byte to 0                  
      //Setting Timers2 mode
      TCCR2A = _BV(COM2A0)|_BV(COM2A1)|_BV(WGM21)|_BV( WGM20)  ;   //seting Timer2 Pins 11 ; Set OC2A on Compare Match, clear OC2A at BOTTOM  ; Fast PWM
      TCCR2B=4  ;  // Seting prescaler /starting timer2
      //TCCR2B=0;
  
      
      Serial.println("DabartineMashinosBusena=IsKraunam***********************************");


      PrintTimer1();

}

void IsjungtiIskrovima( )
{
 ///////////SETTING Timer1////////////////////////
      TIMSK1=  0; //disablinginterupts;
      
      //TCNT1H = 0; // set timer1 high byte to 0
      //TCNT1L = 0; // set timer1 low byte to 0
      TCNT1=0;  //  set timer1 to 0
      
      //Setting Timers mode      
      TCCR1B=0  ;            //NORMAL   NO CLOCK Source.(TIMERIS ISJUNGAS)

      
      TIFR1=0;    // cleaning  Interrupts; 

///////////SETTING Timer2////////////////////////
      
      TCCR2B=0;   //uzseinam Timeri i Normalia busena.  
      TCCR2A =  _BV(COM2A1)   ;   //seting Timer1 Pins 11 ; Clear on Outout Compare  
      TCCR2B=_BV(FOC2A);      // Force Outout Compare
  
      
      Serial.println("DabartineMashinosBusena=CikloPradzia");
 
}


//Timer1 Overflow event 
ISR(TIMER1_OVF_vect) 
{
   PrintTimer1();
    Serial.println("Timer1 Overflow");
  //  StateMashine(IEvent::LazerChargeTimePassed);
}

//Timer/Counter1 Capture Event
ISR(TIMER1_CAPT_vect) 
{

     PrintTimer1();
    Serial.println("Timer1 Capture Event");    

    TCNT1=0;
 //   StateMashine(IEvent::LazerChargeTimePassed);
}



//Timer/Counter1 Compare Match B
//ISR(TIMER1_COMPB_vect) 
//{
//   PrintTimer1();   
//   Serial.println("Timer1 Compare Match B");    
//}



//Timer/Counter1 Compare Match A
ISR(TIMER1_COMPA_vect) 
{
   PrintTimer1();
   Serial.println("Timer1 Compare Match A");
   //StateMashine(IEvent::LazerChargeTimePassed);
}
 
bool ijugti=true;
void loop() 
{
    // put your main code here, to run repeatedly:
    delay(100);
  
  if(ijugti)
  {
    //IsjungtiIskrovima();
    ijugti=false;
  }
}
  


