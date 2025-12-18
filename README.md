# LED Display on WS2812B
Project implements WS2812B LED matrix controller based on the STM32F302R8T7 microcontroller.
LEDs are controlled using PWM with DMA for faster transmission. Projects includes BME280 sensor, auto brightness with ADC and BTE communication via an HC-05 module. Using HAL library.

# Pinout

Timer1 pwm output:
- PC0
BME280 I2C:
- SDA PB9
- SCL PB8
ADC channel1: PA0
HC-05 USART:
- RX PC11
- TX PC10
