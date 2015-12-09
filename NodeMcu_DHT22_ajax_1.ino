// original sketch by katz (i found it at http://www.esp8266.com/viewtopic.php?f=8&t=4307 )
// adapted sketch by niq_ro ( http://www.tehnic.go.ro )
// I use a NodeMcu with DHT22 at D1 pin (GPIO5)

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);
const char* ssid="SSID";
const char* password="pasword";
String webSite,javaScript,XML;

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



void buildWebsite(){
  buildJavascript();
  webSite="<!DOCTYPE HTML>\n";
  webSite+=javaScript;
  webSite+="<BODY onload='process()'>\n";
  webSite+="<center><BR>NodelMCU website with DHT22 using AJAX<BR>\n";
  webSite+="<A id='runtime'></A>\n";
  webSite+="</BODY>\n";
  webSite+="</HTML>\n";
}

void buildJavascript(){
  javaScript="<SCRIPT>\n";
  javaScript+="var xmlHttp=createXmlHttpObject();\n";

  javaScript+="function createXmlHttpObject(){\n";
  javaScript+=" if(window.XMLHttpRequest){\n";
  javaScript+="    xmlHttp=new XMLHttpRequest();\n";
  javaScript+=" }else{\n";
  javaScript+="    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');\n";
  javaScript+=" }\n";
  javaScript+=" return xmlHttp;\n";
  javaScript+="}\n";

  javaScript+="function process(){\n";
  javaScript+=" if(xmlHttp.readyState==0 || xmlHttp.readyState==4){\n";
  javaScript+="   xmlHttp.open('PUT','xml',true);\n";
  javaScript+="   xmlHttp.onreadystatechange=handleServerResponse;\n"; // no brackets?????
  javaScript+="   xmlHttp.send(null);\n";
  javaScript+=" }\n";
  javaScript+=" setTimeout('process()',10000);\n";
  javaScript+="}\n";
  
  javaScript+="function handleServerResponse(){\n";
  javaScript+=" if(xmlHttp.readyState==4 && xmlHttp.status==200){\n";
  javaScript+="   xmlResponse=xmlHttp.responseXML;\n";
  javaScript+="   xmldoc = xmlResponse.getElementsByTagName('response');\n";
  javaScript+="   message = xmldoc[0].firstChild.nodeValue;\n";
  javaScript+="   document.getElementById('runtime').innerHTML=message;\n";
  javaScript+=" }\n";
  javaScript+="}\n";
  javaScript+="</SCRIPT>\n";
}

void buildXML(){
  XML="<?xml version='1.0'?>";
  XML+="<response>";
  XML+=millis2time();
  XML+="</response>";
}

String millis2time(){
  String Time="";
/*  unsigned long ss;
  byte mm,hh;
  ss=millis()/1000;
  hh=ss/3600;
  mm=(ss-hh*3600)/60;
  ss=(ss-hh*3600)-mm*60;
  if(hh<10)Time+="0";
  Time+=(String)hh+":";
  if(mm<10)Time+="0";
  Time+=(String)mm+":";
  if(ss<10)Time+="0";
  Time+=(String)ss;
*/

te = 10*dht.readTemperature();
te1 = te/10;
te2 = te - 10*te1;
has = dht.readHumidity();
Serial.print("h = ");
Serial.print(has);
Serial.println(" %");
Serial.print("t = ");
Serial.print(te/10);
Serial.println("^C");
      Time+= "temp: " + (String)te1 + "," + (String)te2 + "°C ";
      Time+= " humidity: " + (String)has + "%";  
  return Time;
}

void handleWebsite(){
  buildWebsite();
  server.send(200,"text/html",webSite);
}

void handleXML(){
  buildXML();
  server.send(200,"text/xml",XML);
}

void setup() {
  Serial.begin(115200);  
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED)delay(500);
  WiFi.mode(WIFI_STA);
  Serial.println("\n\nBOOTING ESP8266 ...");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("Station IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/",handleWebsite);
  server.on("/xml",handleXML);
  server.begin();  
}

void loop() {
  server.handleClient();
}
