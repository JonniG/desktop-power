#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Wifi + Server Configuration
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
  setup_wifi(); 
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
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
}
