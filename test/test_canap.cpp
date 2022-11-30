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
#define MQTT_SERIAL_PUBLISH_CH "/showroom/canap"
#define MQTT_SERIAL_RECEIVER_CH "/showroom/canap"
#define MQTT_TEST "HELLO "

//Timing init et intervalle
long before=0;
long timeout= 500;

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
#include <Arduino.h>


//déclaration variable

int PIN1=39;
int PIN2=34;
int PIN3=35;
int PIN4=32;


//mesure du capteur a vide
// int const VX1   = analogRead(PIN1);
// int const VX2   = analogRead(PIN2);
// int const VX3   = analogRead(PIN3);
// int const VX4   = analogRead(PIN4);
int const VX1   = 0;
int const VX2   = 0;
int const VX3   = 0;
int const VX4   = 0;

    
float V1,V2,V3,V4;
float moyv1,moyv2,moyv3,moyv4;
float diffx,diffy;
float moydifx,moydify;

//decla tableaux
unsigned int const nbval=10;
float Val1[nbval],Val2[nbval],Val3[nbval],Val4[nbval];
unsigned int const nbdiff=5;
float tdiffx[nbdiff],tdiffy[nbdiff];



//fonctions

void readVal(){

    for (size_t i = 0; i < nbval-1; i++)
    {
        /* code */
        Val1[i]=Val1[i+1];
        Val2[i]=Val2[i+1];
        Val3[i]=Val3[i+1];
        Val4[i]=Val4[i+1];
    }

    V1   = analogRead(PIN1)-VX1;
    V2   = analogRead(PIN2)-VX2;
    V3   = analogRead(PIN3)-VX3;
    V4   = analogRead(PIN4)-VX4;

    Val1[nbval-1]=V1;
    Val2[nbval-1]=V2;
    Val3[nbval-1]=V3;
    Val4[nbval-1]=V4;

}

float moyenne(float tab[], unsigned int l){

    int somme=0;
    float m;
    for (size_t i=0 ; i< l ; i++)
    {
        somme += tab[i];
    }
    m = somme / (float)l;
    return m;

}

void moyenneVal(){

    readVal();
    moyv1= moyenne(Val1,nbval);
    moyv2= moyenne(Val2,nbval);
    moyv3= moyenne(Val3,nbval);
    moyv4= moyenne(Val4,nbval);

}

void calculModif(){

    moyenneVal();
    diffx=moyv1+moyv2-moyv3-moyv4;
    diffy=moyv2+moyv4-moyv1-moyv3;
    for (size_t i = 0; i < nbdiff-1; i++)
    {
        tdiffx[i]=tdiffx[i+1];
        tdiffy[i]=tdiffy[i+1];
    }
    tdiffx[nbdiff-1]=diffx;
    tdiffy[nbdiff-1]=diffy;

    moydifx=moyenne(tdiffx,nbdiff);
    moydify=moyenne(tdiffy,nbdiff);

}

String createJson(){

    //Mesure les valeurs fait les moyennes et les diff
    calculModif();
    
    //x
    String JSon = "[{\"x\":";
    JSon+= moydifx;
    JSon+= "},";
    //y
    JSon+="{\"y\":";
    JSon+= moydify;
    JSon+= "}]";
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
    pinMode(PIN1, INPUT);
    pinMode(PIN2, INPUT);
    pinMode(PIN3, INPUT);
    pinMode(PIN4, INPUT);

}


void loop() {
  
    client.loop();

    //Publie diretement une Sting Json
    publishTempodData(createJson());
    // Serial.print(V1);
    // Serial.print(';');
    
    // Serial.print(V2);
    // Serial.print(';');
    // Serial.print(V3);
    // Serial.print(';');
    // Serial.print(V4);
    // Serial.print(';');
    // Serial.print(moydifx);
    // Serial.print(';');
    // Serial.print(moydify);
    // Serial.println(';');
    // delay(100);


}

 
