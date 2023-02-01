//ESP32
#define ACTUALIZAR_DATOS 300000 //5 min
#define tempMaxTermos 25.0
#define tempMinTermos 18.0
#define entidadesURL "http://200.126.14.228:3826/v2/entities/"
#define suscripcionesURL "http://200.126.14.228:3826/v2/subscriptions/"
#define notifyQuantum "http://quantumleap:8668/v2/notify"

#include <WiFi.h> //Libreria Wi-Fi
#include "time.h" //libreria para obtener hora
#include <WiFiUdp.h> //Libreria Wi-Fi para trabajar con UDP
#include <ESP32Ping.h> //Libreria de prueba ping
#include <HTTPClient.h> //Libreria para conectarse con broker Orion
#include <ArduinoJson.h> //Libreria para manejar strings JSON
//#include "arduino_secrets.h" //Libreria que contiene credenciales Wi-Fi

//Apagar brownout
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

//Datos de nodos
int heartbeats[8]; //Arreglo que almacena heartbeats
int numero[8]; //Arreglo que almacena el numero del nodo que se añade
char direcciones[8][16]; //Arreglo que almacena las direcciones IP de los nodos
int nodosAnadidos=0; //Contador de cuantos nodos se ha añadido
float temperatura[8]; //Arreglo que almacena los datos de temperatura de los nodos
float humedad[8]; //Arreglo que almacena los datos de humedad de los nodos
float promTemp;
float promHum;
int actualizado=0;
int primerPedido=0;

//Configuracion y credenciales station
const char* ssid       = "NotQuery"; //SECRET_WIFISSID;
const char* password   = "L4bLRD$@"; //SECRET_WIFIPASS;
/*IPAddress local_IP(192, 168, 100, 214);
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 255, 0);*/

//configuracion UDP
WiFiUDP Udp;
unsigned int localUdpPort = 3333;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets

//Configuracion servidor NTP
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -18000;
const int   daylightOffset_sec = 0;
char fecha_hora[20];

//Configuracion reles y a/c
int estadoDelAC=0;
float tempMax;
float tempMin;
const int relePIN = 25;
/*const int CompresorPIN = 25;
const int CondensadorPIN = 27;*/

//API URLs
String nodo="Nodo";
String promedio="Promedio";
String configuracion="Configuracion";
String nodoCentral="NodoCentral";

//Timers
unsigned long HB;
unsigned long patch;
unsigned long mediciones;
unsigned long revisionEstado;
unsigned long revisionConfiguracion;

//Funciones UDP
void escucharUDP(){
  delay(1);
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    //receive incoming UDP packets
    //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);
    deserializar(incomingPacket, (char *)Udp.remoteIP().toString().c_str());
  }
}

void deserializar(char *input, char *direccionIP){
  StaticJsonDocument<64> doc;
  DeserializationError error = deserializeJson(doc, input);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  
  bool tieneInicio = doc.containsKey("inicio");
  bool tieneHB = doc.containsKey("heartbeat");
  bool tieneDatos = doc.containsKey("temperatura") and doc.containsKey("humedad");

  if(tieneInicio){
    int anadir=anadirNodo((char *) direccionIP, doc["inicio"]);
    int numero=1;
    if (anadir==2 or anadir==3){
      numero=2;
    }
    char resp[20];
    StaticJsonDocument<64> doc;
    doc["respuesta_ini"] = numero;
    serializeJson(doc, resp);
    Udp.beginPacket(direccionIP, localUdpPort);
    Udp.print(resp);
    Udp.endPacket();
    if (anadir==0){
      revisionEstado=millis();
      Serial.printf("Se anadio el nodo %s. Nodos anadidos: %d",(char *)direccionIP, nodosAnadidos);Serial.println();
    }
  }

  else if (tieneHB){
    int pos=buscarPosicion((char *)direccionIP);
    if (pos!=-1){
      heartbeats[pos]=heartbeats[pos]+1;
      Serial.printf("Se recibio HB de: %s", (char *)direccionIP);Serial.println();
    }
    else {return;}
  }

  else if (tieneDatos){
    int pos=buscarPosicion((char *)direccionIP);
    if (pos!=-1){
      temperatura[pos]=doc["temperatura"];
      humedad[pos]=doc["humedad"];
      Serial.printf("Se recibieron datos de: %s", (char *)direccionIP);Serial.println();
    }
    else {return;}
  }
}

int anadirNodo(char *direccionIP, int numero_de_nodo){ //Funcion para añadir nodo. 0=nodo anadido | 1=nodo ya existe | 2=numero de nodo ya existe | 3=limite de nodos alcanzado
  //Limite de nodos alcanzado
  if(nodosAnadidos>=8){
    Serial.println("No se puede anadir mas nodos");
    return 3;
  }
  //Numero de nodo ya existe
  for (size_t a=0; a<nodosAnadidos; a++){
    if(numero[a]==numero_de_nodo){
      Serial.println("Numero de nodo ya existe");
      return 2;
    }
  }
  //Nodo previamente anadido
  char buffer[16];
  memcpy(buffer, direccionIP, 16);
  for (size_t x=0; x<nodosAnadidos; x++){
    if (memcmp(&direcciones[x], &buffer, sizeof(char)*16)==0){
      Serial.printf("Nodo ya existe. IP: %s", buffer);Serial.println();
      return 1;
    }
  }
  //Nodo anadido exitosamente
  memcpy(direcciones[nodosAnadidos], buffer, 16);
  numero[nodosAnadidos]=numero_de_nodo;
  nodosAnadidos++;
  return 0;
}

void crearHeartbeat(){
  HB=millis();
  if(nodosAnadidos>0){
    for (size_t x=0; x<nodosAnadidos; x++){
      char heartbeat[20];
      StaticJsonDocument<64> doc;
      doc["heartbeat"] = 1;
      serializeJson(doc, heartbeat);
      Udp.beginPacket(direcciones[x], localUdpPort);
      Udp.print(heartbeat);
      Udp.endPacket();
      Serial.printf("hb enviado a: %s", direcciones[x]);Serial.println();
    }
  }
}

int buscarPosicion(char *direccionIP){
  for (size_t x=0; x<nodosAnadidos; x++){
    if (memcmp(direcciones[x], direccionIP, sizeof(char)*16)==0){
      return x; 
    }
  }
  //Serial.println("No se encontro esa dir IP");
  return -1;
}

//************************************************Funciones de prueba************************************************
void imprimir(){
  Serial.println();
  Serial.println("Nodos disponibles:");
  for (size_t x=0; x<nodosAnadidos; x++){
    Serial.printf("Nodo #%d con direccion IP %s. datos: temperatura=%f y humedad=%f", numero[x], direcciones[x], temperatura[x], humedad[x]); Serial.println();
  }
  Serial.println();
}
//************************************************Funciones de prueba************************************************

void verificarHB(){
  revisionEstado=millis();
  for (size_t x=0; x<nodosAnadidos; x++){
    if (heartbeats[x]==0){
      patchDesconexion(x);
      int copia_numero[8];
      char copia_direcciones[8][16];
      float copia_temperatura[8]; 
      float copia_humedad[8];
      for (size_t y=0; y<nodosAnadidos; y++){
        memcpy(copia_direcciones[y], direcciones[y], 16);
        copia_numero[y]=numero[y];
        copia_temperatura[y]=temperatura[y];
        copia_humedad[y]=humedad[y];
        memset(direcciones[y], 0, 16);
        numero[y]=0;
        temperatura[y]=0;
        humedad[y]=0;
      }
      for (size_t z=0; z<nodosAnadidos; z++){
        if (z<x){
          memcpy(direcciones[z], copia_direcciones[z], 16);
          numero[z]=copia_numero[z];
          temperatura[z]=copia_temperatura[z];
          humedad[z]=copia_humedad[z];
        }
        else if (z==x){
          Serial.printf("Se elimino el nodo con direccion IP %s", copia_direcciones[z]); Serial.println();
        }
        else if (z>x){
          memcpy(direcciones[z-1], copia_direcciones[z], 16);
          numero[z-1]=copia_numero[z];
          temperatura[z-1]=copia_temperatura[z];
          humedad[z-1]=copia_humedad[z];
        }
      }
      nodosAnadidos=nodosAnadidos-1;
    }
  }
  for (size_t a=0; a<nodosAnadidos; a++){
    heartbeats[a]=0; 
  }
}

void patchDesconexion(int numero){ //Funcion para notificar al Backend alguna desconexion de los nodos sensores
  String buff;
  StaticJsonDocument<384> doc;
  getTime();
  JsonObject fecha = doc.createNestedObject("fecha");
  fecha["type"] = "DateTime";
  fecha["value"] = fecha_hora;
  JsonObject estado = doc.createNestedObject("estado");
  estado["type"] = "Number";
  estado["value"] = 0;
  JsonObject temperaturaJSON = doc.createNestedObject("temperatura");
  temperaturaJSON["type"] = "Number";
  temperaturaJSON["value"] = 0;
  JsonObject humedadJSON = doc.createNestedObject("humedad");
  humedadJSON["type"] = "Number";
  humedadJSON["value"] = 0;
  serializeJson(doc, buff);

  WiFiClient client;
  HTTPClient http;
  int httpResponseCode; //Respuesta HTTP al verificar si la entidad existe
  //Serial.println(buff);
  http.begin(client, entidadesURL+nodo+numero+"/attrs/");
  http.addHeader("Content-Type", "application/json");
  httpResponseCode = http.PATCH(buff);
  //Serial.printf("respuesta patch: %d", httpResponseCode); Serial.println();
  if (httpResponseCode!=204){
    Serial.printf("Error actualizar datos del Nodo%d", numero);Serial.println();
    return;
  }
  http.end();
  Serial.printf("Nodo%d actualizado como desconectado", numero);Serial.println();
}

void actualizarDatos(){
  mediciones=millis();
  if(nodosAnadidos>0){
    for (size_t x=0; x<nodosAnadidos; x++){
      char actualizar[20];
      StaticJsonDocument<64> doc;
      doc["actualizar"] = "";
      serializeJson(doc, actualizar);
      Udp.beginPacket(direcciones[x], localUdpPort);
      Udp.print(actualizar);
      Udp.endPacket();
      Serial.printf("Mensaje de atualizacion enviado a: %s", direcciones[x]);Serial.println();
    }
    patch=millis();
    actualizado=1;
  }
  primerPedido=1;
}

//Funciones HTTP
int crearNodo(int numero){ //Funcion para crear entidades NODO. 0:Todo correcto | 1:Error al crear entidad
  String buff;
  StaticJsonDocument<384> doc;
  doc["id"] = nodo+(String) numero;
  doc["type"] = "AirQualityObserved";
  JsonObject fecha = doc.createNestedObject("fecha");
  fecha["type"] = "DateTime";
  fecha["value"] = "2023-01-01T00:00:00";
  JsonObject estado = doc.createNestedObject("estado");
  estado["type"] = "Number";
  estado["value"] = "0.0";
  JsonObject temperatura = doc.createNestedObject("temperatura");
  temperatura["type"] = "Number";
  temperatura["value"] = "0.0";
  JsonObject humedad = doc.createNestedObject("humedad");
  humedad["type"] = "Number";
  humedad["value"] = "0.0";
  serializeJson(doc, buff);

  WiFiClient client;
  HTTPClient http;
  http.begin(client, entidadesURL);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(buff);
  if (httpResponseCode!=201){
    Serial.printf("Error al crear entidad Nodo%d", numero);Serial.println();
    return 1;
  }
  http.end();
  return 0;
}

int crearPromedio(){ //Funcion para crear entidad PROMEDIO. 0:Todo correcto | 1:Error al crear entidad
  String buff;
  StaticJsonDocument<384> doc;
  doc["id"] = promedio;
  doc["type"] = "AirQualityObserved";
  JsonObject fecha = doc.createNestedObject("fecha");
  fecha["type"] = "DateTime";
  fecha["value"] = "2023-01-01T00:00:00";
  JsonObject temperatura = doc.createNestedObject("temperatura");
  temperatura["type"] = "Number";
  temperatura["value"] = "0.0";
  JsonObject humedad = doc.createNestedObject("humedad");
  humedad["type"] = "Number";
  humedad["value"] = "0.0";
  serializeJson(doc, buff);

  WiFiClient client;
  HTTPClient http;
  http.begin(client, entidadesURL);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(buff);
  if (httpResponseCode!=201){
    Serial.printf("Error al crear entidad Promedio");
    return 1;
  }
  http.end();
  return 0;
}

int crearConfiguracion(){ //Funcion para crear entidad CONFIGURACION. 0:Todo correcto | 1:Error al crear entidad
  String buff;
  StaticJsonDocument<384> doc;
  doc["id"] = configuracion;
  doc["type"] = "AirQualityObserved";
  JsonObject fecha = doc.createNestedObject("fecha");
  fecha["type"] = "DateTime";
  fecha["value"] = "2023-01-01T00:00:00";
  JsonObject estado = doc.createNestedObject("estado"); //Estado del aire (encendido o apagadp)
  estado["type"] = "Number";
  estado["value"] = "0.0";
  JsonObject tempMinTermostato = doc.createNestedObject("tempMinTermostato"); //Temperatura a la que el termostato se apaga
  tempMinTermostato["type"] = "Number";
  tempMinTermostato["value"] = tempMinTermos;
  JsonObject tempMaxTermostato = doc.createNestedObject("tempMaxTermostato"); //Temperatura a la que el termostato se enciende
  tempMaxTermostato["type"] = "Number";
  tempMaxTermostato["value"] = tempMaxTermos;
  serializeJson(doc, buff);

  WiFiClient client;
  HTTPClient http;
  http.begin(client, entidadesURL);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(buff);
  if (httpResponseCode!=201){
    Serial.println("Error al crear entidad Configuracion");
    return 1; 
  }
  http.end();
  return 0;
}

int verificarSuscripcion(String Get, String suscripcion){ 
  DynamicJsonDocument doc(8192);
  DeserializationError error = deserializeJson(doc, Get);  
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return 2;
  }
  String sus="Suscripción_";
  for(size_t x=0; x<10; x++){
    JsonObject root = doc[x];
    String desc = root["description"];
    //Serial.println(desc);
    //Serial.println(sus+suscripcion);
    if (desc==sus+suscripcion){
      //Serial.println("Suscripcion existe");
      return 0;
    }
  } 
  Serial.printf("Suscripcion no encontrada: %s", suscripcion); Serial.println();
  return 1;
}

int crearSusNodo(int numero){
  String buff;
  StaticJsonDocument<768> doc;
  doc["description"] = "Suscripción_Nodo"+(String) numero;  
  JsonObject subject = doc.createNestedObject("subject");  
  JsonObject subject_entities_0 = subject["entities"].createNestedObject();
  subject_entities_0["idPattern"] = "Nodo"+(String) numero;
  subject_entities_0["type"] = "AirQualityObserved";  
  JsonArray subject_condition_attrs = subject["condition"].createNestedArray("attrs");
  subject_condition_attrs.add("fecha");
  subject_condition_attrs.add("estado");
  JsonObject notification = doc.createNestedObject("notification");  
  JsonArray notification_attrs = notification.createNestedArray("attrs");
  notification_attrs.add("id");
  notification_attrs.add("fecha");
  notification_attrs.add("estado");
  notification_attrs.add("temperatura");
  notification_attrs.add("humedad");
  notification["http"]["url"] = notifyQuantum;  
  JsonArray notification_metadata = notification.createNestedArray("metadata");
  notification_metadata.add("dateCreated");
  notification_metadata.add("dateModified");
  serializeJson(doc, buff);
  WiFiClient client;
  HTTPClient http;
  http.begin(client, suscripcionesURL);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(buff);
  if (httpResponseCode!=201){
    return 1; 
  }
  http.end();
  return 0;
}

int crearSusPromedio(){
  String buff;
  StaticJsonDocument<512> doc;
  doc["description"] = "Suscripción_Promedio";
  JsonObject subject = doc.createNestedObject("subject");
  JsonObject subject_entities_0 = subject["entities"].createNestedObject();
  subject_entities_0["idPattern"] = "Promedio";
  subject_entities_0["type"] = "AirQualityObserved";
  subject["condition"]["attrs"][0] = "fecha";
  JsonObject notification = doc.createNestedObject("notification");
  JsonArray notification_attrs = notification.createNestedArray("attrs");
  notification_attrs.add("id");
  notification_attrs.add("fecha");
  notification_attrs.add("temperatura");
  notification_attrs.add("humedad");
  notification["http"]["url"] = notifyQuantum;
  JsonArray notification_metadata = notification.createNestedArray("metadata");
  notification_metadata.add("dateCreated");
  notification_metadata.add("dateModified");
  serializeJson(doc, buff);
  WiFiClient client;
  HTTPClient http;
  http.begin(client, suscripcionesURL);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(buff);
  if (httpResponseCode!=201){
    return 1; 
  }
  http.end();
  return 0;
}

int crearSusConfiguracion(){
  String buff;
  StaticJsonDocument<768> doc;
  doc["description"] = "Suscripción_Configuracion"; 
  JsonObject subject = doc.createNestedObject("subject");  
  JsonObject subject_entities_0 = subject["entities"].createNestedObject();
  subject_entities_0["idPattern"] = "Configuracion";
  subject_entities_0["type"] = "AirQualityObserved";  
  JsonArray subject_condition_attrs = subject["condition"].createNestedArray("attrs");
  subject_condition_attrs.add("fecha");
  subject_condition_attrs.add("estado");
  subject_condition_attrs.add("tempMinTermostato");
  subject_condition_attrs.add("tempMaxTermostato"); 
  JsonObject notification = doc.createNestedObject("notification");  
  JsonArray notification_attrs = notification.createNestedArray("attrs");
  notification_attrs.add("id");
  notification_attrs.add("fecha");
  notification_attrs.add("estado");
  notification_attrs.add("tempMinTermostato");
  notification_attrs.add("tempMaxTermostato");
  notification["http"]["url"] = notifyQuantum;  
  JsonArray notification_metadata = notification.createNestedArray("metadata");
  notification_metadata.add("dateCreated");
  notification_metadata.add("dateModified");  
  serializeJson(doc, buff);
  WiFiClient client;
  HTTPClient http;
  http.begin(client, suscripcionesURL);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(buff);
  if (httpResponseCode!=201){
    return 1; 
  }
  http.end();
  return 0;
}

/*int crearSusNodoCentral(){ //ACTIVAR SI SE USAN LAS SUSCRIPCIONES DEL BROKER (ORION)
  String buff;
  StaticJsonDocument<768> doc;
  doc["description"] = "Suscripción_NodoCentral";  
  JsonObject subject = doc.createNestedObject("subject"); 
  JsonObject subject_entities_0 = subject["entities"].createNestedObject();
  subject_entities_0["idPattern"] = "Configuracion";
  subject_entities_0["type"] = "AirQualityObserved";
  JsonArray subject_condition_attrs = subject["condition"].createNestedArray("attrs");
  subject_condition_attrs.add("fecha");
  subject_condition_attrs.add("estado");
  subject_condition_attrs.add("tempMinTermostato");
  subject_condition_attrs.add("tempMaxTermostato");
  JsonObject notification = doc.createNestedObject("notification");
  JsonArray notification_attrs = notification.createNestedArray("attrs");
  notification_attrs.add("estado");
  notification_attrs.add("tempMinTermostato");
  notification_attrs.add("tempMaxTermostato");
  notification["http"]["url"] = "http://192.168.100.214:3333";
  JsonArray notification_metadota = notification.createNestedArray("metadata");
  notification_metadota.add("dateCreated");
  notification_metadota.add("dateModified");
  serializeJson(doc, buff);
  WiFiClient client;
  HTTPClient http;
  http.begin(client, suscripcionesURL);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(buff);
  Serial.println(httpResponseCode);
  if (httpResponseCode!=201){
    return 1; 
  }
  http.end();
  return 0;
}*/

int verificarBackend(){ //Funcion para verificar conexion con el Backend. 0:Todo correcto | 1:no hay conexión con el Backend | 2: error al crear entidad 
  bool success = Ping.ping("192.168.100.160", 5);
  if(!success){
    Serial.println("No se puede estbalcer conexion con Backend");
    return 1;
  }

  WiFiClient client;
  HTTPClient http;
  int httpResponseCode; //Respuesta HTTP al verificar si la entidad existe

  //Obtener suscripciones
  http.begin(client, suscripcionesURL);
  httpResponseCode = http.GET();
  //Serial.println(httpResponseCode);
  if (httpResponseCode!=200){
    Serial.println("Error al obtener suscripciones");
    http.end();
    return 1;
  }
  String subsJSON=http.getString();
  http.end();
  
  //Verificar entidades NODO
  for (int x=1; x<9; x++){
    // Your Domain name with URL path or IP address with path
    http.begin(client, entidadesURL+nodo+(String)x);
    httpResponseCode = http.GET();
    //Serial.print("HTTP Response code: "); Serial.println(httpResponseCode);
    if (httpResponseCode!=200){
      Serial.printf("Error al consultar entidad %s%s", nodo, (String) x);Serial.println();
      Serial.println("Creando nodo...");
      if (crearNodo(x)==1){
        return 2;
      }
      Serial.println("Nodo creado");
    }
    if (verificarSuscripcion(subsJSON, nodo+(String)x)!=0){
      if (crearSusNodo(x)!=0){
        Serial.println("Error al crear suscripcion");
        return 1;
      }
      Serial.println("Suscripcion creada");
    }
    // Free resources
    http.end();
  }

  //Verificar entidad PROMEDIO
  http.begin(client, entidadesURL+promedio);
  httpResponseCode = http.GET();
  //Serial.print("HTTP Response code: "); Serial.println(httpResponseCode);
  if (httpResponseCode!=200){
    Serial.printf("Error al consultar entidad %s", promedio);Serial.println();
    Serial.println("Creando promedio...");
    if (crearPromedio()==1){
      return 2;
    }
    Serial.println("Promedio creado");
  }
  if (verificarSuscripcion(subsJSON, promedio)!=0){
    if (crearSusPromedio()!=0){
      Serial.println("Error al crear suscripcion");
      return 1;
    }
    Serial.println("Suscripcion creada");
  }
  http.end();

  //Verificar entidad CONFIGURACION
  http.begin(client, entidadesURL+configuracion);
  httpResponseCode = http.GET();
  //Serial.print("HTTP Response code: "); Serial.println(httpResponseCode);
  if (httpResponseCode!=200){
    Serial.printf("Error al consultar entidad %s", configuracion);Serial.println();
    Serial.println("Creando configuracion...");
    if (crearConfiguracion()==1){
      return 2;
    }
    Serial.println("Configuracion creado");
  }
  if (verificarSuscripcion(subsJSON, configuracion)!=0){
    if (crearSusConfiguracion()!=0){
      Serial.println("Error al crear suscripcion");
      return 1;
    }
    Serial.println("Suscripcion creada");
  }
  http.end();

  // Verificar suscripcion Configutacion (ESTO SE USA SI SE USAN LAS SUSCRIPCIONES DEL BROKER)
  /*if (verificarSuscripcion(subsJSON, nodoCentral)!=0){
    if (crearSusNodoCentral()!=0){
      Serial.println("Error al crear suscripcion");
      return 1;
    }
    Serial.println("Suscripcion creada");
  }*/
 
  Serial.println("Se establecio conexion con el Backend");
  return 0;
}

void sacarPromedio(){
  float sum1=0;
  float sum2=0;
  for (size_t x=0; x<nodosAnadidos; x++){
    sum1=sum1+temperatura[x];
    sum2=sum2+humedad[x];
  }
  promTemp=sum1/nodosAnadidos;
  promHum=sum2/nodosAnadidos;
}

//Subir datos
int patchDatos(){
  actualizado=0;
  WiFiClient client;
  HTTPClient http;
  String buff;
  int httpResponseCode;
  StaticJsonDocument<384> doc;
  //Patch nodos
  for (int x=1; x<9; x++){
    for (int y=0; y<8; y++){
      if (x==numero[y]){
        getTime();
        JsonObject fecha = doc.createNestedObject("fecha");
        fecha["type"] = "DateTime";
        fecha["value"] = fecha_hora;
        JsonObject estado = doc.createNestedObject("estado");
        estado["type"] = "Number";
        estado["value"] = 1;
        JsonObject temperaturaJSON = doc.createNestedObject("temperatura");
        temperaturaJSON["type"] = "Number";
        temperaturaJSON["value"] = temperatura[y];
        JsonObject humedadJSON = doc.createNestedObject("humedad");
        humedadJSON["type"] = "Number";
        humedadJSON["value"] = humedad[y];
        serializeJson(doc, buff);
      
        //Serial.println(buff);
        http.begin(client, entidadesURL+nodo+x+"/attrs/");
        http.addHeader("Content-Type", "application/json");
        httpResponseCode = http.PATCH(buff);
        //Serial.printf("respuesta patch: %d", httpResponseCode); Serial.println();
        if (httpResponseCode!=204){
          Serial.printf("Error actualizar datos del Nodo%d", x);Serial.println();
          return 1;
        }
        http.end();
        doc.clear();
        buff.remove(0);
      }
    }
  }

  //Patch prom
  getTime();
  sacarPromedio();
  JsonObject fecha = doc.createNestedObject("fecha");
  fecha["type"] = "DateTime";
  fecha["value"] = fecha_hora;
  JsonObject temperaturaJSON = doc.createNestedObject("temperatura");
  temperaturaJSON["type"] = "Number";
  temperaturaJSON["value"] = promTemp;
  JsonObject humedadJSON = doc.createNestedObject("humedad");
  humedadJSON["type"] = "Number";
  humedadJSON["value"] = promHum;
  serializeJson(doc, buff);

  http.begin(client, entidadesURL+promedio+"/attrs");
  http.addHeader("Content-Type", "application/json");
  httpResponseCode = http.PATCH(buff);
  if (httpResponseCode!=204){
    Serial.println("Error actualizar datos de promedio");
    return 1;
  }
  http.end();
  return 0;
}

//Funciones NTP
void getTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  strftime(fecha_hora, 20, "%Y-%m-%dT%H:%M:%S", &timeinfo); //"2023-01-01T00:00:00"
}

void leerConfiguracion(){ //Funcion que lee constantemente la entidad Configuracion para encender o apagar el A/C 
  WiFiClient client;
  HTTPClient http;
  int httpResponseCode; //Respuesta HTTP al verificar si la entidad existe

  http.begin(client, entidadesURL+configuracion);
  httpResponseCode = http.GET();
  //Serial.println(httpResponseCode);
  if (httpResponseCode!=200){
    Serial.println("Error al obtener entidad configuracion");
    return;
  }
  String input=http.getString();
  http.end();

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, input); 
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  } 
  int estadoanterior=estadoDelAC;
  float tempmaxanterior=tempMax;
  float tempminanterior=tempMin;
  
  JsonObject estado = doc["estado"];
  estadoDelAC = estado["value"]; // "0.0"
  if (estadoDelAC==1 and estadoDelAC!=estadoanterior){
    digitalWrite(relePIN, LOW);
    Serial.println("A/C ENCENDIDO!!!");
  }
  else if (estadoDelAC==0 and estadoDelAC!=estadoanterior){
    digitalWrite(relePIN, HIGH);
    Serial.println("A/C APAGADO!!!");
  }
  
  JsonObject tempMaxTermostato = doc["tempMaxTermostato"];
  tempMax = tempMaxTermostato["value"]; // 25
  if (tempMax!=tempmaxanterior){
    Serial.printf("Temp max cambio a %f", tempMax); Serial.println();
  }
  
  JsonObject tempMinTermostato = doc["tempMinTermostato"];
  tempMin = tempMinTermostato["value"]; // 18
  if (tempMin!=tempminanterior){
    Serial.printf("Temp min cambio a %f", tempMin); Serial.println();
  }
}

/*void patchConfiguracion(){
  getTime();
  String buff;
  StaticJsonDocument<384> doc;
  JsonObject fecha = doc.createNestedObject("fecha");
  fecha["type"] = "DateTime";
  fecha["value"] = fecha_hora;
  JsonObject estado = doc.createNestedObject("estado");
  estado["type"] = "Number";
  estado["value"] = estadoDelAC;
  serializeJson(doc, buff);

  WiFiClient client;
  HTTPClient http;
  http.begin(client, entidadesURL+configuracion+"/attrs");
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.PATCH(buff);
  if (httpResponseCode!=204){
    Serial.println("Error actualizar configuracion");
    return;
  }
  http.end();
}*/

void termostato(){
  if (primerPedido==1){
    sacarPromedio();
    if (promTemp>tempMax and estadoDelAC==1){
      digitalWrite(relePIN, LOW);
      //patchConfiguracion();
      Serial.println("Temperatura ambiental alta, encendiendo A/C");
    }
    else if (promTemp<tempMin and estadoDelAC==1){
      digitalWrite(relePIN, HIGH);
      //patchConfiguracion();
      Serial.println("Temperatura ambiental baja, apagando A/C");
    }
    else if (promTemp<tempMax and promTemp>tempMin){
      return;
    } 
  }
}

void setup()
{
  Serial.begin(115200);

  /*pinMode(CompresorPIN, OUTPUT);
  pinMode(CondensadorPIN, OUTPUT);
  digitalWrite(CondensadorPIN, HIGH);
  digitalWrite(CompresorPIN, HIGH);*/
  pinMode(relePIN, OUTPUT);
  digitalWrite(relePIN, HIGH);

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //apagar BO detector

  // Configures static IP address
  /*if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }*/
  
  //Conectarse a WiFi
  Serial.printf("Conectandose a %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.print(" conectado"); Serial.println();

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  //Conectarse al Backend
  if (verificarBackend()!=0){
    Serial.println("Error en la conexion con el Backend");
    while(1) delay(1);
  }

  //Iniciar puerto UDP
  Udp.begin(localUdpPort);
  Serial.printf("Escuchando en IP %s, puertp UDP %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

  tempMax=tempMaxTermos;
  tempMin=tempMinTermos;

  leerConfiguracion();

  HB=millis();
  mediciones=millis();
  revisionConfiguracion=millis();
}

void loop()
{
  escucharUDP();
  termostato();
  
  if(millis()-revisionConfiguracion>=5000){ //Revisar entidad configuracion cada 5 seg
    leerConfiguracion();
  }

  if(millis()-HB>=30000 and nodosAnadidos>0){ //Crear mensajes HB cada 30 seg
    Serial.println("Creando mensajes HB");
    crearHeartbeat();
  }
  if (millis()-revisionEstado>=90000 and nodosAnadidos>0){ //Revisar respuestas de HB cada 1.5 min
    Serial.println("Verificando respuestas HB");
    verificarHB();
    imprimir();
  }
  if (millis()-mediciones>=ACTUALIZAR_DATOS and nodosAnadidos>0){ //Pedir datos a los sensores
    Serial.println("Pidiendo datos a los nodos...");
    actualizarDatos();
  }
  if (millis()-patch>=30000 and actualizado==1 and nodosAnadidos>0){ //Actualizar datos en backend
    if (patchDatos()!=0){
      Serial.println("Error actualizando datos");
    }
    else {
      Serial.println("Datos actualizados correctamente");
    }
  }
}
