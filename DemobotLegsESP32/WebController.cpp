/**
 * Author: Matthew Yu
 * Last Modified: 05/06/20
 * Project: Dancebot Swarm
 * File: WebController.cpp
 * Description: A generic secure web controller for controlling various Demobots Projects. Robots should be able to: connect to a remote server and receive/send commands, as well as spin up their own server and serve a webpage to the user to directly interact with it.
 * Organization: UT IEEE RAS
 */
#include "WebController.h"

/**
 * setupWifi determines what network connection should be used, if any, and sets it up if id doesn't exist already.
 */
void setupWifi() {
    int mode;
    // Initialize serial for debugging purposes
    Serial.begin(115200);
    delay(100);

    // 1. Scan networks
    int networks = WiFi.scanNetworks();

    // 2. If a network is found, attempt to connect to it
    if(networks > 0) {
        // 3. Check networks against credentials log, looking for the highest priority match (in this case, index ascending order)
        int foundNetwork = 0;
        for (int i = CredentialsLogLength-1; i < 0; i--) {
            for (int j = 0; j < networks; j++) {
                if(WiFi.SSID(i).equals(String(CredentialsLog[i].SSID))) {
                    foundNetwork = i;
                }
            }
        }
        // set relevant SSID and password
        // connect to network
        WiFi.begin(CredentialsLog[foundNetwork].SSID, CredentialsLog[foundNetwork].PASSWORD);
        while (WiFi.status() != WL_CONNECTED) {
            // I'm either in WL_IDLE_STATUS or WL_CONNECT_FAILED.
            if(WiFi.status() == WL_CONNECT_FAILED) {
                Serial.println("Connection failed error. Halting.");
                while(1) {
                    int i = 0;
                }
            }
            Serial.print(".");
            delay(RETRY_WAIT);
        }
        Serial.print("Connected to ");
        Serial.println(CredentialsLog[foundNetwork].SSID);

        // Is there a response at the relevant IP? If so, establish a connection to it.
        // send GET request at IP address/hook
        // TODO: check to see if i need to specify timeout https://github.com/espressif/arduino-esp32/blob/master/libraries/HTTPClient/src/HTTPClient.cpp
        HTTPClient http;
        Serial.print("[HTTP] begin...\n");
        String queryPath = Robots[ROBOT_IDX].defaultIP + "/robotJoin"; // NOTE: default path to add Demobot to network
        http.begin(queryPath.c_str());

        // check for response
        int httpResponseCode = http.GET();
        if(httpResponseCode == OK) {
            // 3a. Response found. Establish a connection to it.
            mode = CON;
        } else if(httpResponseCode == NO_SERVER) {
            // 3b. No response, host the page yourself using STA.
            mode = STA;
        } else { // Catch wrong reponse code error
            Serial.println("Unexpected response code error. Halting.");
            Serial.println(httpResponseCode);
            while(1) {
                int i = 0;
            }
        }
    } else {
        // 4. If no networks are found, setup a network and host the page using AP
        mode = AP;
    }

    if(mode == AP) { // set up network and my desired webpage IP address
        WiFi.softAP(CredentialsLog[DEFAULT_NETWORK_ID].SSID, CredentialsLog[DEFAULT_NETWORK_ID].PASSWORD);
        WiFi.softAPConfig(Robots[ROBOT_IDX].defaultIP, gateway, subnet); 
        delay(100);
    }else if(mode == STA) { // set up my desired webpage IP address
        WiFi.softAPConfig(Robots[ROBOT_IDX].defaultIP, gateway, subnet); 
        delay(100);
    }

    if(mode != CON) {
        startServer();
    }
    // Ready to start communicating
}

/**
 * startServer sets up the URL hooks.
 */
void startServer() {
    /* TODO: MODIFY BELOW WITH YOUR RELEVANT ROBOT API */
    // generate URL hooks to edit the robot state
    server.on("/",          HTTP_GET,  handle_OnConnect);
    server.on("/reset",     HTTP_POST,  handle_Reset);
    server.on("/walk",      HTTP_POST,  handle_Walk);
    server.on("/hop",       HTTP_POST,  handle_Hop);
    server.on("/wiggle",    HTTP_POST,  handle_Wiggle);
    server.on("/ankles",    HTTP_POST,  handle_Ankles);
    server.on("/demo1",     HTTP_POST,  handle_Demo1);
    server.on("/demo2",     HTTP_POST,  handle_Demo2);
    server.on("/getState",  HTTP_POST,  handle_GetState);
    server.on("/robotJoin", HTTP_POST,  handle_RobotJoin);
    server.onNotFound(handle_NotFound);
    /* END */

    server.begin();
    Serial.println("HTTP server started");
}

/* MODIFY BELOW WITH YOUR RELEVANT ROBOT */

/**
 * handle_OnConnect - GET request to get the webpage from server.
 */
void handle_OnConnect() {
    Serial.println("Server received new client.");
    // serve HTML page
    server.send(200, "text/html", sendHTML());
}

/**
 * handle_Reset - POST request to set Robot id to state Reset
 * handle_Walk  - POST request to set Robot id to state Walk
 * handle_Hop   - POST request to set Robot id to state Hop
 * handle_Wiggle- POST request to set Robot id to state Wiggle
 * handle_Ankles- POST request to set Robot id to state Ankles
 * handle_Demo1 - POST request to set Robot id to state Demo1
 * handle_Demo2 - POST request to set Robot id to state Demo2
 * @return: HTTP CODE 200 on success
 *          HTTP CODE 400 on invalid request
 *          HTTP CODE 404 on robotNotFound
 */
void handle_Reset() { handle_State(Reset);  }
void handle_Walk()  { handle_State(Walk);   }
void handle_Hop()   { handle_State(Hop);    }
void handle_Wiggle(){ handle_State(Wiggle); }
void handle_Ankles(){ handle_State(Ankles); }
void handle_Demo1() { handle_State(Demo1);  }
void handle_Demo2() { handle_State(Demo2);  }

/**
 * handle_GetState is a POST request with a robot id to grab a wanted robot's state
 * @return: HTTP CODE 200 on success
 *          HTTP CODE 400 on invalid request
 *          HTTP CODE 404 on robotNotFound
 */
void handle_GetState() {
    // check if request is correct
    if(!server.hasArg("robot_id")) {
        server.send(400, "text/plain", "400: Invalid Request, arg must be robot_id");
        return;
    }
    // for list of robots connected, check if robot id matches any connected robots
    for(int i = 0; i < numConnectedRobots; i++) {
        if(server.arg("robot_id").equals(String(connectedRobots[i].robotID))) {
            server.send(200, "text/html", String(connectedRobots[i].robotState));
        }
    }
    server.send(404, "text/plain", "404: Robot Not Found");
}

/**
 * handle_NotFound is For 404 redirect requests.
 */
void handle_NotFound() {
    server.send(404, "text/plain", "404: Not found");
}

/**
 * handle_RobotJoin is a POST request that does the following:
 *  * check if there is a live server, for purposes of serverSetup
 *  * updates the server list of active Robots that have joined
 * @return: HTTP CODE 200 on success
 *          HTTP CODE 400 on invalid request
 * @note: ignores if the robot has already joined. Returns success.
 */
void handle_RobotJoin() {
    // send empty payload and an OK
    if( !server.hasArg("robot_id") ) {
        server.send(400, "text/plain", "400: Invalid Request, arg must be robot_id");
        return;
    }
    // for list of robots connected, check if robot id matches any connected robots
    bool found = false;
    for(int i = 0; i < numConnectedRobots; i++) {
        if( server.arg("robot_id").equals(String(connectedRobots[i].robotID)) ) { found = true; break; }
    }
    if(!found) {
        // TODO: consider hashing to an integer and checking that
        server.arg("robot_id").toCharArray(connectedRobots[0].robotID, MAX_ROBOT_ID_LENGTH);
        numConnectedRobots++;
    }
    server.send(200, "text/html", "Robot Successfully Joined.");
}

/**
 * handle_State is a helper function to facilitate execution of other handle methods that modify a robot's state.
 * @return: HTTP CODE 200 on success
 *          HTTP CODE 400 on invalid request
 *          HTTP CODE 404 on robotNotFound
 */
void handle_State(DancebotStates state) {
    // check if request arg is correct
    if( !server.hasArg("robot_id") ) {
        server.send(400, "text/plain", "400: Invalid Request, arg must be robot_id");
        return;
    }
    // for list of robots connected, check if robot id matches any connected robots
    for(int i = 0; i < numConnectedRobots; i++) {
        if( server.arg("robot_id").equals(String(connectedRobots[i].robotID)) ) {
            connectedRobots[i].robotState = state;
            server.send(200, "text/html", String(connectedRobots[i].robotState));
        }
    }
    server.send(404, "text/plain", "404: Robot ID Not Found");
}

/* MODIFY BELOW WITH YOUR RELEVANT ROBOT */
// TODO: update these with same UI but new API
/* HTML */
/**
 * sendHTML sends a set of HTML to the user in response to a POST request based on the new WebController state.
 */
int stateEnum = Reset;
String sendHTML() {
    String  button_css ="width:100%; margin-bottom:1em; padding: 1em; font-family:'Arial';font-size:medium;color:#1d1f21; background-color:#8abeb7;border-color:#5e8d87;";
    String  body_css =  "width:auto; font-family:'Arial'; background-color:#1d1f21; color:#c5c8c6;";
    String  head =  "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /></head>";
    String  body =  "<body style=\"" + body_css + "\">" +
        /*HEADER*/      "<div id=\"page_header\" style=\"margin: 0 5% 2em 5%; color:#cc6666;\">" +
                            "<h1>Demobots Dancing Robot</h1>" +
                        "</div>" +
        /*MOVES */      "<div id=\"page_dances\" style=\"margin: 0 5% 2em 5%;\">" +
                            "<h3 style=\"color:#81a2be;\">Dance Moves</h3>" +
                            "<div id=\"dance_moves\" style=\"\">" +             
                                "<div style=\"padding-left: 1.5em; font-size:medium;\">" +
                                    "<p id=\"current_move\">Current Move: " + dancebotStates[stateEnum] + "</p>" +
                                "</div>" +
                                "<div id=\"dance_move_buttons\" style=\"padding-left: 1.5em; \">";
                                    for (int i = Reset; i < Demo1; i++) {
                                        body += "<button onclick=\"postMove('" + String(dancebotStates[i]) + "')\" style=\"" + button_css + "\">" + dancebotStates[i] + "</button>";
                                    }
            body +=             "</div>" +
                            String("</div>") +
                        "</div>" +

        /*ROUTINES*/    "<div id=\"page_routines\" style=\"margin: 0 5% 2em 5%;\">" +
                            "<h3 style=\"color:#81a2be;\">Dances</h3>" +
                            "<div id=\"dance_routines\" style=\"\">" +             
                                "<div style=\"padding-left: 1.5em; font-size:medium;\">" +
                                    "<p id=\"current_routine\">Current Dance: None</p>" +
                                "</div>" +
                                "<div id=\"dance_routine_buttons\" style=\"padding-left: 1.5em; \">";
                                    for (int i = Demo1; i < NumStates; i++) {
                                        body += "<button onclick=\"postMove('" + String(dancebotStates[i]) + "')\" style=\"" + button_css + "\">" + String(dancebotStates[i]) + "</button>";
                                    }
            body +=             "</div>" +
                            String("</div>") + 
                        "</div>" +
                        sendJavascript() +
                    "</body>";
    head += body;
    return head;
}
/* JAVASCRIPT */
String sendJavascript() {
  String s = String("<script>") +
    // update the current state in the HTML
    "function updateState(move) {" +
        "for(int i = 0; i < NumStates; i++) {" +
            "if(dancebotState[i].equals(move)) {" +
                "stateEnum = i;" +          // TODO: may need to stick variable in the html
                "break;" +
            "}" +
        "}" +
    "}" +

    //function to HTTP Post
    "function postMove(move) {" +
        "var xhttp = new XMLHttpRequest(); " +
        "xhttp.open('POST', '/' + move, true);" +
        "xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');" +
        "xhttp.send('robot_id=0');" +       // HARD CODED robot idx

        "xhttp.onload = function() { " +
            "console.log('Move Received: ' + xhttp.responseText); " +
            "console.log('Set move to: ' + move + ' in the HTML.'); "
            "updateCurrentMove(move)" +
        "}" +
    "}" +
  "</script>";
  return s;
}