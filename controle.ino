// Pinos do Joystick
const int JOY_X_PIN = 34;
const int JOY_Y_PIN = 35;

// Botão
const int BOTAO_DESLIGAR_PIN = 5;

// LEDs
const int LED_VERDE_CTR = 12;
const int LED_VERMELHO_CTR = 13;

bool controle_ativo = true;

void setup() {
  Serial.begin(115200);

  pinMode(BOTAO_DESLIGAR_PIN, INPUT_PULLUP);
  pinMode(LED_VERDE_CTR, OUTPUT);
  pinMode(LED_VERMELHO_CTR, OUTPUT);

  digitalWrite(LED_VERDE_CTR, HIGH);
  digitalWrite(LED_VERMELHO_CTR, LOW);
  
  Serial.println("Controle Remoto Inicializado. Aguardando comandos...");
}

void loop() {
  if (digitalRead(BOTAO_DESLIGAR_PIN) == LOW) {
    if (controle_ativo) {
      controle_ativo = false;
      
      digitalWrite(LED_VERDE_CTR, LOW);
      digitalWrite(LED_VERMELHO_CTR, HIGH);
      
      Serial.println("Comando enviado: DESLIGAR");
    }
    delay(300); // Debounce
    return;
  }

  if (!controle_ativo) {
    delay(100);
    return;
  }

  // Leitura analógica dos eixos do Joystick (Valores de 0 a 4095 no ESP32)
  int xValue = analogRead(JOY_X_PIN);
  int yValue = analogRead(JOY_Y_PIN);

  if (yValue < 1500) {
    Serial.println("Comando: Frente");
  }
  // Trás -> Motores para trás
  else if (yValue > 2500) {
    Serial.println("Comando: Trás");
  }
  // Esquerda -> Motor direito ativo
  else if (xValue < 1500) {
    Serial.println("Comando: Esquerda");
  }
  // Direita -> Motor esquerdo ativo
  else if (xValue > 2500) {
    Serial.println("Comando: Direita");
  }

  delay(200); 