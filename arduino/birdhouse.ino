// Temp + Humidity: http://www.seeedstudio.com/wiki/Grove-_Temperature_and_Humidity_Sensor
#include "DHT.h"
#define DHTPIN A0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// COSM settings
#define FEEDID  78529


/*
/* Grove - Light Sensor demo v1.0
* 
* signal wire to A0.
* By: http://www.seeedstudio.com
*/
#include <math.h>
float Rsensor;                   //Resistance of sensor in K


// Dust sensor
/*
 Interface to Shinyei Model PPD42NS Particle Sensor
 Program by Christopher Nafis 
 Written April 2012
 
 http://www.seeedstudio.com/depot/grove-dust-sensor-p-1050.html
 http://www.sca-shinyei.com/pdf/PPD42NS.pdf
 
 JST Pin 1 (Black Wire)  => Arduino GND
 JST Pin 3 (Red wire)    => Arduino 5VDC
 JST Pin 4 (Yellow wire) => Arduino Digital Pin 8
 */

int pin = 2;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 10000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;



// SETUP
void setup() {
  Serial.begin(9600); 
  Serial.println("LOG, Birdhouse starting! ");
  
  // Temp and Humidity
  dht.begin();
  
  // Dust sensor
  pinMode(8,INPUT);
  starttime = millis();

}

// LOOP
void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    Serial.println("ERROR, Failed to read from DHT");
  } else {
    //Serial.print("Humidity: "); 
    //Serial.print(h);
    //Serial.print(" %\t");
    //Serial.print("Temperature: "); 
    //Serial.print(t);
    //Serial.println(" *C");
  }
  
  
  // Light
  int sensorValue = analogRead(1); 
  Rsensor=(float)(1023-sensorValue)*10/sensorValue;
  //Serial.println("the analog read data is ");
  //Serial.println(sensorValue);
  //Serial.println("the sensor resistance is ");
  //Serial.println(Rsensor,DEC);//show the ligth intensity on the serial monitor;
  
  
  // Dust sensor   
  duration = pulseIn(pin, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;

  if ((millis()-starttime) > sampletime_ms)
  {
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=>100
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
    Serial.print(lowpulseoccupancy);
    Serial.print(",");
    Serial.print(ratio);
    Serial.print(",");
    Serial.println(concentration);
    lowpulseoccupancy = 0;
    starttime = millis();
  }
  
  Serial.print("FEED"); 
  Serial.print(","); 
  Serial.print(FEEDID); 
  Serial.print(","); 
  Serial.print("10"); 
  Serial.print(",");
  Serial.println("20"); 
  delay(1000); 
  
}

