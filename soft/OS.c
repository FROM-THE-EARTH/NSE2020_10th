#include "OS.h"

#include "MPU9250.h"
#include "NMEA.h"
#include "IM920.h"
#include "fatfs.h"

#include "string.h"
#include "stdio.h"

char str[100];

uint16_t year;
uint8_t month;
uint8_t date;
uint8_t hour;
uint8_t minute;
uint8_t second;

char N_S;
double lat;
char E_W;
double lon;
char Lat[15];
char Lon[15];

float Ax,Ay,Az;
float Gx,Gy,Gz;
char AX[8],AY[8],AZ[8];
char GX[8],GY[8],GZ[8];

uint32_t current_tick;
uint32_t last_tick_mpuread;
uint32_t last_tick_im920;
uint32_t last_tick_sdsync;

const uint32_t interval_tick_mpuread = 200;
const uint32_t interval_tick_im920 = 3000;
const uint32_t interval_tick_sdsync = 1000;

uint8_t Uart1RxData;
uint8_t Uart2RxData;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

static void ftos(float f, char* str){
	float _f;
	if(f < 0){
		*str++ = '-';
		_f = -f;
	}else{
		_f = f;
	}
	unsigned char val;
	unsigned int t = 1000000000;
	_f -= (unsigned int)_f / t * t;
	for(;t >= 10 && _f / t < 1;t /= 10);
	for(;t >= 1;t /= 10){
		val = _f / t;
		*str++ = val + '0';
		_f -= val * t;
	}
	*str++ = '.';
	for(t = 10;t <= 10000;t *= 10){
		val = _f * t;
		*str++ = val + '0';
		_f -= (float)val / t;
	}
	*str = '\0';
}

void LED_ON(uint8_t LEDNUM){
	switch(LEDNUM){
		case 1:HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,1);
			break;
		case 2:HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,1);
			break;
		case 3:HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,1);
			break;
	}
}

void LED_OFF(uint8_t LEDNUM){
	switch(LEDNUM){
		case 1:HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,0);
			break;
		case 2:HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,0);
			break;
		case 3:HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,0);
			break;
	}
}

void Initialize_IM920(){
	if(!IM920_Initialize()){
		while(1){HAL_GPIO_TogglePin(LED3_GPIO_Port,LED3_Pin);HAL_Delay(100);}
	}
	//IM920_SetRateMode(1);
}

void Initialize_MPU9250(){
	if(!MPU9250_Initialize(16,2000,16)){
		while(1){HAL_GPIO_TogglePin(LED3_GPIO_Port,LED3_Pin);HAL_Delay(200);}
	}
}

void Initialize_SD(){
	if(!file_open("DATA.CSV")){
		while(1){HAL_GPIO_TogglePin(LED3_GPIO_Port,LED3_Pin);HAL_Delay(30);}
	}
	sprintf(str,"h:m:s,systick,Accx,Accy,Accz,Gyrx,Gyry,Gyrz\r\n");
	file_write(str,strlen(str));
	file_sync();
}

void Initialize_Modules(){
	HAL_UART_Receive_IT(&huart1, &Uart1RxData, 1);
	HAL_UART_Receive_IT(&huart2, &Uart2RxData, 1);
	Initialize_IM920();
	Initialize_MPU9250();
	Initialize_SD();
}


void Process_GPS(){
	GPS_Get_UTC(&year,&month,&date,&hour,&minute,&second);
	GPS_Get_Position_DDD(&N_S,&lat,&E_W,&lon);
	ftos(lat,Lat);
	ftos(lon,Lon);
}

void Process_MPU9250(){
	MPU9250_ReadAccGyr(&Ax,&Ay,&Az,&Gx,&Gy,&Gz);
	ftos(Ax,AX);
	ftos(Ay,AY);
	ftos(Az,AZ);
	ftos(Gx,GX);
	ftos(Gy,GY);
	ftos(Gz,GZ);
	GPS_Get_UTC(&year,&month,&date,&hour,&minute,&second);
	sprintf(str,"%d:%d:%d,%d,%s,%s,%s,%s,%s,%s\r\n",hour,minute,second,(int)current_tick,AX,AY,AZ,GX,GY,GZ);
	file_write(str,strlen(str));
}

void Process_IM920(){
	if(IM920_NewMessage()){
		uint8_t data[100];
		IM920_Read(data);
		IM920_Send(data,(uint16_t)strlen((const char*)data));
	}

	Process_GPS();
	char str[100];
	sprintf(str,"%c %s %c %s",N_S,Lat,E_W,Lon);
	IM920_Send((uint8_t*)str,(uint16_t)strlen(str));
}

void Process_SD(){
	file_sync();
}

void OsStart(){

	Initialize_Modules();

	while(1){
		current_tick = HAL_GetTick();
		if(current_tick - last_tick_mpuread > interval_tick_mpuread){
			last_tick_mpuread = current_tick;
			LED_ON(2);
			Process_MPU9250();
			LED_OFF(2);
		}

		current_tick = HAL_GetTick();
		if(current_tick - last_tick_im920 > interval_tick_im920){
			last_tick_im920 = current_tick;
			Process_IM920();
		}

		current_tick = HAL_GetTick();
		if(current_tick - last_tick_sdsync > interval_tick_sdsync){
			last_tick_sdsync = current_tick;
			LED_ON(3);
			Process_SD();
			LED_OFF(3);
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_15,1);
	if(huart == &huart1){
		HAL_UART_Receive_IT(&huart1, &Uart1RxData, 1);
		IM920_UART_Receive(Uart1RxData);
	}else if(huart == &huart2){
		HAL_UART_Receive_IT(&huart2, &Uart2RxData, 1);
		GPS_UART_Receive(Uart2RxData);
	}
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_15,0);
}
