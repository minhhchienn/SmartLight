#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <stdint.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>


#define BUTTON      16
#define BUTTON_BACK 16
#define BUTTON_NEXT 15

#define LED1        42
#define LED2        41

#define ESP_INTR_FLAG_DEFAULT 0
#define DEBOUNCE_TIME_MS 5

typedef void (*input_callback_t)(gpio_num_t gpio_num);

void button_init(void);
void input_set_callback(void *cb);

#endif