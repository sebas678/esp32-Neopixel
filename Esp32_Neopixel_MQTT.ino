#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <MQTT.h>

const char ssid[] = "*****";       //Nombre de la red
const char pass[] = "*****";       //Pass de la red

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;
#define PIN 23                     // Define Pin al que está conectado Neopixel

Adafruit_NeoPixel strip = Adafruit_NeoPixel(10, PIN, NEO_GRB + NEO_KHZ800);
//Primer parametro    Numero de leds
//Segudo parametro    Pin al que estan conectados
//Tercer Parametro    Definidos por la libreria

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("Cosa", "6a7d6803", "42b5031ae34d08e9")) {
        //Nombre para el broker mqtt con sus identificadores
        
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/color");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("recibiendo: " + topic + " - " + payload);
  //Recibe un RGB en formato texto plano y lo muestra para edición
  String colores = payload;
  colores.length();
  colores.remove(0, 3);
  String color1 = colores;
  String color2 = colores;
  String color3 = colores;

  String r = colores.substring(1, (color1.indexOf(",")));
  Serial.println(r);
  colores.remove(0, (color1.indexOf(",")));
  colores.remove(0, 2);
  String g = colores.substring(0, (colores.indexOf(",")));
  Serial.println(g);
  colores.remove(0, (colores.indexOf(" ") + 1));
  String b = (colores.substring(0, (colores.indexOf(")"))));
  Serial.println(b);

  colorWipe(strip.Color(r.toInt(), g.toInt(), b.toInt()), 50);
  //Asigna a la funcion los parametros RGB recibidos y separados por color
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  client.begin("broker.shiftr.io", net);
  client.onMessage(messageReceived);
  connect();

  strip.begin();
  strip.show();
}

void loop() {
  client.loop();
  delay(10);
  if (!client.connected()) {
    connect();
  }
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    client.publish("/color", "");
  }
}
