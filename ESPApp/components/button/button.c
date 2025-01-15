#include "button.h"
#include <esp_log.h>

input_callback_t input_callback = NULL;
static QueueHandle_t button_event_queue;
static uint32_t last_interrupt_time = 0;

/// @brief xử lý khi gọi ngắt ngoài
/// @param arg 
static void IRAM_ATTR gpio_input_handler(void* arg) {
    int num = (uint32_t) arg;
    uint32_t interrupt_time = xTaskGetTickCountFromISR();

    // Kiểm tra thời gian từ lần ngắt cuối cùng và trạng thái nút
    if ((interrupt_time - last_interrupt_time) > (DEBOUNCE_TIME_MS / portTICK_PERIOD_MS)) {
        last_interrupt_time = interrupt_time;
        xQueueSendFromISR(button_event_queue, &num, NULL);
    }
}

// hàm tác vụ / task xử lý sự kiện nhấn nút
static void button_task(void* arg) {
    uint32_t num;
    while (1) {
        if (xQueueReceive(button_event_queue, &num, portMAX_DELAY)) {
            if (gpio_get_level(num) == 0){
                vTaskDelay(DEBOUNCE_TIME_MS / portTICK_PERIOD_MS);
                if (gpio_get_level(num) == 0){
                    switch (num){
                        case BUTTON_BACK:
                            ESP_LOGI("Task", "Button BACK triggered");
                            input_callback(num);
                            break;
                        
                        case BUTTON_NEXT:
                            ESP_LOGI("Task", "Button NEXT triggered");
                            input_callback(num);
                            break;

                        default:
                            break;
                    }
                }
            }
            
        }
    }
}

/**
 * @func	init
 * @brief	Khởi tạo các input, state, output, ngắt ngoài và xử lý ngắt
 * @param	None
 * @retval  None
*/ 
void button_init(void)
{   
    //setup cho Task
    button_event_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL);

    //setup nút nhấn
    gpio_reset_pin(BUTTON_BACK);
    gpio_reset_pin(BUTTON_NEXT);

    gpio_set_direction(BUTTON_BACK, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_BACK, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(BUTTON_BACK, GPIO_INTR_NEGEDGE);

    gpio_set_direction(BUTTON_NEXT, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_NEXT, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(BUTTON_NEXT, GPIO_INTR_NEGEDGE);

    // setup ngắt ngoài IRS
    gpio_install_isr_service(0);

    // gán ngắt cho nút nhấn
    gpio_isr_handler_add(BUTTON_NEXT, gpio_input_handler, (void*) BUTTON_NEXT);
    gpio_isr_handler_add(BUTTON_BACK, gpio_input_handler, (void*) BUTTON_BACK);
}

/// @brief Hàm định nghĩa hàm được gọi để xử lý ngắt
/// @param cb 
void input_set_callback(void *cb) {
    input_callback = cb;
}