#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <SPI.h>
#include <LoRa.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
// define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26
#define BAND 433E6
// OLED pins
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
float temperature = 0;
float humidity = 0;
float co2 = 0;
float lux = 0;
String b1, b2, b3, b4, b5, b6, b7, b8, b9, b10;
int b11, b21, b31, b41, b51;
// Main Functions
void setup()
{
    pinMode(25, OUTPUT); // Send success, LED W
    pinMode(16, OUTPUT);
    digitalWrite(16, LOW); // set GPI016 low
    delay(50);
    digitalWrite(16, HIGH);
    // initialize Serial Monitor
    Serial.begin(115200);

    // reset OLED display via software
    pinMode(OLED_RST, OUTPUT);
    digitalWrite(OLED_RST, LOW);
    delay(20);
    digitalWrite(OLED_RST, HIGH);

    // initialize OLED
    Wire.begin(OLED_SDA, OLED_SCL);
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false))
    { // Address 0x3C for 128x32
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }

    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(5, 5);
    display.print("LORA RECEIVER ");
    display.display();

    Serial.println("LoRa Receiver Test");

    // SPI LoRa pins
    SPI.begin(SCK, MISO, MOSI, SS);
    // setup LoRa transceiver module
    LoRa.setPins(SS, RST, DIO0);

    if (!LoRa.begin(BAND))
    {
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }
    Serial.println("LoRa Initializing OK!");
    display.setCursor(0, 20);
    display.println("LoRa Initializing OK!");
    display.display();
    Serial.println("CLEARDATA");
    Serial.println("LABEL, Date, Time, Temperature, Humidity, co2, lux");
}

int a = 0;

void loop()
{
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
        // received a packets // Serial.print("Received packet. ");
        display.clearDisplay();
        // display.setFont(ArialMT_Plain_10);
        display.display();

        
        // read packet
        while (LoRa.available())
        {
            String a = LoRa.readString();
            int a1 = a.indexOf("a");
            int a2 = a.indexOf("b");
            int a3 = a.indexOf("c");
            int a4 = a.indexOf("d");
            int a5 = a.indexOf("e");
            int a6 = a.indexOf("f");
            int a7 = a.indexOf("g");
            int a8 = a.indexOf("h");
            int a9 = a.indexOf("i");
            int a10 = a.indexOf("j");
            int a11 = a.indexOf("k");
            int a12 = a.indexOf("l");
            int a13 = a.indexOf("m");
            int a14 = a.indexOf("n");
            int a15 = a.indexOf("o");
            int a16 = a.indexOf("p");
            int a17 = a.indexOf("q");
            int a18 = a.indexOf("r");
            int a19 = a.indexOf("s");
            int a20 = a.indexOf("t");

            b1 = a.substring(a1 + 1, a2);
            b2 = a.substring(a3 + 1, a4);
            b3 = a.substring(a5 + 1, a6);
            b4 = a.substring(a7 + 1, a8);
            b5 = a.substring(a9 + 1, a10);
            b6 = a.substring(a11 + 1, a12);
            b7 = a.substring(a13 + 1, a14);
            b8 = a.substring(a15 + 1, a16);
            b9 = a.substring(a17 + 1, a18);
            b10 = a.substring(a19 + 1, a20);

            Serial.print("DATA, DATE, TIME,");
            Serial.print(b1);
            Serial.print(",");
            Serial.print(b2);
            Serial.print(",");
            Serial.print(b3);
            Serial.print(",");
            Serial.print(b4);
            Serial.println(".");
            Serial.print(b5);
            Serial.print(".");

            display.setCursor(5, 0);
//            display.print("Date: " + String(b5));
             display.print("Date: 01/12/2022");
            display.setCursor(5, 10);
            display.print("Light: " + String(b4) + " lux");
//            display.print("Time: "+ String(b6));  
//            display.print("Time: 01:30 PM");
            display.setCursor(5, 20);
            display.print("Temperature: " + String(b1) + " C");
            display.setCursor(5, 30);
            display.print("Humidity: " + String(b2) + " RH");
            display.setCursor(5, 40);
            display.print("CO2: " + String(b3) + " PPM");
            
            if(b5 == "1.00")
            {
            display.setCursor(5, 50);
//            display.print("Light: " + String(b4) + " lux");
              display.print("Cooling Fan ON");
            }
            else
            {
              display.setCursor(5, 50);
              display.print("Cooling Fan OFF");
            }
        }
    }
    delay(10);
}
