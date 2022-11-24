/**
 * @file main.cpp
 * @author Elvandry Ghiffary (elvandry13@gmail.com)
 * @brief SMART CONVEYOR 3 - Kontrol kecepatan + counter + filter
 * @version 0.1
 * @date 2022-08-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <Arduino.h>
#include <AntaresESP32HTTP.h>
#include "analogWrite.h"
#include <Servo.h>

#define ACCESSKEY "d20dbc9cea58054b:df24fbe0dc2d9332"
#define WIFISSID "AntaresEdu"
#define PASSWORD "antares123"

#define projectName "antares-edu"
#define deviceSensor "conveyor-sensor"
#define deviceMotor "conveyor-motor"

#define RPWM 13
#define LPWM 26
#define PWM 5

#define SENSOR1_PIN 32
#define SENSOR2_PIN 33

int pwm = 50;
int counter1 = 0;
int counter2 = 0;
unsigned long start_timeout;
bool send_data = false;

AntaresESP32HTTP antares(ACCESSKEY);
Servo myservo;

void setup()
{
	Serial.begin(115200);

    // Motor driver pin setup
    pinMode(RPWM, OUTPUT);
    pinMode(LPWM, OUTPUT);
    pinMode(PWM, OUTPUT);

    // Proximity pin setup
    pinMode(SENSOR1_PIN, INPUT);
    pinMode(SENSOR2_PIN, INPUT);

    // Servo setup
    myservo.attach(12);
	myservo.write(0);

    // Antares wifi setup
	antares.setDebug(true);
	antares.wifiConnection(WIFISSID, PASSWORD);
}

void loop()
{
	// Receive data from Antares
    antares.get(projectName, deviceMotor);
    if (antares.getSuccess())
    {
        pwm = antares.getInt("pwm");

        Serial.println("Set PWM to " + String(pwm));
    }

    // Turn ON Motor DC
    digitalWrite(PWM, HIGH);
    analogWrite(RPWM, pwm);
    analogWrite(LPWM, 0);

    // Detect object & counting
    while (digitalRead(SENSOR1_PIN) == 1)
    {
        if (digitalRead(SENSOR1_PIN) == 0)
        {
            start_timeout = millis();
            send_data = true;
            counter1++;
            Serial.println("Counter 1 : " + String(counter1));

            myservo.write(0);

            break;
        }
    }

    while (digitalRead(SENSOR2_PIN) == 1)
    {
        if (digitalRead(SENSOR2_PIN) == 0)
        {
            start_timeout = millis();
            send_data = true;
            counter2++;
            Serial.println("Counter 2 : " + String(counter2));

            myservo.write(75);

            break;
        }
    }

    // Send data after 5s no detection
    if (millis() - start_timeout > 5000 && send_data == true)
    {
        antares.add("passed_object", counter1);
        antares.add("filtered_object", counter2);
        antares.send(projectName, deviceSensor);

        send_data = false;
    }
}
