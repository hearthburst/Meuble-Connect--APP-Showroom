////////////////////////////Partie Wifi et MQTT/////////////////////////////////////////////////////:
//Init Wifi et MQTT
#include <WiFi.h>
#include <PubSubClient.h>
#include <String.h>
#include <cstring>

// Update these with values suitable for your network.
const char* ssid = "showroom";
const char* password = "wt0meevfc6";
const char* mqtt_server = "10.100.100.117";
#define mqtt_port 443
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#define MQTT_SERIAL_PUBLISH_CH "/showroom/PIR"
#define MQTT_SERIAL_RECEIVER_CH "/showroom/PIR"
#define MQTT_TEST "HELLO "

//Timing init et intervalle
long before=0;
long timeout= 300;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

//Fonctions Wifi et MQTT 

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      //Once connected, publish an announcement...
      //client.publish(MQTT_SERIAL_PUBLISH_CH, "hello world");
      // ... and resubscribe
      //client.subscribe(MQTT_SERIAL_RECEIVER_CH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//lorsque qu'un message est recu sur un topic
void callback(char* topic, byte *payload, unsigned int length) {
    Serial.println("-------new message from broker-----");
    Serial.print("channel:");
    Serial.println(topic);
    Serial.print("data:");  
    Serial.write(payload, length);
    Serial.println();
}

//convertit la string en tableau de char et publie sur le topic
void publishData(String data){

  if (!client.connected()) {
    reconnect();
  }

  //conversion
  char* payload= new char [data.length()+1];
  std::strcpy(payload, data.c_str());
  
  int i;  
  for (i = 0; i < sizeof(payload); i++) {
      payload[i] = data[i];
  }
  //publish
  client.publish(MQTT_SERIAL_PUBLISH_CH, payload);
    //test
    //client.publish(MQTT_SERIAL_PUBLISH_CH, data.c_str());
    
}

//ajoute un delais entre 2 publish
void publishTempodData(String data){
    long now = millis();
    
    if(now-before >= timeout)
    {   
        publishData(data);
        before= now;
    }

}

////////////////////////////Partie Acquisition et format///////////////////////////////////////////////
//Init acquisition
#include <Arduino.h>

float coeffH=0.6;
float coeffL=0.4;
//alim en 3,3V
//pin d'entrée
int PIN0=36;
int PIN1=39;
int PIN2=34;
int PIN3=35;
int const nbPIN=4;
int const offsetPin=0;


int pin[nbPIN]={PIN0,PIN1,PIN2,PIN3};

//init read
float x0,x1,x2,x3;
float x0b,x1b,x2b,x3b;
float x0max,x1max,x2max,x3max;
float res[nbPIN]={x0,x1,x2,x3};
float resmax[nbPIN]={x0max,x1max,x2max,x3max};
float resbool[nbPIN]={x0b,x1b,x2b,x3b};

//Fonctions acquisition et Json
void initPin(){
    for (size_t i = 0; i < nbPIN; i++)
    {
        //output mode
        pinMode(pin[i], INPUT);
    }    
}


boolean filtreTest(float a, float x_max){

  float high= coeffH*x_max;
  float low= x_max*coeffL;

  if ((a<low) || (a>high))
  {
    return true;
  }
  else
  {
    return false;
  }
}


float valmax(float x,float x_max){

  if (x > x_max)
  {
    x_max=x;
  }
  return x_max;
}



//creation d'une string prete à l'envoi
String createJson(float table[]){
    //lecture des pin
    
    //debut string
    String JSon = "[";
    for (size_t i = 0; i < nbPIN; i++)
    {
        //num capteur
        String num_capteur= "{\"PIR";
        num_capteur+=i+offsetPin;
        num_capteur+="\":";
        //concat JSon
        JSon+= num_capteur;
        JSon+= table[i];
        JSon+= "},";
    }
    //fermeture JSon
    //retire la derniere virgule
    JSon= JSon.substring(0,JSon.length()-1);
    JSon+="]";
    //retourne la valeur
    return JSon;
}

void readPin(){
  bool publish=false;
    for (size_t i = 0; i < nbPIN; i++)
    {
        //lecture directe
        res[i]=analogRead(pin[i]);
        //resmax[i]=valmax(res[i],resmax[i]);
        resmax[i]=2400;
        resbool[i]=filtreTest(res[i],resmax[i]);
        
        if (resbool[i])
        {
          publish=true;
        }   

    }   
  if (publish)
  {
    publishTempodData(createJson(resbool)); 
    
  }
  Serial.print("val,");
  Serial.println(createJson(res));
  Serial.print("max,");
  Serial.println(createJson(resmax));
  publish=false;
  
    
}



////////////////////////////////Set up + Loop/////////////////////////////////////////////////////:
void setup() {
    Serial.begin(9600);

    //Wifi/MQTT setup
    setup_wifi();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    reconnect();

    //Acquisition setup
    initPin();

}


void loop() {
  
    client.loop();

    //Publie diretement une Sting Json
    readPin();

}

 
