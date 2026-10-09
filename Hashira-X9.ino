/*
 * =========================================================================================
 * █ █ ▄▀█ █▀ █░█ █ █▀█ ▄▀█   ▀▄▀ ▀█   █▀█ █▀
 * █▀█ █▀█ ▄█ █▀█ █ █▀▄ █▀█   █░█ ░▄▀  █▄█ ▄█
 * =========================================================================================
 * OS VERSION: 8.1.0 (Dark-Tech Patch + RADAR REMOVED)
 * CREATOR: Kartik
 * CORE LOGIC: Tri-State Quantum State Machine (Wind, Mist, Serpent)
 * HARDWARE PROFILES: RGB Mode Indicators. Sci-Fi Access SFX. Classic Doorbell.
 * MUSIC: "Abyssal Protocol" - Cinematic, Dark-Tech Boss Sequence (15s)
 * =========================================================================================
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include <avr/wdt.h>       
#include <avr/pgmspace.h>  

// ============================================================================
// [1] SYSTEM PIN MAP
// ============================================================================
// --- STATUS LEDS ---
#define LED_RED_PIN 0      // D0 - Serpent Mode (WARNING: UNPLUG TO UPLOAD!)
#define LED_GREEN_PIN A3   // A3 - Wind Mode
#define LED_BLUE_PIN 9     // D9 - Mist Mode

// --- SENSORS & MODULES ---
#define DHTPIN 2           
#define DHTTYPE DHT11      
#define RFID_RST_PIN 3     
// RADAR PIN REMOVED
#define TOUCH_PIN 5        
#define BUZZER_PIN 6       
#define IR_PIN 7           
#define LASER_PIN 8        
#define RFID_SS_PIN 10     
#define LDR_PIN A0         
#define GAS_PIN A1         
#define FIRE_PIN A2        

// ============================================================================
// [2] SECURITY & CALIBRATION 
// ============================================================================
String masterKey = "0C3FCB06"; 

const int GAS_THRESHOLD = 800;     
const int LDR_THRESHOLD = 500;     
const int FIRE_THRESHOLD = 200;    
const int TAMPER_SENSITIVITY = 8000; 
const int EEPROM_MODE_SLOT = 0;        

int currentMode = 1; 

// ============================================================================
// [3] GLOBAL OBJECTS & STATE VARIABLES
// ============================================================================
LiquidCrystal_I2C lcd(0x27, 16, 2); 
DHT dht(DHTPIN, DHTTYPE);
MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN); 

bool isSystemLocked = false; 
String currentThreat = "";

int16_t baseAcX = 0;
int16_t baseAcY = 0;

unsigned long currentMillis = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastSensorRead = 0;
unsigned long lastDoorbellTime = 0;
unsigned long lastIRWarning = 0;
unsigned long lastScanTime = 0;
unsigned long alarmFlashTimer = 0;

int cachedGas = 0;
int cachedFire = 0;
int cachedLDR = 0;
float cachedTemp = 0;
float cachedHum = 0;

// ============================================================================
// [4] PROGMEM GRAPHICS 
// ============================================================================
const byte heartShape[8] PROGMEM = { 0b01010, 0b11111, 0b11111, 0b11111, 0b01110, 0b00100, 0b00000, 0b00000 };
const byte crossSwords[8] PROGMEM = { 0b10001, 0b01010, 0b00100, 0b01010, 0b11011, 0b10001, 0b00000, 0b00000 };
const byte dagger[8] PROGMEM = { 0b00100, 0b00100, 0b11111, 0b01110, 0b00100, 0b00100, 0b00100, 0b00000 };

// ============================================================================
// [5] PROGMEM AUDIO: "ABYSSAL PROTOCOL" (Cinematic Dark Techno)
// ============================================================================
#define N_D4_SYS  294   // Reject Alarm Note
#define N_A4_SYS  440   // Warning Chime
#define N_G5_SYS  784   // System UI Note

// E Phrygian Scale - Deep, attractive, and menacing
#define N_E3  165  // The Heavy Drone
#define N_F3  175  // The Dark Step
#define N_G3  196  
#define N_B3  247  
#define N_C4  262  
#define N_E4  330  // The High Ping
#define N_F4  349  
#define REST  0

const uint16_t melody[] PROGMEM = {
  // Bar 1: Driving Low Pulse (Relentless)
  N_E3, N_E3, N_E3, N_F3, N_E3, N_E3, N_G3, N_F3,
  // Bar 2: Tech Arpeggio (Fast and Impressive)
  N_E3, N_E4, N_C4, N_B3, N_E3, N_E4, N_C4, N_B3,
  // Bar 3: Heavy Dissonant Boss Drops (Slow & Imposing)
  N_E3, REST, N_F3, REST, N_E3, N_B3, N_F3, N_E3,
  // Bar 4: High Cascading Glitch (Modern Tech)
  N_E4, N_F4, N_E4, N_C4, N_B3, N_G3, N_F3, N_E3
};

const uint8_t durations[] PROGMEM = {
  // Bar 1 (Punchy Mid-Tempo)
  2, 2, 2, 2, 2, 2, 2, 2,
  // Bar 2 (Fast Tech Sweep)
  1, 1, 1, 1, 1, 1, 1, 1,
  // Bar 3 (Heavy 4-beat drones mixed with short rests)
  4, 1, 4, 1, 2, 2, 2, 2,
  // Bar 4 (Cascading glitch run)
  1, 1, 1, 1, 1, 1, 1, 1
};
const int totalNotes = sizeof(melody) / sizeof(melody[0]);

// ============================================================================
// [6] SYSTEM KERNEL SETUP (DYNAMIC ARTICULATION ENGINE)
// ============================================================================
void setup() {
  wdt_disable(); 

  pinMode(TOUCH_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  // RADAR PIN REMOVED
  pinMode(LASER_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  
  digitalWrite(BUZZER_PIN, LOW);

  currentMode = EEPROM.read(EEPROM_MODE_SLOT);
  if (currentMode < 1 || currentMode > 3) currentMode = 1; 
  applyModeHardware(); 

  SPI.begin();      
  mfrc522.PCD_Init(); 
  lcd.init();
  lcd.backlight();
  lcd.setBacklight(HIGH); 
  dht.begin();
  
  // Gyroscope Calibration Sequence
  Wire.beginTransmission(0x68); Wire.write(0x6B); Wire.write(0); Wire.endTransmission(true);
  long tempX = 0; long tempY = 0;
  for(int i = 0; i < 50; i++) {
    Wire.beginTransmission(0x68); Wire.write(0x3B); Wire.endTransmission(false);
    Wire.requestFrom(0x68, 4, true); 
    tempX += (Wire.read()<<8 | Wire.read());  
    tempY += (Wire.read()<<8 | Wire.read());
    delay(20);
  }
  baseAcX = tempX / 50; baseAcY = tempY / 50;

  byte buffer[8];
  for(int i=0; i<8; i++) buffer[i] = pgm_read_byte(&(heartShape[i])); lcd.createChar(0, buffer);
  for(int i=0; i<8; i++) buffer[i] = pgm_read_byte(&(crossSwords[i])); lcd.createChar(1, buffer);
  for(int i=0; i<8; i++) buffer[i] = pgm_read_byte(&(dagger[i])); lcd.createChar(2, buffer);

  lcd.setCursor(5, 0); lcd.print(F("Hello!")); 
  lcd.setCursor(0, 1); lcd.print(F("Whats Up Kartik!")); 
  
  unsigned long bootStart = millis();
  bool emojisDrawn = false;
  bool titleDrawn = false;
  
  int beatDuration = 100; 

  while (millis() - bootStart < 15000) { 
    for (int i = 0; i < totalNotes; i++) {
      
      unsigned long elapsed = millis() - bootStart;
      
      if (!emojisDrawn && elapsed > 5000) { 
        lcd.clear();
        for(int col = 0; col < 16; col += 4) {
          lcd.setCursor(col, 0); lcd.write(0); lcd.write(1); lcd.write(0); lcd.write(2); 
          lcd.setCursor(col, 1); lcd.write(0); lcd.write(1); lcd.write(0); lcd.write(2); 
        }
        emojisDrawn = true;
      }
      if (!titleDrawn && elapsed > 10000) {
        lcd.clear();
        lcd.setCursor(1, 0); lcd.print(F("Hashira X9 OS"));
        lcd.setCursor(0, 1); lcd.print(F("Warming Sensors."));
        titleDrawn = true;
      }

      int note = pgm_read_word(&(melody[i]));
      int beats = pgm_read_byte(&(durations[i]));
      int noteDuration = beats * beatDuration;

      if (note != REST) {
        tone(BUZZER_PIN, note);
      }
      
      if (beats >= 4) {
        delay(noteDuration * 0.95); 
        noTone(BUZZER_PIN);
        delay(noteDuration * 0.05); 
      } else {
        delay(noteDuration * 0.60); 
        noTone(BUZZER_PIN);
        delay(noteDuration * 0.40); 
      }
      
      if(millis() - bootStart > 15000) break; 
    }
  }
  
  lcd.clear();
  wdt_enable(WDTO_4S); 
}

// ============================================================================
// [7] MAIN LOOP
// ============================================================================
void loop() {
  wdt_reset(); 
  currentMillis = millis(); 

  checkRFID();    

  if (isSystemLocked) {
    handleThreatLockdown();
  } else {
    if (currentMillis - lastSensorRead > 250) {
      pollSensorsSmoothly();
      lastSensorRead = currentMillis;
    }

    if (currentMode == 1) runWindMode();
    else if (currentMode == 2) runMistMode();
    else if (currentMode == 3) runSerpentMode();
  }
}

// ============================================================================
// [8] STATE MACHINES & HARDWARE
// ============================================================================
void applyModeHardware() {
  digitalWrite(LED_GREEN_PIN, LOW);
  digitalWrite(LED_BLUE_PIN, LOW);
  digitalWrite(LED_RED_PIN, LOW);
  digitalWrite(LASER_PIN, LOW);
  
  if (currentMode == 1) {
    digitalWrite(LED_GREEN_PIN, HIGH);     
  } else if (currentMode == 2) {
    digitalWrite(LED_BLUE_PIN, HIGH);      
  } else if (currentMode == 3) {
    digitalWrite(LED_RED_PIN, HIGH);       
    digitalWrite(LASER_PIN, HIGH);         
  }
}

void switchMode() {
  currentMode++;
  if (currentMode > 3) currentMode = 1;
  
  EEPROM.write(EEPROM_MODE_SLOT, currentMode);
  applyModeHardware();
  
  tone(BUZZER_PIN, 1047, 80); delay(100);  
  tone(BUZZER_PIN, 1318, 80); delay(100);  
  tone(BUZZER_PIN, 1568, 250); delay(250); 
  
  lcd.clear();
  lcd.setCursor(0, 0);
  if (currentMode == 1)      lcd.print(F(">> WIND MODE << "));
  else if (currentMode == 2) lcd.print(F(">> MIST MODE << "));
  else if (currentMode == 3) lcd.print(F(">> SERPENT MODE <<"));
  
  lcd.setCursor(0, 1);
  lcd.print(F("Protocol Engaged"));
  
  isSystemLocked = false;
  digitalWrite(BUZZER_PIN, LOW);
  
  lastScanTime = currentMillis;
  unsigned long tDelay = millis();
  while(millis() - tDelay < 2000) { wdt_reset(); } 
  lcd.clear();
}

// ============================================================================
// [9] RFID SYSTEM (OPTIMIZED KERNEL)
// ============================================================================
void checkRFID() {
  // Look for new cards. This function is non-blocking and lightning fast.
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
    
  String scannedUID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) scannedUID += "0";
    scannedUID += String(mfrc522.uid.uidByte[i], HEX);
  }
  scannedUID.toUpperCase(); 

  // Threat Evaluation
  if (scannedUID == masterKey) {
    if (currentMillis - lastScanTime > 2000) { 
      switchMode(); 
    }
  } else {
    tone(BUZZER_PIN, N_D4_SYS, 500); 
    delay(500); 
  }
    
  // --- MEMORY FLUSH: Proper halt AND encryption stop (This prevents freezes!) ---
  mfrc522.PICC_HaltA(); 
  mfrc522.PCD_StopCrypto1(); 
}

// ============================================================================
// [10] DATA ACQUISITION & ANTI-FLICKER UI
// ============================================================================
void pollSensorsSmoothly() {
  cachedGas = analogRead(GAS_PIN);
  cachedFire = analogRead(FIRE_PIN);
  cachedLDR = digitalRead(LDR_PIN); 
  
  static unsigned long lastDHTRead = 0;
  if (currentMillis - lastDHTRead > 2500) {
    cachedTemp = dht.readTemperature();
    cachedHum = dht.readHumidity();
    lastDHTRead = currentMillis;
  }
}

void printDashboard() {
  if (currentMillis - lastDisplayUpdate > 3000) {
    lcd.setCursor(0, 0); 
    lcd.print(F("T:")); lcd.print((int)cachedTemp); lcd.print(F("C H:")); lcd.print((int)cachedHum); lcd.print(F("%   "));
    
    lcd.setCursor(0, 1);
    if (currentMode == 1)      lcd.print(F("[WIND ACTIVE]   "));
    else if (currentMode == 2) lcd.print(F("[MIST ACTIVE]   "));
    else if (currentMode == 3) lcd.print(F("[SERPENT ACTIVE]"));
    
    lastDisplayUpdate = currentMillis;
  }
}

// ============================================================================
// [11] MODES & PERIMETER PROTOCOLS
// ============================================================================
void runWindMode() {
  checkDoorbell();
  printDashboard();
}

void runMistMode() {
  checkDoorbell();
  if (digitalRead(IR_PIN) == LOW && (currentMillis - lastIRWarning > 8000)) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("** WARNING **")); 
    lcd.setCursor(0, 1); lcd.print(F("Movement detected"));
    tone(BUZZER_PIN, N_A4_SYS, 200);
    lastIRWarning = currentMillis; 
    lastDisplayUpdate = currentMillis + 2000;
  }
  if (cachedFire < FIRE_THRESHOLD) triggerDisaster(F("Fire detected"));
  if (cachedGas > GAS_THRESHOLD)   triggerDisaster(F("Gas leaked"));
  
  Wire.beginTransmission(0x68); Wire.write(0x3B); Wire.endTransmission(false);
  Wire.requestFrom(0x68, 4, true); 
  int16_t AcX = Wire.read()<<8 | Wire.read();  
  int16_t AcY = Wire.read()<<8 | Wire.read();  
  if (abs(AcX - baseAcX) > TAMPER_SENSITIVITY || abs(AcY - baseAcY) > TAMPER_SENSITIVITY) triggerDisaster(F("Earthquake      "));
  
  printDashboard();
}

void runSerpentMode() {
  checkDoorbell();
  if (cachedFire < FIRE_THRESHOLD) triggerDisaster(F("FIRE DETECTED!"));
  if (cachedGas > GAS_THRESHOLD)   triggerDisaster(F("GAS DETECTED!"));
  
  Wire.beginTransmission(0x68); Wire.write(0x3B); Wire.endTransmission(false);
  Wire.requestFrom(0x68, 4, true); 
  int16_t AcX = Wire.read()<<8 | Wire.read();  
  int16_t AcY = Wire.read()<<8 | Wire.read();  
  if (abs(AcX - baseAcX) > TAMPER_SENSITIVITY || abs(AcY - baseAcY) > TAMPER_SENSITIVITY) triggerDisaster(F("TAMPER DETECTED!"));
  
  if (cachedLDR == HIGH) triggerDisaster(F("LASER BREACH!"));
  // RADAR TRIGGER REMOVED
  if (digitalRead(IR_PIN) == LOW) triggerDisaster(F("IR BREACH!"));
  
  printDashboard();
}

// ============================================================================
// [12] UTILITIES & ALARMS
// ============================================================================
void checkDoorbell() {
  if (digitalRead(TOUCH_PIN) == HIGH) {
    if (currentMillis - lastDoorbellTime > 5000) { 
      lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("++ DOORBELL ++")); lcd.setCursor(0, 1); lcd.print(F("VISITOR AT DOOR!"));
      
      tone(BUZZER_PIN, 659, 400); 
      unsigned long t = millis(); while(millis()-t<450) wdt_reset(); 
      tone(BUZZER_PIN, 523, 600); 
      
      lastDoorbellTime = currentMillis; lastDisplayUpdate = currentMillis + 1500; 
    }
  }
}

void triggerDisaster(const __FlashStringHelper* reason) {
  if (!isSystemLocked) {
    isSystemLocked = true;
    currentThreat = String(reason);
    
    digitalWrite(LED_GREEN_PIN, LOW); 
    digitalWrite(LED_BLUE_PIN, LOW); 
    digitalWrite(LED_RED_PIN, LOW); 
    
    lcd.clear();
  }
}

void handleThreatLockdown() {
  if (currentMillis - alarmFlashTimer > 150) {
    alarmFlashTimer = currentMillis;
    static bool flip = false; flip = !flip;
    
    if (flip) {
      lcd.setCursor(0, 0); lcd.print(F("!!! ALERT !!!   "));
      lcd.setCursor(0, 1); lcd.print(currentThreat);
      for(unsigned int i = currentThreat.length(); i < 16; i++) lcd.print(" ");
      tone(BUZZER_PIN, 659); 
    } else {
      lcd.setCursor(0, 0); lcd.print(F("                ")); 
      lcd.setCursor(0, 1); lcd.print(F("                ")); 
      tone(BUZZER_PIN, 784); 
    }
  }
}
