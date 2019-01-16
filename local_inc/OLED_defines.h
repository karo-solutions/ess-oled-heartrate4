/*
 * defines for OLED_C Click
 * controller SEPS114A
 */

#ifndef OLED_DEFINES_H_
#define OLED_DEFINES_H_

/*
 * GPIO defines
 */
#define Display_Soft_Reset_LOW 0x00
#define Display_Soft_Reset_HIGH 0xFF

#define SYSCTL_PERIPH_CS SYSCTL_PERIPH_GPIOH
#define CS_PORT GPIO_PORTH_BASE
#define CS_PIN GPIO_PIN_2

#define SYSCTL_PERIPH_RST SYSCTL_PERIPH_GPIOC
#define RST_PORT GPIO_PORTC_BASE
#define RST_PIN GPIO_PIN_7

#define SYSCTL_PERIPH_DC SYSCTL_PERIPH_GPIOM
#define DC_PORT GPIO_PORTM_BASE
#define DC_PIN GPIO_PIN_3

#define SYSCTL_PERIPH_RW SYSCTL_PERIPH_GPIOE
#define RW_PORT GPIO_PORTE_BASE
#define RW_PIN GPIO_PIN_4

/*
 * value defines
 */
#define BLUE 0x01F
#define WHITE 0xFFFF
#define start_left 0
#define start_after_string 0x30
#define font_hight 0xC
#define font_width 0x08

#define row_TEMP 0x54
#define row_PULS 0x34
#define row_SPO2 0x14

#define disp_x_min 0x00
#define disp_x_max 0x5F
#define disp_y_min 0x00
#define disp_y_max 0x5F

/*
 * OLED Adress defines
 */
#define SOFT_RESET 0x01
#define DISPLAY_ON_OFF 0x02
#define ANALOG_CONTROL 0x0F
#define STANDBY_ON_OFF 0x14
#define OSC_ADJUST 0x1A
#define ROW_SCAN_DIRECTION 0x09
#define DISPLAY_X1 0x30
#define DISPLAY_X2 0x31
#define DISPLAY_Y1 0x32
#define DISPLAY_Y2 0x33
#define DISPLAYSTART_X 0x38
#define DISPLAYSTART_Y 0x39
#define CPU_IF 0x0D
#define MEM_X1 0x34
#define MEM_X2 0x35
#define MEM_Y1 0x36
#define MEM_Y2 0x37
#define MEMORY_WRITE_READ 0x1D
#define RAM_DATA_ACCESS_PORT 0x08
#define DISCHARGE_TIME 0x18
#define PEAK_PULSE_DELAY 0x16
#define PEAK_PULSE_WIDTH_R 0x3A
#define PEAK_PULSE_WIDTH_G 0x3B
#define PEAK_PULSE_WIDTH_B 0x3C
#define PRECHARGE_CURRENT_R 0x3D
#define PRECHARGE_CURRENT_G 0x3E
#define PRECHARGE_CURRENT_B 0x3F
#define COLUMN_CURRENT_R 0x40
#define COLUMN_CURRENT_G 0x41
#define COLUMN_CURRENT_B 0x42
#define ROW_OVERLAP 0x48
#define SCAN_OFF_LEVEL 0x49
#define ROW_SCAN_ON_OFF 0x17
#define ROW_SCAN_MODE 0x13
#define SCREEN_SAVER_CONTEROL 0xD0
#define SS_SLEEP_TIMER 0xD1
#define SCREEN_SAVER_MODE 0xD2
#define SS_UPDATE_TIMER 0xD3
#define RGB_IF 0xE0
#define RGB_POL 0xE1
#define DISPLAY_MODE_CONTROL 0xE5


#endif
