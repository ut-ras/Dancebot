/* ESP32 Web Controller for DanceBot
 *
 * Sets up the ESP32 as a WiFi Access Point or connects to an existing WiFi network
 * Launches a Web Server with buttons to input dance moves to the DancingServos object
 *
 * WIFI_MODE variable
 * AP Mode: Launches the web server at http://192.168.4.1:80/       AP untested
 * STA Mode: Connects to an existing WiFi network
 *
 * Set WiFi credentials with "ssid" and "pass" strings

 *
 * Resources
 *
 * Arduino ESP32 and ESP32Servo Docs
 * https://github.com/espressif/arduino-esp32
 * https://www.arduinolibraries.info/libraries/esp32-servo
 *
 * Web Server
 * https://github.com/espressif/arduino-esp32/tree/master/libraries/WebServer
 * Arduino IDE: Examples > Examples for ESP32 > WebServer > AdvancedWebServer
 * 
 * Javascript AJAX and DOM
 * https://www.w3schools.com/js/js_htmldom.asp
 * https://www.w3schools.com/js/js_ajax_intro.asp
 * https://www.w3schools.com/js/js_ajax_http_send.asp
 * https://www.w3schools.com/jsref/event_onclick.asp
 */
 
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <esp_now.h>
#include "WebController.h"
#include "DancingServos.h"
#include "PowerController.h"


void handleRoot();
void handleReceivedDanceMove(const uint8_t * mac, const uint8_t *incomingData, int len);
void handleDanceMove();
void handleDance();
void handleNotFound();
void handleUnknownMove();

String indexHTML();
String getJavascript();

int dancebotID;

/* Data Transmission*/
esp_now_peer_info_t peerInfo;
uint8_t address[] = {0x30, 0x83, 0x98, 0xD7, 0x33, 0xE0}; //mothership ESP32 MAC address
struct_message transmitMessage; //contains info that will be transmitted
struct_message receivedMessage; //contains info that will be received
int rcvFlag; //high when we received a message
int setIDOnce = 1;
//enums correspond to each dance move
enum{
  STOP,
  RESET,
  WALK,
  HOP,
  WIGGLE,
  ANKLES,

   // dev notes: new moves below:
  LEFT_HEELTOE,
  RIGHT_HEELTOE,
  LEFT_STANK,
  RIGHT_STANK,
  BWALK,
  WAVE,

  // dev notes: TEST MOVES below:
  // ANKLES_TEST,
  // ANKLES_PHASE,
  // ANKLES_OFFSET,
  // LEGS,
  // LEGS_PHASE,
  // LEGS_OFFSET,

  DEMO1,
  DEMO2,

  // dev notes: new dance routines below:
  DEMO3,
  DEMO4
};
// enum for return info
enum{
  None,
  SetID,
  BattLevel,
}; 

//Web Server
const char * server_ssid;
const char * server_pass;
int port = 80;
IPAddress ip;
String webServerPath = "http://";

//Web server at port 80
WebServer server(port);

//DancingServos object
DancingServos* dance_bot;

//PowerController object
PowerController* power;

WiFiClient master;
unsigned long previousRequest = 0;

void printMACAddress(){
  Serial.println(WiFi.macAddress());
}

//callback when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

//when called, takes in received data from transmitter and sets flag (used for dance moves)
void onDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len){
  memcpy(&receivedMessage, incomingData, sizeof(receivedMessage));
  Serial.println("Received message...");
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Status is: ");
  Serial.println(receivedMessage.status);
  Serial.println("Message is: ");
  Serial.println(receivedMessage.character);
  Serial.println();

  //if transmitter requested battery level, send value, our ID, and acknowledgement
  if(receivedMessage.batteryFlag){
    Serial.println("Sending battery level...");
    transmitMessage.id = dancebotID;
    transmitMessage.batteryLevel = power->calculateBatteryPercentage();
    transmitMessage.status = BattLevel;
    esp_err_t result = esp_now_send(address, (uint8_t *) &transmitMessage, sizeof(transmitMessage));
  }
  if(receivedMessage.status == SetID){ //only set ID once
    Serial.print("I set my own ID: "); Serial.println(receivedMessage.id);
    dancebotID = receivedMessage.id;
    setIDOnce = 0;
  }
  rcvFlag = 1;
}

/* Setup Functions */

/* setupESPNOW
 * Sets up transmit and receive communication with Mothership
 */
int setupESPNOW(DancingServos* _dance_bot, PowerController* _power){
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return 0;
  }
  
  esp_now_register_send_cb(onDataSent); //func called when we send data
  esp_now_register_recv_cb(onDataRecv); //func called when we receive data

  Serial.println("memcpy");
  memcpy(peerInfo.peer_addr, address, sizeof(peerInfo.peer_addr));
  peerInfo.channel = 0; 
  peerInfo.encrypt = false;

  // Add peer       
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return 0;
  }

  //dance_bot and power object in this file points to object in main file
  dance_bot = _dance_bot; 
  power = _power;

  Serial.println("Finished ESPNOW init");
  return 1;
}

/* setupWiFi
 * NOTE: this legacy function = setupAPNetwork() in DancebotESP32
 * STA = connect to a WiFi network with name ssid
 * AP = create a WiFi access point with  name ssid
 */
void setupWiFi(String mode, const char * ssid, const char * pass) {
  server_ssid = ssid;
  server_pass = pass;

  WiFi.mode(WIFI_STA);

  if (mode.equals("AP")) {
    //Turn on Access Point
    WiFi.softAP(ssid, pass);
    ip = WiFi.softAPIP();
  }
  else {
    //Connect to a WiFi network
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      yield();
      //Serial.print(".");
    }
    ip = WiFi.localIP();

    if (MDNS.begin("esp32")) {
      Serial.println("MDNS responder started");
    }
  }

  Serial.println("WiFi mode=" + mode + ", ssid = " + String(ssid) + ", pass = " + String(pass));
}

void setupWebServer(DancingServos* _dance_bot) {
  //Turn on a web server at port 80
  //Map paths to hander functions, can also specify HTTP methods

  server.on("/", handleRoot);
  server.on("/danceM", HTTP_POST, handleDanceMove);
  server.on("/danceM", HTTP_GET, handleRoot);
  server.on("/dance", HTTP_POST, handleDance);
  server.on("/dance", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);    //404 Not Found

  server.begin();

  webServerPath += ip.toString() + ":" + String(port) + "/";

  Serial.println("Web server at " + webServerPath);

  dance_bot = _dance_bot;
}

/* Main Loop */
void loopWebServer() {
  //handle web server
  server.handleClient();
}


/* Request Handlers */

//main page   "/"
void handleRoot() {
  Serial.println("Server received new client");
  server.send(200, "text/html", indexHTML());
}

//dance moves    "/danceM"
void handleDanceMove() {
  //if we have received a message, do corresponding dance move
  if(rcvFlag){
    switch(receivedMessage.danceMove) {
      case STOP:
        dance_bot->stopOscillation();
        dance_bot->enableDanceRoutine(false);
        break;

      case RESET: 
        dance_bot->position0();
        break;
    
      case WALK: 
        dance_bot->walk(-1, 1500, false);
        break;
    
      case HOP: 
        dance_bot->hop(25, -1);
        break;
    
      case WIGGLE: 
        dance_bot->wiggle(30, -1);
        break;
    
      case ANKLES: 
        dance_bot->themAnkles(-1);
        break;

    
    // dev notes: new moves below:
      case LEFT_HEELTOE: 
        dance_bot->heel_toe(-1, true);
        break;

      case RIGHT_HEELTOE: 
        dance_bot->heel_toe(-1, false);
        break;

      case LEFT_STANK: 
        dance_bot->stank(-1, true);
        break;

      case RIGHT_STANK: 
        dance_bot->stank(-1, false);
        break;

      case BWALK: 
        dance_bot->walk(-1, 1500, false);
        break;

      case WAVE: 
        dance_bot->wave(40, -1);
        break;

      case DEMO1: 
        dance_bot->setDanceRoutine(0);
        dance_bot->enableDanceRoutine(true);
        break;
      
      case DEMO2: 
        dance_bot->setDanceRoutine(1);
        dance_bot->enableDanceRoutine(true);
        break;
      
      case DEMO3: 
        dance_bot->setDanceRoutine(2);
        dance_bot->enableDanceRoutine(true);
        break;
      
      case DEMO4: 
        dance_bot->setDanceRoutine(3);
        dance_bot->enableDanceRoutine(true);
        break;

      default:
        Serial.println("Dance move not recognized, ERROR too lit for this robot");
        return;
    }
  }
//   else {
//     Serial.println("Dance move not recognized, ERROR too lit for this robot");
//     return;
//   }
}

//dance routines    "/dance"
void handleDance() {
  String dance_routine = "";
  if(server.hasArg("dance_routine")) {
    dance_routine = server.arg("dance_routine");
    Serial.println("Server received dance_routine: " + dance_routine);

    if (dance_routine.equals("Demo 1")) {
      dance_bot->setDanceRoutine(0);
      dance_bot->enableDanceRoutine(true);
    }
    else if (dance_routine.equals("Demo 2")) {
      dance_bot->setDanceRoutine(1);
      dance_bot->enableDanceRoutine(true);
    }
    else {
      Serial.println("Dance routine not recognized, ERROR too lit for this robot");
      handleUnknownMove();
      return;
    }
  }
  else {
    dance_routine = "ERROR Server did not find dance routine argument in HTTP request";
  }
  
  server.send(200, "text/plain", dance_routine);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: " + server.uri() + "\n";
  message += "Method: " + String((server.method() == HTTP_GET)?"GET":"POST") + "\n";
  message += "Arguments: " + String(server.args()) + "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleUnknownMove() {
  String message = "Dance move not recognized \nERROR too lit for this robot\n\n";
  server.send(404, "text/plain", message);
}



/* HTML */
String indexHTML() {
  String button_css = "width:100%; margin-bottom:1em; padding: 1em; font-family:'Arial';font-size:medium;color:#1d1f21; background-color:#8abeb7;border-color:#5e8d87;";
  String * danceMoves = dance_bot->getDanceMoves();
  String * danceRoutines = dance_bot->getDanceRoutines();
  
  String htmlPage = String("<head>") +
              "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />" + 
            "</head>" +
            
            "<body style=\"width:auto; font-family:'Arial'; background-color:#1d1f21; color:#c5c8c6;\">" +
              //Header
              "<div id=\"page_header\" style=\"margin: 0 5% 2em 5%; color:#cc6666;\">" +
                "<h1>Demobots Dancing Robot</h1>" +
              "</div>" +

              //Dance Moves
              "<div id=\"page_dances\" style=\"margin: 0 5% 2em 5%;\">" +
                "<h3 style=\"color:#81a2be;\">Dance Moves</h3>" +
                
                "<div id=\"receivedMessage.danceMoves\" style=\"\">" +             
                  "<div style=\"padding-left: 1.5em; font-size:medium;\">" +
                    "<p id=\"current_move\">Current Move: " + danceMoves[0] + "</p>" +
                  "</div>" +
  
                  "<div id=\"receivedMessage.danceMove_buttons\" style=\"padding-left: 1.5em; \">";
                    for (int i = 0; i < dance_bot->getNumDanceMoves(); i++) {
                      htmlPage += "<button onclick=\"postDancemove('" + danceMoves[i] + "')\" style=\"" + button_css + "\">" + danceMoves[i] + "</button>";
                    }
  htmlPage += String("</div>") +
                "</div>" +
              "</div>" +

              //Dance Routines
              "<div id=\"page_routines\" style=\"margin: 0 5% 2em 5%;\">" +
                "<h3 style=\"color:#81a2be;\">Dances</h3>" +

                "<div id=\"dance_routines\" style=\"\">" +             
                  "<div style=\"padding-left: 1.5em; font-size:medium;\">" +
                    "<p id=\"current_routine\">Current Dance: None</p>" +
                  "</div>" +
                  "<div id=\"dance_routine_buttons\" style=\"padding-left: 1.5em; \">";
                  for (int i = 0; i < dance_bot->getNumDanceRoutines(); i++) {
                      htmlPage += "<button onclick=\"postDanceRoutine('" + danceRoutines[i] + "')\" style=\"" + button_css + "\">" + danceRoutines[i] + "</button>";
                    }
  htmlPage += String("</div>") +
                "</div>" + 
                
              "</div>" +
              
              getJavascript() +
            "</body>";
  return htmlPage;
}



/* JAVASCRIPT */

String getJavascript() {
  String s = String("<script>") +
      "function updateCurrentMove(move) {" +
        "document.getElementById('current_move').innerText = 'Current Move: ' + move; " +
      "}" +

      "function updateCurrentRoutiune(routine) {" +
        "document.getElementById('current_routine').innerText = 'Current Dance: ' + routine; " +
      "}" +
      
      //function to HTTP Post
      "function postDancemove(move) {" +
        "var xhttp = new XMLHttpRequest(); " +
        "xhttp.open('POST', '/danceM', true);" +
        "xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');" +
        "xhttp.send('receivedMessage.danceMove=' + move);" +

        "xhttp.onload = function() { " +
          "console.log('Move Received: ' + xhttp.responseText); " +
          "updateCurrentMove(xhttp.responseText)" +
        "}" +
      "}" +
      
      "function postDanceRoutine(routine) {" +
        "var xhttp = new XMLHttpRequest(); " +
        "xhttp.open('POST', '/dance', true);" +
        "xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');" +
        "xhttp.send('dance_routine=' + routine);" +

        "xhttp.onload = function() { " +
          "console.log('Dance Received: ' + xhttp.responseText); " +
          "updateCurrentRoutiune(xhttp.responseText)" +
        "}" +
      "}" +

  "</script>";
  return s;
}


