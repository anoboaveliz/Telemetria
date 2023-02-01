//ESP8266
#define NUMERO_NODO 4
#define DHTPIN 2
#define DHTTYPE DHT11

//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h> //Libreria para manejar strings JSON
#include <DHT.h>
//#include "Adafruit_SHTC3.h"
//#include "arduino_secrets.h" //Libreria que contiene credenciales Wi-Fi

//Definir el pin del led azul de la tarjeta 
/*static const int LED_PIN = 2;
int ledState = HIGH;*/

//Configuracion y credenciales del AP
const char *ssid = "NotQuery"; //SECRET_WIFISSID;
const char *password = "L4bLRD$@"; //SECRET_WIFIPASS;

//Tiempo de envio broadcast
unsigned long tiempoDeEnvio=5000;

//Variables Udp
WiFiUDP Udp;
int NC=0;
char incomingPacket[255];
char nodoCentral[16];
int estado;
const char *broadcast="255.255.255.255";
unsigned int localPort = 3333;

//Variables de higrometro
DHT dht(DHTPIN, DHTTYPE);
float temperatura;
float humedad;

//Timers
unsigned long revisionEstado;
unsigned long timer;

//Variables de prueba
int intentos=0;

//Funciones
void leerSensor(){
  /*sensors_event_t humidity, temp;
  shtc3.getEvent(&humidity, &temp);*/
  
  temperatura=dht.readTemperature();
  humedad=dht.readHumidity();;
}

void escucharUdp(){
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming Udp packets
    //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    Serial.printf("Udp packet contents: %s\n", incomingPacket);
    deserializar(incomingPacket, Udp.remoteIP().toString().c_str());
  }
}

void obtenerNodoCentral(){
  char inicio[20];
  StaticJsonDocument<64> doc;
  doc["inicio"] = NUMERO_NODO;
  serializeJson(doc, inicio);
  Udp.beginPacket(broadcast, localPort);
  Udp.print(inicio);
  Udp.endPacket();
}

void ResponderHeartbeat(){
  char heartbeat[20];
  StaticJsonDocument<64> doc;
  doc["heartbeat"] = 1;
  serializeJson(doc, heartbeat);
  Udp.beginPacket(nodoCentral, localPort);
  Udp.print(heartbeat);
  Udp.endPacket();
  Serial.println("Se respondio HB");
}

void deserializar(char *input, const char *direccionIP){
  char buffer[16];
  memcpy(buffer, direccionIP, sizeof(char)*16);
  StaticJsonDocument<64> doc;
  DeserializationError error = deserializeJson(doc, input);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  
  bool tieneInicio = doc.containsKey("respuesta_ini");
  bool tieneHB = doc.containsKey("heartbeat");
  bool tieneActualizar = doc.containsKey("actualizar");

  if (tieneInicio){
    NC=doc["respuesta_ini"];
    memcpy(nodoCentral, buffer, 16);
    revisionEstado=millis();
    Serial.printf("Nodo Central encontrado: NC=%d", NC);Serial.println();
  }

  else if (tieneHB and NC==1){
    estado=doc["heartbeat"];
    Serial.printf("estado: %d", estado);Serial.println();
    revisionEstado=millis();
    ResponderHeartbeat();
  }

  else if (tieneActualizar and NC==1){
    leerSensor();
    char datos[50];
    StaticJsonDocument<64> doc;
    doc["temperatura"] = temperatura;
    doc["humedad"] = humedad;
    serializeJson(doc, datos);
    Udp.beginPacket(nodoCentral, localPort);
    Udp.print(datos);
    Udp.endPacket();
    Serial.printf("Se enviaron los datos sensados, temperatura=%f y humedad=%f", temperatura, humedad);
  }
}

void setup() {
  Serial.begin(115200);

  //pinMode(LED_PIN, OUTPUT);
  //digitalWrite(LED_PIN, HIGH);

  /*if (! shtc3.begin()) {
    Serial.println("Couldn't find SHTC3");
    //ledState = 1 - ledState;
    //digitalWrite(LED_PIN, LOW);
    while(1) delay(1);
  }
  Serial.println("Found SHTC3 sensor");*/
  
  dht.begin();
  
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" connected");
  
  Udp.begin(localPort);
  
  Serial.printf("Buscando NC y escuchando la IP %s en el puerto UDP %d\n", WiFi.localIP().toString().c_str(), localPort);Serial.println();
  timer=millis();
}

void loop() {
  escucharUdp();
  
  if (millis()-timer>=tiempoDeEnvio and NC==0){
    obtenerNodoCentral();
    Serial.println("enviando mensaje");
    timer=millis();
  }

  else if (NC==1){
    //Serial.println("aqui1");
    if (millis()-revisionEstado>=90000){ //Olvidar NC despues de 1.5 min
      Serial.println("aqui2");
      NC=0;
      memset(nodoCentral, 0, 16);
      Serial.println("Se perdio conexion con NC");
    }
  }

  else if (NC==2){
   tiempoDeEnvio=60000;
   NC=0; 
  }
}
