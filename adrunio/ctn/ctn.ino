#include "DHT.h"
#include <SoftwareSerial.h>
#include "serLCD.h"
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
const float maxTemp = 75.0;
byte fan = 0;
void setup()
{

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
	lcd.print("V1.2  4/26/2017");
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

	if ((millis() - lastCheck) >= sensotTogglePeriod)
	{
		lastCheck = millis();
		readTemp();
		printTemp();
		fanControl();
	}

}
void fanControl()
{
	if (maxHumidity <= humidity || maxTemp <= tempature)
	{
		Serial.println("Fan On");
		analogWrite(FANPIN, 255);
    fan = 1;
	}
	else
	{
		Serial.println("Fan Off");
		analogWrite(FANPIN, 0);
    fan = 0;
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
void receiveEvent(int howMany)
{
	while (1 < Wire.available())
	{ // loop through all but the last
		char c = Wire.read(); // receive byte as a character
		Serial.print(c);         // print the character
	}
	int x = Wire.read();    // receive byte as an integer
	Serial.println(x);         // print the integer
}
void requestEvent()
{

    byte data[5];

  data[0] = ((int)humidity & 0xff00) >> 8;
  data[1] = (int)humidity & 0xff;

  data[2] = ((int)tempature & 0xff00) >> 8;
  data[3] = (int)tempature & 0xff;

  data[4] = fan;
	
  Wire.write(data,5);
}

