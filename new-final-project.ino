#include <AntaresESP32HTTP.h>
#include <Servo.h>

#define ACCESSKEY "your-access-key"
#define WIFISSID "your-wifi-ssid"
#define PASSWORD "your-wifi-password"

#define projectName "your-project-name"
#define deviceGarage "your-project-name"
#define deviceCar "your-project-name"

#define SENSOR_PIN 32

bool parking_status = false;
bool last_parking_status = false;
int data;
bool garage_status = false;
int pos = 0;

AntaresESP32HTTP antares(ACCESSKEY);
Servo myservo;

void setup()
{
	Serial.begin(115200);
	pinMode(SENSOR_PIN, INPUT);

	myservo.attach(12);
	myservo.write(0);

	antares.setDebug(true);
	antares.wifiConnection(WIFISSID, PASSWORD);
}

void loop()
{
	if (digitalRead(SENSOR_PIN) == 0)
	{
		data = 0;
		parking_status = true;
	}
	else if (digitalRead(SENSOR_PIN) == 1)
	{
		data = 1;
		parking_status = false;
	}

	if (parking_status != last_parking_status)
	{
		// send data
		antares.add("car", data);
		antares.send(projectName, deviceCar);

		last_parking_status = parking_status;
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
