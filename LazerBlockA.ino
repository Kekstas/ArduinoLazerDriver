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
        
    digitalWrite (PinLed, LOW);
    
    digitalWrite (PinA, LOW);
    digitalWrite (PinB, LOW);
  
    
  pinMode(PinLed, OUTPUT);
  pinMode(PinA, OUTPUT);
  pinMode(PinB, OUTPUT);
  
  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 31250;            // compare match register 16MHz/256/2Hz
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts
}

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

void loop()
{
  // your program here...
}
