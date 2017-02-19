//
// This sketch uses an ESP8266
// The value are sent as feeds to an Adafruit IO account via Adafruit MQTT publish calls
//
// To save power, deep sleep from the ESP8266 library is used
// To wake up the board a signal on pin 16 is sent to the reset input
// (so you need a wire between D16 and the reset pin)
// Note! The connection D16 -> Reset has to be removed when uploading the sketch!
// All activity happens in the setup() method as the board is reset after sleeping
//

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WLAN_SSID  "RileyGuest"
#define WLAN_PASS  "Widget12"

// Configure Adafruit IO access.
#define AIO_USERNAME "ptbw2000"
#define AIO_KEY    "47595f08228e46da8e624b1d4f127851"
//
// Adafruit IO setup
//
#define AIO_SERVER      "io.adafruit.com"
//#define AIO_SERVERPORT  8883 // Use SSL
#define AIO_SERVERPORT  1883 // No SSL


WiFiClient client;      // No SSL
//WiFiClientSecure client;  // Use SSL

// Store the MQTT server, username, and password in flash memory.
// This is required for usi#define ng the Adafruit MQTT library.
const char MQTT_SERVER[] PROGMEM    = AIO_SERVER;
const char MQTT_USERNAME[] PROGMEM  = AIO_USERNAME;
const char MQTT_PASSWORD[] PROGMEM  = AIO_KEY;

Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_USERNAME, MQTT_PASSWORD);

const char COLOUR_FEED[] PROGMEM = AIO_USERNAME "/feeds/frontdoor";
Adafruit_MQTT_Publish colourPub = Adafruit_MQTT_Publish(&mqtt, COLOUR_FEED);

void MQTT_connect() 
{
  if (mqtt.connected()) 
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  int8_t ret;
  while ((ret = mqtt.connect()) != 0) 
  { 
    // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);
  }
  Serial.println("MQTT Connected!");
}

unsigned int getColour( )
{ 
    return random(0xFFFFFF);
}

void sendDataToAdafruitIO()
{
  unsigned long startTime = millis();

  String colour = "#" + String(getColour(),HEX);  
  
  Serial.println("Start sending data to Adafruit IO...");

  MQTT_connect();

  if (colourPub.publish(colour.c_str()))
  {
    Serial.println(colour);
    Serial.println("Sent colour ok");
  }
  else
  {
    Serial.println("Failed sending colour");
  }
  
  unsigned long endTime = millis();

  Serial.print("Sending data took ");
  Serial.print((endTime-startTime)/1000.0);
  Serial.println(" second(s)");
}

void setupWiFi()
{
  unsigned long startTime = millis();

  // Setup serial port access.
  Serial.begin(115200);

  // Connect to WiFi access point.
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  unsigned long endTime = millis();

  Serial.print("Setting up WiFi took ");
  Serial.print((endTime-startTime)/1000.0);
  Serial.println(" second(s)");

}

void setup() 
{
  setupWiFi();
  
//  // Put the board to deep sleep to save power. Will send a signal on D16 when it is time to wake up.
//  // Thus, connect D16 to the reset pin. After reset, setup will be executed again.
//  Serial.print("Going to deep sleep for ");
//  Serial.print(SLEEP_SECONDS);
//  Serial.println(" seconds");
//  ESP.deepSleep(SLEEP_SECONDS * 1000000);
}

void loop() 
{
  sendDataToAdafruitIO();
  delay(1000);
}
