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
  delay(10); // 延迟 10 毫秒
  Serial.println(); // 在串口监视器中打印空行
  Serial.print("Scanning for networks..."); // 在串口监视器中打印信息

  int n = WiFi.scanNetworks(); // 搜索可用的 Wi-Fi 网络
  if (n == 0) { // 如果没有搜索到任何网络
    Serial.println("No networks found"); // 在串口监视器中打印信息
  } else { // 如果搜索到了网络
    int strongestSignalIndex = -1; // 初始化最强信号的索引为 -1
    int strongestSignalStrength = -1000; // 初始化最强信号的强度为 -1000
    for (int i = 0; i < n; i++) { // 遍历所有搜索到的网络
      if (WiFi.encryptionType(i) == ENC_TYPE_NONE && WiFi.RSSI(i) > strongestSignalStrength) { // 如果该网络无需密码且信号强度大于当前最强信号的强度
        strongestSignalIndex = i; // 更新最强信号的索引
        strongestSignalStrength = WiFi.RSSI(i); // 更新最强信号的强度
      }
    }
    if (strongestSignalIndex == -1) { // 如果没有找到无需密码的网络
      Serial.println("No open networks found"); // 在串口监视器中打印信息
    } else { // 如果找到了无需密码的网络
      String ssid = WiFi.SSID(strongestSignalIndex); // 获取该网络的 SSID
      Serial.print("Connecting to "); // 在串口监视器中打印信息
      Serial.println(ssid); // 在串口监视器中打印 SSID
      WiFi.begin(ssid.c_str()); // 连接到该网络
    }
  }

  while (WiFi.status() != WL_CONNECTED) { // 等待连接成功
    delay(1000); // 延迟 1 秒钟
    Serial.println("Connecting to WiFi..."); // 在串口监视器中打印信息
  }

  Serial.println(""); // 在串口监视器中打印空行
  Serial.println("WiFi connected"); // 在串口监视器中打印信息
  Serial.println("IP address: "); // 在串口监视器中打印信息
  Serial.println(WiFi.localIP()); // 在串口监视器中打印本地 IP 地址
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
