/* Meteorological Unit - Temp, relative humidity, pressure and air density
 * 
 * V 1.4.0 - replaced 7 segments LCD by I2C 16 char x 2 lines (1602A I2C) LCD.
 * V 1.3.1 - Integrated Real Time clock and SD Card slot for data logging.
 *  Fixed 1s sampling time based on RTC clock for precise tame stamping.

*Created by Filipe Brandao Using
*Sparkfun GY-BME280 Library
*LiquidCrystal_I2C
*DS3231 RTC Library
*SD Arduino Library
*Using examples from Adafruit and Sparkfun Libraries.
*
   The GY-BME280 is using address 0x76 (jumper closed)

  Hardware connections:
  
  BME280 -> Arduino
  GND -> GND
  3.3 -> 3.3
  SDA -> A4
  SCL -> A5

  LCD 1602A I2C
  LCD   -> Arduino
  VCC   -> VCC
  GND   -> GND
  SDA   -> A4
  SCK   -> A5

  DS3231 RTC
  RTC    Arduino
  SCL -> A5
  SDA -> A4
  VCC -> 5V
  GND -> GND 

 SD Card reader
 PCB    Arduino
 CS   -> D10
 SCK  -> D13
 MOSI -> D11
 MISO -> D12
 VCC  -> 5V
 GND  -> GND

  
 REQUIRES the following Arduino libraries:
 - https://github.com/sparkfun/SparkFun_BME280_Arduino_Library
 - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor
 - Real time clock DS3231: https://github.com/jarzebski/Arduino-DS3231
 - SD Card Reader: https://github.com/arduino-libraries/SD
 - LCD 1602A: https://github.com/Freenove/Freenove_LCD1602_Starter_Kit/archive/master.zip
*/

#include <Adafruit_Sensor.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "SparkFunBME280.h"
#include <DS3231.h>
#include <SPI.h>
#include <SD.h>

//used to select chipset for type of SD reader. Adafruit=10
const int chipSelect = 10;

//delay for serial print, update display and write output file
uint32_t delayMS=1000;

//create an LCD object -  using address 0x27
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//create sensor object (BOSH BME 280 combined sensor)
BME280 mySensor;

//initialize clock and create RTC object:
DS3231 clock;
RTCDateTime rtc;

void setup() {
  
  lcd.init();
  lcd.backlight();

  //test lcd with initial message
  testLcd();
  delay(delayMS);
  
  Serial.begin(9600);
  Serial.println("System started.");
  
  Wire.begin();
  mySensor.setI2CAddress(0x76); //Connect to sensor
  if(mySensor.beginI2C() == false) {
      Serial.println("Sensor connection failed!");
      lcd.setCursor(0, 0);
      lcd.print("Sensor connection");
      lcd.setCursor(0, 1);
      lcd.print("connection FAIL");
      delay(1000);
      //Display an error message on the LCD too
      while(1){}
    }

  Serial.println("Sensor connection OK!");
      lcd.setCursor(0, 0);
      lcd.print("Sensor");
      lcd.setCursor(0, 1);
      lcd.print("connection OK");
      delay(2000);
  // Initialize DS3231 RTC
  Serial.println("Initialize Real Time Clock DS3231");
  clock.begin();

  
  // Set sketch compiling time - please uncomment before first clock sync and recompile/upload
  //to Arduino. Afterwards, do comment. Otherwise, clock will be always reset
  //to the sketch compiling date/time.
  
  //clock.setDateTime(__DATE__, __TIME__);

  //Card reader setup
  if (!SD.begin(chipSelect)) {
    Serial.println("SD Card failed, or not present");
    lcd.setCursor(0, 0);
    lcd.print("SD Card failed");
    lcd.setCursor(0, 1);
    lcd.print("or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("SD Card read successfully.");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SD Card read");
  lcd.setCursor(0, 1);
  lcd.print("successfully");
  delay(2000);
  lcd.clear();
  //write header for data stored:
  File dataFile = SD.open("data.txt", FILE_WRITE);
  dataFile.println("Date Time Temp RelHumid Pressure Density");
  dataFile.close();
  Serial.println("Date Time Temp RelHumid Pressure Density");  
}


void loop() {

  //created a way to alternate display values and record timestamp
  //in the SD card every second. refresh LCD in the even seconds.
  int counter=0;
  int second=clock.getDateTime().second;

while (counter < 8){
  if (second !=clock.getDateTime().second){
    sendSerial();
    sdWrite();
    if(counter<5){
    refreshLCD();
    }
    else{
    showDateTime(); 
    }   
    counter++;
    second=clock.getDateTime().second;
    }   
  }
}

void refreshLCD(){

  //shows temp in ºC
  displayTemp(mySensor.readTempC());
    
  //shows pressure in hPa (millibar)
  displayPressure(mySensor.readFloatPressure()/100.0);

  //shows humidity in %
  displayHumid(mySensor.readFloatHumidity());
    
  //shows air density in g/L
  displayAirDensity();
    
}

void displayTemp(float temp){
  //shows temperature in C°
    lcd.setCursor(0,0);
    lcd.print(temp);
    lcd.write(0xDF);
    lcd.print("C");
    lcd.print("         ");
}

void displayPressure(float pressure){
  lcd.setCursor(9,0);
  lcd.print(pressure);
  lcd.setCursor(13,0);
  lcd.print("hPa");

}

void displayHumid(float humid){
  //shows relative humidity, not more than 99%
  if (humid >=100.0){
    humid=99.0;    
    }
    lcd.setCursor(0,1);
    lcd.print(humid);
    lcd.print("%");
    lcd.print("         ");

}

void displayAirDensity(){ 

// Custom superscript 3 (Cubic)
byte cubicThree[8] = {
  0b00110,  //   ##
  0b00001,  //     #
  0b00110,  //   ##
  0b00001,  //     #
  0b00110,  //   ##
  0b00000,
  0b00000,
  0b00000
};

  lcd.setCursor(7,1);
  lcd.print(airDensity());
  lcd.print("kg/m");
  lcd.createChar(2, cubicThree);
  lcd.setCursor(15,1);
  lcd.write(2);
}

void testLcd(){
  lcd.setCursor(0, 0);
  lcd.print("ArduMet v1.4.0");
  lcd.setCursor(0, 1);
  lcd.print("by filipecebr1980");
  delay(2000);
  for (int i = 0; i < 16; i++) {
    lcd.scrollDisplayLeft();
    delay(150);
  }
  lcd.clear();
}

void showDateTime(){
  rtc=clock.getDateTime();
  lcd.setCursor(0,0);
  lcd.print("   ");
  lcd.print(rtc.day);
  lcd.print("/");
  lcd.print(rtc.month);
  lcd.print("/");
  lcd.print(rtc.year);
  lcd.print("        ");
  lcd.setCursor(0,1);
  lcd.print("    ");
  lcd.print(rtc.hour);
  lcd.print(":");
  lcd.print(rtc.minute);
  lcd.print(":");
  lcd.print(rtc.second);
  lcd.print("          ");
}

  /*This function calculates air density as function of Temp, Press. and Rel.Humidity
   * Uses the methodology of defining dry air pressure and water vapor pressure
   * as well as using the universal constants for ideal gases for both water vapor and dry air.
   * For more details about this calculation, visit: https://www.omnicalculator.com/physics/air-density
   */
float airDensity(){   
  double t,p,rh,p1,pv,pd,Rd,Rv,density;

  //Specific gas constant for dry air: 287.058 J/(kg.K)
  Rd=287.058;
  
  //Specific gas constant for water vapor 461.495 J/(kg.K)
  Rv=461.495;

  //reads temp in C°
  t=(double)mySensor.readTempC();
  
  //read pressure in Pa
  p=(double)mySensor.readFloatPressure();
  
  //reads rel humidity
  rh=(double)mySensor.readFloatHumidity();
  
  //calculates the saturation vapor pressure, temperature converted to Kelvin
  p1=6.1078*pow(10,7.5*t/(t+237.3));

  //actual vapor pressure as function of relative humidity and temperature
  pv=p1*rh;
  
  //actual dry air pressure
  pd=p-pv;
    
  //temperature in kelvin:
  t=t+273.15;
    
  //calculates air density:
  density = (pd/(Rd*t))+(pv/(Rv*t));
  
  return (float)density;
}

//Sends measurements to USB Serial
void sendSerial(){

  Serial.print(timeStamp());
  Serial.print(" ");
  Serial.print(mySensor.readTempC());
  Serial.print(" ");
  Serial.print(mySensor.readFloatHumidity());
  Serial.print(" ");
  Serial.print(mySensor.readFloatPressure());
  Serial.print(" ");
  Serial.println(airDensity(),3);
}

//Records data in SD Card
void sdWrite(){
    File dataFile = SD.open("data.txt", FILE_WRITE);
    if (dataFile) {
    dataFile.print(timeStamp());
    dataFile.print(" ");
    dataFile.print(mySensor.readTempC());
    dataFile.print(" ");
    dataFile.print(mySensor.readFloatHumidity());
    dataFile.print(" ");
    dataFile.print(mySensor.readFloatPressure());
    dataFile.print(" ");
    dataFile.println(airDensity(),3);
    dataFile.close();
    }
    else{
    Serial.println("Error opening data.txt");
    lcd.clear();
    lcd.print("Error opening");
    lcd.print("data.txt");
    }
}

//Gets timestamps from RTC real time clock
String timeStamp(){
  rtc=clock.getDateTime();
  return (String)rtc.year+"-"+ (String)rtc.month+"-"+ (String)rtc.day+" "+ (String)rtc.hour + ":" + (String)rtc.minute + ":" + (String)rtc.second;
}
