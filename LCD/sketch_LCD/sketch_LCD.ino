// подключаем неоходимую библиотеку
#include <LiquidCrystal.h>

// указаем к каким пинам подключен дисплей
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);

void setup()
{
  // указываем, что наш дисплей имеет 16 символов и 2 строки
  lcd.begin(16, 2);
}

void loop()
{
  lcd.setCursor(5, 0);           // ставим курсор в 5ый символ и нулевую строку 
  lcd.print("Hello");            // выводим сообщение на дисплей
  lcd.setCursor(0, 1);           // ставим курсор в нулевой символ и первую строку 
  lcd.print("                "); // очистка строки
  delay(5000);                   // задержка 
  lcd.setCursor(2, 1);  // сиавим курсор во вторую строку и 4ый символ
  lcd.print("ARDUINOMANIA"); 
  delay(5000);
  lcd.setCursor(5, 0);  
  lcd.print("YTM  "); 
  lcd.setCursor(0, 1);           
  lcd.print("123456789abcdefh"); 
  delay(5000);
}

