//This script uses an ESP8266 and a relay to control a ceiling fan remotely through both wifi and an ir remote via Ky-022
//this includes css style that uses 2 buttons
//the buttons are nicely center aligned

#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <SPI.h>
#include <Wire.h>
#include <IRrecv.h>

 
const char* ssid     = "xxxxxxxx";
const char* password = "xxxxxxxxxxxxxxxxxxxxxxxx";


int RECV_PIN = D3; //an IR detector
int relay = D2;
WiFiServer server(80);
IRrecv irrecv(RECV_PIN);

 
void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  delay(10);
 
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
 
  // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
 
}


int turn_on_off()
{
  if(digitalRead(relay) == 1)//turn the light off if already on
  {
    digitalWrite(relay, 0);
  }
  else if (digitalRead(relay) == 0)
  {
      digitalWrite(relay, 1);
  }
}


void translateIR()
{
  switch(results.value)
  {
    //the red power button
    case 0xFFA25D: turn_on_off();
  
    default:
    Serial.println("wrong key ");
  }
}

 
 
void loop() {

  if (irrecv.decode(&results)) {
    translateIR();
  irrecv.resume(); // Receive the next value
  }
  delay(100);
  
  
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new connection");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
  int value = LOW;
  if (request.indexOf("/ON") != -1 && digitalRead(relay) == LOW) {
    digitalWrite(relay, HIGH);
    value = HIGH;
  }
  if (request.indexOf("/OFF") != -1 && digitalRead(relay) == HIGH){
    digitalWrite(relay, LOW);
    value = LOW;
  }
 
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("<br><br><h1 style=\"font-size:70px;text-align: center;\">Fan is now: ");
 
  if(value == HIGH || digitalRead(relay) == HIGH) {
    client.print("On</h1>");  
  } else {
    client.print("Off</h1>");
  }
  client.println("<button style=\"display: block;margin-right: auto; margin-left: auto;margin-top: 35%; background-color:dodgerblue;color: white;font-size: 60px;padding: 70px 200px;border: none;border-radius: 5px;text-align: center;\"><a style=\"text-decoration: none;color:white\" href=\"/ON\">ON</a></button>");
  client.println("<button style=\"display: block;margin-right: auto; margin-left: auto;margin-top: 5%;background-color:dodgerblue;color: white;font-size: 60px;padding: 70px 200px;border: none;border-radius: 5px;text-align: center;\"><a style=\"text-decoration: none;color:white\" href=\"/OFF\">OFF</a></button>");
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
 
}
