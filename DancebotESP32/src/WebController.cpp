/**
 * Author: Matthew Yu
 * Last Modified: 05/06/20
 * Project: Dancebot Swarm
 * File: WebController.cpp
 * Description: A generic secure web controller for controlling various Demobots Projects. Robots should be able to: connect to a remote server and receive/send commands, as well as spin up their own server and serve a webpage to the user to directly interact with it.
 * Organization: UT IEEE RAS
 */
#include "WebController.h"

AsyncWebServer server(80);
int mode = CON;

/**
 * setupWifi determines what network connection should be used, if any, and sets it up if id doesn't exist already.
 */
void setupWifi() {
    Serial.println("Setting up WIFI.");
    Serial.println("Robot identified as: " + String(Robots[ROBOT_ID].name));

    // Initialize serial for debugging purposes
    Serial.begin(115200);
    delay(100);

    // 1. Scan networks
    Serial.println("Starting Network Scan.");
    int networks = WiFi.scanNetworks();

    if (networks == 0) {
        Serial.println("No networks found.");
    } else {
        Serial.print(networks);
        Serial.println(" networks found: ");
        for (int i = 0; i < networks; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }

    // 2. If a network is found, attempt to connect to it
    if(networks > 0) {
        // 3. Check networks against credentials log, looking for the highest priority match (in this case, index ascending order)
        Serial.println("Checking against credentials log.");

        int foundNetwork = -1;
        for (int i = 0; (i < CRED_LOG_LEN) && (foundNetwork == -1); i++) {
            for (int j = 0; j < networks; j++) {
                if(WiFi.SSID(j).equals(String(CredentialsLog[i].SSID))) {
                    foundNetwork = i;
                    break;
                }
            }
        }

        if(foundNetwork == -1) {
            Serial.println("Didn't find any matching networks.");
            mode = AP;
        }else {
            Serial.println("Attempting to connect to network: " + String(CredentialsLog[foundNetwork].SSID));
            // set relevant SSID and password
            // connect to network TODO: Guru core panic here
            WiFi.begin(CredentialsLog[foundNetwork].SSID, CredentialsLog[foundNetwork].PASSWORD);
            Serial.println("Checking wifi status");
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

            Serial.println("\nConnected to network: " + String(CredentialsLog[foundNetwork].SSID));
            Serial.println("Attempting to connect to server at: " + IpAddress2String(Robots[ROBOT_ID].defaultIP));
            // Is there a response at the relevant IP? If so, establish a connection to it.
            // send POST request at IP address/hook /robotJoin with robot id payload.
            int httpResponseCode = joinServer();
            if(httpResponseCode == OK) {
                Serial.println("OK response. Connected.");
                // 3a. Response found. Establish a connection to it.
                mode = CON;
            } else if(httpResponseCode == NO_SERVER) {
                Serial.println("NO_SERVER response. Starting up my own server.");
                // 3b. No response, host the page yourself using STA.
                mode = STA;
            } else { // Catch wrong reponse code error
                Serial.println("Unexpected response code error. Halting.");
                Serial.println(httpResponseCode);
                while(1) {
                    int i = 0;
                }
            }
        }
    } else {
        // 4. If no networks are found, setup a network and host the page using AP
        Serial.println("No networks found.");
        mode = AP;
    }

    if(mode == AP) { // set up network and my desired webpage IP address
        // TODO: guru core panic here
        Serial.println("Setting up default network at: " + String(CredentialsLog[DEFAULT_NETWORK_ID].SSID));
        WiFi.softAP(CredentialsLog[DEFAULT_NETWORK_ID].SSID, CredentialsLog[DEFAULT_NETWORK_ID].PASSWORD);
        delay(500);
        Serial.println("Setting up server at: " + IpAddress2String(Robots[ROBOT_ID].defaultIP));
        WiFi.softAPConfig(Robots[ROBOT_ID].defaultIP, gateway, subnet); 
        delay(500);
    }else if(mode == STA) { // set up my desired webpage IP address
        Serial.println("Setting up server at: " + IpAddress2String(Robots[ROBOT_ID].defaultIP));
        WiFi.softAPConfig(Robots[ROBOT_ID].defaultIP, gateway, subnet); 
        delay(500);
    }
    if(mode != CON) {
        startServer();

        // send self request to join.
        int httpResponseCode = joinServer();
        // handle response code.
        if(httpResponseCode == OK) {
            Serial.println("OK response. Connected.");
        } else {
            if(httpResponseCode == NO_SERVER) {
                Serial.println("NO_SERVER response.");
            } else {
                Serial.println("Unexpected response code error: " + String(httpResponseCode));
            }
            Serial.println("Halting.");
            // while(1) {
            //     int i = 0;
            // }
        }
    }

    // Ready to start communicating
    Serial.println("Server is up and running.");
}

/**
 * startServer sets up the URL hooks.
 */
void startServer() {
    Serial.println("Setting up server URL hooks");
    /* TODO: MODIFY BELOW WITH YOUR RELEVANT ROBOT API */
    // generate URL hooks to edit the robot state
    server.on("/",          HTTP_GET,   [](AsyncWebServerRequest *request) {
        Serial.println("A user requests for the html page.");
        request->send(200, "text/html", sendHTML());
    });
    // robot states
    server.on("/reset",     HTTP_POST,  [](AsyncWebServerRequest *request) {
        handle_State(request, Reset);
    });
    server.on("/walk",      HTTP_POST,  [](AsyncWebServerRequest *request) {
        handle_State(request, Walk);
    });
    server.on("/hop",       HTTP_POST,  [](AsyncWebServerRequest *request) {
        handle_State(request, Hop);
    });
    server.on("/wiggle",    HTTP_POST,  [](AsyncWebServerRequest *request) {
        handle_State(request, Wiggle);
    });
    server.on("/ankles",    HTTP_POST,  [](AsyncWebServerRequest *request) {
        handle_State(request, Ankles);
    });
    server.on("/demo1",     HTTP_POST,  [](AsyncWebServerRequest *request) {
        handle_State(request, Demo1);
    });
    server.on("/demo2",     HTTP_POST,  [](AsyncWebServerRequest *request) {
        handle_State(request, Demo2);
    });

    server.on("/getState",  HTTP_POST,  [](AsyncWebServerRequest *request) {
        handle_GetState(request);
    });
    
    server.on("/robotJoin", HTTP_POST,  [](AsyncWebServerRequest *request) {
        handle_RobotJoin(request);
    });
    server.onNotFound(handle_NotFound);
    /* END */

    server.begin();
    delay(1000);
    Serial.println("HTTP server started");
}

// /**
//  * manageRequests manages client and user requests.
//  */
// void manageRequests() {
//     // https://www.reddit.com/r/esp8266/comments/942579/what_does_serverhandleclient_do/
//     if(mode != CON) {
//         server.handleClient();
//     }
// }

/**
 * handle_NotFound is For 404 redirect requests.
 */
void handle_NotFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "404: Not found");
}


/**
 * handle_GetState is a POST request with a robot id to grab a wanted robot's state
 * @return: HTTP CODE 200 on success
 *          HTTP CODE 400 on invalid request
 *          HTTP CODE 404 on robotNotFound
 */
void handle_GetState(AsyncWebServerRequest *request) {
    // check if request is correct
    if(!request->hasArg("robot_id")) {
        request->send(400, "text/plain", "400: Invalid Request, arg must be robot_id");
        return;
    }
    // for list of robots connected, check if robot id matches any connected robots
    for(int i = 0; i < numConnectedRobots; i++) {
        if( request->arg("robot_id").equals(String(connectedRobots[i].robotID)) ) {
            request->send(200, "text/html", String(connectedRobots[i].robotState));
            return;
        }
    }
    // 404 if robot isn't connected or doesn't exist
    request->send(404, "text/plain", "404: Robot Not Found");
}

/**
 * handle_RobotJoin is a POST request that does the following:
 *  * check if there is a live server, for purposes of serverSetup
 *  * updates the server list of active Robots that have joined
 * @return: HTTP CODE 200 on success
 *          HTTP CODE 400 on invalid request
 * @note: ignores if the robot has already joined. Returns success.
 */
void handle_RobotJoin(AsyncWebServerRequest *request) {
    Serial.println("Received a robot join request.");
    // send empty payload and an OK
    if( !request->hasArg("robot_id") ) {
        Serial.println("Payload is malformed. Num Args: " + String(request->args()));
        request->send(400, "text/plain", "400: Invalid Request, arg must be robot_id");
        return;
    }
    // for list of robots connected, check if robot id matches any connected robots
    bool found = false;
    for(int i = 0; i < numConnectedRobots; i++) {
        if( request->arg("robot_id").equals(String(connectedRobots[i].robotID)) ) { 
            found = true; 
            break; 
        }
    }
    if(!found) {
        connectedRobots[numConnectedRobots].robotID = request->arg("robot_id").toInt();
        numConnectedRobots++;
    }
    Serial.println("Robot successfully joined. ID: " + request->arg("robot_id"));
    request->send(200, "text/html", "Robot Successfully Joined.");
}

/**
 * handle_State is a helper function to facilitate execution of other handle methods that modify a robot's state.
 * @return: HTTP CODE 200 on success
 *          HTTP CODE 400 on invalid request
 *          HTTP CODE 404 on robotNotFound
 */
void handle_State(AsyncWebServerRequest *request, int state) {
    // check if request arg is correct
    if( !request->hasArg("robot_id") ) {
        request->send(400, "text/plain", "400: Invalid Request, arg must be robot_id");
        return;
    }
    // for list of robots connected, check if robot id matches any connected robots
    for(int i = 0; i < numConnectedRobots; i++) {
        if( request->arg("robot_id").equals(String(connectedRobots[i].robotID)) ) {
            connectedRobots[i].robotState = state;
            request->send(200, "text/html", String(connectedRobots[i].robotState));
        }
    }
    request->send(404, "text/plain", "404: Robot ID Not Found");
}



/**
 * IpAddress2String converts an IPAddress object into a String object.
 * Returns a String.
 * @author: apicquot from https://forum.arduino.cc/index.php?topic=228884.0
 */
String IpAddress2String(const IPAddress& ipAddress) {
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ; 
}

/**
 * joinServer sends a POST request to IP/robotJoin. Returns http response code.
 */
int joinServer() {
    HTTPClient http;
    String queryPath = "http://" + IpAddress2String(Robots[ROBOT_ID].defaultIP) + "/robotJoin"; // NOTE: default path to add Demobot to network
    http.begin(queryPath.c_str());
    Serial.println("Send POST request to domain " + queryPath);

    // check for response
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "robot_id=" + String(ROBOT_ID);
    Serial.println("With request payload " + httpRequestData);

    Serial.println("Sending POST request.");
    return http.POST(httpRequestData);
}










/* MODIFY BELOW WITH YOUR RELEVANT ROBOT */
/* HTML */
/**
 * sendHTML sends a set of HTML to the user in response to a POST request based on the new WebController state.
 */
// TODO: currently no support for multiple robots.
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
