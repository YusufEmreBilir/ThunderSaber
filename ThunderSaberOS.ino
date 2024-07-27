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

CRGB colorPresets[6] = {CRGB::Blue, CRGB::Green, CRGB::Purple, CRGB::Yellow, CRGB::White, CRGB::Red};
CRGB currentBladeColor;
byte currentColorPreset = 0;
byte movementValue;
byte internalVolume, prevInternalVolume;   
byte errorCounter = 0, buttonClickCount = 0;
byte brightness;
byte randomBrightness;
byte unstableLedToFlicker;
sensors_event_t accel, gyro, temp;
byte sumAccel, sumGyro;
unsigned long ignitionMillis, flickerMillis, soundEngineMillis, soundFadeMillis;
unsigned long ignitionSoundFadeTimer, bladeSwitchCooldownTimer, mainButtonFunctionTimer;
unsigned int randomFlicker = 0; 
int currentLed1 = 0, currentLed2 = NUM_LEDS - 1;
bool saberIsOn = false, mainButtonState = false, switchingBlade = false, flickered = false;
bool soundFading = false, moving = false, colorChangeOverride = false;


/*---------- Objeler ----------*/

SoftwareSerial FPSerial(/*tx =*/11, /*rx =*/10);
CRGB led[NUM_LEDS];
DFPlayerMini_Fast df;
Adafruit_MPU6050 mpu;


/*---------- Parametreler ----------*/    //Belli özellikleri kişiselleştirmek için gelişmiş seçenekler.

//IŞIK
bool bladeIsUnstable = false;               //Bıçağın dengesizce yanıp sönmesini sağlar (kylo ren efekti).
byte unstableLedCount = 60;                 //Aynı anda yakıp söndürülecek led sayısı.
byte unstableFlickerFreqLimit = 1;          //Bıçağın titreme frekansının üst sınırı, 0 ile bu değer arasında rastgele oluşturulur.
byte unstableFlickerBrightnessLimit = 255;  //Bıçağın titreme yaparken dönüşüm yapacağı parlaklık çarpanı üst sınırı.

CRGB bladeColor(colorPresets[currentColorPreset]); //Bıçak rengi.
byte brightness = 65025 / (currentBladeColor.r + currentBladeColor.g + currentBladeColor.b);

byte flickerFreqLimit = 100;           //Bıçağın titreme frekansının üst sınırı, 0 ile bu değer arasında rastgele oluşturulur.
byte flickerBrightness = 75;           //Bıçağın titreme yaparken düşeceği parlaklık yüzdesi.

byte ignitionSpeed = 5;                //Ateşleme/Geri çekme hızı, Azaldıkça hızlanır.
byte ledPerStep = 2;                   //Her ateşleme adımında yakılacak LED miktarı.


//SES
byte idleVolume = 15;                  //Ses seviyesi, yükseltmek sallama efektini güçsüzleştirir. Önerilen Max: 20
byte soundEngineFreq = 50;             //Ses motoru denetleme frekansı, azaldıkça hassaslık artar, tutarlılık azalır. Min: 50
int ignitionSoundFadeLenght = 200;     //Ateşleme sesinin ateşleme bittikten sonra varsayılana dönmesi için verilen süre (ms).


//TİTREŞİM
byte idleMotorPower = 75;              //Hareketsiz durumdayken titreşim motoru gücü, Min: 75, Max: 150
byte maxMotorPower = 255;              //Motorun çıkabileceği maksimum güç, Max: 255


//BUTON
int bladeSwitchCooldownLenght = 1500;  //Ateşleme tetiklendikten sonra yeniden tetiklenebilmesi için geçmesi gereken gereken süre (ms).
int mainButtonFunctionLenght = 300;    //Ana butona basıldıktan sonra başka fonksiyonları tetiklemek için arka arkaya basmalar arasındaki max süre (ms).



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
  brightness = 65025 / (currentBladeColor.r + currentBladeColor.g + currentBladeColor.b);
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
  //Başlangıç rutini:
  Serial.begin(9600);
  setPins();
  DFPlayerInitialize();
  MPUInitialize();
  ledInitialize();
  catchErrors();
  finalizeSetup();
}


void loop() //MARK:loop
{
  //Döngü rutini:
  checkMainButton();
  switchBlade();
  flicker();
  fadeSound();
  soundEngine();
}


//MARK:CheckMainButton
void checkMainButton()
{
  if ((millis() - mainButtonFunctionTimer > mainButtonFunctionLenght) && buttonClickCount != 0)
  {
    switch (buttonClickCount)
    {
      case 1:
        startSwitchingBlade();
        break;

      case 2:
        //Renk değiştir.
        break;

      case 3:
        //Ön ayar değiştir.
        break;

      default:
        break;
  
    }

    buttonClickCount = 0;
  }

  if (!digitalRead(MAIN_BUTTON_PIN)) {mainButtonState = false; return;}
  if (mainButtonState) {return;}
  if (digitalRead(MAIN_BUTTON_PIN) && !mainButtonState) {mainButtonState = true;}

  buttonClickCount++;
  mainButtonFunctionTimer = millis();
}



/*----------- Işık Fonsksiyonları -----------*/


//MARK:StartSwitchingBlade
void startSwitchingBlade()
{
  if (millis() - bladeSwitchCooldownTimer < bladeSwitchCooldownLenght) {return;}
  
  if (!saberIsOn || colorChangeOverride)
  {
    saberIsOn = true;
    colorChangeOverride = false;
    setBladeColor(bladeColor);
    df.play(SFX_IGNITE_AND_HUM);
    delay(60);  
    internalVolume = 30;
    analogWrite(MOTOR_PIN, maxMotorPower);
    soundFadeMillis = millis();

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
    internalVolume = idleVolume;
    analogWrite(MOTOR_PIN, maxMotorPower);
    soundFadeMillis = millis();
    soundFading = true;

    #if DEBUG
    digitalWrite(STATUS_LED, 0);
    Serial.println(F("retracting"));
    #endif
  }

  switchingBlade = true;
}
void switchBlade()  //MARK:SwitchBlade
{
  if (!switchingBlade || millis() - ignitionMillis < ignitionSpeed) {return;}

  for (int i = 0; i<ledPerStep; i++)   //Igniting
  {
    led[currentLed1] = currentBladeColor;
    led[currentLed2] = currentBladeColor;
    if (saberIsOn && currentLed1 < 58)
    {
      led[currentLed1+1] = CRGB::White;
      led[currentLed2-1] = CRGB::White;
      led[currentLed1+2] = CRGB::White;    //nasıl durduğunun farkındayım bi ara düzeltecem inş.
      led[currentLed2-2] = CRGB::White;
      led[currentLed1+3] = CRGB::White;
      led[currentLed2-3] = CRGB::White;
    }

    if (saberIsOn)
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
    switchingBlade = false;
    bladeSwitchCooldownTimer = millis();
    ignitionSoundFadeTimer = millis();
    soundFading = true;
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
    bladeSwitchCooldownTimer = millis();
    switchingBlade = false;

    return;
  }
}


void flicker()  //MARK:Flicker
{
  if (bladeIsUnstable) {unstableBladeFlicker(); return;}

  if ((switchingBlade || saberIsOn) && (millis() - flickerMillis > randomFlicker))
  {
    randomFlicker =  random(0, flickerFreqLimit);
    if (!flickered)
    {
      FastLED.setBrightness(brightness);
      flickered = true;
    }
    else
    {
      FastLED.setBrightness(brightness * flickerBrightness / 100);
      flickered = false;
    }

    FastLED.show();
    flickerMillis = millis();
  }
}


//MARK:UnstableBladeFlicker
void unstableBladeFlicker()
{
  if ((switchingBlade || saberIsOn) && (millis() - flickerMillis > randomFlicker))
  {
    randomFlicker =  random(0, unstableFlickerFreqLimit);

    if (!flickered)
    {
      for (byte i = 0; i <= unstableLedCount; i++)
      {
        unstableLedToFlicker = random(0, NUM_LEDS);
        if (led[unstableLedToFlicker] != CRGB::Black)
        {
          randomBrightness = random(1.3, unstableFlickerBrightnessLimit);
          led[unstableLedToFlicker] = 
          CRGB((currentBladeColor.r / randomBrightness), (currentBladeColor.g / randomBrightness), (currentBladeColor.b / randomBrightness));
        }
      }
      flickered = true;
    }
    else
    {
      for (byte i = 0; i <= NUM_LEDS; i++)
      {
        if (led[i] != CRGB::Black)
        {
          led[i] = currentBladeColor;
        }
      }
      flickered = false;
    }

    FastLED.show();
    flickerMillis = millis();
  }
}


//MARK:SetBladeColor
void setBladeColor(byte R, byte G, byte B)
{
  if (currentBladeColor == CRGB(R, G, B)) {return;}
  currentBladeColor = CRGB(R, G, B);
}
void setBladeColor(CRGB colorpreset)
{
  if (currentBladeColor == colorpreset) {return;}
  currentBladeColor = colorpreset;
}


//MARK:ChangeBladeColor
void changeBladeColor()
{
  currentColorPreset++;

  if (currentColorPreset == 7) 
  {
    bladeIsUnstable = true;
    bladeColor = CRGB::Red;
  }
  else 
  {
    bladeIsUnstable = false;
    bladeColor = colorPresets[currentColorPreset];
  }

  if (currentColorPreset > 7) {currentColorPreset = 0;}
  
  if (saberIsOn)
  {
    colorChangeOverride = true;
    startSwitchingBlade();
  }
}



/*----------- Ses Fonsksiyonları -----------*/


void soundEngine()  //MARK:SoundEngine
{
  if (switchingBlade || !saberIsOn || soundFading) {return;}
  if (millis() - soundEngineMillis < soundEngineFreq) {return;}
  mpu.getEvent(&accel, &gyro, &temp);
  sumAccel = abs(accel.acceleration.x) + abs(accel.acceleration.y) + abs(accel.acceleration.z);
  sumGyro = abs(gyro.gyro.x) + abs(gyro.gyro.y) + abs(gyro.gyro.z);
  movementValue = sumAccel + sumGyro;

  if (sumAccel > 16 || sumGyro > 0.4)
  {
    internalVolume = map(movementValue, 10, 45, idleVolume, 30);
    if (internalVolume > 30) {internalVolume = 30;} //failsafe

    if (internalVolume == idleVolume || internalVolume != prevInternalVolume)
    {
      moving = true;
      prevInternalVolume = internalVolume;
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
    moving = false;
    internalVolume = idleVolume;
    df.volume(internalVolume);                    //sesi eski seviyesine geri getir.
    analogWrite(MOTOR_PIN, idleMotorPower);       //titreşim motorunu eski seviyesine geri getir.

    #if DEBUG_SOUND
    Serial.println("stopped");
    #endif
     
    /*ses seviyesini DFplayer'dan okursak komut göndermiş oluruz, bu yüzden
    hemen arkasından ses seviyesini ayarlamak için yolladığımız komut algılanmayacaktır.
    Çözümü ise ses seviyesi için ayrı bir değişken kullanmaktır.*/
  }

  soundEngineMillis = millis();
}

//MARK:HaltUntilEndOfTrack
void haltUntilEndOfTrack()
{
  #if DEBUG
  Serial.println(F("Sesin baslamasi icin bekleniyor..."));
  #endif
  
  while (digitalRead(BUSY_PIN))
  {
    delay(1);     //Sesin başlamasını bekle.
  }

  #if DEBUG
  Serial.println(F("Ses basladi, bitmesi bekleniyor..."));
  #endif

  while (!digitalRead(BUSY_PIN))
  {
    delay(1);     //DUR YOLCU!
  }

  #if DEBUG
  Serial.println(F("Devam ediliyor..."));
  #endif
}



//MARK:FadeSound
//false = out, true = in
void fadeSound()
{
  if (!soundFading || internalVolume < idleVolume || internalVolume > 30)
  {
    soundFading = false;
    return;
  }


  if (!saberIsOn && (millis() - soundFadeMillis > 50))
  {
    internalVolume = map(currentLed1, -1, 61, 31, idleVolume);
    df.volume(internalVolume);
    #if DEBUG_SOUND
    Serial.println(internalVolume);
    #endif
    soundFadeMillis = millis();
  }

  else if ((millis() - ignitionSoundFadeTimer > ignitionSoundFadeLenght) &&
   currentLed1 > 59 && (millis() - soundFadeMillis > 50))
  {
    internalVolume -= 2;
    df.volume(internalVolume);
    soundFadeMillis = millis();

    #if DEBUG_SOUND
    Serial.println(internalVolume);
    #endif
  }

  analogWrite(MOTOR_PIN, map(internalVolume, idleVolume, 30, idleMotorPower, maxMotorPower));
}


