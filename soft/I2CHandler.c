#include "I2CHandler.h"

extern I2C_HandleTypeDef hi2c1;

bool initialized = false;

void I2cInitialize(){
	if(!initialized){
		initialized = true;
	}
}

void I2cWriteByte(uint8_t add, uint8_t reg, uint8_t data)
{
	HAL_I2C_Mem_Write(&hi2c1, add << 1, reg, 1, &data, 1, 100);
}

uint8_t I2cReadByte(uint8_t add, uint8_t reg)
{
	uint8_t data;
	HAL_I2C_Mem_Read(&hi2c1, add << 1, reg, 1, &data, 1, 100);
	return data;
}

void I2cReadBytes(uint8_t add, uint8_t reg, uint8_t *data, uint8_t count)
{
	HAL_I2C_Mem_Read(&hi2c1, add << 1, reg, 1, data, count, 100);
	//HAL_I2C_Mem_Read_DMA(&hi2c1, add << 1, reg, 1, data, count);
	//HAL_I2C_Master_Transmit_DMA(&hi2c1, add << 1, &reg, 1);
	//HAL_I2C_Master_Receive_DMA(&hi2c1, add << 1, data, count);
	//HAL_I2C_Master_Transmit(&hi2c1, add << 1, &reg, 1, 100);
	//HAL_I2C_Master_Receive(&hi2c1, add << 1, data, count, 100);
}
