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
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


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
// Definindo os tamanhos da tela OLED 128x64
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// instanciando o display oled 128x64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// rtc para o horário
ThreeWire myWire(data_rtc, clock_rtc, rst_rtc);
RtcDS1302<ThreeWire> Rtc(myWire);

// assinatura de funções
void printDate(const RtcDateTime& dt, char* buffer);
void printTime(const RtcDateTime& dt, char* buffer);

// Potenciômetro
int valor_potenciometro = 0;
// LED RGB
RGBLed led(red_pin, green_pin, blue_pin, RGBLed::COMMON_ANODE);
// Sensor HC-SR04
Ultrasonic sensorNivel(pinTrigger, pinEcho);
int porcentagem_reservatorio = 0;
// Sensor de umidade
int sensor_umidade = 0;


void setup() {
  Serial.begin(115200);
  pinMode(pinRele, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledRed, OUTPUT);

  // Inicializa o RTC
  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  if (!Rtc.IsDateTimeValid()) {
    Serial.println("RTC lost confidence in the DateTime!");
    Rtc.SetDateTime(compiled);
  }

  // Inicializa o display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.display();
}

void loop() {
  valor_potenciometro = analogRead(pinPotenciometro);
  valor_potenciometro = map(valor_potenciometro, 0, 4095, 0, 100);
  porcentagem_reservatorio = map(sensorNivel.read(), 10, 2, 0, 100);
  sensor_umidade = analogRead(pinSensor);
  sensor_umidade = map(sensor_umidade, 0, 4095, 0, 100);

  // Obtém o horário atual
  RtcDateTime now = Rtc.GetDateTime();

  // Modos de irrigação
  if (valor_potenciometro < 33) {
    Serial.println("Irrigacao desligada");
    digitalWrite(pinRele, LOW);
    led.setColor(255, 0, 0);
  } else if (valor_potenciometro >= 33 && valor_potenciometro < 66) {
    Serial.println("Modo manual...");
    digitalWrite(pinRele, HIGH);
    led.setColor(0, 255, 0);
  } else {
    led.setColor(0, 0, 255);
    // irriga quando o horario estiver entre 07:00 e 17:00 e a umidade do solo estiver menor que 70%
    if ((now.Hour() >= 07 && now.Hour() <= 17) && sensor_umidade < 70) {
      digitalWrite(pinRele, HIGH);
    } else {
      digitalWrite(pinRele, LOW);
    }
  }

  // Nível de água
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

  // Atualiza o display OLED
  display.clearDisplay();
  display.setFont(NULL);
  display.setTextSize(1);
  display.setTextColor(WHITE);

  char dateBuffer[12];
  char timeBuffer[9];
  
  printDate(now, dateBuffer);
  display.setCursor(0, 0);
  display.print("Data: ");
  display.println(dateBuffer);

  printTime(now, timeBuffer);
  display.setCursor(0, 10);
  display.print("Hora: ");
  display.println(timeBuffer);

  display.setCursor(0, 20);
  display.print("Umidade: ");
  display.print(sensor_umidade);
  display.println("%");

  display.setCursor(0, 30); 
  display.print("Nivel Agua: ");
  display.print(porcentagem_reservatorio);
  display.println("%");

  display.display();

  delay(1000);
}

// Função que imprime a data atual em um buffer no formato dd/mm/aaaa 
void printDate(const RtcDateTime& dt, char* buffer) {
  snprintf_P(buffer, 12,
             PSTR("%02u/%02u/%04u"),
             dt.Day(),
             dt.Month(),
             dt.Year());
}

// Função que imprime a hora atual em um buffer no formato hh:mm:ss
void printTime(const RtcDateTime& dt, char* buffer) {
  snprintf_P(buffer, 12,
             PSTR("%02u:%02u:%02u"),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
}