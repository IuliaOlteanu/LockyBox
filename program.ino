#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9

// led verde : pin 7
#define LED_G 7

// led rosu : pin 6
#define LED_R 6

// buzzer : pin 3
#define BUZZER 3

// potentiometru 1 : pin A0
#define POTENTIOMETRU1 A0

// potentiometru 2 : pin A1
#define POTENTIOMETRU2 A1

// creare instanta MFRC522
MFRC522 mfrc522(SS_PIN, RST_PIN);

// creare servo motor
Servo myServo;

// ecran LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// stare seif : blocat False, deschis True
volatile bool stare;

// cod introdus prin intermediul potentiometrelor;
String cod;

// numar de incercari gresite
volatile int nr;

volatile int aux = 0;

// valori citite prin intermediul potentiometrelor
int val1, val2;

const byte Speaker[8] = {
0b00001,
0b00011,
0b01111,
0b01111,
0b01111,
0b00011,
0b00001,
0b00000
};

const byte Heart[8] = {
0b00000,
0b01010,
0b11111,
0b11111,
0b01110,
0b00100,
0b00000,
0b00000
};

const byte Check[8] = {
0b00000,
0b00001,
0b00011,
0b10110,
0b11100,
0b01000,
0b00000,
0b00000
};

const byte Lock[8] = {
0b01110,
0b10001,
0b10001,
0b11111,
0b11011,
0b11011,
0b11111,
0b00000
};

void stare_seif() {
  lcd.clear();
  if(stare == true) {
    lcd.setCursor(0, 0);
    lcd.print("Seif deschis");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Seif inchis");
    // afisare caracterele speciale pe LCD
    for(int i = 0; i <= 10; i = i + 2) {
      lcd.setCursor(i, 1);
      lcd.write(3);
    }
  }
}

void afisare_mesaj_cod() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Introduceti cod");
  delay(1000);
}

void afisare_mesaj_card() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Apropiati card");
  delay(1000);
}

void acces_depasit() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cod/Card gresit");
  lcd.setCursor(1, 1);
  lcd.print("De trei ori");

  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Folositi card");
  lcd.setCursor(1, 1);
  lcd.print("Pt deblocare");

  delay(2000);

  while(1) {
    digitalWrite(LED_R, HIGH);
    tone(BUZZER, 300);
     
    delay(50);

    digitalWrite(LED_R, LOW);
    noTone(BUZZER);
    delay(50);

    lcd.clear();
    for(int i = 0; i <= 6; i = i + 2) {
      lcd.setCursor(i, 0);
      lcd.write(0);
    }

    for(int i = 0; i <= 8; i = i + 2) {
      lcd.setCursor(i, 1);
      lcd.write(0);
    }

    if(!mfrc522.PICC_IsNewCardPresent()) {
      continue;
    }
    if(!mfrc522.PICC_ReadCardSerial()) {
      continue;
    }
    String content = "";
    byte letter;
    for(byte i = 0; i < mfrc522.uid.size; i++) {
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    content.toUpperCase();
    if(content.substring(1) == "23 66 D9 10") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Deblocat");
      delay(1000);
      acces_permis();
      break;
    } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Card gresit");
        delay(1000);
    }
  }
}

void acces_permis() {
  nr = 0;
  delay(500);
  digitalWrite(LED_G, HIGH);
  tone(BUZZER, 500);
  delay(300);
  noTone(BUZZER);
  myServo.write(100);

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Acces permis");
  for(int i = 2; i <= 12; i = i + 2) {
    lcd.setCursor(i, 1);
    lcd.write(2);
  }
  delay(2000);

  stare = 1;
  stare_seif();
  delay(3000);

  myServo.write(0);
  digitalWrite(LED_G, LOW);
}

void acces_nepermis() {
  digitalWrite(LED_R, HIGH);
  tone(BUZZER, 300);

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Acces interzis");
  lcd.setCursor(3, 1);
  lcd.print("Reincercati");
  delay(2000);

  stare = 0;
  stare_seif();
  delay(1500);

  digitalWrite(LED_R, LOW);
  noTone(BUZZER);
}

void setup() {
  // initializare comunicatie seriala
  Serial.begin(9600);

  // initializare magistrala SPI
  SPI.begin();

  // initializare MFRC522
  mfrc522.PCD_Init();

  // Pini servomotor
  myServo.attach(3);
  myServo.attach(5);
  // Pozitia de start a servomotorului
  myServo.write(0);

  // initializare leduri
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);

  // potentiometre

  pinMode(POTENTIOMETRU1, INPUT);
  pinMode(POTENTIOMETRU2, INPUT);

  // intializare buzzer
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);

  // afisare text de inceput
  lcd.init();
  lcd.backlight();

  // caractere speciale
  lcd.createChar(0, Speaker);
  lcd.createChar(1, Heart);
  lcd.createChar(2, Check);
  lcd.createChar(3, Lock);

  lcd.setCursor(0, 0);
  lcd.print("Bine ati venit");
  // afisare caracterele speciale pe LCD
  for(int i = 2; i <= 12; i = i + 2) {
    lcd.setCursor(i, 1);
    lcd.write(1);
  }
  delay(2000);
  // afisare stare curenta a seifului
  stare_seif();
  delay(2000);

  // afisare posibilitata initiala de acces la seif
  afisare_mesaj_cod();

  // stare initiala a seifului : inchis
  stare = false;
  nr = 0;

  // citire valori potentiometre

  val1 = map(analogRead(POTENTIOMETRU1), 0, 1023, 0, 100);
  val2 = map(analogRead(POTENTIOMETRU2), 0, 1023, 0, 100);

  cod = "Cod: ";
  cod = cod + val1 + " ";
  cod = cod + val2;
}
void citire_cod() {
  val1 = map(analogRead(POTENTIOMETRU1), 0, 1023, 0, 100);
  val2 = map(analogRead(POTENTIOMETRU2), 0, 1023, 0, 100);

  cod = "Cod: ";
  cod = cod + val1 + " ";
  cod = cod + val2;
}
void loop() {
  if(val1 + val2 <= 15) {
    afisare_mesaj_cod();
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(cod);
    delay(2000);
  }

  if(val1 + val2 >= 50 && val1 + val2 <= 150) {
    // cod corect 
    // afisare cod introdus
    aux++;
    if(aux == 1) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(cod);
      delay(2000);
    }
    
    // afisare mesaj pt card
    afisare_mesaj_card();
    delay(2000);
    if(!mfrc522.PICC_IsNewCardPresent()) {
      return;
    }
    if(!mfrc522.PICC_ReadCardSerial()) {
      return;
    }

    String content = "";
    byte letter;
    for(byte i = 0; i < mfrc522.uid.size; i++) {
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    content.toUpperCase();
    if(content.substring(1) == "23 66 D9 10") {
      acces_permis();
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Card gresit");
      delay(1000);
      nr++;
      if(nr == 3) {
        // avertizare sonora si vizuala mai accentuata
        acces_depasit();
      } else {
        acces_nepermis();
      }
    }
  } else if (val1 + val2 >= 16 && val1 + val2 < 50){
    // afisare cod gresit
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(cod);
    lcd.setCursor(0, 1);
    lcd.print("Reincercati");
    delay(1000);
    nr++;
    if(nr == 3) {
      // avertizare sonora si vizuala mai accentuata
      acces_depasit();
    } else {
      acces_nepermis();
    }
  } 
  aux = 0;
  afisare_mesaj_cod();
  delay(2000);
  citire_cod();
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
