#include <Arduino.h>
#include <nvs_flash.h>
#include <tcpip_adapter.h>

#include <Sodaq_nbIOT.h>
#include <Sodaq_nbIOT_Client.h>

#define NBIOT_RX 25
#define NBIOT_TX 26
#define NBIOT_BAUD 115200
#define NBIOT_ENABLE_PIN 17
#define NBIOT_ENABLE_LOGIC LOW
#define NBIOT_DTR 2
#define NBIOT_RTS 14

#include <HardwareSerial.h>

HardwareSerial nbiotSerial(1);
Sodaq_nbIOT sara;
Sodaq_nbIOT_Client client(&sara);

void setup() {
    Serial.begin(115200);

    pinMode(NBIOT_DTR, OUTPUT);
    pinMode(NBIOT_RTS, OUTPUT);
    pinMode(NBIOT_ENABLE_PIN, OUTPUT);

    digitalWrite(NBIOT_DTR, LOW);
    digitalWrite(NBIOT_RTS, LOW);
    digitalWrite(NBIOT_ENABLE_PIN, !NBIOT_ENABLE_LOGIC);
    delay(500);
    digitalWrite(NBIOT_ENABLE_PIN, NBIOT_ENABLE_LOGIC);
    delay(500);
    digitalWrite(NBIOT_ENABLE_PIN, !NBIOT_ENABLE_LOGIC);

    nbiotSerial.begin(NBIOT_BAUD, SERIAL_8N1, NBIOT_RX, NBIOT_TX);
    sara.init(nbiotSerial, -1, -1, -1, 1);
    sara.setDiag(&Serial);
    sara.on();

    delay(2000);

    if (!sara.connect("iotxl", "")) {
        Serial.println("Failed connecting!");
        delay(3000);
        esp_restart();
    }

    sara.enableHex();

    bool connected = client.connect(IPAddress(52,221,141,22), 8080);
    if (connected) {
        String message = "{\"eventName\":\"light\",\"status\":\"1\",\"l\":2314,\"\m\":\"0627335104637551\"}";
        client.println("POST /api/pcs/Generic_brand_617GENERIC_DEVICEv1n HTTP/1.1");
        client.println("Host: 52.221.141.22:8080");
        client.println("User-Agent: nbiot-shield/1.0");
        client.println("Content-Type: application/json");
        client.println("Accept: */*");
        client.print("Content-Length: ");
        client.println(message.length());
        client.println("");
        client.println(message);

        while (true) {
            int available = client.available();
            Serial.printf("Available bytes: %d\n", available);

            if (available) {
                char buff[1024] = {0};
                int read = client.read((uint8_t*)buff, available);

                Serial.printf("read: %d\n%s\n", read, buff);
            }

            delay(500);
        }
    }
}

void loop() {

}