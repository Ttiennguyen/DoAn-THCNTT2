
#include "DHT.h"
#include <ArduinoJson.h>
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <time.h> // thư viện thời gian thực

#define DHTPIN D3     // Chân DATA nối với 2
#define SOIL_MOIST_1_PIN A0 // Chân A1 nối với cảm biến độ ẩm
#define DHTTYPE DHT11   // DHT 11

#define WIFI_SSID "Dau nhon My Thai_2.4G"    // kết nối với mật khẩu wifi của mình 
#define WIFI_PASSWORD "SU0903800185"

#define FIREBASE_HOST "nhietdofinal1-default-rtdb.firebaseio.com/" // kết nối firebase bỏ phần http
#define FIREBASE_AUTH "zmlV0Ju0loxfhuiZ1j0JgGww1RxOsfOSHCGGtT2f"

FirebaseData firebaseData;
//WiFiClient client;
String path ="/";
FirebaseJson json;

int humDHT;
int tempDHT;
int soilMoist;
int timezone = 7*3600;
int dst =0;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Khởi tạo cổng serial baud 115200
  Serial.begin(115200);
  Serial.println("DHTxx test!");
  // Bắt đầu đọc dữ liệu
  dht.begin();
 /*---------connect wifi----------*/
 WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
 while(WiFi.status() != WL_CONNECTED)
 {
  delay(500);
  Serial.print(".");
 }
 
 Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
 Firebase.reconnectWiFi(true);
 if(!Firebase.beginStream(firebaseData,path))
 {
  Serial.println("REASON"+firebaseData.errorReason());
  Serial.println();
 }
 Serial.print("Connected with IP: ");
 Serial.println(WiFi.localIP());
 Serial.println();

// thời gian thực
 configTime(timezone,dst,"pool.ntp.org","time.nist.gov");
 Serial.println("Waiting for server");
 while(!time(nullptr))
 {
  Serial.print("#");
  delay(500);
 }
 Serial.println("Time respone ok!");
 
}

void loop() {
  readSensors();
  //Thoigian();
  // IN thông tin ra màn hình
  Serial.print("Do am: ");
  Serial.print(humDHT);
  Serial.print(" %\t");
  Serial.print("Nhiet do: ");
  Serial.print(tempDHT);
  Serial.print(" *C\t");
  Serial.print(" %\t");
  Serial.print("Do am dat: ");
  Serial.print(soilMoist);
  Serial.println(" %");
  
   delay(60000); // 1 phút thay đổi giá trị trên firebase 1 lần 
  
  //Firebase.setInt(firebaseData, path + "/DoAm",humDHT);
  //Firebase.setInt(firebaseData, path + "/NhietDo",tempDHT);
  //Firebase.setInt(firebaseData, path + "/DoAmDat",soilMoist);
  
  Firebase.pushString(firebaseData, path +"/DoAm",humDHT);
  Firebase.pushString(firebaseData, path +"/NhietDo",tempDHT);
  Firebase.pushString(firebaseData, path +"/DoAmDat",soilMoist);
  
}

int getSoilMoist()
{
  int i = 0;
  int anaValue = 0;
  for (i = 0; i < 10; i++)  
  {
    anaValue += analogRead(SOIL_MOIST_1_PIN); //Đọc giá trị cảm biến độ ẩm đất
    delay(50);   // Đợi đọc giá trị ADC
  }
  anaValue = anaValue / (i);
  anaValue = map(anaValue, 1023, 0, 0, 100); //Ít nước:0%  ==> Nhiều nước 100%
  return anaValue;
}
void readSensors(void)
{
  tempDHT = dht.readTemperature();   //Đọc nhiệt độ DHT22
  humDHT = dht.readHumidity();       //Đọc độ ẩm DHT22
  soilMoist = getSoilMoist();        //Đọc cảm biến độ ẩm đất
}

void Thoigian()
{
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  
  Serial.print(p_tm->tm_mday);  
  Serial.print("/");  
  Serial.print(p_tm->tm_mon+1);  
  Serial.print("/");  
  Serial.print(p_tm->tm_year+1900);
  Serial.print(" ");  
  
  Serial.print(p_tm->tm_hour);  
  Serial.print(":");  
  Serial.print(p_tm->tm_min);  
  Serial.print(":");  
  Serial.println(p_tm->tm_sec);
  delay(1000);  
}
 
