#include <stdio.h>
#include <webserver.h>
#include <lcd.h>
#include <GUI.h>
#include <freertos/FreeRTOS.h>
#include <webserver.h>
#include <esp_spiffs.h>
#include <button.h>
#include <driver/gpio.h>

#include <esp_log.h>


// Trạng thái của 2 đèn
volatile int led1_state = 0;
volatile int led2_state = 0;

void input_event_callback(int pin);

// Trạng thái kết nối wifi
volatile int wifiState = WIFI_DISCONNECTED;

const char* loadingString = "Loading...";

void GUI_Init();
void spiffs_init();
void LED_Init();
void LCD_Update();

void app_main(void)
{
    // Init SPIFFS
    spiffs_init();

    // Init LED
    LED_Init();

    // Init Button
    button_init();

    // Init GUI
    GUI_Init();
    
    //Wifi Init
    wifi_init();

    //Webserver Init
    webserver_init();

    // lấy hàm input_event_callback làm hàm xử lý ngắt
    input_set_callback(input_event_callback);

    while(1){
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void LED_Init(){
    gpio_reset_pin(LED1);
    gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
    gpio_set_level(LED1, led1_state);

    gpio_reset_pin(LED2);
    gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
    gpio_set_level(LED2, led2_state);
}

void Toggle_Led(uint8_t led){
    if (led == 1){
        led1_state = 1 - led1_state;
        gpio_set_level(LED1, led1_state);
        LCD_Update();
    } else if (led == 2){
        led2_state = 1 - led2_state;
        gpio_set_level(LED2, led2_state);
        LCD_Update();
    }
}

// Hàm xử lý ngắt (ISR -> tránh dùng delay)
void input_event_callback(int pin)
{
    if (pin == BUTTON_BACK) {
        Toggle_Led(1);
    }

    if (pin == BUTTON_NEXT) {
        Toggle_Led(2);
    }
}

void GUI_Init(){
    //LCD Init
    LCD_Init();
    LCD_Direction(3);
    LCD_Clear(BLACK);
    LCD_ShowCentredString(WHITE, BLACK, loadingString, 16, 1);
    vTaskDelay(1 / portTICK_PERIOD_MS);

    LCD_ShowImg(161,130);
    vTaskDelay(1 / portTICK_PERIOD_MS);

    LCD_DrawFillBox(20, 15, 120, 90, SKIN, 1);
    LCD_DrawFillBox(20, 15, 120, 90, WHITE, 0);
    LCD_DrawFillBox(25, 20, 110, 80, WHITE, 0);

    // Hiển thị trạng thái kết nối mạng
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Net: Connected");
    LCD_ShowString(30, 25, BLACK, 0xDE79, (uint8_t *)buffer, 14, 1);

    // Khởi động hiển thị trạng thái thiết bị
    snprintf(buffer, sizeof(buffer), "Device 1 is ");
    LCD_ShowString(30, 60, BLACK, 0xDE79, (uint8_t *)buffer, 15, 1);
    LCD_ShowString(110, 60, led1_state ? BLUE : RED, 0xDE79, (uint8_t *)(led1_state ? "ON" : "OFF"), 15, 0);

    snprintf(buffer, sizeof(buffer), "Device 2 is ");
    LCD_ShowString(30, 80, BLACK, 0xA732, (uint8_t *)buffer, 15, 1);
    LCD_ShowString(110, 80, led2_state ? BLUE : RED, 0xDE79, (uint8_t *)(led2_state ? "ON" : "OFF"), 15, 0);
}

void LCD_Update(){
    char buffer[64];
    // Hiển thị trạng thái thiết bị 1
    snprintf(buffer, sizeof(buffer), "%s", (led1_state) ? "ON " : "OFF");
    LCD_ShowString(110, 60, led1_state ? BLUE : RED, 0xDE79, (uint8_t *)buffer, 15, 0);

    // Hiển thị trạng thái thiết bị 2
    snprintf(buffer, sizeof(buffer), "%s", (led2_state) ? "ON " : "OFF");
    LCD_ShowString(110, 80, led2_state ? BLUE : RED, 0xA732, (uint8_t *)buffer, 15, 0);
}

void spiffs_init(){
    esp_err_t ret;

    // SPIFFS Init
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK){
        ESP_LOGE("SPIFFS", "Failed to mount or format SPIFFS");
    }
}