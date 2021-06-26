//=================================================================================
//  _____ _____ __  __ ____  _____ ____      _  _____ _   _ ____      _    
// |_   _| ____|  \/  |  _ \| ____|  _ \    / \|_   _| | | |  _ \    / \   
//   | | |  _| | |\/| | |_) |  _| | |_) |  / _ \ | | | | | | |_) |  / _ \  
//   | | | |___| |  | |  __/| |___|  _ <  / ___ \| | | |_| |  _ <  / ___ \ 
//   |_| |_____|_|  |_|_|   |_____|_| \_\/_/   \_\_|  \___/|_| \_\/_/   \_\
//=================================================================================

#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <DHT.h>



#define DHTTYPE DHT11 
#define DHTPIN5  5
#define DHTPIN4  4
#define DHTPIN2  2

  DHT dht1(DHTPIN5, DHTTYPE);
  DHT dht2(DHTPIN4, DHTTYPE);
  DHT dht3(DHTPIN2, DHTTYPE);

  String msgTemperatura;

  static char sensor1_celsiusTemp[7];
  static char sensor2_celsiusTemp[7];
  static char sensor3_celsiusTemp[7];
 
  static char sensor1_humidityTemp[7];
  static char sensor2_humidityTemp[7];
  static char sensor3_humidityTemp[7];


  float h1,h2,h3;
  float t1,t2,t3;


  String sh1,sh2,sh3;
  String st1,st2,st3;
    
  String sensor1MessageT,sensor2MessageT,sensor3MessageT;
  String sensor1MessageH,sensor2MessageH,sensor3MessageH;
    
  bool bandSensor1T = false, bandSensor2T = false, bandSensor3T = false;
  bool bandSensor1H = false, bandSensor2H = false, bandSensor3H = false;

//====================================
//           WIFICONFIG 
//====================================
const char* ssid = "IZZI-284C";
const char* password =  "A811FC14284C";


//====================================
//             MQTT CONFIG 
//====================================

//const char *mqtt_server = "192.168.0.19"; //Local HOST
const char *mqtt_server = "35.239.161.254";
const int mqtt_port = 1883;

const char *root_topic_subscribeT = "/casa/temperatura/input";
const char *root_topic_publishT = "/casa/temperatura/output";

const char *root_topic_subscribeH = "/casa/humedad/input";
const char *root_topic_publishH = "/casa/humedad/output";

const char *mqtt_user = "";
const char *mqtt_pass = "";


//====================================
//               GLOBALES   
//====================================
WiFiClient espClient;

//PubSubClient client(espClient);// <------------------ 

PubSubClient clientT(espClient);
PubSubClient clientH(espClient);




//====================================
//           F U N C I O N E S 
//====================================
void callback(char* topic, byte* payload, unsigned int length);

void reconnectT();
void reconnectH();

void setup_wifi();

void setup() {
  

  Serial.begin(115200);
  dht1.begin();
  dht2.begin();
  dht3.begin();
  setup_wifi();
  
  clientT.setServer(mqtt_server, mqtt_port);
  clientT.setCallback(callback);
  
  clientH.setServer(mqtt_server, mqtt_port);
  clientH.setCallback(callback);
}

void loop() {

   if(clientT.connected() == true &&  bandSensor1T == true  && bandSensor2T == true  && bandSensor3T == true  && clientH.connected() == true && bandSensor1H == true  && bandSensor2H == true  && bandSensor3H == true){
     
      Serial.print(sensor1T());
      clientT.publish(root_topic_publishT, sensor1T().c_str());
      Serial.print(sensor2T());
      clientT.publish(root_topic_publishT, sensor2T().c_str());
      Serial.print(sensor3T());
      clientT.publish(root_topic_publishT, sensor3T().c_str());

      Serial.print(sensor1H());
      clientH.publish(root_topic_publishH, sensor1H().c_str());
      Serial.print(sensor2H());
      clientH.publish(root_topic_publishH, sensor2H().c_str());
      Serial.print(sensor3H());
      clientH.publish(root_topic_publishH, sensor3H().c_str());
      
    }

    //=============================================
    //    Activación de sensores de temperatura 
    //=============================================
    
    if(clientT.connected() == true && bandSensor1T == true){
      Serial.print(sensor1T());
      clientT.publish(root_topic_publishT, sensor1T().c_str());
    }
    
    if(clientT.connected() == true && bandSensor2T == true){
      Serial.print(sensor2T());
      clientT.publish(root_topic_publishT, sensor2T().c_str());
    }
    
    if(clientT.connected() == true && bandSensor3T == true){
      Serial.print(sensor3T());
      clientT.publish(root_topic_publishT, sensor3T().c_str());
    }

    //=============================================
    //    Activación de sensores de Humedad
    //=============================================
    
    if(clientH.connected() == true && bandSensor1H == true){
      Serial.print(sensor1H());
      clientH.publish(root_topic_publishH, sensor1H().c_str());
    }
    
    if(clientH.connected() == true && bandSensor2H == true){
      Serial.print(sensor2H());
      clientH.publish(root_topic_publishH, sensor2H().c_str());
    }
    
     if(clientH.connected() == true && bandSensor3H == true){
      Serial.print(sensor3H());
      clientH.publish(root_topic_publishH, sensor3H().c_str());
    }

  
  if (!clientT.connected()) {
    reconnectT();
  }

  if (clientT.connected()){
    Serial.print("cliente temperatura conectado\n");
    delay(10000);
    
  }

  if (!clientH.connected()) {
    reconnectH();
  }

  if (clientH.connected()){
    Serial.print("cliente humedad conectado\n");
    delay(10000);
    
  }
  clientT.loop();
  clientH.loop();
}

//====================================
//           CONEXION WIFI      
//====================================
void setup_wifi(){
  delay(10);
  Serial.println();
  Serial.print("Conectando a ssid: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a red WiFi!");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

//====================================
//             CONEXION MQTT      
//====================================

void reconnectT() {

  while (!clientT.connected()) {
    Serial.print("Intentando conexión Mqtt temperatura...\n");

    String clientId = "TemperaturaESP-UV-";
    clientId += String(random(0xffff), HEX) ;
 
    if (clientT.connect(clientId.c_str())) {
      Serial.println("Temperatura Conectado!\n");
      
      if(clientT.subscribe(root_topic_subscribeT)){
        Serial.println("Suscripcion temperatura: ON");
      }else{
        Serial.println("fallo Suscripciión de temperatura");
      }
      
    } else {
      Serial.print("falló :( con error ===> ");
      Serial.print(clientT.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void reconnectH() {

  while (!clientH.connected()) {
    Serial.print("Intentando conexión Mqtt humedad...\n");
    String clientId = "HumedadESP-UV-";
    clientId += String(random(0xffff), HEX) ;
  
    if (clientH.connect(clientId.c_str())) {
      Serial.println("Humedad Conectado!\n");
      
      if(clientH.subscribe(root_topic_subscribeH)){
        Serial.println("Suscripcion humedad: ON");
      }else{
        Serial.println("fallo Suscripciión de humedad");
      }
      
    } else {
      Serial.print("falló :( con error ===> ");
      Serial.print(clientH.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}


//====================================
//        CALLBACK   
//====================================

void callback(char* topic, byte* payload, unsigned int length){
 byte* p = (byte*)malloc(length);
   memcpy(p,payload,length);
  Serial.print("Se recibió mensaje:  ");
  Serial.print(topic);
  Serial.println("");

  String incoming = (char*)p;
  incoming.trim();

  String incomingVaidated = "";
  for (int i = 0; i <= 1; i++){
    incomingVaidated += (char)incoming[i];
 
  }
  Serial.println("Mensaje temperatura ==>  " + incomingVaidated + "\n\n");


Serial.println(topic);
Serial.println(root_topic_subscribeT);

//ACCIONES PARA TEMPERATURA
if(topic == root_topic_subscribeT ){
  
}

//ACCIONES PARA TEMPERATURA
if(topic == root_topic_subscribeH ){
  
}

//======================================
// APAGADO/ENCENDIDO TODO LOS SENSORES
//======================================
  if(incomingVaidated == "1"){
    bandSensor1T = true;
    bandSensor2T = true;
    bandSensor3T = true;
  }
  
  if(incomingVaidated == "0"){
    bandSensor1T = false;
    bandSensor2T = false;
    bandSensor3T = false;
  }

//============================
// APAGADO/ENCENDIDO SENSOR1
//============================
  if(incomingVaidated == "11"){
    bandSensor1T = true;
  }
  
  if(incomingVaidated== "01"){
    bandSensor1T = false;
  }

//============================
// APAGADO/ENCENDIDO SENSOR2
//============================

if(incomingVaidated == "12"){
    bandSensor2T = true;
  }
  
  if(incomingVaidated == "02"){
    bandSensor2T = false;
  }
//============================
// APAGADO/ENCENDIDO SENSOR3
//============================

if(incomingVaidated == "13"){
    bandSensor3T = true;
  }
  
  if(incomingVaidated == "03"){
    bandSensor3T = false;
  }


 free(p);
}


//====================================
//        CALLBACK HUMEDAD   
//====================================

void callbackH(char* topic, byte* payload, unsigned int length){
 byte* p = (byte*)malloc(length);
   memcpy(p,payload,length);
  Serial.print("Se recibió mensaje en Humedad:  ");
  Serial.print(topic);
  Serial.println("");

  String incoming = (char*)p;
  incoming.trim();

  String incomingVaidated = "";
  for (int i = 0; i <= 1; i++){
    incomingVaidated += (char)incoming[i];
 
  }
  Serial.println("Mensaje humedad ==>  " + incomingVaidated + "\n\n");


//======================================
// APAGADO/ENCENDIDO TODO LOS SENSORES
//======================================
  if(incomingVaidated == "1"){
    bandSensor1H = true;
    bandSensor2H = true;
    bandSensor3H = true;
  }
  
  if(incomingVaidated == "0"){
    bandSensor1H = false;
    bandSensor2H = false;
    bandSensor3H = false;
  }

//============================
// APAGADO/ENCENDIDO SENSOR1
//============================
  if(incomingVaidated == "11"){
    bandSensor1H = true;
  }
  
  if(incomingVaidated== "01"){
    bandSensor1H = false;
  }

//============================
// APAGADO/ENCENDIDO SENSOR2
//============================

if(incomingVaidated == "12"){
    bandSensor2H = true;
  }
  
  if(incomingVaidated == "02"){
    bandSensor2H = false;
  }
//============================
// APAGADO/ENCENDIDO SENSOR3
//============================

if(incomingVaidated == "13"){
    bandSensor3H = true;
  }
  
  if(incomingVaidated == "03"){
    bandSensor3H = false;
  }


 free(p);
}


String sensor1T(){
    t1 = dht1.readTemperature();
    st1 = String(t1);
    sensor1MessageT = "S1:"+ st1 +"°C,";
      return sensor1MessageT;
}

String sensor2T(){
    t2 = dht2.readTemperature();
    st2 = String(t2);
    sensor2MessageT = "S2:"+ st2 +"°C,";
      return sensor2MessageT;
}


String sensor3T(){
    t3 = dht3.readTemperature();
    st3 = String(t3);
    sensor3MessageT = "S3:"+ st3 +"°C";
      return sensor3MessageT;
}



String sensor1H(){
    h1 = dht1.readHumidity();  
    sh1 = String(h1);
    sensor1MessageH = "S1:"+ sh1 +"%,";
      return sensor1MessageH;
}

String sensor2H(){
    h2 = dht2.readHumidity();
    sh2 = String(h2);
    sensor2MessageH = "S2:"+ sh2 +"%,";
      return sensor2MessageH;
}

String sensor3H(){
    h3 = dht3.readHumidity();
    sh3 = String(h3);
    sensor3MessageH = "S3:"+ sh3 +"%";
      return sensor3MessageH;
}
