// *********************************************
// ALAA'S ADAFRUIT DASHBOARD 
// https://io.adafruit.com/alaa_amed/dashboards

// *********************************************

// Program for collecting humidity and temperature information from DHT22 sensor and barometric pressure from MPL115A2-I2C sensor.
// The information is displayed on the OLED module. 

#include "config.h"

/************************ Example Starts Here *******************************/
// Libraries that need to be installed & included
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <Adafruit_MPL115A2.h>

Adafruit_MPL115A2 mpl115a2;


// pin connected to DH22 data line
#define DATA_PIN 12
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// create DHT22 instance
DHT_Unified dht(DATA_PIN, DHT22);

// set up the 'temperature' and 'humidity' feeds
AdafruitIO_Feed *temperature = io.feed("temperature");
AdafruitIO_Feed *humidity = io.feed("humidity");

void setup() {

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  mpl115a2.begin();
  display.display();
  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  // initialize dht22
  dht.begin();

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();


}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  sensors_event_t event;
  dht.temperature().getEvent(&event);

  float celsius = event.temperature;
  float fahrenheit = (celsius * 1.8) + 32;
  float pressureKPA = 0, temperatureC = 0;  

  Serial.print("celsius: ");
  Serial.print(celsius);
  Serial.println("C");

  Serial.print("fahrenheit: ");
  Serial.print(fahrenheit);
  Serial.println("F");

  // save fahrenheit (or celsius) to Adafruit IO
  temperature->save(fahrenheit);
  
  dht.humidity().getEvent(&event);

  Serial.print("humidity: ");
  Serial.print(event.relative_humidity);
  pressureKPA = mpl115a2.getPressure();  
  Serial.print("Pressure (kPa): "); Serial.print(pressureKPA, 4); Serial.println(" kPa");
  
  // Cast all float values into string so it can pe printed
  String hum = String(event.relative_humidity);
  String temp = String(celsius); 
  String pressure = String(pressureKPA);
  // Clear the screen
  display.clearDisplay();
  // Normal 1:1 pixel scale
  display.setTextSize(1);
  // Start at top-left corner
  display.setCursor(0, 0);
  // Draw white text
  display.setTextColor(WHITE);
  // Display Humidity on OLED
  display.println("Humidity: " + hum);
  // Display Temperature on OLED
  display.println("Temperature: " + temp + " C");
  // Display Pressure on OLED
  display.println("Pressure: " + pressure + " kPa");
  // Update the screen-- without this command, nothing will be pushed/displayed 
  display.display();
  // wait 2 seconds (2000 milliseconds == 2 seconds)
  delay(2000);
  display.clearDisplay();
  Serial.println("%");

  // Save humidity to Adafruit IO
  humidity->save(event.relative_humidity);

  // wait 1 second (1000 milliseconds == 1 second)
  delay(1000);

}
