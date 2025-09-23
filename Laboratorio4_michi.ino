#include <WiFi.h>  // Librería para ESP32
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/
#define WLAN_SSID "ARTEFACTOS"
#define WLAN_PASS "ARTEFACTOS"

/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883 // usar 8883 para SSL
#define AIO_USERNAME "MichiiMltz"
#define AIO_KEY "aio_ptpe70Pxm6RPwTbQskWngNbflkhO"

/************ Global State ******************/
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

/****************************** Feed para publicar ***************************************/
Adafruit_MQTT_Publish ldrFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/prueba-labo");

/*************************** Pines y variables globales ***********************************/
const int ldrPin = 34; // GPIO34 es analógico solo lectura (ideal para LDR)

/*************************** Setup ***********************************/
void setup()
{
  Serial.begin(115200);
  delay(10);

  pinMode(ldrPin, INPUT);

  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

/*************************** Loop principal ***********************************/
void loop()
{
  MQTT_connect(); // Asegura conexión MQTT

  mqtt.processPackets(10000);

  if (!mqtt.ping())
  {
    mqtt.disconnect();
  }

  int ldrValue = analogRead(ldrPin); // 0 - 4095
  Serial.print("Valor de luz: ");
  Serial.println(ldrValue);

  // Publicar en Adafruit IO
  if (!ldrFeed.publish((int32_t)ldrValue)) {
  Serial.println("Error al publicar el valor LDR.");
}


  delay(2000);
}

/*************************** Conexión a MQTT ***********************************/
void MQTT_connect()
{
  int8_t ret;

  if (mqtt.connected())
  {
    return;
  }

  Serial.print("Conectando a MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0)
  {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Reintentando en 10 segundos...");
    mqtt.disconnect();
    delay(10000);
    retries--;
    if (retries == 0)
    {
      while (1)
        ;
    }
  }
  Serial.println("¡MQTT conectado!");
}
