import time
import random
import requests
import DAN
import paho.mqtt.client as mqtt
from sklearn.linear_model import LogisticRegression
import numpy as np
ServerURL = 'http://120.108.111.234:9999'  # with non-secure connection
# ServerURL = 'https://DomainName' #with SSL connection
Reg_addr = None  # if None, Reg_addr = MAC address

DAN.profile['dm_name'] = 'GDF1080210790' #新建iottalk裝置名稱
DAN.profile['df_list'] = ['Gdf0519','Gdf']    #新建iottalk裝置Device_Feature名稱
DAN.profile['d_name'] = 'Gdf'  #這個裝置的名稱

DAN.device_registration_with_retry(ServerURL, Reg_addr)
# DAN.deregister()  #if you want to deregister this device, uncomment this line
# exit()            #if you want to deregister this device, uncomment this line


DAN.ControlChannel
det_Humidity = 0
det_Temperature= 0
det_Light = 0



control_message = False


def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # 將訂閱主題寫在on_connet中
    # 如果我們失去連線或重新連線時
    # 地端程式將會重新訂閱
 
    client.subscribe("inTopic") #MQTT的Topic訂閱

# 當接收到從伺服器發送的訊息時要進行的動作


def on_message(client, userdata, msg):
    global det_Temperature, det_Humidity, control_message, det_Light, message

    # 轉換編碼utf-8才看得懂中文
    message = msg.payload.decode('utf-8')
    distance = int(message)  # 將距離參數轉換為浮點數

    # 訓練數據集
    # 轉換為特征和標簽的列表
    training_data = [
        [50, "eat"],
        [60, "not eat"],
        [45, "eat"],
        [70, "not eat"],
        [35, "eat"],
        [80, "not eat"],
        # 其他訓練樣本...
    ]

    # 提取特征和標簽
    features = [sample[0] for sample in training_data]
    labels = [sample[1] for sample in training_data]

    # 創建邏輯回歸分類器
    classifier = LogisticRegression()

    # 轉換為NumPy數組，並確保樣本數一致
    features = np.array(features).reshape(-1, 1)
    labels = np.array(labels)

    # 訓練分類器
    classifier.fit(features, labels)

    # 使用分類器進行距離預測
    prediction = classifier.predict([[distance]])
    if prediction == "eat":
        print(msg.topic, message + ' cm EAT')
        DAN.push('Gdf0519', str(int(message)) + ' cm EAT')
        DAN.push('Gdf', str(int(message)) + ' cm EAT')
        return "eat"
    else:
        print(msg.topic, message + ' cm NOT EAT')
        DAN.push('Gdf0519', str(int(message)) + ' cm NOT EAT')
        DAN.push('Gdf', str(int(message)) + ' cm NOT EAT')
        return "not eat"

# 其他程式碼部分不需要修改

    #print(msg.topic,message, type(message))
    print(msg.topic,message)

    

    detect_data = message.split(",")
    
    control_message = True
    print("iottalk",message)


# 連線設定
# 初始化地端程式
client = mqtt.Client()

# 設定連線的動作
client.on_connect = on_connect

# 設定接收訊息的動作
client.on_message = on_message
print("client:", client.on_message)

# 設定登入帳號密碼
#client.username_pw_set("test", "test")

# 設定連線資訊(IP, Port, 連線時間)
client.connect("113.61.241.157", 1883, 60)


while True:
    if DAN.state == "RESUME":
        try:
            client.loop_start()
            if (control_message == True):
                DAN.push('Gdf0519', int(message ))
                DAN.push('Gdf',int(message ))
               # DAN.push('20230515_light', det_Light)


                control_message = False

        except Exception as e:
            print(e)
            if str(e).find('mac_addr not found:') != -1:
                print('Reg_addr is not found. Try to re-register...')
                DAN.device_registration_with_retry(ServerURL, Reg_addr)
            else:
                print('Connection failed due to unknow reasons.')
                time.sleep(1)
    else:
        client.loop_stop()
