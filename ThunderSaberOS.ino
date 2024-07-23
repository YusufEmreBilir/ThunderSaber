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
#define DEBUG_SOUND false

#define NUM_LEDS 120                //Bıçaktaki LED sayısı.
#define CHIPSET WS2812B             //LED şerit tipi.

#define STATUS_LED 7                //Durum bildirme ledi.
#define BLADE_LED_PIN 6             //LED şeritin data pini.
#define MOTOR_PIN 5                 //Titreşim motorunun pini.
#define MAIN_BUTTON_PIN 4           //Açma/Kapama butonu pini.
#define BUSY_PIN 3                  //DFplayer'ın meşgullük bildiren pini, Çalıyor = 0, Boşta = 1

#define SWING_START_TRESHOLD 1      //Sallama rutinini başlatmak için gereken hareket miktarının hassasiyeti.
#define SWING_STOP_TRESHOLD 1       //Sallama rutinini durdurmak için gereken hareketsizlik miktarının hassasiyeti.


/*---------- İç Değişkenler ----------*/       //Elle DEĞİŞTİRİLMEMESİ gereken değişkenler.

byte stationary, moving, movementValue;
byte internalVolume, prevInternalVolume;   
byte errorCounter = 0;
byte buttonClickCounter = 0;
byte randomBrightness;
byte unstableLedsToFlicker[60];
sensors_event_t accel, gyro, temp;
unsigned long ignitionMillis, flickerMillis, soundEngineMillis, mainButtonMillis, fadeSoundMillis;
int ignitionFadeTime;
unsigned int randomFlicker = 0; 
int currentLed1 = 0, currentLed2 = NUM_LEDS - 1;
bool saberIsOn = false, mainButtonState = false, igniting = false, flickered = false;


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
byte flickerBrightnessPercent = 75;     //Bıçağın titreme yaparken düşeceği parlaklık yüzdesi.
byte ignitionSpeed = 5;        //Ateşleme/Geri çekme hızı, Azaldıkça hızlanır.
byte ledPerStep = 2;            //Her ateşleme adımında yakılacak LED miktarı.

//SES
byte idleVolume = 15;          //Ses seviyesi, yükseltmek sallama efektini güçsüzleştirir. Önerilen Max: 20
byte soundEngineFreq = 50;      //Ses motoru denetleme frekansı, azaldıkça hassaslık artar, tutarlılık azalır. Min: 50

//DİĞER
byte mainButtonFreq = 50;       //Ana buton denetleme frekansı. Yüksek değerler algılanmayan komutlarla sonuçlanır.
byte motorPower = 75;           //Hareketsiz durumdayken titreşim motoru gücü, Min: 75, Max: 150


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
  delay(500);        //DFplayerMini yavaş bir komponenttir, komutlar arası zamana ihtiyaç duyar.  
  df.reset();
  delay(200);         //peşpeşe gönderilen komutlar ya algılanmayacak yada komple senkronu bozacaktır.
  df.stop();          
  delay(200);
  df.volume(30);
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
  delay(600);
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
}


void mainButtonCheck()  //MARK:MainButtonCheck
{
  if (igniting) {switchBlade(saberIsOn);  fadeSound(!saberIsOn);}
  if (millis() - mainButtonMillis < mainButtonFreq) {return;}
  if (digitalRead(MAIN_BUTTON_PIN) == 1) {mainButtonState = true; buttonClickCounter++;}
  if (!mainButtonState) {return;}
  if (digitalRead(MAIN_BUTTON_PIN) == 0) {mainButtonState = false;}
  if (!saberIsOn && !igniting)
  {
    saberIsOn = true;
    setBladeColor(blade_R, blade_G, blade_B);
    df.play(SFX_IGNITE_AND_HUM);  
    //Normalde Setup dışında delay kullanmak, multitasking yapılan bir kodda,  
    //mantıklı değil ancak burada zaten bütün donanımın DFplayer'ın çalmasını beklemesi gerekli.
    delay(100);
    internalVolume = 30;
    df.volume(internalVolume);


    analogWrite(MOTOR_PIN, 120); //Motor başlat, başlatma hızı min: 120

    #if DEBUG
    digitalWrite(STATUS_LED, 1);
    Serial.println(F("igniting"));
    #endif
  }
  else if (!igniting)
  {
    saberIsOn = false;
    setBladeColor(CRGB::Black);
    delay(50);
    df.play(SFX_RETRACT);
    delay(50);
    internalVolume = idleVolume;
    df.volume(internalVolume);
    analogWrite(MOTOR_PIN, 120);

    #if DEBUG
    digitalWrite(STATUS_LED, 0);
    Serial.println(F("retracting"));
    #endif
  }
  igniting = true;
  mainButtonMillis = millis();
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
  
  if (currentLed1 >= 61)   //ateşleme/geri çekme tamamlandı.
  {
    igniting = false;
    analogWrite(MOTOR_PIN, motorPower); //motor idle
    currentLed1 = 60;
    currentLed2 = 60;

    #if DEBUG
    Serial.println(F("ignited"));
    #endif

    return;
  }
  else if (currentLed1 <= -1)
  {
    igniting = false;
    analogWrite(MOTOR_PIN, 0); //motor durdur
    currentLed1 = 0;
    currentLed2 = NUM_LEDS;

    #if DEBUG
    Serial.println(F("retracted"));
    #endif

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
      FastLED.setBrightness(brightness * flickerBrightnessPercent / 100);
      FastLED.show();
      flickered = false;
    }

    flickerMillis = millis();
  }
}


void soundEngine()  //MARK:SoundEngine
{
if (igniting || !saberIsOn) {return;}
if (millis() - soundEngineMillis < soundEngineFreq) {return;}
mpu.getEvent(&accel, &gyro, &temp);
byte sumAccel = abs(accel.acceleration.x) + abs(accel.acceleration.y) + abs(accel.acceleration.z);
byte sumGyro = abs(gyro.gyro.x) + abs(gyro.gyro.y) + abs(gyro.gyro.z);
movementValue = sumAccel + sumGyro;

if (sumAccel > 16 || sumGyro > 0.4)
{
  if (stationary <= SWING_STOP_TRESHOLD) {stationary++;}
  if (stationary < SWING_STOP_TRESHOLD) {return;}
  moving = 0;

  internalVolume = map(movementValue, 10, 50, idleVolume, 30);
  if (internalVolume > 30) {internalVolume = 30;} //failsafe

  if (internalVolume == idleVolume || internalVolume != prevInternalVolume)
  {
    prevInternalVolume = internalVolume;  //dinamik işitsel geri bildirim (sallama sesi).
    df.volume(internalVolume);
    analogWrite(MOTOR_PIN, map(internalVolume, idleVolume, 30, motorPower, 200));  //dinamik haptik geri bildirim (titreşim).

    #if DEBUG
    Serial.print("moving,  Volume: ");
    Serial.print(internalVolume);
    Serial.print(" / movement value: ");
    Serial.println(movementValue);
    #endif
  }
}
else
{
  if (moving <= SWING_START_TRESHOLD) {moving++;}
  if (moving < SWING_START_TRESHOLD) {return;}
  stationary = 0;
  if (internalVolume != idleVolume)   //ses seviyesini DFplayer'dan okursak komut göndermiş oluruz, bu yüzden
  {                               //hemen arkasından ses seviyesini ayarlamak için yolladığımız komut algılanmayacaktır.
                                  //Çözümü ise ses seviyesi için ayrı bir değişken kullanmaktır.
    internalVolume = idleVolume;
    df.volume(internalVolume);           //sesi eski seviyesine geri getir.
    analogWrite(MOTOR_PIN, motorPower);  //titreşim motorunu eski seviyesine geri getir.

    #if DEBUG
    Serial.println("stoppped");
    #endif
  }
}

soundEngineMillis = millis();
}

void haltUntilEndOfTrack()
{
  while (BUSY_PIN)
  {
    delay(1);
    //Sesin başlamasını bekle.
  }
  while (!BUSY_PIN)
  {
    delay(1);
    //DUR YOLCU!
  }
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

  if (internalVolume < idleVolume || internalVolume > 30)
  {
    return;
  }

  if (inOut)
  {
    if (millis() - fadeSoundMillis > 50)
    {
      internalVolume = map(currentLed1, -1, 61, 30, idleVolume);
      df.volume(internalVolume);
      #if DEBUG_SOUND
      Serial.println(internalVolume);
      #endif
      fadeSoundMillis = millis();
    }
  }
  else
  {
    if (millis() - fadeSoundMillis > 50)
    {
      internalVolume = map(currentLed1, -1, 61, 30, idleVolume);
      df.volume(internalVolume);
      #if DEBUG_SOUND
      Serial.println(internalVolume);
      #endif
      fadeSoundMillis = millis();
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

