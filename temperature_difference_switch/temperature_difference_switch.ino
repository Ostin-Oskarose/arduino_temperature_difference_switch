#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define KNOWN_RESISTOR 15000

#define TEMPERATURE_DELTA 2

#define THERMISTOR_1_PIN A1
#define THERMISTOR_2_PIN A2

#define C1 1.287720493e-03
#define C2 2.356959101e-04
#define C3 0.9520790711e-7

void setup() {
  pinMode(2, OUTPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  
  cli();  // This next section of code is timing critical, so interrupts are disabled  
  // Start the timed Sequence for configuring the clock prescaler
  CLKPR = 0x80;
  CLKPR = 0x01;
  sei();        // Enable interrupts

  Serial.begin(9600);

  digitalWrite(2, HIGH);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.display();

}



void loop() {
  float voltage1 = analogRead(THERMISTOR_1_PIN);
  float voltage2 = analogRead(THERMISTOR_2_PIN);

  float temp1 = calculate_temperature(voltage1);
  float temp2 = calculate_temperature(voltage2);

  display_temperatures(temp1, temp2);

  if (temp2 - temp1 > TEMPERATURE_DELTA) { 
    digitalWrite(2, LOW);
  } else {
    digitalWrite(2, HIGH);
  }

  delay(500);
}

float calculate_temperature(int voltage) {
  float resistance = KNOWN_RESISTOR * (4095.0 / (float)voltage - 1);

  float logResistance = log(resistance);

  float T = (1.0 / (C1 + C2*logResistance + C3*logResistance*logResistance*logResistance));

  T = T - 273.15;

  return T;
}

void display_temperatures(float temp1, float temp2) {
  display.clearDisplay();

  display.setCursor(0, 0);     // Start at top-left corner

  display.print("T1: ");
  display.print(temp1);
  display.print(" ");
  display.print((char)248);
  display.println("C");

  display.print("T2: ");
  display.print(temp2);
  display.print(" ");
  display.print((char)248);
  display.println("C");

  display.display();
}
