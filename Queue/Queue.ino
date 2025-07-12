#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include "DisneylandLogo_Small.h"
#include "DCABW.h"
#include "USH.h"
#include "KBF.h"
#include "LegoLandCali.h"
#include "DW.h"
#include "TDL.h"

#define BUTTON_NEXT 0     // Adjust if needed for your board revision
#define BUTTON_SELECT 14

const char* ssid = "";
const char* password = "";

TFT_eSPI tft = TFT_eSPI();

String allParks[] = { "Tokyo Disneyland", "Legoland", "Dollywood", "Disneyland", "DCA", "Universal Hollywood", "Knotts", "Animal Kingdom", "Hollywood Studios", "Magic Kingdom", "Epcot" };
String selectedIDs[] = { "274", "279", "55", "16", "17", "66", "61", "8", "7", "6", "5" };
bool parkSelected[10] = { false };
int currentIndex = 0;

void initButtons() {
  pinMode(BUTTON_NEXT, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);
}

void blinkSelectFeedback() {
  for (int i = 0; i < 2; i++) {
    tft.invertDisplay(true);
    delay(100);
    tft.invertDisplay(false);
    delay(100);
  }
}

void showMenu() {
  tft.fillScreen(TFT_BLACK);
  for (int i = 0; i < 3; i++) {
    int idx = (currentIndex + i) % 10;
    int yPos = 30 + i * 20;

    if (i == 0) {
      tft.fillRect(5, yPos - 2, tft.width() - 10, 18, TFT_DARKGREY);
    }

    uint16_t textColor = parkSelected[idx] ? TFT_GREEN : TFT_WHITE;
    tft.setTextColor(textColor, TFT_BLACK);
    tft.drawString(allParks[idx], 10, yPos, 2);

    if (parkSelected[idx]) {
      tft.drawString("*", tft.width() - 20, yPos, 2);
    }
  }
}

void parkSelectionMenu() {
  unsigned long pressStartTime = 0;
  const unsigned long longPressDuration = 1000; // milliseconds
  bool selectionDone = false;

  showMenu();
  while (!selectionDone) {
    if (digitalRead(BUTTON_NEXT) == LOW) {
      currentIndex = (currentIndex + 1) % 10;
      showMenu();
      delay(200);
    }
    if (digitalRead(BUTTON_SELECT) == LOW) {
      parkSelected[currentIndex] = !parkSelected[currentIndex];
      // blinkSelectFeedback();
      showMenu();
      delay(200);
    }
    if (digitalRead(BUTTON_NEXT) == LOW) {
      if (pressStartTime == 0) {
        pressStartTime = millis(); // Start timing
      } else if (millis() - pressStartTime >= longPressDuration) {
        selectionDone = true;
        // blinkSelectFeedback(); // Optional feedback
        tft.fillScreen(TFT_BLACK);
      }
    } else {
      pressStartTime = 0; // Reset if button released early
    }
  }
  delay(500);
}

/*
  Helper function to display ride information.
  Depending on the park (using IDNum) a background image is selected.
  It also checks for blank wait time or closed status.
*/
void displayRideInfo(String IDNum, const char* landName, JsonObject ride) {
  int fontColor = TFT_YELLOW;
  int waitTimeColor = TFT_WHITE;
  int parkId = IDNum.toInt();


  // Select background image and colors based on park id.
  switch (parkId) {
    case 16:
      tft.fillScreen(TFT_BLACK);
      tft.pushImage(0, 0, 302, 170, DisneylandLogo_Small);
      fontColor = TFT_YELLOW;
      waitTimeColor = TFT_WHITE;
      break;
    case 55:
      tft.fillScreen(TFT_BLACK);
      tft.pushImage(0, 20, 320, 113, DW);
      fontColor = TFT_YELLOW;
      waitTimeColor = TFT_CYAN;
      break;
    case 17:
      tft.fillScreen(TFT_BLACK);
      tft.pushImage(0, 15, 320, 165, DCABW);
      fontColor = TFT_YELLOW;
      waitTimeColor = TFT_CYAN;
      break;
    case 66:
      tft.fillScreen(TFT_WHITE);
      tft.pushImage(0, 0, 320, 151, USH);
      fontColor = TFT_YELLOW;
      waitTimeColor = TFT_WHITE;
      break;
    case 61:
      tft.fillScreen(TFT_BLACK);
      tft.pushImage(75, 0, 170, 170, KBF);
      fontColor = TFT_YELLOW;
      waitTimeColor = TFT_WHITE;
      break;
    case 279:
      tft.fillScreen(TFT_WHITE);
      tft.pushImage(0, 20, 320, 117, LegoLandCali);
      fontColor = TFT_YELLOW;
      waitTimeColor = TFT_CYAN;
      break;
    case 274:
      tft.fillScreen(TFT_BLACK);
      tft.pushImage(0, 20, 320, 115, TDL);
      fontColor = TFT_YELLOW;
      waitTimeColor = TFT_WHITE;
      break;
    default:
      tft.fillScreen(TFT_BLACK);
      fontColor = TFT_YELLOW;
      waitTimeColor = TFT_CYAN;
      break;
  }

  tft.setTextDatum(MC_DATUM);
  // Draw the land/area name (or a default "Park" if not available)
  drawOutlinedText(landName, tft.width() / 2, 15, TFT_GREEN, 2);

  // Print ride name (wrapped into chunks)
  printChunksByWord(ride["name"].as<const char*>(), 25, fontColor);

  // Check whether the ride is open. Also, if the "is_open" field is blank we treat it as closed.
  //const char* isOpenStr = ride["is_open"].as<const char*>();
  String isOpenStr = ride["is_open"].as<String>();
 // drawOutlinedText(isOpenStr, tft.width() / 2, 130, TFT_RED, 4);
 // if (isOpenStr == nullptr || strcmp(isOpenStr, "") == 0 || strcmp(isOpenStr, "false") == 0) 
    if (ride["is_open"].as<String>() == "false")
    {
    drawOutlinedText("Closed", tft.width() / 2, 130, TFT_RED, 4);
    //drawOutlinedText(isOpenStr, tft.width() / 2, 130, TFT_RED, 4);
  }
  else {
    // Check wait_time for blank
    drawOutlinedText(ride["wait_time"].as<String>(), tft.width() / 2, 130, waitTimeColor, 6);
    drawOutlinedText("minute wait time", tft.width() / 2, 155, waitTimeColor, 2);
  }
  delay(5000);
}

/*
  Modified function to accept both JSON data formats.
  The code first deserializes the wait times JSON,
  then checks if the document contains a non‑empty "lands" array.
  If not, it checks for a "rides" array directly.
*/
void fetchQueueTimes(String IDNum) {
  HTTPClient http;
  http.begin("https://queue-times.com/parks.json");
  int httpCode = http.GET();
  if (httpCode == 200) {
    String payload = http.getString();
    DynamicJsonDocument doc(8192);
    deserializeJson(doc, payload);
    String waitUrl = "https://queue-times.com/parks/" + IDNum + "/queue_times.json";
    HTTPClient waitHttp;
    waitHttp.begin(waitUrl);
    int waitCode = waitHttp.GET();
    if (waitCode == 200) {
      String waitPayload = waitHttp.getString();
      DynamicJsonDocument waitDoc(8192);
      DeserializationError err = deserializeJson(waitDoc, waitPayload);
      if (err) {
        tft.fillScreen(TFT_BLACK);
        drawOutlinedText("JSON Error", tft.width()/2, 15, TFT_RED, 4);
        delay(5000);
        return;
      }
      // Check if the JSON contains a valid "lands" array.
      if (waitDoc.containsKey("lands") && waitDoc["lands"].is<JsonArray>() && waitDoc["lands"].size() > 0) {
        for (JsonObject land : waitDoc["lands"].as<JsonArray>()) {
          if (land.containsKey("rides") && land["rides"].is<JsonArray>() && land["rides"].size() > 0) {
            // Use the land name from the data
            for (JsonObject ride : land["rides"].as<JsonArray>()) {
              displayRideInfo(IDNum, land["name"].as<const char*>(), ride);
            }
          }
        }
      }
      // If there's no "lands" array, check for a flat "rides" array.
      else if (waitDoc.containsKey("rides") && waitDoc["rides"].is<JsonArray>() && waitDoc["rides"].size() > 0) {
        for (JsonObject ride : waitDoc["rides"].as<JsonArray>()) {
          displayRideInfo(IDNum, "Ride", ride); // Default land name value.
        }
      }
      else {
        tft.fillScreen(TFT_BLACK);
        drawOutlinedText("No ride data", tft.width()/2, 15, TFT_RED, 4);
        delay(5000);
      }
    }
    waitHttp.end();
  } else {
    tft.setTextDatum(MC_DATUM);
    tft.fillScreen(TFT_BLACK);
    drawOutlinedText("Web Connection Error. Reboot if connection fails again.", tft.width() / 2, 15, TFT_RED, 4);
    delay(10000);
  }
  http.end();
}

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);

  tft.init();
/* 
  // Static WiFi setup
  WiFi.begin(ssid, password);
  tft.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    tft.print(".");
  }
  tft.println("\nConnected! ");
   */
 
  WiFi.mode(WIFI_AP_STA);
  WiFi.beginSmartConfig();
  tft.println("Waiting for SmartConfig...");
  while (!WiFi.smartConfigDone()) {
    delay(500);
    tft.print(".");
  }
  tft.println("\nSmartConfig received.");
  tft.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    tft.print(".");
  }
  tft.println("\nWiFi connected!");
  tft.print("IP Address: ");
  tft.println(WiFi.localIP());
  delay(2000);  
 
  initButtons();
  parkSelectionMenu();
}

void loop() {
  tft.setSwapBytes(true);
  for (int i = 0; i < 10; i++) {
    if (parkSelected[i]) {
      fetchQueueTimes(selectedIDs[i]);
      delay(5000);
    }
  }
}

// Utility function to draw outlined text.
void drawOutlinedText(String text, int x, int y, int fontColor, int font) {
  tft.setTextColor(TFT_BLACK);
  for (int dx = -3; dx <= 3; dx++) {
    for (int dy = -3; dy <= 3; dy++) {
      if (dx != 0 || dy != 0) {
        tft.drawString(text, x + dx, y + dy, font);
      }
    }
  }
  tft.setTextColor(fontColor);
  tft.drawString(text, x, y, font);
}

// Utility function to print text broken into chunks at word breaks.
void printChunksByWord(String text, int maxLength, int fontColor) {
  int start = 0;
  int Ystart = 50;
  while (start < text.length()) {
    int end = start + maxLength;
    if (end >= text.length()) {
      drawOutlinedText(text.substring(start), tft.width() / 2 , Ystart , fontColor, 4);
      break;
    }
    int spaceIndex = text.lastIndexOf(' ', end);
    if (spaceIndex <= start) spaceIndex = end;
    String chunk = text.substring(start, spaceIndex);
    drawOutlinedText(chunk, tft.width() / 2 , Ystart , fontColor, 4);
    Ystart += 25;
    start = (text.charAt(spaceIndex) == ' ') ? spaceIndex + 1 : spaceIndex;
  }
}