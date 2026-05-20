// Wifi e whatsapp
#include <WiFi.h>
#include <HTTPClient.h>
#include <UrlEncode.h>

// Mensagem para whatsapp
String phoneNumber = "+557186378606";
String apiKey = "4740574";

// URL do Backend Python
// No Wokwi para acessar o localhost da máquina local usa o IP 10.0.1.2
String serverUrl = "http://10.0.1.2:8000/leituras";

void sendMessage(String message){
  String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);    
  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
 
  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200){
    Serial.print("Message sent successfully");
  }
  else{
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

// Função para enviar os dados para o Banco de Dados via API Python
void enviarDadosServidor(float t, float h, float l, int p, float prob) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    
    String jsonPayload = "{\"temperatura_c\":" + String(t) + 
                         ",\"umidade_pct\":" + String(h) + 
                         ",\"luminosidade\":" + String(l) + 
                         ",\"presenca\":" + String(p) + 
                         ",\"probabilidade_vida\":" + String(prob) + "}";

    int httpResponseCode = http.POST(jsonPayload);
    
    if (httpResponseCode > 0) {
      Serial.print("Dados enviados ao servidor. Código de resposta: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Falha ao enviar dados via HTTP POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Erro: Wi-Fi desconectado. Não foi possível enviar os dados.");
  }
}

// Sensor de temperatura e humidade
#include "DHT.h"
const int DHT_PIN = 4;
#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);
float temp; 
float hum; 

// Mortor servo PWM
#include <ESP32Servo.h>
const int SERVO1_PIN = 27;
const int SERVO2_PIN = 25;
Servo servo1;
Servo servo2;

// Fotorresistor
const int LDR_PIN = 35;
int luz;
float voltage;
float resistance;
float lux;
const float GAMMA = 0.7;
const float RL10 = 50;

// Sensor de Presença
const int PIR_PIN = 14;
bool presenca = false;

// LEDs
const int LED_VERDE_PIN = 12;
const int LED_VERMELHO_PIN = 13;

// Mensagem
bool msgEnviada = false;

// Interrupcao
const int BOTAO_PIN = 5;
volatile bool sistema_ativo = true;
bool ultimo_estado = true;  

void IRAM_ATTR handleButtonInterrupt() {
  sistema_ativo = !sistema_ativo;  
}

// Timer
unsigned long previousMillis = 0; 
const long interval = 2000;        

void setup() {
  Serial.begin(115200);
  dht.begin();

  servo1.attach(SERVO1_PIN, 500, 2400);
  servo2.attach(SERVO2_PIN, 500, 2400);

  pinMode(PIR_PIN, INPUT);
  pinMode(LED_VERDE_PIN, OUTPUT);
  pinMode(LED_VERMELHO_PIN, OUTPUT);

  digitalWrite(LED_VERDE_PIN, LOW);
  digitalWrite(LED_VERMELHO_PIN, LOW);

  Serial.println("Conectando-se ao Wi-Fi");
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("Conectado!");

  pinMode(BOTAO_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BOTAO_PIN), handleButtonInterrupt, RISING);

  sendMessage("Um olá do ESP32 para vocês, Laura e Samara!");
}

void lerTempUmid() {
  hum = dht.readHumidity();
  temp = dht.readTemperature();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("A leitura do sensor de temperatura e umidade falhou.");
    return;
  }
  else {
    Serial.println("  ");
    Serial.print("Temp: ");
    Serial.print(temp);
    Serial.println(" ºC");
    Serial.print("Umid: ");
    Serial.print(hum);
    Serial.println(" %");
  }
}

void lerFotorresistor() {
  luz = analogRead(LDR_PIN);
  voltage = luz / 4096. * 5;
  resistance = 2000 * voltage / (1 - voltage / 5);
  lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));
  Serial.print("Fotorresistor: ");
  Serial.print(lux);
  Serial.println(" lux");
}

void lerPresenca() {
  presenca = digitalRead(PIR_PIN);
  if (presenca) {
    Serial.println("Sensor de Presença: Presença detectada");
  } else {
    Serial.println("Sensor de Presença: Sem presença");
  }
}

void processarLogicaRobo() {
  int probabilidade_vida = 0;

  if (temp >= 15.0 && temp <= 30.0) {
    probabilidade_vida += 25;
  }
  if (hum >= 40.0 && hum <= 70.0) {
    probabilidade_vida += 25;
  }
  if (lux > 500.0) {
    probabilidade_vida += 20;
  }
  if (presenca) {
    probabilidade_vida += 30;
  }

  Serial.print("Valor calculado da probabilidade de vida (%): ");
  Serial.print(probabilidade_vida);
  Serial.println("%");

  if (probabilidade_vida <= 75) {
    digitalWrite(LED_VERDE_PIN, HIGH);
    digitalWrite(LED_VERMELHO_PIN, LOW);
    Serial.println("Estado do robô: LIGADO");
    Serial.println("Exploração normal. Nenhum indício relevante detectado.");
    msgEnviada = false; 
  } else {
    digitalWrite(LED_VERDE_PIN, LOW);
    digitalWrite(LED_VERMELHO_PIN, HIGH);
    Serial.println("Estado do robô: ALERTA");
    Serial.println("ALERTA! Alta probabilidade de vida detectada!");
    
    if (!msgEnviada) {
      sendMessage("Alerta! Alta probabilidade de vida detectada no planeta.");
      msgEnviada = true;
    }
  }

  // Executa o envio dos dados em tempo real p o banco de dados
  enviarDadosServidor(temp, hum, lux, (presenca ? 1 : 0), (float)probabilidade_vida);
}

void loop() {
  if (sistema_ativo != ultimo_estado) {
    ultimo_estado = sistema_ativo;

    if (sistema_ativo) {
      Serial.println("Sistema ligado.");
      servo1.attach(SERVO1_PIN, 500, 2400);
      servo2.attach(SERVO2_PIN, 500, 2400);
    } else {
      Serial.println("Sistema desligado. Pressione o botão para ligar.");
      servo1.detach();
      servo2.detach();
      digitalWrite(LED_VERDE_PIN, LOW);
      digitalWrite(LED_VERMELHO_PIN, HIGH);
    }
  }

  if (!sistema_ativo) {
    delay(100);
    return;
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    lerTempUmid();
    lerFotorresistor();
    lerPresenca();
    processarLogicaRobo();
  }

  for(int posDegrees = 0; posDegrees <= 180; posDegrees++) {
    servo1.write(posDegrees);
    servo2.write(posDegrees);
    delay(1);
  }
  for(int posDegrees = 180; posDegrees >= 0; posDegrees--) {
    servo1.write(posDegrees);
    servo2.write(posDegrees);
    delay(1);
  }
}