#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h> 
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#define RST_PIN D1
#define SDA_PIN D2

const char* ssid     = "Tolong Jangan Dipakai";
const char* password = "janganmaling";

String serverName = "http://192.168.100.36:8080/api";

MFRC522 mfrc522(SDA_PIN, RST_PIN);

void setup() {
  pinMode(D4, OUTPUT);
  digitalWrite(D4, HIGH);
  
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Put your card to the reader...");
  
  digitalWrite(D4, LOW);
}

void loop() {
  if(WiFi.status() !=  WL_CONNECTED){
    return;
  }
  
  if(!mfrc522.PICC_IsNewCardPresent()){
    return;
  }

  if(!mfrc522.PICC_ReadCardSerial()){
    return;
  }

  Serial.print("UID tag :");
  String content = "";
  byte letter;

  for(byte i = 0; i < mfrc522.uid.size; i++){
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  content.toUpperCase();

  Serial.println("");
  Serial.println("UID tag :" + content);
      
  HTTPClient http;
  
  String serverPath = serverName + "/card?tagId=" + content;
  http.begin(serverPath);

  Serial.println(serverPath);
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    if (http.getString() != "[]") {
      Serial.println("Kartu cocok");
      digitalWrite(D4, HIGH);
    } else {
      Serial.println("Kartu tidak cocok");
      digitalWrite(D4, LOW);
    }
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
  
  delay(1000);
  
//  if(content.substring(1) == "D0 F2 8D 32"){
//    digitalWrite(D4, HIGH);se
//    Serial.println("Kartu cocok");
//    Serial.println();
//    delay(1000);
//  }
//
//  else if(content.substring(1) == "79 D0 24 A3"){
//    digitalWrite(D4, HIGH);
//    Serial.println("Kartu cocok");
//    Serial.println();
//    delay(1000);
//  }
//
//  else{
//    digitalWrite(D4, LOW);
//    Serial.println("Kartu Tidk cocok");
//    delay(1000);
//  }

}
