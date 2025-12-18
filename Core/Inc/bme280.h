/*
 * bme280.h
 *
 *  Created on: Jan 11, 2024
 *      Author: Ventu
 */

#ifndef INC_BME280_H_
#define INC_BME280_H_

#include "main.h"
//#include "i2c.h"

/******************************| PRE-PROCESSOR SECTION |******************************/
#define BME280_REG_HUM_LSB     0xFE
#define BME280_REG_HUM_MSB     0xFD
#define BME280_REG_TEMP_XLSB   0xFC // bits: 7-4
#define BME280_REG_TEMP_LSB    0xFB
#define BME280_REG_TEMP_MSB    0xFA
#define BME280_REG_TEMP        (BMP280_REG_TEMP_MSB)
#define BME280_REG_PRESS_XLSB  0xF9 // bits: 7-4
#define BME280_REG_PRESS_LSB   0xF8
#define BME280_REG_PRESS_MSB   0xF7
#define BME280_REG_PRESSURE    (BMP280_REG_PRESS_MSB)
#define BME280_REG_CONFIG      0xF5 // bits: 7-5 t_sb; 4-2 filter; 0 spi3w_en
#define BME280_REG_CTRL_MEAS   0xF4 // bits: 7-5 osrs_t; 4-2 osrs_p; 1-0 mode
#define BME280_REG_STATUS      0xF3 // bits: 3 measuring; 0 im_update
#define BME280_REG_CTRL_HUM    0xF2 // bits: 2-0 osrs_h
#define BME280_REG_RESET       0xE0
#define BME280_REG_ID          0xD0
#define BME280_CHIP_ID		   0x60
//#define BME280_REG_CALIB       0xA1 ... 0x88

#define BME280_I2C_ADDRESS_0  0x76  // SDO connected to GND	'0'
#define BME280_I2C_ADDRESS_1  0x77	// SDO connected to VDDIO '1'

#define BME280_RESET_VALUE	0xB6
/******************************| VARIABLE SECTION |******************************/
typedef enum{
	BME280_OK = 0,
	BME280_ERROR = 1,
	BME280_READ_ERROR = 2,
	BME280_WRITE_ERROR = 3,
	BME280_READ_CALIB_ERROR = 4,
	BME280_WRONG_CHIPID_ERROR = 5,
	BME280_CONFIG_ERROR = 6,
	BME280_CTRL_MEAS_ERROR = 7,
	BME280_CTRL_HUM_ERROR = 8,

}BME280_Status;

typedef enum{
	BME280_MODE_SLEEP = 0,
	BME280_MODE_FORCED = 1,
	BME280_MODE_NORMAL = 3
}BME280_Mode;

typedef enum{
	BME280_FILTER_OFF = 0,
	BME280_FILTER_2 = 1,
	BME280_FILTER_4 = 2,
	BME280_FILTER_8 = 3,
	BME280_FILTER_16 = 4
}BME280_Filter;

typedef enum{
	BME280_SKIPPED = 0,
	BME280_ULTRA_LOW_POWER = 1,
	BME280_LOW_POWER = 2,
	BME280_STANDARD = 3,
	BME280_HIGH_RES = 4,
	BME280_ULTRA_HIGH_RES = 5
}BME280_Oversampling;

typedef enum{
	BME280_STANDBY_05 = 0,
	BME280_STANDBY_62 = 1,
	BME280_STANDBY_125 = 2,
	BME280_STANDBY_250 = 3,
	BME280_STANDBY_500 = 4,
	BME280_STANDBY_1000 = 5,
	BME280_STANDBY_2000 = 6,
	BME280_STANDBY_4000 = 7
}BME280_StandbyTime;

typedef struct{
	BME280_Mode mode;
	BME280_Filter filter;
	BME280_Oversampling pressure_oversampling;
	BME280_Oversampling temperature_oversampling;
	BME280_Oversampling humidity_oversampling;
	BME280_StandbyTime standby_time;
}BME280_param_t;

typedef struct{

	uint16_t dig_T1;
	int16_t  dig_T2;
	int16_t  dig_T3;
	uint16_t dig_P1;
	int16_t  dig_P2;
	int16_t  dig_P3;
	int16_t  dig_P4;
	int16_t  dig_P5;
	int16_t  dig_P6;
	int16_t  dig_P7;
	int16_t  dig_P8;
	int16_t  dig_P9;
	uint8_t	 dig_H1;
	int16_t	 dig_H2;
	uint8_t  dig_H3;
	int16_t  dig_H4;
	int16_t  dig_H5;
	int8_t	 dig_H6;

//	int32_t temperature;
//	uint32_t pressure;
	int32_t t_fine;

	uint8_t addr;

	I2C_HandleTypeDef * i2c;

	BME280_param_t param;

	uint8_t  id;        /* Chip ID */

}BME280_HandleTypedef;

BME280_Status BME280_Init(BME280_HandleTypedef *sBME280, I2C_HandleTypeDef * I2C, uint8_t Address);
BME280_Status BME280_Read_Double(BME280_HandleTypedef *sBME280, float *temp, float *press, float *hum);
BME280_Status BME280_Read_Fixed(BME280_HandleTypedef * sBME280, int32_t * temperature, uint32_t *pressure);
BME280_Status BME280_Read_Temp(BME280_HandleTypedef *sBME280, int32_t * temperature);


#endif /* INC_BME280_H_ */
