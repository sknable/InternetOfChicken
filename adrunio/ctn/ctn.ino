#include "DHT.h"
#include <SoftwareSerial.h>
#include "serLCD.h"
#include <Wire.h>
#include <OneWire.h> 
#define DHTPIN 2     // what digital pin we're connected to
#define SERPIN 7
#define FANPIN 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define DS18S20 4 //DS18S20 Signal pin on digital 2

//Temperature chip i/o
OneWire ds(DS18S20); // on digital pin 2
DHT dht(DHTPIN, DHTTYPE);
serLCD lcd(SERPIN);

float humidity = 0;
float tempature = 0;
float water_temp = 0;
int methane = 0;
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
    water_temp = getTemp(); //will take about 750ms to run
    Serial.print("Water Temp: ");
    Serial.println(water_temp);
    methane = analogRead(0);       // read analog input pin 0
    Serial.print("Methane: ");
    Serial.println(methane, DEC);  // prints the value read
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

    byte data[9];

  data[0] = ((int)humidity & 0xff00) >> 8;
  data[1] = (int)humidity & 0xff;

  data[2] = ((int)tempature & 0xff00) >> 8;
  data[3] = (int)tempature & 0xff;

  data[4] = fan;

  data[5] = ((int)water_temp & 0xff00) >> 8;
  data[6] = (int)water_temp & 0xff;


  data[7] = ((int)methane & 0xff00) >> 8;
  data[8] = (int)methane & 0xff;
	
  Wire.write(data,9);
}
float getTemp(){
 //returns the temperature from one DS18S20 in DEG Celsius

 byte data[12];
 byte addr[8];

 if ( !ds.search(addr)) {
   //no more sensors on chain, reset search
   Serial.println("CANT FIND!");
   ds.reset_search();
   return -1000;
 }

 if ( OneWire::crc8( addr, 7) != addr[7]) {
   Serial.println("CRC is not valid!");
   return -1000;
 }

 if ( addr[0] != 0x10 && addr[0] != 0x28) {
   Serial.print("Device is not recognized");
   return -1000;
 }

 ds.reset();
 ds.select(addr);
 ds.write(0x44,1); // start conversion, with parasite power on at the end
 
 delay(750); // Wait for temperature conversion to complete

 byte present = ds.reset();
 ds.select(addr);  
 ds.write(0xBE); // Read Scratchpad

 
 for (int i = 0; i < 9; i++) { // we need 9 bytes
  data[i] = ds.read();
 }
 
 ds.reset_search();
 
 byte MSB = data[1];
 byte LSB = data[0];

 float tempRead = ((MSB << 8) | LSB); //using two's compliment
 float TemperatureSum = tempRead / 16;
 
 return ((int)round(1.8*TemperatureSum+32));
 
}
