#include <ESP8266WiFi.h> // 引入 ESP8266WiFi 庫
#include <PubSubClient.h> // 引入 PubSubClient 庫

// 替換為您的網絡憑據
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// 替換為您的 MQTT 代理的主機名和端口
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200); // 初始化串口通信
  setup_wifi(); // 設置 WiFi 連接
  client.setServer(mqtt_server, mqtt_port); // 設置 MQTT 服務器信息
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // 連接到 WiFi 網絡

  while (WiFi.status() != WL_CONNECTED) { // 等待連接成功
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); // 打印本地 IP 地址
}

void loop() {
  if (!client.connected()) { // 如果未連接到 MQTT 服務器，則重新連接
    reconnect();
  }
  client.loop(); // 處理 MQTT 消息

  // 替換為您的傳感器讀取代碼
  float sensorValue = analogRead(A0);

  // 將傳感器值發布到 MQTT 主題
  client.publish("your_topic", String(sensorValue).c_str());

  delay(1000); // 等待一秒鐘
}

void reconnect() {
  while (!client.connected()) { // 如果未連接到 MQTT 服務器，則嘗試重新連接
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) { // 如果連接成功，則打印信息
      Serial.println("connected");
    } else { // 如果連接失敗，則打印錯誤信息並等待五秒鐘再次嘗試
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
