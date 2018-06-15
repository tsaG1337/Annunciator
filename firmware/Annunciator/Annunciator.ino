//*** ShiftPWM Setup ***//
const int ShiftPWM_latchPin = 9; //Latch Pin for Shift-Register
const bool ShiftPWM_invertOutputs = true; //Invert Outputs
const bool ShiftPWM_balanceLoad = true; //If true, only one Output will be on at the time (Power Saving)
#include <ShiftPWM.h>   // including ShiftPWM.h after setting the pins

unsigned char maxBrightness = 255;
unsigned char pwmFrequency = 120;
int numRegisters = 3;
int numRGBleds = numRegisters * 8 / 3;
int currBrightness = 128;

//*** Definition of operational ranges ***//

int rpmRange[] = {0, 1440, 1800, 5500, 5800, 5850};
// Red_end , yellow_begin, green_begin, green_end, yellow_end, red_begin) (1/10 Bar)
int oilPressRange[] = { 0 , 8 , 20 , 50 , 70};
int oilTempRange[] = { 1 , 50 , 90 , 110 , 140};
int egtRange[] = { 500, 800, 880};
int CHTTempRange[] = {120, 150};
int fuelPressureRange[] = {15, 40}; // ( 1/100 Bar)
int fuelLHRange[] = {0, 200, 400};
int fuelRHRange[] = {0, 200, 400};
int voltsRange[] = {100, 140};

//*** Settings for FAKE NMEA Decoder ***//
enum values { RPM=1, manPress, oilPress, fuelPress, oilTemp, CHT1, CHT2, OAT, fuelLH, fuelRH, fuelFlow, fuelSum, EGT1, EGT2, flaps, volts, amps };
bool dataUpdated = 0;
int16_t valueArray[20];
unsigned long lastUpdatedMillis = 0;
unsigned long previousMillis = 0;
unsigned long timeOutIntervall = 1500;

void setup() {
  Serial.begin(230400);

  //Init Shiftregisters
  ShiftPWM.SetAmountOfRegisters(numRegisters);
  ShiftPWM.SetPinGrouping(1);
  ShiftPWM.Start(pwmFrequency, maxBrightness);

  // Turn all LED's off.
  ShiftPWM.SetAll(0);

  for (int hue = 0; hue < 360; hue++) {
    ShiftPWM.SetAllHSV(hue, 255, 255);
    delay(5);
  }
  //Set all LEDs to blue
  for (int led = 0; led < numRGBleds; led++) {
    ShiftPWM.SetRGB(led, 0, 0, currBrightness); // blue
  }
  //End init Shiftregisters

}


void loop() {

  while (Serial.available() > 0) {
    char incomingByte = Serial.read();
    readIncomingData(incomingByte);
  }

  if (dataUpdated) {
    setLED();
    dataUpdated = 0;
  }
  unsigned long currentmillis = millis();
  if (currentmillis - lastUpdatedMillis >= 1500) {
    for (int led = 0; led < numRGBleds; led++) {
      ShiftPWM.SetRGB(led, 0, 0, currBrightness); // blue
    }
  }
}

void setLED() {
  //Set Oil Pressure LED
  //int oilPressRange[] = { 0 , 8 , 20 , 50 , 70};
  int currOilPress = valueArray[oilPress];
Serial.println(currOilPress);
  if (currOilPress <= oilPressRange[1] ||  currOilPress >= oilPressRange[4]) {
    ShiftPWM.SetRGB(7, currBrightness, 0, 0); //red
  } else if (currOilPress >= oilPressRange[1] &&  currOilPress <= oilPressRange[2]) {
  ShiftPWM.SetRGB(7, currBrightness, currBrightness, 0 ); //yellow
  } else if (currOilPress >= oilPressRange[2] &&  currOilPress <= oilPressRange[3]) {
    ShiftPWM.SetRGB(7, 0, currBrightness, 0); //green
  }
  //End Oil Pressure


  //Set Fuel LH LED
  //int fuelLHRange[] = {0, 200, 400};
  int currfuelLH = valueArray[fuelLH];
  if (currfuelLH <= fuelLHRange[1] ) {
    ShiftPWM.SetRGB(6, currBrightness, 0, 0); //red
  } else  {
    ShiftPWM.SetRGB(6, 0, currBrightness, 0); //green
  }
  //End Fuel LH


  //Set Oil Temp LED
  //int oilTempRange[] = { 1 , 50 , 90 , 110 , 140};
  int currOilTemp = valueArray[oilTemp];
  if (currOilTemp <= oilTempRange[1] ||  currOilTemp >= oilTempRange[4]) {
    ShiftPWM.SetRGB(5, currBrightness, 0, 0); //red
  } else if (currOilTemp >= oilTempRange[1] &&  currOilTemp <= oilTempRange[2] || currOilTemp >= oilTempRange[3] && currOilTemp <= oilTempRange[4] ) {
  ShiftPWM.SetRGB(5, currBrightness, currBrightness, 0 ); //yellow
  } else if (currOilTemp >= oilTempRange[2] &&  currOilTemp <= oilTempRange[3]) {
    ShiftPWM.SetRGB(5, 0, currBrightness, 0); //green
  }
  //End Oil Temp


  //Set Fuel Pressure LED
  //int fuelPressureRange[] = {15, 40};
  int currFuelPress = valueArray[fuelPress];
  if ((currFuelPress < fuelPressureRange[0] ) ||  (currFuelPress > fuelPressureRange[1] )) {
    ShiftPWM.SetRGB(4, currBrightness, 0, 0); //red
  } else if (currFuelPress >= fuelPressureRange[0] &&  currFuelPress <= fuelPressureRange[1]) {
    ShiftPWM.SetRGB(4, 0, currBrightness, 0); //green
  }
  //End Fuel Pressure


  //Set Volts LED
  //int voltsRange[] = {100,140};
  int currVolts = valueArray[volts];
  if ((currVolts < voltsRange[0] ) ||  (currVolts > voltsRange[1] )) {
    ShiftPWM.SetRGB(3, currBrightness, 0, 0); //red
  } else if (currVolts >= voltsRange[0] &&  currVolts <= voltsRange[1]) {
    ShiftPWM.SetRGB(3, 0, currBrightness, 0); //green
  }
  //End Volts


  //Set CHT Temp LED
  //int CHTTempRange[] = {60, 150};
  int currCHTTemp = valueArray[CHT1];
  if ((currCHTTemp < CHTTempRange[0] ) ||  (currCHTTemp > CHTTempRange[1] )) {
    ShiftPWM.SetRGB(2, currBrightness, 0, 0); //red
  } else if (currCHTTemp >= CHTTempRange[0] &&  currCHTTemp <= CHTTempRange[1]) {
    ShiftPWM.SetRGB(2, 0, currBrightness, 0); //green
  }
  //End CHT Temp


  //Set Fuel RH LED
  //int fuelRHRange[] = {0, 200, 400};

  int currfuelRH = valueArray[fuelRH];
  if (currfuelRH <= fuelRHRange[1] ) {
    ShiftPWM.SetRGB(1, currBrightness, 0, 0); //red
  } else  {
    ShiftPWM.SetRGB(1, 0, currBrightness, 0); //green
  }
  //End Fuel RH


  //Set EGT LED
  //int egtRange[] = { 500, 800, 880};
  int currEGT = valueArray[EGT1];
  if ((currEGT <= egtRange[0]) ) {
    ShiftPWM.SetRGB(0, currBrightness - 10, currBrightness, 0); //yellow
  } else  if ((currEGT >= egtRange[2]) ) {
    ShiftPWM.SetRGB(0, currBrightness, 0, 0); //red
  } else {
    ShiftPWM.SetRGB(0, 0, currBrightness, 0); //green
  }
  //End EGT
}

void readIncomingData(char ByteRead) {
  static char dataArray[100];
  static byte valuePointer = 0;
  static byte dataPointer = 0;
  static byte dataBeginPointer = 0;
  if (ByteRead == '$') {
    Serial.println("Array, Received!");

    setLED();
      memset(valueArray, 0, sizeof(valueArray));
    memset(dataArray, 0, sizeof(dataArray));
    dataBeginPointer = 0;
    dataPointer = 0;
    valuePointer = 0;
    dataArray[dataPointer] = ByteRead;
    dataPointer++;
    dataUpdated = 1;
    lastUpdatedMillis = millis();

  } else if (ByteRead == ',') {
    int dataValue = 0;
    dataValue = convert_slice(dataArray, dataBeginPointer, dataPointer);
    valueArray[valuePointer] = dataValue;
    valuePointer++;
    dataBeginPointer = dataPointer;

  } else if (ByteRead == '\r') {
    int dataValue = 0;
    dataValue = convert_slice(dataArray, dataBeginPointer, dataPointer);
    valueArray[valuePointer] = dataValue;
    valuePointer++;
    dataBeginPointer = dataPointer;
  } else if (ByteRead == '\n') {
    Serial.println("END");

  } else {
    dataArray[dataPointer] = ByteRead;
    dataPointer++;
  }

}

int convert_slice(const char *s, size_t a, size_t b) {
  int val = 0;
  while (a < b) {
    val = val * 10 + s[a++] - '0';
  }
  return val;
}
