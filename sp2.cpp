/**
 * @file main.cpp
 * @author Elvandry Ghiffary (elvandry13@gmail.com)
 * @brief TIMEOUT-DURASI
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
unsigned long durasi, start, stop;

AntaresESP32HTTP antares(ACCESSKEY);

String getReadableTime(unsigned long currentMillis)
{
    String readableTime = "";
    
    unsigned long seconds;
    unsigned long minutes;
    unsigned long hours;
    unsigned long days;

    // currentMillis = millis();
    seconds = currentMillis / 1000;
    minutes = seconds / 60;
    hours = minutes / 60;
    days = hours / 24;
    currentMillis %= 1000;
    seconds %= 60;
    minutes %= 60;
    hours %= 24;

    if (days > 0)
    {
        readableTime = String(days) + " ";
    }

    if (hours > 0)
    {
        readableTime += String(hours) + ":";
    }

    if (minutes < 10)
    {
        readableTime += "0";
    }
    readableTime += String(minutes) + "m ";

    if (seconds < 10)
    {
        readableTime += "0";
    }
    readableTime += String(seconds) + "s";

    return readableTime;
}

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
            start = millis();

			// send data
			antares.add("car", 1);
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
            stop = millis();

            durasi = stop - start;
			String readableTime = getReadableTime(durasi);
			Serial.println("Durasi : " + readableTime);

			// send data
			antares.add("car", 0);
            antares.add("time", readableTime);
			antares.send(projectName, deviceName);
			
			parking_status = false;
		}
	}
}
