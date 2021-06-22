// Pin Numbering
/*
Rasperry Pi Physical Pin Numbering w/ bcm2835 library pin Numbering
            BCM2835           Raspberry Pi
Function     pin#             physical pin#
SDA1          2                   3       LTC2057_SHDN
SCL1          3                   5       --
GPCLK0        4                   7       K1_SET_SIGNAL
GPCLK1        5                   29      K7_SET_SIGNAL
GPCLK2        6                   31      RES_ARRAY_RESET_SIGNAL
SPI0_CE1_N    7                   26      K15_SET_SIGNAL
SPI0_CE0_N    8                   24      BANK_A_RESET_SIGNAL
SPI0_MISO     9                   21      K5_SET_SIGNAL
SPI0_MOSI     10                  19      K4_SET_SIGNAL
SPI0_SCLK     11                  23      K6_SET_SIGNAL
PWM0          12                  32      K14_SET_SIGNAL
PWM1          13                  33      K8_SET_SIGNAL
TXD0          14                  8       UART_TX
RXD0          15                  10      UART_RX
SPI1_CE2      16                  36      K13_SET_SIGNAL
SPI1_CE1      17                  11      K1_RESET_SIGNAL
PCM_CLK       18                  12      CAL_RESET_SIGNAL
SPI1_MISO     19                  35      K9_SET_SIGNAL
SPI1_MOSI     20                  38      K12_SET_SIGNAL
SPI1_SCLK     21                  40      K11_SET_SIGNAL
SD0_CLK       22                  15      K3_SET_SIGNAL
SD0_CMD       23                  16      K17_SET_SIGNAL
SD0_DAT0      24                  18      K16_SET_SIGNAL
SD0_DAT1      25                  22      BANK_B_RESET_SIGNAL
SD0_DAT2      26                  37      K10_SET_SIGNAL
SD0_DAT3      27                  13      K2_SET_SIGNAL
              01                  28      ------
              00                  27      -----
*/

#ifndef PIN_DEFINITIONS
#define PIN_DEFINITIONS

#define ONE_MILLISECOND 1000

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __arm__
    #define HIGH  1
    #define LOW 0
#endif

                            //BCM2835 pin#
#define K1_SET_SIGNAL           4   // physical pin 7
#define K15_SET_SIGNAL          7   // physical pin 26
#define BANK_A_RESET_SIGNAL     8   // physical pin 24
#define K14_SET_SIGNAL          12  // physical pin 32
#define K8_SET_SIGNAL           13  // physical pin 33
#define K13_SET_SIGNAL          16  // physical pin 36
#define K1_RESET_SIGNAL         17  // physical pin 11
#define CAL_RESET_SIGNAL        18  // physical pin 12
#define K9_SET_SIGNAL           19  // physical pin 35
#define K12_SET_SIGNAL          20  // physical pin 38
#define K11_SET_SIGNAL          21  // physical pin 40
#define K17_SET_SIGNAL          23  // physical pin 16
#define K16_SET_SIGNAL          24  // physical pin 18
#define BANK_B_RESET_SIGNAL     25  // physical pin 22
#define K10_SET_SIGNAL          26  // physical pin 37


#define K1_RELAY     1
#define K8_RELAY     8
#define K9_RELAY     9
#define K10_RELAY    10
#define K11_RELAY    11
#define K12_RELAY    12
#define K13_RELAY    13
#define K14_RELAY    14
#define K15_RELAY    15
#define K16_RELAY    16
#define K17_RELAY    17


#ifdef __cplusplus
}
#endif


#endif





