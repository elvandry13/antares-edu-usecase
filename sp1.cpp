/**
 * @file main.cpp
 * @author Elvandry Ghiffary (elvandry13@gmail.com)
 * @brief TIMEOUT
 * @version 0.1
 * @date 2022-08-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <Arduino.h>
#include <AntaresESP32HTTP.h>

#define ACCESSKEY "your-access-key"
#define WIFISSID "your-wifi-ssid"
#define PASSWORD "your-wifi-password"

#define projectName "your-project-name"
#define deviceName "your-project-name"

#define SENSOR_PIN 32
#define TIMEOUT_MASUK 5000 // 5 detik
#define TIMEOUT_KELUAR 5000 // 5 detik

bool parking_status = false;

AntaresESP32HTTP antares(ACCESSKEY);

void setup()
{
	Serial.begin(115200);
	pinMode(SENSOR_PIN, INPUT);

	antares.setDebug(true);
	antares.wifiConnection(WIFISSID, PASSWORD);
}

void loop()
{
	unsigned long timeout_masuk = millis();
	while (digitalRead(SENSOR_PIN) == 1 && parking_status == false)
	{
		if (millis() - timeout_masuk > TIMEOUT_MASUK)
		{
			Serial.println("Mobil terdeteksi");
			// send data
			antares.add("status", 1);
			antares.send(projectName, deviceName);
			
			parking_status = true;
		}
	}

	unsigned long timeout_keluar = millis();
	while (digitalRead(SENSOR_PIN) == 0 && parking_status == true)
	{
		if (millis() - timeout_keluar > TIMEOUT_KELUAR)
		{
			Serial.println("Mobil tidak terdeteksi");
			// send data
			antares.add("status", 0);
			antares.send(projectName, deviceName);
			
			parking_status = false;
		}
	}
}
