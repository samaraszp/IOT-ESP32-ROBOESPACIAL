// Wi-fi e whatsapp
#include <WiFi.h>
#include <HTTPClient.h>
#include <UrlEncode.h>

// Mandar mensagem para Whatsapp
// +international_country_code + phone number
String phoneNumber = "+557186378606";
String apiKey = "4740574";

void sendMessage(String message){

  // Data to send with HTTP POST
  String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);    
  HTTPClient http;
  http.begin(url);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
 
  // Send HTTP POST request
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

// Sensor de temperatura e humidade
#include "DHT.h"
const int DHT_PIN = 4;
#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);
float temp;
float hum;

// Mortor servoPWM analogWrite
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
bool ultimo_estado = true;  // Armazena o estado anterior

void IRAM_ATTR handleButtonInterrupt() {
  sistema_ativo = !sistema_ativo;  // Inverte o estado atual liga ou desligg
}

// Timer
unsigned long previousMillis = 0;
const long interval = 2000;        // Intervalo para leitura dos sensores de 2s

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

  // Send Message to WhatsAPP
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

  Serial.print("Valor calculated da probabilidade de vida (%): ");
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
}

void loop() {
  unsigned long inicio = millis();
  // Verifica se houve mudança de estado
  if (sistema_ativo != ultimo_estado) {
    ultimo_estado = sistema_ativo;

    if (sistema_ativo) {
      Serial.println("Sistema ligado.");
      // Reanexar os servos
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
  // Se o sistema está desligado, nn faz mais nada no loop
  if (!sistema_ativo) {
    delay(100);
    return;
  }

  unsigned long currentMillis = millis();
  // Leitura dos sensores a cada intervalo de tempo 
  if (currentMillis - previousMillis >= interval) {
    // Salva o ultimo tempo
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
  unsigned long fim = millis();
  //Serial.print("Tempo do loop (ms): ");
  //Serial.println(fim - inicio);
}