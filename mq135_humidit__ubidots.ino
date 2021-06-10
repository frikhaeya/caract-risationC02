// ******************************************** ****
// Mesure analogique et numérique de l’humidité du sol
// Avec le module Humidité Sol
//Envoi Vers Ubidots
// ************************* ******************************

// Ubidots Connexion
#include "UbidotsMicroESP8266.h"
#define TOKEN "BBFF-u4YtEeVGCsmOr6ixYPxwEPAdPRuw0e"

#define WIFISSID "Airbox-1DF6"
#define PASSWORD "02332801"

#include <Wire.h>
#include "MQ135.h"

/// The load resistance on the board
#define RLOAD 10.0
/// Calibration resistance at atmospheric CO2 level
#define RZERO 76.63
/// Parameters for calculating ppm of CO2 from sensor resistance
#define PARA 116.6020682
#define PARB 2.769034857
/// Atmospheric CO2 level for calibration purposes
#define ATMOCO2 397.13

uint8_t mq135Pin = A0;     // Pin sur lequel est branché de MQ135
int PinAnalogiqueHumidite = D0; // Broche Analogique de mesure de l’humidité

MQ135 gasSensor = MQ135(mq135Pin);  // Initialise l'objet MQ135 sur le Pin spécifié

Ubidots client(TOKEN);


const int R1 = 30; // R1 = 30 Ohm
const float U = 5.0; // valeur de la tension constante
float UR2; // Valeur de la tension mesurée UR2
float hsol;


void setup() {
  Serial.begin(9600); // Initialise le port série à 9600 bps 
  pinMode(mq135Pin, INPUT);
  pinMode (PinAnalogiqueHumidite, INPUT); // pin D0 en entrée analogique

  WiFi.begin(WIFISSID, PASSWORD);
  client.setDataSourceName("detection_CO2");
  client.setDataSourceLabel("detection_CO2");

}

void loop()
{
  Serial.print("Nouvelle Mesure");
  client.sendAll(true);
  // Capteur MQ135
    //Serial.print("A0: ");
    int val = analogRead(mq135Pin);
    //Serial.print(val);
    float resistance = ((1023./(float)val)) * RLOAD;  
    float ppm = PARA * pow((resistance/RZERO), -PARB)/ 1000;
    //Serial.print(" ppm CO2: ");
    //Serial.println(ppm);
    client.add("ppm-co2", ppm);
    client.sendAll(true);

  // Capteur Humidité
    UR2 = analogRead (PinAnalogiqueHumidite); // Lit la tension analogique
    UR2 =(UR2*U)/1024.0;
    hsol = (R1*UR2)/((U - UR2)*3000);
    
    //Serial.print ("humidité du sol: ");
    //Serial.println (hsol); // afficher la mesure
    client.add("humidité", hsol);
    client.sendAll(true);
    
    delay(1000);        // Actualise la mesure chaque seconde
}
