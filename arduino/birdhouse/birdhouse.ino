/*********************************************************************************************************
 * 
 * SMART BIRDHOUSE
 *
 * Sensors: 
 *   1. Light
 *   2. Sound
 *   3. Air quality
 *   4. Temperature and humidity
 *   5. Fine dust (particles)
 *
 * 
 * 
 **********************************************************************************************************/
 

/**********************************************************************************************************
 * LIBRARIES
 **/
#include "DHT.h"                             // 4. Temp and Humidity Sensor


/**********************************************************************************************************
 * VARIABLES
 **/
#define FEEDID  78529                        // COSM settings: feedID
int LED_PIN = 8;                            // The number of the LED Output pin

// Reading variables
unsigned long readingStartTime; 
int readingInterval = 60000; 
int lightReading = -1;
int soundReading = -1;
int tempReading = -1;
int humidityReading = -1; 
int dustReading = -1; 
int airQualityReading = -1; 
unsigned long starttime;

// SENSORS
//   1. Light
int LIGHT_SENSOR_PIN = 0;
float Rsensor;                               // Light sensor: Resistance of sensor in K
//   2. Sound
int SOUND_SENSOR_PIN = 1; 

//   3. Air quality
int AIR_SENSOR_PIN = 2;

//   4. Temperature and humidity
int DHT_PIN = 3;
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);
//   5. Fine dust (digital pin)
int DUST_PIN = 4; 
unsigned long dust_duration;
unsigned long dust_starttime;
unsigned long dust_sampletime_ms = 10000;
unsigned long dust_lowpulseoccupancy = 0;
float dust_ratio = 0;
float dust_concentration = 0;


/**********************************************************************************************************
 * Reading functions
 **/

// Reading the light sensor
void readLight() {
  
  // Read the value (voltage)
  lightReading = analogRead(LIGHT_SENSOR_PIN);
  
  Serial.print("LOG, "); 
  Serial.print(lightReading); 
  Serial.println(" Light Sensor value"); 
}

// Reading the sound sensor
void readSound(){

  // Read the value (voltage)
  soundReading = analogRead(SOUND_SENSOR_PIN);
  
  Serial.print("LOG, ");  
  Serial.print(soundReading); 
  Serial.println(" Sound Sensor value"); 
}


//Reading the Air Quality sensor
void readAirQuality() {
 
  airQualityReading = analogRead(AIR_SENSOR_PIN);
 
  Serial.print("LOG, "); 
  Serial.print(airQualityReading); 
  Serial.println(" Air Quality Sensor"); 
}
  
// Reading the Temperature and Humidity
void readDHT(){
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    Serial.println("ERROR, Failed to read from DHT");
    t = -1; 
    h = -1; 
  } else { }
  
  Serial.print("LOG, ");  
  Serial.print(h); 
  Serial.println(" Humidity Sensor: ");
  Serial.print("LOG, "); 
  Serial.print(t); 
  Serial.println(" Temperature Sensor"); 
  
  tempReading = t; 
  humidityReading = h;
}

// Reading the Dust sensor
void readDust() {
  dust_starttime = millis(); 
  
  while ((millis()-dust_starttime) < dust_sampletime_ms) {
    Serial.print("."); 
    dust_duration = pulseIn(DUST_PIN, LOW);
    dust_lowpulseoccupancy = dust_lowpulseoccupancy+dust_duration;
  }

  dust_ratio = dust_lowpulseoccupancy/(dust_sampletime_ms*10.0);  // Integer percentage 0=>100
  dust_concentration = 1.1*pow(dust_ratio,3)-3.8*pow(dust_ratio,2)+520*dust_ratio+0.62; // using spec sheet curve
  dust_lowpulseoccupancy = 0;
  
  Serial.print("LOG, "); 
  Serial.print(dust_concentration);
  Serial.println(" Dust concentration");
    
  dustReading = dust_concentration;
}

// Send sensor reading data to Serial. 
void sendReadingData() {
  
  Serial.print("FEED"); 
  Serial.print(","); 
  Serial.print(FEEDID); 
  Serial.print(","); 
  Serial.print(lightReading); 
  Serial.print(",");
  Serial.print(soundReading); 
  Serial.print(",");
  Serial.print(tempReading); 
  Serial.print(",");
  Serial.print(humidityReading); 
  Serial.print(",");
  Serial.print(dustReading); 
  Serial.print(",");
  Serial.println(airQualityReading); 
  
}

void doBlink() {
  digitalWrite(LED_PIN, HIGH);
  delay(300);
  digitalWrite(LED_PIN, LOW);
  delay(200);
   digitalWrite(LED_PIN, HIGH);
  delay(300);
  digitalWrite(LED_PIN, LOW);
  delay(200);
   digitalWrite(LED_PIN, HIGH);
  delay(300);
  digitalWrite(LED_PIN, LOW);
}


/**********************************************************************************************************
 ** 
 ** SETUP
 ** 
 ** ***** *************************************************************************************************/
void setup() {

  // Start the serial monitor
  Serial.begin(9600); 
  Serial.println("LOG, Birdhouse starting! ");
  
  pinMode(LED_PIN, OUTPUT); 
  
  // Setup the sensors

  //   1. Light
  Serial.println("LOG, Setup sensor 1: Light sensor START ");
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  Serial.println("LOG, Setup sensor 1: Light sensor COMPLETE");

  //   2. Sound
  //Serial.println("LOG, Setup sensor 2: Sound sensor START");
  pinMode(SOUND_SENSOR_PIN, INPUT);
  Serial.println("LOG, Setup sensor 2: Sound sensor COMPLETE");

  //   3. Air quality
  Serial.println("LOG, Setup sensor 3: Air quality sensor START");
  pinMode(AIR_SENSOR_PIN, INPUT);
  //airqualitysensor.init(AIR_SENSOR_PIN);
  Serial.println("LOG, Setup sensor 3: Air quality COMPLETE");

  //   4. Temperature and humidity
  Serial.println("LOG, Setup sensor 4: Temp and Humidity sensor START");
  dht.begin();
  Serial.println("LOG, Setup sensor 4: Temp and Humidity sensor COMPLETE"); 
  
  //   5. Fine dust (particles)
  Serial.println("LOG, Setup sensor 5: Dust sensor START");
  pinMode(8,INPUT);
  Serial.println("LOG, Setup sensor 5: Dust sensor COMPLETE");
  
  // Define the current timestamp 
  starttime = millis();

}



/**********************************************************************************************************
 **
 ** MAIN LOOP
 ** 
 **********************************************************************************************************/
void loop() {
  // Start the readingTimer()
  readingStartTime = millis(); 
  
  //   1. Light
  readLight(); 
  //   2. Sound
  readSound(); 
  //   3. Air Quality
  readAirQuality(); 
  //   4. Temp and Humidity (DHT)
  readDHT(); 
  //   5. Dust
  readDust(); 
  // send Reading Data
  sendReadingData(); 
  
  doBlink(); 
  // wait for some time. How long? (ms)
  int wait = readingInterval - (millis() - readingStartTime); 
  if (wait < 0 ) 
    wait = 0; 
  //Serial.print("LOG, delay: "); 
  //Serial.println(wait); 
  delay(wait); 
  
}
