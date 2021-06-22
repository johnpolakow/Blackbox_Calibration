#include "pins.h"

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


void Initialize_Relay_Board_GPIO_pins()
{

    	bcm2835_gpio_fsel(K1_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
    	bcm2835_gpio_write(K1_SET_SIGNAL, LOW);

        bcm2835_gpio_fsel(K15_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
    	bcm2835_gpio_write(K15_SET_SIGNAL, LOW);

    	bcm2835_gpio_fsel(BANK_A_RESET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
    	bcm2835_gpio_write(BANK_A_RESET_SIGNAL, LOW);

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

        bcm2835_gpio_fsel(K17_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(K17_SET_SIGNAL, LOW);

        bcm2835_gpio_fsel(K16_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(K16_SET_SIGNAL, LOW);

        bcm2835_gpio_fsel(BANK_B_RESET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(BANK_B_RESET_SIGNAL, LOW);

        bcm2835_gpio_fsel(K10_SET_SIGNAL, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(K10_SET_SIGNAL, LOW);

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
