#include <Arduino.h>
#include <Servo.h>
Servo myservo; 
int pos = 5;

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>

#include <SocketIoClient.h>

#include <SoftwareSerial.h>
SoftwareSerial s(13,15);
#define USE_SERIAL Serial

#define ledPin D2
#define ledPin_D1 D1
#define ledPin D3



bool connectedToServer = false;


ESP8266WiFiMulti WiFiMulti;
SocketIoClient webSocket;

void event(const char * payload, size_t length) {
  USE_SERIAL.printf("got message: %s\n", payload);
}


void setup() {
  myservo.attach(5);  // attaches the servo on pin 5 to the servo object
  myservo.write(pos);
    s.begin(115200);
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    pinMode(2, OUTPUT);
    USE_SERIAL.begin(115200);
//    USE_SERIAL.begin(9600);

    USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

      for(uint8_t t = 4; t > 0; t--) {
          USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
          USE_SERIAL.flush();
          delay(1000);
      }

    WiFiMulti.addAP("Monirah SA", "12345678");

    while(WiFiMulti.run() != WL_CONNECTED) {
        digitalWrite(2, HIGH);
        delay(100);
    }

    webSocket.on("receive_message", servo_open);
//    webSocket.begin("192.168.100.6", 3001);
    webSocket.begin("dry-brushlands-40059.herokuapp.com");


    webSocket.on("connect", handler);
    webSocket.on("disconnect", handler_disconnected);

}

//char rdata;
String myString;
void loop() {
    webSocket.loop();
//    USE_SERIAL.println("Do we come here??");
//    rdata = Serial.read(); 
//    myString = myString+ rdata; 
    
//    delay(1000);

//    digitalWrite(ledPin_D1, HIGH);

  if (WiFi.status() == WL_CONNECTED){
//    USE_SERIAL.println("WIFI Connected");
//      digitalWrite(LED_BUILTIN, HIGH);
//      digitalWrite(2, LOW);
    if (connectedToServer){
      digitalWrite(2, LOW);
    }else {
      digitalWrite(2, LOW);   // Turn the LED on by making the voltage LOW
      delay(100);            // Wait for a second
      digitalWrite(2, HIGH);  // Turn the LED off by making the voltage HIGH
      delay(100);            // Wait for two seconds 
    }
  }

    delay(100);
    if(connectedToServer){
      if (s.available() > 0){
        String dataSerial = s.readString();
        Serial.println(dataSerial);
        String sendThis = "\"" + dataSerial + "\"";
        String json= "{\"message\":" + sendThis + "}";
        webSocket.emit("send_this", json.c_str());
//        webSocket.emit("send_message", json.c_str());

      }

    }
    
}

// This runs when we connected to the server
void handler(const char * payload, size_t length) {
  connectedToServer = true;
}

// This runs when we are disconnected after being connected to the server
void handler_disconnected (const char * payload, size_t length) {
  connectedToServer = false;
}


void servo_open(const char* message, size_t length){

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, message);

  String message_2 = doc["message"];
  USE_SERIAL.println(message_2);

  if (message_2.equals("O")){
    myservo.write(78);
    delay(3000);
    myservo.write(5);
  }
  
  


}
