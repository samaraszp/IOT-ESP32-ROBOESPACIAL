# Projeto Robô Explorador Espacial
### Componente Curricular: Práticas Integradas (Bloco 4)

O projeto consiste no ecossistema completo de um robô explorador espacial projetado para mapear e calcular a probabilidade de existência de vida extraterrestre em planetas simulados. A solução integra hardware microcontrolado simulado (ESP32), sensores ambientais, atuadores para locomoção, notificações automatizadas via WhatsApp (CallmeBot) e persistência local de dados através de uma API desenvolvida em Python conectada a um banco de dados relacional.



## Integrantes do Grupo:
- Jessica Campos
- Ryan Luz
- Samara Souza

Projeto no WOKWI: https://wokwi.com/projects/464384424690153473

## 📁 Estrutura do Repositório

```text
IOT-ESP32-ROBOESPACIAL/
├── WOKWI/
│   ├── diagram.json      # Esquemático físico e conexões da bancada virtual no Wokwi
│   ├── joystick.ino      # Código específico para tratamento dos eixos do Joystick
│   └── robo.ino          # Código principal de percepção, cálculo e rede do Robô
├── .env                  # Variáveis de ambiente locais e privadas (Com chaves e portas)
├── .env.example          # Modelo de configuração para variáveis de ambiente
├── .gitignore            # Instruções para o Git ignorar arquivos sensíveis (como o .env)
├── requirements.txt      # Dependências e bibliotecas necessárias do ecossistema Python
├── robo_app.py           # Backend API estruturado em Python para recepção dos dados
├── schema.sql            # Script SQL de definição estrutural do banco de dados
└── tabela.sql            # Script complementar para consultas e manipulação do banco
