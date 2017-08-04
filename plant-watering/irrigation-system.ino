/*
This is a sketch for an urban irrigation solution.
It uses the CuriePME to learn temperature data and classify the optimum plant-watering condition against it.
It then waters the plant at that time and relearns the data, cycling infinitely.

Please note that this sketch MUST BE RUN BEFORE 8 am of the target day to start the device.

Data sources:

"WeatherSpark.com." Average Weather in Vancouver, Canada, Year Round - Weather Spark. N.p., n.d. Web. 04 July 2017. <https://weatherspark.com/y/476/Average-Weather-in-Vancouver-Canada>.
"Just 4 Growers: Global Garden Community." Just For Growers. N.p., n.d. Web. 10 July 2017. <http://www.just4growers.com/stream/temperature-humidity-and-c02/understanding-the-optimum-temperature-for-plants.aspx>.
*/
// Libraries for use with this code. All of them except CuriePME can be found in the ide. CuriePME can be downloaded from the GitHub repository.
#include "CuriePME.h"
#include <CurieBLE.h>
#include <CurieTime.h>
#include <SerialFlash.h>
#include <SPI.h>
#include <LiquidCrystal.h>
#include <Servo.h>

// Servo pinout code.
Servo waterPipe;
// LCD pinout code.
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


// Global constants/variables.
#define thermometer  A3
#define rainSensor  1
#define humidity1  A4
#define humidity2  A1


int tm1;
int tm2;
int tm3;
int tm4;
int tm5;
int tm6;
int tm7;
int tm8;
int tm9;
int tm10;
int tm11;
int tm12;
int tm13;
int tm14;
int tm15;
int tm16;
int tm17;
int tm18;
int tm19;
int tm20;
int tm21;
int tm22;
int tm23;
int tm24;
int tm25;
int tm26;
int tm27;
int tm28;
int tm29;
int tm30;
int average;
int progav;
float voltage;
float temperatureC;
int tm;
int hourTime = -1;
int minuteTime = -1;
int dayTime = -1;
int monthTime = -1;
int confirmTime = -1;
// BLE service data.
BLEService plantService("19B10000-E8F2-537E-4F6C-D104768A1214");

// BLE characteristic, readable/writable by central.
BLEUnsignedCharCharacteristic timeCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);




void setup() {  // This runs once.
  waterPipe.attach(9);
  waterPipe.write(0);
  pinMode(rainSensor, INPUT);
  lcd.begin(16, 2); // Begin and clear the LCD.
  lcd.clear();
  if (!SerialFlash.begin(ONBOARD_FLASH_SPI_PORT, ONBOARD_FLASH_CS_PIN));
  lcd.setCursor(0, 0);
  lcd.print("Input Time: BLE");
  // Initialize the BLE service with name, service, characteristic, and characteristic value.
  BLE.begin();
  BLE.setLocalName("Arduino 101");
  BLE.setAdvertisedService(plantService);
  plantService.addCharacteristic(timeCharacteristic);
  BLE.addService(plantService);
  timeCharacteristic.setValue(0); // 0 until written by central.
  BLE.advertise();
  lcd.setCursor(0, 1);
  lcd.print("Waiting"); // Ready to connect.
x:
  BLEDevice central = BLE.central();

  if (central) { // If a device links to the board.
    lcd.setCursor(8, 1);
    lcd.print(" Done");
    delay(3000);
    while (central.connected()) { // While the device is still connected.

      if (timeCharacteristic.written()) {
        // Code that sequentially fills up all the time variable slots after each byte is sent from the device. Data is sent four times, plus one for confirmation of link.
        if (confirmTime == -1) {
          confirmTime = timeCharacteristic.value();
        }
        else if (hourTime == -1) {
          hourTime = timeCharacteristic.value();
        }
        else if (minuteTime == -1)
        {
          minuteTime = timeCharacteristic.value();
        }
        else if (dayTime == -1)
        {
          dayTime = timeCharacteristic.value();
        }
        else if (monthTime == -1)
        {
          monthTime = timeCharacteristic.value();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Time set.");
          lcd.setCursor(0, 1);
          lcd.print("Disconnect device.");

        }

      }
    }
  }
  else
  {
    goto x; // Loop back if a device isn't connected yet.
  }
  // Set the time with the collected variables. Seconds and year do not matter for this system.
  setTime(hourTime, minuteTime, 00, dayTime, monthTime, 2017);

  // Initialize the PME.
  lcd.clear();
  CuriePME.begin();
  lcd.setCursor(0, 0);
  lcd.print("Checking for save.");
  delay(3000);
  const char *filename = "NeurData.dat";
  if (check_if_exists(filename) == true)
  {
    restoreNetworkKnowledge();
    lcd.setCursor(0, 1);
    lcd.print("Found!");
    delay(3000);
    lcd.clear();
    goto z;
  }
  else
  {
    lcd.setCursor(0, 1);
    lcd.print("Not found!");
    delay(3000);
    lcd.clear();
  }
  /*
    The remaining portion of setup collects temperature data over the course of the day.
    It makes 30 checks per hour, spaced at 2 minute intervals, and at the end of the hour it takes an average of the hour's data
    by finding the arithmetic mean of all the data except for the first scan.
    This mean is then put into the PME and learned.
    This repeats from 8 am to 9 pm, at which point the full dataset learned by the PME is saved into the CurieFlash memory as a default
    for if and when the device loses power or needs to be restarted.
  */
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Learning");
  for (int i = 8; i < 22; i++) // Repeat this 14 times, each saving under a different category.
  {
    // Collect 30 pieces of temperature data, saved as an int mapped between 0 and 255.
    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm1 = constrain(tm, 0, 255);
    delay(114000); // Compensate for check delay above.

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm2 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm3 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm4 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm5 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm6 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm7 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm8 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm9 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm10 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm11 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm12 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm13 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm14 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm15 = constrain(tm, 0, 255);
    // During the first learning period, plant watering will be done at the peak of the temperatures for that month.
    if ((month() == 1 || month() == 2 || month() == 11 || month() == 12) && hour() == 12)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Running..."); // Water the plant if it is not raining and soil moisture is low enough.
      int readRain = digitalRead(rainSensor);
      if (readRain == HIGH)
      {
        lcd.setCursor(0, 1);
        lcd.print("Raining right now.");
        goto a2;
      }
      // Uncomment to use moisture sensors.
      //else if (analogRead(humidity1) > 400 || analogRead(humidity2) > 400) // Adjust these to match your soil.
      //{
      //lcd.setCursor(0, 1);
      //lcd.print("Soil too moist.");
      //goto a2;
      //}
      else
      {
        waterPipe.write(135);
        delay(7000); // Water for 7 seconds.
        waterPipe.write(0);
      }
      lcd.setCursor(0, 1);
      lcd.print("Done");
a2:
      delay(3000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Learning");
    }
    else if ((month() == 3 || month() == 4 || month() == 10) && hour() == 14)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Running...");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Running..."); // Water the plant if it is not raining and soil moisture is low enough.
      int readRain = digitalRead(rainSensor);
      if (readRain == HIGH)
      {
        lcd.setCursor(0, 1);
        lcd.print("Raining right now.");
        goto a31;
      }
      // Uncomment to use moisture sensors.
      //else if (analogRead(humidity1) > 400 || analogRead(humidity2) > 400) // Adjust these to match your soil.
      //{
      //lcd.setCursor(0, 1);
      //lcd.print("Soil too moist.");
      //goto a31;
      //}
      else
      {
        waterPipe.write(135);
        delay(7000); // Water for 7 seconds.
        waterPipe.write(0);
      }
      lcd.setCursor(0, 1);
      lcd.print("Done");
a31:
      delay(3000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Learning");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Learning");
    }
    else if ((month() == 5 || month() == 6 || month() == 9) && hour() == 15)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Running...");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Running..."); // Water the plant if it is not raining and soil moisture is low enough.
      int readRain = digitalRead(rainSensor);
      if (readRain == HIGH)
      {
        lcd.setCursor(0, 1);
        lcd.print("Raining right now.");
        goto a3;
      }
      // Uncomment to use moisture sensors.
      //else if (analogRead(humidity1) > 400 || analogRead(humidity2) > 400) // Adjust these to match your soil.
      //{
      //lcd.setCursor(0, 1);
      //lcd.print("Soil too moist.");
      //goto a3;
      //}
      else
      {
        waterPipe.write(135);
        delay(7000); // Water for 7 seconds.
        waterPipe.write(0);
      }
      lcd.setCursor(0, 1);
      lcd.print("Done");
a3:
      delay(3000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Learning");
    }
    else if ((month() == 7 || month() == 8) && hour() == 16)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Running...");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Running..."); // Water the plant if it is not raining and soil moisture is low enough.
      int readRain = digitalRead(rainSensor);
      if (readRain == HIGH)
      {
        lcd.setCursor(0, 1);
        lcd.print("Raining right now.");
        goto a4;
      }
      // Uncomment to use moisture sensors.
      //else if (analogRead(humidity1) > 400 || analogRead(humidity2) > 400) // Adjust these to match your soil.
      //{
      //lcd.setCursor(0, 1);
      //lcd.print("Soil too moist.");
      //goto a4;
      //}
      else
      {
        waterPipe.write(135);
        delay(7000); // Water for 7 seconds.
        waterPipe.write(0);
      }
      lcd.setCursor(0, 1);
      lcd.print("Done");
a4:
      delay(3000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Learning");
    }

    delay(110000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm16 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm17 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm18 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm19 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm20 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm21 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm22 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm23 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm24 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm25 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm26 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm27 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm28 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm29 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm30 = constrain(tm, 0, 255);
    delay(120000);

    // Reconvert the data to the temperature sensor's specifications.
    tm1 = map(tm1, 0, 255, -40, 125);
    tm1 = constrain(tm1, -40, 125);
    tm2 = map(tm2, 0, 255, -40, 125);
    tm2 = constrain(tm2, -40, 125);
    tm3 = map(tm3, 0, 255, -40, 125);
    tm3 = constrain(tm3, -40, 125);
    tm4 = map(tm4, 0, 255, -40, 125);
    tm4 = constrain(tm4, -40, 125);
    tm5 = map(tm5, 0, 255, -40, 125);
    tm5 = constrain(tm5, -40, 125);
    tm6 = map(tm6, 0, 255, -40, 125);
    tm6 = constrain(tm6, -40, 125);
    tm7 = map(tm7, 0, 255, -40, 125);
    tm7 = constrain(tm7, -40, 125);
    tm8 = map(tm8, 0, 255, -40, 125);
    tm8 = constrain(tm8, -40, 125);
    tm9 = map(tm9, 0, 255, -40, 125);
    tm9 = constrain(tm9, -40, 125);
    tm10 = map(tm10, 0, 255, -40, 125);
    tm10 = constrain(tm10, -40, 125);
    tm11 = map(tm11, 0, 255, -40, 125);
    tm11 = constrain(tm11, -40, 125);
    tm12 = map(tm12, 0, 255, -40, 125);
    tm12 = constrain(tm12, -40, 125);
    tm13 = map(tm13, 0, 255, -40, 125);
    tm13 = constrain(tm13, -40, 125);
    tm14 = map(tm14, 0, 255, -40, 125);
    tm14 = constrain(tm14, -40, 125);
    tm15 = map(tm15, 0, 255, -40, 125);
    tm15 = constrain(tm15, -40, 125);
    tm16 = map(tm16, 0, 255, -40, 125);
    tm16 = constrain(tm16, -40, 125);
    tm17 = map(tm17, 0, 255, -40, 125);
    tm17 = constrain(tm17, -40, 125);
    tm18 = map(tm18, 0, 255, -40, 125);
    tm18 = constrain(tm18, -40, 125);
    tm19 = map(tm19, 0, 255, -40, 125);
    tm19 = constrain(tm19, -40, 125);
    tm20 = map(tm20, 0, 255, -40, 125);
    tm20 = constrain(tm20, -40, 125);
    tm21 = map(tm21, 0, 255, -40, 125);
    tm21 = constrain(tm21, -40, 125);
    tm22 = map(tm22, 0, 255, -40, 125);
    tm22 = constrain(tm22, -40, 125);
    tm23 = map(tm23, 0, 255, -40, 125);
    tm23 = constrain(tm23, -40, 125);
    tm24 = map(tm24, 0, 255, -40, 125);
    tm24 = constrain(tm24, -40, 125);
    tm25 = map(tm25, 0, 255, -40, 125);
    tm25 = constrain(tm25, -40, 125);
    tm26 = map(tm26, 0, 255, -40, 125);
    tm26 = constrain(tm26, -40, 125);
    tm27 = map(tm27, 0, 255, -40, 125);
    tm27 = constrain(tm27, -40, 125);
    tm28 = map(tm28, 0, 255, -40, 125);
    tm28 = constrain(tm28, -40, 125);
    tm29 = map(tm29, 0, 255, -40, 125);
    tm29 = constrain(tm29, -40, 125);
    tm30 = map(tm30, 0, 255, -40, 125);
    tm30 = constrain(tm30, -40, 125);
    // Find the arithmetic mean and commit it to memory.
    average = (tm2 + tm3 + tm4 + tm5 + tm6 + tm7 + tm8 + tm9 + tm10 + tm11 + tm12 + tm13 + tm14 + tm15 + tm16 + tm17 + tm18 + tm19 + tm20 + tm21 + tm22 + tm23 + tm24 + tm25 + tm26 + tm27 + tm28 + tm29 + tm30) / 29;
    commitSample(i, average);

  }
  saveNetworkKnowledge(); // Save this new data to flash memory.
z:
  delay(1);
}
void commitSample (int category, uint8_t s1)
{
  // Commit to memory a single vector (the average), along with the category, which represents the hour of that data.
  uint8_t vector[1];
  vector[0] = s1;
  CuriePME.learn(vector, 1, category);
}
void loop() { // Infinitely repeats.
  /*
    This code attempts to classify the optimum temperature for watering plants, 25 Celsius, among the data learned by the PME.
    If it can classify the data, the returned category becomes the hour at which the plant will be watered.
    The data is then erased and relearned, which infinitely repeats.
    IF IT CANNOT CLASSIFY THE DATA, it will take the monthly defaults from earlier in the sketch.
  */
  uint8_t vector[1];
  vector[0] = 25;
  int answer = CuriePME.classify(vector, 1 );
  if (answer == CuriePME.noMatch)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("NO MATCHES!");
    if (month() == 1 || month() == 2 || month() == 11 || month() == 12)
    {
      answer = 12;
    }
    else if (month() == 3 || month() == 4 || month() == 10)
    {
      answer = 14;
    }
    else if (month() == 5 || month() == 6 || month() == 9)
    {
      answer = 15;
    }
    else if (month() == 7 || month() == 8)
    {
      answer = 16;
    }
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Category:");
    lcd.setCursor(0, 1);
    lcd.print(answer);
  }
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Optimization Done");
  delay(3000);
  CuriePME.forget(); // Erase and relearn. This does not erase the flash memory.
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waiting");
  while (hour() != 8);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Learning");
  // Learn the data again.
  for (int i = 8; i < 22; i++)
  {

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm1 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm2 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm3 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm4 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm5 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm6 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm7 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm8 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm9 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm10 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm11 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm12 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm13 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm14 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm15 = constrain(tm, 0, 255);
    // The time in the day at which to water the plant, as determined by the PME.
    if (hour() == answer)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Running..."); // Water the plant if it is not raining and soil moisture is low enough.
      int readRain = digitalRead(rainSensor);
      if (readRain == HIGH)
      {
        lcd.setCursor(0, 1);
        lcd.print("Raining right now.");
        goto a5;
      }
      // Uncomment to use moisture sensors.
      //else if (analogRead(humidity1) > 400 || analogRead(humidity2) > 400) // Adjust these to match your soil.
      //{
      //  lcd.setCursor(0, 1);
      //  lcd.print("Soil too moist.");
      //  goto a5;
      //}
      else
      {
        waterPipe.write(135);
        delay(7000); // Water for 7 seconds.
        waterPipe.write(0);
      }
      lcd.setCursor(0, 1);
      lcd.print("Done");
a5:
      delay(3000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Learning");
    }

    delay(110000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm16 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm17 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm18 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm19 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm20 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm21 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm22 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm23 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm24 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm25 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm26 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm27 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm28 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm29 = constrain(tm, 0, 255);
    delay(120000);

    voltage = analogRead(thermometer) * 3.3;
    voltage /= 1024.0;
    temperatureC = (voltage - 0.5) * 100 ;
    tm = map(temperatureC, -40, 125, 0, 255);
    tm30 = constrain(tm, 0, 255);
    delay(120000);


    tm1 = map(tm1, 0, 255, -40, 125);
    tm1 = constrain(tm1, -40, 125);
    tm2 = map(tm2, 0, 255, -40, 125);
    tm2 = constrain(tm2, -40, 125);
    tm3 = map(tm3, 0, 255, -40, 125);
    tm3 = constrain(tm3, -40, 125);
    tm4 = map(tm4, 0, 255, -40, 125);
    tm4 = constrain(tm4, -40, 125);
    tm5 = map(tm5, 0, 255, -40, 125);
    tm5 = constrain(tm5, -40, 125);
    tm6 = map(tm6, 0, 255, -40, 125);
    tm6 = constrain(tm6, -40, 125);
    tm7 = map(tm7, 0, 255, -40, 125);
    tm7 = constrain(tm7, -40, 125);
    tm8 = map(tm8, 0, 255, -40, 125);
    tm8 = constrain(tm8, -40, 125);
    tm9 = map(tm9, 0, 255, -40, 125);
    tm9 = constrain(tm9, -40, 125);
    tm10 = map(tm10, 0, 255, -40, 125);
    tm10 = constrain(tm10, -40, 125);
    tm11 = map(tm11, 0, 255, -40, 125);
    tm11 = constrain(tm11, -40, 125);
    tm12 = map(tm12, 0, 255, -40, 125);
    tm12 = constrain(tm12, -40, 125);
    tm13 = map(tm13, 0, 255, -40, 125);
    tm13 = constrain(tm13, -40, 125);
    tm14 = map(tm14, 0, 255, -40, 125);
    tm14 = constrain(tm14, -40, 125);
    tm15 = map(tm15, 0, 255, -40, 125);
    tm15 = constrain(tm15, -40, 125);
    tm16 = map(tm16, 0, 255, -40, 125);
    tm16 = constrain(tm16, -40, 125);
    tm17 = map(tm17, 0, 255, -40, 125);
    tm17 = constrain(tm17, -40, 125);
    tm18 = map(tm18, 0, 255, -40, 125);
    tm18 = constrain(tm18, -40, 125);
    tm19 = map(tm19, 0, 255, -40, 125);
    tm19 = constrain(tm19, -40, 125);
    tm20 = map(tm20, 0, 255, -40, 125);
    tm20 = constrain(tm20, -40, 125);
    tm21 = map(tm21, 0, 255, -40, 125);
    tm21 = constrain(tm21, -40, 125);
    tm22 = map(tm22, 0, 255, -40, 125);
    tm22 = constrain(tm22, -40, 125);
    tm23 = map(tm23, 0, 255, -40, 125);
    tm23 = constrain(tm23, -40, 125);
    tm24 = map(tm24, 0, 255, -40, 125);
    tm24 = constrain(tm24, -40, 125);
    tm25 = map(tm25, 0, 255, -40, 125);
    tm25 = constrain(tm25, -40, 125);
    tm26 = map(tm26, 0, 255, -40, 125);
    tm26 = constrain(tm26, -40, 125);
    tm27 = map(tm27, 0, 255, -40, 125);
    tm27 = constrain(tm27, -40, 125);
    tm28 = map(tm28, 0, 255, -40, 125);
    tm28 = constrain(tm28, -40, 125);
    tm29 = map(tm29, 0, 255, -40, 125);
    tm29 = constrain(tm29, -40, 125);
    tm30 = map(tm30, 0, 255, -40, 125);
    tm30 = constrain(tm30, -40, 125);
    average = (tm2 + tm3 + tm4 + tm5 + tm6 + tm7 + tm8 + tm9 + tm10 + tm11 + tm12 + tm13 + tm14 + tm15 + tm16 + tm17 + tm18 + tm19 + tm20 + tm21 + tm22 + tm23 + tm24 + tm25 + tm26 + tm27 + tm28 + tm29 + tm30) / 29;
    progav = (tm30 - tm2) / 2;
    commitSample(i, average);

  }


}
/*
  A quick note on the flash memory data:
  The data will only ever be saved once; that is, it cannot be changed with this code.
  As a result, if the device loses power, IT WILL DEFAULT TO THE SETTINGS OF THE FIRST TIME IT WAS USED WHEN IT IS REACTIVATED,
  even if the monthly averages have changed.
  Keeping the device on an extra day will allow it to software obtain new averages, but the flash memory will stay the same.
  To erase flash completely, upload "EraseEverything" from "CurieSerialFlash" in the IDE.
  Then reupload this sketch to save new averages to the flash memory.
*/

void saveNetworkKnowledge()  // Code for saving to flash memory.
{
  const char *filename = "NeurData.dat";
  SerialFlashFile file;

  Intel_PMT::neuronData neuronData;
  uint32_t fileSize = 128 * sizeof(neuronData);

  create_if_not_exists( filename, fileSize );
  // Open the file and write test data
  file = SerialFlash.open(filename);
  file.erase();

  CuriePME.beginSaveMode();
  if (file) {
    // iterate over the network and save the data.
    while ( uint16_t nCount = CuriePME.iterateNeuronsToSave(neuronData)) {
      if ( nCount == 0x7FFF)
        break;

      uint16_t neuronFields[4];

      neuronFields[0] = neuronData.context;
      neuronFields[1] = neuronData.influence;
      neuronFields[2] = neuronData.minInfluence;
      neuronFields[3] = neuronData.category;

      file.write( (void*) neuronFields, 8);
      file.write( (void*) neuronData.vector, 128 );
    }
  }

  CuriePME.endSaveMode();
}


bool create_if_not_exists (const char *filename, uint32_t fileSize) {
  if (!SerialFlash.exists(filename)) {
    return SerialFlash.createErasable(filename, fileSize);
  }
  return true;
}

bool check_if_exists (const char *filename) {
  if (!SerialFlash.exists(filename)) {
    return false;
  }
  else
  {
    return true;
  }
}

void restoreNetworkKnowledge ()  // Code that restores the originally saved temperature defaults.
{
  const char *filename = "NeurData.dat";
  SerialFlashFile file;
  int32_t fileNeuronCount = 0;

  Intel_PMT::neuronData neuronData;

  file = SerialFlash.open(filename);

  CuriePME.beginRestoreMode();
  if (file) {
    while (1) {


      uint16_t neuronFields[4];
      file.read( (void*) neuronFields, 8);
      file.read( (void*) neuronData.vector, 128 );

      neuronData.context = neuronFields[0] ;
      neuronData.influence = neuronFields[1] ;
      neuronData.minInfluence = neuronFields[2] ;
      neuronData.category = neuronFields[3];

      if (neuronFields[0] == 0 || neuronFields[0] > 127)
        break;

      fileNeuronCount++;
      CuriePME.iterateNeuronsToRestore( neuronData );
    }
  }

  CuriePME.endRestoreMode();
}
