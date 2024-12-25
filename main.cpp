/***************************************************************
 *
 * 
 * Hardware:
 *  - Wemos D1 mini (ESP8266)
 *  - NeoPixel strip with at least 20 LEDs
 *  - Data pin = D1
 * 
 * Behavior:
 *  - Creates an Access Point "MyLampAP" (pwd "12345678").
 *  - Web server on port 80, minimal HTML form to pick lamp 0..8.
 *  - If user picks 0 => all off.
 *  - If user picks 1..8 => 
 *      1) Turn lamp 0 on
 *      2) Wait "X seconds" (where X is user’s choice)
 *      3) Turn lamp X on
 *      4) Then turn on lamps descending from (X-1) down to 1, 
 *         with 1-second waits in between
 *  - All turned-on lamps flicker with a random red/orange "fire" effect
 *    until a new user selection.
 ***************************************************************/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>

// --------------------------
// USER CONFIGURATION
// --------------------------
#define LED_PIN         D1               // NeoPixel data pin
#define LED_COUNT       20               // Total number of LEDs on the strip
#define WIFI_SSID       "Hanukia"       // AP SSID
#define WIFI_PASSWORD   ""       // AP Password
#define FIRE_UPDATE_MS  50               // Fire flicker update interval (ms) meaning the speed of the fire effect

// --------------------------
// GLOBAL OBJECTS & VARIABLES
// --------------------------
ESP8266WebServer server(80);
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// The user’s selection from the HTML form
volatile int userSelection = 0;

// We'll keep track of which lamps are ON or OFF
bool lampIsOn[9] = { false };

// --------------------------
// LAMP -> LED MAPPING
// --------------------------
int lamp0[] = {8, 9, 10};
int lamp1[] = {0, 1};
int lamp2[] = {2, 3};
int lamp3[] = {4, 5};
int lamp4[] = {6, 7};
int lamp5[] = {11, 12};
int lamp6[] = {13, 14};
int lamp7[] = {15, 16};
int lamp8[] = {17, 18};

struct LampMapping {
  int *ledArray;  // pointer to array of LED indices
  int size;       // how many LEDs in that lamp
};

LampMapping lampMap[] = {
  { lamp0, sizeof(lamp0)/sizeof(lamp0[0]) },  // lamp 0
  { lamp1, sizeof(lamp1)/sizeof(lamp1[0]) },  // lamp 1
  { lamp2, sizeof(lamp2)/sizeof(lamp2[0]) },  // lamp 2
  { lamp3, sizeof(lamp3)/sizeof(lamp3[0]) },  // lamp 3
  { lamp4, sizeof(lamp4)/sizeof(lamp4[0]) },  // lamp 4
  { lamp5, sizeof(lamp5)/sizeof(lamp5[0]) },  // lamp 5
  { lamp6, sizeof(lamp6)/sizeof(lamp6[0]) },  // lamp 6
  { lamp7, sizeof(lamp7)/sizeof(lamp7[0]) },  // lamp 7
  { lamp8, sizeof(lamp8)/sizeof(lamp8[0]) }   // lamp 8
};

void handleRoot() {
  // This version uses English for everything:
  // - Title, headings, dropdown options, button label
  // - All HTML/CSS is in English
  String html = R"====(
    <html>
    <head>
      <title>Hanukkah Lamp Selector</title>
      <style>
        body {
          margin: 0;
          height: 100vh;
          display: flex;
          justify-content: center;
          align-items: center;
          background-color: #78FF78; /* Green background */
          font-family: Arial, sans-serif;
        }
        .container {
          text-align: center;
          background-color: #ffffff;
          padding: 40px;
          border-radius: 15px;
          box-shadow: 0 0 20px rgba(0,0,0,0.2);
        }
        h1 {
          margin-bottom: 20px;
          color: #006600;
        }
        .big-select {
          font-size: 150%;
          padding: 10px;
          margin-bottom: 20px;
          border-radius: 5px;
        }
        .big-button {
          font-size: 400%;
          padding: 20px 40px;
          cursor: pointer;
          border-radius: 8px;
          background-color: #00cc44;
          color: #ffffff;
          border: none;
        }
        .big-button:hover {
          background-color: #009933;
        }
      </style>
    </head>
    <body>
      <div class="container">
        <h1>Choose Day (0..8)</h1>
        <form action="/set" method="GET">
          <!-- Dropdown with English labels -->
          <select name="lamp" class="big-select">
            <option value="0">Off</option>
            <option value="1">Day 1</option>
            <option value="2">Day 2</option>
            <option value="3">Day 3</option>
            <option value="4">Day 4</option>
            <option value="5">Day 5</option>
            <option value="6">Day 6</option>
            <option value="7">Day 7</option>
            <option value="8">Day 8</option>
          </select>
          <br><br>
          <input type="submit" value="Confirm" class="big-button">
        </form>
      </div>
    </body>
    </html>
  )====";

  server.send(200, "text/html", html);
}



// // Web server root: simple HTML form
// void handleRoot() {
//   // Inline CSS for centering and enlarging the button
//   // The important changes:
//   //  - Using <select> instead of <input type="number">
//   //  - Font-size and layout centered via flexbox
//   String html = R"====(
//     <html>
//     <head>
//       <title>Lamp Selector</title>
//       <style>
//         body {
//           margin: 0;
//           height: 100vh;
//           display: flex;
//           justify-content: center;
//           align-items: center;
//           background-color: #f0f0f0;
//         }
//         .container {
//           text-align: center;
//         }
//         .big-button {
//           font-size: 400%;
//           padding: 20px 40px;
//         }
//         .big-select {
//           font-size: 200%;
//           padding: 10px;
//         }
//       </style>
//     </head>
//     <body>
//       <div class="container">
//         <h1>Select a lamp (0..8)</h1>
//         <form action="/set" method="GET">
//           <!-- Dropdown instead of text input -->
//           <select name="lamp" class="big-select">
//             <option value="0">0</option>
//             <option value="1">1</option>
//             <option value="2">2</option>
//             <option value="3">3</option>
//             <option value="4">4</option>
//             <option value="5">5</option>
//             <option value="6">6</option>
//             <option value="7">7</option>
//             <option value="8">8</option>
//           </select>
//           <br><br>
//           <input type="submit" value="Submit" class="big-button">
//         </form>
//       </div>
//     </body>
//     </html>
//   )====";

//   server.send(200, "text/html", html);
// }


// Web server "set": read the "lamp" parameter
void handleSet() {
  if(server.hasArg("lamp")) {
    userSelection = server.arg("lamp").toInt();
  }
  // Redirect back to root
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

// Turn ALL lamps off
void turnOffAll() {
  for(int i=0; i<9; i++) {
    lampIsOn[i] = false;
  }
  // Clear the strip’s pixels
  for(int i=0; i<LED_COUNT; i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }
  strip.show();
}

// Mark a lamp as ON
void turnLampOn(int lampNumber) {
  if(lampNumber < 0 || lampNumber > 8) return;
  lampIsOn[lampNumber] = true;
}

// Fire effect: For each lamp that’s ON, flicker its LEDs 
// with random red/orange values.
void updateFireEffect() {
  // For each lamp
  for(int lampNumber = 0; lampNumber < 9; lampNumber++) {
    if(!lampIsOn[lampNumber]) continue;  // skip if lamp is off

    // Flicker each LED in this lamp
    for(int i = 0; i < lampMap[lampNumber].size; i++) {
      int ledIndex = lampMap[lampNumber].ledArray[i];

      // random fire color
      int r = random(75,85);  //
      int g = random(15, 30);    // 
      int b = 0;                 // keep at 0 for a more "fire" look

      strip.setPixelColor(ledIndex, r, g, b);
    }
  }
  strip.show();
}

// Wait in small increments so the fire can animate
// and the web server remains responsive.
void waitInSlices(unsigned long totalMs) {
  unsigned long start = millis();
  while((millis() - start) < totalMs) {
    server.handleClient();   // let web server handle incoming requests
    updateFireEffect();      // flicker effect during wait
    delay(FIRE_UPDATE_MS);   // short delay
  }
}

void setup() {
  Serial.begin(9600);

  // Start Wi-Fi Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  // Web server setup
  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();

  // Initialize NeoPixel
  strip.begin();
  strip.setBrightness(255);
  strip.show();

  // Turn all lamps off initially
  turnOffAll();
}

void loop() {
  server.handleClient();

  // Check if user made a new selection
  static int oldSelection = -1;
  if(oldSelection != userSelection) {
    oldSelection = userSelection;

    // Turn everything off first
    turnOffAll();

    if(userSelection == 0) {
      // If "0", keep everything off
    } 
    else {
      // If user picks 1..8:
      // 1) Turn on lamp 0
      turnLampOn(0);

      // 2) Wait "userSelection" seconds
      waitInSlices(userSelection * 1000UL);

      // 3) Turn on chosen lamp
      turnLampOn(userSelection);

      // 4) Descend from (userSelection - 1) down to 1,
      //    with a 1-second wait each time
      for(int L = userSelection - 1; L >= 1; L--) {
        waitInSlices(1000UL);
        turnLampOn(L);
      }
    }
  }

  // Always keep updating fire effect
  updateFireEffect();
  delay(FIRE_UPDATE_MS);
}





















