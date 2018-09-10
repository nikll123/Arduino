#include <LiquidCrystal_I2C.h>
#include "RF24.h"
#include "printf.h"

#define PIN_BEEP 15

struct radioPack
{
  unsigned long timePack = 0;
  bool btn[6] = {0, 0, 0, 0, 0, 0};
  uint8_t count = 0;
};

radioPack rpTransmit;
radioPack rpRecieve;

LiquidCrystal_I2C lcd(0x3f, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

RF24 radio(9, 10);
const uint64_t pipes[2] = { 0x0808F0F0E1LL, 0x0808F0F0D2LL };

// Pins on the remote for buttons
const uint8_t button_pins[] = {2, 3, 4, 5, 6, 7};
const uint8_t num_but_rel = sizeof(button_pins);
const unsigned long timeout = 200000;  // mks
bool logconnect[100];
const uint8_t loglen = sizeof(logconnect);
uint8_t logindex = 0;

uint8_t countPrev = 0;

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_BEEP, OUTPUT);
  //Serial.println(F("2"));
  lcd.begin(16, 2);  // initialize the lcd for 16 chars 2 lines, turn on backlight

  lcd.noBacklight();
  delay(250);
  lcd.backlight();

  lcd.clear();
  lcd.print("PMS remote");
  lcd.setCursor(0, 1);
  lcd.print("Ver. 1.0.0");
  delay(1000);

  lcd.clear();
  lcd.print("Remote control");

  printf_begin();
  radio.begin();

  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1, pipes[1]);
  //radio.setAutoAck(0);
  radio.setPALevel(RF24_PA_MAX);
  radio.printDetails();

  beep(200);
  lcd.clear();
  for (int i = 0; i < loglen; i++)
  {
    logconnect[i] = 1;
  }

  for (int i = 0; i < num_but_rel; i++)
  {
    pinMode(button_pins[i], INPUT_PULLUP);
    lcdTransmitUpd(i);
    lcdRecieveUpd(i);
  }
  lcdCountUpdate(1);
  lcdConnectQuality(1);
}

void loop(void)
{
  String txt = "";
  for (int i = 0; i < num_but_rel; i++)
  {
    bool x = !digitalRead(button_pins[i]);
    if (rpTransmit.btn[i] != x)
    {
      rpTransmit.btn[i] = x;
      lcdTransmitUpd(i);
      lcd.setCursor(8, 1);
      lcd.print(i);
    }
  }

  rpTransmit.timePack = millis();
  radio.stopListening();
  bool ok = radio.write( &rpTransmit, sizeof(radioPack) );
  Serial.print(F("Sent. "));
  Serial.print(rpTransmit.timePack);
  Serial.print(F(" "));

  unsigned long started_waiting_at = micros();
  boolean timeout1 = false;
  radio.startListening();
  while (!radio.available())
  {
    unsigned long timewait =  micros() - started_waiting_at;
    timeout1 = timewait > timeout;
    if (timeout1)
      break;
  }
  radio.stopListening();

  logindex++;
    if (logindex < loglen )
  {}
  else
  {
    logindex = 0;
  }
  
  logconnect[logindex] = !timeout1;
  lcdConnectQuality(0);
  if ( timeout1 )
  {
    Serial.println(F("Timeout. "));
    beep(20);
  }
  else
  {
    Serial.print(F("Get response. "));
    radio.read( &rpRecieve, sizeof(radioPack) );
    if (rpRecieve.timePack == rpTransmit.timePack )
    {
      for (int i = 0; i < num_but_rel; i++)
      {
        lcdRecieveUpd(i);
      }
      lcdCountUpdate(0);
      //    txt = "Good.";
    }
    else
    {
      Serial.print(" ");
      Serial.print(rpRecieve.timePack);
      Serial.print(" ");
      //      txt = "Bad. ";
    }
    Serial.println();
  }

  //  lcd.setCursor(10, 1);
  //  lcd.print(txt);

  delay(50);
}

void beep(int x)
{
  digitalWrite(PIN_BEEP, HIGH);
  delay(x);
  digitalWrite(PIN_BEEP, LOW);
}


void lcdTransmitUpd(int i)
{
  _lcdUpdate(rpTransmit, i, 0);
}

void lcdRecieveUpd(int i)
{
  _lcdUpdate(rpRecieve, i, 1);
}

void _lcdUpdate(radioPack rp, int i, int line)
{
  lcd.setCursor(i, line);
  lcd.print(rp.btn[i]);
}


void lcdCountUpdate(bool forced)
{
  if (forced || rpRecieve.count != countPrev)
  {
    String txt = String(rpRecieve.count);
    txt = "  ";
    txt = txt + rpRecieve.count;
    txt = txt.substring(txt.length() - 3);
    lcd.setCursor(7, 0);
    lcd.print(txt);

    txt = "  ";
    txt = txt + rpRecieve.count * 100 / 255;
    txt = txt + "%";
    txt = txt.substring(txt.length() - 4);
    lcd.setCursor(11, 0);
    lcd.print(txt);
    countPrev = rpRecieve.count;
  }
}


void lcdConnectQuality(bool forced)
{
  if (forced || logindex / 10 * 10 == logindex )
  {
    int sum = 0;
    for ( int i = 0; i < loglen; i++ )
    {
      sum = sum + logconnect[i];
    }
    String txt = String(sum);
    txt = "c:" + txt + "%";
    lcd.setCursor(10, 1);
    lcd.print(txt);
  }
}

