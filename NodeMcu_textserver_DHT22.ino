/**
 * DOCS:
 *    https://github.com/esp8266/Arduino/blob/master/doc/reference.md
 *    https://www.descurcareste.ro/tehnologie/server-web-cu-nodemcuesp8266-si-dht11-in-arduino-ide-308
 */
 
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

const char* ssid = "SSID";
const char* password = "pasword";

ESP8266WebServer server(80);

#include "DHT.h"
#define DHTPIN 5     // what pin we're connected the DHT output
// note 5 is GPIO5 at NodelMCU (or D1) - http://www.14core.com/wp-content/uploads/2015/06/Node-MCU-Pin-Out-Diagram1.png
//#define DHTTYPE DHT11   // DHT 11 
#define DHTTYPE DHT22   // DHT 22 
DHT dht(DHTPIN, DHTTYPE,11);

int citire = 0;   // for read sensor with pause
long int taim5 = 0;
float te;
int te1,te2,has;

String txt;

String temp_txt = "";

void handleRoot()
{
//  server.send(200, "text/plain", get_temp_humidity_string());
te = 10*dht.readTemperature();
te1 = te/10;
te2 = te - 10*te1;
has = dht.readHumidity();
Serial.print("h = ");
Serial.println(has);
Serial.print("t = ");
Serial.println(te);
      txt = "T: " + String(te1) + "," + String(te2) + "gr.C ";
      txt += " H: " + String(has) + "%";

//  server.send(200, "text/plain", get_temp_humidity_string());
  server.send(200, "text/plain", txt);
}

void setup()
{
  dht.begin();  
  delay(5000);
  Serial.begin(115200);

float te = 10*dht.readTemperature();
int te1 = te/10;
int te2 = te - 10*te1;
int has = dht.readHumidity();
Serial.print("h = ");
Serial.println(has);
Serial.print("t = ");
Serial.println(te/10);
  
  delay(5000);
  connect_as_hosting_wifi();
}

void connect_as_hosting_wifi()
{
  WiFi.softAP(ssid, password);
  temp_txt = "WiFi setup with " + (String) ssid + ":" + (String) password;
  Serial.println("-----");
  Serial.println(temp_txt);

  IPAddress myIP = WiFi.softAPIP();
  
  server.on("/", handleRoot);
  
  server.begin();
  temp_txt = "HTTP Server started on " + (String) myIP;
  Serial.println(temp_txt);
}


void loop()
{
  server.handleClient();

// if webpage is without client(s)
 if (citire == 0)   // must read temperature 
 {
 taim5 = millis();  // note time reading
te = dht.readTemperature();
has = dht.readHumidity();
Serial.print("h = ");
Serial.println(has);
Serial.print("t = ");
Serial.println(te);
 citire = 1;        // note that (temperature was read)
 }
 if (millis() > taim5 + 15000) // if time between sensor reading is too long 
{
 citire = 0;        // must read again the temperature
 Serial.println("!");
}
}
