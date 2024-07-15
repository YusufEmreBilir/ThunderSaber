/*

---------- ThunderSaberOS_V3 By YEB ----------

MPU 6050 ve DFPlayer ile çalışması için tasarlanmıştır,
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

#define DEBUG true                  //Seri monitör vs. için debug ayarı.

#define NUM_LEDS 120                //Bıçaktaki LED sayısı.
#define BLADE_LED_PIN 6             //LED şeritin data pini.
#define MOTOR_PIN 5                 //Titreşim motorunun pini.
#define ACTIVATION_BUTTON_PIN 4     //Açma/Kapama butonu pini.
#define BUSY_PIN 3                  //DFplayer'ın meşgullük bildiren pini, Çalıyor = 0, Boşta = 1

#define SWING_STOP_TRESHOLD 1       //Sallama sesini başlatmak için gerek hareket miktarının hassasiyeti.
#define SWING_START_TRESHOLD 1


/*---------- İç Değişkenler ----------*/       //Elle değiştirilmemesi gereken değişkenler.

byte stationary, moving;
byte internalVolume, prevInternalVolume;                    
sensors_event_t a, g, temp;
unsigned long ignitionMillis, flickerMillis, soundEngineMillis;
unsigned int randomFlicker = 0; 
int currentLed1 = 0, currentLed2 = 119;
bool saberIsOn = false, igniting = false, flickered = false;


/*---------- Objeler ----------*/

SoftwareSerial FPSerial(/*tx =*/11, /*rx =*/10);
Adafruit_NeoPixel led (NUM_LEDS, BLADE_LED_PIN, NEO_GRB + NEO_KHZ800);
DFPlayerMini_Fast df;
Adafruit_MPU6050 mpu;


/*---------- Parametreler ----------*/    //Belli özellikleri kişiselleştirmek için gelişmiş seçenekler.

byte selectedR = 0;
byte selectedG = 0;           //Bıçak Rengi.
byte selectedB = 255;

byte VOLUME = 15;             //Ses seviyesi, Max: 20
byte BRIGHTNESS = 10;         //Parlaklık, Max: 255

byte ignitionSpeed = 10;      //Ateşleme/Geri çekme hızı, Azaldıkça hızlanır.
byte ledPerStep = 1;          //Her ateşleme adımında yakılacak LED miktarı.

byte soundEngineFreq = 50;    //Ses motoru denetleme frekansı, azaldıkça hassaslık artar, Min: 50
byte flickerFreqLimit = 100;  //Bıçağın titreme frekansının üst sınırı, 0 ile değer arasında rastgele oluşturulur.
byte motorPower = 75;         //Hareketsiz durumdayken titreşim motoru gücü, Min: 75, Max: 150


void setup() 
{
    #if DEBUG                 //Debug mod aktifse;
    Serial.begin(115200);     //seri monitörü başlat.
    #endif

    FPSerial.begin(9600);
    if(!df.begin(FPSerial, false))
    {
      Serial.println(F("df player hata"));
    }
    delay(1000);
    df.stop();
    delay(500);
    df.reset();
    delay(500);
    df.volume(VOLUME);
    internalVolume = VOLUME;
    prevInternalVolume = internalVolume;
    pinMode(MOTOR_PIN, OUTPUT);
    pinMode(ACTIVATION_BUTTON_PIN, INPUT);
    pinMode(BUSY_PIN, INPUT);
    led.begin();          
    led.show();          
    led.setBrightness(BRIGHTNESS);
    if(!mpu.begin()) {Serial.println(F("mpu hata"));}
    mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
    mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    randomSeed(analogRead(0));

}

void loop() 
{
  activationButtonCheck();
  flicker();
  soundEngine();
}

void activationButtonCheck()
{

  if (igniting) {switchBlade(saberIsOn);}
  if (digitalRead(ACTIVATION_BUTTON_PIN) == 0) {return;}
  if (!saberIsOn && !igniting)
  {
    df.play(1);
    analogWrite(MOTOR_PIN, 120); //motor başlat, başlatma hızı min: 120
    saberIsOn = true;
    Serial.println(F("igniting"));
  }
  else if (!igniting)
  {
    df.play(3);
    analogWrite(MOTOR_PIN, 120);
    saberIsOn = false;
    Serial.println(F("retracting"));
  }
  igniting = true;
}


void switchBlade(bool state)
{
  if (millis() - ignitionMillis > ignitionSpeed)
  {
    // Serial.print(currentLed1);
    // Serial.print(F("\t"));
    // Serial.println(currentLed2);

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
      Serial.println(F("ignited"));
      analogWrite(MOTOR_PIN, motorPower); //motor idle
      currentLed1 = 60;
      currentLed2 = 60;
      return;
    }
    else if (currentLed1 <= -1)
    {
      igniting = false;
      Serial.println(F("retracted"));
      analogWrite(MOTOR_PIN, 0); //motor durdur
      currentLed1 = 0;
      currentLed2 = NUM_LEDS;
      return;
    }

  }

}


void flicker()
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

void soundEngine()
{
if (igniting || !saberIsOn) {return;}
if (millis() - soundEngineMillis < soundEngineFreq) {return;}
mpu.getEvent(&a, &g, &temp);
byte sumAccel = abs(a.acceleration.x) + abs(a.acceleration.y) + abs(a.acceleration.z);
byte sumGyro = abs(g.gyro.x) + abs(g.gyro.y) + abs(g.gyro.z);

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
    Serial.print("moving,  Volume: ");
    Serial.println(internalVolume);

    analogWrite(MOTOR_PIN, map(internalVolume, VOLUME, 30, motorPower, 255));  //dinamik haptik geri bildirim (titreşim).
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
    Serial.println("stoppped");

    analogWrite(MOTOR_PIN, motorPower);  //titreşim motorunu eski seviyesine geri getir.
  }
}
soundEngineMillis = millis();
}
