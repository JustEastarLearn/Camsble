bool isConnected = false;

//---------------------------------   BLE -_,-
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#define SERVICE_UUID        "fc1d6c97-b334-45bd-ad8e-964cf37fba58"

//---------------------------------
BLECharacteristic modeCs("d6b1d851-f38b-4a40-ab1a-323dca8b59c0", BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
BLECharacteristic triggerTimesCs("18a09d26-50df-4539-b0c5-5cc8190dfac3", BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
BLECharacteristic triggerDelayCs("2286b0e0-0fd4-470d-b07d-c2a04cac41c3", BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
BLECharacteristic interValCs("8d4f535a-ec08-4957-bde7-fe0d02fa235f", BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
BLECharacteristic scheduleCs("f3ab231f-d13c-49bb-a2dc-94bfbb1237cf", BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
BLECharacteristic bShutterCs("203e69eb-471b-40dc-8d75-7824e112165b", BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
BLECharacteristic selfieDelayCs("62f876a1-fbe9-4524-b548-6c3d1df6c4ad", BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

//---------------------------------   Callback -_,-
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {      
    isConnected = true;
    Serial.println("BLE Connected");
  };
  void onDisconnect(BLEServer* pServer) {
    isConnected = false;
    Serial.println("BLE Disconnected");
    BLEDevice::startAdvertising();
  }
};

class modeCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      changeModeUni(String(value.c_str()).toInt());
  }
};

class triggerTimesCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      Serial.println(value.c_str());
      triggerTimes = String(value.c_str()).toInt();
      preferences.putInt("triggerTimes", triggerTimes);      
      Serial.println(String("Change triggerTimes to:") + triggerTimes);
  }
};

class triggerDelayCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      triggerDelay = String(value.c_str()).toInt();
      preferences.putInt("triggerDelay", triggerDelay);      
      Serial.println(String("Change triggerDelay to:") + triggerDelay);
  }
};

class interValCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      interVal = String(value.c_str()).toInt();
      preferences.putInt("interVal", interVal);      
      Serial.println(String("Change interVal to:") + interVal);
  }
};

class scheduleCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      schedule = String(value.c_str()).toInt();
      preferences.putInt("schedule", schedule);      
      Serial.println(String("Change schedule to:") + schedule);
  }
};

class bShutterCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      bShutter = String(value.c_str()).toInt();
      preferences.putInt("bShutter", bShutter);      
      Serial.println(String("Change bShutter to:") + bShutter);
  }
};

class selfieDelayCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      selfieDelay = String(value.c_str()).toInt();
      preferences.putInt("selfieDelay", selfieDelay);      
      Serial.println(String("Change selfieDelay to:") + selfieDelay);
  }
};
//-----------------------------------


//---------------------------------- Init BLE -_,-
void initBLE(){
  
  Serial.println("Starting BLE work!");
  BLEDevice::init("Camsble");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *camsbleService = pServer->createService(BLEUUID(SERVICE_UUID),30,0);

  camsbleService->addCharacteristic(&modeCs);
  modeCs.addDescriptor(new BLE2902());
  camsbleService->addCharacteristic(&triggerTimesCs);
  triggerTimesCs.addDescriptor(new BLE2902());
  camsbleService->addCharacteristic(&triggerDelayCs);
  triggerDelayCs.addDescriptor(new BLE2902());
  camsbleService->addCharacteristic(&interValCs);
  interValCs.addDescriptor(new BLE2902());
  camsbleService->addCharacteristic(&scheduleCs);
  scheduleCs.addDescriptor(new BLE2902());
  camsbleService->addCharacteristic(&bShutterCs);
  bShutterCs.addDescriptor(new BLE2902());
  camsbleService->addCharacteristic(&selfieDelayCs);
  selfieDelayCs.addDescriptor(new BLE2902());

  // Start the service
  camsbleService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

// ----------------------- BLE init & notify -_,-

  modeCs.setCallbacks(new modeCallbacks());
  modeCs.setValue(String(mode).c_str());
  modeCs.notify();

  triggerTimesCs.setCallbacks(new triggerTimesCallbacks());
  triggerTimesCs.setValue(String(triggerTimes).c_str());
  triggerTimesCs.notify();

  triggerDelayCs.setCallbacks(new triggerDelayCallbacks());
  triggerDelayCs.setValue(String(triggerDelay).c_str());
  triggerDelayCs.notify();

  interValCs.setCallbacks(new interValCallbacks());
  interValCs.setValue(String(interVal).c_str());
  interValCs.notify();

  scheduleCs.setCallbacks(new scheduleCallbacks());
  scheduleCs.setValue(String(schedule).c_str());
  scheduleCs.notify();

  bShutterCs.setCallbacks(new bShutterCallbacks());
  bShutterCs.setValue(String(bShutter).c_str());
  bShutterCs.notify();

  selfieDelayCs.setCallbacks(new selfieDelayCallbacks());
  selfieDelayCs.setValue(String(selfieDelay).c_str());
  selfieDelayCs.notify();

//------------------------
}

void updateSchedule(int newVal){
  //scheduleCs.setValue(String(newVal).c_str());
  scheduleCs.notify();  
}

