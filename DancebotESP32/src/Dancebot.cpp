/**
 * @file Dancebot.cpp
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Controls our Dancebot.
 * @version 0.1.0
 * @date 2023-04-20
 * @copyright Copyright (c) 2023
 *
 */
#include "Dancebot.hpp"
#include <ArduinoLog.h>

/**
 * @brief Data structure used by a server hosted by the Dancebot to manage other
 * statuses.
 *
 */
typedef struct DancebotEntry {
    String id;
    enum Dancebot::DancebotMovement movement;
    enum Dancebot::DancebotExpression expression;
    enum Dancebot::DancebotEyeColor eye_color;
    float soc;
    bool is_active;
    DancebotEntry *next_entry;
} DancebotEntry, *DancebotEntryPtr;

static DancebotEntryPtr _db;

static void serve_root(AsyncWebServerRequest *request) {
    Log.trace("[serve_root] The server got a client trying to access '/'!");
    request->send(200, "text/plain", "Hello, world");
}

static void serve_join(AsyncWebServerRequest *request) {
    Log.trace("[serve_join] Someone is attempting to join the server!");

    int num_params = request->params();
    if (num_params == 4) {

        // TODO: parse back into enum
        AsyncWebParameter *id = request->getParam("id", true);
        // AsyncWebParameter *move = request->getParam("move");
        // AsyncWebParameter *es = request->getParam("es");
        // AsyncWebParameter *ec = request->getParam("ec");

        // TODO: check if ID already exists
        DancebotEntryPtr entry = new DancebotEntry();
        entry->id = id->value();
        entry->movement = Dancebot::DancebotMovement::RESET;
        entry->expression = Dancebot::DancebotExpression::NONE;
        entry->eye_color = Dancebot::DancebotEyeColor::WHITE;
        entry->soc = 0.0;
        entry->is_active = true;
        entry->next_entry = NULL;

        if (_db == NULL) {
            _db = entry;
        } else {
            DancebotEntryPtr next = _db->next_entry;
            while (next != NULL) {
                next = next->next_entry;
            }
            next = entry;
        }
        Log.trace("Dancebot %s Joins the Server!", entry->id);
        request->send(200, "text/plain", "Successfully joined the server.");
        return;
    }

    // request->send(409, "text/plain", "A robot already exists with this ID!");
    request->send(400, "text/plain", "The payload is malformed!");
}

static void serve_update(AsyncWebServerRequest *request) {
    Log.trace("[serve_update] The server got a client trying to access '/update'!");
    request->send(200, "text/plain", "Hello, world");
}


#define NUM_ROUTES 3
static const struct DemobotServer::Route _routes[NUM_ROUTES] = {
    {"/", WebRequestMethod::HTTP_GET, serve_root},
    {"/join", WebRequestMethod::HTTP_POST, serve_join},
    {"/update", WebRequestMethod::HTTP_POST, serve_update}
};

Dancebot::Dancebot(const char *name) {
    Log.trace("[Dancebot]");

    _name = const_cast<char*>(name);
    Log.trace("\tHello robot %s!", _name);

    _id = DemobotID::DANCEBOT;

    _movement = DancebotMovement::RESET;
    _expression = DancebotExpression::NONE;
    _eye_color = DancebotEyeColor::WHITE;
    _soc = 0.0; // TODO: no hw support for measuring battery

    _db = NULL;

    /** Connect to demobots server. */
    enum DemobotServer::StaticIPMode mode;
    if (!setup_network(&mode)) {
        Log.trace("\tSwitching to autonomous mode.");
        // TODO: switch to autonomous mode.
    }

    if (!setup_client_server(mode)) {
        Log.trace("\tSwitching to autonomous mode.");
        // TODO: switch to autonomous mode.
    }

    if (!join_server()) {
        Log.trace("\tSwitching to autonomous mode.");
    }

    if (_db != NULL) {
        Log.trace("Robot in db: %s", _db->id);
    }
}

Dancebot::Dancebot(
    const char *name,
    DancebotMovement move,
    DancebotExpression expression,
    DancebotEyeColor eye_color
) : Dancebot(name) {
    _movement = move;
    _expression = expression;
    _eye_color = eye_color;
}

Dancebot::~Dancebot() {
    if (_network != NULL) {
        free(_network);
    }

    if (_server != NULL) {
        free(_server);
    }

    if (_client != NULL) {
        free(_client);
    }

    if (_db != NULL) {
        DancebotEntryPtr curr = _db;
        DancebotEntryPtr next = _db->next_entry;
        do {
            /** Free current, get next. */
            free(curr);
            curr = next;
            next = next->next_entry;
        } while (curr != NULL);
    }
}

bool Dancebot::setup_network(enum DemobotServer::StaticIPMode *mode){
    Log.trace("[setup_network]");

    Log.trace("\tConnecting to a network.");
    _network = new DemobotNetwork(_name);
    bool success = _network->connect_network();
    if (!success) {
        Log.trace("\tNo matching networks was found. Setting up our own network.");
        success = _network->setup_network();
        if (!success) {
            Log.trace("\tNo networks were established. Defaulting to autonomous mode.");
            return false;
        } else {
            *mode = DemobotServer::MODE_AP;
        }
    } else {
        *mode = DemobotServer::MODE_STA;
    }
    Log.trace("\tA network connection has been established!");
    return true;
}

bool Dancebot::setup_client_server(enum DemobotServer::StaticIPMode mode) {
    Log.trace("[setup_client_server]");

    /** Setup client. */
    Log.trace("\tSetting up our client.");
    _client = new DemobotClient(get_robot_ip(_id));

    /** Connect to server. */
    Log.trace("\tConnecting to a server.");
    _server = new DemobotServer(get_robot_ip(_id));
    if (!_client->ping_server()) {
        Log.trace("\tWe did not find a server online. Setting up our own server.");
        if (!_server->setup_server(_routes, NUM_ROUTES, mode)) {
            Log.notice("\tUnable to setup our own server. Defaulting to autonomous mode.");
            return false;
        } else {
            Log.trace("\tStarting server.");
            _server->start_server();
        }
    }
    if (!_client->ping_server()) {
        Log.trace("\tUnable to communicate to our own server. Defaulting to autonomous mode.");
        return false;
    }

    /** Setup db for server. */
    _db = NULL;

    Log.trace("\tA server connection is available!");
    return true;
}

bool Dancebot::join_server(void) {
    Log.trace("[join_server]");

    /** Ping server. */
    if (!_client->ping_server()) {
        Log.trace("\tUnable to communicate to our own server.");
        return false;
    }

    /** Send join request with robot metadata to server. */
    const char *keys[4] = {"id", "move", "es", "ec"};
    const char *vals[4] = {
        _name,
        move2string(_movement),
        expression2string(_expression),
        eyeColor2string(_eye_color)
    };
    String response;
    bool success = _client->send_HTTP_request(
        "/join",
        WebRequestMethod::HTTP_POST,
        keys,
        vals,
        4,
        &response
    );

    Log.trace("\tGot response %s.", response.c_str());
    return success;
}

// api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14

const char* Dancebot::move2string(DancebotMovement state) {
    switch (state) {
        case RESET:
            return "reset";
        default:
            return "reset";
    }
}

const char* Dancebot::expression2string(DancebotExpression expression) {
    switch (expression) {
        case NONE:
            return "NONE";
        default:
            return "NONE";
    }
}

const char* Dancebot::eyeColor2string(DancebotEyeColor eye_color) {
    switch (eye_color) {
        case WHITE:
            return "WHITE";
        default:
            return "WHITE";
    }
}

