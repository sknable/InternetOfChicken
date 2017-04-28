/*****************************************************************

 *****************************************************************/
// SPI and the pair of SFE_CC3000 include statements are required
// for using the CC300 shield as a client device.
#include <SPI.h>
#include <SFE_CC3000.h>
#include <SFE_CC3000_Client.h>
#include <Wire.h>
////////////////////////////////////
// CC3000 Shield Pins & Variables //
////////////////////////////////////
// Don't change these unless you're using a breakout board.
#define CC3000_INT      2   // Needs to be an interrupt pin (D2/D3)
#define CC3000_EN       7   // Can be any digital pin
#define CC3000_CS       10  // Preferred is pin 10 on Uno
#define IP_ADDR_LEN     4   // Length of IP address in bytes

////////////////////
// WiFi Constants //
////////////////////
char ap_ssid[] = "";                // SSID of network
char ap_password[] = "";        // Password of network
unsigned int ap_security = WLAN_SEC_WPA2; // Security of network
// ap_security can be any of: WLAN_SEC_UNSEC, WLAN_SEC_WEP, 
//  WLAN_SEC_WPA, or WLAN_SEC_WPA2
unsigned int timeout = 30000;             // Milliseconds
char server[] = "data.internetofchicken.com";      // Remote host site

// Initialize the CC3000 objects (shield and client):
SFE_CC3000 wifi = SFE_CC3000(CC3000_INT, CC3000_EN, CC3000_CS);
SFE_CC3000_Client client = SFE_CC3000_Client(wifi);

const String publicKey = "2vL1A3yowXHMaXrQzoqySzX90DV";
const String privateKey = "vyL37d2POph3BD6gbL9qTWwqRYO";
const byte NUM_FIELDS = 5;
const String fieldNames[NUM_FIELDS] =
{ "humidity", "temp","fan","door","light" };
String fieldData[NUM_FIELDS];

unsigned long lastUpdate = 0;
const unsigned long updatePeriod = 26000;

void setup()
{
	Serial.begin(9600);
	Wire.begin();        // join i2c bus (address optional for master)
	setupWiFi();

  for(int i = 0;i < NUM_FIELDS;i++)
    fieldData[i] = "0";
lastUpdate = millis();
}

void loop()
{

  if ((millis() - lastUpdate) >= updatePeriod)
  {
    Serial.println("--Start Request--");
    lastUpdate = millis();
  	Wire.requestFrom(8, 5); 
    int i = 0;
    //Forgot what max is
    byte data[256];
  	while (Wire.available())
  	{ 
  		data[i] = Wire.read();
      Serial.println(data[i]);
      i++;
  	}

    //First 2 are hum
    int hum = data[0] << 8;
    hum |= data[1];
    //Next 2 temp
    int temp = data[2] << 8;
    temp |= data[3];
    //last fan
    byte fan = data[4];
    
    fieldData[0] = String(hum); 
    fieldData[1] = String(temp); 
    fieldData[2] = String(fan); 

    Wire.requestFrom(9, 2); 
    data[0] = 0;
    data[1] = 0;
    i=0;
    while (Wire.available())
    { 
      data[i] = Wire.read();
      Serial.println(data[i]);
      i++;
    }
    fieldData[3] = data[0]; 
    fieldData[4] = data[1];
    
  	Serial.println("--Done with Request--");
    postData();
  }


}

void postData()
{

  Serial.println("Start HTTP");
	// Make a TCP connection to remote host
	if (!client.connect(server, 80))
	{
		Serial.println(F("Error: Could not make a TCP connection"));
    wifi.disconnect();
    setupWiFi();
	}
  else
  {

  	client.print("GET /input/");
    Serial.print("GET /input/");
  	client.print(publicKey);
    Serial.print(publicKey);
  	client.print("?private_key=");
  	client.print(privateKey);
    Serial.print("?private_key=");
    Serial.print(privateKey);
  	for (int i = 0; i < NUM_FIELDS; i++)
  	{
  		client.print("&");
  		client.print(fieldNames[i]);
  		client.print("=");
  		client.print(fieldData[i]);
  
      Serial.print("&");
      Serial.print(fieldNames[i]);
      Serial.print("=");
      Serial.print(fieldData[i]);
  	}
  	client.println(" HTTP/1.1");
  	client.print("Host: ");
  	client.println(server);
  	client.println("Connection: close");
  	client.println();
  
    Serial.println(" HTTP/1.1");
    Serial.print("Host: ");
    Serial.println(server);
    Serial.println("Connection: close");
    Serial.println();
  
  	while (client.connected())
  	{
  		if (client.available())
  		{
  			char c = client.read();
  			Serial.print(c);
  		}
  	}
  }
	Serial.println("End HTTP");
}

void setupWiFi()
{
	ConnectionInfo connection_info;
	int i;

	// Initialize CC3000 (configure SPI communications)
	if (wifi.init())
	{
		Serial.println(F("CC3000 Ready!"));
	}
	else
	{
		Serial.println(F("Error: Something went wrong during CC3000 init"));
	}

	// Connect using DHCP
	Serial.print(F("Connecting to: "));
	Serial.println(ap_ssid);
	if (!wifi.connect(ap_ssid, ap_security, ap_password, timeout))
	{
		Serial.println(F("Error: Could not connect to AP"));
	}

	// Gather connection details and print IP address
	if (!wifi.getConnectionInfo(connection_info))
	{
		Serial.println(F("Error: Could not obtain connection details"));
	}
	else
	{
		Serial.print(F("My IP: "));
		for (i = 0; i < IP_ADDR_LEN; i++)
		{
			Serial.print(connection_info.ip_address[i]);
			if (i < IP_ADDR_LEN - 1)
			{
				Serial.print(".");
			}
		}
		Serial.println();
	}
}
