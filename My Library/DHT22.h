#ifndef _DHT22_H_
#define _DHT22_H_

#include "stm32f1xx_hal.h"

typedef struct
{
    /* DHT22 Pin Connection*/
    TIM_HandleTypeDef *htim;
    GPIO_TypeDef *dataPort;
    uint16_t dataPin;

    /*DHT22's measured value */
    float temperature;
    float humidity;
} DHT22_Typedef;

/**
 * @brief Initialize DHT22 sensor structure with hardware parameters
 * @param DHT22x: Pointer to DHT22 structure instance
 * @param htim: Pointer to timer handle for timing operations
 * @param dataPort: GPIO port for data pin
 * @param dataPin: GPIO pin for data communication
 */
void DHT22_Init(DHT22_Typedef *DHT22x, TIM_HandleTypeDef *htim, GPIO_TypeDef *dataPort, uint16_t dataPin);

/**
 * @brief Read temperature and humidity data from DHT22 sensor
 * @param DHT22x: Pointer to DHT22 structure instance
 * @return Operation status: 0 = success, 1 = error
 */
uint8_t DHT22_Read(DHT22_Typedef *DHT22x);

/**
 * @brief Send start signal to DHT22 sensor to initiate communication
 * @param DHT22x: Pointer to DHT22 structure instance
 */
void DHT22_Start(DHT22_Typedef *DHT22x);

/**
 * @brief Check sensor response after start signal
 * @param DHT22x: Pointer to DHT22 structure instance
 * @return Response status: 0 = valid response, 1 = no response or error
 */
uint8_t DHT22_CheckResponse(DHT22_Typedef *DHT22x);

/**
 * @brief Read single bit from DHT22 sensor
 * @param DHT22x: Pointer to DHT22 structure instance
 * @return Bit value: 0 or 1
 */
uint8_t DHT22_ReadBit(DHT22_Typedef *DHT22x);

/**
 * @brief Read one byte (8 bits) from DHT22 sensor
 * @param DHT22x: Pointer to DHT22 structure instance
 * @return Byte value read from sensor
 */
uint8_t DHT22_ReadByte(DHT22_Typedef *DHT22x);

#endif /* _DHT22_H_ */