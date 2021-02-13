#include <LiquidCrystal_PCF8574.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN D3
#define SDA_PIN D4

// Change this according to the WiFi SSID and password
String wifi_ssid = "SSID";
String wifi_pass = "password";

// Change this according to the server address
String server_address = "http://192.168.1.1:8080";

// Change this according to the gate id
String gate_id = "5ff98afe90384e440c3d6bec";

LiquidCrystal_PCF8574 lcd(0x3F);

MFRC522 mfrc522(SDA_PIN, RST_PIN);

void lcdPrint(String a, String b) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(a);

  lcd.setCursor(0, 1);
  lcd.print(b);

  Serial.println(a + " " + b);
}

void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);

  lcdPrint("Inisialisasi", "perangkat");
  delay(2000);

  SPI.begin();
  mfrc522.PCD_Init();
}

void loop() {
  if (WiFi.status() !=  WL_CONNECTED) {
    lcdPrint("Menghubungkan ke", wifi_ssid);

    WiFi.begin(wifi_ssid, wifi_pass);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
    }

    lcdPrint("Terhubung ke", "WiFi");
    delay(2000);

    lcdPrint("Alamat IP", WiFi.localIP().toString());
    delay(2000);

    lcdPrint("Mengubungkan ke", server_address);

    HTTPClient http;
    http.begin(server_address);

    int http_rc = http.GET();
    while (http_rc <= 0) {
      http_rc = http.GET();
      delay(1000);
    }

    http.end();

    lcdPrint("Terhubung ke", "server");
    delay(2000);

    lcdPrint("Silahkan tempel", "kartu");
    delay(2000);
  }

  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  String tag_id = "";
  for(byte i = 0; i < mfrc522.uid.size; i++){
    tag_id.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
    tag_id.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  tag_id.toUpperCase();

  lcdPrint("Kartu terdeteksi:", tag_id);
  delay(2000);

  lcdPrint("Memverifikasi", "kartu");

  HTTPClient http;
  String api_address = server_address + "/api/device/visitation/";
  http.begin(api_address + gate_id + "/" + tag_id);

  int http_rc = http.POST("");
  if (http_rc >= 0) {
    lcd.clear();
    lcd.setCursor(0, 0);

    lcd.print(http.getString());
    Serial.println(http.getString());
    delay(2000);
  }
  else {
    lcdPrint("Kesalahan perangkat", "");
    delay(2000);
  }

  http.end();

  lcdPrint("Silahkan tempel", "kartu");
  delay(2000);
}
