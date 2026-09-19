#include<vector>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "secrets.h"

const int KEYPAD_ROW_1 = 14;
const int KEYPAD_ROW_2 = 27;
const int KEYPAD_ROW_3 = 26;
const int KEYPAD_ROW_4 = 25;

const int KEYPAD_COL_1 = 33;
const int KEYPAD_COL_2 = 32;
const int KEYPAD_COL_3 = 23;

const int GREEN_LED = 13;
const int RED_LED = 18;

const int BUZZER = 19;

const int PIR_OUT = 34;

const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;

const int sda = 21;
const int scl = 22;

std::vector <char> v;
std::vector <char> password {'2', '5', '8', '0'};
std::vector <char> armed {'0', '0', '0', '0', '0', '0'};

bool is_armed {false};
bool is_triggered {false};
bool notification_sent {false};

const unsigned long NOTIFICATION_RETRY_MS = 5000UL;
unsigned long lastNotificationAttempt = 0;

int rows[4] = {KEYPAD_ROW_1, KEYPAD_ROW_2, KEYPAD_ROW_3, KEYPAD_ROW_4};
int cols[3] = {KEYPAD_COL_1, KEYPAD_COL_2, KEYPAD_COL_3};

char keys[4][3] =
{
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

HTTPClient http;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void printCentered(String text)
{
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  int horizontal_start {0};
  horizontal_start = (SCREEN_WIDTH - w) / 2;
  int vertical_start {0};
  vertical_start = (SCREEN_HEIGHT - h) / 2;
  display.setCursor(horizontal_start, vertical_start);
  display.println(text);
  display.display();
}

void clearScreen()
{
  display.clearDisplay();
  display.display();
}

void setup() 
{
  Serial.begin(115200);

  Wire.begin(sda, scl);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED initialization failed.");
    while (true)
    {
      delay(100);
    }
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  WiFi.begin(ssid, wifi_password);

  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);

  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  pinMode(PIR_OUT, INPUT);
  
  for (int i = 0; i < 4; i++)
  {
    pinMode(rows[i], OUTPUT);
    digitalWrite(rows[i], HIGH);
  }

  for (int i = 0; i < 3; i++)
  {
    pinMode(cols[i], INPUT_PULLUP);
  }

  unsigned long wifiStart = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 10000UL)
  {
    delay(500);
    Serial.print('.');
  }

  clearScreen();

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi connected!");
    printCentered("WiFi connected");
  }
  else
  {
    Serial.println("\nOffline mode.");
    printCentered("Offline mode");
  }

  delay(1500);
  clearScreen();
}

void loop() 
{
  for (int r = 0; r < 4; r++)
  {
    digitalWrite(rows[r], LOW);

    for (int c = 0; c < 3; c++)
    {
      if (digitalRead(cols[c]) == LOW)
      {
        delay(25);
        if (digitalRead(cols[c]) == LOW)
        {
          if (keys[r][c] == '#')
          {
            if (v == password)
            {
              Serial.println("\nAccess granted!");
              clearScreen();
              printCentered("System disarmed");
              digitalWrite(GREEN_LED, HIGH);
              digitalWrite(RED_LED, LOW);
              digitalWrite(BUZZER, HIGH);
              delay(100);
              digitalWrite(BUZZER, LOW);
              delay(50);
              digitalWrite(BUZZER, HIGH);
              delay(100);
              digitalWrite(BUZZER, LOW);
              delay(1250);
              digitalWrite(GREEN_LED, LOW);
              clearScreen();
              is_triggered = false;
              is_armed = false;
              notification_sent = false;
              v.clear();
            }
            else if (v == armed)
            {
              Serial.println("\nSystem will be armed in 10 seconds.");
              clearScreen();
              printCentered("Arming..");

              digitalWrite(GREEN_LED, HIGH);
              digitalWrite(RED_LED, HIGH); 

              for (int i = 0; i < 10; i++)
              {
                digitalWrite(BUZZER, HIGH);
                delay(150);
                digitalWrite(BUZZER, LOW);
                delay(850);
              }

              digitalWrite(GREEN_LED, LOW);
              digitalWrite(RED_LED, LOW);

              is_armed = true;
              v.clear();

              digitalWrite(BUZZER, HIGH);
              delay(100);
              digitalWrite(BUZZER, LOW);
              delay(100);
              digitalWrite(BUZZER, HIGH);
              delay(100);
              digitalWrite(BUZZER, LOW);

              Serial.println("System armed!");
              clearScreen();
              printCentered("System armed");
              delay(2000);
              clearScreen();
            }
            else
            {
              Serial.println("\nWrong password!");
              clearScreen();
              printCentered("Wrong password");
              digitalWrite(RED_LED, HIGH);
              digitalWrite(GREEN_LED, LOW);
              digitalWrite(BUZZER, HIGH);
              delay(300);
              digitalWrite(BUZZER, LOW);
              delay(300);
              digitalWrite(BUZZER, HIGH);
              delay(300);
              digitalWrite(BUZZER, LOW);
              delay(300);
              digitalWrite(BUZZER, HIGH);
              delay(300);
              digitalWrite(BUZZER, LOW);
              digitalWrite(RED_LED, LOW);
              clearScreen();
              v.clear();
            }
          }
          else if (keys[r][c] == '*')
          {
            if (v.size() != 0)
            {
              v.pop_back();
              Serial.print("\n");

              String masked = "";
              for (int i = 0; i < v.size(); i++)
              {
                Serial.print("*");
                masked += "*";
              }

              clearScreen();
              printCentered(masked);
            }
          }
          else
          {
            v.push_back(keys[r][c]);
            Serial.print("*");
            if (v.size() > 0)
            {
              String masked = "";
              for (int i{0}; i < v.size(); i++)
              {
                masked += "*";
              }

              clearScreen();
              printCentered(masked);
            }
          }
          while (digitalRead(cols[c]) == LOW)
          {
          }
          delay(25);
        }
      }
    }
  digitalWrite(rows[r], HIGH);
  }
  if (is_armed && digitalRead(PIR_OUT) == HIGH && !is_triggered)
  {
    is_triggered = true;
    lastNotificationAttempt = millis() - NOTIFICATION_RETRY_MS;
    clearScreen();
    printCentered("System triggered");
  }
  if (is_triggered)
  {
    digitalWrite(BUZZER, HIGH);
    digitalWrite(RED_LED, HIGH);
  }
  if (is_triggered && !notification_sent && WiFi.status() == WL_CONNECTED &&
      millis() - lastNotificationAttempt >= NOTIFICATION_RETRY_MS)
  {
    http.begin(ntfy_url);
    int responseCode = http.POST("Motion detected!");
    http.end();
    lastNotificationAttempt = millis();
    Serial.println("Motion detected!");
    
    if (responseCode > 199 && responseCode < 300)
    {
      Serial.println("Notification sent to user.");
      notification_sent = true;
    }
    else
    {
      Serial.println("Notification unsuccessful.");
    }
  }
}
