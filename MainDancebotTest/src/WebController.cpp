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
//#include <esp_now.h>
#include <esp_now.h>
#include "DancingServos.h"


void handleRoot();
void handleDanceMove();
void handleDance();
void handleBattery();
void handleNotFound();
void handleUnknownMove();

String indexHTML();
String getJavascript();


/* Data Transmission */
esp_now_peer_info_t peerInfo;
#define NUM_ADDRESS 2 //# of clients / MAC addresses
 /* add more MAC addresses below */
uint8_t address0[] = {0x30, 0x83, 0x98, 0xDF, 0xC0, 0xAC}; // REPLACE WITH YOUR RECEIVER MAC Address
uint8_t address1[] = {0x30, 0x83, 0x98, 0xDF, 0xC1, 0x68};
uint8_t* addressArr[] = {address0, address1};
struct_message transmitMessage;  //message sent to clients
struct_message receivedMessage; //message received by clients

//enums that correspond to dance moves
enum{
  STOP,
  RESET,
  WALK,
  HOP,
  WIGGLE,
  ANKLES,
  DEMO1,
  DEMO2
};
// enum for return info
enum{
  None,
  SetID,
  BattLevel,
}; 

//battery levels for each dancebot
float batteryLevel[NUM_ADDRESS];

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

void printMACAddress(){
  Serial.println(WiFi.macAddress());
}

//callback when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //print MAC address
  char macStr[18];
  Serial.print("Transmitted to MAC: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);

  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

//callback when data is received
void onDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
  memcpy(&receivedMessage, incomingData, sizeof(receivedMessage));
  Serial.println("Received message...");
  Serial.print("Bytes received: ");
  Serial.println(len);

  if(receivedMessage.status == BattLevel){
    transmitMessage.batteryFlag = 0; //don't ask for battery level anymore (could be redundant if you always set flag = 0 each time you ask for battlvl)
    Serial.print("Received battery level from Dancebot"); Serial.println(receivedMessage.id);
    Serial.print("Battery Level is: "). Serial.println(receivedMessage.batteryLevel);
    batteryLevel[receivedMessage.id] = receivedMessage.batteryLevel; //retrieve battery level from dancebot X
  }
}

/* Setup Functions */

/* setupESPNOW 
 * Sets up transmit and receive communication with Mothership
*/
int setupESPNOW(){
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return 0;
  }

  esp_now_register_send_cb(onDataSent); //func called when we send data

  //for all dancebots, assign IDs and add as peer
  for(int i = 0; i < NUM_ADDRESS; i++){ 
    memcpy(peerInfo.peer_addr, addressArr[i], sizeof(peerInfo.peer_addr));
    peerInfo.channel = 0; 
    peerInfo.encrypt = false;

    // Add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.println("Failed to add peer");
      return 0;
    }
    //assign ID of newly added peer
    Serial.print("Assigning ID to Dancebot "); Serial.println(i);
    transmitMessage.id = i;
    transmitMessage.status = SetID;
    esp_err_t result = esp_now_send(addressArr[i], (uint8_t *) &transmitMessage, sizeof(transmitMessage));
    Serial.println("Transmitted message");
    transmitMessage.status = None;
    batteryLevel[i] = 100; //initialize battery level values for receiving
  }

  esp_now_register_recv_cb(onDataRecv); //func called when we receive data
  Serial.println("Finished setting up ESPNOW");
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
  
  /*****TEST WIFI CODE******/
  WiFi.mode(WIFI_STA);
 
  // if (esp_now_init() != ESP_OK) {
  //   Serial.println("Error initializing ESP-NOW");
  //   return;
  // }

  //  esp_now_register_send_cb(data_sent); //each time msg sent, calls data_sent()

  // //pairing main dancebot to other client dancebots
  // esp_now_peer_info_t peerInfo;
  // peerInfo.channel = 0;  
  // peerInfo.encrypt = false;

  // //add dancebot client 1 as peer to main dancebot
  // memcpy(peerInfo.peer_addr, DancebotReceiver1, 6);
  // int result = esp_now_add_peer(&peerInfo);
  // if (result != ESP_OK){
  //   Serial.println("Failed to add peer");
  //   Serial.println("Result = " + result);
  //   return;
  // }

  /*****OLD WIFI CODE******/
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
  server.on("/battery", HTTP_GET, handleBattery); 
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
  //hasArg() checks if the last HTTP request in the server has an argument
  //arg() gets the value of the arg by name

  //check for serial input form
  String dance_move = "";
  if(server.hasArg("dance_move")) {
    dance_move = server.arg("dance_move");
    Serial.println("Server received dance_move: " + dance_move);

    if (dance_move == "Stop") {
      dance_bot->stopOscillation();
      dance_bot->enableDanceRoutine(false);
      transmitMessage.danceMove = STOP;
      strcpy(transmitMessage.character, "Dance: STOP");
      transmitMessage.batteryFlag = 1; // ************REMOVE AFTER TESTING **************
    }
    else if (dance_move == "Reset") {
      dance_bot->position0();
      transmitMessage.danceMove = RESET;
      strcpy(transmitMessage.character, "Dance: RESET");
    }
    else if (dance_move == "Walk") {
      dance_bot->walk(-1, 1500, false);
      transmitMessage.danceMove = WALK;
      strcpy(transmitMessage.character, "Dance: WALK");
    }
    else if (dance_move == "Hop") {
      dance_bot->hop(25, -1);
      transmitMessage.danceMove = HOP;
      strcpy(transmitMessage.character, "Dance: HOP");
    }
    else if (dance_move == "Wiggle") {
      dance_bot->wiggle(30, -1);
      transmitMessage.danceMove = WIGGLE;
      strcpy(transmitMessage.character, "Dance: WIGGLE");
    }
    else if (dance_move == "Ankles") {
      dance_bot->themAnkles(-1);
      transmitMessage.danceMove = ANKLES;
      strcpy(transmitMessage.character, "Dance: ANKLES");
    }
    else {
      Serial.println("Dance move not recognized, ERROR too lit for this robot");
      handleUnknownMove();
      return;
    }
  }
  else {
    dance_move = "ERROR Server did not find dance move argument in HTTP request";
    strcpy(transmitMessage.character, "Server argument not found");
  }

  //transmit message to all clients
  for(int i = 0; i < NUM_ADDRESS; i++){
    esp_err_t result = esp_now_send(addressArr[i], (uint8_t *) &transmitMessage, sizeof(transmitMessage));
    if (result == ESP_OK) {
      Serial.print("Sent Dancebot ");
      Serial.print(i);
      Serial.print(" msg with success");
    }
    else {
      Serial.print("Error sending Dancebot ");
      Serial.print(i);
      Serial.println(" data");
    }
  }

  //delay(1000);

  //handleRoot();     //now the form is handled with JS so there is no need to respond with index html
  server.send(200, "text/plain", dance_move);
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
      //strcpy(message.character, "Demo 1");
      transmitMessage.danceMove = DEMO1;
    }
    else if (dance_routine.equals("Demo 2")) {
      dance_bot->setDanceRoutine(1);
      dance_bot->enableDanceRoutine(true);
      //strcpy(message.character, "Demo 2");
      transmitMessage.danceMove = DEMO2;
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

void handleBattery() {
  // TO DO
  
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
                
                "<div id=\"dance_moves\" style=\"\">" +             
                  "<div style=\"padding-left: 1.5em; font-size:medium;\">" +
                    "<p id=\"current_move\">Current Move: " + danceMoves[0] + "</p>" +
                  "</div>" +
  
                  "<div id=\"dance_move_buttons\" style=\"padding-left: 1.5em; \">";
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

              // Battery Level
              "<div id=\"battery-indicator\" style=\"display: flex; justify-content:center; flex-direction:column; width:50%; height:fit-content; align-items: center; margin: auto; padding: 10px; border-radius: 5px; background-color:#42f5ef\">" +
                "<div id=\"battery-meter\" style=\"position:relative; width: 70%; margin:auto; background-color:white; padding:10px; align-items:center; border-radius:50px; text-align: center; overflow: hidden\">" +
                  "<div id=\"battery-fill\" style=\"position:absolute; top:0; left: 0; height: 100%; width: 100%; background-color:aquamarine; transition: width 0.5s ease; z-index: 1;\"></div>" +
                  "<span id=\"battery-percentage\" style=\"position: relative; margin: auto; z-index: 2\">100%</span>" +
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

      "function updateBatteryLevel(battery) {" +
        "document.getElementById('battery').innerText = battery + '%';" +
        "document.getElementById('battery_fill').width = battery + '%';" +
      "}" +

      "function updateCurrentRoutiune(routine) {" +
        "document.getElementById('current_routine').innerText = 'Current Dance: ' + routine; " +
      "}" +
      
      //function to HTTP Post
      "function postDancemove(move) {" +
        "var xhttp = new XMLHttpRequest(); " +
        "xhttp.open('POST', '/danceM', true);" +
        "xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');" +
        "xhttp.send('dance_move=' + move);" +

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

      "function getBatteryLevel() {" +
        "var xhttp = new XMLHttpRequest();" +
        "xhttp.open('GET', '/battery', true);" +
        "xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');" +
        "xhttp.send();" +
        "xhttp.onload = function() {" +
          "console.log('Battery Received: ' + xhttp.responseText);" +
          "updateBatteryLevel(xhttp.responseText);" +
        "}" +
      "}" + 

      "getBatteryLevel();" +
      "setInterval(getBatteryLevel, 10000);" +

  "</script>";
  return s;
}




