#include "ILI9488.h"
#include <stdio.h>


extern SPI_HandleTypeDef hspi2;
#define LCD_CLEAR_SEND_NUMBER (ILI9488_HEIGHT * ILI9488_WIDTH * 3)
static uint32_t BACK_COLOR = ILI9488_POINT_COLOR_WHITE, FORE_COLOR = ILI9488_POINT_COLOR_BLACK;


/* Private user code ---------------------------------------------------------*/
static HAL_StatusTypeDef ili9488_send_cmd(uint8_t cmd)
{
	ILI9488_PIN_DC_LOW();
	
	return HAL_SPI_Transmit(&hspi2, &cmd, 1, 1000);
}

static HAL_StatusTypeDef ili9488_send_bytes(uint8_t *buf, uint16_t size)
{
	ILI9488_PIN_DC_HIGH();

	return HAL_SPI_Transmit(&hspi2, buf, size, 1000);
}

static HAL_StatusTypeDef ili9488_send_byte(uint8_t byte)
{
	ILI9488_PIN_DC_HIGH();
	
	return HAL_SPI_Transmit(&hspi2, &byte, 1, 1000);
}

static HAL_StatusTypeDef ili9488_send_three_bytes(uint32_t three_bytes)
{
	uint8_t data[3] = {0};
	
	data[0] = (uint8_t)(three_bytes >> 16);
	data[1] = (uint8_t)(three_bytes >> 8);
	data[2] = (uint8_t)(three_bytes);
	
	ILI9488_PIN_DC_HIGH();
	
	return HAL_SPI_Transmit(&hspi2, data, 3, 1000);
}

static HAL_StatusTypeDef ili9488_read_byets(uint8_t *buf, size_t size)
{
	ILI9488_PIN_DC_HIGH();
		
	return HAL_SPI_Receive(&hspi2, buf, size, 1000);
}

void lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    ili9488_send_cmd(0x2a);
    ili9488_send_byte(x1 >> 8);
    ili9488_send_byte(x1);
    ili9488_send_byte(x2 >> 8);
    ili9488_send_byte(x2);

    ili9488_send_cmd(0x2b);
    ili9488_send_byte(y1 >> 8);
    ili9488_send_byte(y1);
    ili9488_send_byte(y2 >> 8);
    ili9488_send_byte(y2);

    ili9488_send_cmd(0x2C);
}

void ili9488_init(void)
{
	ILI9488_PIN_BL_LOW();
	ILI9488_PIN_RESET_HIGH();
	ili9488_delay(10);
	ILI9488_PIN_RESET_LOW();
	ili9488_delay(50);
	ILI9488_PIN_RESET_HIGH();
	ili9488_delay(200);
		
	ili9488_send_cmd(ILI9488_CMD_POSITIVE_GAMMA_CONTROL);
	ili9488_send_byte(0x00);
    ili9488_send_byte(0x07);
    ili9488_send_byte(0x0F);
    ili9488_send_byte(0x0D);
    ili9488_send_byte(0x1B);
    ili9488_send_byte(0x0A);
    ili9488_send_byte(0x3C);
    ili9488_send_byte(0x78);
    ili9488_send_byte(0x4A);
    ili9488_send_byte(0x07);
    ili9488_send_byte(0x0E);
    ili9488_send_byte(0x09);
    ili9488_send_byte(0x1B);
    ili9488_send_byte(0x1e);
    ili9488_send_byte(0x1F);

    ili9488_send_cmd(ILI9488_CMD_NEGATIVE_GAMMA_CONTROL);
    ili9488_send_byte(0x00);
    ili9488_send_byte(0x22);
    ili9488_send_byte(0x24);
    ili9488_send_byte(0x06);
    ili9488_send_byte(0x12);
    ili9488_send_byte(0x07);
    ili9488_send_byte(0x36);
    ili9488_send_byte(0x47);
    ili9488_send_byte(0x47);
    ili9488_send_byte(0x06);
    ili9488_send_byte(0x0a);
    ili9488_send_byte(0x07);
    ili9488_send_byte(0x30);
    ili9488_send_byte(0x37);
    ili9488_send_byte(0x0f);

    ili9488_send_cmd(ILI9488_CMD_POWER_CONTROL1);
    ili9488_send_byte(0x10);
    ili9488_send_byte(0x10);

    ili9488_send_cmd(ILI9488_CMD_POWER_CONTROL2);
    ili9488_send_byte(0x41);

    ili9488_send_cmd(ILI9488_CMD_VCOM_CONTROL1);
    ili9488_send_byte(0x00);
    ili9488_send_byte(0x22);
    ili9488_send_byte(0x80);

    ili9488_send_cmd(ILI9488_CMD_MEMORY_ACCESS_CONTROL);
    ili9488_send_byte(0x48);

    ili9488_send_cmd(ILI9488_CMD_INTERFACE_PIXEL_FORMAT);
    ili9488_send_byte(0x66);

    ili9488_send_cmd(ILI9488_CMD_INTERFACE_MODE_CONTROL);
    ili9488_send_byte(0x00);
	
    ili9488_send_cmd(ILI9488_CMD_FRAME_RATE_CONTROL_NORMAL);
    ili9488_send_byte(0xB0);
    ili9488_send_byte(0x11);
	
    ili9488_send_cmd(ILI9488_CMD_DISPLAY_INVERSION_CONTROL);
    ili9488_send_byte(0x02);
	
    ili9488_send_cmd(ILI9488_CMD_DISPLAY_FUNCTION_CONTROL);
    ili9488_send_byte(0x02);
    ili9488_send_byte(0x02);

    ili9488_send_cmd(ILI9488_CMD_ENTRY_MODE_SET);
    ili9488_send_byte(0xC6);

    ili9488_send_cmd(ILI9488_CMD_SET_IMAGE_FUNCTION);
    ili9488_send_byte(0x00);

    ili9488_send_cmd(ILI9488_CMD_ADJUST_CONTROL3);
    ili9488_send_byte(0xA9);
    ili9488_send_byte(0x51);
    ili9488_send_byte(0x2C);
    ili9488_send_byte(0x82);

    ili9488_send_cmd(ILI9488_CMD_SLEEP_OUT);
    ili9488_delay(120);
    ili9488_send_cmd(ILI9488_CMD_DISPLAY_ON);
    ili9488_delay(50);
}

uint32_t ili9488_read_display_id_info(void)
{
	uint32_t display_id_info = 0x00;
	uint8_t buf[4] = {0};
	
	ili9488_send_cmd(ILI9488_CMD_READ_DISPLAY_ID_INFO);
	ili9488_read_byets(buf, 4);
	
	for(int i = 0; i < 4; ++i)	printf("display_id_info buf[%d]: %x\r\n", i, buf[i]);
	
	return display_id_info;
}

uint32_t ili9488_read_display_status(void)
{
	uint8_t display_status[5] = {0};
	
	ili9488_send_cmd(ILI9488_CMD_READ_DISPLAY_STATUS);
	ili9488_read_byets(display_status, 5);
	
	for(int i = 0; i < 5; ++i)	printf("buf[%d]: %x\r\n", i, display_status[i]);
	
	return 0;
}

void ili9488_set_color(uint32_t back, uint32_t fore)
{
    BACK_COLOR = back;
    FORE_COLOR = fore;
}

void ili9488_enter_sleep(void)
{
	ILI9488_PIN_BL_LOW();
	ili9488_delay(5);
	ili9488_send_cmd(ILI9488_CMD_DISPLAY_OFF);
	ili9488_delay(10);
	ili9488_send_cmd(ILI9488_CMD_SLEEP_IN);
	ili9488_delay(120);
}

void ili9488_exit_sleep(void)
{
	ILI9488_PIN_BL_HIGH();
	ili9488_delay(5);
	ili9488_send_cmd(ILI9488_CMD_SLEEP_OUT);
	ili9488_delay(120);
	ili9488_send_cmd(ILI9488_CMD_DISPLAY_ON);
	ili9488_delay(10);
}

void ili9488_set_address(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    ili9488_send_cmd(ILI9488_CMD_COLUMN_ADDR_SET);
    ili9488_send_byte(x1 >> 8);
    ili9488_send_byte(x1);
    ili9488_send_byte(x2 >> 8);
    ili9488_send_byte(x2);

    ili9488_send_cmd(ILI9488_CMD_PAGE_ADDR_SET);
    ili9488_send_byte(y1 >> 8);
    ili9488_send_byte(y1);
    ili9488_send_byte(y2 >> 8);
    ili9488_send_byte(y2);

    ili9488_send_cmd(ILI9488_CMD_MEMORY_WRITE);
}

void ili9488_clear_lcd(uint32_t color)
{
    uint32_t i, j;
    uint8_t data[3] = {0};
    uint8_t *buf = NULL;

    data[0] = color >> 16;
    data[1] = color >> 8;
    data[2] = color;
    ili9488_set_address(0, 0, ILI9488_WIDTH - 1, ILI9488_HEIGHT - 1);

	for (i = 0; i < ILI9488_WIDTH; i++)
	{
		for (j = 0; j < ILI9488_HEIGHT; j++)
		{
			ili9488_send_three_bytes(color);
		}
	}
}

void ili9488_draw_point_color(uint16_t x, uint16_t y, uint32_t color)
{
	ili9488_set_address(x, y, x, y);
	ili9488_send_three_bytes(color);
}

void ili9488_draw_point(uint16_t x, uint16_t y)
{
	ili9488_draw_point_color(x, y, FORE_COLOR);
}

void ili9488_fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint32_t color)
{
	uint32_t i = 0, j = 0;
    uint32_t size = 0, size_remain = 0;
	
	size = (x_end - x_start) * (y_end - y_start) * 3;
	if(size > LCD_CLEAR_SEND_NUMBER)
	{
        size_remain = size - LCD_CLEAR_SEND_NUMBER;
        size = LCD_CLEAR_SEND_NUMBER;
	}
	
	ili9488_set_address(x_start, y_start, x_end, y_end);

	for (i = y_start; i <= y_end; i++)
	{
		for (j = x_start; j <= x_end; j++)	ili9488_send_three_bytes(color);
	}
}

