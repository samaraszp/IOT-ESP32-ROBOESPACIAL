// Pinos do Joystick Analógico
const int JOY_X_PIN = 34;
const int JOY_Y_PIN = 35;

// Botão Remoto de Desligar
const int BOTAO_DESLIGAR_PIN = 5;

// LEDs de Status do Controle Remoto
const int LED_VERDE_CTR = 12;
const int LED_VERMELHO_CTR = 13;

// Variável de estado do sistema de transmissão
bool controle_ativo = true;

void setup() {
  Serial.begin(115200);

  // Configuração dos pinos dos LEDs e Botão
  pinMode(BOTAO_DESLIGAR_PIN, INPUT_PULLUP);
  pinMode(LED_VERDE_CTR, OUTPUT);
  pinMode(LED_VERMELHO_CTR, OUTPUT);

  // Inicializa o controle remoto com status OK (Verde Aceso)
  digitalWrite(LED_VERDE_CTR, HIGH);
  digitalWrite(LED_VERMELHO_CTR, LOW);
  
  Serial.println("Controle Remoto Inicializado. Aguardando comandos...");
}

void loop() {
  // Verifica se o botão de desligar remoto foi pressionado
  if (digitalRead(BOTAO_DESLIGAR_PIN) == LOW) {
    if (controle_ativo) {
      controle_ativo = false;
      
      // Atualiza os LEDs indicadores do controle remoto
      digitalWrite(LED_VERDE_CTR, LOW);
      digitalWrite(LED_VERMELHO_CTR, HIGH);
      
      // Mensagens obrigatórias exigidas no Monitor Serial
      Serial.println("Comando enviado: DESLIGAR");
    }
    delay(300); // Debounce para evitar leituras falsas do botão
    return;
  }

  // Se o controle foi desativado pelo botão, ele trava o envio de comandos de movimento
  if (!controle_ativo) {
    delay(100);
    return;
  }

  // Leitura analógica dos eixos do Joystick (Valores de 0 a 4095 no ESP32)
  int xValue = analogRead(JOY_X_PIN);
  int yValue = analogRead(JOY_Y_PIN);

  // Lógica de mapeamento de zonas mortas para detecção das 4 direções
  if (yValue < 1500) {
    Serial.println("Comando: Frente");
  }
  else if (yValue > 2500) {
    Serial.println("Comando: Trás");
  }
  else if (xValue < 1500) {
    Serial.println("Comando: Esquerda");
  }
  else if (xValue > 2500) {
    Serial.println("Comando: Direita");
  }

  delay(200); // Pequena pausa para evitar excesso de mensagens no Monitor Serial
}