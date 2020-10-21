/*
 * SD.h
 *
 *  Created on: Jun 12, 2020
 *      Author: oku_d
 */

#ifndef INC_SDHANDLER_H_
#define INC_SDHANDLER_H_

#include "main.h"

typedef struct _SD_Card{
	uint32_t sectors;
	bool initialized;
	bool is_SDHC;
}SD_Card;

bool SDInitialize();

bool SDWriteMulti(uint32_t sector, uint8_t *data, uint16_t count);

bool SDReadMulti(uint32_t sector, uint8_t *data, uint16_t count);

bool SDWrite(uint32_t sector, uint8_t *data);

bool SDRead(uint32_t sector, uint8_t *data);

#endif /* INC_SDHANDLER_H_ */
