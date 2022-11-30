
#include <WiFi.h>


// Replace the next variables with your SSID/Password combination
const char* ssid = "showroom";
const char* password = "wt0meevfc6";


WiFiClient espClient;

void setup_wifi();

void setup() {
  Serial.begin(9600);
  // default settings
  // (you can also pass in a Wire library object like &Wire2)
 

  setup_wifi();


 
}

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




void loop() {

  Serial.print("Connecté?");
  delay(2000);

}