\# ESP32 Alarm System



An ESP32-based alarm system controlled with a matrix keypad. It detects motion with a PIR sensor and can send notifications through ntfy.



\## Features



\- Arm and disarm the system using a matrix keypad

\- Detect motion using a PIR sensor

\- Display system status on an OLED screen

\- Provide audible and visual warnings using a buzzer and LEDs

\- Send motion notifications through ntfy

\- Continue operating as a local alarm when Wi-Fi is unavailable

\- Retry failed notifications at five-second intervals



\## Required Hardware



\- ESP32 development board

\- 4x3 matrix keypad

\- PIR motion sensor

\- SSD1306 OLED display

\- Buzzer

\- Red and green LEDs

\- Suitable resistors and connection wires



\## Configuration



Copy `secrets.example.h` as `secrets.h` and enter your own credentials:



```cpp

\#pragma once



const char\* ssid = "YOUR\_WIFI\_NAME";

const char\* wifi\_password = "YOUR\_WIFI\_PASSWORD";

const char\* ntfy\_url = "https://ntfy.sh/YOUR\_PRIVATE\_TOPIC";



\## Known Limitation



The keypad scanning code waits until a pressed key is released. This prevents one long key press from being registered repeatedly, but it also temporarily blocks the main loop. Therefore, PIR motion detection is paused while a keypad button is held down.



I left this behavior unchanged because this is a small prototype, keypad presses are normally very short, and the limitation does not significantly affect its intended use.



