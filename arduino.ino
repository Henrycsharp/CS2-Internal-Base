#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const int buzzerPin = 8;

String receivedData = "";

float distance = 0.0;
unsigned long lastBeepTime = 0;
float beepInterval = 500;
unsigned long lastDataTime = 0;
const unsigned long dataTimeout = 1000;
bool dataActive = false;
const unsigned long beepDuration = 50;

byte forward[] = {
  B00100,
  B01110,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000
};

byte back[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B01110,
  B00100
};

byte right[] = {
  B00000,
  B01100,
  B01110,
  B01111,
  B01110,
  B01100,
  B00000,
  B00000
};

byte left[] = {
  B00000,
  B00110,
  B01110,
  B11110,
  B01110,
  B00110,
  B00000,
  B00000
};

byte forwardRight[] = {
  B11111,
  B01111,
  B00111,
  B00011,
  B00001,
  B00000,
  B00000,
  B00000
};

byte forwardLeft[] = {
  B11111,
  B11110,
  B11100,
  B11000,
  B10000,
  B00000,
  B00000,
  B00000
};

byte backLeft[] = {
  B00000,
  B00000,
  B00000,
  B10000,
  B11000,
  B11100,
  B11110,
  B11111
};

byte backRight[] = {
  B00000,
  B00000,
  B00000,
  B00001,
  B00011,
  B00111,
  B01111,
  B11111
};

void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  tone(buzzerPin, 1000, 50);
  noTone(buzzerPin);
  lcd.begin(16, 2);
  lcd.createChar(0, forward);
  lcd.createChar(1, back);
  lcd.createChar(2, left);
  lcd.createChar(3, right);
  lcd.createChar(4, forwardRight);
  lcd.createChar(5, forwardLeft);
  lcd.createChar(6, backLeft);
  lcd.createChar(7, backRight);
  lcd.clear();
  lcd.write(byte(0)); 
  lcd.setCursor(1, 0);
  lcd.write(byte(1)); 
  lcd.setCursor(2, 0);
  lcd.write(byte(2)); 
  lcd.setCursor(3, 0);
  lcd.write(byte(3)); 
  lcd.setCursor(4, 0);
  lcd.write(byte(4)); 
  lcd.setCursor(5, 0);
  lcd.write(byte(5)); 
  lcd.setCursor(6, 0);
  lcd.write(byte(6)); 
  lcd.setCursor(7, 0);
  lcd.write(byte(7)); 
  lcd.setCursor(0, 1);
  lcd.print("Welcome");
  lcd.setCursor(8, 1);
  lcd.print("V2.22");
}

void loop() {
  static unsigned long lastProcessTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastProcessTime >= 200) {
    lastProcessTime = currentTime;

    while (Serial.available() > 0) {
      char c = Serial.read();
      if (c == '\n') {
        lastDataTime = currentTime;
        dataActive = true;

        lcd.clear();
        lcd.setCursor(0, 0);

        int spaceIndex = receivedData.indexOf(' ');
        String dir = "";
        if (spaceIndex > 0) {
          dir = receivedData.substring(0, spaceIndex);
        } else {
          dir = receivedData;
        }

        if (dir == "F") {
          lcd.write(byte(0));
        } else if (dir == "B") {
          lcd.write(byte(1));
        } else if (dir == "L") {
          lcd.write(byte(2));
        } else if (dir == "R") {
          lcd.write(byte(3));
        } else if (dir == "FR") {
          lcd.write(byte(4));
        } else if (dir == "FL") {
          lcd.write(byte(5));
        } else if (dir == "BL") {
          lcd.write(byte(6));
        } else if (dir == "BR") {
          lcd.write(byte(7));
        } else {
          lcd.print(dir);
        }

        if (spaceIndex > 0 && spaceIndex < receivedData.length() - 1) {
          String distStr = receivedData.substring(spaceIndex + 1);
          distance = distStr.toFloat();

          lcd.setCursor(0, 1);
          lcd.print(distance, 2);
          lcd.print(" m"); 

          float minDistance = 0.0;
          float maxDistance = 50.0;
          float minInterval = 10.0;
          float maxInterval = 1000.0;

          if (distance < minDistance) distance = minDistance;
          if (distance > maxDistance) distance = maxDistance;

          beepInterval = minInterval + (distance - minDistance) * (maxInterval - minInterval) / (maxDistance - minDistance);
        }

        receivedData = "";
      } else {
        receivedData += c;
      }
    }
  }

  if (currentTime - lastDataTime >= dataTimeout) {
    if (dataActive) {
      dataActive = false;
      noTone(buzzerPin);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("No data");
    }
  }

  if (dataActive) {
    unsigned long timeSinceLastBeep = currentTime - lastBeepTime;
    if (timeSinceLastBeep >= beepInterval + beepDuration) {
      lastBeepTime = currentTime;
      tone(buzzerPin, 1000, beepDuration);
    }
  }
}

