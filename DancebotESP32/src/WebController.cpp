/**
 * Author: Matthew Yu
 * Last Modified: 05/22/20
 * Project: Dancebot Swarm
 * File: WebController.cpp
 * Description: A generic secure web controller for controlling various Demobots Projects. Robots should be able to: connect to a remote server and receive/send commands, as well as spin up their own server and serve a webpage to the user to directly interact with it.
 * Organization: UT IEEE RAS
 */
#include "WebController.h"

// AsyncWebServer server(80);

// Our dancebot
char dancebotName[9] = "Dancebot";
Dancebot dancebot {
    dancebotName,
    0,
    100.0,
    Reset,
    None,
    Off
};

// the network 
DemobotNetwork network(String(dancebot.robotType) + String(dancebot.robotID));

bool setupNetworking() {
    if (network.connectNetwork()) {
        // we're in the network
        if (network.pingServer()) {
            // and the remote server is up
            return true;
        } else {
            // remote server isn't up. Start our own?
            if (!setupServer()) return false;
            // attempt to ping again
            if (network.pingServer()) return true;
            else return false;
        }
    } else {
        Serial.println("[WARN] [setupNetworking] Unable to identify an available network.");
        if (!setupAPNetwork()) return false;
        // attempt to ping server
        if (network.pingServer()) {
            // and the remote server is up
            return true;
        } else {
            // remote server isn't up. Start our own?
            if (!setupServer()) return false;
            // attempt to ping again
            if (network.pingServer()) return true;
            else return false;
        }
    }
}

bool setupAPNetwork() {
    // set up our own network
    char* ssid = network.getNetworkSSID();
    char* pass = network.getNetworkPassword();
    if ((ssid == nullptr) || (pass == nullptr)) {
        Serial.println("[ERROR] [setupAPNetwork] Network was improperly configured and could not setup a network on AP mode.");
        return false;
    }

    Serial.println("[setupAPNetwork] Setting up default network at: " + String(ssid));
    if (!WiFi.softAP(ssid, pass)) {
        Serial.println("[ERROR] [setupAPNetwork] Unable to setup access point.");
        return false;
    }

    // give the network some time to run up
    delay(500);
    return true;
}

bool setupServer() {
    IPAddress ip = network.getIPAddress();
    if (!ip) {
        Serial.println("[ERROR] [setupServer] Server IP was improperly configured and could not setup a server on AP mode.");
        return false;
    }

    Serial.println("[setupServer] Setting up server at: " + network.IpAddress2String(network.getIPAddress()));
    if (!WiFi.softAPConfig(ip, gateway, subnet)) {
        Serial.println("[ERROR] [setupServer] Unable to setup server IP.");
        return false;
    }

    // give the server some time to run up
    startServer();
    delay(500);
    return true;
}


/**
 * startServer sets up the URL hooks.
 */
void startServer() {
    // Serial.println("Setting up server URL hooks");
    // // generate URL hooks to edit the robot state
    // server.on("/",          HTTP_GET,   [](AsyncWebServerRequest *request) {
    //     Serial.println("[SERVER] A user requests for the html page.");
    //     request->send(200, "text/html", sendHTML());
    // });
    // // robot states
    // server.on("/reset",     HTTP_POST,  [](AsyncWebServerRequest *request) {
    //     handle_state(request, Reset);
    // });
    // server.on("/walk",      HTTP_POST,  [](AsyncWebServerRequest *request) {
    //     handle_state(request, Walk);
    // });
    // server.on("/hop",       HTTP_POST,  [](AsyncWebServerRequest *request) {
    //     handle_state(request, Hop);
    // });
    // server.on("/wiggle",    HTTP_POST,  [](AsyncWebServerRequest *request) {
    //     handle_state(request, Wiggle);
    // });
    // server.on("/ankles",    HTTP_POST,  [](AsyncWebServerRequest *request) {
    //     handle_state(request, Ankles);
    // });
    // server.on("/demo1",     HTTP_POST,  [](AsyncWebServerRequest *request) {
    //     handle_state(request, Demo1);
    // });
    // server.on("/demo2",     HTTP_POST,  [](AsyncWebServerRequest *request) {
    //     handle_state(request, Demo2);
    // });
    // server.on("/robotJoin", HTTP_POST,  [](AsyncWebServerRequest *request) {
    //     handle_joinServer(request);
    // });
    // server.on("/getState",  HTTP_POST,  [](AsyncWebServerRequest *request) {
    //     handle_getState(request);
    // });
    // server.onNotFound(handle_notFound);
    // /* END */

    // server.begin();
    // delay(1000);
    // Serial.println("HTTP server started");
}


/* ---------------------CLIENT REQUESTS--------------------- */

String joinServer() {
    String keys[5] = {
        "ID",
        "CHARGE",
        "MOVE",
        "EC",
        "EE"
    };

    String vals[5] = {
        String(dancebot.robotID),
        String(dancebot.soc),
        state2string(dancebot.robotState),
        expression2String(dancebot.robotExpression),
        eyeColor2String(dancebot.robotEyeColor)
    };

    String *response = nullptr;
    int responseCode = network.sendGETRequest("/robotJoin", keys, vals, 5, response);
    if (responseCode != OK) {
        // throw error message
        return String("[ERROR] [joinServer] Unable to join the server. Bad HTTP code response: " + String(responseCode) + ".");
    }
    return *response;
}

String getState() {
    String keys[1] = {
        "ID"
    };

    String vals[1] = {
        String(dancebot.robotID)
    };

    String *response = nullptr;
    int responseCode = network.sendGETRequest("/robotJoin", keys, vals, 5, response);
    if (responseCode != OK) {
        // throw error message
        return String("[ERROR] [getState] Unable to update the robot state. Bad HTTP code response: " + String(responseCode) + ".");
    }
    return *response;
}


/* ---------------------SERVER HANDLER REQUESTS--------------------- */
/**
 * handle_getState is a POST request with a robot id to grab a wanted robot's state
 * @return: HTTP CODE 200 on success
 *          HTTP CODE 400 on invalid request
 *          HTTP CODE 404 on robotNotFound
 */
// void handle_getState(AsyncWebServerRequest *request) {
//     Serial.println("[SERVER] Received a request by a ROBOT to GRAB its updated state.");

//     // check if request is correct
//     if(!request->hasArg("robot_id")) {
//         request->send(400, "text/plain", "400: Invalid Request, arg must be robot_id");
//         return;
//     }

//     // for list of robots connected, check if robot id matches any connected robots
//     for(int i = 0; i < numConnectedRobots; i++) {
//         if( request->arg("robot_id").equals(String(connectedRobots[i].robotID)) ) {
//             // TODO: currently only sends robot dance state. Increase functionality by sending a JSON/struct of eye color and expression. maybe ID to crosscheck requests.
//             request->send(200, "text/html", "STATE:" + String(connectedRobots[i].robotState) + ";"); // 
//             return;
//         }
//     }
//     // 404 if robot isn't connected or doesn't exist
//     request->send(404, "text/plain", "404: Robot Not Found");
// }
/**
 * handle_joinServer is a POST request that does the following:
 *  * check if there is a live server, for purposes of serverSetup
 *  * updates the server list of active Robots that have joined
 * @return: HTTP CODE 200 on success
 *          HTTP CODE 400 on invalid request
 * @note: ignores if the robot has already joined. Returns success.
 */
// void handle_joinServer(AsyncWebServerRequest *request) {
//     Serial.println("[SERVER] Received request by a ROBOT to join the server.");
//     // send empty payload and an OK
//     if( !request->hasArg("robot_id") ) {
//         Serial.println("Payload is malformed. Num Args: " + String(request->args()));
//         request->send(400, "text/plain", "400: Invalid Request, arg must be robot_id");
//         return;
//     }
//     // for list of robots connected, check if robot id matches any connected robots
//     bool found = false;
//     for(int i = 0; i < numConnectedRobots; i++) {
//         if( request->arg("robot_id").equals(String(connectedRobots[i].robotID)) ) { 
//             found = true; 
//             break; 
//         }
//     }
//     if(!found) {
//         connectedRobots[numConnectedRobots].robotID = request->arg("robot_id").toInt();
//         numConnectedRobots++;
//     }
//     Serial.println("Robot successfully joined. ID: " + request->arg("robot_id"));
//     request->send(200, "text/html", "Robot Successfully Joined.");
// }
/**
 * handle_state is a helper function to facilitate execution of other handle methods that modify a robot's state.
 * @return: HTTP CODE 200 on success
 *          HTTP CODE 400 on invalid request
 *          HTTP CODE 404 on robotNotFound
 */
// void handle_state(AsyncWebServerRequest *request, int state) {
//     // Serial.println("[SERVER] Received a request by a USER to UPDATE a robot's state.");
//     // check if request arg is correct
//     if( !request->hasArg("robot_id") ) {
//         request->send(400, "text/plain", "400: Invalid Request, arg must be robot_id");
//         return;
//     }

//     // for list of robots connected, check if robot id matches any connected robots
//     for(int i = 0; i < numConnectedRobots; i++) {
//         if( request->arg("robot_id").equals(String(connectedRobots[i].robotID)) ) {
//             Serial.println("Robot " + request->arg("robot_id") + " state changed to " + dancebotStates[state] + ".");
//             connectedRobots[i].robotState = state;
//             request->send(200, "text/html", String(connectedRobots[i].robotState));
//             return;
//         }
//     }
//     request->send(404, "text/plain", "404: Robot ID Not Found");
// }
/**
 * handle_NotFound is For 404 redirect requests.
 */
// void handle_notFound(AsyncWebServerRequest *request) {
//     request->send(404, "text/plain", "404: Not found");
// }

/* ---------------------WEB PAGE------------------------------------ */
/**
 * sendHTML sends a set of HTML to the user in response to a POST request based on the new WebController state.
 * TODO: currently no support for multiple robots.
 */
// String sendHTML() {
    // String  button_css ="width:100%; margin-bottom:1em; padding: 1em; font-family:'Arial';font-size:medium;color:#1d1f21; background-color:#8abeb7;border-color:#5e8d87;";
    // String  body_css =  "width:auto; font-family:'Arial'; background-color:#1d1f21; color:#c5c8c6;";
    // String  head =  "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /></head>";
    // String  body =  "<body style=\"" + body_css + "\">" +
    //     /*HEADER*/      "<div id=\"page_header\" style=\"margin: 0 5% 2em 5%; color:#cc6666;\">" +
    //                         "<h1>Demobots Dancing Robot</h1>" +
    //                     "</div>" +
    //     /*MOVES */      "<div id=\"page_dances\" style=\"margin: 0 5% 2em 5%;\">" +
    //                         "<h3 style=\"color:#81a2be;\">Dance Moves</h3>" +
    //                         "<div id=\"dance_moves\" style=\"\">" +             
    //                             "<div style=\"padding-left: 1.5em; font-size:medium;\">" +
    //                                 "<p id=\"current_move\">Current Move: Reset</p>" +
    //                             "</div>" +
    //                             "<div id=\"dance_move_buttons\" style=\"padding-left: 1.5em; \">";
    //                                 for (int i = Reset; i < Demo1; i++) {
    //                                     body += "<button onclick=\"postMove('" + String(dancebotStates[i]) + "')\" style=\"" + button_css + "\">" + dancebotStates[i] + "</button>";
    //                                 }
    //         body +=             "</div>" +
    //                         String("</div>") +
    //                     "</div>" +

    //     /*ROUTINES*/    "<div id=\"page_routines\" style=\"margin: 0 5% 2em 5%;\">" +
    //                         "<h3 style=\"color:#81a2be;\">Dances</h3>" +
    //                         "<div id=\"dance_routines\" style=\"\">" +             
    //                             "<div style=\"padding-left: 1.5em; font-size:medium;\">" +
    //                                 "<p id=\"current_routine\">Current Dance: None</p>" +
    //                             "</div>" +
    //                             "<div id=\"dance_routine_buttons\" style=\"padding-left: 1.5em; \">";
    //                                 for (int i = Demo1; i < NumStates; i++) {
    //                                     body += "<button onclick=\"postMove('" + String(dancebotStates[i]) + "')\" style=\"" + button_css + "\">" + String(dancebotStates[i]) + "</button>";
    //                                 }
    //         body +=             "</div>" +
    //                         String("</div>") + 
    //                     "</div>" +
    //                     sendJavascript() +
    //                 "</body>";
    // head += body;
    // return head;
// }
/**
 * sendJavascript sends a set of JS to the user in response to a POST request based on the new WebController state.
 * TODO: currently no support for multiple robots.
 */
// String sendJavascript() {
//   String s = String("<script>") +
//     // update the current state in the HTML
//     "function updateCurrentMove(move) {" +
//         "document.getElementById('current_move').innerText = 'Current Move: ' + move;" +
//         "document.getElementById('current_routine').innerText = 'Current Dance: ' + move;" +
//     "}" +

//     //function to HTTP Post
//     "async function postMove(move) {" +
//         "let response = await fetch('/' + move, {" +
//             "method: 'POST'," +
//             "headers: {" +
//                 "'Content-Type': 'application/x-www-form-urlencoded;charset=UTF-8'" +
//             "}," +
//             "body: \"robot_id=0\"" +
//         "}).then(function(response) {" +
//             "console.log(response.status);" +
//             "if(response.status == 200) {" + 
//                 "updateCurrentMove(move);" +    
//             "}" +
//             "console.log(response.text());" +
//         "});" +
//     "}" +
//   "</script>";
//   return s;
// }