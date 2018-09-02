/*
  MKV RF
  NRF24L01_Test example sketch for nRF24L01+ radios
  This is a very basic example of how to send data from one node to another
  Updated: Aug 2018 by MKV
*/

#include "RF24.h"
#include "printf.h"
#define RECIVE_TIMEUOT 2000000
#define LOOP_PAUSE 20
#define PIPE1 0xF0F0F0F0D1LL
#define PIPE2 0xF0F0F0F0D2LL
#define PIN_INI_ROLE 4
#define SIGNAL_POWER RF24_PA_LOW // RF24_PA_MAX

/***      Set this radio as radio number 0 or 1         ***/

RF24 radio(9, 10);
enum roles {ROLE_TRANSMIT, ROLE_RECEIVE, ROLE_STOP};
roles role = ROLE_STOP;
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
  if (digitalRead(PIN_INI_ROLE) == LOW)
  {
    role = ROLE_TRANSMIT;
  }
  role = set_role(role);
}

void loop() {

  /* TRANSMIT */
  if (role == ROLE_STOP)
  {
    //Serial.println(F("ROLE_STOP"));
  }
  else if (role == ROLE_TRANSMIT)
  {
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
      radio.stopListening();
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
  else if (role == ROLE_RECEIVE)
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
    if ( c == 'T' && role != ROLE_TRANSMIT)
      role = set_role(ROLE_TRANSMIT);
    else if  ( c == 'R' && role != ROLE_RECEIVE)
      role = set_role(ROLE_RECEIVE);
    else if  ( c == 'S' && role != ROLE_STOP)
      role = set_role(ROLE_STOP);
  }
}


roles set_role(roles role)
{
  radio.stopListening();
  String new_role = "";
  if (role == ROLE_STOP)
  {
    new_role = "ROLE_STOP";
  }
  else if (role == ROLE_TRANSMIT)
  {
    new_role = "ROLE_TRANSMIT";
    radio.openWritingPipe(PIPE1);
    radio.openReadingPipe(1, PIPE2);
  }
  else // if (role == ROLE_RECEIVE)
  {
    new_role = "ROLE_RECEIVE";
    radio.openWritingPipe(PIPE2);
    radio.openReadingPipe(1, PIPE1);
    radio.startListening();
  }
  Serial.print(new_role);
  Serial.println(F(" (t - TRANSMIT; r - RECEIVE; s - STOP)"));
  radio.printDetails();
  return role;
}
