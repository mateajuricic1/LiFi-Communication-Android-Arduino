// Inicijalizacija LCDa
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

// Korisničke postavke
int timeUnitLength = 90; //vremenska jedinica za koju se šalje Morseov kod
int ControlLed = 13; // Feedback LED
int photoResistorPin = A0; // Pin na kojem je priključen fotoopornik

//--------------------
// Debug postavke
bool debug = false; // Općeniti debug
bool debugTiming = true; // debug za određivanje koliko vremena je svjetlo upaljeno,tj. izgašeno
bool debugSensor=false; // debug za fotootpornik
//--------------------

// MorseTree, polje podataka korišteno za dekodiranje
const char MorseTree[] = {'\0','E', 'T', 'I', 'A', 'N', 'M', 'S',
                          'U', 'R', 'W', 'D', 'K', 'G', 'O', 'H',
                          'V', 'F', 'U', 'L', 'A', 'P', 'J', 'B',
                          'X', 'C', 'Y', 'Z', 'Q', '\0','\0','5',
                          '4', '\0','3', '\0','\0','\0','2', '\0',
                          '\0','+', '\0','\0','\0','\0','1', '6',
                          '=', '/', '\0','\0','\0','(', '\0','7',
                          '\0','\0','\0','8', '\0','9', '0', '\0',
                          '\0','\0','\0','\0','\0','\0','\0','\0',
                          '\0','\0','\0','?', '_', '\0','\0','\0',
                          '\0','"', '\0','\0','.', '\0','\0','\0',
                          '\0','@', '\0','\0','\0','\0','\0','\0',
                          '-', '\0','\0','\0','\0','\0','\0','\0',
                          '\0',';', '!', '\0',')', '\0','\0','\0',
                          '\0','\0',',', '\0','\0','\0','\0',':',
                          '\0','\0','\0','\0','\0','\0','\0'
                         };

int val = 0; // Vrijednost očitana sa fotootpornika
int codePtr = 0; // za šetanje po MorseTree
bool notAnalysed = false; // Prati jesmo li tretirali prethodni "State" bljeskalice
bool endOfTrans = true; // Šaljemo li još? Važno za ispisivanje zadnjeg slova transmisije
static unsigned long timer = millis();
bool lightState = false; // stanje svjetla(upaljeno/izgašeno)
int lightOnLen = 0; // koliko je vremenski svjetlo upaljeno
int lightOffLen = 0; // koliko je vremenski svjetlo izgašeno
int lcdPos = 0; // Pomaže LCDu kada treba u novi red prijeći 

void getMorse(){    

    // Čitanje vrijednosti sa fotootpornika
    val = analogRead(photoResistorPin);
    if(debugSensor){ Serial.println("Vrijednost je :" + String(val)); }
 
  if (val >= 250)
  {
    // kada djeluje bljeskalica na LDR
    if(!lightState){
     lightOffLen = millis()-timer;
     timer=millis();
     lightState = true;     
     notAnalysed = true;
     endOfTrans = false;
     if(debugTiming){ Serial.println("Lenght Off: " + String(lightOffLen) + " or " + String(lightOffLen/timeUnitLength) + " units"); }
     lightOffLen = lightOffLen/timeUnitLength; // mjeri se vremenskim jedinicama definiranim varijablom timeUnitLength
    }
    if(debug){ digitalWrite(ControlLed, HIGH); }
  } else {
    // kada je LDR u "tami"
    if(lightState){
     lightOnLen = millis()-timer;
     timer=millis();
     lightState = false;
     notAnalysed = true;
     endOfTrans = false;
     if(debugTiming){ Serial.println("Lenght On: " + String(lightOnLen) + " or " + String(lightOnLen/timeUnitLength) + " units"); }
     lightOnLen = lightOnLen/timeUnitLength; // mjeri se vremenskim jedinicama definiranim varijablom timeUnitLength
    }
    if(debug){ digitalWrite(13, LOW); }

    // Kraj transmisije + završavanje ispisivanja zadnjeg slova
    if(!endOfTrans && ((millis()-timer)/timeUnitLength) >= 33){
          if(debug){ Serial.println(); Serial.println("--Slovo/broj: " + String(MorseTree[codePtr]) + " -- Code: " + String(codePtr)); }
          Serial.print(MorseTree[codePtr]);
          lcdPrint((char)MorseTree[codePtr]);
          codePtr = 0;
          endOfTrans = true;
          if(debug){ Serial.println(" Kraj transmisije "); }
          Serial.println("");
          Serial.println("---------------------------------------------------------------");
    }
}

    /*** Dekodiranje Morseova koda ***/
    // Kada se bljeskalica izgasi, provjerava se je li poslan kratki ili duzi signal.
    if(notAnalysed && !lightState){
      if ( lightOnLen <=2) {
        Serial.print(".");
        codePtr = (2*codePtr) + 1;
        notAnalysed = false;
      } else if (lightOnLen >= 3 && lightOnLen<=7) {
        Serial.print("-");
        codePtr = (2*codePtr) + 2;
        notAnalysed = false;
      } else if (lightOnLen >= 8) {
        Serial.println("**** Bljeskalica je predugo upaljena ****");
      }
    }

    // Kada se bljeskalica upali, provjeravaju se razmaci i kraj rijeci ili transmisije
    if(notAnalysed && lightState){
      // razmak između dijelova slova/broja, tj. tockica i crtica
        if(lightOffLen <= 10){
          notAnalysed = false;  
        } 
      // razmak između slova u rijeci  
        else if(lightOffLen >= 15 && lightOffLen < 26){
           if(debug){ Serial.println(); Serial.println("--Slovo/broj: " + String(MorseTree[codePtr]) + " -- Code: " + String(codePtr)); }
          Serial.print(MorseTree[codePtr]);
          lcdPrint((char)MorseTree[codePtr]);
          codePtr = 0;
          notAnalysed = false;
        } 
      // razmak između rijeci, odnosno brojeva  
        else if(lightOffLen >= 26 && lightOffLen < 34){
            if(debug){ Serial.println(); Serial.println("--Slovo/broj: " + String(MorseTree[codePtr]) + " -- Code: " + String(codePtr)); }
          Serial.print(MorseTree[codePtr]);
          lcdPrint((char)MorseTree[codePtr]);
          codePtr = 0;
          notAnalysed = false;
          if(debug){ Serial.println(" Kraj rijeci "); }
          Serial.print('#');
          lcdPrint((char)'#');
        } 
      // Ako je svjetlo predugo izgašeno, znači da je vrijeme za novu transmisiju  
        else if(lightOffLen > 50){
          lcd.clear();
          lcdPos=0;  
        }
}
   }

void lcdPrint(char c){
  // Promijeni redak na 1. slovo/broj 
  if(lcdPos==0){ lcd.setCursor(0,0); }
  // Promijeni redak na 16. slovo/broj
  if(lcdPos==16){ lcd.setCursor(0,1); }
  // Poruka preduga, izbrisi sve i vrati se u prvi redak 
  if(lcdPos==32){ lcd.clear(); lcd.setCursor(0,0); lcdPos=0; }
 
  // Prikazi slovo/broj!
  lcd.print(c);
  lcdPos++;
}


void setup()
{ 
  pinMode(13,OUTPUT); // Feedback LED
  pinMode(A0,INPUT); // fotootpornik
  Serial.begin(9600);// serijska komunikacija
  Serial.println("-----------------------");
  Serial.println("- Zavrsni rad -");
  Serial.println("-----------------------");
  Serial.println("");
  delay(50);

  // odredi broj redaka i stupaca LCD zaslona:
  Wire.begin();
  lcd.begin(16,2);
  lcd.backlight();// upaljeno pozadinsko svjetlo LCD zaslona
  // Ispiši poruku na LCD zaslon:
  lcd.setCursor(0,0);
  lcd.print("ZAVRSNI");
  lcd.setCursor(0, 1);
  lcd.print("Morse dekoder");
  delay(2000);
  lcd.clear(); // brisanje prikazane poruke na LCD-u
}

void loop()
{
  getMorse();
}
