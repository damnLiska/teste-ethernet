#include <PubSubClient.h> //Biblioteca que faz a comunicção MQTT
#include <WiFi.h>


// -------- Conexão a rede WiFi
const char* ssid = "LISKA";
const char* password = "liska@123";

// -------- Token e ID do dispositivo para coneção thingsboard
#define TOKEN "Qii2kpeEyT7E0iK6rYue" //Access token of device Display
#define ID_DEVICE "00ef3350-9132-11ea-a3df-b1bca87d6c8b" // Device ID to access

// --------
#define ANALOG_PIN 36

// -------- Instanciando as classes. Obs: a PubSub precisa da internet, caso for usar Ethernet, passa o instancia Ethernet
WiFiClient wifiClient;
PubSubClient client(wifiClient);
int status = WL_IDLE_STATUS; //Status inicial, sem coneção

int analog_value = 0;
IPAddress server(200, 134, 31, 225); // IP do servidor MQTT UTFPR

void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("connected to");
  Serial.println(ssid);
  client.setServer( server, 1883 ); //Salva o endereço de comunicação com a ThingsBoard
}



void getAndSendData(){

  analog_value = analogRead(ANALOG_PIN);
  Serial.println(analog_value);
  // Cria um modelo de json para enviar
  String payload = "{";
 payload += "\"Ozonio\":";
 payload += 10;
 payload += ",";
 payload += "\"Temperature\":";
 payload += 25;
 payload += "}";


  char attributes[1000];

  payload.toCharArray( attributes, 1000 );

  //publica para o gateway. obs: Sempre vai cair no dispositivo do ID, podendo filtrar pelos atributos do payload
  client.publish( "v1/devices/me/telemetry",attributes);
  

}




void reconnect() {
  // Loop até se conectar

  while (!client.connected()) {
    status = WiFi.status();
    //referente ao wifi, se não encontrar rede
    if ( status != WL_CONNECTED) {
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Inicia a conexão (clientId, token, password)
    if ( client.connect(ID_DEVICE, TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.println( " : retrying in 2 seconds]" );
      delay( 2000 );
    }
  }
}

void loop(){
  //Começa a conexão com t TB
  if ( !client.connected() ){
      reconnect();
  }
  getAndSendData();
  delay(1000);
}
