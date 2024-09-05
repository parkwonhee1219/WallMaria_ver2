#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#define Water_pin A0 //수위 감지 센서 핀 

// WiFi 설정
const char* ssid = "juh5032";
const char* password = "asdf1234";

// MQTT 브로커 설정
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "weather/WallMaria";

String message="null";// MQTT 브로커로 받을 메세지 

// 클라이언트 객체 생성
WiFiClient espClient;
PubSubClient client(espClient);

// 모터 핀 설정
const int servoPin = 4; //서보모터 핀 
Servo servo;

void setup() {
  Serial.begin(115200);// debug용 시리얼 통신 
  servo.attach(servoPin);
  pinMode(Water_pin, INPUT);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);  // 콜백 함수 설정
  if(client.connect("Esp8266_WallMaria")){
    client.subscribe(mqtt_topic);
  }
}

// WiFi 연결 함수
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// 메시지 수신 콜백 함수
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  message="";

  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.println(message);

  int Water_value = analogRead(Water_pin);
  Serial.print("Water Value: ");
  Serial.println(Water_value);
  
  // 수위센서 on
  if (Water_value > 350) {
     if (message == "open") {
      Serial.println("WALLMARIA is OPEN / Water_value > 350 && message == open");
      servo.write(179);
    } else if (message == "close") {
      Serial.println("WALLMARIA is Close / Water_value > 350 && message == close");
      servo.write(0);
    }
  }
  // 수위센서 off
  else if (Water_value < 350) {
     if (message == "open") {
      Serial.println("WALLMARIA is OPEN / Water_value < 350 && message == open");
      servo.write(179);
    } else if (message == "close") {
      Serial.println("WALLMARIA is Close / Water_value < 350 && message == close");
      servo.write(0);
    }
  }
}

// MQTT 재연결 함수
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
      Serial.print("Subscribed to topic: ");
      Serial.println(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again");
      delay(5000);  // 딜레이 시간을 증가시켜 재시도
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(1000);
}

