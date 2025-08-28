#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "driver/gpio.h"
#include "esp_lcd_ili9341.h"
//#include "esp_lcd_st7735.h"  //ST7735 header file
#include "esp_lcd_panel_commands.h"
#include "esp_lcd_panel_dev.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "hal/gpio_types.h"
#include "hal/spi_types.h"

#include "hello.c"


#define LCD_HOST	    VSPI_HOST
#define PIN_NUM_SCLK    18
#define PIN_NUM_MOSI    23
#define PIN_NUM_MISO    19
#define PIN_NUM_LCD_CS  5
#define PIN_NUM_BKL     4
#define PIN_NUM_RST     22
#define PIN_NUM_LCD_DC  21


#define LCD_H_RES	240
#define LCD_V_RES	320

// ST7735
//#define LCD_H_RES	128
//#define LCD_V_RES	160

#define LCD_PIXEL_CLOCK_HZ	20*1000*1000
#define LCD_CMD_BITS	8
#define LCD_PARAM_BITS	8

esp_lcd_panel_io_handle_t io_handle = NULL;
esp_lcd_panel_handle_t lcd_panel_handle = NULL;

static void display_init (void)
{
	gpio_set_direction(PIN_NUM_BKL, GPIO_MODE_OUTPUT);
	gpio_set_level(PIN_NUM_BKL, 1);
	
	spi_bus_config_t buscfg = {
    .sclk_io_num = PIN_NUM_SCLK,
    .mosi_io_num = PIN_NUM_MOSI,
    .miso_io_num = PIN_NUM_MISO,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = LCD_H_RES * 80 * sizeof(uint16_t), // transfer 80 lines of pixels (assume pixel is RGB565) at most in one SPI transaction
};
ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO)); // Enable the DMA feature


esp_lcd_panel_io_spi_config_t io_config = {
    .dc_gpio_num = PIN_NUM_LCD_DC,
    .cs_gpio_num = PIN_NUM_LCD_CS,
    .pclk_hz = LCD_PIXEL_CLOCK_HZ,
    .lcd_cmd_bits = LCD_CMD_BITS,
    .lcd_param_bits = LCD_PARAM_BITS,
    .spi_mode = 0,
    .trans_queue_depth = 10,
};
// Attach the LCD to the SPI bus
ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));


esp_lcd_panel_dev_config_t panel_config = {
    .reset_gpio_num = PIN_NUM_RST,
    .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
    .bits_per_pixel = 16,
};
// Create LCD panel handle for ST7789, with the SPI IO device handle
ESP_ERROR_CHECK(esp_lcd_new_panel_ili9341(io_handle, &panel_config, &lcd_panel_handle));
//ESP_ERROR_CHECK(esp_lcd_new_panel_st7735(io_handle, &panel_config, &lcd_panel_handle));

esp_lcd_panel_reset(lcd_panel_handle);
esp_lcd_panel_init(lcd_panel_handle);
esp_lcd_panel_disp_on_off(lcd_panel_handle, true);
}

void app_main(void)
{
	display_init();
	esp_lcd_panel_swap_xy(lcd_panel_handle, true);
//	esp_lcd_panel_mirror(lcd_panel_handle, false, true);
	esp_lcd_panel_draw_bitmap(lcd_panel_handle, 0, 0, 100, 50, hello_map);
    while (true) {
        sleep(1);
    }
}
