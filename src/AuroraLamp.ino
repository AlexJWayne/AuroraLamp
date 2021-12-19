#include <FastLED.h>
FASTLED_USING_NAMESPACE

#define LED_TYPE APA102
#define LED_DAT A5
#define LED_CLK A3
#define LED_COUNT 24
#define LED_COLOR_ORDER BGR

#define X 3
#define Y 8
#define NOISE_SCALE 1200
#define NOISE_SCALE_Z 64

CRGB leds[LED_COUNT];

int brightness = 255;

double speed = 1;

// Magic Rainbow
// Breathe
// Torch
String mode = "";

void setup() {
  FastLED.addLeds<LED_TYPE, LED_DAT, LED_CLK, LED_COLOR_ORDER, DATA_RATE_MHZ(20)>(leds, LED_COUNT);
  FastLED.setBrightness(brightness);

  Particle.variable("brightness", brightness);
  Particle.variable("speed", speed);
  Particle.variable("mode", mode);

  Particle.function("setSpeed_float", setSpeed_float);
  Particle.function("setBrightness_255", setBrightness_255);

  Particle.function("startMagicRainbow", startMagicRainbow);
  Particle.function("startBreathe", startBreathe);
  Particle.function("startTorch", startTorch);

  startMagicRainbow("");
}

int setSpeed_float(String arg) {
  speed = arg.toFloat();
  return 0;
}

int setBrightness_255(String arg) {
  brightness = arg.toInt();
  FastLED.setBrightness(brightness);
  return 0;
}

int startMagicRainbow(String arg) {
  mode = "Magic Rainbow";
  speed = 1;
  return 0;
}

int startBreathe(String arg) {
  mode = "Breathe";
  speed = 0.15;
  return 0;
}

int startTorch(String arg) {
  mode = "Torch";
  speed = 1;
  return 0;
}

void loop() {
  uint32_t now = millis() * speed;

  if (mode == "Magic Rainbow") loopMagicRainbow(now);
  if (mode == "Breathe") loopBreathe(now);
  if (mode == "Torch") loopTorch(now);

  FastLED.show();
}

void loopMagicRainbow(uint32_t now) {
  for (uint8_t x = 0; x < X; x++) {
    for (uint8_t y = 0; y < Y; y++) {
      uint8_t h = now * 0.02;
      h += y * 12;
      h += getNoise(now * 0.4, x, y) * 0.5;
      
      uint8_t s = getNoise(now * 0.35, x, y);
      s = map(s, 0, 255, 128, 255);

      uint8_t v = getNoise(now * 0.5, x, y);

      setLed(x, y, h, s, v);
    }
  }
}

void loopBreathe(uint32_t now) {
  for (uint8_t x = 0; x < X; x++) {
    for (uint8_t y = 0; y < Y; y++) {
      uint8_t h = now * 0.02;
      h += y * 12;
      h += getNoise(now * 0.8, x, y) * 0.5;
      
      uint8_t v = beatsin8(speed * 60, 0, 255, now, -y * 15);
      v = v < 120 ? 0 : map(v, 120, 255, 0, 255);

      setLed(x, y, h, 255, v);
    }
  }
}

void loopTorch(uint32_t now) {
  for (uint8_t x = 0; x < X; x++) {
    for (uint8_t y = 0; y < Y; y++) {
      uint8_t h = 0;

      switch (y) {
        case 0: h = 200;  break;
        case 1: h = 0;  break;
        case 2: h = 0;  break;
        case 3: h = 0;  break;
        case 4: h = 15;  break;
        case 5: h = 25;  break;
        case 6: h = 15;  break;
        case 7: h = 5;  break;
      }

      h += map(
        getNoise(now * 3.1, x, y),
        0, 255,
        0, 25
      );

      uint8_t v = getNoise(now * 4.9, x, y);
      v = map(v, 0, 255, 160, 255);

      setLed(x, y, h, 255, v);
    }
  }
}

uint8_t getNoise(uint32_t now, uint8_t x, uint8_t y) {
  return inoise8(
    cos8(x * 255 / X) * NOISE_SCALE,
    sin8(x * 255 / X) * NOISE_SCALE,
    -now + y * NOISE_SCALE_Z
  );
}

uint8_t noiseX(uint8_t x) {
  return cos8(x * 255 / X) * NOISE_SCALE;
}

uint8_t noiseY(uint8_t x) {
  return sin8(x * 255 / X) * NOISE_SCALE;
}


void setLed(uint8_t x, uint8_t y, uint8_t h, uint8_t s, uint8_t v) {
  if (x == 1) y = Y - y - 1;
  leds[x * Y + y] = CHSV(h, s, v);
}