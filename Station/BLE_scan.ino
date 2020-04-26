/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/
//header containing aws credentials
#include "certs.h"

//Libraries for bluetooth
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <string>
#include <BLEAddress.h>
#include <BLEUUID.h>

//Libraries for wifi
#include <WiFiClientSecure.h>
#include "WiFi.h"

//Libraries for json processing
#include <ArduinoJson.h>

//Libraries for MQTT
#include <MQTTClient.h>

#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

//address of object
String person1 = "";
const char *p1 = person1.c_str();
String person2 = "";
const char *p2 = person2.c_str();
String person3 = "";
const char *p3 = person3.c_str();

// wifi methods starts

// Wifi credentials
const char *WIFI_SSID = "SSID";
const char *WIFI_PASSWORD = "Password";

// The name of the device. This MUST match up with the name defined in the AWS console
#define DEVICE_NAME "station"

// The MQTTT endpoint for the device (unique for each AWS account but shared amongst devices within the account)
#define AWS_IOT_ENDPOINT ""

// The MQTT topic that this device should publish to
#define AWS_IOT_TOPIC "$aws/things/" DEVICE_NAME "/shadow/update"

// How many times we should attempt to connect to AWS
#define AWS_MAX_RECONNECT_TRIES 50

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

void connectToWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Only try 15 times to connect to the WiFi
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 15)
  {
    delay(500);
    Serial.print(".");
    retries++;
  }

  // If we still couldn't connect to the WiFi, go to deep sleep for a minute and try again.
  if (WiFi.status() != WL_CONNECTED)
  {
    esp_sleep_enable_timer_wakeup(1 * 60L * 1000000L);
    esp_deep_sleep_start();
  }
}

//AWS methods
void connectToAWS()
{
  // Configure WiFiClientSecure to use the AWS certificates we generated
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Try to connect to AWS and count how many times we retried.
  int retries = 0;
  Serial.print("Connecting to AWS IOT");

  while (!client.connect(DEVICE_NAME) && retries < AWS_MAX_RECONNECT_TRIES)
  {
    Serial.print(".");
    delay(100);
    retries++;
  }

  // Make sure that we did indeed successfully connect to the MQTT broker
  // If not we just end the function and wait for the next loop.
  if (!client.connected())
  {
    Serial.println(" Timeout!");
    return;
  }

  // If we land here, we have successfully connected to AWS!
  // And we can subscribe to topics and send messages.
  Serial.println("Connected!");
}

////aws methods ends

int scanTime = 5; //In seconds
BLEScan *pBLEScan;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{

  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    BLEAddress ad = advertisedDevice.getAddress();
    if ((strcmp(ad.toString().c_str(), p1) == 0) || (strcmp(ad.toString().c_str(), p2) == 0) || (strcmp(ad.toString().c_str(), p3) == 0))
    {
      StaticJsonDocument<128> jsonDoc;
      JsonObject stateObj = jsonDoc.createNestedObject("state");
      JsonObject reportedObj = stateObj.createNestedObject("reported");

      reportedObj["Server"] = "Server1";

      if ((strcmp(ad.toString().c_str(), p1) == 0))
      {
        Serial.printf("\nName: Person1");
        reportedObj["Name"] = "Person1";
      }

      if ((strcmp(ad.toString().c_str(), p2) == 0))
      {
        Serial.printf("\nName: Person2");
        reportedObj["Name"] = "Person2";
      }
      if ((strcmp(ad.toString().c_str(), p3) == 0))
      {
        Serial.printf("\nName: Person3");
        reportedObj["Name"] = "Person3";
      }

      if (advertisedDevice.haveServiceUUID())
      {
        BLEUUID bd = advertisedDevice.getServiceUUID();
        Serial.printf("\nUUID:\t%s", bd.toString().c_str());
        reportedObj["UUID"] = bd.toString().c_str();
      }

      if (advertisedDevice.haveManufacturerData())
      {
        char *pHex = BLEUtils::buildHexData(nullptr, (uint8_t *)advertisedDevice.getManufacturerData().data(), advertisedDevice.getManufacturerData().length());
        Serial.printf("\nManufacturer Data:\t%s", pHex);
        reportedObj["Manufacturer_Data"] = pHex;
      }

      Serial.printf("\nAddress:\t\t\t%s", ad.toString().c_str());
      reportedObj["Address"] = ad.toString().c_str();

      if (advertisedDevice.haveRSSI())
      {
        Serial.printf("\nRssi:\t\t\t\t%d", (int)advertisedDevice.getRSSI());
        reportedObj["RSSI"] = (int)advertisedDevice.getRSSI();
      }

      if (advertisedDevice.haveTXPower())
      {
        Serial.printf("\nTXPower:\t\t\t%d", (int)advertisedDevice.getTXPower());
        reportedObj["TXPower"] = (int)advertisedDevice.getTXPower();
      }

      Serial.println("\nPublishing message to AWS...");
      char jsonBuffer[512];
      serializeJson(jsonDoc, jsonBuffer);

      client.publish(AWS_IOT_TOPIC, jsonBuffer);

      Serial.printf("\n\n----------------------------------------------------------------------------------------------\n\n");
    }
  }
};

void setup()
{
  Serial.begin(115200);
  Serial.println("\nConnecting to WIFI");
  connectToWiFi();
  Serial.println("\nConnecting to AWS");
  connectToAWS();
  Serial.println("\nScanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); // less or equal setInterval value
}

void loop()
{
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("\nDevices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("\nScan done!");
  client.loop();
  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
}
