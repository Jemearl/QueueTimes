#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include "DisneylandLogo_Small.h";

const char* ssid = "";
const char* password = "";

TFT_eSPI tft = TFT_eSPI();

#define BUTTON_1 0  // GPIO0
#define BUTTON_2 35 // GPIO35

void setup() {

  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);

  WiFi.begin(ssid, password);
  tft.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    tft.print(".");
  }
  tft.println("\nConnected! ");
 //pinMode(BUTTON_1, INPUT_PULLUP);
 //pinMode(BUTTON_2, INPUT_PULLUP);
  
}


void loop() {
/*    if (digitalRead(BUTTON_1) == LOW) {
        Serial.println("Button 1 Pressed!");
    }
*/
tft.setSwapBytes(true);
fetchQueueTimes("16");
delay(20000);
fetchQueueTimes("17");
delay(20000);

}

void drawOutlinedText(String text, int x, int y, int fontColor, int font ) {
/*
//void drawOutlinedText(TFT_eSPI &tft, const String &text, int x, int y, uint16_t textColor, uint16_t outlineColor) {
  

 // Get the current font height to scale the outline thickness
  int outlineOffset = tft.fontHeight() / 10; // Adjust ratio as needed

  if (outlineOffset < 1) outlineOffset = 1; // Prevent zero offset on small fonts

  // Draw outline in 8 directions around the center
  tft.setTextColor(fontColor);
  for (int dx = -outlineOffset; dx <= outlineOffset; dx++) {
    for (int dy = -outlineOffset; dy <= outlineOffset; dy++) {
      if (dx == 0 && dy == 0) continue;
      tft.drawString(text, x + dx, y + dy, font);
    }
  }

  // Draw center text
  tft.setTextColor(textColor);
  tft.drawString(text, x, y, font);

*/






  tft.setTextColor(TFT_BLACK);
  for (int dx = -1; dx <= 1; dx++) {
  for (int dy = -1; dy <= 1; dy++) {
  if (dx != 0 || dy != 0) {
    tft.drawString(text, x + dx, y + dy, font);
      }
    }
  }
  tft.setTextColor(fontColor);
  tft.drawString(text, x, y, font);
}


void printChunksByWord(String text, int maxLength) {
  int start = 0;
  int Ystart = 45;
  int FontColor = TFT_YELLOW;

  while (start < text.length()) {
    int end = start + maxLength;

    if (end >= text.length()) {
      drawOutlinedText(text.substring(start), tft.width() / 2 , Ystart , FontColor, 4 );
     // tft.drawString(text.substring(start), tft.width() / 2 , Ystart , 4);
      break;
    }

    int spaceIndex = text.lastIndexOf(' ', end);

    if (spaceIndex <= start) {
      // No space found, force split at max length
      spaceIndex = end;
    }

    String chunk = text.substring(start, spaceIndex);
    drawOutlinedText(chunk, tft.width() / 2 , Ystart , FontColor, 4 );
    //tft.drawString(chunk, tft.width() / 2 , Ystart , 4);
    Ystart = Ystart + 25;
    
    // Skip the space (if one was found)
    start = (text.charAt(spaceIndex) == ' ') ? spaceIndex + 1 : spaceIndex;
  } 
}



void fetchQueueTimes(String IDNum) {
  HTTPClient http;
  http.begin("https://queue-times.com/parks.json");
  int httpCode = http.GET();
 //  tft.println("Grabbed the httpCode");

  if (httpCode == 200) {
    String payload = http.getString();
    DynamicJsonDocument doc(8192);
    deserializeJson(doc, payload);
     // tft.println("Deserialized Json");

    JsonArray parks = doc["parks"];
     //    tft.println(parks);
  /*  for (JsonObject park : parks) {
      tft.println("For loop 1");
      const char* name = park["name"];
      int id = park["id"];
      tft.println(name); */ 

      String waitUrl = "https://queue-times.com/parks/" + IDNum +  "/queue_times.json"; // + String(id) + "/queue_times.json"; 
      HTTPClient waitHttp;
      waitHttp.begin(waitUrl);
      int waitCode = waitHttp.GET();
     // tft.println(waitUrl);
     // tft.println(waitCode);
      if (waitCode == 200) {
        String waitPayload = waitHttp.getString();
        DynamicJsonDocument waitDoc(8192);
        deserializeJson(waitDoc, waitPayload);
      //  tft.println(waitPayload);

        if (waitDoc.containsKey("lands")) {
        //  tft.println("Inside the if for lands");
          for (JsonObject land : waitDoc["lands"].as<JsonArray>()) {
         //   tft.fillScreen(TFT_BLACK);
          //  tft.println("Inside FOR - Lands");
          // tft.drawString(land["name"].as<const char*>(),10,20,2);
            //tft.println(land["name"].as<const char*>());
            for (JsonObject ride : land["rides"].as<JsonArray>()) {
                tft.fillScreen(TFT_BLACK);
                if (IDNum == "16") {
                  tft.pushImage(0,0,302,170,DisneylandLogo_Small);  // Disneyland Logo
                }
               // tft.setTextColor(TFT_GREEN);
                tft.setTextDatum(MC_DATUM);
                drawOutlinedText(land["name"].as<const char*>(), tft.width() / 2 , 15, TFT_GREEN, 2 );
               // tft.drawString(land["name"].as<const char*>(),tft.width() / 2 ,15, 2);
             // tft.printf(" %s: %d min\n", ride["name"].as<const char*>(), ride["wait_time"].as<int>());
                tft.setTextColor(TFT_BLUE);
                printChunksByWord(ride["name"].as<const char*>(), 25);
               // tft.drawString(ride["name"].as<const char*>(), tft.width() / 2, 45, 4);
                if (ride["is_open"].as<String>() == "false")
                {
                  //tft.setTextColor(TFT_RED);
                  drawOutlinedText("Closed", tft.width() / 2 , 130, TFT_RED, 4 );
                 // tft.drawString("Closed", tft.width() / 2, 120, 4);
                }
                else
                {
                 // tft.setTextColor(TFT_WHITE);
                  drawOutlinedText(ride["wait_time"].as<String>(), tft.width() / 2 , 130, TFT_WHITE, 6 );
                 // tft.drawString(ride["wait_time"].as<String>(), tft.width() / 2, 120, 6);
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
  //  }
  } else {
    tft.printf("HTTP Error: %d\n", httpCode);
  }
  http.end();
}

