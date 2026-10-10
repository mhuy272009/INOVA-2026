#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include "WebSocketMCP.h"
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <vector>

// ========= BUTTON =========
#define BOOT_PIN 0

// ========= OLED CONFIG (I2C) =========
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ========= SENSOR DHT22 =========
#define DHTPIN 23       
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE);

// ========= ĐÈN CẢNH BÁO NHIỆT ĐỘ =========
#define WARNING_LED 2    
float maxTemperatureWarning = 34.0; // Ngưỡng cảnh báo mặc định

// ========= OBJECT =========
WebServer server(80);
Preferences preferences;
WebSocketMCP mcpClient;

// ========= WIFI CONFIG =========
String wifi_ssid = "";
String wifi_pass = "";
String mcpEndpoint = "";

// ========= MODE =========
bool apMode = false;

// ========= BUTTON =========
unsigned long buttonPressStart = 0;
bool buttonHolding = false;

// ========= STATE =========
unsigned long wifiLostTime = 0;
unsigned long lastSensorCheck = 0;
unsigned long lastBlinkTime = 0;
bool warningLedState = false;

float currentTemp = 0.0;
float currentHumi = 0.0;

// Biến lưu trạng thái chống nhiễu màn hình OLED
float lastDisplayedTemp = -999.0;
float lastDisplayedHumi = -999.0;
float lastDisplayedWarning = -999.0;
bool lastDisplayedStatus = false;

// ========= TOOL STRUCT =========
struct ToolConfig {
  String name;
  String desc;
  int pin;
};

std::vector<ToolConfig> tools;

// ========= LOAD CONFIG =========
void loadConfig() {
  preferences.begin("config", true);

  wifi_ssid = preferences.getString("ssid", "");
  wifi_pass = preferences.getString("pass", "");
  mcpEndpoint = preferences.getString("mcp", "");
  
  String defaultTools = "["
                        "{\"name\":\"fan_control\",\"desc\":\"Den phong ngu nho\",\"pin\":13},"
                        "{\"name\":\"fan0_control\",\"desc\":\"Den phong ngu lon\",\"pin\":12},"
                        "{\"name\":\"fan1_control\",\"desc\":\"Den nha ve sinh\",\"pin\":14},"
                        "{\"name\":\"fan2_control\",\"desc\":\"Den nha tam\",\"pin\":27},"
                        "{\"name\":\"fan3_control\",\"desc\":\"Den suoi\",\"pin\":26},"
                        "{\"name\":\"fan4_control\",\"desc\":\"Den ban cong\",\"pin\":17},"
                        "{\"name\":\"fan5_control\",\"desc\":\"Den phong khach\",\"pin\":16},"
                        "{\"name\":\"fan6_control\",\"desc\":\"Den phong bep\",\"pin\":4},"
                        "{\"name\":\"fan7_control\",\"desc\":\"Den canh bao\",\"pin\":2},"
                        "{\"name\":\"fan8_control\",\"desc\":\"Den phong khach\",\"pin\":15},"
                        "{\"name\":\"fan9_control\",\"desc\":\"bat quat\",\"pin\":18}"
                        "]";
                        
  String json = preferences.getString("tools", defaultTools);
  preferences.end();

  DynamicJsonDocument doc(4096);
  if (deserializeJson(doc, json)) return;

  tools.clear();
  for (JsonObject obj : doc.as<JsonArray>()) {
    ToolConfig t;
    t.name = obj["name"].as<String>();
    t.desc = obj["desc"].as<String>();
    t.pin = obj["pin"];

    pinMode(t.pin, OUTPUT);
    digitalWrite(t.pin, LOW);
    tools.push_back(t);
  }
}

// ========= SAVE CONFIG =========
void saveConfig(String ssid, String pass, String mcp, String toolsJson) {
  preferences.begin("config", false);
  preferences.putString("ssid", ssid);
  preferences.putString("pass", pass);
  preferences.putString("mcp", mcp);
  preferences.putString("tools", toolsJson);
  preferences.end();
}

// ========= UPDATE OLED DISPLAY =========
void updateDisplay(bool force = false) {
  bool isWarning = (currentTemp >= maxTemperatureWarning);
  
  if (force || currentTemp != lastDisplayedTemp || currentHumi != lastDisplayedHumi || 
      maxTemperatureWarning != lastDisplayedWarning || isWarning != lastDisplayedStatus) {
    
    lastDisplayedTemp = currentTemp;
    lastDisplayedHumi = currentHumi;
    lastDisplayedWarning = maxTemperatureWarning;
    lastDisplayedStatus = isWarning;

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(0, 0);
    display.print("Nhiet do: ");
    display.print(currentTemp, 1);
    display.print(" C");

    display.setCursor(0, 16);
    display.print("Do am   : ");
    display.print(currentHumi, 1);
    display.print(" %");

    display.setCursor(0, 32);
    display.print("Canh bao: ");
    display.print(maxTemperatureWarning, 1);
    display.print(" C");

    display.setCursor(0, 48);
    if (isWarning) {
      display.print("Trang thai: CANH BAO!");
    } else {
      display.print("Trang thai: OK");
    }

    display.display();
  }
}

// ========= MCP TOOLS REGISTRATION =========
void registerMcpTools() {
  for (auto &t : tools) {
    mcpClient.registerTool(
      t.name.c_str(),
      t.desc.c_str(),
      "{\"type\":\"object\",\"properties\":{\"state\":{\"type\":\"string\",\"enum\":[\"on\",\"off\"]}},\"required\":[\"state\"]}",
      [t](const String& args) {
        DynamicJsonDocument doc(256);
        if (deserializeJson(doc, args)) {
          return WebSocketMCP::ToolResponse("{\"success\":false}");
        }
        String state = doc["state"];
        digitalWrite(t.pin, state == "on" ? HIGH : LOW);
        return WebSocketMCP::ToolResponse("{\"success\":true}");
      }
    );
  }

  mcpClient.registerTool(
    "temperature_and_humidity_values",
    "Lay nhiet do va do am phong tu cam bien DHT22",
    "{\"type\":\"object\",\"properties\":{}}",
    [](const String& args) {
      String responseJson = "{\"temperature\": " + String(currentTemp) + ", \"humidity\":" + String(currentHumi) + "}";
      return WebSocketMCP::ToolResponse(responseJson);
    }
  );

  mcpClient.registerTool(
    "set_temperature_warning",
    "Cai dat nguong nhiet do canh bao",
    "{\"type\":\"object\",\"properties\":{\"max_temperature\":{\"type\":\"number\"}},\"required\":[\"max_temperature\"]}",
    [](const String& args) {
      DynamicJsonDocument doc(256);
      if (deserializeJson(doc, args)) {
        return WebSocketMCP::ToolResponse("{\"success\":false}");
      }
      maxTemperatureWarning = doc["max_temperature"];
      String res = "{\"success\":true,\"max_temperature\":" + String(maxTemperatureWarning) + "}";
      return WebSocketMCP::ToolResponse(res);
    }
  );
}

void onConnectionStatus(bool connected) {
  if (connected) {
    Serial.println("✅ MCP connected");
    registerMcpTools();
  }
}

void WiFiEvent(WiFiEvent_t event) {
  switch(event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      wifiLostTime = 0;
      if (!apMode && mcpEndpoint != "") {
        mcpClient.begin(mcpEndpoint.c_str(), onConnectionStatus);
      }
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      wifiLostTime = millis();
      WiFi.reconnect();
      break;
    default:
      break;
  }
}

void handleRoot() {
  preferences.begin("config", true);
  String ssid = preferences.getString("ssid", "");
  String pass = preferences.getString("pass", "");
  String mcp  = preferences.getString("mcp", "");
  
  String defaultTools = "["
                        "{\"name\":\"fan_control\",\"desc\":\"Den phong ngu nho\",\"pin\":13},"
                        "{\"name\":\"fan0_control\",\"desc\":\"Den phong ngu lon\",\"pin\":12},"
                        "{\"name\":\"fan1_control\",\"desc\":\"Den nha ve sinh\",\"pin\":14},"
                        "{\"name\":\"fan2_control\",\"desc\":\"Den nha tam\",\"pin\":27},"
                        "{\"name\":\"fan3_control\",\"desc\":\"Den suoi\",\"pin\":26},"
                        "{\"name\":\"fan4_control\",\"desc\":\"Den ban cong\",\"pin\":17},"
                        "{\"name\":\"fan5_control\",\"desc\":\"Den phong khach\",\"pin\":16},"
                        "{\"name\":\"fan6_control\",\"desc\":\"Den phong bep\",\"pin\":4},"
                        "{\"name\":\"fan7_control\",\"desc\":\"Den canh bao\",\"pin\":2},"
                        "{\"name\":\"fan8_control\",\"desc\":\"Den phong khach\",\"pin\":15},"
                        "{\"name\":\"fan9_control\",\"desc\":\"bat quat\",\"pin\":18}"
                        "]";
                        
  String tools = preferences.getString("tools", defaultTools);
  preferences.end();

  String html = "<html><body><h2>Cấu hình ESP32</h2><form method='POST' action='/save'>"
                "SSID:<input name='ssid' value='" + ssid + "'><br>"
                "Pass:<input name='pass' type='password' value='" + pass + "'><br>"
                "MCP:<input name='mcp' value='" + mcp + "'><br>"
                "Tools:<textarea name='tools' rows='12' cols='50'>" + tools + "</textarea><br>"
                "<button type='submit'>Lưu</button></form></body></html>";
  server.send(200, "text/html; charset=utf-8", html);
}

void handleSave() {
  saveConfig(server.arg("ssid"), server.arg("pass"), server.arg("mcp"), server.arg("tools"));
  server.send(200, "text/html; charset=utf-8", "Đã lưu! Khởi động lại...");
  delay(1500);
  ESP.restart();
}

void startConfigMode() {
  WiFi.disconnect(true);
  delay(500);
  WiFi.softAP("ESP32_Config", "12345678");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("AP Mode Active");
  display.setCursor(0, 16);
  display.print("IP: 192.168.4.1");
  display.display();

  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.begin();
  apMode = true;
}

void handleButton() {
  if (digitalRead(BOOT_PIN) == LOW) {
    if (!buttonHolding) {
      buttonHolding = true;
      buttonPressStart = millis();
    }
    if (millis() - buttonPressStart > 5000 && !apMode) {
      startConfigMode();
    }
  } else {
    buttonHolding = false;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BOOT_PIN, INPUT_PULLUP);
  pinMode(WARNING_LED, OUTPUT);
  digitalWrite(WARNING_LED, LOW);

  // Khởi tạo I2C và màn hình OLED ngay lập tức ở đầu hàm
  Wire.begin(21, 22);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Dang khoi dong...");
  display.display();

  dht.begin();
  loadConfig();
  WiFi.onEvent(WiFiEvent);

  if (wifi_ssid == "") {
    startConfigMode();
    return;
  }

  WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
}

void loop() {
  if (apMode) {
    server.handleClient();
  } else {
    if (WiFi.status() == WL_CONNECTED) {
      mcpClient.loop();
    }

    // Đọc cảm biến DHT22 định kỳ mỗi 2 giây
    if (millis() - lastSensorCheck > 2000) {
      lastSensorCheck = millis();
      float h = dht.readHumidity();
      float t = dht.readTemperature();

      if (!isnan(h) && !isnan(t)) {
        currentHumi = h;
        currentTemp = t;
      }

      if (!apMode) {
        updateDisplay(); // Cập nhật màn hình OLED liên tục
      }
    }

    // Hiệu ứng đèn cảnh báo nhiệt độ
    if (currentTemp >= maxTemperatureWarning) {
      if (millis() - lastBlinkTime > 500) {
        lastBlinkTime = millis();
        warningLedState = !warningLedState;
        digitalWrite(WARNING_LED, warningLedState ? HIGH : LOW);
      }
    } else {
      digitalWrite(WARNING_LED, LOW);
      warningLedState = false;
    }

    if (wifiLostTime > 0 && millis() - wifiLostTime > 15000) {
      startConfigMode();
    }
  }

  handleButton();
}