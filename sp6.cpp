/**
 * @file main.cpp
 * @author Elvandry Ghiffary (elvandry13@gmail.com)
 * @brief PINTU MANUAL - PINTU OTOMATIS - PENCURIAN
 * @version 0.1
 * @date 2022-08-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <Arduino.h>
#include <AntaresESP32HTTP.h>
#include <Servo.h>

#define ACCESSKEY "d20dbc9cea58054b:df24fbe0dc2d9332"
#define WIFISSID "AntaresEdu"
#define PASSWORD "antares123"

#define projectName "antares-edu"
#define deviceGarage "garage-status"
#define deviceCar "car-status"

#define SENSOR_PIN 32
#define BUZZER_PIN 14
#define TIMEOUT_MASUK 5000 // 5 detik
#define TIMEOUT_KELUAR 5000 // 5 detik

bool parking_status = false;
unsigned long durasi, start, stop;
int jumlah_mobil = 0;

bool garage_status = false;
int pos = 0;

AntaresESP32HTTP antares(ACCESSKEY);
Servo myservo;

void buzz(int dutycycle, int freq)
{
    ledcSetup(0, freq, 8);
    ledcAttachPin(BUZZER_PIN, 0);
    ledcWrite(0, dutycycle);
}

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
    pinMode(BUZZER_PIN, OUTPUT);

	myservo.attach(12);
	myservo.write(0);

	antares.setDebug(true);
	antares.wifiConnection(WIFISSID, PASSWORD);
}

void loop()
{
	unsigned long timeout_masuk = millis();
	while (digitalRead(SENSOR_PIN) == 1 && parking_status == false)
	{
		if ((millis() - timeout_masuk > TIMEOUT_MASUK) && (garage_status == true))
		{
			Serial.println("Mobil terdeteksi");
            start = millis();

			// send data
			antares.add("car", 1);
			antares.add("count", jumlah_mobil);
			antares.send(projectName, deviceCar);
			
			parking_status = true;

            // close garage
            antares.add("garasi", 0);
			antares.send(projectName, deviceGarage);
            Serial.println("Servo Closed");
			for (pos = 55; pos >= 0; pos -= 1)
			{
				myservo.write(pos);
				delay(4);
			}
            garage_status = false;
		}
	}

	unsigned long timeout_keluar = millis();
	while (digitalRead(SENSOR_PIN) == 0 && parking_status == true)
	{
		if ((millis() - timeout_keluar > TIMEOUT_KELUAR) && (garage_status == true))
		{
			Serial.println("Mobil tidak terdeteksi");
            stop = millis();

            // menghitung durasi
            durasi = stop - start;
			String readableTime = getReadableTime(durasi);
            Serial.println("Durasi : " + readableTime);

            // menghitung jumlah mobil
            jumlah_mobil++;
            Serial.println("Jumlah mobil : " + jumlah_mobil);

			// send data
			antares.add("car", 0);
            antares.add("time", readableTime);
            antares.add("count", jumlah_mobil);
			antares.send(projectName, deviceCar);
			
			parking_status = false;

            // close garage
			antares.add("garasi", 0);
			antares.send(projectName, deviceGarage);
            Serial.println("Servo Closed");
			for (pos = 55; pos >= 0; pos -= 1)
			{
				myservo.write(pos);
				delay(4);
			}
            garage_status = false;
		}

        else if ((millis() - timeout_keluar > TIMEOUT_KELUAR) && (garage_status == false))
        {
            Serial.println("Mobil dicuri");
            stop = millis();

            // menghitung durasi
            durasi = stop - start;
			String readableTime = getReadableTime(durasi);
            Serial.println("Durasi : " + readableTime);

            // menghitung jumlah mobil
            jumlah_mobil++;
            Serial.println("Jumlah mobil : " + jumlah_mobil);

			// send data
			antares.add("car", 2);
            antares.add("time", readableTime);
            antares.add("count", jumlah_mobil);
			antares.send(projectName, deviceCar);
			
			parking_status = false;

            // alarm
            for (int i=0; i < 10; i++)
            {
                buzz(125, 4000);
                delay(200);
                buzz(0, 0);
                delay(200);
            }
        }
	}

    // manual
	antares.get(projectName, deviceGarage);
	if (antares.getSuccess())
    {
        int status = antares.getInt("garasi");

        // Open garage when status == 1
        if (status == 1 && garage_status == false)
        {
            // open servo
			Serial.println("Servo Open");
			for (pos = 0; pos <= 55; pos += 1)
			{
				myservo.write(pos);
				delay(4);
			}

			garage_status = true;
        }

        // Close garage when status = 0
        if (status == 0 && garage_status == true)
        {
            // close servo
			Serial.println("Servo Closed");
			for (pos = 55; pos >= 0; pos -= 1)
			{
				myservo.write(pos);
				delay(4);
			}

			garage_status = false;
        }
    }
}
