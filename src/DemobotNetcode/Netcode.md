# Netcode Structure

The Demobot communication code can be broken down into three objects:

- DemobotNetwork - Configures, sets up, and connects to a given network and IP
  address based on the robot.
  - Uses IPAddress and WiFi libraries.
- Server - Configures and sets up a server at a specified port, given that the
  network and IP address are already set up.
  - Uses the AsyncWebServer library.
- Client - Provides robot functions for sending HTTP requests to a server
  (whether the server is hosted by the robot or from another device).
  - Uses the asyncHTTPRequest library.
