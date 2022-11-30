////////////////////////////Partie Wifi et MQTT/////////////////////////////////////////////////////:
//Init Wifi et MQTT
#include <WiFi.h>
#include <PubSubClient.h>
#include <String.h>
#include <cstring>



//Timing init et intervalle
long before=0;
long timeout= 100;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

//Fonctions Wifi et MQTT 



//convertit la string en tableau de char et publie sur le topic
void publishData(String data){


    Serial.println(data);
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

float coeffH=0.65;
float coeffL=0.35;
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
double x0b,x1b,x2b,x3b;
double x0max,x1max,x2max,x3max;
double res[nbPIN]={x0,x1,x2,x3};
double resmax[nbPIN]={x0max,x1max,x2max,x3max};
double resbool[nbPIN]={x0b,x1b,x2b,x3b};

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

void readPin(){
    for (size_t i = 0; i < nbPIN; i++)
    {
        //lecture directe
        res[i]=analogRead(pin[i]);
        resmax[i]=valmax(res[i],resmax[i]);
        resbool[i]=filtreTest(res[i],resmax[i]);

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
        JSon+= resbool[i];
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

    //Acquisition setup
    initPin();

}


void loop() {
  
    client.loop();

    //Publie diretement une Sting Json
    publishTempodData(createJson());

}

 
