/* Meteorological Unit - Temp, relative humidity, pressure and air density
 * 
 *  V 1.3.1 - Integrated Real Time clock and SD Card slot for data logging.
 *  Fixed 1s sampling time based on RTC clock for precise tame stamping.

*Created by Filipe Brandao Using
*Sparkfun GY-BME280 Library
*SMS0408E2 Library
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

  SUNMAN SMS0408E2
  LCD   -> Arduino
  VDD   -> 6
  DI    -> 5
  VSS   -> 4
  CLK   -> 3
  BLA   -> 2
  BLK   -> GND or 7

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
 - SUNMAN SMS0408E2 LDC 7 Segments display: https://github.com/filipecebr1980/Sunman-SMS0408E2
*/

#include <Adafruit_Sensor.h>
#include <Sms0408.h>
#include <Wire.h>
#include "SparkFunBME280.h"
#include <DS3231.h>
#include <SPI.h>
#include <SD.h>

//used to select chipset for type of SD reader. Adafruit=10
const int chipSelect = 10;

//delay for serial print, update display and write output file
uint32_t delayMS=1000;

const int VDD_PIN=6;
const int DI_PIN=5;
const int VSS_PIN=4;
const int CLK_PIN=3;
const int BLA_PIN=2;
const int BLK_PIN=7;

//create an LCD object
Sms0408 myLCD(DI_PIN,CLK_PIN,BLK_PIN);

//create sensor object
BME280 mySensor;

//initialize clock and create RTC object:
DS3231 clock;
RTCDateTime rtc;

void setup() {
  
  //Configure Pins as OUTPUT
  pinMode(VSS_PIN,OUTPUT);
  pinMode(VDD_PIN,OUTPUT);
  pinMode(CLK_PIN,OUTPUT);
  pinMode(DI_PIN,OUTPUT);
  pinMode(BLA_PIN,OUTPUT);
  pinMode(BLK_PIN,OUTPUT);

  //powers up SUNMAN SMS0408
  digitalWrite(VDD_PIN, HIGH); 
  digitalWrite(VSS_PIN, LOW);
  digitalWrite(BLA_PIN, HIGH);
  digitalWrite(BLK_PIN, LOW);
  
  Serial.begin(9600);
  Serial.println("System started.");
  
  Wire.begin();
  mySensor.setI2CAddress(0x76); //Connect to sensor
  if(mySensor.beginI2C() == false) {
      Serial.println("Sensor B connection failed!");
      myLCD.displayError();
      myLCD.adjust();
      while(1){}
    }

  Serial.println("Sensor connection OK!");

  //tests lcd (all segments ON for 1 second
  testLcd();
  delay(delayMS);

  // Initialize DS3231 RTC
  Serial.println("Initialize Real Time Clock DS3231");;
  clock.begin();

  
  // Set sketch compiling time - please comment after first clock sync
  //and recompile/upload to Arduino. otherwise, clock will be always reset
  //to the sketch compiling date/time
  
  //clock.setDateTime(__DATE__, __TIME__);

  //Card reader setup
  if (!SD.begin(chipSelect)) {
    Serial.println("SD Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("SD Card initialized successfully.");

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
    refreshLCD(counter);    
    counter++;
    second=clock.getDateTime().second;
    }   
  }
}

void refreshLCD(int option){

  switch(option){

    case 0:
    //shows temperature in C°    
    displayTemp(mySensor.readTempC());
    break;
    
    case 2:
    //shows humidity in %
    displayHumid(mySensor.readFloatHumidity());
    break;
    
    case 4:
    //shows pressure in hPa (millibar)
    displayPressure(mySensor.readFloatPressure()/100.0);
    break;
    
    case 6:
    //shows air density in kg/m³
    displayAirDensity();
    break;
    }
    
}

void displayTemp(float temp){
  //shows temperature in C°
  if (temp>=0.0){

  myLCD.clearLCD();  
  myLCD.displayInt((int)temp);
  myLCD.codig(18,0);
  myLCD.adjust();
  }
  else {
  //for negative temperature
  myLCD.clearLCD();
  myLCD.codig(19,0);  
  myLCD.displayInt((int)(abs(temp)));
  myLCD.codig(18,0);
  myLCD.adjust();          
    }  
}

void displayHumid(float humid){
  //shows relative humidity, not more than 99%
  if (humid >=100.0){
    humid=99.0;    
    }

  myLCD.clearLCD(); 
  myLCD.showColumn();
  myLCD.codig(17,0); 
  myLCD.codig(16,0);  
  myLCD.displayInt((int)humid);
  myLCD.adjust();
 
}

void displayPressure(float pressure){

  myLCD.clearLCD(); 
  
  if(pressure < 1000.0){
    myLCD.displayFloatAuto(pressure);    
  }
  else
  {
    myLCD.displayInt((int)pressure);
  }
  myLCD.adjust();
}

void displayAirDensity(){

  myLCD.clearLCD(); 
  myLCD.displayFloatAuto(airDensity());    
  myLCD.adjust();
}

void testLcd(){
  //fills LCD (all segments on- good to test if has any 
  //bad segment
  myLCD.fillLCD();    
  delay(1000);
  myLCD.clearLCD();
}

  /*This function calculates air density as function of Temp, Press. and Rel.Humidity
   * Uses the methodology of defining dry air pressure and water vapor pressure
   * as well as using the universal constants for ideal gases for both water vapor and dry air.
   * For more details about this calculation, visit: https://www.omnicalculator.com/physics/air-density
   */
float airDensity(){   
  double t,p,rh,p1,pv,pd,Rd,Rv,density;

  //Specific gas constant for dry air: 287.058 J/(Kg.K)
  Rd=287.058;
  
  //Specific gas constant for water vapor 461.495 J/(Kg.K)
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
    }
}

//Gets timestamps from RTC real time clock
String timeStamp(){
  rtc=clock.getDateTime();
  return (String)rtc.year+"-"+ (String)rtc.month+"-"+ (String)rtc.day+" "+ (String)rtc.hour + ":" + (String)rtc.minute + ":" + (String)rtc.second;
}
