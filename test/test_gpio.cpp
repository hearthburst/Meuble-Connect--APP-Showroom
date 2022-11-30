#include <Arduino.h>
//alim en 3,3V
//pin d'entrée
int PIN0=36;
int PIN1=39;
int PIN2=34;
int PIN3=35;
int PIN4=32;
int PIN5=33;
int PIN6=25;
int PIN7=26;
int PIN8=27;
int PIN9=14;

int pin[10]={PIN0,PIN1,PIN2,PIN3,PIN4,PIN5,PIN6,PIN7,PIN8,PIN9};

//init read
double x0,x1,x2,x3,x4,x5,x6,x7,x8,x9;
double res[10]={x0,x1,x2,x3,x4,x5,x6,x7,x8,x9};


void initPin(){

    for (size_t i = 0; i < 10; i++)
    {
        pinMode(pin[i], INPUT);
    }
    
}

void readPin(){
    for (size_t i = 0; i < 10; i++)
    {
        /* code */
        res[i]=analogRead(pin[i]);
        
    }
    
}

String createJson(){

     readPin();
    //debut string
    String JSon = "[";

    for (size_t i = 0; i < 10; i++)
    {
        //num capteur
        String num_capteur= "{\"PIR";
        num_capteur+=i;
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

void publishtempo(){
    long temps = millis();
    if(temps >= 6000)
    {   
        Serial.println(createJson());
        temps = 0;
    }

}




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  initPin();

}

void loop() {
  // put your main code here, to run repeatedly:
  


    publishtempo();

}


 