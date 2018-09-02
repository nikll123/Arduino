#include "RF24.h"
#include "printf.h"

struct radioPack
{
  unsigned long timePack = 0;
  bool btn[6] = {0, 0, 0, 0, 0, 0};
  int count = 0;
};
radioPack rpRecieve;
radioPack rpSend;
radioPack rpRecievePrev;

RF24 radio(9, 10);
const uint64_t pipes[2] = { 0x0808F0F0E1LL, 0x0808F0F0D2LL };

const uint8_t relay_pins[] = { 2, 3, 4, 5, 6, 7 };
const uint8_t num_relay_pins = sizeof(relay_pins);
uint8_t relay_states[num_relay_pins];
const unsigned long timeoutReset = 1000;
unsigned long timeCounterLast = 0;

void setup(void)
{

  Serial.begin(115200);
  printf_begin();
  radio.begin();

  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1, pipes[0]);
  radio.startListening();

  for (int i = 0; i < num_relay_pins; i++)
  {
    pinMode(relay_pins[i], OUTPUT);
  }
  Reset();
  radio.setPALevel(RF24_PA_LOW);
  radio.printDetails();
}

void loop(void)
{

  unsigned long started_waiting_at = millis();
  boolean timeout = false;
  String txt = "";
  while (!radio.available())
  {
    timeout = (millis() - started_waiting_at) > timeoutReset;
    if (timeout)
      break;
    delay(3);
  }

  if (timeout)
  {
    Reset();
  }
  else
  {
    txt = "Get data. ";

    radio.read( &rpRecieve, sizeof(radioPack) );
    for (int i = 0; i < num_relay_pins; i++)
    {
      txt = txt + rpRecieve.btn[i];
    }
    txt = txt + " " + rpRecieve.timePack + " - ";

    setRel(0);
    setRel(5);

    triggerRel(1);
    triggerRel(4);

    counter(2, -1);
    counter(3, 1);

    rpSend.timePack = rpRecieve.timePack;
    radio.stopListening();
    bool ok = radio.write( &rpSend, sizeof(radioPack) );
    txt = txt + "Send back. ";
    radio.startListening();

    rpRecievePrev = rpRecieve;
    Serial.println(txt);
  }
}

//----------------------------------------//
void Reset()
{
  for (int i = 0; i < num_relay_pins; i++)
  {
    digitalWrite(relay_pins[i], 1);
    rpSend.btn[i] = 0;
  }
  rpSend.count = 0;
  Serial.println("Timeout. Reset. ");
}


//----------------------------------------//
void setRel(int i)
{
  digitalWrite(relay_pins[i], !rpRecieve.btn[i]);
  rpSend.btn[i] = rpRecieve.btn[i];
}


//----------------------------------------//
void triggerRel(int i)
{
  if (rpRecievePrev.btn[i] == 0 && rpRecieve.btn[i] == 1)
  {
    bool x = digitalRead(relay_pins[i]);
    digitalWrite(relay_pins[i], !x);
    rpSend.btn[i] = x;
  }
}

//----------------------------------------//
void counter(int i, int x)
{
  if (rpRecieve.btn[i] && 100 < rpRecieve.timePack - timeCounterLast )
  {
    int newCount = rpSend.count + x;
    if (newCount >= 0 && newCount <= 255)
    {
      rpSend.count = newCount;
      bool x = newCount > 100;
      digitalWrite(relay_pins[2], !x);
      rpSend.btn[2] = x;
      x = newCount > 200;
      digitalWrite(relay_pins[3], !x);
      rpSend.btn[3] = x;
    }
    timeCounterLast = rpRecieve.timePack;
  }
}

