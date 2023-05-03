// Importing the Arduino library to program the NodeMCU like an Arduino Board
#include <Arduino.h>

// Importing the servo library to control the servo motor (used to control the gate)
#include <Servo.h>

// Defining the servo variable which will be used to control the servo motor
Servo myservo; 

// Defining the initial position of the servo motor (which is 5°)  
int pos = 5;

// Importing the wifi libraries to connect the NodeMCU to a Wi-Fi network
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

// Importing the ArduinoJson library to handle JSON data 
#include <ArduinoJson.h>

// Importing SocketIoClient library to establish socketio connection
#include <SocketIoClient.h>

// We will be using software serial communicatoin for this we are importing the SoftwareSerial library
#include <SoftwareSerial.h>

// Defining a variable of the "s" where pin 13 in RX and 15 is TX
SoftwareSerial s(13,15);

// Difining a varibale for the serial. We use this print the output in serial of Arduino IDE
#define USE_SERIAL Serial


// We will use this variable to check if we are connected to the server, intially the value is set to false as we 
// are not connected to the server. 
bool connectedToServer = false;


// We will use this variable to connect to Wi-Fi 
ESP8266WiFiMulti WiFiMulti;

// We will use this variable to connect to establish and maintain the socketio connection 
SocketIoClient webSocket;


void setup() {
  pinMode(2, OUTPUT);  // Setting the input BLUE pin (connected at pin 2 internally) of NodeMCU to output
  myservo.attach(5);  // attaches the servo on pin 5 to the servo object (connecting the control pin of servo to oin 5 of NodeMCU)
  myservo.write(pos); // Setting the position to 5° (as we declared pos = 5)
  s.begin(115200);  // Setting the board rate for the software serial communication to 115200 
  USE_SERIAL.begin(115200); // Setting the board rate for the serial communication to 115200 

  USE_SERIAL.setDebugOutput(true); // The function turns on debug output from the WiFi library so you can see what is happening to your WiFi connection.

  // Used to pint 3 blank lines for good visual representation in the serial monitor
  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  // Setting the Wifi credentials
     WiFiMulti.addAP("Monirah SA", "12345678");

  // We use the run method to connect to the WiFi and if we fail to connect it prints WiFi not connected!
  // in the serial also it turns the Blue light off 
  while(WiFiMulti.run() != WL_CONNECTED) {
      digitalWrite(2, HIGH); // Turn off the internal blue LED 
      USE_SERIAL.println("WiFi not connected!"); // Print to serial that we are not connected to Wifi
      delay(100); // Dealy for 100 milliseconds
  }


  webSocket.on("receive_message", servo_open); // We run the servo_open() function found at the bottom to carry out an action when we the "receive_message" is triggered on the server's socketio logic
  // webSocket.begin("192.168.100.6", 3001); // Connect to local server used for testing and development 
  webSocket.begin("dry-brushlands-40059.herokuapp.com"); // Connect to the server in hosted in the cloud


  // The two functions as the name suggests run when we connect or disconnect from the server
  webSocket.on("connect", handler);
  webSocket.on("disconnect", handler_disconnected);

}

String myString;
void loop() {
  
    // We will keep looping to connect to the server.
    webSocket.loop();

  // We check if we are connected to Wi-Fi  
  if (WiFi.status() == WL_CONNECTED){
    // Now we check if we are connected to the server
    if (connectedToServer){
      digitalWrite(2, LOW); // If connected to the server we will keep the blue light on
    }else { // if we are not connected to server we will blink the light
      digitalWrite(2, LOW);   // Turn the LED on by making the voltage LOW
      delay(100);            // Wait for 100 milliseconds
      digitalWrite(2, HIGH);  // Turn the LED off by making the voltage HIGH
      delay(100);            // Wait for 100 milliseconds 
    }
  }

    delay(100); // Wait for 100 milliseconds 

    // In the if condition below we check if we are connected to server and if we are connected 
    // we send the data received from the Arduino Mega to the server.
    if(connectedToServer){
      if (s.available() > 0){ // In this if condition we check if we got any data from the Arduino via serial communication
        // We read the data and store it as dataSerial and print it in serial for our convenience
        String dataSerial = s.readString();
        Serial.println(dataSerial);
        // We format the data to send it to the cloud and make the json form of the data
        String sendThis = "\"" + dataSerial + "\"";
        String json= "{\"message\":" + sendThis + "}";
        webSocket.emit("send_this", json.c_str()); // The data is sent to the server 
      }

    }
    
}

// This runs when we connected to the server and it sets connectedToServer to be true 
void handler(const char * payload, size_t length) {
  connectedToServer = true;
}

// This runs when we are disconnected after being connected to the server and it sets connectedToServer to be false
void handler_disconnected (const char * payload, size_t length) {
  connectedToServer = false;
}


// This runs when we the socketio gets the "receive_message" command 
void servo_open(const char* message, size_t length){

  // We convert the message in a form in which it can be read in 
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, message);

  // We get the message in string form and print it in Serial
  String message_2 = doc["message"];
  USE_SERIAL.println(message_2);

  // If the message value is O then we open the gate by setting servo angle to be 78 and after 3 seconds we close the gate
  if (message_2.equals("O")){
    myservo.write(78);
    delay(3000);
    myservo.write(5);
  }
  
  


}
