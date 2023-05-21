#include <ESP8266WiFi.h> // 引入 ESP8266WiFi 庫
#include <PubSubClient.h> // 引入 PubSubClient 庫
const int trigPin = 2;  //D4
const int echoPin = 0;  //D3
long duration, cm, inches;


// 替換為您的 MQTT 代理的主機名和端口
const char* mqtt_server = "113.61.241.157";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
   Serial.begin (9600);             // 設定序列埠通訊頻率
  pinMode(trigPin, OUTPUT);        // 定義輸入及輸出 
  pinMode(echoPin, INPUT);
 
  setup_wifi(); // 設置 WiFi 連接
  client.setServer(mqtt_server, mqtt_port); // 設置 MQTT 服務器信息
}

void setup_wifi() {
  delay(10); // 延迟 10 毫秒
  Serial.println(); // 在串口监视器中打印空行
  Serial.print("Scanning for networks..."); 

  int n = WiFi.scanNetworks(); 
  if (n == 0) { 
    Serial.println("No networks found");
  } else { // 
    int strongestSignalIndex = -1; 
    int strongestSignalStrength = -1000; 
    for (int i = 0; i < n; i++) { 
      if (WiFi.encryptionType(i) == ENC_TYPE_NONE && WiFi.RSSI(i) > strongestSignalStrength) { 
        strongestSignalIndex = i; 
        strongestSignalStrength = WiFi.RSSI(i); 
      }
    }
    if (strongestSignalIndex == -1) { 
      Serial.println("No open networks found"); 
    } else { 
      String ssid = WiFi.SSID(strongestSignalIndex); 
      Serial.print("Connecting to "); 
      Serial.println(ssid); 
      WiFi.begin(ssid.c_str()); 
    }
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000); 
    Serial.println("Connecting to WiFi...");
  }

  Serial.println(""); // 在串口监视器中打印空行
  Serial.println("WiFi connected"); 
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP()); 
}
void loop() {
  if (!client.connected()) { // 如果未連接到 MQTT 服務器，則重新連接
    reconnect();
  }
  client.loop(); // 處理 MQTT 消息
  

  // 替換為您的傳感器讀取代碼
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);     // 給 Trig 高電位，持續 10微秒
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  pinMode(echoPin, INPUT);             // 讀取 echo 的電位
  duration = pulseIn(echoPin, HIGH);   // 收到高電位時的時間
 
  cm = (duration/2) / 29.1;         // 將時間換算成距離 cm 或 inch  
  inches = (duration/2) / 74; 

 
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  
  delay(250);
  //int sensorValue = analogRead(cm);

  // 將傳感器值發布到 MQTT 主題
  client.publish("inTopic", String(cm).c_str());

  delay(1000); // 等待一秒鐘
}

void reconnect() {
  while (!client.connected()) { // 如果未連接到 MQTT 服務器，則嘗試重新連接
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) { // 如果連接成功，則打印信息
      Serial.println("connected");
    } else { // 如果連接失敗，則打印錯誤信息並等待1秒鐘再次嘗試
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1 seconds");
      delay(1000);
    }
  }
}
