/* Arduino 101: timer and interrupts
   1: Timer1 compare match interrupt example 
   more infos: http://www.letmakerobots.com/node/28278
   created by RobotFreak 
*/

#define PinLed 13

#define PinA 2
#define PinB 3
enum Tranzistors {
  unknown,
  tranistorA,
  tranistorB
};

Tranzistors ActiveTranzistor = unknown;

void setup()
{
 
          noInterrupts();           // disable all interrupts


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



 
}

/*
16.9.5 Phase and Frequency Correct PWM Mode

In phase and frequency correct PWM mode the counter is incremented until the counter value matches either
the value in ICR1 (WGM13:0 = 8), or the value in OCR1A (WGM13:0 = 9). The counter has then reached the
TOP and changes the count direction. The TCNT1 value will be equal to TOP for one timer clock cycle. The
timing diagram for the phase correct and frequency correct PWM mode is shown on Figure 16-9 on page 128.
The figure shows phase and frequency correct PWM mode when OCR1A or ICR1 is used to define TOP. The
TCNT1 value is in the timing diagram shown as a histogram for illustrating the dual-slope operation. The
diagram includes non-inverted and inverted PWM outputs. The small horizontal line marks on the TCNT1 slopes
represent compare matches between OCR1x and TCNT1. The OC1x Interrupt Flag will be set when a compare
match occurs.


ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
  digitalWrite(PinA, 0);   // toggle LED pin
  digitalWrite(PinB, 0);   // toggle LED pin
  delay(500);
  
  if(ActiveTranzistor!=tranistorB)
  {
    ActiveTranzistor=tranistorB;
    digitalWrite(PinB, 1);
  }
  else if(ActiveTranzistor!=tranistorA)
  {
    ActiveTranzistor=tranistorA;
    digitalWrite(PinA, 1);
  }  
  digitalWrite(PinLed, digitalRead(PinLed) ^ 1);   // toggle LED pin
}
*/

void loop()
{
  // your program here...
}
