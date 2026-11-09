
idf_component_register(
    SRCS "PingTest.c" "motors.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES
        esp_driver_uart
        esp_driver_gpio
        esp_driver_ledc
)