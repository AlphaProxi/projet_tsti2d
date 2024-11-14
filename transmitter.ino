#include <WiFi.h> // Bibliothèque pour la gestion du WiFi
#include <WiFiClient.h> // Bibliothèque pour le client WiFi
#include <WiFiUdp.h> // Bibliothèque pour la communication UDP

const char *ssid = "";             // Définir le SSID (nom du réseau Wi-Fi)
const char *password = "";        // Définir le mot de passe du réseau Wi-Fi
const unsigned int localUdpPort = 12345;
const IPAddress receiverIP(192, 168, 198, 238); // Adresse IP de destination pour les paquets UDP

// Structure pour les variables d'accélération et de freinage
struct AccelFrein {
  int accel; // Variable pour l'accélération
  int angle = 55; // Angle initial du volant
  bool sens = 1; // Direction : marche avant (1) ou marche arrière (0)
};

WiFiUDP udp; // Objet pour la communication UDP

unsigned long appuihaut, appuibas = 0; // Temps des derniers appuis haut et bas des capteurs
int c1 = 4; // Pin du premier capteur
int c2 = 5; // Pin du deuxième capteur
bool valuehaut,valuebas; // État du système (ON/OFF)
unsigned long prevhaut, prevbas; // Temps précédents pour les capteurs et les affichages

AccelFrein valeurs; // Instance de la structure AccelFrein
//BlynkTimer timer; // Objet pour le timer Blynk
void fonction(); // Déclaration de la fonction

void setup() {
  Serial.begin(115200); // Initialisation du moniteur série à 115200 bauds
  for (int i = 1; i <= 5; i++) {
     pinMode(i, INPUT);
  }
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi");
  udp.begin(localUdpPort); // Initialisation de la communication UDP
}

void loop() {
  fonction();
}

void fonction() {
  if (digitalRead(c1) && valuehaut == false) {
    appuihaut = micros();
    valuehaut = true;
  }
  if (digitalRead(c2) && valuebas == false) {
    appuibas = micros();
    valuebas = true;
  }
  if (valuebas && valuehaut && !prevhaut && !prevbas) {
    valeurs.angle += (appuihaut < appuibas) ? 5 : -5;
    Serial.println("Angle du volant " + String(valeurs.angle) + "°");
    prevhaut = appuihaut;
    prevbas = appuibas;
  }
  valeurs.angle = max(20, min(70, valeurs.angle)); // Limite de l'angle entre 20° et 70°
  boutons(); // Mise à jour des boutons et de l'accélération
  udp.beginPacket(receiverIP, localUdpPort);
  udp.write((uint8_t *)&valeurs, sizeof(valeurs)); // Conversion des données en octets et envoi
  udp.endPacket();
}

void boutons() {
  valeurs.accel = map(analogRead(1), 0, 3170, 0, 255);
  }
