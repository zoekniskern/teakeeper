//Project 1: Teakeeper
//Purpose: to detect when an electric kettle is boiled,
//and then monitor the temperature of a mug of tea
//Zoe Kniskern

//Materials:
//RGB LED
//Thermistor
//Photocell
//Button

//References
//RGB:
//Thermistor: https://learn.adafruit.com/thermistor/using-a-thermistor
//Photocell: https://learn.adafruit.com/photocells/arduino-code

//GLOBAL VARIABLES
const int photocellPin = 1; //connected to A1
int photocellReading; //analog reading

//Thermistor specific
//resister
#define SERIESRESISTOR 10000
//pin
#define THERMISTORPIN A0
//var for reading
float ThermReading;
//sampling for an accurate read
#define NUMSAMPLES 5
int samples[NUMSAMPLES];
// resistance at 25 degrees C
#define THERMISTORNOMINAL 10000
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950

const int redPin = 11;
const int greenPin = 10;
const int bluePin = 9;
//using a Common Anode LED
#define COMMON_ANODE

bool kettleDone = false;
int buttonPin = 8;
bool pourTea = false;

//setup
void setup() {
  //to debug with the monitor
  Serial.begin(9600);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  //Thermistor connected to 3.3V
  //analogReference(EXTERNAL);
}//end of setup

//LOOP
void loop() {

  readPhotoCell();
  
  if(kettleDone){
    while(!pourTea){
      checkButton();
      blinking(255,0,0);
    }
  }

  while(pourTea){
//    setColor(0,255,0);
//    delay(1000);
    readThermistor();
  }
  
  if (photocellReading > 750) {
    Serial.println(" - Light off");
    setColor(0, 0, 0);  // white    
  } else {
    Serial.println(" - Very bright"); //USING THIS AS THE KETTLE LIGHT CHANGE
    kettleDone = true;        
  }
  delay(1000);
  Serial.println("");
}//end of loop

//set color
void setColor(int red, int green, int blue){
  //Serial.println("setColor ran");
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);  
}//end of setColor

//read photocell
void readPhotoCell(){
  //receive reading from photocell
  photocellReading = analogRead(photocellPin);
  delay(10);
  photocellReading = analogRead(photocellPin);
  delay(10);
  //print reading
  Serial.print("Photo Analog reading = ");
  Serial.println(photocellReading);     // the raw analog reading
}//end of readPhotoCell

//read thermistor
void readThermistor(){
  //receive multiple readings from thermistor
  uint8_t i;
  float average;
 
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
 
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
 
  Serial.print("Average analog reading "); 
  Serial.println(average);

  //convert to resistance
  //R = 10K*ADC / (1023 - ADC)
  float bottom = 1023 - average;
  average = (SERIESRESISTOR * average) / bottom;
  //print reading
  Serial.print("Therm Analog reading "); 
  Serial.println(average);

  //convert to temperature
  float steinhart;
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
  //convert to farenheit
  float faren;
  faren = steinhart * 1.8000 + 32.00;
   
  Serial.print("Temperature "); 
  Serial.print(faren);
  Serial.println(" *F");

  if(faren > 140.00){
    Serial.println("VERY HOT");
    setColor(255,0,0);
    delay(1000);
  }else if(faren > 80 && faren < 141) {
    Serial.println("HOT");
    setColor(0,255,0);
    delay(1000);
  }else{
    Serial.println("NOT HOT");
    setColor(0,0,255);
    delay(1000);
  }

  delay(1000);

//OLD CODE
//  ThermReading = analogRead(THERMISTORPIN);
//  delay(10);
//  ThermReading = analogRead(THERMISTORPIN);
//  delay(10);
//  //convert to temperature
//  //R = 10K*ADC / (1023 - ADC)
//  float bottom = 1023 - ThermReading;
//  ThermReading = (SERIESRESISTOR * ThermReading) / bottom;
//  //print reading
//  Serial.print("Therm Analog reading "); 
//  Serial.println(ThermReading);
//
}//end of readThermistor

//check button
void checkButton(){
  if (digitalRead(buttonPin) == LOW){
    Serial.println("Button pressed");
    pourTea = true;
  }
}//end of checkButton

//blinking led
void blinking(int red, int green, int blue){
  setColor(red,green,blue);
  delay(100);
  setColor(0,0,0);
  delay(100);
}//end of blinking
