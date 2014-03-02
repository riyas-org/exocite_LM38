/*
It uses parts of code from exocite and their libraries. 
Create an account there and use that api key for CIK
Set the mac adress for ethernet shield
Replace writeParam = "number="; with the data name of your device in exocite

This is modified to use LM38 temperature sensor and uses ANALOGUE PIN 5 on arduino uno

The original file can be seen at https://raw.github.com/exosite-garage/arduino_exosite_library/master/examples/TemperatureMonitor/TemperatureMonitor.ino

Credit goes to exocite and please see the above link

For more details on setting up and instructions see 

http://blog.riyas.org/2013/12/build-low-cost-remote-temperature.html

*/
  
  
#include <SPI.h>
#include <Ethernet.h>
#include <Exosite.h>
 

 
/*==============================================================================
* Configuration Variables
*
* Change these variables to your own settings.
*=============================================================================*/
String cikData = "0000000000000000000000000000000080000000";  // <-- FILL IN YOUR CIK HERE! (https://portals.exosite.com -> Add Device)
byte macData[] = {0xDE, 0xAD, 0xBE, 0xEE, 0xEE, 0xFF};        // <-- FILL IN YOUR Ethernet shield's MAC address here.

// User defined variables for Exosite reporting period and averaging samples
#define REPORT_TIMEOUT 30000 //milliseconds period for reporting to Exosite.com
#define SENSOR_READ_TIMEOUT 5000 //milliseconds period for reading sensors in loop
#define LM35Pin 14

/*==============================================================================
* End of Configuration Variables
*=============================================================================*/

class EthernetClient client;
Exosite exosite(cikData, &client);

//
// The 'setup()' function is the first function that runs on the Arduino.
// It runs completely and when complete jumps to 'loop()' 
//
void setup() {
  Serial.begin(9600);
  analogReference(INTERNAL);
  Serial.println("Boot");
  Serial.println("Starting Exosite Temp Monitor");
  Ethernet.begin(macData);
  // wait 3 seconds for connection
  delay(3000); 
 
}
 
//
// The 'loop()' function is the 'main' function for Arduino 
// and is essentially a constant while loop. 
//
void loop() {
  static unsigned long sendPrevTime = 0;
  static unsigned long sensorPrevTime = 0; 
  static float tempC;
  char buffer[7];
  String readParam = "";
  String writeParam = "";
  String returnString = "";  
   
  Serial.print("."); // print to show running
 
 // Read sensor every defined timeout period
  if (millis() - sensorPrevTime > SENSOR_READ_TIMEOUT) {
    Serial.println();
    Serial.println("Requesting temperature...");
    tempC =  analogRead(LM35Pin) / 9.31;
    Serial.print("Celsius:    ");
    Serial.print(tempC);
    Serial.println(" C ..........DONE");   
    sensorPrevTime = millis();
  }
 
  // Send to Exosite every defined timeout period
  if (millis() - sendPrevTime > REPORT_TIMEOUT) {
    Serial.println(); //start fresh debug line
    Serial.println("Sending data to Exosite...");
    
    readParam = "";        //nothing to read back at this time e.g. 'control&status' if you wanted to read those data sources
    writeParam = "number="; //parameters to write e.g. 'temp=65.54' or 'temp=65.54&status=on'
    
    String tempValue = dtostrf(tempC, 1, 2, buffer); // convert float to String, minimum size = 1, decimal places = 2
    
    writeParam += tempValue;    //add converted temperature String value
    
    //writeParam += "&message=hello"; //add another piece of data to send

    if ( exosite.writeRead(writeParam, readParam, returnString)) {
      Serial.println("Exosite OK");
      if (returnString != "") {
        Serial.println("Response:");
        Serial.println(returnString);
      }
    }
    else {
      Serial.println("Exosite Error");
    }
 
    sendPrevTime = millis(); //reset report period timer
    Serial.println("done sending.");
  }
  delay(1000); //slow down loop
}
