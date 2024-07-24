/*

---------- ThunderSaberOS_V3 By YEB ----------

MPU 6050, DFPlayer mini ve 60LED/m WS2812B ile çalışması için tasarlanmıştır,
arduino nano ve uno üzerinde test edilmiştir.

Son revizyon: 2024

-Yusuf Emre Bilir

*/


/*---------- Kütüphaneler ----------*/

#include "Sounds.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <FastLED.h>
#include <DFPlayerMini_Fast.h>
#include <SoftwareSerial.h>
#include <Adafruit_MPU6050.h>

/*---------- Pre-Proccessor Değerler ----------*/

#define DEBUG true                  //Seri monitör vs. için debug ayarı. Açık olması Arduinoyu bir miktar yavaşlatır.
#define DEBUG_SOUND true

#define NUM_LEDS 120                //Bıçaktaki LED sayısı.
#define CHIPSET WS2812B             //LED şerit tipi.

#define STATUS_LED 7                //Durum bildirme ledi.
#define BLADE_LED_PIN 6             //LED şeritin data pini.
#define MOTOR_PIN 5                 //Titreşim motorunun pini.
#define MAIN_BUTTON_PIN 4           //Açma/Kapama butonu pini.
#define BUSY_PIN 3                  //DFplayer'ın meşgullük bildiren pini, Çalıyor = 0, Boşta = 1


/*---------- İç Değişkenler ----------*/       //Elle DEĞİŞTİRİLMEMESİ gereken değişkenler.

byte movementValue;
byte internalVolume, prevInternalVolume;   
byte errorCounter = 0, buttonClickCounter = 0;
byte randomBrightness;
byte unstableLedsToFlicker[60];
sensors_event_t accel, gyro, temp;
unsigned long ignitionMillis, flickerMillis, soundEngineMillis, soundFadeMillis;
unsigned int ignitionSoundFadeTimer, mainButtonCooldownTimer;
unsigned int randomFlicker = 0; 
int currentLed1 = 0, currentLed2 = NUM_LEDS - 1;
bool saberIsOn = false, mainButtonState = false, igniting = false, flickered = false, soundFadeNeeded = false;
bool moving = false, mainButtonInteractable = true;


/*---------- Objeler ----------*/

SoftwareSerial FPSerial(/*tx =*/11, /*rx =*/10);
CRGB led[NUM_LEDS];
DFPlayerMini_Fast df;
Adafruit_MPU6050 mpu;


/*---------- Parametreler ----------*/    //Belli özellikleri kişiselleştirmek için gelişmiş seçenekler.

//IŞIK
bool bladeIsUnstable = false;    //Bıçağın dengesize yanıp sönmesini sağlar (kylo ren efekti).
byte unstableLedCount = 60;
byte unstableFlickerFreqLimit = 1;     //Bıçağın titreme frekansının üst sınırı, 0 ile bu değer arasında rastgele oluşturulur.
byte unstableFlickerBrightnessLimit = 255;  //Bıçağın titreme yaparken dönüşüm yapacağı parlaklık çarpanı üst sınırı.

byte blade_R = 0;
byte blade_G = 0;           //Bıçak Rengi.
byte blade_B = 255;
CRGB bladeColor(blade_R, blade_G, blade_B);
byte brightness =
65025 / (blade_R + blade_G + blade_B);  //Otomatik parlaklık, 1.5 Amperi aşmayacak şekilde en yüksek parlaklığı ayarlar.

byte flickerFreqLimit = 100;     //Bıçağın titreme frekansının üst sınırı, 0 ile bu değer arasında rastgele oluşturulur.
byte flickerBrightness = 75;     //Bıçağın titreme yaparken düşeceği parlaklık yüzdesi.

byte ignitionSpeed = 5;        //Ateşleme/Geri çekme hızı, Azaldıkça hızlanır.
byte ledPerStep = 2;            //Her ateşleme adımında yakılacak LED miktarı.


//SES
byte idleVolume = 15;          //Ses seviyesi, yükseltmek sallama efektini güçsüzleştirir. Önerilen Max: 20
byte soundEngineFreq = 50;      //Ses motoru denetleme frekansı, azaldıkça hassaslık artar, tutarlılık azalır. Min: 50
int ignitionSoundFadeLenght = 2000;     //Ateşleme sesinin ateşleme bittikten sonra varsayılana dönmesi için verilen süre.


//DİĞER
int mainButtonCooldownLenght = 900;  //Ana butona basıldıktan sonra yeniden basıldığında algılanması için gereken süre.
byte idleMotorPower = 75;             //Hareketsiz durumdayken titreşim motoru gücü, Min: 75, Max: 150
byte maxMotorPower = 255;             //Motorun çıkabileceği maksimum güç, Max: 255


/*---------- Başlangıç Fonksiyonları ----------*/

void setPins()
{
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);

  pinMode(MAIN_BUTTON_PIN, INPUT);
  pinMode(BUSY_PIN, INPUT);
}

void ledInitialize()
{
  FastLED.addLeds<CHIPSET, BLADE_LED_PIN, GRB>(led, NUM_LEDS);
  for (byte i; i <= NUM_LEDS; i++)
  {
    led[i] = CRGB::Black;
  }          
  FastLED.show();          
  FastLED.setBrightness(brightness);
  randomSeed(analogRead(0));
}

void DFPlayerInitialize()
{
  FPSerial.begin(9600);
  if(!df.begin(FPSerial, false)) 
  {
    errorCounter++;
    #if DEBUG
    Serial.println(F("DFplayer hata"));
    #endif
  }
  delay(1000);        //DFplayerMini yavaş bir komponenttir, komutlar arası zamana ihtiyaç duyar.
  df.reset();
  delay(100); 
  df.stop();          
  delay(100);
  df.volume(idleVolume);
  internalVolume = idleVolume;
  prevInternalVolume = internalVolume;
}

void MPUInitialize()
{
  if(!mpu.begin()) 
  {
    errorCounter++;
    #if DEBUG
    Serial.println(F("mpu hata"));
    #endif
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void catchErrors()
{
  if (errorCounter > 0) 
  {
    delay(500);
    
    #if DEBUG
    df.play(SFX_BOOT_FAIL_FUNNY);
    Serial.println(F(">UYARI: Hata/Hatalar tespit edildi<"));
    #else
    df.play(SFX_BOOT_FAIL);
    #endif

    while (true)
    {
      digitalWrite(STATUS_LED, 0);
      delay(1000/errorCounter);
      digitalWrite(STATUS_LED, 1);
      delay(1000/errorCounter);
    }
  }
}

void finalizeSetup()
{
  digitalWrite(STATUS_LED, 1);
  df.play(SFX_BOOT_SUCCESS);
  haltUntilEndOfTrack();
  df.loop(SFX_STANDBY);
  delay(50);
  internalVolume = 30;
  df.volume(internalVolume);
  digitalWrite(STATUS_LED, 0);
  #if DEBUG
  Serial.println(F(">Baslatma basarili<"));
  #endif
}


void setup() //MARK:setup
{
  #if DEBUG                 //Debug modu aktifse;
  Serial.begin(9600);       //seri monitörü başlat.
  #endif

  //Başlangıç rutini:
  setPins();
  DFPlayerInitialize();
  MPUInitialize();
  ledInitialize();
  catchErrors();
  finalizeSetup();
}


void loop() //MARK:loop
{
  mainButtonCheck();
  soundEngine();
  flicker();
  fadeSound(saberIsOn);
}


void mainButtonCheck()  //MARK:MainButtonCheck
{
  if (igniting) {switchBlade(saberIsOn); return;}
  if (millis() - mainButtonCooldownTimer < mainButtonCooldownLenght) {return;}
  if (digitalRead(MAIN_BUTTON_PIN) == 0) {mainButtonState = false; return;}
  if (mainButtonState) {return;}
  if (digitalRead(MAIN_BUTTON_PIN) == 1 && !mainButtonState) {mainButtonState = true;}
  
  if (!saberIsOn)
  {
    saberIsOn = true;
    setBladeColor(blade_R, blade_G, blade_B);
    df.play(SFX_IGNITE_AND_HUM);
    delay(60);  
    soundFadeMillis = millis();
    internalVolume = 30;
    ignitionSoundFadeTimer = millis();
    soundFadeNeeded = true;

    #if DEBUG
    digitalWrite(STATUS_LED, 1);
    Serial.println(F("igniting"));
    #endif
  }
  else
  {
    saberIsOn = false;
    setBladeColor(CRGB::Black);
    df.play(SFX_RETRACT);
    soundFadeMillis = millis();
    internalVolume = idleVolume;
    soundFadeNeeded = true;

    #if DEBUG
    digitalWrite(STATUS_LED, 0);
    Serial.println(F("retracting"));
    #endif
  }

  igniting = true;
}


void switchBlade(bool operation)  //MARK:SwitchBlade
{
  if (millis() - ignitionMillis < ignitionSpeed){return;}
  for (int i = 0; i<ledPerStep; i++)   //Igniting
  {
    led[currentLed1] = bladeColor;
    led[currentLed2] = bladeColor;
    if (saberIsOn && currentLed1 < 58)
    {
      led[currentLed1+1] = CRGB::White;
      led[currentLed2-1] = CRGB::White;
      led[currentLed1+2] = CRGB::White;    //nasıl durduğunun farkındayım bi ara düzeltecem inş.
      led[currentLed2-2] = CRGB::White;
      led[currentLed1+3] = CRGB::White;
      led[currentLed2-3] = CRGB::White;
    }
    if (operation)
    {
      currentLed2--;
      currentLed1++;
    }
    else
    {
      currentLed2++;
      currentLed1--;
    }
  }

  FastLED.show();

  ignitionMillis = millis();
  
  if (currentLed1 >= 61)   //ateşleme tamamlandı.
  {
    igniting = false;
    mainButtonCooldownTimer = millis();
    analogWrite(MOTOR_PIN, idleMotorPower); //motor idle
    currentLed1 = 60;
    currentLed2 = 60;

    #if DEBUG
    Serial.println(F("ignited"));
    #endif

    return;
  }
  else if (currentLed1 <= -1) //geri çekme tamamlandı.
  {
    analogWrite(MOTOR_PIN, 0); //motor durdur
    currentLed1 = 0;
    currentLed2 = NUM_LEDS;

    #if DEBUG
    Serial.println(F("retracted"));
    #endif

    haltUntilEndOfTrack();
    df.loop(SFX_STANDBY);
    mainButtonCooldownTimer = millis();
    igniting = false;

    return;
  }
}


void flicker()  //MARK:Flicker
{
  if (bladeIsUnstable) {unstableBladeFlicker(); return;}
  if ((igniting || saberIsOn) && millis() - flickerMillis > randomFlicker)
  {
    randomFlicker =  random(0, flickerFreqLimit);
    if (!flickered)
    {
      FastLED.setBrightness(brightness);
      FastLED.show();
      flickered = true;
    }
    else
    {
      FastLED.setBrightness(brightness * flickerBrightness / 100);
      FastLED.show();
      flickered = false;
    }

    flickerMillis = millis();
  }
}


void soundEngine()  //MARK:SoundEngine
{
  if (igniting || !saberIsOn || soundFadeNeeded) {return;}
  if (millis() - soundEngineMillis < soundEngineFreq) {return;}
  mpu.getEvent(&accel, &gyro, &temp);
  byte sumAccel = abs(accel.acceleration.x) + abs(accel.acceleration.y) + abs(accel.acceleration.z);
  byte sumGyro = abs(gyro.gyro.x) + abs(gyro.gyro.y) + abs(gyro.gyro.z);
  movementValue = sumAccel + sumGyro;

  if (sumAccel > 16 || sumGyro > 0.4)
  {
    internalVolume = map(movementValue, 10, 45, idleVolume, 30);
    if (internalVolume > 30) {internalVolume = 30;} //failsafe

    if (internalVolume == idleVolume || internalVolume != prevInternalVolume)
    {
      moving = true;
      prevInternalVolume = internalVolume;  //dinamik işitsel geri bildirim (sallama sesi).
      df.volume(internalVolume);
      analogWrite(MOTOR_PIN, map(internalVolume, idleVolume, 30, idleMotorPower, maxMotorPower));  //dinamik haptik geri bildirim (titreşim).

      #if DEBUG_SOUND
      Serial.print("moving,  Volume: ");
      Serial.print(internalVolume);
      Serial.print(" / movement value: ");
      Serial.println(movementValue);
      #endif
    }
  }
  else if (moving)
  {
    if (internalVolume != idleVolume)   //ses seviyesini DFplayer'dan okursak komut göndermiş oluruz, bu yüzden
    {                               //hemen arkasından ses seviyesini ayarlamak için yolladığımız komut algılanmayacaktır.
                                    //Çözümü ise ses seviyesi için ayrı bir değişken kullanmaktır.
      moving = false;
      internalVolume = idleVolume;
      df.volume(internalVolume);           //sesi eski seviyesine geri getir.
      analogWrite(MOTOR_PIN, idleMotorPower);  //titreşim motorunu eski seviyesine geri getir.

      #if DEBUG_SOUND
      Serial.println("stoppped");
      #endif
     }
  }

  soundEngineMillis = millis();
}

//MARK:HALT
void haltUntilEndOfTrack()
{
  #if DEBUG
  Serial.println(F("Bekleniyor..."));
  #endif
  while (digitalRead(BUSY_PIN))
  {
    delay(1);
    //Sesin başlamasını bekle.
  }
  #if DEBUG
  Serial.println(F("Ses basladi, bitmesi bekleniyor..."));
  #endif
  while (!digitalRead(BUSY_PIN))
  {
    delay(1);
    //DUR YOLCU!
  }
  #if DEBUG
  Serial.println(F("Devam ediliyor..."));
  #endif
}

//MARK:SetBladeColor
void setBladeColor(byte R, byte G, byte B)
{
  if (bladeColor == CRGB(R, G, B)) {return;}
  bladeColor = CRGB(R, G, B);
}
void setBladeColor(CRGB colorpreset)
{
  if (bladeColor == colorpreset) {return;}
  bladeColor = colorpreset;
}


//MARK:FadeSound
//false = out, true = in
void fadeSound(bool inOut)
{
  if (!soundFadeNeeded || internalVolume < idleVolume || internalVolume > 30)
  {
    soundFadeNeeded = false;
    return;
  }

  analogWrite(MOTOR_PIN, map(internalVolume, idleVolume, 30, idleMotorPower, maxMotorPower));

  if (!inOut)
  {
    if (millis() - soundFadeMillis > 50)
    {
      internalVolume = map(currentLed1, -1, 61, 31, idleVolume);
      df.volume(internalVolume);
      #if DEBUG_SOUND
      Serial.println(internalVolume);
      #endif
      soundFadeMillis = millis();
    }
  }
  else
  {
    if (currentLed1 > 59 && (millis() - soundFadeMillis > 50))
    {
      internalVolume -= 2;
      df.volume(internalVolume);
      #if DEBUG_SOUND
      Serial.println(internalVolume);
      #endif
      soundFadeMillis = millis();
    }
  }
}

//MARK:UnstableBlade
void unstableBladeFlicker()
{
  if ((igniting || saberIsOn) && millis() - flickerMillis > randomFlicker)
  {
    randomFlicker =  random(0, unstableFlickerFreqLimit);

    if (!flickered)
    {
      for (byte i = 0; i < unstableLedCount; i++)
      {
        unstableLedsToFlicker[i] = random(0, NUM_LEDS);
      }
      randomBrightness = random(1.1, unstableFlickerBrightnessLimit);
    }

    if (!flickered)
    {
      for (byte i = 0; i < unstableLedCount; i++)
      {
        if (led[unstableLedsToFlicker[i]] != CRGB(0, 0, 0))
        {
          led[unstableLedsToFlicker[i]] = 
          CRGB(blade_R / randomBrightness, blade_G / randomBrightness, blade_B / randomBrightness);
        }
      }
      FastLED.show();
      flickered = true;
    }
    else
    {
      for (byte i = 0; i < unstableLedCount; i++)
      {
        if (led[unstableLedsToFlicker[i]] != CRGB(0, 0, 0))
        {
          led[unstableLedsToFlicker[i]] = 
          CRGB(blade_R * randomBrightness, blade_G * randomBrightness, blade_B * randomBrightness);
        }
      }
      FastLED.show();
      flickered = false;
    }

    flickerMillis = millis();
  }
}

