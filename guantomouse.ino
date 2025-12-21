#include <WiFi.h>
#include <WebSocketsServer.h>
#include <BMI160Gen.h>
#include <Wire.h>

#define DEADZONE_X 2
#define DEADZONE_Y 2
#define MS 30 // 5: 200Hz, 10: 100Hz, 16: 60Hz

const float SENS_X = 300.0;
const float SENS_Y = 300.0;

#define L_BUTTON 0
#define R_BUTTON 2 // cambiare a 1 
#define S_BUTTON 1 // cambiare a 2
#define BOARD_LED_PIN 8

#define DEBUG false

const char* ssid = "ssid";
const char* password = "password";

float offsetX, offsetY;
bool prevL = false, prevR = false;

unsigned long lastSend = 0;

uint8_t clientID = 0;

WebSocketsServer webSocket = WebSocketsServer(80);

void setup() {
    setCpuFrequencyMhz(160);
    Serial.begin(115200);

    pinMode(L_BUTTON, INPUT_PULLUP);
    pinMode(R_BUTTON, INPUT_PULLUP);
    pinMode(S_BUTTON, INPUT_PULLUP);
    pinMode(BOARD_LED_PIN, OUTPUT);
    digitalWrite(BOARD_LED_PIN, HIGH);

    Wire.begin(4, 5);
     
    BMI160.begin(BMI160GenClass::I2C_MODE, 0x69);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connessione WiFi...");
    }

    Serial.print("Connesso! IP: ");
    Serial.println(WiFi.localIP());

    int gx, gy, gz;
    float sumOffSetX = 0, sumOffSetY = 0;
    int q = 50;
    for (int i = 0; i < q; i++) {
        BMI160.readGyro(gx, gy, gz);
        sumOffSetX -= gz;
        sumOffSetY -= gx;
        delay(10);
    }
    offsetX = sumOffSetX / q;
    offsetY = sumOffSetY / q;

    Serial.println("Offset X: " + String(offsetX));
    Serial.println("Offset Y: " + String(offsetY));

    webSocket.begin();
    webSocket.onEvent([](uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
        if (type == WStype_CONNECTED) {
            Serial.printf("Client %u connesso\n", num);
            clientID = num;
        } else if (type == WStype_DISCONNECTED) {
            Serial.printf("Client %u disconnesso\n", num);
        }
    });
    WiFi.setSleep(true);
}

void loop() {
    webSocket.loop();

    if (millis() - lastSend < MS) return;
    lastSend = millis();

    int gx, gy, gz;
    BMI160.readGyro(gx, gy, gz);

    float tempMX = -(gz + offsetX) / SENS_X;
    float tempMY =  (gy + offsetY) / SENS_Y;

    static float filtMX = 0, filtMY = 0;
    float alpha = 0.3;
    filtMX = alpha * tempMX + (1 - alpha) * filtMX;
    filtMY = alpha * tempMY + (1 - alpha) * filtMY;

    int mx = (abs(filtMX) > DEADZONE_X) ? (int)filtMX : 0;
    int my = (abs(filtMY) > DEADZONE_Y) ? (int)filtMY : 0;

    bool nowL = !digitalRead(L_BUTTON);
    if (nowL == true && prevL == false) webSocket.broadcastTXT("LP");
    if (nowL == false && prevL == true) webSocket.broadcastTXT("LR");
    prevL = nowL;

    bool nowR = !digitalRead(R_BUTTON);
    if (nowR == true && prevR == false) webSocket.broadcastTXT("RP");
    if (nowR == false && prevR == true) webSocket.broadcastTXT("RR");
    prevR = nowR;

    char buf[16];
    bool coordsBlock = false;

    if (digitalRead(S_BUTTON) == 0) coordsBlock = true;

    if (!coordsBlock && (mx != 0 || my != 0)) {
        snprintf(buf, sizeof(buf), "%d,%d", mx, my);
        webSocket.sendTXT(clientID, buf, false);
        if (DEBUG) Serial.println(buf);
    }

    WiFi.setSleep(true);
}

