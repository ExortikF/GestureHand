#include <WiFi.h>
#include <ESP32Servo.h>  // Библиотека для PWM-сервоприводов
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
// Wi-Fi настройки
const char* ssid = "Patron";
const char* password = "patron08";
WiFiServer server(8266);

// Пины для сервоприводов
#define SERVO1_PIN 17  // Кулак
#define SERVO2_PIN 5  // Локоть
#define SERVO3_PIN 21  // Плечо

#define TFT_CS 13   // Chip Select (CS)
#define TFT_RST 33  // Reset (RST)
#define TFT_DC 32   // Data/Command (DC)

// Инициализация дисплея
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Размер текста
#define TEXT_SIZE 2

// Объекты сервоприводов
Servo servo1;
Servo servo2;
Servo servo3;

// Предыдущие углы
int prev_angles[3] = { 0, 0, 0 };

void showCenteredText(String text) {
  tft.fillScreen(ST7735_BLACK);  // Очищаем экран
  tft.setTextSize(TEXT_SIZE);
  tft.setTextColor(ST7735_WHITE);

  // Вычисляем ширину и высоту текста
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  // Вычисляем центр экрана
  int16_t centerX = (tft.width() - w) / 2;
  int16_t centerY = (tft.height() - h) / 2;

  // Устанавливаем курсор и выводим текст
  tft.setCursor(centerX, centerY);
  tft.print(text);
}

void setup() {
  Serial.begin(115200);
  
  // Подключение к Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Подключение к Wi-Fi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    Serial.println("\nОшибка Wi-Fi! Код: " + String(WiFi.status()));
  }
  Serial.println("\nWi-Fi подключён!");
  Serial.print("IP ESP32: ");
  Serial.println(WiFi.localIP());

  server.begin();

  // Подключаем сервоприводы к GPIO
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);

  Serial.println("Сервоприводы готовы!");
  tft.initR(INITR_BLACKTAB);  // Инициализация ST7735 (чёрная плата)
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);  // Можно менять (0-3) для правильной ориентации

  showCenteredText("Готово!");
  delay(1000);
}

// Функция для управления сервоприводами
void moveServo(int servoIndex, int angle) {
  if (servoIndex == 0) {
    servo1.write(angle);  // Двигаем кулак
    Serial.println("двигаем серво");
    Serial.println(servoIndex);
    Serial.println(angle);
  } else if (servoIndex == 1) {
    servo2.write(angle);  // Двигаем локоть
    Serial.println("двигаем серво");
    Serial.println(servoIndex);
    Serial.println(angle);
  } else if (servoIndex == 2) {
    servo3.write(angle);  // Двигаем плечо
    Serial.println("двигаем серво");
    Serial.println(servoIndex);
    Serial.println(angle);
  }
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Клиент подключился!");

    while (client.connected()) {
      if (client.available()) {
        String data = client.readStringUntil('\n');
        Serial.print("Принято по Wi-Fi: ");
        Serial.println(data);
        showCenteredText(data);
        

        // Разбираем строку "45,90,120" → [45,90,120]
        int angles[3];
        int index = 0;
        char* ptr = strtok((char*)data.c_str(), ",");
        while (ptr != NULL && index < 3) {
          angles[index] = atoi(ptr);
          ptr = strtok(NULL, ",");
          index++;
        }


        // Проверяем, изменились ли углы
        for (int i = 0; i < 3; i++) {
          if (angles[i] != prev_angles[i]) {
            moveServo(i, angles[i]);
            prev_angles[i] = angles[i];
            
          }
        }
      }
    }

    client.stop();
    Serial.println("Клиент отключился");
  }
}
