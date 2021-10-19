/**
   Main code for frequency detection of TSL237 sensor
   was heavily based on a pre-written code made by Rob Tillaart
   Made: 2011 - 05 - 16
   Source: https://playground.arduino.cc/Main/TSL235R/

   BLE functionalities were based on code written by Neil Kolban
   https://github.com/nkolban/ESP32_BLE_Arduino

*/

//Libraries needed to run the program:
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

//values for setup, frequency measurement, and ambient light detection:
volatile unsigned long cnt = 0;
unsigned long oldcnt = 0;
unsigned long t = 0;
unsigned long last;
int ambient;
boolean hasChosen = false;  //hasChosen is set to true when user chooses a mode. 

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

//Operation mode: 0 for LCD display, 1 for BLE
int operateMode = 0;

//MACROS SECTION:
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"       



//##-----------------------------------------------------------------
//values for calibration curve:
/**
   Notes:
   M = slope of calibration curve
   B = y-intercept, i.e. x = 0.
*/
float sensorM;
float sensorB;

float expM;
float expB;
//------------------------------------------------------------------


//START OF CODE:

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

void irq1() {
  cnt++;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(2, INPUT);  //Pin connected to TSL237 output.
  pinMode(15, INPUT);
  pinMode(5, INPUT_PULLUP);
  digitalWrite(2, HIGH);
  attachInterrupt(digitalPinToInterrupt(2), irq1, RISING); //set interrupt pin to pin 2. 

  
  
  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();

}//end of setup().

/*
   this function will print every 1000 milliseconds (cnt will increment during this time),
   current millis() value will be assigned to unsigned long last,
   t will get the current value of cnt (which is updated by irq1),
   hz will be the difference between current cnt and old cnt.
   value for hz will be printed after every 1000 milliseconds (1 second)
   NOTE: Hertz is number of cycles PER SECOND, hence the measurement every 1000 ms.
*/
void loop() {
  
  if(digitalRead(5) == HIGH){
    operateMode = 1;
  }
  else{
    operateMode = 0;
  }
  Serial.println(digitalRead(5));
  
  if (operateMode == 0) {
    if (millis() - last >= 1000) {
      if (ambient > 0) {
        lcd.setCursor(0, 1);
        lcd.print("Close lid.");
      }
      last = millis();
      t = cnt;
      unsigned long hz = t - oldcnt;
      lcd.init();
      lcd.backlight();
      lcd.setCursor(0, 0);
      lcd.print("Reading...");
      lcd.setCursor(0, 1);
      lcd.print("FRQ:");
      lcd.print(hz);

      oldcnt = t;
    }
  }

  else if (operateMode == 1) {
    
    unsigned long hz;

    if (deviceConnected) {
      if (millis() - last >= 1000) {
        last = millis();
        t = cnt;
        hz = t - oldcnt;
        oldcnt = t;
        lcd.setCursor(0,1);
        lcd.print(hz);
        char hzStr[10];
        char* inStr = itoa(hz, hzStr, 10);
  
        pCharacteristic->setValue(inStr);
        pCharacteristic->notify();

        delay(3);
      }
       
    }

    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
      delay(500); // give the bluetooth stack the chance to get things ready
      pServer->startAdvertising(); // restart advertising
      Serial.println("start advertising");
      oldDeviceConnected = deviceConnected;
    }

    // connecting
    if (deviceConnected && !oldDeviceConnected) {
      // do stuff here on connecting
      oldDeviceConnected = deviceConnected;
    }
  }

}//end of loop().

//END OF FILE
