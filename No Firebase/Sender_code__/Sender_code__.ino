#include <LoRa.h>
#include <Wire.h>
#include <SPI.h>
#include "DFRobot_SHT20.h"
#include <BH1750.h>
#include "MQ135.h"
#include <Arduino.h>

#define SS 18
#define RST 14
#define DIO 26
#define BAND 433E6  //91556

DFRobot_SHT20 sht20;
BH1750 lightMeter;

float temp = 0;
float humd = 0;
float pressure = 0;
float lux = 0;
String sensor_data = "";
const int relay32 = 32;
const int relay33 = 33;
const int relay13 = 13;
const int relay23 = 23;
const int s1 = 15;
const int mq = 4;
int s = 0;

void setup()
{
  Serial.begin(115200);
  pinMode(mq, INPUT);
  pinMode(s, INPUT);
  pinMode(25, OUTPUT); //Send success, LED will bright 1 second
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);
  delay(50);
  digitalWrite(16, HIGH);
  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DIO);
  Serial.println("LoRa Sender");
  if (!LoRa.begin(BAND)) {
    Serial.println("starting LoRa failed!");
  }
  Serial.println("LORA Initial OK!");
  delay(500);
  sht20.initSHT20();
  delay(100);
  sht20.checkSHT20();
  pinMode(relay32, OUTPUT);
  pinMode(relay33, OUTPUT);
  pinMode(relay13, OUTPUT);
  pinMode(relay23, OUTPUT);
  delay(500);
  Wire.begin();
  lightMeter.begin();
  delay(500);
}

void loop()
{
  LoRa.beginPacket();
   
  MQ135 gasSensor = MQ135(mq);

  float rzero = gasSensor.getRZero();
  Serial.print("C02=");
  Serial.print(rzero);
  Serial.println(" PPM");
  delay(500);

  float humd = sht20.readHumidity();
  float temp = sht20.readTemperature();
  Serial.print("SHT20 humidity: ");
  Serial.println(humd);
  delay(1000);
  Serial.print("SHT20 temp: ");
  Serial.println(temp);
  delay(1000);

  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  delay(500);

  String sensor_data = 'a' + String(temp) + 'b' + 'c' + String(humd) + 'd' + 'e' + String(rzero) + 'f' + 'g' + String(lux) + 'h' + 'i' + String(s) + 'j';
  Serial.print(sensor_data);
  LoRa.print(sensor_data);
  LoRa.endPacket();

if (temp >= 30)
  {
    s = 1;
    digitalWrite(relay23, LOW);
    Serial.println("RELAY 23 on");
    Serial.println("RELAY 23 Cooling Fan on");
//    digitalWrite(relay33, HIGH);
//    Serial.println("RELAY 33 off");
//    Serial.println("RELAY 33 buzzer off");
//    digitalWrite(relay13, HIGH);
//    Serial.println("RELAY 13 off");
//    Serial.println("RELAY 13 water spray off");
//    digitalWrite(relay32, HIGH);
//    Serial.println("RELAY 32 off");
//    Serial.println("RELAY 32 heater off");
  }
  else if(temp < 30)
  {
    digitalWrite(relay23, HIGH);
    //    Serial.println("RELAY 23 off");
    Serial.println("RELAY 23 Cooling Fan off");
  }

//  if ((humd <= 85))
  if (0)
  {
    digitalWrite(relay13, LOW);
    Serial.println("RELAY 13 on");
    Serial.println("RELAY 13 water spray on");
    digitalWrite(relay23, HIGH);
    Serial.println("RELAY 23 off");
    Serial.println("RELAY 23 cooling fan off");
    digitalWrite(relay33, HIGH);
    Serial.println("RELAY 33 off");
    Serial.println("RELAY 33 buzzer off");
    digitalWrite(relay32, HIGH);
    Serial.println("RELAY 32 off");
    Serial.println("RELAY 32 heater off");
    delay(10000);
  }
  else if (0)
  {
    digitalWrite(relay13, HIGH);
    //    Serial.println("RELAY 13 off");
    Serial.println("RELAY 13 Water Sprayer off");
  }
  Serial.println("--------------------------------------------------");
}
