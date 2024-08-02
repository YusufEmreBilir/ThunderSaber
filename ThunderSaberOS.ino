/*

---------- ThunderSaberOS_V3 By YEB ----------

MPU 6050, DFPlayer mini ve 60LED/m WS2812B ile çalışması için tasarlanmıştır,
arduino nano ve uno üzerinde test edilmiştir.

Tam fonksiyonalite için;
Yine benim tarafımdan yazılan "ThunderSaber Helper App" ile beraber çalışır.

Son revizyon: 2024

-Yusuf Emre Bilir

*/


/*---------- Kütüphaneler ----------*/

#include "Sounds.h"
#include "Memory.h"
#include "ComCodes.h"

#include <EEPROM.h>
#include <FastLED.h>
#include <DFPlayerMini_Fast.h>
#include <SoftwareSerial.h>
#include <Adafruit_MPU6050.h>

/*---------- Pre-Proccessor Değerler ----------*/

#define DEBUG false                  //Seri monitör vs. için debug ayarı. Açık olması Arduinoyu bir miktar yavaşlatır.
#define DEBUG_SOUND false

#define NUM_LEDS 120                //Bıçaktaki LED sayısı.
#define CHIPSET WS2812B             //LED şerit tipi.

#define STATUS_LED 7                //Durum bildirme ledi.
#define BLADE_LED_PIN 6             //LED şeritin data pini.
#define MOTOR_PIN 5                 //Titreşim motorunun pini.
#define MAIN_BUTTON_PIN 4           //Açma/Kapama butonu pini.
#define BUSY_PIN 3                  //DFplayer'ın meşgullük bildiren pini, Çalıyor = 0, Boşta = 1

#define SFX_FILE_COUNT 10
#define MEMORY_ADRESS_COUNT 13


/*---------- İç Değişkenler ----------*/       //Elle DEĞİŞTİRİLMEMESİ gereken değişkenler.

CRGB colorPresets[7] = {CRGB(0, 0, 255), CRGB(0, 255, 0), CRGB(0, 0, 255), CRGB(255, 255, 0), CRGB(255, 0, 255),
                        CRGB(255, 255, 255), CRGB(255, 0, 0)};
CRGB currentBladeColor;
byte colorPresetIndex = 1;
byte movementValue;
byte internalVolume, prevInternalVolume;   
byte errorCounter = 0, buttonClickCount = 0;
byte brightness, flickerBrightness;
byte randomBrightness;
byte unstableLedToFlicker;
sensors_event_t accel, gyro, temp;
byte sumAccel, sumGyro;
unsigned long ignitionMillis, flickerMillis, soundEngineMillis, soundFadeMillis;
unsigned long ignitionSoundFadeTimer, bladeSwitchCooldownTimer, mainButtonFunctionTimer, appSerialTimer;
unsigned int randomFlicker = 0; 
int currentLed1 = 0, currentLed2 = NUM_LEDS - 1;
bool saberIsOn = false, mainButtonState = false, switchingBlade = false, flickered = false;
bool soundFading = false, moving = false, colorChangeOverride = false, holdingButton = false;
void emptyFunction(){}; //Eğer app bağlantısı yoksa loop içinde kaynak harcamamak için delegate'e atanacak fonk.
void (*appInterfaceDelegate)() = emptyFunction; //Loop içinde dönecek app arayüzünün delegate fonksiyonu.


/*---------- Objeler ----------*/

SoftwareSerial FPSerial(/*tx =*/11, /*rx =*/10);
CRGB leds[NUM_LEDS];
DFPlayerMini_Fast df;
Adafruit_MPU6050 mpu;


/*---------- Parametreler ----------*/    //Belli özellikleri kişiselleştirmek için gelişmiş seçenekler.

//IŞIK
byte bladeIsUnstable = 0;               //Bıçağın dengesizce yanıp sönmesini sağlar (kylo ren efekti).
byte unstabilityIntensity = 60;                 //Aynı anda yakıp söndürülecek led sayısı.
byte unstableFlickerFreqLimit = 1;          //Bıçağın titreme frekansının üst sınırı, 0 ile bu değer arasında rastgele oluşturulur.
byte unstableFlickerBrightnessLimit = 255;  //Bıçağın titreme yaparken dönüşüm yapacağı parlaklık çarpanı üst sınırı.

CRGB bladeColor; //Bıçak rengi.
byte customBladeColor_R = 255;
byte customBladeColor_G = 255;
byte customBladeColor_B = 255;

byte flickerFreqLimit = 100;           //Bıçağın titreme frekansının üst sınırı, 0 ile bu değer arasında rastgele oluşturulur.
byte flickerBrightnessPercent = 75;    //Bıçağın titreme yaparken düşeceği parlaklık yüzdesi.

byte ignitionSpeed = 5;                //Ateşleme/Geri çekme hızı, Azaldıkça hızlanır.
byte ledPerStep = 2;                   //Her ateşleme adımında yakılacak LED miktarı.


//SES
byte idleVolume = 15;                  //Ses seviyesi, yükseltmek sallama efektini güçsüzleştirir. Önerilen Max: 20
byte soundEngineFreq = 50;             //Ses motoru denetleme frekansı, azaldıkça hassaslık artar, tutarlılık azalır. Min: 50
int ignitionSoundFadeLenght = 200;     //Ateşleme sesinin ateşleme bittikten sonra varsayılana dönmesi için verilen süre (ms).
byte soundPackIndex = 0;               //Ses efekti paketinin indexi.


//TİTREŞİM
byte idleMotorPower = 75;              //Hareketsiz durumdayken titreşim motoru gücü, Min: 75, Max: 150
byte maxMotorPower = 255;              //Motorun çıkabileceği maksimum güç, Max: 255


//BUTON
int bladeSwitchCooldownLenght = 1500;  //Ateşleme tetiklendikten sonra yeniden tetiklenebilmesi için geçmesi gereken gereken süre (ms).
int mainButtonFunctionLenght = 400;    //Ana butona basıldıktan sonra başka fonksiyonları tetiklemek için arka arkaya basmalar arasındaki max süre (ms).

byte* configurableParameters[MEMORY_ADRESS_COUNT+1] = 
{&colorPresetIndex, &bladeIsUnstable, &unstabilityIntensity,
&customBladeColor_R, &customBladeColor_G, &customBladeColor_B,
&flickerFreqLimit, &flickerBrightnessPercent, &ignitionSpeed, 
&ledPerStep, &idleVolume, &soundPackIndex, &idleMotorPower, &maxMotorPower};


/*---------- Başlangıç Fonksiyonları ----------*/


//MARK:TryConnectToApp
void tryConnectToApp()
{
  for (byte i = 0; i < 3; i++)
  {
    Serial.write(COM_OFFER_CODE); //Uygulamaya çağrı gönder.
    delay(50);
    if (Serial.read() == COM_EXPECTED_RESPONSE_CODE)  //Beklenen cevap gelirse;
    {
      Serial.write(COM_SUCCESSFULL);  //Uygulamaya bağlandığını bildir.
      appInterfaceDelegate = changeParametersWithApp; //App bağlantısı varsa delegate'e gerçek fonskiyonu ata.

      #if DEBUG
      Serial.println(F("Windows uygulamasina baglandi"));
      #endif

      return; //Fonksiyondan çık.
    }
    else
    {
      Serial.write(COM_FAILED); //Uygulamaya beklentinin karşılanmadığını bildir.

      #if DEBUG
      Serial.println(F("Windows uygulama baglantisi basarisiz yeniden deneniyor."));
      #endif

      delay(50); //Beklenen cevap gelmezse biraz bekle ve yeniden dene.
    }
  }

  //3 kere denenmesine rağmen uygulamaya bağlanamazsa denemeyi bırak:
  #if DEBUG
  Serial.println(F("Windows uygulamasi tespit edilemedi. Rutine devam ediliyor."));
  #endif

  Serial.end();
}


//MARK:RememberChoices
void rememberChoices()
{
  colorPresetIndex = EEPROM.read(COLOR_PRESET_MEMORY);
  bladeColor = colorPresets[colorPresetIndex];
  if (colorPresetIndex == 0)
  {
    applyPreset(EEPROM.read(CUSTOM_PRESET_MEMORY));
  }
}


//MARK:SetPins
void setPins()
{
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);

  pinMode(MAIN_BUTTON_PIN, INPUT);
  pinMode(BUSY_PIN, INPUT);
}


//MARK:LedIntialize
void ledInitialize()
{
  FastLED.addLeds<CHIPSET, BLADE_LED_PIN, GRB>(leds, NUM_LEDS);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  brightness = 255; //65025 / (bladeColor.r + bladeColor.g + bladeColor.b);
  FastLED.setBrightness(brightness);
  FastLED.show();
  randomSeed(analogRead(0));
}


//MARK:DFPlayerInitialize
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


//MARK:MPUInitialize
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


//MARK:CatchErrors
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


//MARK:FinalizeSetup
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


//MARK:setup
void setup()
{
  //Başlangıç rutini:
  Serial.begin(9600);
  tryConnectToApp();
  setPins();
  rememberChoices();
  DFPlayerInitialize();
  MPUInitialize();
  ledInitialize();
  catchErrors();
  finalizeSetup();
}


//MARK:loop
void loop()
{
  //Döngü rutini:
  checkMainButton();
  flicker();
  fadeSound();
  soundEngine();
  switchBlade();
  appInterfaceDelegate();
}



/*---------- Çok İşlevli Fonksiyonlar ----------*/


//MARK:CheckMainButton
void checkMainButton()
{
  if ((millis() - mainButtonFunctionTimer > mainButtonFunctionLenght+1000) && buttonClickCount != 0 && holdingButton)
  {
    applyPreset(buttonClickCount);
    sSB_Override();
    buttonClickCount = 0;
  }
  else if ((millis() - mainButtonFunctionTimer > mainButtonFunctionLenght) && buttonClickCount != 0 && !holdingButton)
  {
    switch (buttonClickCount)
    {
      case 1:
        startSwitchingBlade();
        break;

      case 2:
        changeBladeColor();
        break;

      default:
        break;
    }

    buttonClickCount = 0;
  }

  if (!digitalRead(MAIN_BUTTON_PIN)) {mainButtonState = false; holdingButton = false; return;}
  if (mainButtonState) {holdingButton = true; return;}
  if (digitalRead(MAIN_BUTTON_PIN) && !mainButtonState) {mainButtonState = true;}

  buttonClickCount++;
  mainButtonFunctionTimer = millis();
}


//MARK:ApplyPreset
void applyPreset(byte presetIndex)
{
  colorPresetIndex = 0;
  EEPROM.update(COLOR_PRESET_MEMORY, colorPresetIndex);
  EEPROM.update(CUSTOM_PRESET_MEMORY, presetIndex);

  *configurableParameters[1] = EEPROM.read(MEMORY_ADRESS_COUNT*presetIndex + UNSTABLE_BLADE_MEMORY) == 1;
  for (byte i = 2; i <= MEMORY_ADRESS_COUNT; i++)
  {
    *configurableParameters[i] = EEPROM.read(MEMORY_ADRESS_COUNT*presetIndex + i);

    #if DEBUG
    Serial.print(F(">"));
    Serial.print(i);
    Serial.print(F(". Parametre uygulandi: "));
    Serial.println(*configurableParameters[i]);
    #endif
  }

  bladeColor = CRGB(customBladeColor_R, customBladeColor_G, customBladeColor_B);

  #if DEBUG
  Serial.print(presetIndex);
  Serial.println(F(". On ayar uygulandi"));
  #endif
}


//MARK:SavePreset
//indexleme 1'den başlar.
void savePreset(byte presetIndex)    //Windows uygulamasını yapana kadar geçici preset kaydetme fonksiyonu.
{
  EEPROM.update(MEMORY_ADRESS_COUNT*presetIndex + UNSTABLE_BLADE_MEMORY, *configurableParameters[1] ? (byte)1:(byte)0);
  for (byte i = 2; i <= MEMORY_ADRESS_COUNT; i++)
  {
    EEPROM.update(MEMORY_ADRESS_COUNT*presetIndex + i, *configurableParameters[i]);

    #if DEBUG
    Serial.print(F(">"));
    Serial.print(i);
    Serial.print(F(". Parametre guncellendi: "));
    Serial.println(*configurableParameters[i]);
    #endif
  }
  #if DEBUG
  Serial.print(presetIndex);
  Serial.println(F(". On ayar yuvasi yazildi."));
  #endif
  delay(1000);
}


//MARK:ChangeWithApp
void changeParametersWithApp()
{
  if(Serial.available() != 2) {return;}
  
  byte parameterIndex = Serial.read();
  byte value = Serial.read();

  if (parameterIndex == COM_SAVE_PRESET)
  {
    #if DEBUG
    Serial.print(F("Değerler "));
    Serial.print(value);
    Serial.println(F(". ayar yuvasina kaydedildiliyor...\n"));
    #endif

    savePreset(value);
  }
  else
  {
    *configurableParameters[parameterIndex] = value;

    if (parameterIndex == 3, 4, 5)
    {
      bladeColor = CRGB(customBladeColor_R, customBladeColor_G, customBladeColor_B);
      fill_solid(leds, NUM_LEDS, bladeColor);
    }
    

    #if DEBUG
    Serial.print(parameterIndex);
    Serial.print(F(". ayar '"));
    Serial.print(value);
    Serial.println(F("' olarak guncellendi."));
    #endif
  }
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
    df.play(SFX_FILE_COUNT*soundPackIndex + SFX_IGNITE_AND_HUM);
    delay(60);
    internalVolume = 30;
    analogWrite(MOTOR_PIN, maxMotorPower);
    soundFadeMillis = millis();
    flickerBrightness = brightness * flickerBrightnessPercent / 100; 

    #if DEBUG
    digitalWrite(STATUS_LED, 1);
    Serial.println(F("igniting"));
    #endif
  }
  else
  {
    saberIsOn = false;
    setBladeColor(CRGB::Black);
    df.play(SFX_FILE_COUNT*soundPackIndex + SFX_RETRACT);
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


//MARK:SS_Override
//Bıçak zaten açıksa ve renk değiştirilicekse kapanmayı geçersiz kılar.
void sSB_Override()
{
  currentLed1 = 0;
  currentLed2 = NUM_LEDS;
  colorChangeOverride = true;
  startSwitchingBlade();
}


void switchBlade()  //MARK:SwitchBlade
{
  if (!switchingBlade || millis() - ignitionMillis < ignitionSpeed) {return;}

  for (int i = 0; i<ledPerStep; i++)   //Igniting
  {
    leds[currentLed1] = currentBladeColor;
    leds[currentLed2] = currentBladeColor;
    if (saberIsOn && currentLed1 < 58)
    {
      leds[currentLed1+1] = CRGB::White;
      leds[currentLed2-1] = CRGB::White;
      leds[currentLed1+2] = CRGB::White;    //nasıl durduğunun farkındayım bi ara düzeltecem inş.
      leds[currentLed2-2] = CRGB::White;
      leds[currentLed1+3] = CRGB::White;
      leds[currentLed2-3] = CRGB::White;
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
  if ((saberIsOn || switchingBlade) && (millis() - flickerMillis > randomFlicker))
  {
    randomFlicker = random(0, flickerFreqLimit);
    if (!flickered)
    {
      FastLED.setBrightness(brightness);
      flickered = true;
    }
    else
    {
      FastLED.setBrightness(flickerBrightness);
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
      for (byte i = 0; i <= unstabilityIntensity; i++)
      {
        unstableLedToFlicker = random(0, NUM_LEDS);
        if (leds[unstableLedToFlicker] != CRGB::Black)
        {
          randomBrightness = random(1.3, unstableFlickerBrightnessLimit);
          leds[unstableLedToFlicker] = 
          CRGB((currentBladeColor.r / randomBrightness), (currentBladeColor.g / randomBrightness), (currentBladeColor.b / randomBrightness));
        }
      }
      flickered = true;
    }
    else
    {
      for (byte i = 0; i <= NUM_LEDS; i++)
      {
        if (leds[i] != CRGB::Black)
        {
          leds[i] = currentBladeColor;
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
  colorPresetIndex++;
  if (bladeColor == colorPresets[colorPresetIndex]) {colorPresetIndex++;}
  if (colorPresetIndex > 6) {colorPresetIndex = 1;}
  bladeColor = colorPresets[colorPresetIndex];
  // brightness = 65025 / (bladeColor.r + bladeColor.g + bladeColor.b);
  // FastLED.setBrightness(brightness);
  // FastLED.show();

  if (saberIsOn) {sSB_Override();}

  EEPROM.update(COLOR_PRESET_MEMORY, colorPresetIndex);

  #if DEBUG
  Serial.println(F(">Renk kaydedildi."));
  #endif
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
  if (!soundFading) {return;}


  if (!saberIsOn && (millis() - soundFadeMillis > 50))
  {
    internalVolume = map(currentLed1, -1, 61, 31, idleVolume);
    df.volume(internalVolume);
    #if DEBUG_SOUND
    //Serial.println(internalVolume);
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
    //Serial.println(internalVolume);
    #endif
  }

  analogWrite(MOTOR_PIN, map(internalVolume, idleVolume, 30, idleMotorPower, maxMotorPower));

  if (internalVolume < idleVolume || internalVolume > 30)
  {
    soundFading = false;
    analogWrite(MOTOR_PIN, idleMotorPower);
  }
}


