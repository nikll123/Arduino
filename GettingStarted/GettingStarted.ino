/*
   MKV RF
  Getting Started example sketch for nRF24L01+ radios
  This is a very basic example of how to send data from one node to another
  Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"
#include "printf.h"
#define RECIVE_TIMEUOT 200000
#define LOOP_PAUSE 20
#define PIPE1 0xF0F0F0F0D1LL
#define PIPE2 0xF0F0F0F0D2LL
#define PIN_INI_ROLE 4
#define SIGNAL_POWER RF24_PA_LOW
//#define SIGNAL_POWER RF24_PA_MAX
#define ROLE_TRANSMIT 0
#define ROLE_RECEIVE 1

/***      Set this radio as radio number 0 or 1         ***/

RF24 radio(9, 10);
bool role;
unsigned long countOk = 0;
unsigned long countFault1 = 0;
unsigned long countFault2 = 0;

void setup() {
  Serial.begin(115200);
  Serial.println(F("RF/GettingStarted."));
  printf_begin();
  radio.begin();

  // RF24_PA_MAX is default.
  radio.setPALevel(SIGNAL_POWER);
  radio.setAutoAck(1);
  pinMode(PIN_INI_ROLE, INPUT_PULLUP);
  role = digitalRead(PIN_INI_ROLE);
  role = set_role(role);
}

void loop() {

  /* TRANSMIT */
  if (role == ROLE_TRANSMIT)  {

    radio.stopListening();
    unsigned long start_time = micros();
    Serial.print(F("Trasmit "));
    Serial.print(start_time);
    if (radio.write(&start_time, sizeof(unsigned long)))
    {
      Serial.print(F(" ok."));
      unsigned long started_waiting_at = micros();
      boolean timeout = false;
      radio.startListening();
      while (!timeout)
      {
        if (radio.available())
        {
          unsigned long got_time;                                 // Grab the response, compare, and send to debugging spew
          radio.read( &got_time, sizeof(unsigned long) );
          unsigned long end_time = micros();
          Serial.print(F(" Got response "));
          if (got_time == start_time)
            Serial.print("correct");
          else
            Serial.print("wrong");
          Serial.print(F(". Delay "));
          Serial.print(end_time - start_time);
          Serial.print(F(" microseconds :)"));
          countOk++;
          break;
        }
        timeout = micros() - started_waiting_at > RECIVE_TIMEUOT;
      }
      if (timeout)
      {
        Serial.print(F(" Response failed, timed out :("));
        countFault2++;
      }
      Serial.println();
    }
    else
    {
      Serial.println(F(" failed."));
      countFault1++;
    }
    delay(LOOP_PAUSE);
  }
  else             //  role == ROLE_RECEIVE
  {
    unsigned long got_time;

    if (radio.available())
    {
      // Variable for the received timestamp
      while (radio.available())
      {
        radio.read( &got_time, sizeof(unsigned long) );             // Get the payload
      }

      radio.stopListening();
      radio.write( &got_time, sizeof(unsigned long) );
      radio.startListening();
      Serial.print(F("Sent response "));
      Serial.println(got_time);
    }
  }

  /*Serial.print(F("Ok "));
  Serial.print(countOk);
  Serial.print(F("   countFault1 "));
  Serial.print(countFault1);
  Serial.print(F("   countFault2 "));
  Serial.println(countFault2);*/

  /*  Roles  */

  if ( Serial.available() )
  {
    char c = toupper(Serial.read());
    if ( c == 'T' && role == ROLE_RECEIVE)
      role = set_role(ROLE_TRANSMIT);
    else if  ( c == 'R' && role == ROLE_TRANSMIT)
      role = set_role(ROLE_RECEIVE);
  }
}


bool set_role(bool role )
{
  if (role == ROLE_TRANSMIT) {
    radio.openWritingPipe(PIPE1);
    radio.openReadingPipe(1, PIPE2);
    Serial.println(F("Role TRANSMIT (press 'r' for recieve)"));
  }
  else
  {
    radio.openWritingPipe(PIPE2);
    radio.openReadingPipe(1, PIPE1);
    Serial.println(F("Role RECEIVE (press 't' for transmit)"));
    radio.startListening();
  }
  radio.printDetails();
  return role;
}
