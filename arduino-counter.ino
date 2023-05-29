#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

#include <HX711.h>
HX711 scale;
const int LOADCELL_DOUT_PIN = 8;
const int LOADCELL_SCK_PIN = 9;

#include <SoftwareSerial.h>
SoftwareSerial esp(10, 11); // RX, TX pins -> nodemcu

// init variables
int timer = 50;
float weight = 0;

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
  
  // calibration
  scale.set_scale(500);
  
  // set initial weight to zero
  scale.tare();

}

void loop() {

  // Read the weight from the load cell
  weight = scale.get_units();

  // serial display
  showWeightSerial(weight);

  // lcd display
  showWeightLCD(weight);

  // nodemcu send
  timer ++;

  if (timer > 60) {
    Serial.print("Sending data: ");
    Serial.println(weight);
    esp.print(weight);
    timer = 0;
  }

  // nodemcu text parse
  if (esp.available()) {
    String text = esp.readString();
    Serial.println(text);
  }

  // wait one sec for each loop
  delay(1000);

}

void showWeightSerial(float weight) {
  Serial.print("Weight: ");
  Serial.print(weight);
  Serial.println(" grams");
}

void showWeightLCD(int x) {
  lcd.setCursor(0, 1);
  lcd.print("Weight:          ");
  lcd.setCursor(8, 1);

  // if (x < 1000)
  //   lcd.print(" ");

  // if (x < 100)
  //   lcd.print(" ");

  // if (x < 10)
  //   lcd.print(" ");

  lcd.print(x);
}

void updateCapCount(int x) {
  lcd.setCursor(0, 0);
  lcd.print("Cap count: ");
  lcd.setCursor(11, 0);

  if (x < 100)
    lcd.print(" ");

  if (x < 10)
    lcd.print(" ");

  lcd.print(x);
}