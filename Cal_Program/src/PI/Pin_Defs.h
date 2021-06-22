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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __arm__
    #define HIGH  1
    #define LOW 0
#endif

                            //BCM2835 pin#
#define LTC2057_SHDN            2   // physical pin 3
#define K1_SET_SIGNAL           4   // physical pin 7
#define K7_SET_SIGNAL           5   // physical pin 29
#define RES_ARRAY_RESET_SIGNAL  6   // physical pin 31
#define K15_SET_SIGNAL          7   // physical pin 26
#define BANK_A_RESET_SIGNAL     8   // physical pin 24
#define K5_SET_SIGNAL           9   // physical pin 21
#define K4_SET_SIGNAL           10  // physical pin 19
#define K6_SET_SIGNAL           11  // physical pin 23
#define K14_SET_SIGNAL          12  // physical pin 32
#define K8_SET_SIGNAL           13  // physical pin 33
#define UART_TX                 14  // physical pin 8
#define UART_RX                 15  // physical pin 10
#define K13_SET_SIGNAL          16  // physical pin 36
#define K1_RESET_SIGNAL         17  // physical pin 11
#define CAL_RESET_SIGNAL        18  // physical pin 12
#define K9_SET_SIGNAL           19  // physical pin 35
#define K12_SET_SIGNAL          20  // physical pin 38
#define K11_SET_SIGNAL          21  // physical pin 40
#define K3_SET_SIGNAL           22  // physical pin 15
#define K17_SET_SIGNAL          23  // physical pin 16
#define K16_SET_SIGNAL          24  // physical pin 18
#define BANK_B_RESET_SIGNAL     25  // physical pin 22
#define K10_SET_SIGNAL          26  // physical pin 37
#define K2_SET_SIGNAL           27  // physical pin 13


#define K1_RELAY_NUMBER     1
#define K2_RELAY_NUMBER     2
#define K3_RELAY_NUMBER     3
#define K4_RELAY_NUMBER     4
#define K5_RELAY_NUMBER     5
#define K6_RELAY_NUMBER     6
#define K7_RELAY_NUMBER     7
#define K8_RELAY_NUMBER     8
#define K9_RELAY_NUMBER     9
#define K10_RELAY_NUMBER    10
#define K11_RELAY_NUMBER    11
#define K12_RELAY_NUMBER    12
#define K13_RELAY_NUMBER    13
#define K14_RELAY_NUMBER    14
#define K15_RELAY_NUMBER    15
#define K16_RELAY_NUMBER    16
#define K17_RELAY_NUMBER    17


typedef enum RELAY_STATE{ RELAY_ON, RELAY_OFF }R_STATE;

typedef struct RELAY_STRUCTURE
{
    int bcm_set_pin;
    int bcm_reset_pin;
    int relay_number;
    int physical_set_pin;
    int physical_reset_pin;
    R_STATE state;

} RELAY;

RELAY K1;
RELAY K2;
RELAY K3;
RELAY K4;
RELAY K5;
RELAY K6;
RELAY K7;
RELAY K8;
RELAY K9;
RELAY K10;
RELAY K11;
RELAY K12;
RELAY K13;
RELAY K14;
RELAY K15;
RELAY K16;
RELAY K17;

#ifdef __cplusplus
}
#endif


#endif
