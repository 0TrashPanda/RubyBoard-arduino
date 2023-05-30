#include <Wire.h>
#include <Adafruit_NeoPixel.h>

const int numExpanders = 3;
const int expanderAddresses[numExpanders] = { 0x20, 0x21, 0x22 };
const int ledPin = 27;
#define NUMPIXELS 39

Adafruit_NeoPixel pixels(NUMPIXELS, ledPin, NEO_GRB + NEO_KHZ800);

byte previousState[numExpanders][2] = { {0} };  // Array to store previous button states for each port
int currentLED = 0;  // Variable to keep track of the current LED being lit

void setup() {
    pixels.begin();
    pixels.clear();

    Wire1.setSDA(28);  // Set SDA pin for Wire1
    Wire1.setSCL(29);  // Set SCL pin for Wire1
    Wire1.begin();     // Initialize Wire1

    // Iterate over each expander
    for (int i = 0; i < numExpanders; i++) {
        Wire1.beginTransmission(expanderAddresses[i]);
        Wire1.write(0x06);  // Register address for configuration
        Wire1.write(0xFF);  // Set all pins as inputs with pull-up resistors
        Wire1.endTransmission();
    }

    // Initialize serial communication
    Serial.begin(9600);
}

void loop() {
    // Turn on the current LED
    pixels.setPixelColor(currentLED, pixels.Color(140, 10, 20));
    pixels.show();
    delay(50);

    bool pressed = false;

    while (pressed == false) {

        // Iterate over each expander
        for (int i = 0; i < numExpanders; i++) {
            // Read pins from port 0
            Wire1.beginTransmission(expanderAddresses[i]);
            Wire1.write(0x00);  // Register address for input port 0
            Wire1.endTransmission();

            Wire1.requestFrom(expanderAddresses[i], 1);  // Request 1 byte of data
            byte inputStatePort0 = Wire1.read();  // Read the input state for port 0

            // Read pins from port 1
            Wire1.beginTransmission(expanderAddresses[i]);
            Wire1.write(0x01);  // Register address for input port 1
            Wire1.endTransmission();

            Wire1.requestFrom(expanderAddresses[i], 1);  // Request 1 byte of data
            byte inputStatePort1 = Wire1.read();  // Read the input state for port 1

            // Iterate over each pin of port 0
            for (int pin = 0; pin < 8; pin++) {
                byte currentState = bitRead(inputStatePort0, pin);
                byte prevState = bitRead(previousState[i][0], pin);

                // Check if the button state has changed
                if (currentState == LOW && prevState == HIGH) {
                    pressed = true;
                    // Button is pressed on this pin and expander (port 0)
                    Serial.print("Button pressed on expander ");
                    Serial.print(expanderAddresses[i], HEX);
                    Serial.print(", port 0, pin ");
                    Serial.print(pin);
                    Serial.print(" [");
                    Serial.print(currentLED);
                    Serial.println("]");
                }

                bitWrite(previousState[i][0], pin, currentState);  // Update the previous button state for port 0
            }

            // Iterate over each pin of port 1
            for (int pin = 0; pin < 8; pin++) {
                byte currentState = bitRead(inputStatePort1, pin);
                byte prevState = bitRead(previousState[i][1], pin);

                // Check if the button state has changed
                if (currentState == LOW && prevState == HIGH) {
                    // Button is pressed on this pin and expander (port 1)
                    pressed = true;
                    Serial.print("Button pressed on expander ");
                    Serial.print(expanderAddresses[i], HEX);
                    Serial.print(", port 1, pin ");
                    Serial.print(pin);
                    Serial.print(" [");
                    Serial.print(currentLED);
                    Serial.println("]");
                }

                bitWrite(previousState[i][1], pin, currentState);  // Update the previous button state for port 1
            }
        }
    }

    // Turn off the current LED
    pixels.setPixelColor(currentLED, pixels.Color(0, 0, 0));
    pixels.show();
    delay(50);

    currentLED++;  // Move to the next LED
    if (currentLED >= NUMPIXELS) {
        currentLED = 0;  // Reset the LED count
    }
}


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
