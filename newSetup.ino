#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

const int trigPin = 12;
const int echoPin = 14;

const int dustbinLength = 30;

long duration;
float distanceCm;
float distanceInch;

#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

int val = 0;

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "Area 51 - America"
#define WIFI_PASSWORD "Darshana1997"
#define API_KEY "AIzaSyAg77T_vOAKzxlog7vwyVfusJrRxIZgfS8 "
#define DATABASE_URL "https://smart-eco-69349-default-rtdb.firebaseio.com/"

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

void setup()
{
    Serial.begin(115200);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    config.api_key = API_KEY;

    config.database_url = DATABASE_URL;

    if (Firebase.signUp(&config, &auth, "", ""))
    {
        Serial.println("ok");
        signupOK = true;
    }
    else
    {
        Serial.printf("%s\n", config.signer.signupError.message.c_str());
    }

    config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}

void loop()
{
    val = getData();
    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
    {
        sendDataPrevMillis = millis();
        if (Firebase.RTDB.setInt(&fbdo, "data", val))
        {
            Serial.println("Updated");
        }
        else
        {
            Serial.println("FAILED");
            Serial.println("REASON: " + fbdo.errorReason());
        }
        if (Firebase.RTDB.setInt(&fbdo, "distance", distanceCm))
        {
            Serial.println("Updated");
        }
        else
        {
            Serial.println("FAILED");
            Serial.println("REASON: " + fbdo.errorReason());
        }
    }
}

int getData()
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);

    distanceCm = duration * SOUND_VELOCITY / 2;

    distanceInch = distanceCm * CM_TO_INCH;

    Serial.print("Remain Amount Percentage ");
    Serial.print(distanceCm / dustbinLength * 100);
    Serial.print(" %");
    Serial.println("-------------------------");
    return (distanceCm / dustbinLength * 100);
    delay(100);
}
