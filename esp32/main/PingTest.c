#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/uart.h"
#include "esp_log.h"

#define UART_PORT UART_NUM_0
#define BUFFER_SIZE 128

static const char *TAG = "PI_COMM";

void app_main(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT
    };

    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));

    ESP_ERROR_CHECK(
        uart_driver_install(
            UART_PORT,
            BUFFER_SIZE * 2,
            0,
            0,
            NULL,
            0
        )
    );

    ESP_LOGI(TAG, "Waiting for Raspberry Pi...");

    uint8_t data[BUFFER_SIZE];

    while (1)
    {
        int length = uart_read_bytes(
            UART_PORT,
            data,
            BUFFER_SIZE - 1,
            pdMS_TO_TICKS(100)
        );

        if (length > 0)
        {
            data[length] = '\0';

            ESP_LOGI(TAG, "Received: %s", data);

            if (strstr((char *)data, "PING") != NULL)
            {
                uart_write_bytes(
                    UART_PORT,
                    "PONG\n",
                    5
                );

                ESP_LOGI(TAG, "Sent: PONG");
            }
        }
    }
}