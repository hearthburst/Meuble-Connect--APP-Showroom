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
long timeout= 1000;

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
      client.subscribe(MQTT_SERIAL_RECEIVER_CH);
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
double x0,x1,x2,x3;
double res[nbPIN]={x0,x1,x2,x3};

//Fonctions acquisition et Json
void initPin(){
    for (size_t i = 0; i < nbPIN; i++)
    {
        //output mode
        pinMode(pin[i], INPUT);
    }    
}

void readPin(){
    for (size_t i = 0; i < nbPIN; i++)
    {
        //lecture directe
        res[i]=analogRead(pin[i]);
    }    
}

//creation d'une string prete à l'envoi
String createJson(){
    //lecture des pin
    readPin();
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
        JSon+= res[i];
        JSon+= "},";
    }
    //fermeture JSon
    //retire la derniere virgule
    JSon= JSon.substring(0,JSon.length()-1);
    JSon+="]";
    //retourne la valeur
    return JSon;
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
    publishTempodData(createJson());

}

 
