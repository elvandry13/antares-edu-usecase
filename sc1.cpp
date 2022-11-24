/**
 * @file main.cpp
 * @author Elvandry Ghiffary (elvandry13@gmail.com)
 * @brief SMART CONVEYOR 1 - Kontrol Kecepatan
 * @version 0.1
 * @date 2022-08-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <Arduino.h>
#include <AntaresESP32HTTP.h>
#include "analogWrite.h"

#define ACCESSKEY "d20dbc9cea58054b:df24fbe0dc2d9332"
#define WIFISSID "AntaresEdu"
#define PASSWORD "antares123"

#define projectName "antares-edu"
#define deviceSensor "conveyor-sensor"
#define deviceMotor "conveyor-motor"

#define RPWM 13
#define LPWM 26
#define PWM 5

int pwm = 50;

AntaresESP32HTTP antares(ACCESSKEY);

void setup()
{
	Serial.begin(115200);

    // Motor driver pin setup
    pinMode(RPWM, OUTPUT);
    pinMode(LPWM, OUTPUT);
    pinMode(PWM, OUTPUT);

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
}
