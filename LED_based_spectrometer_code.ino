

// defining variables and constants


#include <Servo.h>
Servo myservo;
unsigned long serialdata; // all of the serial data
int inbyte; // is the single piece of information you're sending through serial
float average; 
const int servoPin = 9;
const int numReadings = 50;
const int photodiodePin = 0;
float withOpticsArray[10]; // an array to store the 10 photodiode readings for the with optics scan
float withoutOpticsArray[10]; // an array to store the 10 photodiode readings for the without optics scan
float transmissionRatioArray[10];
const int ledPin[4] = {2,3,4,5}; /* This array represents the 4 address input pins for the 74HC154 chip. These address input pins 
                                    select which LED to turn on. The corresponding array is {A0, A1, A2, A3}. */
const int ledBrightPin = 6; /* enable input E1 to control brightness for each LED (both enable pins have to be set to low for
                                    the chip to work. One of the pins is permanently set to ground and the other is varied through PWM */
const int ledData[10][7] = {{LOW, HIGH, LOW, HIGH, 0, 2.0, 370}, {LOW, LOW, LOW, HIGH, 252, 20.0, 470}, {HIGH, HIGH, HIGH, LOW, 250, 40.0, 505}, \
                        {LOW, HIGH, HIGH, LOW, 250, 57.0, 591}, {HIGH, LOW, HIGH, LOW, 247, 76.0, 611}, {LOW, LOW, HIGH, LOW, 253, 94.0, 624}, \
                        {HIGH, HIGH, LOW, LOW, 254, 114.0, 740}, {LOW, HIGH, LOW, LOW, 254, 136.0, 770}, {HIGH, LOW, LOW, LOW, 253, 154.0, 850}, \
                        {LOW, LOW, LOW, LOW, 251, 174.0, 950}};
                        /* Each array represents the 4 inputs that the pins in the
                        ledPin[4] array need to be, to turn on the corresponding LED and the associated brightness
                        and angle for each LED and the associated wavelength. The corresponding array is {A0, A1, A2, A3, PWM, angle, wavelength}. */
const float A350700CoatingData[10] = {0.9358825488, 0.9382193675, 1, 0.9437982447, 0.9017887028, 0.9390707735, 0.9007557755, 0.8768901671, \
                                      0.8311509986, 0.7931682296};
const float A400600CoatingData[10] = {0.8813873868, 0.9151516976, 0.9253567508, 0.978106305, 0.9662209265, 0.9884124029, 0.8748401242, 0.9035324382, \
                                       0.8402265902, 0.6836941487};
const float BCoatingData[10] = {0.6892448636, 0.7356082359, 0.7578021393, 0.9112546883, 0.9454897309, 0.9636759409, 1, 0.9568512001, 0.9927107484, 0.9929537747};




                                        
// setup 


void setup() {
  inbyte = 0;
  serialdata = 0;
  pinMode(ledPin[0], OUTPUT); // activating each of the 4 address input pins of the ledPin[4] array
  pinMode(ledPin[1], OUTPUT);  
  pinMode(ledPin[2], OUTPUT);  
  pinMode(ledPin[3], OUTPUT);  
  pinMode(ledBrightPin, OUTPUT); // activating the enable pin 
  analogWrite(ledBrightPin, 255); // setting all of the LEDs effectively off to start                                  
  myservo.attach(9);
  myservo.write(0); // setting the starting position of the motor to be 0
  Serial.begin(9600); // initializing serial communication
  delay(1000); // delay for stability
  Serial.println(F("This program can be used to test optics or to calibrate the system."));
  Serial.println(F("To communicate enter a number followed by a backslash."));
  Serial.println(F("Enter 1 to test optics."));
  Serial.println(F("Enter 2 for calibration."));
              }





// functions


void ledOn(int ledNum) { // function that turns on a corresponding LED based on address input settings 
                        //  according to the truth table for the 74HC154                              
  for (int i=0; i<4; i++) {
    digitalWrite(ledPin[i], ledData[ledNum - 1][i]); 
                          }
                        }


float avgPhotodiodeReading() { // function that finds the average photodiode reading over a numReadinds sample size
  average = 0;                    // setting average to 0 for each start 
  int readings[numReadings];      // the readings from the analog input
  int index = 0;                  // the index of the current reading
  float total = 0;                  // the running total             
  myservo.detach();                // detach the motor for photodiode reading stability
    for (index = 0; index < numReadings; index++) {  
  readings[index] = analogRead(photodiodePin); // read from the sensor:  
 delay(50); // delay in between reads for stability  
  total = total + readings[index]; // add the reading to the total:
                                                   }     
 
 average = total / numReadings; // calculate the average  
 return average;
                             }
                             
                             
void calibrationScan() { // function that completes an entire scan of the LEDs, it also prints each LED wavelength and corresponding
                             // photodiode reading 
  Serial.println("Wavelength, Photodiode Reading"); // columns of the effective table 
   for (int x=0; x<10; x++) {
         Serial.print(ledData[x][6]); // printing the current LED wavelength
         Serial.print(","); // separating the data
         myservo.attach(9); // reattaching the motor because the avgPhotodiodeReading function detaches it for stability
         myservo.write(ledData[x][5]); // moving the current LED to its position
         ledOn(x+1); // turning on the current LED
         analogWrite(ledBrightPin, ledData[x][4]); // setting the LED to its specified brightness
         delay(2000); // delay for calibration purposes
         avgPhotodiodeReading(); // taking the average photodiode reading
         Serial.println(average); // printing the photodiode reading
         analogWrite(ledBrightPin, 255); // turning all of the LEDs off 
         delay(500); // delay for stability
                           }
                        }
           
           
long getSerial() { /* a function that allows you to communicate numbers via serial (it translates between the serial monitor and the actual serial communication)
                      *this function can only handle integers* */
    serialdata = 0;
    while (inbyte != '/') { // the / allows to break up the numbers
      while(!Serial.available());
      inbyte = Serial.read();
      if (inbyte >= 0 && inbyte != '/') {
        serialdata = serialdata * 10 + inbyte - '0';
                                        }
                          }
    inbyte = 0; // resetting everything as to move on to the next message
    return serialdata;
}


float findTransmissionRatio() { // dividing the with optics data by the without optics data (the reference scan)
  for (int x=0; x<10; x++) {
  transmissionRatioArray[x] = withOpticsArray[x] / withoutOpticsArray[x];
                           }
  return transmissionRatioArray[10];
                              }  

    
// run loop 


void loop() {
  if (Serial.available() > 0) {

    getSerial();
                            
     if (serialdata == 1) { // testing optics (two scans, one with and one without optics)
         Serial.println(F("To test optics we will first complete one reference scan without optics, and then there"));
         Serial.println(F("will be time for you to add optics. The reference data will be shown after each scan."));
         Serial.println(F("It will be separated by commas so that it's easy to transfer or graph."));
         Serial.println(F("Let's first start with the reference scan...")); 
         Serial.println(F("Reference Reading Data:")); 
         delay(1000);
         for (int x=0; x<10; x++) { /* similar to the calibrationScan() function, except for there is no values are 
                                       printed becuase they're being stored to print after both scans have finished */
           myservo.attach(9); // reattach to take into account the fact that the avgPhotodiodeReading() function detaches the motor 
           myservo.write(ledData[x][5]); // moving the LED to its set position based on the array 
           ledOn(x+1); // turning the led on
           analogWrite(ledBrightPin, ledData[x][4]); // setting the LED to its set brightness based on the array 
           delay(1000); // delay for stability
           avgPhotodiodeReading(); // finding an avg photodiode reading
           analogWrite(ledBrightPin, 255); // turning the led off
           delay(500); // stability delay 
           withoutOpticsArray[x] = average; // storing the reference scan's photodiode values
           Serial.println(average) // letting user know the reference values
                                  }
           Serial.println(F("The reference scan is now complete. Enter 0 when you're ready to start the with optics scan."));

         while(1) { // loop that is always true unless you enter 8/ to break out of it
          getSerial();
         if (serialdata == 0) { // the cue that that optics has already been placed
           for (int x=0; x<10; x++) { // same function as above but now storing photodiode readings in the withOpticsArray[10] 
           myservo.attach(9);
           myservo.write(ledData[x][5]);
           ledOn(x+1);
           analogWrite(ledBrightPin, ledData[x][4]);
           delay(1000);
           avgPhotodiodeReading();
           analogWrite(ledBrightPin, 255);
           delay(500);
           withOpticsArray[x] = average; // storing the with optics scan's photodiode values 
                                  }
                                    
         Serial.println(F("Testing optics is complete! Here is the data:"));
         Serial.println(F("Photodiode Reading With Optics,Transmission Ratio")); // defining the columns
         for (int y=0; y<10; y++) { // printing all of the data in 6 columns
           Serial.println(withOpticsArray[y]); // photodiode reading with optics
           Serial.print(F(","));
           findTransmissionRatio();
           Serial.print(transmissionRatioArray[y]); // normalized transmission ratio
                                 }
       
        Serial.println(F("Enter 0 if you want to test another piece of optics against the original reference scan, or enter 8"));
        Serial.println(F("to exit out of this menu."));     
                           }
                           
         if (serialdata == 8) {
           Serial.println(F("You exited the menu, you can now enter 1 to restart the testing process or 2 for the main calibration menu."));
           break;
                              }
         }
     }
  }
    
  

if (serialdata == 2) { // calibration code
  Serial.println(F("Welcome to the main calibration menu!"));
  Serial.println(F("Enter 1 to go through a slow scan.")); // similar to going through a really slow reference scan (the calibration scan function) 
  Serial.println(F("Enter 2 to find the optimal motor angle")); // finding optimal motor angle by stepping through the angle near what it is currently set and taking readings
  Serial.println(F("Enter 3 to calibrate the brightness of a LED.")); // moving the LED to its set position and focusing on tuning the brightness
  Serial.println(F("Enter 4 to turn on a LED.")); // turn on a LED, set brightness
  Serial.println(F("Enter 5 to move the motor.")); // move motor
  Serial.println(F("Enter 6 to take a PD Reading.")); // take a photodiode reading

  
getSerial();

 if (serialdata == 1) { // going through one slow scan
        Serial.println(F("This scan will slowly go through one complete cycle. A live photodiode reading will be printed with each LED"));
        Serial.println(F("wavelength."));
        Serial.println(F("Scan starting..."));
        calibrationScan(); /* function that is similar to a reference scan but slower for calibration purposes. You can tune how slow the scan is
                               by adjusting the delay time in this function */        
        Serial.println(F("Done!"));
        Serial.println(F("Enter 1 to test optics or 2 to return back to the main calibration menu."));
                      }
      
      
if (serialdata == 2) { // finding optimal motor angle
   for (int y=0; y<30.0; y++) {
    if (y==0) {
      for (float x=-3; x<3; x++) {
       Serial.print(ledData[y][5]+x); // printing the current LED wavelength
         Serial.print(" "); // separating the data
         myservo.attach(9);
         myservo.write(ledData[y][5]+x);
         ledOn(y+1); // turning on the current LED
         analogWrite(ledBrightPin, ledData[y][4]); // setting the LED to its specified brightness
         delay(1000); // delay for calibration purposes
         avgPhotodiodeReading(); // taking the average photodiode reading
         Serial.println(average); // printing the photodiode reading
         analogWrite(ledBrightPin, 255); // turning all of the LEDs off 
        // delay for stability
         myservo.attach(9);
         myservo.write(ledData[y][5]+x+20.0);
         delay(500);
         }
    } 
    else if (ledData[y][5] < 160) {
      for (float x=-3; x<3; x++) {
       Serial.print(ledData[y][5]+x); // printing the current LED wavelength
         Serial.print(" "); // separating the data
         myservo.attach(9);
         myservo.write(ledData[y][5]+x);
         ledOn(y+1); // turning on the current LED
         analogWrite(ledBrightPin, ledData[y][4]); // setting the LED to its specified brightness
         delay(1000); // delay for calibration purposes
         avgPhotodiodeReading(); // taking the average photodiode reading
         Serial.println(average); // printing the photodiode reading
         analogWrite(ledBrightPin, 255); // turning all of the LEDs off 
        // delay for stability
         myservo.attach(9);
         myservo.write(ledData[y][5]+x+20.0);
         delay(500);
         }
    } 
      else if (y==9) {
      for (float x=-3; x<3; x++) {
       Serial.print(ledData[y][5]+x); // printing the current LED wavelength
         Serial.print(" "); // separating the data
         myservo.attach(9);
         myservo.write(ledData[y][5]+x);
         ledOn(y+1); // turning on the current LED
         analogWrite(ledBrightPin, ledData[y][4]); // setting the LED to its specified brightness
         delay(1000); // delay for calibration purposes
         avgPhotodiodeReading(); // taking the average photodiode reading
         Serial.println(average); // printing the photodiode reading
         analogWrite(ledBrightPin, 255); // turning all of the LEDs off 
        // delay for stability
         myservo.attach(9);
         myservo.write(ledData[y][5]+x-20.0);
         delay(500);
         } 
      }

      else {
      for (float x=-3; x<3; x++) {
       Serial.print(ledData[y][5]+x); // printing the current LED wavelength
         Serial.print(" "); // separating the data
         myservo.attach(9);
         myservo.write(ledData[y][5]+x);
         ledOn(y+1); // turning on the current LED
         analogWrite(ledBrightPin, ledData[y][4]); // setting the LED to its specified brightness
         delay(1000); // delay for calibration purposes
         avgPhotodiodeReading(); // taking the average photodiode reading
         Serial.println(average); // printing the photodiode reading
         analogWrite(ledBrightPin, 255); // turning all of the LEDs off 
        // delay for stability
         myservo.attach(9);
         myservo.write(ledData[y][5]+x-20.0);
         delay(500);    
    }
      }
   }
    }
  
 
    
    
  if (serialdata == 3) { // adjusting brightness setting
    Serial.println(F("This part of the menu is used to calibrate the brightness. Enter the LED number you'd like to work with."));
    getSerial();
    Serial.print(F("LED number: "));
    Serial.println(serialdata);
    Serial.println(F("Moving the LED to its set position..."));
    myservo.attach(servoPin); 
    delay(1000);
    myservo.write(ledData[serialdata - 1][5]); // putting the LED to its position
    ledOn(serialdata); // turning the LED on
    Serial.println(F("Now we're ready to tune the brightness."));
    Serial.print(F("The current brightness setting is "));
    Serial.print(ledData[serialdata - 1][4]);
    Serial.println(F(" out of 255."));
    Serial.println(F("Enter 0 to start."));
    
    while(1) {
      getSerial();
      if (serialdata == 0) {
        Serial.println(F("Set Brigthness by entering a number from 0 - 255.")); 
        getSerial(); // getting the brightness number
        Serial.print(F("Brightness: "));
        Serial.println(serialdata); // letting you know which brightness you picked
        analogWrite(ledBrightPin, serialdata); // setting the brightness
        Serial.println(F("Now we're going to take a photodiode reading..."));
        Serial.print(F("Photodiode reading: "));
        avgPhotodiodeReading();
        Serial.println(average);
        Serial.println(F("Done!"));
        Serial.println(F("Enter 0 to continue tuning the brightness or 8 to exit."));
      }
     if (serialdata == 8) {
        Serial.println(F("You have now exited out of the tuning brightness option. Enter 1 to test optics or 2"));
        Serial.println(F("to return to the main calibration menu."));
        break;
     }
    }
  }
      
   
 if (serialdata == 4) {
    Serial.println(F("Enter a number to turn on LED x."));
    getSerial(); 
    Serial.print(F("LED number: "));
    Serial.println(serialdata);
    ledOn(serialdata); // turning on the lED
    Serial.println(F("Set Brigthness by entering a number from 0 - 255."));
    getSerial(); // getting brightness
    Serial.print(F("Brightness: "));
    Serial.println(serialdata);
    analogWrite(ledBrightPin, serialdata);
    Serial.println("Done!");
    Serial.println(F("Enter 1 to test optics or 2 to return to the main calibration menu."));
  }
  
  
    if (serialdata == 5) {
      Serial.println(F("Enter a position from 0 - 180."));
      getSerial(); // getting motor position
      Serial.print(F("Position: "));
      Serial.println(serialdata); // letting you know which position you picked
      myservo.attach(servoPin); // attaching the motor just in case it became detached somewhere
      myservo.write(serialdata); // moving the motor to its position 
      Serial.println(F("Done!"));
      Serial.println(F("Enter 1 to test optics or 2 to return to the main calibration menu."));
    }   
                         
     
 if (serialdata == 6) {
   avgPhotodiodeReading();
   Serial.print(F("Photodiode Reading: "));
   Serial.println(average);
   Serial.println(F("Done!"));
   Serial.println(F("Enter 1 to test optics or 2 to return to the main calibration menu."));
 }
  }
}      

