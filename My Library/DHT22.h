/**
 ******************************************************************************
 * @file           : DHT22.h
 * @brief          : Header file for DHT22 temperature and humidity sensor.
 *                   Provides functions to initialize, read temperature and
 *                   humidity data from DHT22 sensor via single-wire interface.
 ******************************************************************************
 * @attention
 *
 * This module allows reading temperature and humidity data from DHT22 sensor
 * using a GPIO pin and hardware timer for precise timing. It supports data
 * validation through checksum and handles the sensor's communication protocol.
 *
 * Example usage:
 * @code
 *   DHT22_HandleTypeDef dht22;
 *   DHT22_Init(&dht22, &htim2, GPIOA, GPIO_PIN_0);
 *
 *   float temperature, humidity;
 *   if (DHT22_Read_Data(&dht22, &humidity, &temperature) == HAL_OK) {
 *       // Data read successfully
 *   }
 * @endcode
 *
 ******************************************************************************
 */

#ifndef _DHT22_H_
#define _DHT22_H_

#include "stm32f1xx_hal.h"

/* -------------------------------------------------------------------------- */
/*                            DHT22 Handle Struct                             */
/* -------------------------------------------------------------------------- */

/**
 * @brief  DHT22 handle structure definition
 */
typedef struct
{
    TIM_HandleTypeDef *htim; /*!< Pointer to timer handler for microsecond delays */
    GPIO_TypeDef *dataPort;  /*!< Pointer to GPIO port for data pin */
    uint16_t dataPin;        /*!< GPIO pin for data communication */
} DHT22_HandleTypeDef;

/* -------------------------------------------------------------------------- */
/*                            Function Prototypes                             */
/* -------------------------------------------------------------------------- */

/**
 * @brief  Initialize the DHT22 sensor.
 * @param  dht22x: Pointer to DHT22 handle structure.
 * @param  htim: Pointer to timer handler for precise timing.
 * @param  dataPort: Pointer to GPIO port for data pin.
 * @param  dataPin: GPIO pin number for data communication.
 * @retval HAL status
 */
HAL_StatusTypeDef DHT22_Init(DHT22_HandleTypeDef *dht22x, TIM_HandleTypeDef *htim,
                             GPIO_TypeDef *dataPort, uint16_t dataPin);

/**
 * @brief  Read temperature and humidity data from DHT22 sensor.
 * @param  dht22x: Pointer to DHT22 handle structure.
 * @param  humidity: Pointer to store humidity value (percentage).
 * @param  temperature: Pointer to store temperature value (Celsius).
 * @retval HAL status (HAL_OK if successful, HAL_ERROR if checksum fails or no response)
 */
HAL_StatusTypeDef DHT22_Read_Data(DHT22_HandleTypeDef *dht22x, float *humidity, float *temperature);

#endif /* _DHT22_H_ */