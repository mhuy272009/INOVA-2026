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

  * **OLED & Web Management:** Displays real-time status on an OLED screen and includes a captive portal Wi-Fi setup page for reconfiguration without re-flashing firmware.
---

## ⚡ Core Differentiation & Value Proposition

* **Transforming Chatbots into Action Takers:** Utilizing advanced IoT and MCP protocols so the AI does not just chat, but directly executes commands on physical hardware — shifting the system from a "talking brain" to an "acting brain".
* **Practical Value:** Immediate live-demo feedback making presentations highly impactful, low hardware barrier to entry, and a natural roadmap ideal for academic coursework or graduation theses in IoT and embedded AI.

---

## 🏗️ System Architecture & Components

* **Workflow Model:** IoT Device (Voice Input/Output) -> Cloud AI Service (`STT` -> `LLM` -> `TTS`) -> `MCP Server` -> Physical Relays, Fans, and Sensors.
* **Main Components:**
  * **Hardware:** ESP32-S3 board (XiaoZhi), 2-channel relay module, DHT22 temperature/humidity sensor, Raspberry Pi.
  * **Software:** XiaoZhi firmware, STT/LLM/TTS services, custom-built MCP Server, MQTT Broker.
  * **Auxiliary Features:** Real-time visual emotion indicators rendered on an LCD display.
<img width="465" height="252" alt="image" src="https://github.com/user-attachments/assets/acbc8cf8-c46f-4f22-917b-d1c57caff440" />
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/881b4129-2804-42ed-ab7e-88364fe50603" />


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

