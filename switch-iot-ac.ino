#include "config.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
const char *clientId = WIFI_HOSTNAME;

const char *mqtt_server = MQTT_HOST;
const unsigned int mqtt_port = MQTT_PORT;

const char *mqtt_user = MQTT_USER;
const char *mqtt_pass = MQTT_PASS;
const char *mqtt_topic = MQTT_TOPIC;
const char *mqtt_message = MQTT_MSG;

WiFiClient espClient;
PubSubClient client(espClient);

bool pressed = false;

void setWorking(bool on)
{
  digitalWrite(LED_BUILTIN, on ? LOW : HIGH);
}

void wifi()
{
  while (WiFi.status() != WL_CONNECTED) // hold till is online
  {
    setWorking(true);
    delay(500); // check
    setWorking(false);

    if (WiFi.status() == WL_CONNECT_FAILED)
    {
      ESP.deepSleep(0); // could not connect to wifi > go to sleep forever
      return;
    }
  }
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PRESS_PIN, INPUT);

  WiFi.mode(WIFI_STA);
  WiFi.hostname(WIFI_HOSTNAME);
  WiFi.begin(ssid, password);

  client.setServer(mqtt_server, mqtt_port);
}

void loop()
{
  wifi();
  setWorking(pressed);

  if (!client.connected() && !client.connect(clientId, mqtt_user, mqtt_pass))
  {
    delay(1000);
    return;
  }

  if (pressed && client.publish(mqtt_topic, mqtt_message))
  {
    delay(1500); // give time to release
    pressed = false;
  }

  if (digitalRead(PRESS_PIN) == LOW)
  {
    pressed = true;
  }

  client.loop();
}
