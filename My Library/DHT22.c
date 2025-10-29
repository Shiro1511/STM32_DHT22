#include "DHT22.h"

void delayMicroseconds(TIM_HandleTypeDef *htim, uint16_t us)
{
    // htim->Instance->CNT = 0;
    __HAL_TIM_SET_COUNTER(htim, 0);
    HAL_TIM_Base_Start(htim);
    while (__HAL_TIM_GET_COUNTER(htim) < us)
        ;
    HAL_TIM_Base_Stop(htim);
}

uint8_t DHT22_CheckResponse(DHT22_Typedef *DHT22x)
{
    uint8_t response = 0;

    delayMicroseconds(DHT22x->htim, 40);
    if (!(HAL_GPIO_ReadPin(DHT22x->dataPort, DHT22x->dataPin)))
    {
        delayMicroseconds(DHT22x->htim, 80);
        if (HAL_GPIO_ReadPin(DHT22x->dataPort, DHT22x->dataPin))
        {
            response = 1;
        }
        else
        {
            response = 0;
        }
    }

    while (HAL_GPIO_ReadPin(DHT22x->dataPort, DHT22x->dataPin))
        ; // Wait for the pin to go low

    return response;
}

uint8_t DHT22_ReadBit(DHT22_Typedef *DHT22x)
{
    uint8_t bit_value = 0;

    while (!(HAL_GPIO_ReadPin(DHT22x->dataPort, DHT22x->dataPin)))
        ;

    delayMicroseconds(DHT22x->htim, 40);

    if (HAL_GPIO_ReadPin(DHT22x->dataPort, DHT22x->dataPin))
        bit_value = 1;
    else
        bit_value = 0;

    while (HAL_GPIO_ReadPin(DHT22x->dataPort, DHT22x->dataPin))
        ;

    return bit_value;
}

uint8_t DHT22_ReadByte(DHT22_Typedef *DHT22x)
{
    uint8_t byte = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        byte <<= 1;
        byte |= DHT22_ReadBit(DHT22x);
    }
    return byte;
}

uint8_t DHT22_Read(DHT22_Typedef *DHT22x)
{
    uint8_t data[5] = {0};
    uint8_t presence = 0;
    int16_t temperature, humidity;

    DHT22_Start(DHT22x);

    presence = DHT22_CheckResponse(DHT22x);

    if (presence)
    {
        data[0] = DHT22_ReadByte(DHT22x); // Humidity high byte
        data[1] = DHT22_ReadByte(DHT22x); // Humidity low byte
        data[2] = DHT22_ReadByte(DHT22x); // Temperature high byte
        data[3] = DHT22_ReadByte(DHT22x); // Temperature low byte
        data[4] = DHT22_ReadByte(DHT22x); // Checksum
    }

    if (data[4] == (uint8_t)(data[0] + data[1] + data[2] + data[3]))
    {
        humidity = (data[0] << 8) | data[1];
        temperature = (data[2] << 8) | data[3];

        if (temperature & 0x8000)
        {
            temperature = -(temperature & 0x7FFF);
        }

        DHT22x->humidity = humidity / 10.0f;
        DHT22x->temperature = temperature / 10.0f;

        return 1; // Success
    }

    DHT22x->humidity = 0.0f;
    DHT22x->temperature = 0.0f;

    return 0; // Failure
}

void DHT22_Start(DHT22_Typedef *DHT22x)
{
    /* Pin configuration is OUTPUT */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT22x->dataPin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT22x->dataPort, &GPIO_InitStruct);

    /* Pull the DATA pin down for at least 1ms */
    HAL_GPIO_WritePin(DHT22x->dataPort, DHT22x->dataPin, GPIO_PIN_RESET);
    delayMicroseconds(DHT22x->htim, 1500);

    /* Pull the DATA pin up for 30us */
    HAL_GPIO_WritePin(DHT22x->dataPort, DHT22x->dataPin, GPIO_PIN_SET);
    delayMicroseconds(DHT22x->htim, 30);

    /* Pin configuration is INPUT */
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DHT22x->dataPort, &GPIO_InitStruct);
}

void DHT22_Init(DHT22_Typedef *DHT22x, TIM_HandleTypeDef *htim, GPIO_TypeDef *dataPort, uint16_t dataPin)
{
    if (DHT22x == NULL)
        return;

    DHT22x->htim = htim;
    DHT22x->dataPort = dataPort;
    DHT22x->dataPin = dataPin;
    DHT22x->humidity = 0.0f;
    DHT22x->temperature = 0.0f;
}
