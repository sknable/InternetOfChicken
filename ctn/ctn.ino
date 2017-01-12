#include "DHT.h"
#include <SoftwareSerial.h>
#include <serLCD.h>
#include <Wire.h>
#define DHTPIN 2     // what digital pin we're connected to
#define SERPIN 7
#define FANPIN 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
serLCD lcd(SERPIN);

float humidity = 0;
float tempature = 0;
const unsigned long sensotTogglePeriod = 2600;
unsigned long lastCheck = 0;
const float maxHumidity = 45.0;

void setup() {
  Serial.begin(9600);
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event
  pinMode(FANPIN, OUTPUT); 
  dht.begin();
  lastCheck = millis();
  setVersion();

  
}
void setVersion()
{
  lcd.clear(); 
  lcd.print("Chicken Coop");
  lcd.selectLine(2);
  lcd.print("V1.1  10/14/2016");
  lcd.setSplash(); 
}
void printTemp()
{
  lcd.clear(); 
  lcd.print(tempature);
  lcd.print(" F");
  lcd.selectLine(2);
  lcd.print(humidity);
  lcd.print(" %");
  
}

void loop() 
{

    if((millis() - lastCheck) >= sensotTogglePeriod)
    {
        lastCheck = millis();
        readTemp();
        printTemp();                                          
        fanControl();
    }

}
void fanControl()
{
  if(maxHumidity <= humidity)
  {
    Serial.println("Fan On");
    analogWrite(FANPIN, 255); 
  }
  else
  {
    Serial.println("Fan Off");
    analogWrite(FANPIN, 0);
  }
}
void readTemp()
{
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
   humidity = dht.readHumidity();
  // Read temperature as Fahrenheit (isFahrenheit = true)
   tempature = dht.readTemperature(true);

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.println(tempature);
}
void receiveEvent(int howMany) {
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}
void requestEvent() {
  String temp = String(humidity,1);
  Wire.write(temp.c_str()); // respond with message of 6 bytes
  // as expected by master
}

