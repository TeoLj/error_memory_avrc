
#include <avr/io.h>
#include <util/delay.h>


#define ERRORS_LEN 5


volatile char previous_Error_Type;
volatile int counter_error = 0;

unsigned long int start_time = 0;
char error_array[ERRORS_LEN];

void insertError(char errorType);
void printErrors();
void processError(char errorType);
void CodeA();
void CodeB();
void CodeC();



//When DDRx sets the pin to input, PORTx controls the pullup resistor.
//When the PORTx bit is 0,
//the pullups are disabled; when 1, pullups are enabled.



int main ()
{
  DDRD &= ~(1 << DDD2); // Digital2=INT0 as Input
  PORTD |= (1 << PORTD2); //Pull-up enablen

  DDRD &= ~(1 << DDD3); // Digital3=INT1 as Input
  PORTD |= (1 << PORTD3); //Pull-up enablen

  DDRC &= ~(1 << DDC0); // A0=PCINT8 as Input
  PORTC |= (1 << PORTC0); //Pull-up enablen

  //INT0 configuration (falling edge)
  EICRA |= (1 << ISC00);
  EICRA &= ~ (1 << ISC01);
  //INT0 aktivieren
  EIMSK |= (1 << INT0);

  //INT1 configuration (falling edge)
  EICRA &= ~ (1 << ISC10);
  EICRA |= (1 << ISC11);
  //INT0 aktivieren
  EIMSK |= (1 << INT1);

  //PCIE1 activation, Pin change interrupt
  PCICR |= (1 << PCIE1);
  //PCINT8 activation in mask
  PCMSK1 |= (1 << PCINT8);

  //globale Aktivierung
  sei();

  init();
  Serial.begin(9600);

  while (1)
  {
    while ((millis() - start_time) >= 1000)//Sekundentakt
    {
      printErrors();
      start_time = millis();
    }
  }
}



void insertError(char errorType) {
  for (int i = ERRORS_LEN - 1; i > 0 ; i--)
  {
    error_array[i ] = error_array[i - 1]; //Shifting the elements
  }
  error_array[0] = errorType;  // new element in the first place
}


void printErrors() {
  Serial.println("-------------------------");
  Serial.println("Total errors: " + String(counter_error));
  Serial.print("Errors: " );
  for (int i = 0; i < ERRORS_LEN; i++)
  {
    Serial.print(String(error_array[i]) + " ");
  }
  Serial.println("\n");
}


void processError(char errorType) {

  if (previous_Error_Type != errorType) {
    counter_error++;
    insertError(errorType);
  }
  previous_Error_Type = errorType;
}




ISR(INT0_vect) {
  processError('A');  // or: if (error_char[0]!)=0
}

ISR (INT1_vect) {
  processError('B');
}

ISR(PCINT1_vect) {
  if (( PINC & (1 << PCINT8) ) == 0) //falling edge
    processError('C');
}
