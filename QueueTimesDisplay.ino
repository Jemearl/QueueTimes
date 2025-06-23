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

//const char* ssid = "";
//const char* password = "";


TFT_eSPI tft = TFT_eSPI();


void setup() {

  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);

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


  // Set WiFi to station mode and start SmartConfig
  WiFi.mode(WIFI_AP_STA);
  WiFi.beginSmartConfig();

  tft.println("Waiting for SmartConfig...");

  // Wait for SmartConfig to complete
  while (!WiFi.smartConfigDone()) {
    delay(500);
    tft.print(".");
  }

  tft.println("\nSmartConfig received.");

  // Wait for WiFi to connect
  tft.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    tft.print(".");
  }

  tft.println("\nWiFi connected!");
  tft.print("IP Address: ");
  tft.println(WiFi.localIP());

  delay(2000);
}

// 279 - Legoland California
// 55  - DollyWood
// 16  - Disneyland
// 17  - Disney California Adventure
// 66  - Universal Hollywood
// 61  - Knott's Berry Farm
// 8   - Animal Kingdom
// 7   - Disney Hollywood Studios
// 6   - Disney Magic Kingdom
// 5   - Epcot
// 64  - Islands Of Adventure At Universal Orlando
// 67  - Universal Volcano Bay
// 334 - Epic Universe
// 65  - Universal Studios At Universal Orlando

void loop() {

  // Define IDs and their corresponding delays (in ms)
 // String parkIDs[] = {"279", "55", "16", "17", "66", "61" };
  String parkIDs[] = { "5", "6", "7", "8" };
  int numParks = sizeof(parkIDs) / sizeof(parkIDs[0]);
  tft.setSwapBytes(true);

  for (int i = 0; i < numParks; i++) {
    fetchQueueTimes(parkIDs[i]);
    Serial.println(parkIDs[i]);
    //delay(delays[i]);
    delay(5000);
  }

}

void drawOutlinedText(String text, int x, int y, int fontColor, int font ) {
  tft.setTextColor(TFT_BLACK);           // Font Outline Color
  for (int dx = -3; dx <= 3; dx++) {     // dx and dy determine how many pixels outside of the center to draw the black outline
  for (int dy = -3; dy <= 3; dy++) {     // In this case, we're drawing a 3 pixel outline
  if (dx != 0 || dy != 0) {
    tft.drawString(text, x + dx, y + dy, font);
      }
    }
  }
  tft.setTextColor(fontColor);           // set the desired font color
  tft.drawString(text, x, y, font);      // draw text at the center of the black outline
}


void printChunksByWord(String text, int maxLength, int fontColor) {
  int start = 0;
  int Ystart = 55;
 
  while (start < text.length()) {
    int end = start + maxLength;

    if (end >= text.length()) {
      drawOutlinedText(text.substring(start), tft.width() / 2 , Ystart , fontColor, 4 );
      break;
    }

    int spaceIndex = text.lastIndexOf(' ', end);

    if (spaceIndex <= start) {
      // No space found, force split at max length
      spaceIndex = end;
    }

    String chunk = text.substring(start, spaceIndex);
    drawOutlinedText(chunk, tft.width() / 2 , Ystart , fontColor, 4 );
    Ystart = Ystart + 25;
    
    // Skip the space (if one was found)
    start = (text.charAt(spaceIndex) == ' ') ? spaceIndex + 1 : spaceIndex;
  } 
}



void fetchQueueTimes(String IDNum) {
  HTTPClient http;
  http.begin("https://queue-times.com/parks.json");
  int httpCode = http.GET();

  if (httpCode == 200) {
    String payload = http.getString();
    DynamicJsonDocument doc(8192);
    deserializeJson(doc, payload);
      JsonArray parks = doc["parks"];
  
      String waitUrl = "https://queue-times.com/parks/" + IDNum +  "/queue_times.json"; // + String(id) + "/queue_times.json"; 
      HTTPClient waitHttp;
      waitHttp.begin(waitUrl);
      int waitCode = waitHttp.GET();
      if (waitCode == 200) {
        String waitPayload = waitHttp.getString();
        DynamicJsonDocument waitDoc(8192);
        deserializeJson(waitDoc, waitPayload);
  
        if (waitDoc.containsKey("lands")) {
          for (JsonObject land : waitDoc["lands"].as<JsonArray>()) {
            int fontColor = TFT_YELLOW;
            int waitTimeColor = TFT_WHITE;
            for (JsonObject ride : land["rides"].as<JsonArray>()) {
                switch (IDNum.toInt()) {
                  case 16: // Disneyland
                    tft.fillScreen(TFT_BLACK);
                    fontColor = TFT_YELLOW;
                    waitTimeColor = TFT_WHITE;
                    tft.pushImage(0,0,302,170,DisneylandLogo_Small);  
                    break;
                  case 55: // DollyWood
                    tft.fillScreen(TFT_BLACK);
                    fontColor = TFT_YELLOW;
                    waitTimeColor = TFT_CYAN;
                    tft.pushImage(0,20,320,113,DW);  
                    break;                    
                  case 17:  // Disney California Adventure
                    tft.fillScreen(TFT_BLACK);
                    fontColor = TFT_YELLOW;
                    waitTimeColor = TFT_CYAN;
                    tft.pushImage(0,15,320,165,DCABW); 
                  break;
                  case 66: // Universal Hollywood
                    tft.fillScreen(TFT_WHITE);
                    fontColor = TFT_YELLOW;
                    waitTimeColor = TFT_WHITE;
                    tft.pushImage(0,0,320,151,USH);
                    break;
                  case 61: // Knott's Berry Farm
                    tft.fillScreen(TFT_BLACK);
                    fontColor = TFT_YELLOW;
                    waitTimeColor = TFT_WHITE;
                    tft.pushImage(75,0,170,170,KBF); 
                    break;
                  case 279: // Legoland California
                    tft.fillScreen(TFT_WHITE);
                    fontColor = TFT_YELLOW;
                    waitTimeColor = TFT_CYAN;
                    tft.pushImage(0,20,320,117,LegoLandCali); 
                    break;
                  default:
                    tft.fillScreen(TFT_BLACK);
                    fontColor = TFT_YELLOW;
                    waitTimeColor = TFT_CYAN;
                   // tft.pushImage(0,30,320,165,DCABW); 
                    break;
                }
                tft.setTextDatum(MC_DATUM);
                drawOutlinedText(land["name"].as<const char*>(), tft.width() / 2 , 15, TFT_GREEN, 2 );
                printChunksByWord(ride["name"].as<const char*>(), 25, fontColor);
                if (ride["is_open"].as<String>() == "false")
                {
                  drawOutlinedText("Closed", tft.width() / 2 , 130, TFT_RED, 4 );
                }
                else
                {
                  drawOutlinedText(ride["wait_time"].as<String>(), tft.width() / 2 , 130, waitTimeColor, 6 );
                  drawOutlinedText("minute wait time", tft.width() / 2, 155, waitTimeColor, 2 );
                }
                //String testString = ride["name"].as<const char*>();
               tft.drawString(String(strlen(ride["name"].as<const char*>())) , tft.width() / 2 , 175, 2);
              // tft.drawString(strlenth(ride["is_open"].as<string>()) , 10 , 90 , 2 );
              delay(5000);
            }
          }
        } else if (waitDoc.containsKey("rides")) {
          for (JsonObject ride : waitDoc["rides"].as<JsonArray>()) {
            tft.printf(" %s: %d min\n", ride["name"].as<const char*>(), ride["wait_time"].as<int>());
          }
        }
      }
      waitHttp.end();
  } else {
    Serial.printf("HTTP Error: %d\n", httpCode);
    tft.setTextDatum(MC_DATUM);
    tft.fillScreen(TFT_BLACK);
    drawOutlinedText("Web Connection Error. Reboot if connection fails again.", tft.width() / 2 , 15, TFT_RED, 4 );
    delay(10000);
  }
  http.end();
}

