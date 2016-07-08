#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//OTA Config
const char* deviceName = "ESP8266-Desktop-Power";
const char* OTAPass = "update";

// Wifi network details and MQTT broker IP
const char* ssid = "*****";
const char* password = "*****";
const char* mqtt_server = "*****";

WiFiClient espClient;
PubSubClient client(espClient);
int powerPin = 14;
int statusLED = 0;
String switch1;
String strTopic;
String strPayload;

void setup_wifi() {
  delay(10);
  // Connect to Wifi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  strTopic = String((char*)topic);
  if(strTopic == "desktop/power")
    {
    switch1 = String((char*)payload);
    if(switch1 == "ON")
      {
        Serial.println("ON");
        digitalWrite(powerPin, LOW);
        digitalWrite(statusLED, LOW);
        delay(500);
        digitalWrite(powerPin, HIGH);
        digitalWrite(statusLED, HIGH);
      }
    else
      {
        Serial.println("OFF");
        digitalWrite(powerPin, LOW);
        digitalWrite(statusLED, LOW);
        delay(500);
        digitalWrite(powerPin, HIGH);
        digitalWrite(statusLED, HIGH);
      }
    }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Desktop-Power")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe("desktop/power");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
void setup()
{
  Serial.begin(115200);

  //OTA Setup
  WiFi.mode(WIFI_STA);
  ArduinoOTA.setPassword(OTAPass);
  ArduinoOTA.setHostname(deviceName);
  
  setup_wifi(); 
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  
  Serial.println("READY");
  
  pinMode(statusLED, OUTPUT);
  digitalWrite(statusLED, HIGH);
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);
}
 
void loop()
{
  if (WiFi.status() != WL_CONNECTED) {
	setup_wifi();
  }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  ArduinoOTA.handle();
}
