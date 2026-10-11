# 🏠 Vietnamese Voice-Controlled Smart Home Assistant (XiaoZhi + ESP32-S3)

[![Status](https://img.shields.io/badge/Status-Completed-success)]()
[![Platform](https://img.shields.io/badge/Platform-ESP32--S3%20|%20Raspberry%20Pi-blue)]()
[![Tech Stack](https://img.shields.io/badge/Tech%20Stack-IoT%20|%20AI%20|%20MQTT%20|%20MCP%20Protocol-orange)]()

An open-source, AI-integrated smart-home prototype that turns natural Vietnamese voice commands into logged, confirmed hardware actions, using an ESP32-S3 voice terminal, a language model, a custom Model Context Protocol (MCP) server and MQTT messaging.

---

## 💡 Concept & Problem Statement

* **The Problem:** Commercial smart home ecosystems (Google Home, Alexa, etc.) rely heavily on proprietary corporate clouds and hide the link between understanding speech and controlling a device. That makes them hard to customize and unsuitable for deep learning studies in embedded AI.
* **Research question:** Can a modular MCP architecture translate varied Vietnamese voice commands into correct, timely and safe device actions on low-cost embedded hardware?
* **Working hypothesis:** Explicit, bounded tool definitions with a returned result for every action give more traceability and more reliable actuation than an unstructured conversational interface.
* **The Solution:** Combining the **ESP32-S3** board with the open-source **XiaoZhi** platform to deliver a low-cost, self-hosted, and easily expandable architecture.

* **Project Objectives**
  * **Voice Control:** Control room lights and fans using natural Vietnamese voice commands.

  * **Environmental Telemetry:** Query real-time temperature and humidity readings via voice.

  * **Automated Routines:** Trigger conditional device actions based on customizable sensor thresholds.

* **Current Features**
  * **11 Voice-Controlled Outputs:** Supports 9 room lights, 1 ceiling fan, and 1 warning light.

  * **DHT22 Telemetry:** Measures ambient temperature and humidity, providing spoken audio responses.

  * **Temperature Rule Engine:** Automatically blinks the warning light when temperature hits a threshold (default 34 °C), adjustable dynamically via voice commands.

  * **Visual Emotion Indicators:** Displays real-time emotion states on the XiaoZhi device screen to match system interactions.

  * **OLED & Web Management:** Displays real-time status on an OLED screen and includes a captive portal Wi-Fi setup page for reconfiguration without re-flashing firmware.
---

## ⚡ Core Differentiation & Value Proposition

* **Transforming Chatbots into Action Takers:** Utilizing advanced IoT and MCP protocols so the AI does not just chat, but directly executes commands on physical hardware — shifting the system from a "talking brain" to an "acting brain".
* **Practical Value:** Immediate live-demo feedback making presentations highly impactful, low hardware barrier to entry, and a natural roadmap ideal for academic coursework or graduation theses in IoT and embedded AI.

---

## 🏗️ System Architecture & Components

* **End-to-End Pipeline:** 
1. User speaks (Vietnamese): a direct command ("Bật đèn"), a question ("Nhiệt độ bao nhiêu?") or an implicit statement ("Trời nóng quá").
2. Speech-to-text: the audio becomes text.
3. LLM reasoning and tool selection: the model reads the explicit command, question or implied intent, then picks one registered MCP tool, or answers directly if no tool is needed.
4. MCP tool call: the request is sent to the ESP32 over the WebSocket MCP connection.
5. Hardware execution: the ESP32 switches a pin or reads the DHT22.
6. Tool response: the ESP32 sends the result or the sensor reading back over the WebSocket.
7. LLM output: the model turns the result into a natural reply (for example "Đã bật quạt cho bạn rồi nhé").
8. Text-to-speech and playback: the reply is spoken back in Vietnamese.

Hardware (submitted system)

| Component | Description |
| :--- | :--- |
| **ESP32** | Runs the MCP client, switches the outputs, reads the sensor |
| **Raspberry Pi** | Runs the MCP server and the MQTT broker |
| **9 Room Lights** | Small bedroom, large bedroom, toilet, bathroom, kitchen, balcony, living room (x2), heater lamp |
| **Ceiling Fan** | On/off |
| **Warning Light** | Blinks when temperature is at or above the threshold |
| **DHT22** | Temperature and humidity sensor |
| **SSD1306 OLED (128x64)** | Status display |
| **BOOT Button** | Hold 5 seconds to open the Wi-Fi setup page |
   
* **Main Components:**
  * **Hardware:** ESP32-S3 board (XiaoZhi), 2-channel relay module, DHT22 temperature/humidity sensor, Raspberry Pi.
  * **Software:** XiaoZhi firmware, STT/LLM/TTS services, custom-built MCP Server, MQTT Broker.
  * **Auxiliary Features:** Real-time visual emotion indicators rendered on an LCD display.
<img width="465" height="252" alt="image" src="https://github.com/user-attachments/assets/acbc8cf8-c46f-4f22-917b-d1c57caff440" />
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/881b4129-2804-42ed-ab7e-88364fe50603" />

* **Communication Architecture & Protocol Bridge**
  * Raspberry Pi gateway: runs the MCP server and the MQTT broker, linking the AI service (cloud or local) to the ESP32 over Wi-Fi.
  * MQTT: carries sensor telemetry (for example, temperature on home/livingroom/sensor/temperature) and auxiliary system commands between the Raspberry Pi and the ESP32.
  * WebSocket MCP connection: carries tool-call requests and their results between the XiaoZhi server and the ESP32.
* **Tool Design and Safety**
  * Constrained functionality: the ESP32 exposes a fixed set of MCP tools, so the model can only trigger the defined hardware functions.
  * Context-aware descriptions: each tool description says when to use it and includes everyday feelings such as hot, cold, stuffy and dry. This helps the model connect an indirect phrase like “Trời nóng quá” to the right tool. How reliably it does this has not been measured yet.
* **Deployment Modes**
  * Cloud AI (demo setup): speech recognition, language modeling and speech synthesis ran on Google’s cloud AI services.
  * Offline / edge AI (local setup): speech and language models run on a local machine using the open-source XiaoZhi ESP32 server, so voice control works without internet.

---

## 🛠️ Operations, Safety & Reproducibility

* **Limitations and Safety Precautions**
* The demo setup used a cloud AI service, which adds network delay and privacy risk. The offline setup avoids the internet but depends on the speed and quality of the local hardware and models.
* Several outputs sit on ESP32 strapping pins (0, 2, 12, 15). A load on these pins can interfere with booting, so check them before reuse.
* The Wi-Fi setup page uses a fixed access-point password in the source. Change it before use.
* DHT22 accuracy limits the quality of the measurements.
* Use isolated low-voltage demonstration loads until electrical protection and fail-safe behavior are verified. Do not connect mains loads.

* **Future Work**

Read device state back for true confirmation, rename and group the tools by room, extend to more rooms and devices, compare the cloud and offline setups on accuracy and response time, try other local speech models, add stronger authentication, and publish measured results from repeated trials.

---

* **Repository Structure**

```
voicehome-ai/
├── README.md
├── docs/            # poster, architecture diagram, demo video link
├── firmware/        # ESP32 sketch (no Wi-Fi passwords or keys)
├── server/          # MCP server and XiaoZhi configuration
├── hardware/        # wiring diagram, parts list, safety notes
├── tests/           # Vietnamese command set, trial log template
└── results/         # raw logs and summaries (when trials are done)
```

* **Reproduction Step**
  * Wire the ESP32, outputs, DHT22 and OLED using the pin map above.
  * Flash the sketch. Hold BOOT for 5 seconds to open the Wi-Fi setup page and enter your own network and MCP endpoint.
  * Start the XiaoZhi server (cloud or offline).
  * Connect your speech and language service keys (never commit keys to the repository).
  * Run the demonstration sequence above.

---

## 🗓️ 12-Session Implementation Roadmap

The curriculum follows a "demo-first" approach to spark engagement and show immediate results before diving deep into Arduino/C++ firmware development. Divided into 4 phases:
1. *Quick Demo & Arduino Setup (Sessions 1–3)*
2. *Hardware Control Programming (Sessions 4–6)*
3. *AI & MCP Integration Programming (Sessions 7–10)*
4. *System Optimization, Polish & Final Demo (Sessions 11–12)*

| Session | Topic | Main Content | Expected Outcomes |
| :---: | :--- | :--- | :--- |
| **1** | Immediate Demo & Arduino IDE Setup | Flash pre-built XiaoZhi firmware to enable instant voice interaction; install Arduino IDE and configure ESP32-S3 board | Functional live demo and ready development environment |
| **2** | Basic Arduino Code Structure | Explore `setup()`/`loop()` syntax, variables, libraries, and flash a sample Blink LED sketch | Mastery of core Arduino program skeleton and successful code uploading |
| **3** | Understanding XiaoZhi Firmware Source Code | Explore repository directory structure, key audio processing modules, and AI handlers | Comprehension of code organization and ability to edit basic configurations |
| **4** | GPIO Control Programming | Write code to read push buttons and control digital pins (`digitalWrite`/`digitalRead`), with live demo | Ability to write and execute basic digital signal control scripts |
| **5** | Relay Control via Code | Implement raw code functions for manual relay switching, demonstrating light/fan control via firmware | Relays operating correctly based on custom code logic |
| **6** | Combining Code with Simple Voice Commands | Bind relay control functions to fixed spoken keyword triggers for voice toggling | First working voice-controlled hardware demo (speech command switches light) |
| **7** | DHT22 Sensor Interfacing | Install sensor libraries, write code to read temperature and humidity, and print to Serial Monitor | Accurate acquisition and serial display of environmental telemetry |
| **8** | Integrating Sensors into Firmware & Demo | Merge sensor reading routines into the XiaoZhi firmware to enable voice-queried stats | Device responds verbally with real-time temperature and humidity data |
| **9** | Writing MCP Tools for AI | Program C++ callback functions allowing the LLM to trigger actions like turning lights on or fans off | At least two fully operational MCP tools correctly invoked by the AI |
| **10** | MQTT Network Communication | Implement `PubSubClient` library to handle command exchange between the ESP32 board and MCP Server | Stable, bi-directional communication over MQTT |
| **11** | Full System Integration & Live Demo | Chain the complete architecture: Voice $\rightarrow$ AI $\rightarrow$ MCP Tool $\rightarrow$ Relays/Sensors for end-to-end scenarios | Fully integrated system capable of speech control and telemetry retrieval |
| **12** | Code Optimization, Debugging & Reporting | Code refactoring, latency reduction, cleanup, and preparation of presentation reports | Clean codebase, stable demo performance, and presentation readiness |

