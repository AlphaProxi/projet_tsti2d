#include <ESP32Servo.h> // Inclusion de la bibliothèque pour contrôler les servos avec l'ESP32
#include <WiFi.h>       // Inclusion de la bibliothèque pour gérer le Wi-Fi
#include <WiFiUdp.h>    // Inclusion de la bibliothèque pour gérer la communication UDP

const char *ssid = ""; // SSID de votre réseau Wi-Fi
const char *password = ""; // Mot de passe de votre réseau Wi-Fi
const unsigned int localUdpPort = 12345; // Port UDP local à écouter

WiFiUDP udp; // Création d'un objet UDP pour gérer la communication UDP

// Définition d'une structure pour les données de contrôle
struct AccelFrein {
  int accel; // Variable pour l'accélération
  int angle = 55; // Variable pour l'angle du servo, avec une valeur par défaut de 55
  bool sens = 1; // Variable pour le sens du mouvement
};

int pinA1 = 5; // Définition du pin GPIO 5
int pinB1 = 8; // Définition du pin GPIO 8
int pinA2 = 9; // Définition du pin GPIO 9
int pinB2 = 10; // Définition du pin GPIO 10
Servo moteur; // Création de l'objet Servo pour contrôler le moteur

void setup() {
  Serial.begin(115200); // Initialisation de la communication série à 115200 bauds
  WiFi.begin(ssid, password); // Connexion au réseau Wi-Fi avec SSID et mot de passe
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000); // Attendre 1 seconde avant de réessayer
    Serial.println("Connecting to WiFi.."); // Afficher un message de tentative de connexion
  }
  Serial.println("Connected to WiFi"); // Afficher un message de connexion réussie
  Serial.println(WiFi.localIP()); // Afficher l'adresse IP locale de l'ESP32
  udp.begin(localUdpPort); // Initialiser l'écoute UDP sur le port défini

  moteur.attach(6); // Attacher le moteur servo à la broche GPIO 6

  // Configurer les broches GPIO comme sorties
  pinMode(pinA1, OUTPUT);
  pinMode(pinB1, OUTPUT);
  pinMode(pinA2, OUTPUT);
  pinMode(pinB2, OUTPUT);
}

void loop() {
  AccelFrein valeurs; // Création d'une instance de la structure AccelFrein pour stocker les valeurs reçues
  int packetSize = udp.parsePacket(); // Vérifier si un paquet UDP a été reçu
  if (packetSize) {
    // Lire les données UDP reçues et les stocker dans la structure valeurs
    udp.read((uint8_t *)&valeurs, sizeof(valeurs)); 
    // Afficher les valeurs reçues pour l'accélération et l'angle
    Serial.print("Received Accel: ");
    Serial.println(valeurs.accel);
    Serial.print("Received Angle: ");
    Serial.println(valeurs.angle);
    
    moteur.write(valeurs.angle); // Définir la position du servo en fonction de l'angle reçu
    
    if (valeurs.sens == 1) {
      // Si le sens est 1, faire avancer le moteur
      digitalWrite(pinA1, HIGH);
      digitalWrite(pinB1, HIGH);
      analogWrite(pinA2, valeurs.accel); // Contrôle de la vitesse du moteur
      analogWrite(pinB2, valeurs.accel); // Contrôle de la vitesse du moteur
    } else {
      // Si le sens est 0, faire reculer le moteur
      digitalWrite(pinA1, LOW);
      digitalWrite(pinB1, LOW);
      analogWrite(pinA2, valeurs.accel); // Contrôle de la vitesse du moteur
      analogWrite(pinB2, valeurs.accel); // Contrôle de la vitesse du moteur
    }
  }
}
