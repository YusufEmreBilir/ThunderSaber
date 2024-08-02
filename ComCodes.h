


#ifndef COMCODES_H
#define COMCODES_H


#define COM_OFFER_CODE                  0xA1    //Kılıcın windows uygulamasına iletişim teklifi için göndereceği kod.
#define COM_EXPECTED_RESPONSE_CODE      0xA2    //Kılıcın iletişim teklifi karşılığında beklediği kod.
#define COM_SUCCESSFULL                 0xAA    //Kılıcın iletişim beklentileri karşıladığında uygulamaya ileteceği kod.
#define COM_FAILED                      0xFF    //Kılıcın bağlandığı ama beklediği onay kodunu alamadığında göndereceği kod.
#define COM_SAVE_PRESET                 0xBB    //Seçilen ayarların EEPROM'a kaydedilmesi için gönderilecek kod.
#define COM_UPLOAD_SUCCESSFULL          0xAB    //Ayarlar kaydedilirse uygulamaya gönderilecek kod.


#define COM_COLOR_PRESET 0x00                   //Bıçak rengi ön ayarı indexinin iletişim kodu.

#define COM_UNSTABLE_BLADE 0x01                 //Bıçağın dengesizlik durumunun iletişim kodu.
#define COM_UNSTABILITY_INTENSITY 0x02          //Bıçağın dengesiz durumdayken dengesizlik yoğunluğunun iletişim kodu.

#define COM_BLADE_R 0x03                        //Bıçağın özel ayarlanmış kırmızı renginin iletişim kodu.
#define COM_BLADE_G 0x04                        //Bıçağın özel ayarlanmış yeşil renginin iletişim kodu.
#define COM_BLADE_B 0x05                        //Bıçağın özel ayarlanmış mavi renginin iletişim kodu.

#define COM_FLICKER_FREQ 0x06                   //Bıçağın boşta beklerken yanıp sönme frekansının üst sınırının iletişim kodu.
#define COM_FLICKER_BRIGHTNESS 0x07             //Bıçağın boşta beklerken yanıp sönme parlaklığının iletişim kodu.

#define COM_IGNITION_SPEED 0x08                 //Bıçağın ateşlenirken/geri-çekilirkenki hızının iletişim kodu.
#define COM_LED_PER_STEP 0x09                   //Bıçağın ateşlenirken/geri-çekilirken her adımda açılan/kapanan led sayısının iletişim kodu.

#define COM_VOLUME 0x0A                         //Ses seviyesinin iletişim kodu.
#define COM_SOUND_PACK_INDEX 0x0B               //Ses efekti paketi indexinin iletişim kodu.

#define COM_MOTOR_IDLE_POWER 0x0C               //Titreşim motorunun boştaki gücünün iletişim kodu
#define COM_MOTOR_MAX_POWER 0x0D                //Tireşim motorunun çıkabileceği max güç'ün iletişim kodu.

#define COM_CUSTOM_PRESET 0x0E                  //Seçilen ön ayarın iletişim kodu.



#endif