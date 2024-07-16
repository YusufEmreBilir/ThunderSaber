/*

---------- ThunderSaberOS_V3 By YEB ----------

MPU 6050, DFPlayer mini ve 60LED/m WS2812B ile çalışması için tasarlanmıştır,
arduino nano ve uno üzerinde test edilmiştir.

Son revizyon: 2024

-Yusuf Emre Bilir

*/


/*---------- Kütüphaneler ----------*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>
#include <DFPlayerMini_Fast.h>
#include <SoftwareSerial.h>
#include <Adafruit_MPU6050.h>

/*---------- Pre-Proccessor Değerler ----------*/

#define DEBUG true                  //Seri monitör vs. için debug ayarı. Açık olması Arduinoyu bir miktar yavaşlatır.

#define NUM_LEDS 120                //Bıçaktaki LED sayısı.
#define CHIPSET WS2812B             //LED şerit tipi.

#define STATUS_LED 7                //Durum bildirme ledi.
#define BLADE_LED_PIN 6             //LED şeritin data pini.
#define MOTOR_PIN 5                 //Titreşim motorunun pini.
#define ACTIVATION_BUTTON_PIN 4     //Açma/Kapama butonu pini.
#define BUSY_PIN 3                  //DFplayer'ın meşgullük bildiren pini, Çalıyor = 0, Boşta = 1

#define SWING_START_TRESHOLD 1      //Sallama rutinini başlatmak için gereken hareket miktarının hassasiyeti.
#define SWING_STOP_TRESHOLD 1       //Sallama rutinini durdurmak için gereken hareketsizlik miktarının hassasiyeti.


/*---------- İç Değişkenler ----------*/       //Elle DEĞİŞTİRİLMEMESİ gereken değişkenler.

byte stationary, moving;
byte internalVolume, prevInternalVolume;   
byte errorCounter = 0;                 
sensors_event_t accel, gyro, temp;
unsigned long ignitionMillis, flickerMillis, soundEngineMillis, mainButtonMillis;
unsigned int randomFlicker = 0; 
int currentLed1 = 0, currentLed2 = NUM_LEDS - 1;
bool saberIsOn = false, igniting = false, flickered = false;


/*---------- Objeler ----------*/

SoftwareSerial FPSerial(/*tx =*/11, /*rx =*/10);
Adafruit_NeoPixel led (NUM_LEDS, BLADE_LED_PIN, NEO_GRB + NEO_KHZ800);
DFPlayerMini_Fast df;
Adafruit_MPU6050 mpu;


/*---------- Parametreler ----------*/    //Belli özellikleri kişiselleştirmek için gelişmiş seçenekler.

byte selectedR = 0;           //
byte selectedG = 0;           //Bıçak Rengi.
byte selectedB = 255;         //

byte VOLUME = 10;             //Ses seviyesi, Max: 20
byte BRIGHTNESS = 10;         //Parlaklık, Max: 255

byte ignitionSpeed = 10;      //Ateşleme/Geri çekme hızı, Azaldıkça hızlanır.
byte ledPerStep = 1;          //Her ateşleme adımında yakılacak LED miktarı.

byte soundEngineFreq = 60;    //Ses motoru denetleme frekansı, azaldıkça hassaslık artar, tutarlılık azalır. Min: 50
byte mainButtonFreq = 50;     //Ana buton denetleme frekansı.
byte flickerFreqLimit = 100;  //Bıçağın titreme frekansının üst sınırı, 0 ile değer arasında rastgele oluşturulur.
byte motorPower = 75;         //Hareketsiz durumdayken titreşim motoru gücü, Min: 75, Max: 150


/*---------- Başlangıç Fonksiyonları ----------*/

void setPins()
{
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);

  pinMode(ACTIVATION_BUTTON_PIN, INPUT);
  pinMode(BUSY_PIN, INPUT);
}

void ledInitialize()
{
  led.begin();          
  led.show();          
  led.setBrightness(BRIGHTNESS);
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
  delay(100);         //peşpeşe gönderilen komutlar ya algılanmayacak yada komple senkronu bozacaktır.
  df.stop();          
  delay(100);
  df.volume(VOLUME);
  internalVolume = VOLUME;
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

void catchError()
{
  if (errorCounter > 0) 
  {
    delay(500);
    
    #if DEBUG
    df.play(6);
    Serial.println(F(">UYARI: Hata/Hatalar tespit edildi<"));
    #else
    df.play(5);
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


void setup() //MARK:setup
{
  #if DEBUG                 //Debug modu aktifse;
  Serial.begin(9600);     //seri monitörü başlat.
  #endif

  //Başlangıç rutini:
  setPins();
  DFPlayerInitialize();
  MPUInitialize();
  ledInitialize();
  catchError();

  digitalWrite(STATUS_LED, 1);
  delay(100);
  df.play(4);
  digitalWrite(STATUS_LED, 0);
  #if DEBUG
  Serial.println(F(">Baslatma basarili<"));
  #endif
}


void loop() //MARK:loop
{
  activationButtonCheck();
  soundEngine();
  flicker();
}


void activationButtonCheck()  //MARK:ActivationCheck
{
  if (millis() - mainButtonMillis < mainButtonFreq) {return;}
  if (igniting) {switchBlade(saberIsOn);}
  if (digitalRead(ACTIVATION_BUTTON_PIN) == 0) {return;}
  if (!saberIsOn && !igniting)
  {
    saberIsOn = true;
    df.play(1);     //Normalde Setup dışında delay kullanmak, multitasking yapılan bir kodda,  
    delay(600);     //mantıklı değil ancak burada zaten bütün donanımın DFplayer'ın çalmasını beklemesi gerekli.

    analogWrite(MOTOR_PIN, 120); //Motor başlat, başlatma hızı min: 120

    #if DEBUG
    digitalWrite(STATUS_LED, 1);
    Serial.println(F("igniting"));
    #endif
  }
  else if (!igniting)
  {
    saberIsOn = false;
    delay(50);
    df.volume(VOLUME);
    delay(50);
    df.play(3);
    analogWrite(MOTOR_PIN, 120);

    #if DEBUG
    digitalWrite(STATUS_LED, 0);
    Serial.println(F("retracting"));
    #endif
  }
  igniting = true;
  mainButtonMillis = millis();
}


void switchBlade(bool state)  //MARK:SwitchBlade
{
  if (millis() - ignitionMillis > ignitionSpeed)
  {
    if (state)
    {
      for (int i = 0; i<ledPerStep; i++)   //Igniting
      {
        led.setPixelColor(currentLed1, led.Color(selectedR, selectedG, selectedB));
        led.setPixelColor(currentLed2, led.Color(selectedR, selectedG, selectedB));
        if (currentLed1 < 58)
        {
            led.setPixelColor(currentLed1+1, led.Color(255, 255, 255));
            led.setPixelColor(currentLed2-1, led.Color(255, 255, 255)); //iyyrenç kod, bi ara düzeltçem.
            led.setPixelColor(currentLed1+2, led.Color(255, 255, 255));
            led.setPixelColor(currentLed2-2, led.Color(255, 255, 255));
            led.setPixelColor(currentLed1+3, led.Color(255, 255, 255));
            led.setPixelColor(currentLed2-3, led.Color(255, 255, 255));
        }
        currentLed2--;
        currentLed1++;
      }
    }
    else
    {
      for (int i = 0; i<ledPerStep; i++)   //Retracting
      {
        led.setPixelColor(currentLed1, led.Color(0,0,0));
        led.setPixelColor(currentLed2, led.Color(0,0,0));
        currentLed2++;
        currentLed1--;
      }
    }

    led.show();

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

}


void flicker()  //MARK:Flicker
{
  if (!igniting && saberIsOn && millis() - flickerMillis > randomFlicker)
  {
    randomFlicker =  random(0, flickerFreqLimit);
    if (!flickered)
    {
      led.setBrightness(BRIGHTNESS/1.5);
      led.show();
      flickered = true;
    }
    else
    {
      led.setBrightness(BRIGHTNESS*1.5);
      led.show();
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

if (sumAccel > 17 || sumGyro > 0.4)
{
  if (stationary <= SWING_STOP_TRESHOLD) {stationary++;}
  if (stationary < SWING_STOP_TRESHOLD) {return;}
  moving = 0;

  internalVolume = map(sumAccel, 10, 40, VOLUME, 30);
  if (internalVolume > 30) {internalVolume = 30;}

  if (internalVolume == VOLUME || internalVolume != prevInternalVolume)
  {
    prevInternalVolume = internalVolume;  //dinamik işitsel geri bildirim (sallama sesi).
    df.volume(internalVolume);
    analogWrite(MOTOR_PIN, map(internalVolume, VOLUME, 30, motorPower, 255));  //dinamik haptik geri bildirim (titreşim).

    #if DEBUG
    Serial.print("moving,  Volume: ");
    Serial.println(internalVolume);
    #endif
  }
}
else
{
  if (moving <= SWING_START_TRESHOLD) {moving++;}
  if (moving < SWING_START_TRESHOLD) {return;}
  stationary = 0;
  if (internalVolume != VOLUME)
  {
    internalVolume = VOLUME;  //sesi eski seviyesine geri getir.
    df.volume(VOLUME);
    analogWrite(MOTOR_PIN, motorPower);  //titreşim motorunu eski seviyesine geri getir.

    #if DEBUG
    Serial.println("stoppped");
    #endif
  }
}

soundEngineMillis = millis();
}
