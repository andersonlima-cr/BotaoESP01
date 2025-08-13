#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#include <ESP8266Ping.h>
#include <DNSServer.h>

/*****************************DEFINIÇÃO DE VARIÁVEIS************************/

// ⚠️ Altere aqui, se quiser IP fixo
IPAddress local_IP(192, 168, 1, 25);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// ⚙️ Configurações do servidor
const int porta = 8081;
ESP8266WebServer server(porta);

// 🔐 Login
const char* authUser = "admin";
const char* authPass = "1234";

// 🖲️ Porta do relé
const int relePin = 0;
const int resetButtonPin = 2;

// Configurações de monitoramento
const char* hostMonitorado = "192.168.1.100"; // IP ou domínio do computador
const unsigned long intervaloPing = 60000; // 1 minuto em ms
const int falhasMaximas = 20; // 20 falhas consecutivas para acionar

// Variáveis de controle
unsigned long ultimoPing = 0;
int contadorFalhas = 0;

/**************************FUNÇÕES DO CÓDIGO*************************/

void handleRoot() {
  // Requer autenticação
  if (!server.authenticate(authUser, authPass)) {
    return server.requestAuthentication();
  }

  // Se for navegador (root /), retorna vazio
  server.send(204); // No content
}

void handlePowerOff() {
  Serial.println("Acionando relé por 3 segundos...");
  digitalWrite(relePin, LOW);   // Liga relé
  delay(3000);
  digitalWrite(relePin, HIGH);  // Desliga relé
}

void handlePulse() {
  // Requer autenticação via navegador
  if (!server.authenticate(authUser, authPass)) {
    return server.requestAuthentication();
  }

  digitalWrite(relePin, LOW);   // Liga relé
  delay(500);
  digitalWrite(relePin, HIGH);  // Desliga relé

  server.send(200, "text/plain", "Relé acionado com sucesso!");
}

void handleReset() {
  if (!server.authenticate(authUser, authPass)) {
    return server.requestAuthentication();
  }

  WiFi.disconnect(true); // Esquece a rede
  ESP.restart();
}

void handleResetBotao(){
if (digitalRead(resetButtonPin) == LOW) {
      WiFi.disconnect(true);
      ESP.restart();
    }
}

void handleNotFound() {
  // Para qualquer outro caminho, exige autenticação
  if (!server.authenticate(authUser, authPass)) {
    return server.requestAuthentication();
  }

  server.send(404, "text/plain", "Not found");
}

/***************************ROTINA DE INICIALIZAÇÃO DA PLACA************************/

void setup() {

  pinMode(relePin, OUTPUT);
  digitalWrite(relePin, HIGH);  // Relé inativo (desligado)

  pinMode(resetButtonPin, INPUT_PULLUP); // Ativo em LOW

  Serial.begin(115200);

  // Inicia WiFiManager
  WiFiManager wifiManager;
  //wifiManager.resetSettings();  // Apaga as redes salvas

  // Configura IP fixo se desejar
  wifiManager.setSTAStaticIPConfig(local_IP, gateway, subnet);

  // Nome da rede temporária para configuração
  if (!wifiManager.autoConnect("Rele_ESP")) {
    Serial.println("Falha ao conectar. Reiniciando...");
    ESP.restart();
  }

  Serial.println("Conectado com sucesso!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  WiFi.softAPdisconnect(true); // Desliga o modo AP completamente

  // Rotas protegidas
  server.on("/", handleRoot);
  server.on("/pulse", handlePulse);
  server.on("/reset", handleReset);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Servidor iniciado.");
}

/*****************************FUNÇÕES DE EXECUÇÃO ININTERRUPTA************************/

void loop() {
  server.handleClient(); //Monitora as requisições HTTP
  handleResetBotao(); //Monitora o estado do botão de reset

//Rotina de monitoramento da comunicação com o MRG
  unsigned long agora = millis();

  if (agora - ultimoPing >= intervaloPing) {
    ultimoPing = agora;

    Serial.print("Testando conexao com ");
    Serial.println(hostMonitorado);

    if (Ping.ping(hostMonitorado)) {
      Serial.println("Resposta OK");
      contadorFalhas = 0; // reset falhas
    } else {
      contadorFalhas++;
      Serial.print("Falha no ping. Contagem: ");
      Serial.println(contadorFalhas);

      if (contadorFalhas >= falhasMaximas) {
        Serial.println("Limite de falhas atingido. Acionando relé...");
        handlePowerOff();
        contadorFalhas = 0; // zera contagem para reiniciar monitoramento
      }
    }
  }  
}
