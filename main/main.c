#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "driver/ledc.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
// #include "ledc_types.h"

#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_FREQUENCY          (1000) // Frequency in Hertz. Set frequency at 5 kHz
#define DEFAULT_SCAN_LIST_SIZE 10

static const char *TAG = "scan";

void ledBreath()
{
    ledc_timer_config_t ledc_timer = {
    .speed_mode       = LEDC_MODE,
    .timer_num        = 0,
    .duty_resolution  = LEDC_DUTY_RES,
    .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
    .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    for (int i = 0; i < 3; i++)
    {
        ledc_channel_config_t ledc_channel3 = {
        .gpio_num       = 3,
        .speed_mode     = LEDC_MODE,
        .channel        = 3,
        .timer_sel      = 0,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
        };
        ledc_channel_config(&ledc_channel3);

        ledc_channel_config_t ledc_channel4 = {
        .gpio_num       = 4,
        .speed_mode     = LEDC_MODE,
        .channel        = 4,
        .timer_sel      = 0,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
        };
        ledc_channel_config(&ledc_channel4);

        ledc_channel_config_t ledc_channel5 = {
        .gpio_num       = 5,
        .speed_mode     = LEDC_MODE,
        .channel        = 5,
        .timer_sel      = 0,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
        };
        ledc_channel_config(&ledc_channel5);
    }
    while (1)
    {
        for (int intDuty = 0; intDuty < 4000; intDuty+=100)
        {
            for (int i = 0; i < 3; i++)
            {
                ledc_set_duty(LEDC_LOW_SPEED_MODE, i+3, intDuty);
                ledc_update_duty(LEDC_MODE, i+3);
            }
            vTaskDelay(2);
        }
        for (int intDuty = 4000; intDuty >0; intDuty-=100)
        {
            for (int i = 0; i < 3; i++)
            {
                ledc_set_duty(LEDC_LOW_SPEED_MODE, i+3, intDuty);
                ledc_update_duty(LEDC_MODE, i+3);
            }
            vTaskDelay(2);
        }
    }
}

void ledLight()
{
    for (int i = 18; i < 19; i++)
    {
        gpio_reset_pin(i);
        gpio_set_direction(i,GPIO_MODE_OUTPUT);
        gpio_set_level(i,1);
        vTaskDelay(500);
    }
}

void get_wifi()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    uint16_t number = DEFAULT_SCAN_LIST_SIZE;
    wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
    uint16_t ap_count;

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_wifi_scan_start(NULL, true);
    esp_wifi_scan_get_ap_num(&ap_count);
    esp_wifi_scan_get_ap_records(&number, ap_info);
    ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);
    for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++) {
        ESP_LOGI(TAG, "SSID \t\t%s", ap_info[i].ssid);
        ESP_LOGI(TAG, "RSSI \t\t%d", ap_info[i].rssi);
        ESP_LOGI(TAG, "Channel \t\t%d\n", ap_info[i].primary);
    }
}

void app_main(void)
{

    get_wifi();
}

