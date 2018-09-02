byte pinIsNotLocked = 2;
byte pinIsNotClosed = 3;
byte pinLed = 4;
byte pinBeep = 5;
byte pinBeepSwitcher = 6;

int countNotLocked = 0;
int countNotClosed = 0;
bool isCountSwitcherDown = false;
int countOK = 0;
bool doBeep = false;
int timeLedOn = 1000;
int timeLedOff = 1000;
bool beepIsOn = true;

void setup() {
  pinMode(pinIsNotLocked, INPUT_PULLUP);
  pinMode(pinIsNotClosed, INPUT_PULLUP);
  pinMode(pinBeepSwitcher, INPUT_PULLUP);
  pinMode(pinLed, OUTPUT);  // эта команда устанавливает ножку к которй подключен светодиод в режим вывода информации
  pinMode(pinBeep, OUTPUT);
  digitalWrite(pinBeep , LOW);
  // Serial.begin(9600);
}

void loop() {

  if (!digitalRead(pinBeepSwitcher))
  {
    if (! isCountSwitcherDown)
    {
      isCountSwitcherDown = true;
      beepIsOn = ! beepIsOn;
      int time1 = 550;
      int time2 = 150;
      if (beepIsOn)
      {
        int t = time1;
        time1 = time2;
        time2 = t;
      }
      doBlink(time1, 150, 1, 1);
      doBlink(time2, 150, 1, 1);
    }
  }
  else
  {
    isCountSwitcherDown = false;
  }

  int locked = !digitalRead(pinIsNotLocked);
  if (locked)
  {
    countOK ++;
    if (countOK > 2)
    {
      countOK = 0;
      if (beepIsOn)
      {
        doBlink(1, 999, 1, 0);
      }
      else
      {
        doBlink(1, 80, 3, 0);
        delay(757);
      }
    }
    else
    {
      delay(1000);
    }
    countNotLocked = 0;
    countNotClosed = 0;
  }
  else
  {
    if (digitalRead(pinIsNotClosed))
    {
      doBeep = beepIsOn && (countNotClosed > 60);
      doBlink(150, 100, 3, doBeep);
      delay(250);
      countNotClosed = countNotClosed + !doBeep;
    }
    else
    {
      doBeep = beepIsOn && (countNotLocked  > 120);
      doBlink(500, 500, 1, doBeep);
      countNotLocked = countNotLocked + !doBeep;
    }
  }
  //Serial.println(timeLedOff);
}

void doBlink(int timeOn, int timeOff, byte count, bool doBeep)
{
  for (int i = 0; i < count; i++)
  {
    digitalWrite(pinLed , HIGH);
    if (doBeep)
    {
      for (int i = 1; i < timeOn; i = i + 2)
      {
        digitalWrite(pinBeep , HIGH);
        delay(1);
        digitalWrite(pinBeep , LOW);
        delay(1);
      }
    }
    else
    {
      delay(timeOn);
    }
    digitalWrite(pinLed , LOW);
    delay(timeOff);
  }
}

