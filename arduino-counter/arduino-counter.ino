#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

#include <HX711.h>
HX711 scale;
const int LOADCELL_DOUT_PIN = 8;
const int LOADCELL_SCK_PIN = 9;
const int TARE_BTN = 10;

#include <SoftwareSerial.h>
SoftwareSerial esp(12, 13); // RX, TX pins -> nodemcu

#include <Math.h>

// init variables
int timer = 1;
int timeout = 10;
float weight = 0;
int caps = 0;

void setup() {
  
  // set lcd screen with 2 rows 16 columns
  lcd.begin(16, 2);
  // init caps
  updateCapCount(0);

  // serials
  Serial.begin(115200);
  esp.begin(115200);
  
  // init load cell
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  pinMode(TARE_BTN, INPUT_PULLUP);
  
  // calibration
  scale.set_scale(500);
  
  // set initial weight to zero
  scale.tare();

}

void loop() {

  // Set weight to 0 with button press
  if (digitalRead(TARE_BTN) == LOW) {
    scale.tare();
  }

  // Read the weight from the load cell
  weight = scale.get_units();
  caps = round(weight/30);

  if (caps < 0) caps = 0;

  // lcd display
  showWeightLCD(weight);
  updateCapCount(caps);

  // nodemcu send
  timer ++;

  if (timer > timeout) {
    Serial.println("Sending data: " + String(caps));
    esp.print(caps);
    timer = 1;
  }

  // nodemcu text parser
  if (esp.available()) {

    String text = esp.readString();

    if (text == "tare") {
      scale.tare();
    }
    else if (text.substring(0,7) == "timeout") {
      timer = 1;
      timeout = text.substring(8).toInt();

      if (timeout < 10) timeout = 10;

      Serial.println("New timeout: " + String(timeout));
    }
  }

  // wait one sec for each loop
  delay(1000);
}

// print weight on second line of LCD screen
void showWeightLCD(float x) {

  lcd.setCursor(0, 1);
  lcd.print("W:          ");

  lcd.setCursor(4, 1);
  lcd.print(x);

}

// print cap count on first line of LCD screen
void updateCapCount(int x) {

  lcd.setCursor(0, 0);
  lcd.print("Cap count:      ");

  lcd.setCursor(11, 0);

  if (x < 10)
    lcd.print(" ");

  lcd.print(x);

}