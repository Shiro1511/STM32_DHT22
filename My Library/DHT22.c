#include "DHT22.h"

static void delayMicroSeconds(DHT22_HandleTypeDef *dht22x, uint16_t us)
{
    __HAL_TIM_SET_COUNTER(dht22x->htim, 0);
    HAL_TIM_Base_Start(dht22x->htim);
    while (__HAL_TIM_GET_COUNTER(dht22x->htim) < us)
        ;
    HAL_TIM_Base_Stop(dht22x->htim);
}

static void delayMilliSeconds(DHT22_HandleTypeDef *dht22x, uint16_t ms)
{
    for (uint16_t i = 0; i < ms; i++)
    {
        delayMicroSeconds(dht22x, 1000);
    }
}

static void DHT22_Start(DHT22_HandleTypeDef *dht22x)
{
    /* First, Pin configuration is OUTPUT */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = dht22x->dataPin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(dht22x->dataPort, &GPIO_InitStruct);

    /* Pull the DATA pin down for at least 1 -> 10ms */
    HAL_GPIO_WritePin(dht22x->dataPort, dht22x->dataPin, GPIO_PIN_RESET);
    delayMilliSeconds(dht22x, 2);

    /* Pull the DATA pin up and wait 20 - 40us */
    HAL_GPIO_WritePin(dht22x->dataPort, dht22x->dataPin, GPIO_PIN_SET);
    delayMicroSeconds(dht22x, 30);

    /* Then, Pin configuration is INPUT*/
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(dht22x->dataPort, &GPIO_InitStruct);
}

static HAL_StatusTypeDef DHT22_CheckResponse(DHT22_HandleTypeDef *dht22x)
{
    /* Wait 40us after sending start signal */
    delayMicroSeconds(dht22x, 40);

    /* DHT22 should pull low for 80us */
    if (HAL_GPIO_ReadPin(dht22x->dataPort, dht22x->dataPin))
    {
        return HAL_ERROR; /* No response */
    }

    delayMicroSeconds(dht22x, 80);

    /* DHT22 should pull high for 80us */
    if (!HAL_GPIO_ReadPin(dht22x->dataPort, dht22x->dataPin))
    {
        return HAL_ERROR; /* Invalid response */
    }

    while (HAL_GPIO_ReadPin(dht22x->dataPort, dht22x->dataPin))
        ;

    return HAL_OK;
}

static uint8_t DHT22_ReadBit(DHT22_HandleTypeDef *dht22x)
{
    uint8_t bit_value = 0;

    while (!(HAL_GPIO_ReadPin(dht22x->dataPort, dht22x->dataPin)))
        ;

    delayMicroSeconds(dht22x, 50);

    if (HAL_GPIO_ReadPin(dht22x->dataPort, dht22x->dataPin))
    {
        bit_value = 1;
    }
    else
    {
        bit_value = 0;
    }

    while (HAL_GPIO_ReadPin(dht22x->dataPort, dht22x->dataPin))
        ;

    return bit_value;
}

static uint8_t DHT22_ReadByte(DHT22_HandleTypeDef *dht22x)
{
    uint8_t byte = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        byte <<= 1;
        byte |= DHT22_ReadBit(dht22x);
    }
    return byte;
}

HAL_StatusTypeDef DHT22_Read_Data(DHT22_HandleTypeDef *dht22x, float *humidity, float *temperature)
{
    uint8_t data[5] = {0};
    int16_t raw_humi, raw_temp;
    HAL_StatusTypeDef status;

    DHT22_Start(dht22x);

    status = DHT22_CheckResponse(dht22x);
    if (status != HAL_OK)
    {
        return status;
    }

    data[0] = DHT22_ReadByte(dht22x); /* Humidity high byte */
    data[1] = DHT22_ReadByte(dht22x); /* Humidity low byte */
    data[2] = DHT22_ReadByte(dht22x); /* Temperature high byte */
    data[3] = DHT22_ReadByte(dht22x); /* Temperature low byte */
    data[4] = DHT22_ReadByte(dht22x); /* Checksum */

    if (data[4] != (uint8_t)(data[0] + data[1] + data[2] + data[3]))
    {
        return HAL_ERROR;
    }

    raw_humi = (data[0] << 8) | data[1];
    raw_temp = (data[2] << 8) | data[3];

    if (raw_temp & 0x8000)
    {
        raw_temp = -(raw_temp & 0x7FFF);
    }

    *humidity = raw_humi / 10.0f;
    *temperature = raw_temp / 10.0f;

    return HAL_OK;
}

HAL_StatusTypeDef DHT22_Init(DHT22_HandleTypeDef *dht22x, TIM_HandleTypeDef *htim, GPIO_TypeDef *dataPort, uint16_t dataPin)
{
    if (dht22x == NULL || htim == NULL)
    {
        return HAL_ERROR;
    }

    dht22x->htim = htim;
    dht22x->dataPort = dataPort;
    dht22x->dataPin = dataPin;

    return HAL_OK;
}