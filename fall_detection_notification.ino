#define BLYNK_TEMPLATE_ID "TMPL36uBh0Fzo"
#define BLYNK_TEMPLATE_NAME "Fall Detection System using IoT"
#define BLYNK_AUTH_TOKEN "tkDRNsL7fft3aj391vxyboGBBVZwDT1F"

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#define RED_LED_PIN    25
#define GREEN_LED_PIN  27

char ssid[] = "OnePlus";
char pass[] = "1234512345@";

Adafruit_MPU6050 mpu;

#define FALL_THRESHOLD 2.0
#define IMPACT_THRESHOLD 20.0
#define FALL_CONFIRM_TIME 3000

bool falling = false;
unsigned long fallTime = 0;

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, pass);
  Blynk.config(BLYNK_AUTH_TOKEN);  // Non-blocking
  Blynk.connect();

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(GREEN_LED_PIN, HIGH);

  Serial.println("Initializing MPU6050...");
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1);
  }
  Serial.println("MPU6050 Initialized");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

  Serial.println("ESP32 Fall Detection System Ready!\n");
}

void loop() {
  Blynk.run();

  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  float totalAccel = sqrt(accel.acceleration.x * accel.acceleration.x +
                          accel.acceleration.y * accel.acceleration.y +
                          accel.acceleration.z * accel.acceleration.z);

  float roll  = atan2(accel.acceleration.y, accel.acceleration.z) * (180.0 / PI);
  float pitch = atan2(-accel.acceleration.x, 
                       sqrt(accel.acceleration.y * accel.acceleration.y + accel.acceleration.z * accel.acceleration.z)) * (180.0 / PI);

  const char* fallMsg = "Fall Detected by the Device, Report Immediately";
  
  if (totalAccel < FALL_THRESHOLD && !falling) {
    falling = true;
    fallTime = millis();
    Serial.println("Possible Fall Detected: Free Fall!");
    flashRedLED();
    Blynk.logEvent("fall_detected", fallMsg);
  }

  if (falling && totalAccel > IMPACT_THRESHOLD) {
    Serial.println("Impact Detected!");
    flashRedLED();
    Blynk.logEvent("fall_detected", fallMsg);
    fallTime = millis();
  }

  if (falling && (millis() - fallTime) > FALL_CONFIRM_TIME) {
    if (abs(roll) > 60 || abs(pitch) > 60) {
      Serial.println("Fall Confirmed! Alert Triggered!");
      flashRedLED();
      Blynk.logEvent("fall_detected", fallMsg);
    } else {
      Serial.println("False Alarm - User Moved");
    }
    falling = false;
  }

  Serial.print("Acceleration: "); Serial.print(totalAccel); Serial.println(" m/s²");
  Serial.print("Roll: "); Serial.print(roll); Serial.print("°, Pitch: "); Serial.print(pitch); Serial.println("°");
  Serial.println("------------------------------------------------------");
  delay(200);
}

void flashRedLED() {
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(GREEN_LED_PIN, LOW);
  delay(1000);  
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, HIGH);
}
