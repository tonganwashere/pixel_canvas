#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <FastLED.h>

#define LED_PIN 16
#define NUM_LEDS 256

CRGB leds[NUM_LEDS];
uint8_t mode = 0; 
// 0 = draw, 1 = fire, 2 = rainbow

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define RX_UUID      "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

// quadrant position setup
uint16_t XY(uint8_t x, uint8_t y) {
  if (x >= 16 || y >= 16) return 0;
  uint8_t quad = (x < 8 ? 0 : 1) + (y < 8 ? 0 : 2);
  return (quad * 64) + ((7 - (y % 8)) * 8 + (7 - (x % 8)));
}

class RxHandler : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *c) {
    String s = c->getValue().c_str();
    if (!s.length()) return;

    if (s == "CLEAR") { mode = 0; FastLED.clear(true); }
    else if (s.startsWith("M,")) { mode = s.substring(2).toInt(); FastLED.clear(true); }
    else if (s.startsWith("B,")) { FastLED.setBrightness(s.substring(2).toInt()); FastLED.show(); }
    else {

      // format: x,y,r,g,b
      int p[4] = {s.indexOf(','), -1, -1, -1};
      for (int i = 1; i < 4; i++) p[i] = s.indexOf(',', p[i - 1] + 1);
      if (p[3] > 0) {
        mode = 0;
        uint8_t x = s.substring(0, p[0]).toInt();
        uint8_t y = s.substring(p[0] + 1, p[1]).toInt();
        uint8_t r = s.substring(p[1] + 1, p[2]).toInt();
        uint8_t g = s.substring(p[2] + 1, p[3]).toInt();
        uint8_t b = s.substring(p[3] + 1).toInt();
        leds[XY(x, y)] = CRGB(r, g, b);
        FastLED.show();
      }
    }
  }
};

void runFire() {
  static byte h[16][16];
  for (int x = 0; x < 16; x++)
    for (int y = 15; y >= 2; y--)
      h[x][y] = (h[x][y - 1] + h[x][y - 2]) / 2;
  for (int x = 0; x < 16; x++)
    h[x][0] = random8(160, 255);
  for (int x = 0; x < 16; x++)
    for (int y = 0; y < 16; y++)
      leds[XY(x, 15 - y)] = HeatColor(h[x][y]);
  FastLED.show();
  delay(30);
}

void runRainbow() {
  static uint8_t hue = 0;
  for (int x = 0; x < 16; x++)
    for (int y = 0; y < 16; y++)
      leds[XY(x, y)] = CHSV(hue + (x + y) * 8, 255, 255);
  hue += 2;
  FastLED.show();
  delay(20);
}

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(20);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 450);
  FastLED.clear(true);

  BLEDevice::init("PixelArt-BLE");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pRx = pService->createCharacteristic(RX_UUID, BLECharacteristic::PROPERTY_WRITE_NR);
  pRx->setCallbacks(new RxHandler());
  pService->start();

  BLEAdvertising *pAdv = BLEDevice::getAdvertising();
  pAdv->addServiceUUID(SERVICE_UUID);
  pAdv->start();
}

void loop() {
  if (mode == 1) runFire();
  else if (mode == 2) runRainbow();
  else delay(20);
}