/**
 * Author: Matthew Yu
 * Last Modified: 05/22/20
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
            // connect to network
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

            Serial.println("Connected to network: " + String(CredentialsLog[foundNetwork].SSID));
            Serial.println("Attempting to connect to server at: " + IpAddress2String(Robots[ROBOT_ID].defaultIP));
            // Is there a response at the relevant IP? If so, establish a connection to it.
            // send POST request at IP address/hook /robotJoin with robot id payload.
            int httpResponseCode = joinServer();
            if(httpResponseCode == OK) {
                Serial.println("OK response. Connected.");
                // 3a. Response found. Establish a connection to it.
                mode = CON;
            } else if((httpResponseCode == NO_SERVER) || (httpResponseCode == NO_RESPONSE)) {
                Serial.println("No server appears to be up. Starting up my own server.");
                // 3b. No response, host the page yourself using STA.
                mode = STA;
            }
        }
    } else {
        // 4. If no networks are found, setup a network and host the page using AP
        Serial.println("No networks found.");
        mode = AP;
    }

    if(mode == AP) { // set up network and my desired webpage IP address
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
        }
    }

    // Ready to start communicating
    Serial.println("\nIP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("MAC address: ");
    Serial.println(WiFi.macAddress());
    Serial.println("\nServer is up and running.\n\n");
}
/**
 * startServer sets up the URL hooks.
 */
void startServer() {
    Serial.println("Setting up server URL hooks");
    // generate URL hooks to edit the robot state
    server.on("/",          HTTP_GET,   [](AsyncWebServerRequest *request) {
        Serial.println("[SERVER] A user requests for the html page.");
        request->send(200, "text/html", sendHTML());
    });
    // robot states
    server.on("/reset",     HTTP_POST,  [](AsyncWebServerRequest *request) {
        handle_state(request, Reset);
    });
    server.on("/walk",      HTTP_POST,  [](AsyncWebServerRequest *request) {
        handle_state(request, Walk);
    });
    server.on("/hop",       HTTP_POST,  [](AsyncWebServerRequest *request) {
        handle_state(request, Hop);
    });
    server.on("/wiggle",    HTTP_POST,  [](AsyncWebServerRequest *request) {
        handle_state(request, Wiggle);
    });
    server.on("/ankles",    HTTP_POST,  [](AsyncWebServerRequest *request) {
        handle_state(request, Ankles);
    });
    server.on("/demo1",     HTTP_POST,  [](AsyncWebServerRequest *request) {
        handle_state(request, Demo1);
    });
    server.on("/demo2",     HTTP_POST,  [](AsyncWebServerRequest *request) {
        handle_state(request, Demo2);
    });
    server.on("/robotJoin", HTTP_POST,  [](AsyncWebServerRequest *request) {
        handle_joinServer(request);
    });
    server.on("/getState",  HTTP_POST,  [](AsyncWebServerRequest *request) {
        handle_getState(request);
    });
    server.onNotFound(handle_notFound);
    /* END */

    server.begin();
    delay(1000);
    Serial.println("HTTP server started");
}


/* ---------------------CLIENT REQUESTS--------------------- */
/**
 * joinServer sends a POST request with the robot id to IP/robotJoin. Returns http response code.
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

    return http.POST(httpRequestData);
}
/**
 * getState sends a POST request with the robot id to the IP/getState. Returns http response code and robot state struct.
 */
String getState() {
    HTTPClient http;
    String queryPath = "http://" + IpAddress2String(Robots[ROBOT_ID].defaultIP) + "/getState"; // NOTE: default path to add Demobot to network
    http.begin(queryPath.c_str());
    // Serial.println("Send POST request to domain " + queryPath);

    // check for response
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "robot_id=" + String(ROBOT_ID);
    // Serial.println("With request payload " + httpRequestData);

    int httpResponseCode =  http.POST(httpRequestData);

    String payload = "";
    // handle response code.
    if(httpResponseCode == OK) {
        // Serial.println("OK response. Server says that request has been receieved.");
        // code reused from https://github.com/espressif/arduino-esp32/blob/master/libraries/HTTPClient/examples/StreamHttpClient/StreamHttpClient.ino
        // get length of document (is -1 when Server sends no Content-Length header)
        int len = http.getSize();
        // create buffer for read
        uint8_t buff[128] = {0};
        // get tcp stream
        WiFiClient * stream = http.getStreamPtr();
        // read all data from server
        while(http.connected() && (len > 0 || len == -1)) {
            // get available data size
            size_t size = stream->available();
            if(size) {
                // read up to 128 byte
                int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                if(len > 0) {
                    len -= c;
                }
            }
            delay(1);
        }
        payload = String((char*) buff);
    } else {
        if(httpResponseCode == NO_SERVER) {
            Serial.println("NO_SERVER response.");
        } else {
            Serial.println("Unexpected response code error: " + String(httpResponseCode));
        }
    }

    return payload;
}


/* ---------------------SERVER HANDLER REQUESTS--------------------- */
/**
 * handle_getState is a POST request with a robot id to grab a wanted robot's state
 * @return: HTTP CODE 200 on success
 *          HTTP CODE 400 on invalid request
 *          HTTP CODE 404 on robotNotFound
 */
void handle_getState(AsyncWebServerRequest *request) {
    Serial.println("[SERVER] Received a request by a ROBOT to GRAB its updated state.");

    // check if request is correct
    if(!request->hasArg("robot_id")) {
        request->send(400, "text/plain", "400: Invalid Request, arg must be robot_id");
        return;
    }

    // for list of robots connected, check if robot id matches any connected robots
    for(int i = 0; i < numConnectedRobots; i++) {
        if( request->arg("robot_id").equals(String(connectedRobots[i].robotID)) ) {
            // TODO: currently only sends robot dance state. Increase functionality by sending a JSON/struct of eye color and expression. maybe ID to crosscheck requests.
            request->send(200, "text/html", "STATE:" + String(connectedRobots[i].robotState) + ";"); // 
            return;
        }
    }
    // 404 if robot isn't connected or doesn't exist
    request->send(404, "text/plain", "404: Robot Not Found");
}
/**
 * handle_joinServer is a POST request that does the following:
 *  * check if there is a live server, for purposes of serverSetup
 *  * updates the server list of active Robots that have joined
 * @return: HTTP CODE 200 on success
 *          HTTP CODE 400 on invalid request
 * @note: ignores if the robot has already joined. Returns success.
 */
void handle_joinServer(AsyncWebServerRequest *request) {
    Serial.println("[SERVER] Received request by a ROBOT to join the server.");
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
 * handle_state is a helper function to facilitate execution of other handle methods that modify a robot's state.
 * @return: HTTP CODE 200 on success
 *          HTTP CODE 400 on invalid request
 *          HTTP CODE 404 on robotNotFound
 */
void handle_state(AsyncWebServerRequest *request, int state) {
    // Serial.println("[SERVER] Received a request by a USER to UPDATE a robot's state.");
    // check if request arg is correct
    if( !request->hasArg("robot_id") ) {
        request->send(400, "text/plain", "400: Invalid Request, arg must be robot_id");
        return;
    }

    // for list of robots connected, check if robot id matches any connected robots
    for(int i = 0; i < numConnectedRobots; i++) {
        if( request->arg("robot_id").equals(String(connectedRobots[i].robotID)) ) {
            Serial.println("Robot " + request->arg("robot_id") + " state changed to " + dancebotStates[state] + ".");
            connectedRobots[i].robotState = state;
            request->send(200, "text/html", String(connectedRobots[i].robotState));
            return;
        }
    }
    request->send(404, "text/plain", "404: Robot ID Not Found");
}
/**
 * handle_NotFound is For 404 redirect requests.
 */
void handle_notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "404: Not found");
}

/* ---------------------WEB PAGE------------------------------------ */
/**
 * sendHTML sends a set of HTML to the user in response to a POST request based on the new WebController state.
 * TODO: currently no support for multiple robots.
 */
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
                                    "<p id=\"current_move\">Current Move: Reset</p>" +
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
/**
 * sendJavascript sends a set of JS to the user in response to a POST request based on the new WebController state.
 * TODO: currently no support for multiple robots.
 */
String sendJavascript() {
  String s = String("<script>") +
    // update the current state in the HTML
    "function updateCurrentMove(move) {" +
        "document.getElementById('current_move').innerText = 'Current Move: ' + move;" +
        "document.getElementById('current_routine').innerText = 'Current Dance: ' + move;" +
    "}" +

    //function to HTTP Post
    "async function postMove(move) {" +
        "let response = await fetch('/' + move, {" +
            "method: 'POST'," +
            "headers: {" +
                "'Content-Type': 'application/x-www-form-urlencoded;charset=UTF-8'" +
            "}," +
            "body: \"robot_id=0\"" +
        "}).then(function(response) {" +
            "console.log(response.status);" +
            "if(response.status == 200) {" + 
                "updateCurrentMove(move);" +    
            "}" +
            "console.log(response.text());" +
        "});" +
    "}" +
  "</script>";
  return s;
}


/* ---------------------HELPER FUNCTIONS---------------------------- */
/**
 * IpAddress2String converts an IPAddress object into a String object.
 * Returns a String.
 * @author: apicquot from https://forum.arduino.cc/index.php?topic=228884.0
 */
String IpAddress2String(const IPAddress& ipAddress) {
    return String(ipAddress[0]) + String(".") +\
        String(ipAddress[1]) + String(".") +\
        String(ipAddress[2]) + String(".") +\
        String(ipAddress[3]) ;
}
