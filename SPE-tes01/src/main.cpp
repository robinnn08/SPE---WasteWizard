#include "header.h"
#include "firebase.cpp"
#include "function.cpp"

void taskDataSerial(void* parameter) {
  while (1) {
    if (xSemaphoreTake(cSemaphore, portMAX_DELAY)) {
      serialPrint();
      xSemaphoreGive(cSemaphore);
    }
    else {
      Serial.println("Failed to take semaphore");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void taskSegment(void* parameter) {
  while (1) {
    if (xSemaphoreTake(cSemaphore, portMAX_DELAY)) {
      Seven_Segment();
      xSemaphoreGive(cSemaphore);
    }
    else {
      Serial.println("Failed to take semaphore");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void taskPowerSwitch(void* parameter) {
  while (1) {
    if (xSemaphoreTake(cSemaphore, portMAX_DELAY)) {
      powerSwitching();
      xSemaphoreGive(cSemaphore);
    }
    else {
      Serial.println("Failed to take semaphore");
    }
    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}

void taskFirebase(void* parameter) {
  while (1) {
    if (xSemaphoreTake(mutex, portMAX_DELAY)) {
      checkAndReconnectWiFi();
      checkFirebaseConnection();
      sendFirebase();
      xSemaphoreGive(mutex);
    }
    else {
      Serial.println("Failed to take mutex");
    }
    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}

void setup() {
  Serial.begin(115200);

  // Relay
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);

  // UltraSonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // IR
  pinMode(IR_PIN, INPUT);

  // Servo
  servo.attach(SERVO_PIN);
  servo.write(0); // Close the bin

  // HX711
  Serial.println("Initializing Scale....");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calib);
  scale.tare();
  Serial.println("Scale Initialized");
  // 7 Segment
  Serial.println("Initializing 7 Segment....");
  disp.Initialize(10);
  disp.Clear();
  Serial.println("7 Segment Initialized");

  // GPS
  ss.begin(GPS_BAUD);

  wifiSetup();
  
  // NTP
  timeClient.begin();
  timeClient.update();
  timeClient.setTimeOffset(25200);

  firebaseSetup();
  delay(20000); // Untuk nunggu token dari firebase

  xTaskCreate(taskFirebase, "Firebase", configMINIMAL_STACK_SIZE + 20480, NULL, 0, NULL);
  xTaskCreate(taskDataSerial, "Serial", configMINIMAL_STACK_SIZE + 4096, NULL, 0, NULL);
  xTaskCreate(taskPowerSwitch, "Power", configMINIMAL_STACK_SIZE + 2048, NULL, 0, NULL);
  xTaskCreate(taskSegment, "Segment", configMINIMAL_STACK_SIZE + 2048, NULL, 0, NULL);
}

void loop() {
  Open_Bin();
}
