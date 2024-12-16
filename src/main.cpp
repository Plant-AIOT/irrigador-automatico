#include <Arduino.h>
#include <RGBLed.h>
#include <RtcDS1302.h>
#include <WiFi.h>   

#define pinoSensor 36
#define pinRele 13
#define red_pin 26
#define green_pin 27
#define blue_pin 14
#define clock_rtc 25
#define data_rtc 33
#define rst_rtc 32

// rtc para o horario
ThreeWire myWire(data_rtc, clock_rtc, rst_rtc);
RtcDS1302<ThreeWire> Rtc(myWire);
void printDateTime(const RtcDateTime& dt);
// sensor de umidade
int umidade_agua = 0;
// led rgb
RGBLed led(red_pin, green_pin, blue_pin, RGBLed::COMMON_ANODE);

void setup() {
  Serial.begin(115200);
  pinMode(pinRele, OUTPUT);

  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);

  if (!Rtc.IsDateTimeValid()) 
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing
        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

}

void loop() {
  umidade_agua = analogRead(pinoSensor);
  umidade_agua = map(umidade_agua, 0, 4095, 0, 100);

  // imprime o horario atual
  RtcDateTime now = Rtc.GetDateTime();
  
  printDateTime(now);
  Serial.println();
  // imprime o valor do sensor de umidade
  Serial.println(umidade_agua);

  if (umidade_agua < 40) {
    Serial.println("Irrigacao desligada");
    digitalWrite(pinRele, LOW);
    led.setColor(255, 0, 0);

  }
  else if (umidade_agua >= 40 && umidade_agua < 70) {
    Serial.println("Modo manual...");
    digitalWrite(pinRele, HIGH);
    led.setColor(0, 255, 0);
  }
  else {
    Serial.println("Modo automatico...");
    led.setColor(0, 0, 255);
    // irriga quando as horas forem 07 e 16
    if ((now.Hour() == 07 || now.Hour() == 16) && now.Minute() == 0 && now.Second() <= 2) {
      digitalWrite(pinRele, HIGH); 
    }
    else {
      digitalWrite(pinRele, LOW); 
    }
  }
  
  delay(1000);
}

// funcao que imprime a data atual
void printDateTime(const RtcDateTime& dt)
{
    char datestring[26];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}