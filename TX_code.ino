#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <SPI.h>
#include <LoRa.h>

TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

#define BUTTON 4

#define SS 5
#define RST 14
#define DIO0 26
#define WATER_SENSOR 34
#define TRIG 25
#define ECHO 33
#define MOTOR 27
#define MOTOR_PIN 32
#define BUZZER 15

#define PWM_CHANNEL 0
#define PWM_FREQ 2000
#define PWM_RES 8

void setup()
{
  Serial.begin(115200);

  pinMode(BUTTON, INPUT_PULLUP);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);

  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES);
  ledcAttachPin(MOTOR, PWM_CHANNEL);

  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);

  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(433E6))
  {
    Serial.println("LoRa init failed");
    while (1);
  }

  Serial.println("System Ready");
}

void loop()
{
  while (gpsSerial.available())
  {
    gps.encode(gpsSerial.read());
  }

  if (digitalRead(BUTTON) == LOW)
  {
    if (gps.location.isValid())
    {
      float lat = gps.location.lat();
      float lon = gps.location.lng();

      Serial.println("SOS Button Pressed!");

      Serial.print("Latitude: ");
      Serial.println(lat, 6);

      Serial.print("Longitude: ");
      Serial.println(lon, 6);

      LoRa.beginPacket();
      LoRa.print(lat, 6);
      LoRa.print(",");
      LoRa.print(lon, 6);
      LoRa.endPacket();

      Serial.println("Location Sent");

      delay(5000);
    }
  }

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000);
  float distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.println(distance);

  int waterValue = analogRead(WATER_SENSOR);

  Serial.print("Water Value: ");
  Serial.println(waterValue);

  if (distance <= 10 && waterValue > 1500)
  {
    ledcWrite(PWM_CHANNEL, 255);
    digitalWrite(MOTOR_PIN, HIGH);

    digitalWrite(BUZZER, HIGH);
    delay(10);
    digitalWrite(BUZZER, LOW);
    delay(10);
  }

  else if ((distance > 80 || distance <= 0) && waterValue > 1500)
  {
    ledcWrite(PWM_CHANNEL, 0);
    digitalWrite(MOTOR_PIN, HIGH);

    digitalWrite(BUZZER, HIGH);
    delay(10);
    digitalWrite(BUZZER, LOW);
    delay(10);
  }

  else if ((distance > 50 && distance <= 80) && waterValue > 1500)
  {
    ledcWrite(PWM_CHANNEL, 80);
    digitalWrite(MOTOR_PIN, HIGH);

    digitalWrite(BUZZER, HIGH);
    delay(10);
    digitalWrite(BUZZER, LOW);
    delay(10);
  }

  else if ((distance > 25 && distance <= 50) && waterValue > 1500)
  {
    ledcWrite(PWM_CHANNEL, 150);
    digitalWrite(MOTOR_PIN, HIGH);

    digitalWrite(BUZZER, HIGH);
    delay(10);
    digitalWrite(BUZZER, LOW);
    delay(10);
  }

  else if (distance <= 25 && waterValue > 1500)
  {
    ledcWrite(PWM_CHANNEL, 225);
    digitalWrite(MOTOR_PIN, HIGH);

    digitalWrite(BUZZER, HIGH);
    delay(10);
    digitalWrite(BUZZER, LOW);
    delay(10);
  }

  else if (waterValue > 1500)
  {
    ledcWrite(PWM_CHANNEL, 0);
    digitalWrite(MOTOR_PIN, HIGH);

    digitalWrite(BUZZER, HIGH);
    delay(10);
    digitalWrite(BUZZER, LOW);
    delay(10);
  }

  else if (distance > 80 || distance <= 0)
  {
    ledcWrite(PWM_CHANNEL, 0);
    digitalWrite(BUZZER, LOW);
    digitalWrite(MOTOR_PIN, LOW);
  }

  else if (distance > 50 && distance <= 80)
  {
    ledcWrite(PWM_CHANNEL, 80);
    digitalWrite(BUZZER, LOW);
    digitalWrite(MOTOR_PIN, LOW);
  }

  else if (distance > 25 && distance <= 50)
  {
    ledcWrite(PWM_CHANNEL, 150);
    digitalWrite(BUZZER, LOW);
    digitalWrite(MOTOR_PIN, LOW);
  }

  else if (distance <= 10)
  {
    ledcWrite(PWM_CHANNEL, 255);

    digitalWrite(BUZZER, HIGH);
    delay(10);
    digitalWrite(BUZZER, LOW);
    delay(10);

    digitalWrite(MOTOR_PIN, LOW);
  }

  else if (distance <= 25)
  {
    ledcWrite(PWM_CHANNEL, 225);
    digitalWrite(MOTOR_PIN, LOW);
  }

  delay(100);
}