/**
 * A BLE client example that is rich in capabilities.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara
 */

#include "BLEDevice.h"
//#include "BLEScan.h"

// The remote service we wish to connect to.
static BLEUUID serviceUUID("d5060001-a904-deb9-4748-2c7f4a124842");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("d5060101-a904-deb9-4748-2c7f4a124842");

static BLEUUID  EmgDataService              ("d5060005-a904-deb9-4748-2c7f4a124842");
static BLEUUID  EmgData0Characteristic      ("d5060105-a904-deb9-4748-2c7f4a124842");
static BLEUUID  EmgData1Characteristic      ("d5060205-a904-deb9-4748-2c7f4a124842");
static BLEUUID  EmgData2Characteristic      ("d5060305-a904-deb9-4748-2c7f4a124842");
static BLEUUID  EmgData3Characteristic      ("d5060405-a904-deb9-4748-2c7f4a124842");

static BLEUUID  EmgStrength0Characteristic  ("d5061105-a904-deb9-4748-2c7f4a124842");/*
static BLEUUID  EmgStrength1Characteristic  ("d5061205-a904-deb9-4748-2c7f4a124842");
static BLEUUID  EmgStrength2Characteristic  ("d5061305-a904-deb9-4748-2c7f4a124842");
static BLEUUID  EmgStrength3Characteristic  ("d5061405-a904-deb9-4748-2c7f4a124842");*/

static BLEAddress MYOADDRESS("D9:96:BE:67:15:48");
  
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;
BLEClient*  pClient  = BLEDevice::createClient();

static void notifyStrengthCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    //Serial.print("Notify callback for characteristic ");
    //Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    //Serial.print(" of data length ");
    //Serial.println(length);
    //Serial.print("data: ");
    for(uint8_t i=0;i<8;i++){
      Serial.print("Strength"); Serial.print(i); Serial.print(":");
      Serial.print((uint8_t)pData[i]);
      Serial.print(",");
    }
    Serial.println();
}

static void notifyEMGCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    //Serial.print("Notify callback for characteristic ");
    //Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    //Serial.print(" of data length ");
    //Serial.println(length);
    //Serial.print("data: ");
    for(uint8_t i=0;i<8;i++){
      Serial.print("EMG"); Serial.print(i); Serial.print(":");
      Serial.print((int8_t)pData[i]);
      Serial.print(",");
    }
    Serial.println();
    for(uint8_t i=8;i<16;i++){
      Serial.print("EMG"); Serial.print(i-8); Serial.print(":");
      Serial.print((int8_t)pData[i]);
      Serial.print(",");
    }
    Serial.println();
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};



bool connectToEMGService(){
    Serial.println("connect To EMGService");
  
    BLERemoteService* pRemoteService = pClient->getService(EmgDataService);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found EMG service");

    if(true){
      BLEUUID EmgDataCharacteristic[] = {EmgData0Characteristic,EmgData1Characteristic,EmgData2Characteristic,EmgData3Characteristic};
  
      for(uint8_t i=0;i<4;i++){
        // Obtain a reference to the characteristic in the service of the remote BLE server.
        pRemoteCharacteristic = pRemoteService->getCharacteristic(EmgDataCharacteristic[i]);
        if (pRemoteCharacteristic == nullptr) {
          Serial.print("Failed to find EMG characteristic UUID: ");
          Serial.println(charUUID.toString().c_str());
          pClient->disconnect();
          return false;
        }
        Serial.println(" - Found EMG characteristic");
    
        if(pRemoteCharacteristic->canNotify())
          pRemoteCharacteristic->registerForNotify(notifyEMGCallback);
      }
    }
    else{
      Serial.println("connect To EMGService - Strength Char");
  
      BLEUUID EmgStrengthCharacteristic[] = {EmgStrength0Characteristic};
  
      for(uint8_t i=0;i<1;i++){
        // Obtain a reference to the characteristic in the service of the remote BLE server.
        pRemoteCharacteristic = pRemoteService->getCharacteristic(EmgStrengthCharacteristic[i]);
        if (pRemoteCharacteristic == nullptr) {
          Serial.print("Failed to find Strength characteristic UUID: ");
          Serial.println(charUUID.toString().c_str());
          pClient->disconnect();
          return false;
        }
        Serial.println(" - Found Strength characteristic");
    
        if(pRemoteCharacteristic->canNotify())
          pRemoteCharacteristic->registerForNotify(notifyStrengthCallback);
      }
    }
}

bool connectToServerEMG() {
    if(connected){
      return false;
    }

    Serial.print("Connecting to Server... ");
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");
    pClient->setMTU(517); //set client to request maximum MTU from server (default is 23 otherwise)
  
    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
      //std::string value = pRemoteCharacteristic->readValue();
      /*Serial.print("The characteristic value was: ");
      //Serial.println(value.c_str());
      uint8_t *pData = pRemoteCharacteristic->readRawData();
      for(uint8_t i=0;i<10;i++){
        Serial.print(pData[i],HEX);
        Serial.print(" ");
      }
      Serial.println("");*/
    }
      
    connected = true;
    return true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.getAddress() == MYOADDRESS && advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks


void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(10, false);
} // End of setup.


// This is the Arduino main loop function.
void loop() {

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    doConnect = false;
    Serial.println("We are going to connected to the BLE Server...");
    if (connectToServerEMG()) {
      Serial.println("We are now connected to the BLE Server.");
      connectToEMGService();
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    /*String newValue = "Time since boot: " + String(millis()/1000);
    Serial.println("Setting new characteristic value to \"" + newValue + "\"");
    
    // Set the characteristic's value to be the array of bytes that is actually a string.
    pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());*/
  }else if(!connected && doScan){
    Serial.println("Rescan...");
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }
  
  delay(1000); // Delay a second between loops.
} // End of loop
