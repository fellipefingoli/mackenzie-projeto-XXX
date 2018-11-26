#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Stepper.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define DS18B20 D7

OneWire ourWire(DS18B20);
DallasTemperature sensors(&ourWire);

//Configuracao de Server MQTT e Wifi.
const char* ssid = "Xaomi";//Colocar SSID
const char* password = "xaomi123";//Colocar senha wifi
const char* mqtt_server = "m15.cloudmqtt.com";
const char* userName = "afcoawbx";
const char* passWord = "LKe6bRS1j4xq";
const char* clientId = "ESP8266Client-1";


const int stepsPerRevolution = 400;  
// Usando os pinos D1,D2,D5,D6 para o Motor De passo girar
Stepper myStepper(stepsPerRevolution, D1, D2, D5, D6);
WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {

  while (!client.connected()) 
  {
    Serial.print("Conectando no MQTT ...");
    // Tentando
    if (client.connect(clientId,userName,passWord))
    {
      Serial.println("connected");
     //Se Conectou define a Subscribe
      client.subscribe("/aquario/alimento");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retentando em 5 segundos");
      // esperando 6 segundos
      delay(6000);
    }
  }
}


void setup_wifi() {
   delay(100);
    Serial.print("Conectando no ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.println("Endereco IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Command from MQTT broker is : ");
  Serial.print(topic);
  int p =(char)payload[0]-'0';
  // Giro Completo:
  if(p==1) 
  {
    client.publish("/aquario/mensagem", "Girando");    
    myStepper.step(stepsPerRevolution);
    delay(500);
    myStepper.step(stepsPerRevolution);
    delay(500);
    myStepper.step(stepsPerRevolution);
    delay(500);
    myStepper.step(stepsPerRevolution);
    delay(500);
    myStepper.step(stepsPerRevolution);
    Serial.print("  Fim 1" );
   }
  // Meio Giro:
  else if(p==2)
  {
    client.publish("/aquario/mensagem", "Girando");
    delay(500);
    myStepper.step(-stepsPerRevolution);
    delay(500);
    myStepper.step(-stepsPerRevolution);
    delay(500);
    myStepper.step(-stepsPerRevolution);
    Serial.print("  Fim 2" );

   }
   Serial.println();
   client.publish("/aquario/mensagem", "Peixe Alimentado");

}

 


void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 11467);
  client.setCallback(callback);
  myStepper.setSpeed(60);
  
}



void loop() {
  if (!client.connected()) {
    reconnect();
  }
  sensors.requestTemperatures();
  char str[16];
  float temp = sensors.getTempCByIndex(0);
  snprintf(str, sizeof(str), "%.2f", temp);

  client.publish("/aquario/temp", str);
  
  client.loop();

}
