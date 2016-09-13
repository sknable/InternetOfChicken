/*****************************************************************

*****************************************************************/
// SPI and the pair of SFE_CC3000 include statements are required
// for using the CC300 shield as a client device.
#include <SPI.h>
#include <SFE_CC3000.h>
#include <SFE_CC3000_Client.h>
// Progmem allows us to store big strings in flash using F().
// We'll sacrifice some flash for extra DRAM space.
#include <Progmem.h>

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
char ap_ssid[] = "NETWORK_ID";                // SSID of network
char ap_password[] = "NETWORK_PASS";        // Password of network
unsigned int ap_security = WLAN_SEC_UNSEC; // Security of network
// ap_security can be any of: WLAN_SEC_UNSEC, WLAN_SEC_WEP, 
//  WLAN_SEC_WPA, or WLAN_SEC_WPA2
unsigned int timeout = 30000;             // Milliseconds
char server[] = "data.sparkfun.com";      // Remote host site

// Initialize the CC3000 objects (shield and client):
SFE_CC3000 wifi = SFE_CC3000(CC3000_INT, CC3000_EN, CC3000_CS);
SFE_CC3000_Client client = SFE_CC3000_Client(wifi);

/////////////////
// Phant Stuff //
/////////////////
const String publicKey = "6JZbNolApzF4om2l9yYK";
const String privateKey = "Ww0vPW1yrkUNDqWPV9jE";
const byte NUM_FIELDS = 3;
const String fieldNames[NUM_FIELDS] = {"light", "switch", "name"};
String fieldData[NUM_FIELDS];

//////////////////////
// Input Pins, Misc //
//////////////////////
const int triggerPin = 3;
const int lightPin = A0;
const int switchPin = 5;
String name = "Anonymouse";
boolean newName = true;

void setup()
{
  Serial.begin(115200);

  // Setup Input Pins:
  pinMode(triggerPin, INPUT_PULLUP);
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(lightPin, INPUT_PULLUP);

  // Set Up WiFi:
  setupWiFi();

  Serial.println(F("=========== Ready to Stream ==========="));
  Serial.println(F("Press the button (D3) to send an update"));
  Serial.println(F("Type your name, followed by '!' to update name"));
}

void loop()
{
  // If the trigger pin (3) goes low, send the data.
  if (!digitalRead(triggerPin))
  {
    // Gather data:
    fieldData[0] = String(analogRead(lightPin));
    fieldData[1] = String(digitalRead(switchPin));
    fieldData[2] = name;

    // Post data:
    Serial.println("Posting!");
    postData(); // the postData() function does all the work, 
                // check it out below.
    delay(1000);
  }

  // Check for a new name input:
  if (Serial.available())
  {
    char c = Serial.read();
    if (c == '!')
    {
      newName = true;
      Serial.print("Your name is ");
      Serial.println(name);
    }
    else if (newName)
    {
      newName = false;
      name = "";
      name += c;
    }
    else
    {
      name += c;
    }
  }
}

void postData()
{

  // Make a TCP connection to remote host
  if ( !client.connect(server, 80) )
  {
    // Error: 4 - Could not make a TCP connection
    Serial.println(F("Error: 4"));
  }

  // Post the data! Request should look a little something like:
  // GET /input/publicKey?private_key=privateKey&light=1024&switch=0&time=5201 HTTP/1.1\n
  // Host: data.sparkfun.com\n
  // Connection: close\n
  // \n
  client.print("GET /input/");
  client.print(publicKey);
  client.print("?private_key=");
  client.print(privateKey);
  for (int i=0; i<NUM_FIELDS; i++)
  {
    client.print("&");
    client.print(fieldNames[i]);
    client.print("=");
    client.print(fieldData[i]);
  }
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(server);
  client.println("Connection: close");
  client.println();

  while (client.connected())
  {
    if ( client.available() )
    {
      char c = client.read();
      Serial.print(c);
    }      
  }
  Serial.println();
}

void setupWiFi()
{
  ConnectionInfo connection_info;
  int i;

  // Initialize CC3000 (configure SPI communications)
  if ( wifi.init() )
  {
    Serial.println(F("CC3000 Ready!"));
  }
  else
  {
    // Error: 0 - Something went wrong during CC3000 init!
    Serial.println(F("Error: 0"));
  }

  // Connect using DHCP
  Serial.print(F("Connecting to: "));
  Serial.println(ap_ssid);
  if(!wifi.connect(ap_ssid, ap_security, ap_password, timeout))
  {
    // Error: 1 - Could not connect to AP
    Serial.println(F("Error: 1"));
  }

  // Gather connection details and print IP address
  if ( !wifi.getConnectionInfo(connection_info) ) 
  {
    // Error: 2 - Could not obtain connection details
    Serial.println(F("Error: 2"));
  }
  else
  {
    Serial.print(F("My IP: "));
    for (i = 0; i < IP_ADDR_LEN; i++)
    {
      Serial.print(connection_info.ip_address[i]);
      if ( i < IP_ADDR_LEN - 1 )
      {
        Serial.print(".");
      }
    }
    Serial.println();
  }
}
