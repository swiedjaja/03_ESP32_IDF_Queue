#include <stdio.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/gpio.h>
#include "device.h"
#include "gpio_arduino.h"

QueueHandle_t queue;

static void taskProducer(void* arg)
{
    int nValue = 0;
    while (1)
    {
        gpio_set_level(LED_RED, LED_ON);
        xQueueSend(queue, &nValue, portMAX_DELAY);
        printf("Produce: %d\n", nValue);
        fflush(stdout);
        nValue++;
        vTaskDelay(10/portTICK_RATE_MS);
        gpio_set_level(LED_RED, LED_OFF);
        vTaskDelay(1000/portTICK_RATE_MS);
    }
}

static void taskConsumer(void* arg)
{
    while (1)
    {
        int nValue;
        xQueueReceive(queue, &nValue, portMAX_DELAY);
        gpio_set_level(LED_GREEN, LED_ON);        
        vTaskDelay(100/portTICK_RATE_MS);
        printf("Consumer receive: %d\n", nValue);
        fflush(stdout);
        gpio_set_level(LED_GREEN, LED_OFF);
        vTaskDelay(3000/portTICK_RATE_MS);
    }
}

void app_main() {
    gpio_pinMode(LED_BUILTIN, OUTPUT);
    gpio_pinMode(LED_GREEN, OUTPUT);
    gpio_pinMode(LED_RED, OUTPUT);

    queue = xQueueCreate(3, sizeof(int));
    xTaskCreate(taskProducer, "taskProducer", configMINIMAL_STACK_SIZE+1024, NULL, 5, NULL);
    xTaskCreate(taskConsumer, "taskConsumer", configMINIMAL_STACK_SIZE+1024, NULL, 5, NULL);
    while (1)
    {
        gpio_set_level(LED_BUILTIN, LED_BUILTIN_ON);
        vTaskDelay(100/portTICK_RATE_MS);
        gpio_set_level(LED_BUILTIN, LED_BUILTIN_OFF);
        vTaskDelay(1000/portTICK_RATE_MS);
    }
}