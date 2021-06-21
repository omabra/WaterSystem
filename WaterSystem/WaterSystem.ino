#include <WiFiNINA.h>
//#include <utility/wifi_drv.h>
#include <RTCZero.h>

#include "arduino_secrets.h"


char ssid[] = SECRET_SSID;    // your network SSID
char pass[] = SECRET_PASS;    // your network password

int status = WL_IDLE_STATUS;      //connection status
WiFiServer server(80);            //server socket

WiFiClient client = server.available();

RTCZero rtc;
int secs;

int ledPin = 0;
int sensorPin = A0;
int sensorValue = 0;

void setup() {
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  WiFiDrv::pinMode(25, OUTPUT); //Internal led define green pin
  WiFiDrv::pinMode(26, OUTPUT); //Internal led define red pin
  WiFiDrv::pinMode(27, OUTPUT); //Internal led define blue pin

  internalLed (0, 255, 0);

  while (!Serial);

  connect_WiFi();

  //rtc.begin();
  //setRTC();

  server.begin();
  
}

void loop() {
  client = server.available();
  //secs = rtc.getSeconds();

  if (client) {
    unsigned long timeout = millis();
    printWEB();
    
  }
}

void connect_WiFi() {
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
    Serial.println();
    internalLed (255, 0, 0);

  }
}

void printWEB() {
  String payload = "";
  String httpResponseString = "HTTP/1.1 200 OK";
  
  if (client) {                             
    String response = "";

    String header = "";
    String body = "";

    int responseSize = 0;
                  
    while (client.connected()) {            
      if (client.available()) {         

        char c = client.read();             
        response += c;
        
        if (c == '\r'){
          if (response.charAt(response.length()-2) == '\n'){
            //If there response contains an empty line, the header is finished.
          
            header = response;
            response = "";

            Serial.println (header);            
            
            String httpMethod = header.substring(0, header.indexOf(" /"));
            String uri = header.substring(header.indexOf(" /") +1, header.indexOf(" HTTP"));
            
            //String client = header.substring(header.indexOf("Client: ") +1, header.indexOf(" HTTP"));

            //TODO:
            // Extract for header: Client, Authorization and Content-lenght.
            // Client and authorization should kill the request if not authorized
            // Content-lenght should be used to retrive the rest body
            
            
            break;
            if (httpMethod == "GET") {
              httpResponseString = "HTTP/1.1 200 OK";
              payload = "{\"test\":\"Test\"}";
              break;
            } else if (httpMethod == "POST"){
              if(header.indexOf("Content-Length: ") >= 0){

                String s = header.substring(header.indexOf("Content-Length: "));
                Serial.println (s);
              
              }
            } else {
              //other HTTP methods are not supported at the moment
              httpResponseString = "HTTP/1.1 405 Method Not Allowed";
              break;
            }
              

            client.println(httpResponseString);
            client.println("Content-type:application/json");
            client.println("Access-Control-Allow-Origin: *");
            client.println();
            if (payload.length() > 0){
              client.println(payload);
            }
            // The HTTP response ends with another blank line:
            client.println();
            
          }

            
        }
      }

        
        
        //Serial.write(c);                    
                          

        
            
            /*
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:application/json");
            client.println("Access-Control-Allow-Origin: *");
            client.println();
            client.println(payload);
            // The HTTP response ends with another blank line:
            client.println();
            /*

          

        /*
        
        
        if (currentLine.endsWith("POST /setOutput")) {
          digitalWrite(ledPin, HIGH);
          internalLed (0, 127, 0);

          payload = "{\"status\": {\"led1\": \"On\"}}";
        }

        if (currentLine.endsWith("POST /resetOutput")) {
          digitalWrite(ledPin, LOW);
          internalLed (0, 0, 0);

          payload = "{\"status\": {\"led1\": \"Off\"}}";
        }

        if (currentLine.endsWith("GET /sensor")) {
          sensorValue = analogRead(sensorPin);

          payload = "{\"status\": {\"sensor1\": \"" + String(sensorValue) + "\"}}";
        }
        */
      }
      
    }
    // close the connection:
    client.stop();

  }


void internalLed(int Red, int Green, int Blue) {
  WiFiDrv::digitalWrite(25, Green); //GREEN
  WiFiDrv::digitalWrite(26, Red);   //RED
  WiFiDrv::digitalWrite(27, Blue);   //BLUE
}

void setRTC() { // get the time from Internet Time Service
  unsigned long epoch;
  int numberOfTries = 0, maxTries = 6;
  do {
    epoch = WiFi.getTime(); // The RTC is set to GMT or 0 Time Zone and stays at GMT.
    numberOfTries++;
  }
  while ((epoch == 0) && (numberOfTries < maxTries));

  if (numberOfTries == maxTries) {
    Serial.println("NTP unreachable!!");
    //while (1);  // hang
  }
}
