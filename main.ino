#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SHT4x.h>

Adafruit_SHT4x sht4 = Adafruit_SHT4x();

void setup() {
  Serial.begin(115200);
  delay(2000); // Čas na otvorenie konzoly

  Serial.println("\n--- SPUSTAM TEST ORIGINALITY SHT41 (OPRAVENY) ---");

  // Nastavenie pinov D4 (SDA) a D5 (SCL)
  Wire.begin(D4, D5);

  if (! sht4.begin()) {
    Serial.println("CHYBA: Senzor neodpovedá. Skontrolujte káble.");
    while (1) delay(10);
  }

  // --- KROK 1: KONTROLA SÉRIOVÉHO ČÍSLA ---
  // OPRAVA: Používame funkciu readSerial(), nie premennú
  uint32_t serialNum = sht4.readSerial();
  
  Serial.print("1. Sériové číslo: 0x");
  Serial.println(serialNum, HEX);

  if (serialNum == 0 || serialNum == 0xFFFFFFFF) {
    Serial.println("   [!] VAROVANIE: Podozrivé sériové číslo (0 alebo F). Možný klon.");
  } else {
    Serial.println("   [OK] Sériové číslo vyzerá validne.");
  }

  // --- KROK 2: REFERENČNÉ MERANIE (BEZ OHREVU) ---
  Serial.println("\n2. Pripravujem test ohrievača...");
  sht4.setHeater(SHT4X_NO_HEATER);
  
  sensors_event_t humidity, temp;
  sht4.getEvent(&humidity, &temp);
  
  float startTemp = temp.temperature;
  float startHum = humidity.relative_humidity;
  
  Serial.print("   Teplota pred ohrevom: "); Serial.print(startTemp); Serial.println(" °C");
  Serial.print("   Vlhkosť pred ohrevom: "); Serial.print(startHum); Serial.println(" %");

  // --- KROK 3: ZAPNUTIE OHREVU (High Power, 1 sekunda) ---
  Serial.println("   --> Zapínam ohrev na MAX (cca 1 sekunda)...");
  
  // Nastavíme najvyšší výkon ohrevu na krátky čas
  sht4.setHeater(SHT4X_HIGH_HEATER_1S);
  
  // getEvent vykoná príkaz ohrevu a následne zmeria hodnoty
  sht4.getEvent(&humidity, &temp);

  float heatTemp = temp.temperature;
  float heatHum = humidity.relative_humidity;

  Serial.print("   Teplota PO ohreve: "); Serial.print(heatTemp); Serial.println(" °C");
  
  // --- VYHODNOTENIE ---
  Serial.println("\n--- VÝSLEDOK TESTU ---");
  
  float tempDiff = heatTemp - startTemp;
  
  Serial.print("Nárast teploty: "); 
  Serial.print(tempDiff); 
  Serial.println(" °C");

  if (tempDiff > 5.0) { 
    // Originál sa zohreje výrazne (často o viac ako 10-20 °C, ak je čip voľne na vzduchu)
    Serial.println("[ PASS ] Ohrievač funguje správne. Senzor je pravdepodobne ORIGINÁL.");
  } else if (tempDiff > 0.5) {
    Serial.println("[ ? ] Teplota stúpla len mierne. Skúste test zopakovať.");
  } else {
    Serial.println("[ FAIL ] Teplota sa nezmenila. Ohrievač nefunguje. Pravdepodobne KLON.");
  }
  
  // Vypnutie ohrevu
  sht4.setHeater(SHT4X_NO_HEATER);
}

void loop() {
  // Nič nerobíme
}