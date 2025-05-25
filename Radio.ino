#include <Wire.h>


// ==============
//  KONFIGURACJA
// ==============

// Nie musisz nic zmieniać jeśli zrobiono wszystko jak pokazałem

//@itsmgxb na tiktok

// STACJE – możesz zmienić na swoje
// Nie zmieniaj zmiennych (nazw stacji). zmień tylko częstotliwość!
#define STACJA_JEDYNKA 98.8
#define STACJA_MAXXX   105.6
#define STACJA_RMF     91.0

// PINY PRZYCISKÓW
#define PIN_BLOKADY     2
#define PIN_PRZYCISK_JEDYNKA 3
#define PIN_PRZYCISK_MAXXX   4
#define PIN_PRZYCISK_RMF     5

// POTENCJOMETR
#define PIN_POTENCJOMETRU A0

// UŻYWAĆ LCD? (0 = nie, 1 = tak)
#define UZYWAC_LCD 0

// DIODY 
#define DIODA_LOCK 6  // zapala się jak jest lock (zablokowana częstotliwość)

// ===================================================
//  RESZTA PROGRAMU – NIE RUSZAJ JAK DZIAŁA
// ===================================================

#define ADRES_TEA5767 0x60

float PoprzedniaCzestotliwosc = 0;
bool ZablokowanaCzestotliwosc = false;

bool StanPoprzedniBlokady = HIGH;
bool StanPoprzedniJedynki = HIGH;
bool StanPoprzedniMaxxx = HIGH;
bool StanPoprzedniRmf = HIGH;

void UstawCzestotliwosc(float Czestotliwosc) {
  uint16_t WartoscCzestotliwosci = 4 * (Czestotliwosc * 1000000 + 225000) / 32768;
  byte BajtH = WartoscCzestotliwosci >> 8;
  byte BajtL = WartoscCzestotliwosci & 0xFF;

  Wire.beginTransmission(ADRES_TEA5767);
  Wire.write(BajtH);
  Wire.write(BajtL);
  Wire.write(0xB0);
  Wire.write(0x10);
  Wire.write(0x00);
  Wire.endTransmission();

  Serial.print("USTAWIAM STACJĘ: ");
  Serial.print(Czestotliwosc, 1);
  Serial.println(" MHz");
}

void setup() {
  Wire.begin();

  pinMode(PIN_BLOKADY, INPUT_PULLUP);
  pinMode(PIN_PRZYCISK_JEDYNKA, INPUT_PULLUP);
  pinMode(PIN_PRZYCISK_MAXXX, INPUT_PULLUP);
  pinMode(PIN_PRZYCISK_RMF, INPUT_PULLUP);

  pinMode(DIODA_LOCK, OUTPUT); // Dioda pokazująca czy potencjometr zablokowany

  Serial.begin(9600);
  Serial.println("🛠️ RADIO GOTOWE DO PRACY");
}

void loop() {
  // BLOKOWANIE POTENCOMETRU
  bool StanPrzyciskuBlokady = digitalRead(PIN_BLOKADY);
  if (StanPrzyciskuBlokady == LOW && StanPoprzedniBlokady == HIGH) {
    ZablokowanaCzestotliwosc = !ZablokowanaCzestotliwosc;
    Serial.print("CZĘSTOTLIWOŚĆ ");
    Serial.println(ZablokowanaCzestotliwosc ? "ZABLOKOWANA 🔒" : "ODBLOKOWANA 🔓");

    digitalWrite(DIODA_LOCK, ZablokowanaCzestotliwosc ? HIGH : LOW);
    delay(200);
  }
  StanPoprzedniBlokady = StanPrzyciskuBlokady;

  // STACJA 1 – JEDYNKA
  bool StanJedynka = digitalRead(PIN_PRZYCISK_JEDYNKA);
  if (StanJedynka == LOW && StanPoprzedniJedynki == HIGH) {
    UstawCzestotliwosc(STACJA_JEDYNKA);
    PoprzedniaCzestotliwosc = STACJA_JEDYNKA;
    delay(200);
  }
  StanPoprzedniJedynki = StanJedynka;

  // STACJA 2 – MAXXX
  bool StanMaxxx = digitalRead(PIN_PRZYCISK_MAXXX);
  if (StanMaxxx == LOW && StanPoprzedniMaxxx == HIGH) {
    UstawCzestotliwosc(STACJA_MAXXX);
    PoprzedniaCzestotliwosc = STACJA_MAXXX;
    delay(200);
  }
  StanPoprzedniMaxxx = StanMaxxx;

  // STACJA 3 – RMF
  bool StanRmf = digitalRead(PIN_PRZYCISK_RMF);
  if (StanRmf == LOW && StanPoprzedniRmf == HIGH) {
    UstawCzestotliwosc(STACJA_RMF);
    PoprzedniaCzestotliwosc = STACJA_RMF;
    delay(200);
  }
  StanPoprzedniRmf = StanRmf;

  // POTENCJOMETR
  if (!ZablokowanaCzestotliwosc) {
    int Odczyt = analogRead(PIN_POTENCJOMETRU);
    float NowaCzestotliwosc = map(Odczyt, 0, 1023, 875, 1080) / 10.0;

    if (abs(NowaCzestotliwosc - PoprzedniaCzestotliwosc) >= 0.1) {
      UstawCzestotliwosc(NowaCzestotliwosc);
      PoprzedniaCzestotliwosc = NowaCzestotliwosc;
    }
  }

  delay(50);
}
