#include "Pin_Defs.h"

#ifndef INIT_BCM2835
#define INIT_BCM2835

#ifdef __arm__
    #include <bcm2835.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

bool Init_BCM2835_Library();
void Initialize_Relay_Board_GPIO_pins();

void Assign_Relay_Board_GPIO_pins()
{
    K1.bcm_set_pin = K1_SET_SIGNAL;
    K1.bcm_reset_pin = K1_RESET_SIGNAL;
    K1.relay_number = 1;
    K1.physical_set_pin = 7;
    K1.physical_reset_pin = 11;

    K2.bcm_set_pin = K2_SET_SIGNAL;
    K2.bcm_reset_pin = RES_ARRAY_RESET_SIGNAL;
    K2.relay_number = 2;
    K2.physical_set_pin = 13;
    K2.physical_reset_pin = 31;

    K3.bcm_set_pin = K3_SET_SIGNAL;
    K3.bcm_reset_pin = RES_ARRAY_RESET_SIGNAL;
    K3.relay_number = 3;
    K3.physical_set_pin = 15;
    K3.physical_reset_pin = 31;

    K4.bcm_set_pin = K4_SET_SIGNAL;
    K4.bcm_reset_pin = RES_ARRAY_RESET_SIGNAL;
    K4.relay_number = 4;
    K4.physical_set_pin = 19;
    K4.physical_reset_pin = 31;

    K5.bcm_set_pin = K5_SET_SIGNAL;
    K5.bcm_reset_pin = RES_ARRAY_RESET_SIGNAL;
    K5.relay_number = 5;
    K5.physical_set_pin = 9;
    K5.physical_reset_pin = 31;

    K6.bcm_set_pin = K6_SET_SIGNAL;
    K6.bcm_reset_pin = RES_ARRAY_RESET_SIGNAL;
    K6.relay_number = 6;
    K6.physical_set_pin = 11;
    K6.physical_reset_pin = 31;

    K7.bcm_set_pin = K7_SET_SIGNAL;
    K7.bcm_reset_pin = RES_ARRAY_RESET_SIGNAL;
    K7.relay_number = 7;
    K7.physical_set_pin = 5;
    K7.physical_reset_pin = 31;

    K8.bcm_set_pin = K8_SET_SIGNAL;
    K8.bcm_reset_pin = BANK_A_RESET_SIGNAL;
    K8.relay_number = 8;
    K8.physical_set_pin = 13;
    K8.physical_reset_pin = 24;

    K9.bcm_set_pin = K9_SET_SIGNAL;
    K9.bcm_reset_pin = BANK_A_RESET_SIGNAL;
    K9.relay_number = 9;
    K9.physical_set_pin = 19;
    K9.physical_reset_pin = 24;

    K10.bcm_set_pin = K10_SET_SIGNAL;
    K10.bcm_reset_pin = BANK_A_RESET_SIGNAL;
    K10.relay_number = 10;
    K10.physical_set_pin = 26;
    K10.physical_reset_pin = 24;

    K11.bcm_set_pin = K11_SET_SIGNAL;
    K11.bcm_reset_pin = BANK_A_RESET_SIGNAL;
    K11.relay_number = 11;
    K11.physical_set_pin = 21;
    K11.physical_reset_pin = 24;

    K12.bcm_set_pin = K12_SET_SIGNAL;
    K12.bcm_reset_pin = BANK_B_RESET_SIGNAL;
    K12.relay_number = 12;
    K12.physical_set_pin = 20;
    K12.physical_reset_pin = 22;

    K13.bcm_set_pin = K13_SET_SIGNAL;
    K13.bcm_reset_pin = BANK_B_RESET_SIGNAL;
    K13.relay_number = 13;
    K13.physical_set_pin = 36;
    K13.physical_reset_pin = 22;

    K14.bcm_set_pin = K14_SET_SIGNAL;
    K14.bcm_reset_pin = BANK_B_RESET_SIGNAL;
    K14.relay_number = 14;
    K14.physical_set_pin = 12;
    K14.physical_reset_pin = 22;

    K15.bcm_set_pin = K15_SET_SIGNAL;
    K15.bcm_reset_pin = BANK_B_RESET_SIGNAL;
    K15.relay_number = 15;
    K15.physical_set_pin = 7;
    K15.physical_reset_pin = 22;

    K16.bcm_set_pin = K16_SET_SIGNAL;
    K16.bcm_reset_pin = CAL_RESET_SIGNAL;
    K16.relay_number = 16;
    K16.physical_set_pin = 18;
    K16.physical_reset_pin = 12;

    K17.bcm_set_pin = K17_SET_SIGNAL;
    K17.bcm_reset_pin = CAL_RESET_SIGNAL;
    K17.relay_number = 17;
    K17.physical_set_pin = 16;
    K17.physical_reset_pin = 12;
}

void Initialize_Relay_Board_GPIO_pins()
{
    	bcm2835_gpio_fsel(LTC2057_SHDN, BCM2835_GPIO_FSEL_OUTP);			   // physical pin 3, declared as output pin
    	bcm2835_gpio_write(LTC2057_SHDN, LOW);                           // initialize by writing low (inactive)

    	bcm2835_gpio_fsel(K1_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
    	bcm2835_gpio_write(K1_SET_SIGNAL, LOW);

    	bcm2835_gpio_fsel(K7_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
    	bcm2835_gpio_write(K7_SET_SIGNAL, LOW);

        bcm2835_gpio_fsel(RES_ARRAY_RESET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(RES_ARRAY_RESET_SIGNAL, LOW);

        bcm2835_gpio_fsel(K15_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
    	bcm2835_gpio_write(K15_SET_SIGNAL, LOW);

    	bcm2835_gpio_fsel(BANK_A_RESET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
    	bcm2835_gpio_write(BANK_A_RESET_SIGNAL, LOW);

        bcm2835_gpio_fsel(K5_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(K5_SET_SIGNAL, LOW);

        bcm2835_gpio_fsel(K4_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(K4_SET_SIGNAL, LOW);

        bcm2835_gpio_fsel(K6_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(K6_SET_SIGNAL, LOW);

        bcm2835_gpio_fsel(K14_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(K14_SET_SIGNAL, LOW);

        bcm2835_gpio_fsel(K8_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(K8_SET_SIGNAL, LOW);

        bcm2835_gpio_fsel(K13_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(K13_SET_SIGNAL, LOW);

        bcm2835_gpio_fsel(K1_RESET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(K1_RESET_SIGNAL, LOW);

        bcm2835_gpio_fsel(CAL_RESET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(CAL_RESET_SIGNAL, LOW);

        bcm2835_gpio_fsel(K9_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(K9_SET_SIGNAL, LOW);

        bcm2835_gpio_fsel(K12_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(K12_SET_SIGNAL, LOW);

        bcm2835_gpio_fsel(K11_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(K11_SET_SIGNAL, LOW);

        bcm2835_gpio_fsel(K3_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(K3_SET_SIGNAL, LOW);

        bcm2835_gpio_fsel(K17_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(K17_SET_SIGNAL, LOW);

        bcm2835_gpio_fsel(K16_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(K16_SET_SIGNAL, LOW);

        bcm2835_gpio_fsel(BANK_B_RESET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(BANK_B_RESET_SIGNAL, LOW);

        bcm2835_gpio_fsel(K10_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(K10_SET_SIGNAL, LOW);

        bcm2835_gpio_fsel(K2_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(K2_SET_SIGNAL, LOW);
}


bool Init_BCM2835_Library()
{
    if (!bcm2835_init())
    {
        printf("bcm2835_init failed. Are you running as root?? ...use sudo \n");
        return false;
    }
    return true;
}


#ifdef __cplusplus
}
#endif

#endif
