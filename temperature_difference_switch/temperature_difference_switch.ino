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

#define OUTPUT_PIN 2

#define C1 1.287720493e-03
#define C2 2.356959101e-04
#define C3 0.9520790711e-7

void setup() {
  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(THERMISTOR_1_PIN, INPUT);
  pinMode(THERMISTOR_2_PIN, INPUT);
  
  //Required for my arduino nano to run at the correct speed
  cli();
  CLKPR = 0x80;
  CLKPR = 0x01;
  sei();

  Serial.begin(9600);

  digitalWrite(OUTPUT_PIN, HIGH);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.setTextSize(2);      
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true);

  display.display();

}



void loop() {
  float voltage1 = analogRead(THERMISTOR_1_PIN);
  float voltage2 = analogRead(THERMISTOR_2_PIN);

  float temp1 = calculate_temperature(voltage1);
  float temp2 = calculate_temperature(voltage2);

  display_temperatures(temp1, temp2);

  if (temp2 - temp1 > TEMPERATURE_DELTA) { 
    digitalWrite(OUTPUT_PIN, LOW);
  } else {
    digitalWrite(OUTPUT_PIN, HIGH);
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

  display.setCursor(0, 0);

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
