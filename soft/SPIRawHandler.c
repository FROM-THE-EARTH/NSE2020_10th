#include "SPIRawHandler.h"

#define STM32
#ifdef STM32

//#define DMA_ENABLE
//#define DEBUG_PRINT

#ifdef DEBUG_PRINT
int last_print = 0,r = 1,w = 2,rs = 3, ws = 4;
#endif

#include "main.h"
extern SPI_HandleTypeDef hspi2;

void SpiInitialize(){
	SpiDeAsertSS();
}

void SpiRawWrite(uint8_t data){
#ifdef DEBUG_PRINT
	if(last_print != w)printf("\nWRITE :");
	printf(" %x",data);
	last_print = w;
#endif
#ifdef DMA_ENABLE
	HAL_SPI_Transmit_DMA(&hspi2, &data, 1);
	while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY);
#else
	HAL_SPI_Transmit(&hspi2, &data, 1, 100);
#endif
}
uint8_t SpiRawRead(){
	uint8_t data,dummy = 0xFF;
#ifdef DMA_ENABLE
	HAL_SPI_TransmitReceive_DMA(&hspi2, &dummy, &data, 1);
	while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY);
#else
	HAL_SPI_TransmitReceive(&hspi2, &dummy, &data, 1, 100);
#endif

#ifdef DEBUG_PRINT
	if(last_print != r)printf("\nREAD :");
	printf(" %x",data);
	last_print = r;
#endif

	return data;
}
void SpiRawWriteMulti(uint8_t *data,uint16_t count){

#ifdef DEBUG_PRINT
	if(last_print != ws)printf("\nWRITES :");
	for(int i = 0;i < count;i++)printf(" %x",data[i]);
	last_print = ws;
#endif

#ifdef DMA_ENABLE
	HAL_SPI_Transmit_DMA(&hspi2, data, count);
	while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY);
#else
	HAL_SPI_Transmit(&hspi2, data, count, 100);
#endif
}
void SpiRawReadMulti(uint8_t *data,uint16_t count){
#ifdef DMA_ENABLE
	HAL_SPI_Receive_DMA(&hspi2, data, count);
	while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY);
#else
	HAL_SPI_Receive(&hspi2, data, count,100);
#endif

#ifdef DEBUG_PRINT
	if(last_print != rs)printf("\nREADS :");
	for(int i = 0;i < count;i++)printf(" %x",data[i]);
	last_print = rs;
#endif
}

void SpiAsertSS(){
	for(uint8_t i = 0;i < 1;)if(SpiRawRead() == 0xFF)i++;
	HAL_GPIO_WritePin(SDSS_GPIO_Port,SDSS_Pin,0);

#ifdef DEBUG_PRINT
	printf("\n------ASSERT------");
#endif

	for(uint8_t i = 0;i < 1;)if(SpiRawRead() == 0xFF)i++;
}

void SpiDeAsertSS(){
	for(uint8_t i = 0;i < 1;)if(SpiRawRead() == 0xFF)i++;
	HAL_GPIO_WritePin(SDSS_GPIO_Port,SDSS_Pin,1);

#ifdef DEBUG_PRINT
	printf("\n------DEASSERT------");
#endif

	for(uint8_t i = 0;i < 1;)if(SpiRawRead() == 0xFF)i++;
}

#endif
