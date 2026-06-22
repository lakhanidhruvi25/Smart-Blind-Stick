#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <SPI.h>
#include <LoRa.h>

#define SS 5
#define RST 14
#define DIO0 26

const char* ssid = "*****";
const char* password = "*****";

#define BOT_TOKEN "*********"
#define CHAT_ID "*********"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid,password);

  Serial.print("Connecting WiFi");

  while(WiFi.status()!=WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  client.setInsecure();

  Serial.println("\nWiFi Connected");

  LoRa.setPins(SS,RST,DIO0);

  if(!LoRa.begin(433E6))
  {
    Serial.println("LoRa init failed");
    while(1);
  }

  Serial.println("LoRa Receiver Ready");
}

void loop()
{
  int packetSize = LoRa.parsePacket();

  if(packetSize)
  {
    String data="";

    while(LoRa.available())
    {
      data += (char)LoRa.read();
    }

    Serial.println("SOS LOCATION RECEIVED:");
    Serial.println(data);

    String message = "🚨 SOS ALERT!\n\nBlind person needs help.\n\nLocation:\nhttps://maps.google.com/?q=" + data;

    bot.sendMessage(CHAT_ID, message, "");

    Serial.println("Location sent to phone");
  }
}
