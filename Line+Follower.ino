/*
 * Line Follower Robot - Final Complete Code
 * Arduino Mega 2560 + IBT-2 Motor Driver + 5 IR Sensors + RFID + LIDAR
 */

#include <SPI.h>
#include <MFRC522.h>

// Pin definitions
#define POWER_PIN 28
#define SENSOR1 22
#define SENSOR2 24
#define SENSOR3 25
#define SENSOR4 26
#define SENSOR5 27

// IBT-2 Motor Driver pins
#define LEFT_RPWM   5
#define LEFT_LPWM   6
#define LEFT_R_EN   7
#define LEFT_L_EN   8
#define RIGHT_RPWM  9
#define RIGHT_LPWM  10
#define RIGHT_R_EN  11
#define RIGHT_L_EN  12

// RFID ve BUZZER
#define RST_PIN 2
#define SDA_PIN 53
#define BUZZER_PIN 4

// Sensor variables
int s1, s2, s3, s4, s5;

// Speed settings
const int NORMAL_SPEED = 100;
const int CORRECTION_SPEED = 20;
const int TURN_SPEED = 130;
const int SEARCH_SPEED = 80;

// Turn control
unsigned long lastTurnTime = 0;
const unsigned long TURN_COOLDOWN = 1000;

// RFID ve LIDAR setup
MFRC522 rfid(SDA_PIN, RST_PIN);
const int threshold = 50;

// LIDAR fonksiyonu
uint16_t checkLIDAR() {
  if (Serial1.available() >= 9) {
    if (Serial1.read() == 0x59 && Serial1.read() == 0x59) {
      uint8_t buf[7];
      for (int i = 0; i < 7; i++) buf[i] = Serial1.read();
      uint16_t dist = buf[0] + (buf[1] << 8);
      Serial.print("Distance: ");
      Serial.print(dist);
      Serial.println(" cm");
      return dist;
    }
  }
  return 0;
}

// RFID fonksiyonu - BASİT ve ETKİLİ
void checkRFID() {
  // Yeni kart var mı?
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }
  
  // Kart okunabiliyor mu?
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }
  
  Serial.print("Kart UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();
  
  // HEMEN bip sesi çıkar
  digitalWrite(BUZZER_PIN, HIGH);
  delay(50); // Kısa bip için 50ms
  digitalWrite(BUZZER_PIN, LOW);
  
  // RFID'yi hazırda beklet
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void setup() {
  // Power pin
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, HIGH);
  
  // Sensor pins
  pinMode(SENSOR1, INPUT);
  pinMode(SENSOR2, INPUT);
  pinMode(SENSOR3, INPUT);
  pinMode(SENSOR4, INPUT);
  pinMode(SENSOR5, INPUT);

  // RFID pinleri
  pinMode(RST_PIN, OUTPUT);
  pinMode(SDA_PIN, OUTPUT);
  
  // Motor pins
  pinMode(LEFT_RPWM, OUTPUT);
  pinMode(LEFT_LPWM, OUTPUT);
  pinMode(LEFT_R_EN, OUTPUT);
  pinMode(LEFT_L_EN, OUTPUT);
  pinMode(RIGHT_RPWM, OUTPUT);
  pinMode(RIGHT_LPWM, OUTPUT);
  pinMode(RIGHT_R_EN, OUTPUT);
  pinMode(RIGHT_L_EN, OUTPUT);
  
  // Enable motors
  digitalWrite(LEFT_R_EN, HIGH);
  digitalWrite(LEFT_L_EN, HIGH);
  digitalWrite(RIGHT_R_EN, HIGH);
  digitalWrite(RIGHT_L_EN, HIGH);
  
  Serial.begin(9600);
  while (!Serial);
  
  // RFID setup
  SPI.begin();
  rfid.PCD_Init();
  delay(4);
  
  // RFID Self Test
  byte version = rfid.PCD_ReadRegister(rfid.VersionReg);
  Serial.print("MFRC522 Version: 0x");
  Serial.println(version, HEX);
  if (version == 0x00 || version == 0xFF) {
    Serial.println("RFID BAĞLANTI HATASI! VCC=3.3V kontrol et");
    while(1);
  }
  
  Serial.println("RFID modülü hazır");
  
  // LIDAR setup
  Serial1.begin(115200);
  
  // Buzzer setup
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  Serial.println("Line Follower Robot Ready with RFID and LIDAR");
  delay(2000);
}

void loop() {
  // LIDAR Mesafe Kontrolü
  uint16_t distance = checkLIDAR();
  if (distance > 0 && distance <= threshold) {
    stopMotors();
    tone(BUZZER_PIN, 2000, 200);
    delay(400);
    return;
  } else {
    noTone(BUZZER_PIN);
  }
  
  // RFID Kontrolü - HER DÖNGÜDE kontrol et
  checkRFID();
  
  // Line follower mantığı
  readSensors();
  printSensors();
  followLine();
  
  delay(10); // Ana döngüyü biraz yavaşlat
}

void readSensors() {
  s1 = digitalRead(SENSOR1);
  s2 = digitalRead(SENSOR2);
  s3 = digitalRead(SENSOR3);
  s4 = digitalRead(SENSOR4);
  s5 = digitalRead(SENSOR5);
}

void printSensors() {
  Serial.print(s1);
  Serial.print(s2);
  Serial.print(s3);
  Serial.print(s4);
  Serial.print(s5);
  Serial.print(" -> ");
}

void followLine() {
  int total = s1 + s2 + s3 + s4 + s5;
  
  if (total == 5) {
    stopMotors();
    Serial.println("STOP - All sensors");
    delay(2000);
    return;
  }

  if (millis() - lastTurnTime > TURN_COOLDOWN) {
    if (s1 == 0 && s2 == 0 && s3 == 0 && s4 == 1 && s5 == 1) {
      turnLeft90();
      Serial.println("TURN LEFT 90°");
      return;
    }
  
    if (s1 == 1 && s2 == 1 && s3 == 0 && s4 == 0 && s5 == 0) {
      turnRight90();
      Serial.println("TURN RIGHT 90°");
      return;
    }
  }
  
  if (s1 == 1 && s2 == 0 && s3 == 0 && s4 == 1 && s5 == 1) {
    correctLeft();
    Serial.println("CORRECT LEFT");
    return;
  }
  if (s1 == 1 && s2 == 1 && s3 == 0 && s4 == 0 && s5 == 1) {
    correctRight();
    Serial.println("CORRECT RIGHT");
    return;
  }
  
  if (s2 == 0 && s4 == 0 && s3 == 0 && s1 == 1 && s5 == 1) {
    moveForward();
    Serial.println("FORWARD - Both sides");
    return;
  }

  // Varsayılan: ileri git
  moveForward();
  Serial.println("FORWARD - Default");
}

void moveForward() {
  analogWrite(LEFT_RPWM, NORMAL_SPEED);
  analogWrite(LEFT_LPWM, 0);
  analogWrite(RIGHT_RPWM, NORMAL_SPEED);
  analogWrite(RIGHT_LPWM, 0);
}

void correctLeft() {
  analogWrite(LEFT_RPWM, CORRECTION_SPEED);
  analogWrite(LEFT_LPWM, 0);
  analogWrite(RIGHT_RPWM, NORMAL_SPEED - 40);
  analogWrite(RIGHT_LPWM, 0);
}

void correctRight() {
  analogWrite(LEFT_RPWM, NORMAL_SPEED - 40);
  analogWrite(LEFT_LPWM, 0);
  analogWrite(RIGHT_RPWM, CORRECTION_SPEED);
  analogWrite(RIGHT_LPWM, 0);
}
 
void turnLeft90() {
  analogWrite(LEFT_RPWM, 0);
  analogWrite(LEFT_LPWM, TURN_SPEED + 50);
  analogWrite(RIGHT_RPWM, TURN_SPEED + 50);
  analogWrite(RIGHT_LPWM, 0);
  
  delay(600);
  moveForward();
  delay(150);
  lastTurnTime = millis();
}

void turnRight90() {
  analogWrite(LEFT_RPWM, TURN_SPEED + 100);
  analogWrite(LEFT_LPWM, 0);
  analogWrite(RIGHT_RPWM, 0);
  analogWrite(RIGHT_LPWM, TURN_SPEED + 100);
  
  delay(400);
  moveForward();
  delay(150);
  lastTurnTime = millis();
}

void stopMotors() {
  analogWrite(LEFT_RPWM, 0);
  analogWrite(LEFT_LPWM, 0);
  analogWrite(RIGHT_RPWM, 0);
  analogWrite(RIGHT_LPWM, 0);
}
