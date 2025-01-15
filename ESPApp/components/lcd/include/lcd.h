/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <stdint.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
/*! @brief LCD Pins */
#define LCD_SPI					SPI2_HOST
#define PARALLEL_LINES 16

#define LCD_CS_PIN				11
#define LCD_SPI_GPIO_SCK		10
#define LCD_SPI_GPIO_MOSI		9
#define LCD_RST_PIN 			18
#define LCD_AO_PIN 			    8
#define LCD_LED_PIN 			17

/*! @brief LCD_RST Pin */
#define LCD_SPI_RST_SET			gpio_set_level(LCD_RST_PIN, 1)
#define LCD_SPI_RST_RESET		gpio_set_level(LCD_RST_PIN, 0)

/*! @brief LCD_RS Pin */
#define LCD_SPI_RS_SET			gpio_set_level(LCD_AO_PIN, 1)
#define LCD_SPI_RS_RESET		gpio_set_level(LCD_AO_PIN, 0)

/*! @brief LCD_CS Pin */
#define LCD_SPI_CS_SET			gpio_set_level(LCD_CS_PIN, 1)
#define LCD_SPI_CS_RESET		gpio_set_level(LCD_CS_PIN, 0)

/*! @brief LCD_MOSI Pin */
#define LCD_SPI_MOSI_SET		gpio_set_level(LCD_SPI_GPIO_MOSI, 1)
#define LCD_SPI_MOSI_RESET		gpio_set_level(LCD_SPI_GPIO_MOSI, 0)

/*! @brief LCD_SCK Pin */
#define LCD_SPI_SCK_SET			gpio_set_level(LCD_SPI_GPIO_SCK, 1)
#define LCD_SPI_SCK_RESET		gpio_set_level(LCD_SPI_GPIO_SCK, 0)

#define USE_HORIZONTAL  		0

/*! @brief LCD pixels */
#define LCD_W 					130
#define LCD_H 					161

/*! @brief Color */
#define WHITE       			0xFFFF
#define BLACK      				0x0000
#define BLUE       				0x001F
#define RED         			0xF800
#define GREEN       			0x07E0
#define CYAN        			0x7FFF
#define YELLOW      			0xFFE0
#define BROWN 					0XBC40
#define SKIN 					0XFC07
#define GRAY  					0X8430

#define LCD_BASE        		((uint32_t)(0x60000000 | 0x0007FFFE))
#define LCD             		((LCD_TypeDef *) LCD_BASE)

extern spi_device_handle_t spi;

/*! @brief structure lcd */
typedef struct
{
	uint16_t width;
	uint16_t height;
	uint16_t id;
	uint16_t wramcmd;
	uint16_t setxcmd;
	uint16_t setycmd;
}_lcd_dev;

typedef struct
{
	uint16_t LCD_REG;
	uint16_t LCD_RAM;
} LCD_TypeDef;

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/
extern _lcd_dev lcddev;
extern uint16_t POINT_COLOR;
extern uint16_t BACK_COLOR;
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/**
 * @func	LCD_Init
 * @brief	Initialization LCD screen
 * @param	None
 * @retval	None
*/
void LCD_Init(void);



/**
 * @func	LCD_Clear
 * @brief	Full screen filled LCD screen
 * @param	color: Color of the screen (WHITE, BLACK, BLUE, MAGENTA, SKY,...)
 * @retval	None
*/
void LCD_Clear(uint16_t color);



/**
 * @func	LCD_Clear_DMA
 * @brief	Full screen filled LCD screen mode DMA
 * @param	color: Color of the screen (WHITE, BLACK, BLUE, MAGENTA, SKY,...)
 * @retval	None
*/
void LCD_Clear_DMA(uint16_t color);



/**
 * @func	LCD_Direction
 * @brief	Setting the display direction of LCD screen
 * @param	direction: (0: 0 degree
                       	1: 90 degree
					   	2: 180 degree
					   	3: 270 degree
 * @retval	None
*/
void LCD_Direction(uint8_t direction);

