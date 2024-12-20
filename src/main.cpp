/*
  MODOS:
  Azul: Irrigação desligada
  Roxo: Modo manual
  Verde: Modo automatico

  NIVEIS DE AGUA:
  Vermelho: Nivel baixo
  Amarelo: Nivel medio
  Verde: Nivel alto
*/
#include <Arduino.h>
#include <RGBLed.h>
#include <RtcDS1302.h>
#include <WiFi.h> 
#include <Ultrasonic.h>

// pino potenciometro
#define pinPotenciometro 36
// pino rele
#define pinRele 13
// pinos do led rgb que indica os modos de funcionamento
#define red_pin 26
#define green_pin 27
#define blue_pin 14
// pinos do modulo rtc
#define clock_rtc 25
#define data_rtc 33
#define rst_rtc 32
// pino do sensor de umidade
#define pinSensor 4
// pino do sensor de nivel
#define pinEcho 18
#define pinTrigger 19
// pino dos leds de nivel
#define ledGreen 5
#define ledYellow 2
#define ledRed 15

// rtc para o horario
ThreeWire myWire(data_rtc, clock_rtc, rst_rtc);
RtcDS1302<ThreeWire> Rtc(myWire);
void printDateTime(const RtcDateTime& dt);
// potenciometro
int valor_potenciometro = 0;
// led rgb
RGBLed led(red_pin, green_pin, blue_pin, RGBLed::COMMON_ANODE);
// sensor HC-SR04
Ultrasonic sensorNivel(pinTrigger, pinEcho);	
int porcentagem_reservatorio = 0;
// sensor de umidade
int sensor_umidade = 0;

void setup() {
  Serial.begin(115200);
  pinMode(pinRele, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledRed, OUTPUT);

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
  valor_potenciometro = analogRead(pinPotenciometro);
  valor_potenciometro = map(valor_potenciometro, 0, 4095, 0, 100);
  porcentagem_reservatorio = map(sensorNivel.read(), 10, 2, 0, 100);
  sensor_umidade = analogRead(pinSensor);
  sensor_umidade = map(sensor_umidade, 0, 4095, 0, 100);

  // imprime o horario atual
  RtcDateTime now = Rtc.GetDateTime();

  // modos de irrigacao
  if (valor_potenciometro < 33) {
    Serial.println("Irrigacao desligada");
    digitalWrite(pinRele, LOW);
    led.setColor(255, 0, 0);

  }
  else if (valor_potenciometro >= 33 && valor_potenciometro < 66) {
    Serial.println("Modo manual...");
    digitalWrite(pinRele, HIGH);
    led.setColor(0, 255, 0);
  }
  else {
    Serial.println("Modo automatico...");
    led.setColor(0, 0, 255);
    // irriga quando o horario estiver entre 07:00 e 17:00 e a umidade do solo estiver menor que 70%
    if ((now.Hour() >= 07 && now.Hour() <= 17)) {
      if (sensor_umidade < 70) {
        digitalWrite(pinRele, HIGH);
      }
      else {
        digitalWrite(pinRele, LOW);
      }
    }
    else {
      digitalWrite(pinRele, LOW);
    }
  }
  
  // nivel de agua
  if (porcentagem_reservatorio < 25) {
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledYellow, LOW);
    digitalWrite(ledGreen, LOW);
  }
  else if (porcentagem_reservatorio >= 25 && porcentagem_reservatorio < 75) {
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledYellow, HIGH);
    digitalWrite(ledGreen, LOW);
  }
  else {
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledYellow, HIGH);
    digitalWrite(ledGreen, HIGH);
  }
  // imprime o horario atual
  Serial.print("Horario atual: ");
  printDateTime(now);
  Serial.println();
  // imprime o valor do sensor de umidad
  Serial.print("Umidade do solo: ");
  Serial.print(sensor_umidade);
  Serial.println("%");
  // imprime o valor do sensor de nivel
  Serial.print("Nivel de agua: ");
  Serial.print(porcentagem_reservatorio); 
  Serial.println("%");
  Serial.println();

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