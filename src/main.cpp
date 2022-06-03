#include <Arduino.h>
#include <Adafruit_CAP1188.h>
#include <WiFi.h>
#include <HttpClient.h>

#define LED_PIN 15
#define TOUCH_PIN 7

#define SESSION_MSG_STATE 0
#define READY_MSG_STATE 1
#define READY_STATE 2
#define INIT_STATE 3
#define WAIT_STATE 4
#define REACTION_STATE 5

#define MIN_WAIT_TIME 1000
#define MAX_WAIT_TIME 5000

#define SESSION_LENGTH 3

Adafruit_CAP1188 cap = Adafruit_CAP1188();
unsigned long sessionCount = 1;
unsigned long testCount = 1;
unsigned long state;
unsigned long timer;
unsigned long startReaction;

char ssid[] = "RileyHotspot";
char pass[] = "FinalProject";

IPAddress kIPAddress = IPAddress(35, 173, 249, 188);
const int kPort = 5000;
const int kNetworkTimeout = 30000;
const int kNetworkDelay = 1000;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(9600);
  if (!cap.begin()) {
    Serial.println("CAP1188 not found. Check hardware connection.");
    while (1)
      ;
  }
  delay(3000);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.println();
  delay(3000);
}

void loop() {
  uint8_t touched = cap.touched();
  switch (state)
  {
    case SESSION_MSG_STATE:
    {
      Serial.print("|| Starting Session #");
      Serial.print(sessionCount);
      Serial.println(" ||");
      Serial.println();
      state = READY_MSG_STATE;
      break;
    }
    case READY_MSG_STATE:
    {
      Serial.println("Press your finger on C1.");
      state = READY_STATE;
      break;
    }
    case READY_STATE:
    {
      if (touched & 1)
      {
        state = INIT_STATE;
      }
      break;
    }
    case INIT_STATE:
    {
      if (touched & 1)
      {
        Serial.print("Let's Begin! Test #");
        Serial.println(testCount);
        timer = millis() + random(MIN_WAIT_TIME, MAX_WAIT_TIME);
        state = WAIT_STATE;
      }
      else
      {
        state = READY_MSG_STATE;
      }
      break;
    }
    case WAIT_STATE:
    {
      if (touched & 1)
      {
        if (millis() >= timer)
        {
          digitalWrite(LED_PIN, HIGH);
          startReaction = millis();
          state = REACTION_STATE;
        }
      }
      else
      {
        state = READY_MSG_STATE;
      }
      break;
    }
    case REACTION_STATE:
    {
      if (touched & (1 << TOUCH_PIN))
      {
        unsigned long reactionSpeed = millis() - startReaction;
        digitalWrite(LED_PIN, LOW);

        int err = 0;
        WiFiClient c;
        HttpClient http(c);

        String kPathString = "/?var=" + String(reactionSpeed);
        const char *kPath = kPathString.c_str();

        err = http.get(kIPAddress, NULL, kPort, kPath);
        Serial.println();
        Serial.println("------------------------------");
        if (err == 0)
        {
          err = http.responseStatusCode();
          if (err >= 200 && err <= 299)
          {
            err = http.skipResponseHeaders();
            if (err >= 0)
            {
              int bodyLen = http.contentLength();
              unsigned long timeoutStart = millis();
              char c;
              while ((http.connected() || http.available()) &&
                    ((millis() - timeoutStart) < kNetworkTimeout))
              {
                if (http.available())
                {
                  c = http.read();
                  Serial.print(c);
                
                  bodyLen--;
                  timeoutStart = millis();
                }
                else
                {
                    delay(kNetworkDelay);
                }
              }
              testCount++;
            }
            else
            {
              Serial.print("Failed to skip response headers: ");
              Serial.print(err);
            }
          }
          else
          {    
            Serial.print("Getting response failed: ");
            Serial.print(err);
          }
        }
        else
        {
          Serial.print("Connect failed: ");
          Serial.print(err);
        }
        Serial.println();
        Serial.println("------------------------------");
        Serial.println();
        http.stop();
        delay(500);

        if (testCount >= 4)
        {
          Serial.print("|| Finished Session #");
          Serial.print(sessionCount);
          Serial.print(". ");
          int err = 0;
          WiFiClient c;
          HttpClient http(c);

          String kPathString = "/average";
          const char *kPath = kPathString.c_str();

          err = http.get(kIPAddress, NULL, kPort, kPath);
          if (err == 0)
          {
            err = http.responseStatusCode();
            if (err >= 200 && err <= 299)
            {
              err = http.skipResponseHeaders();
              if (err >= 0)
              {
                int bodyLen = http.contentLength();
                unsigned long timeoutStart = millis();
                char c;
                while ((http.connected() || http.available()) &&
                      ((millis() - timeoutStart) < kNetworkTimeout))
                {
                  if (http.available())
                  {
                    c = http.read();
                    Serial.print(c);
                  
                    bodyLen--;
                    timeoutStart = millis();
                  }
                  else
                  {
                      delay(kNetworkDelay);
                  }
                }
                Serial.println(" ||");
              }
              else
              {
                Serial.print("Failed to skip response headers: ");
                Serial.print(err);
              }
            }
            else
            {    
              Serial.print("Getting response failed: ");
              Serial.print(err);
            }
          }
          else
          {
            Serial.print("Connect failed: ");
            Serial.print(err);
          }

          Serial.println();
          sessionCount++;
          testCount = 1;
          state = SESSION_MSG_STATE;
        }
        else
        {
          state = INIT_STATE;
        }
      }
      break;
    }
  }
}