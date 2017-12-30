/**
Copyright 2017 Bruno Horta
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
//MQTT - como instalar -> https://www.youtube.com/watch?v=GMMH6qT8_f4
#include <PubSubClient.h>
//ESP - como instalar -> https://www.youtube.com/watch?v=4d8joORYTIA&t=1s
#include <ESP8266WiFi.h>
//Wi-Fi Manager  como instalar -> https://www.youtube.com/watch?v=wWO9n5DnuLA
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>



// Configuração do acesso ao Broker MQTT
#define MQTT_AUTH false
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""

//Constantes
const String HOSTNAME  = "MySuperProject"; //NOME DO DEVICE, este nome tambem é utilizado apra criar o Access Point para configuração
const char * MQTT_LOG = "system/log"; // Topico onde o Device Publica informações relacionadas com o sistema
const char * MQTT_SYSTEM_CONTROL_TOPIC = "system/set"; // Topico onde o Device subscreve para aceitar instruções de sistema

const char * MQTT_TEST_TOPIC = "superTopic"; //Topico de exemplo onde o Device subscreve (por exemplo controlar uma lâmpada)
//MQTT BROKERS GRATUITOS PARA TESTES https://github.com/mqtt/mqtt.github.io/wiki/public_brokers
const char* MQTT_SERVER = "IP_DNS_BROKER"; //IP ou DNS do Broker MQTT

WiFiClient wclient;
PubSubClient client(MQTT_SERVER,1883,wclient);

void setup() {
  Serial.begin(115200);
  WiFiManager wifiManager;
 
  //wifiManager.resetSettings(); //Limpa a configuração anterior do Wi-Fi SSID e Password, procedimento, 1º descomentar a linha, 2º Fazer Upload do código para o ESP e deixar o ESP arrancar, 3º Voltar a comentar a linha e enviar novamente o código para o ESP
  /*define o tempo limite até o portal de configuração ficar novamente inátivo,
   útil para quando alteramos a password do AP*/
  wifiManager.setTimeout(180);
  wifiManager.autoConnect(HOSTNAME.c_str());
  client.setCallback(callback); //Registo da função que vai responder ás mensagens vindos do MQTT
}

//Chamada de recepção de mensagem 
void callback(char* topic, byte* payload, unsigned int length) {
  String payloadStr = "";
  for (int i=0; i<length; i++) {
    payloadStr += (char)payload[i];
  }
  String topicStr = String(topic);
 if(topicStr.equals(MQTT_SYSTEM_CONTROL_TOPIC)){
  if (payloadStr.equals("REBOOT_"+String(HOSTNAME))){
    Serial.println("REBOOT");
    ESP.restart();
  }
 }else if(topicStr.equals(MQTT_TEST_TOPIC)){
  //TOPICO DE TESTE
    Serial.println(payloadStr);
  } 
} 
  
bool checkMqttConnection(){
  
  if (!client.connected()) {
    if (MQTT_AUTH ? client.connect(HOSTNAME.c_str(),MQTT_USERNAME, MQTT_PASSWORD) : client.connect(HOSTNAME.c_str())) {
      Serial.println("CONNECTED TO MQTT BROKER "+String(MQTT_SERVER));
      client.publish(MQTT_LOG,String("CONNECTED_"+HOSTNAME).c_str());
      //SUBSCRIÇÃO DE TOPICOS
      client.subscribe(MQTT_SYSTEM_CONTROL_TOPIC);
      client.subscribe(MQTT_TEST_TOPIC);
    }
  }
  return client.connected();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (checkMqttConnection()){
      client.loop();
    }
  }
}


