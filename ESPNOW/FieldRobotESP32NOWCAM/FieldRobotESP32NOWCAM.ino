#include <credentials.h> //to hide sensitive credentials
#include <esp_now.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include <WiFiClient.h>
#include "esp_camera.h"
#include "esp_http_server.h"
#include <ESP32Servo.h>

#define CAMERA_MODEL_AI_THINKER // Has PSRAM

#include "camera_pins.h"

const char *ssid = mySSID; // values sotred in credentials.h
const char *password = myPASSWORD ;

void startCameraServer();
void setupLedFlash(int pin);

int MOTOR_CALIBRATION_DELAY = 5000;

// #define JOYSTICK_MOVE_PIN 34
// #define JOYSTICK_STEER_PIN 35
// Structure example to send data
typedef struct struct_message {
  int x;
  int y;
  // int flash;
} struct_message;

// Create a struct_message called myData
struct_message myData;

void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  // Serial.print("Bytes received: ");
  // Serial.print(len);
  // Serial.print("|| Int X: ");
  // Serial.print(myData.x);
  // Serial.print("|| Int Y: ");
  // Serial.println(myData.y);
  // Serial.println();
}

int sliderValue, joystick_steer_pos;
int flash, pic;



#define ESC1_PIN 12
#define ESC2_PIN 13
Servo esc1;
Servo esc2;

const int neutralSignal = 90; // Typically 90 is stop for bidirectional ESCs
const int signalRange = 90;   // 0–180 degrees = full range
// callback function that will be executed when data is received


void setup() {
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  Serial.println();
  

// ////Camera stuff////////////////////////////
// camera_config_t config;
//   config.ledc_channel = LEDC_CHANNEL_0;
//   config.ledc_timer = LEDC_TIMER_0;
//   config.pin_d0 = Y2_GPIO_NUM;
//   config.pin_d1 = Y3_GPIO_NUM;
//   config.pin_d2 = Y4_GPIO_NUM;
//   config.pin_d3 = Y5_GPIO_NUM;
//   config.pin_d4 = Y6_GPIO_NUM;
//   config.pin_d5 = Y7_GPIO_NUM;
//   config.pin_d6 = Y8_GPIO_NUM;
//   config.pin_d7 = Y9_GPIO_NUM;
//   config.pin_xclk = XCLK_GPIO_NUM;
//   config.pin_pclk = PCLK_GPIO_NUM;
//   config.pin_vsync = VSYNC_GPIO_NUM;
//   config.pin_href = HREF_GPIO_NUM;
//   config.pin_sccb_sda = SIOD_GPIO_NUM;
//   config.pin_sccb_scl = SIOC_GPIO_NUM;
//   config.pin_pwdn = PWDN_GPIO_NUM;
//   config.pin_reset = RESET_GPIO_NUM;
//   config.xclk_freq_hz = 20000000;
//   config.frame_size = FRAMESIZE_UXGA;
//   config.pixel_format = PIXFORMAT_JPEG;  // for streaming
//   //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
//   config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
//   config.fb_location = CAMERA_FB_IN_PSRAM;
//   config.jpeg_quality = 12;
//   config.fb_count = 1;

//   // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
//   //                      for larger pre-allocated frame buffer.
//   if (config.pixel_format == PIXFORMAT_JPEG) {
//     if (psramFound()) {
//       config.jpeg_quality = 10;
//       config.fb_count = 2;
//       config.grab_mode = CAMERA_GRAB_LATEST;
//     } else {
//       // Limit the frame size when PSRAM is not available
//       config.frame_size = FRAMESIZE_SVGA;
//       config.fb_location = CAMERA_FB_IN_DRAM;
//     }
//   } else {
//     // Best option for face detection/recognition
//     config.frame_size = FRAMESIZE_240X240;
//   #if CONFIG_IDF_TARGET_ESP32S3
//       config.fb_count = 2;
//   #endif
//     }

//   #if defined(CAMERA_MODEL_ESP_EYE)
//     pinMode(13, INPUT_PULLUP);
//     pinMode(14, INPUT_PULLUP);
//   #endif

//     // camera init
//     esp_err_t err = esp_camera_init(&config);
//     if (err != ESP_OK) {
//       Serial.printf("Camera init failed with error 0x%x", err);
//       return;
//     }

//     sensor_t *s = esp_camera_sensor_get();
//     s->set_hmirror(s, 1);
//     s->set_vflip(s, 1);

//     // initial sensors are flipped vertically and colors are a bit saturated
//     if (s->id.PID == OV3660_PID) {
//       s->set_vflip(s, 1);        // flip it back
//       s->set_brightness(s, 1);   // up the brightness just a bit
//       s->set_saturation(s, -2);  // lower the saturation
//     }
//     // drop down frame size for higher initial frame rate
//     if (config.pixel_format == PIXFORMAT_JPEG) {
//       s->set_framesize(s, FRAMESIZE_SVGA);
//     }

//   #if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
//     s->set_vflip(s, 1);
//     s->set_hmirror(s, 1);
//   #endif

//   #if defined(CAMERA_MODEL_ESP32S3_EYE)
//     s->set_vflip(s, 1);
//   #endif
//   // Setup LED FLash if LED pin is defined in camera_pins.h
//   #if defined(LED_GPIO_NUM)
//     setupLedFlash(LED_GPIO_NUM);
//   #endif


///wifi setup
  WiFi.mode(WIFI_STA);  // Station mode only (no AP or client)
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);  // Match the transmitter's channel
  esp_wifi_set_promiscuous(false);

  // WiFi.begin(ssid, password);
  // WiFi.setSleep(false);

  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }
  // Serial.println("");
  // Serial.println("WiFi connected");

  // startCameraServer();
  // Serial.print("Camera Ready! Use 'http://");
  // Serial.print(WiFi.localIP());
  // Serial.println("' to connect");
  // Serial.println("Code is ready. The video stream is located in the URL: ");
  // String u = "http://"+WiFi.localIP().toString()+":81/stream";//goes directly into video feed, not the camsera server UI
  // Serial.print(u);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
    }
 
  // pinMode(4, OUTPUT);
  setupMotors();
  calibrateMotors();
  
  Serial.println("Receiver ready.");
  // Register the receive callback
  esp_now_register_recv_cb(OnDataRecv);
  delay(1000);
}//end of setup

void loop() {

  int sliderValue = myData.y;
  int joystick_steer_pos = myData.x;
  delay(10);

  int offsetThrottle = sliderValue - 2048;
  int offsetSteering = joystick_steer_pos - 2048;

  int throttleCmd = 0;
  int steerCmd = 0;
  float powerLimit = 0.3;

  if (abs(offsetThrottle) > 600) {
    throttleCmd = map(offsetThrottle, -2048, 2048, -90, 90);
    delay(10);
  }
  if (abs(offsetSteering) > 600) {
    steerCmd = map(offsetSteering, -2048, 2048, -90, 90);
    delay(10);
  }

  float pwmLeft  = neutralSignal + (throttleCmd + steerCmd) * powerLimit;
  float pwmRight = neutralSignal + (throttleCmd - steerCmd) * powerLimit;

  esc1.write(pwmLeft);
  esc2.write(pwmRight);

  // Serial.print("neutralSignal: "); Serial.print(neutralSignal);
  // Serial.print("  throttleCmd: "); Serial.print(throttleCmd);
  // Serial.print("  steerCmd: "); Serial.print(steerCmd);
  // Serial.print("  powerLimit: "); Serial.print(powerLimit);

  // Serial.print("||  Left: ");
  // Serial.print(pwmLeft);
  // Serial.print("||   Right: ");
  // Serial.println(pwmRight);

  // digitalWrite(4, flash == 1 ? HIGH : LOW);

  // if (pic == 1) {
  //   Serial.println("");
  //   Serial.println("Taking pic");
  //   Serial.println("");
  //   delay(500);
  // }
}//end of loop

void setupMotors() {
  esc1.setPeriodHertz(60);
  esc2.setPeriodHertz(60);

  esc1.attach(ESC1_PIN, 500, 2500);
  esc2.attach(ESC2_PIN, 500, 2500);
}

void calibrateMotors() {
  // Serial.println("Setting maximum throttle...");
  // esc1.writeMicroseconds(2000);
  // esc2.writeMicroseconds(2000);
  // delay(MOTOR_CALIBRATION_DELAY);

  // Serial.println("Setting minimum throttle...");
  // esc1.writeMicroseconds(500);
  // esc2.writeMicroseconds(500);
  // delay(MOTOR_CALIBRATION_DELAY);

  //alternate calibration
  esc1.writeMicroseconds(2000); // max signal
  delay(2000);
  esc1.writeMicroseconds(1000); // min signal
  delay(2000);
  esc1.writeMicroseconds(1500); // neutral
  delay(1000);

}

void camStuff(){
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if (config.pixel_format == PIXFORMAT_JPEG) {
      if (psramFound()) {
        config.jpeg_quality = 10;
        config.fb_count = 2;
        config.grab_mode = CAMERA_GRAB_LATEST;
      } else {
        // Limit the frame size when PSRAM is not available
        config.frame_size = FRAMESIZE_SVGA;
        config.fb_location = CAMERA_FB_IN_DRAM;
      }
    } else {
      // Best option for face detection/recognition
      config.frame_size = FRAMESIZE_240X240;
    #if CONFIG_IDF_TARGET_ESP32S3
        config.fb_count = 2;
    #endif
    }

  #if defined(CAMERA_MODEL_ESP_EYE)
    pinMode(13, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
  #endif

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
      Serial.printf("Camera init failed with error 0x%x", err);
      return;
    }

    sensor_t *s = esp_camera_sensor_get();
    s->set_hmirror(s, 1);
    s->set_vflip(s, 1);

    // initial sensors are flipped vertically and colors are a bit saturated
    if (s->id.PID == OV3660_PID) {
      s->set_vflip(s, 1);        // flip it back
      s->set_brightness(s, 1);   // up the brightness just a bit
      s->set_saturation(s, -2);  // lower the saturation
    }
    // drop down frame size for higher initial frame rate
    if (config.pixel_format == PIXFORMAT_JPEG) {
      s->set_framesize(s, FRAMESIZE_SVGA);
    }

  #if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
  #endif

  #if defined(CAMERA_MODEL_ESP32S3_EYE)
    s->set_vflip(s, 1);
  #endif

  // Setup LED FLash if LED pin is defined in camera_pins.h
  #if defined(LED_GPIO_NUM)
    setupLedFlash(LED_GPIO_NUM);
  #endif

    WiFi.begin(ssid, password);
    WiFi.setSleep(false);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    startCameraServer();
  }