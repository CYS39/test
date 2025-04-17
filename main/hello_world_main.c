#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"

#define GPIO_OUTPUT_IO_0    4
#define GPIO_OUTPUT_IO_1    27
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1))

#define GPIO_INPUT_IO_0     5
#define GPIO_INPUT_IO_1     21
#define GPIO_INPUT_IO_2     23
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1) | (1ULL<<GPIO_INPUT_IO_2))

int key1 = 0,key2,key3;
int delay_num = 100;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    if(gpio_get_level(GPIO_INPUT_IO_0)==1){
        key1 = 1;
    }
    if(gpio_get_level(GPIO_INPUT_IO_1)==1){
        key2 = 1;
    }
    if(gpio_get_level(GPIO_INPUT_IO_2)==1){
        key3++;
    }
}

void gpio_init(void){

    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 0;
    io_conf.pull_down_en = 1;
    gpio_config(&io_conf);

    //change gpio interrupt type for one pin
    // gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_POSEDGE);
    // gpio_set_intr_type(GPIO_INPUT_IO_1, GPIO_INTR_POSEDGE);
    // gpio_set_intr_type(GPIO_INPUT_IO_2, GPIO_INTR_POSEDGE);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_EDGE);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, NULL);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, NULL);
    gpio_isr_handler_add(GPIO_INPUT_IO_2, gpio_isr_handler, NULL);
    gpio_intr_enable(GPIO_INPUT_IO_0);
    gpio_intr_enable(GPIO_INPUT_IO_1);
    gpio_intr_enable(GPIO_INPUT_IO_2);

}

void app_main(void)
{
    int cnt = 0;
    printf("Hello world!\n");
    gpio_init();

    while(1){

        cnt++;
        if(key3 % 2){
            gpio_set_level(GPIO_OUTPUT_IO_0, cnt % 2);
        }
        else{
            gpio_set_level(GPIO_OUTPUT_IO_1, cnt % 2);
        }

        if(key1){
            key1 = 0;
            delay_num += 100;
        }
        if(key2){
            key2 = 0;
            delay_num -= 100;
            if(delay_num < 100) delay_num = 100;
        }
        vTaskDelay(delay_num / portTICK_PERIOD_MS);

    }

}
