#include <Arduino.h>
#include <LoRa.h> 
#include <Wire.h>
#include <SPI.h>
#include "DFRobot_SHT20.h"
#include <BH1750.h>
#include "MQ135.h"
#include "RTClib.h"

#include <Firebase_ESP_Client.h>
#include <WiFi.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "R&D-LABS"
#define WIFI_PASSWORD "12345678"

#define API_KEY "AIzaSyAWNvTNgxNiLlOhPAyx7WkZvLyMztt4hGo"
#define DATABASE_URL "https://fir-38924-default-rtdb.firebaseio.com/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

#define SS 18
#define RST 14
#define DIO 26
#define BAND 433E6  //91556

DFRobot_SHT20 sht20;
BH1750 lightMeter;

RTC_DS1307 rtc;

float temp = 0;
float humd = 0;
float pressure = 0;
float lux = 0;
String sensor_data = "";
const int relay32 = 32;
const int relay33 = 33;
const int relay13 = 13;
const int relay23 = 23;
const int mq = 4;
const int s1 = 15;
int s = 0;
float Co2 = 0.0;
int counter = 0;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup()
{
      // rtc.adjust(DateTime(2022,8,18,3,40,0));
  Serial.begin(115200);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  pinMode(mq, INPUT);
  pinMode(s, INPUT);
  pinMode(25, OUTPUT); // Send success, LED will bright 1 second
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);
  delay(50);
  digitalWrite(16, HIGH);
  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DIO);
  Serial.println("LoRa Sender");
  if (!LoRa.begin(BAND))
  {
    Serial.println("starting LoRa failed!");
    while (1)
      ;
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
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // rtc.adjust(DateTime(2022,8,18,3,38,0));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  lightMeter.begin();
  delay(500);
}

void loop()
{
  LoRa.beginPacket();
  DateTime now = rtc.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  String date_string = String(now.day()) + '/' + String(now.month())+'/'+String(now.year());
  String time_string = String(now.twelveHour())+':'+ String(now.minute())+' ';
  delay(500);

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

  String sensor_data = 'a' + String(temp) + 'b' + 'c' + String(humd) + 'd' + 'e' + String(rzero) + 'f' + 'g' + String(lux) + 'h'+'i'+date_string+'j'+'k'+time_string+'l';
  Serial.print(sensor_data);
  LoRa.println(sensor_data);
  LoRa.endPacket();

  if (temp >= 30)
  {
    digitalWrite(relay23, LOW);
    Serial.println("RELAY 23 on");
    Serial.println("RELAY 23 Cooling Fan on");
    digitalWrite(relay33, HIGH);
    Serial.println("RELAY 33 off");
    Serial.println("RELAY 33 buzzer off");
    digitalWrite(relay13, HIGH);
    Serial.println("RELAY 13 off");
    Serial.println("RELAY 13 water spray off");
    digitalWrite(relay32, HIGH);
    Serial.println("RELAY 32 off");
    Serial.println("RELAY 32 heater off");
  }
  else
  {
    digitalWrite(relay23, HIGH);
    Serial.println("RELAY 23 off");
    Serial.println("RELAY 23 cooling fan off");
  }

  if ((humd <= 85) && (now.hour() == 10) && (now.minute() == 01))
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
  else
  {
    digitalWrite(relay13, HIGH);
    Serial.println("RELAY 13 off");
    Serial.println("RELAY 13 water spray off");
  }

  int s = digitalRead(s1); // read analog input pino
  Serial.print("switchs=");
  Serial.println(s); // prints the value read
  while (s == 0)
  {
    counter++;
    Serial.print("counterll=");
    Serial.println(counter);
    digitalWrite(relay32, LOW); // prints the value
    Serial.println("RELAY 32 on");
    Serial.println("RELAY 32 heater on");
    digitalWrite(relay23, HIGH);
    Serial.println("RELAY 23 off");
    Serial.println("RELAY 23 cooling fan off");
    digitalWrite(relay33, HIGH);
    Serial.println("RELAY 33 off");
    Serial.println("RELAY 33 buzzer off");
    digitalWrite(relay13, HIGH);
    Serial.println("RELAY 13 off");
    Serial.println("RELAY 13 water spray off");
    delay(10000);
  }

  while (counter >= 655)
  {
    digitalWrite(relay32, HIGH);
    Serial.print("counter1=");
    Serial.println(counter);
    Serial.println("RELAY 32 off");
    Serial.println("RELAY 32 heater off");
    digitalWrite(relay23, HIGH);
    Serial.println("RELAY 23 off");
    Serial.println("RELAY 23 cooling fan off");
    digitalWrite(relay13, HIGH);
    Serial.println("RELAY 13 off");
    Serial.println("RELAY 13 water spray off");
    digitalWrite(relay33, LOW);
    Serial.println("RELAY 33 on");
    Serial.println("RELAY 33 buzzer on");
    int s = digitalRead(s1); // read analog input pin 0
    Serial.print(" inside switchs");
    Serial.println(s); // prints the value read i

    if (s == 0)
    {
      counter = 0;
      break;
    }
    delay(1000);
  }
    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
          sendDataPrevMillis = millis();
          if(Firebase.RTDB.setFloat(&fbdo,"test/temp",temp)){Serial.print("passed");}
          else{
            Serial.print("failed");
          }
          if(Firebase.RTDB.setFloat(&fbdo,"test/humidity",humd)){}
          if(Firebase.RTDB.setFloat(&fbdo,"test/lux",lux)){}
          if(Firebase.RTDB.setString(&fbdo,"test/date",date_string)){}
          if(Firebase.RTDB.setString(&fbdo,"test/time",time_string)){}
          if(Firebase.RTDB.setString(&fbdo,"test/Co2",rzero)){}
    }
}
