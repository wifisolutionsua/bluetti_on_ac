#include "BLEDevice.h"

#define LED 2
// const int potPin = 34;

// Potentiometer value
int potValue = 0;
// int delayy = 1000*30;
int delayy = 1000*15;
int writecount = 0;
int failcount = 0;

// The remote service we wish to connect to.
static BLEUUID serviceUUID("0000ff00-0000-1000-8000-00805f9b34fb");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("FF02");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

unsigned long lastTime = 0;
unsigned long lastTimeWrite = 0;
unsigned long delaytoload = 1000*100;
// Timer set to 10 minutes (600000)
unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
// unsigned long timerDelay = 10000;
unsigned long timerDelayToScan = 60*1000;//60sec




static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char*)pData);
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    
    // pClient->connect(e0:a4:20:f7:f5:a8);
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
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }

    if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);

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
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

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
  pinMode(LED,OUTPUT);
  // pinMode(potPin,INPUT_PULLUP);
  // sleep(1000*30);
  digitalWrite(LED,HIGH);
  delay(delaytoload);
  digitalWrite(LED,LOW);

  startscan();
}
void startscan()
{
  digitalWrite(LED,HIGH);
  delay(300);
  digitalWrite(LED,LOW);
  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  // pBLEScan->setInterval(1349);
  // pBLEScan->setWindow(449);
  pBLEScan->setInterval(2000);
  pBLEScan->setWindow(1500);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(10, false);
} // End of setup.



// This is the Arduino main loop function.
void loop() {

  if (writecount>50)
  {
    Serial.println("Going to sleep now");
    esp_deep_sleep_start();
    Serial.println("This will never be printed");
  }
    if (failcount>10)
  {
    Serial.println("Restarting in 10 seconds");
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
    delay(100);
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
    delay(100);
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
    delay(100);
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
    delay(100);
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
    delay(100);
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
    delay(100);
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
    delay(100);
    delay(10000);
    ESP.restart();
  }
    // potValue = analogRead(potPin);
  // Serial.println("input: "+potValue);
  //  Serial.println("Loop runMAIN");

  //  if ((millis() - lastTime) > timerDelay) {
  //     Serial.println("Loop run");

// digitalWrite(LED,LOW);
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
      digitalWrite(LED,HIGH);
    delay(300);
    digitalWrite(LED,LOW);
     delay(300);
    digitalWrite(LED,HIGH);
     delay(300);
      digitalWrite(LED,LOW);
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }
  // else
  // {
  //   startscan();
  // }
  // if ((millis() - lastTimeWrite) > timerDelayToScan) {
  //   Serial.println("startscan");
  //   startscan();
  // }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    String newValue = "Time since boot: " + String(millis()/1000);
    // Serial.println("Setting new characteristic value to \"" + newValue + "\"");
    
newValue = "01060BBF00017BCA";
Serial.println("Setting new characteristic value to \"" + newValue + "\"");

byte colors[2][8] = {
    {0x01, 0x06, 0x0B, 0xBF, 0x00, 0x01, 0x7B, 0xCA}, // Blue
    {0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00}  // Red
  };

pRemoteCharacteristic->writeValue(colors[0], sizeof(colors[0]));  
digitalWrite(LED,HIGH);
  delay(300);
  digitalWrite(LED,LOW);
   delay(300);
  digitalWrite(LED,HIGH);
   delay(300);
  digitalWrite(LED,LOW);
     delay(300);
  digitalWrite(LED,HIGH);
   delay(300);
  digitalWrite(LED,LOW);
  lastTimeWrite = millis();
  // delayy=1000*60*5;
  // writecount=writecount+1;
  writecount++;

    // Set the characteristic's value to be the array of bytes that is actually a string.
    // pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
  }else if(doScan){
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
       Serial.println("getScan()->start(0)");
  }

  if (connected)
  {

  }
  else
  {
      digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
      failcount++;
      Serial.println("Not connected");
  }
      // lastTime = millis();
  //  }

  
  
  delay(delayy); // Delay a second between loops.

} // End of loop
