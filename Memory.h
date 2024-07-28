//Hafıza bölümlerinin indexleri

#ifndef MEMORY_H
#define MEMORY_H

    #define COLOR_PRESET_MEMORY 0               //EEPROM'un bıçak rengi ön ayarı indexini tutan hafıza birimi.

    #define UNSTABLE_BLADE_MEMORY 1             //Bıçağın dengesizlik durumunu tutan hafıza birimi.
    #define UNSTABILITY_INTENSITY_MEMORY 2      //Bıçağın dengesiz durumdayken dengesizlik yoğunluğunu tutan hafıza birimi.

    #define BLADE_R_MEMORY 3                    //Bıçağın özel ayarlanmış kırmızı rengini tutan hafıza birimi.
    #define BLADE_G_MEMORY 4                    //Bıçağın özel ayarlanmış yeşil rengini tutan hafıza birimi.
    #define BLADE_B_MEMORY 5                    //Bıçağın özel ayarlanmış mavi rengini tutan hafıza birimi.

    #define FLICKER_FREQ_MEMORY 6               //Bıçağın boşta beklerken yanıp sönme frekansının üst sınırını tutan hafıza birimi.
    #define FLICKER_BRIGHTNESS_MEMORY 7         //Bıçağın boşta beklerken yanıp sönme parlaklığını tutan hafıza birimi.

    #define IGNITION_SPEED_MEMORY 8             //Bıçağın ateşlenirken/geri-çekilirkenki hızını tutan hafıza birimi.
    #define LED_PER_STEP_MEMORY 9               //Bıçağın ateşlenirken/geri-çekilirken her adımda açılan/kapanan led sayısını tutan hafıza birimi.

    #define VOLUME_MEMORY 10                    //Ses seviyesini tutan hafıza birimi.
    #define SOUND_PACK_INDEX_MEMORY 11          //Ses efekti paketi indexini tutan hafıza birimi.
    
    #define MOTOR_IDLE_POWER_MEMORY 12          //Titreşim motorunun boştaki gücünü tutan hafıza birimi.
    #define MOTOR_MAX_POWER_MEMORY 13           //Tireşim motorunun çıkabileceği max güç'ü tutan hafıza birimi.


    #define CUSTOM_PRESET_MEMORY 1023           //Seçilen ön ayarın kaydedildiği hafıza birimi.
#endif