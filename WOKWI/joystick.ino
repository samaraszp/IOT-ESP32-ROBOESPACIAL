// Pinos do Joystick Analógico
const int joyPin_X = 34;
const int joyPin_Y = 35;

// Botão remoto de desligar
const int botao_off = 5;

// LEDs de status do controle joystick
const int led_VerdeCTR = 12;
const int led_VermelhoCTR = 13;

// Variável de estado p/ sistema de transmissão
bool controle_ativo = true;

void setup() {
  Serial.begin(115200);

  // Configuração dos pinos dos LEDs e do botão
  pinMode(botao_off, INPUT_PULLUP);
  pinMode(led_VerdeCTR, OUTPUT);
  pinMode(led_VermelhoCTR, OUTPUT);

  // Inicializa o controle remoto com status OK (Verde Aceso)
  digitalWrite(led_VerdeCTR, HIGH);
  digitalWrite(led_VermelhoCTR, LOW);
  
  Serial.println("Controle Remoto Inicializado. Aguardando comandos...");
}

void loop() {
  // Verifica se o botão de desligar remoto foi pressionado
  if (digitalRead(botao_off) == LOW) {
    if (controle_ativo) {
      controle_ativo = false;
      
      // Atualiza os LEDs de status do controle joystick
      digitalWrite(led_VerdeCTR, LOW);
      digitalWrite(led_VermelhoCTR, HIGH);
      
      // Mensagens obrigatórias exigidas no monitor serial
      Serial.println("Comando enviado: DESLIGAR");
    }
    delay(300); // Debounce para evitar leituras falsas no botão
    return;
  }

  // Se o controle foi desativado pelo botão ele trava o envio de comandos de movimento
  if (!controle_ativo) {
    delay(100);
    return;
  }

  // Leitura analógica dos eixos do Joystick - Valores de 0 a 4095 no ESP32
  int xValue = analogRead(joyPin_X);
  int yValue = analogRead(joyPin_Y);

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

  delay(200); // Pausa para evitar excesso de mensagens no monitor serial
}