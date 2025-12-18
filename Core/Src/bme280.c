/*
 * bme280.c
 *
 *  Created on: Jan 11, 2024
 *      Author: Ventu
 */

#include "bme280.h"

static BME280_Status BME280_Write8(BME280_HandleTypedef *sBME280, uint8_t Address, uint8_t Data)
{

	if(HAL_I2C_Mem_Write(sBME280->i2c, (sBME280->addr << 1), Address, 1, &Data, 1, 1000) == HAL_OK)
	{
		return BME280_OK;
	}
	else return BME280_WRITE_ERROR;

}

static BME280_Status BME280_Read8(BME280_HandleTypedef *sBME280, uint8_t Register, uint8_t *Data)
{

	if(HAL_I2C_Mem_Read(sBME280->i2c, ((sBME280->addr)<<1), Register, 1, Data, 1, 1000) == HAL_OK)
	{
		return BME280_OK;
	}
	else return BME280_READ_ERROR;

}

static BME280_Status BME280_Read16(BME280_HandleTypedef *sBME280, uint8_t Address, uint16_t *Data)
{
	uint8_t tmp[2];
	if(HAL_I2C_Mem_Read(sBME280->i2c, (sBME280->addr << 1), Address, 1, tmp, 2, 1000) == HAL_OK)
	{
		*Data = (uint16_t)((tmp[1] << 8) | (tmp[0]));
		return BME280_OK;
	}
	else return BME280_READ_ERROR;
}

static BME280_Status BME280_Read24(BME280_HandleTypedef *sBME280, uint8_t Address, uint32_t *Data)
{
	uint8_t tmp[3];
	if(HAL_I2C_Mem_Read(sBME280->i2c, (sBME280->addr << 1), Address, 1, tmp, 3, 1000) == HAL_OK)
	{
		*Data = (uint32_t)((tmp[0] << 16) | (tmp[1] << 8) | (tmp[2]));
		return BME280_OK;
	}
	else return BME280_READ_ERROR;
}


static BME280_Status BME280_Read_Temperature_Raw(BME280_HandleTypedef *sBME280, int32_t *Data)
{
	if(BME280_Read24(sBME280, BME280_REG_TEMP_MSB, (uint32_t*)Data) == BME280_OK)
	{
		*Data >>= 4;
		return BME280_OK;
	}
	else return BME280_ERROR;
}

static BME280_Status BME280_Read_Pressure_Raw(BME280_HandleTypedef *sBME280, uint32_t *Data)
{
	if(BME280_Read24(sBME280, BME280_REG_PRESS_MSB, Data) == BME280_OK)
	{
		*Data >>= 4;
		return BME280_OK;
	}
	else return BME280_ERROR;
}

static BME280_Status BME280_Read_Humidity_Raw(BME280_HandleTypedef *sBME280, uint16_t *Data)
{
	if(BME280_Read16(sBME280, BME280_REG_HUM_MSB, Data) == BME280_OK)
	{
		return BME280_OK;
	}
	else return BME280_ERROR;
}

static BME280_Status BME280_Read_Calibration_Data(BME280_HandleTypedef * sBME280)
{
	uint32_t tmp;
	BME280_Read24(sBME280, 0xE4, &tmp);

	sBME280->dig_H4 = (((tmp & 0x000000FF) << 4 ) | ((tmp & 0x00000F00) >> 8));
	sBME280->dig_H5 = (((tmp & 0x0000F000) >> 12) | ((tmp & 0x00FF0000) >> 8));

	if(!BME280_Read16(sBME280, 0x88, &sBME280->dig_T1)
			&& !BME280_Read16(sBME280, 0x8A, (uint16_t*)&sBME280->dig_T2)
			&& !BME280_Read16(sBME280, 0x8C, (uint16_t*)&sBME280->dig_T3)
			&& !BME280_Read16(sBME280, 0x8E, &sBME280->dig_P1)
			&& !BME280_Read16(sBME280, 0x90, (uint16_t*)&sBME280->dig_P2)
			&& !BME280_Read16(sBME280, 0x92, (uint16_t*)&sBME280->dig_P3)
			&& !BME280_Read16(sBME280, 0x94, (uint16_t*)&sBME280->dig_P4)
			&& !BME280_Read16(sBME280, 0x96, (uint16_t*)&sBME280->dig_P5)
			&& !BME280_Read16(sBME280, 0x98, (uint16_t*)&sBME280->dig_P6)
			&& !BME280_Read16(sBME280, 0x9A, (uint16_t*)&sBME280->dig_P7)
			&& !BME280_Read16(sBME280, 0x9C, (uint16_t*)&sBME280->dig_P8)
			&& !BME280_Read16(sBME280, 0x9E, (uint16_t*)&sBME280->dig_P9)
			&& !BME280_Read8(sBME280, 0xA1, &sBME280->dig_H1)
			&& !BME280_Read16(sBME280, 0xE1, (uint16_t*)&sBME280->dig_H2)
			&& !BME280_Read8(sBME280, 0xE3, &sBME280->dig_H3)
			&& !BME280_Read8(sBME280, 0xE7, (uint8_t*)&sBME280->dig_H6))

	{
		return BME280_OK;
	}

	return BME280_READ_CALIB_ERROR;
}

static void BME280_Init_Default(BME280_HandleTypedef *sBME280)
{
	sBME280->param.mode = BME280_MODE_NORMAL;
	sBME280->param.filter = BME280_FILTER_16;
	sBME280->param.pressure_oversampling = BME280_ULTRA_HIGH_RES;
	sBME280->param.temperature_oversampling = BME280_LOW_POWER;
	sBME280->param.humidity_oversampling = BME280_ULTRA_LOW_POWER;
	sBME280->param.standby_time = BME280_STANDBY_500;
}

BME280_Status BME280_Init(BME280_HandleTypedef *sBME280, I2C_HandleTypeDef * I2C, uint8_t Address)
{
	uint8_t ctrl_meas, ctrl_hum;
	uint8_t config;

	sBME280->i2c = I2C;
	sBME280->addr = Address;

	BME280_Init_Default(sBME280);

	// read device's id
	BME280_Read8(sBME280, BME280_REG_ID, &sBME280->id);

	// check if it is same id
	if(sBME280->id != BME280_CHIP_ID)
	{
		return BME280_WRONG_CHIPID_ERROR;
	}

//	BME280_Write8(sBME280, BME280_REG_RESET, BME280_RESET_VALUE);

	// read calibration data if everything is ok
	while(BME280_Read_Calibration_Data(sBME280))
	{
		return BME280_READ_CALIB_ERROR;
	}

	// config
	config = (sBME280->param.standby_time << 5) | (sBME280->param.filter << 2);
	if(BME280_Write8(sBME280, BME280_REG_CONFIG, config))
	{
		return BME280_CONFIG_ERROR;
	}

	// control_humidity
	ctrl_hum = (sBME280->param.humidity_oversampling);
	if(BME280_Write8(sBME280, BME280_REG_CTRL_HUM, ctrl_hum))
	{
		return BME280_CTRL_HUM_ERROR;
	}

	// control_measurement
	ctrl_meas = (sBME280->param.temperature_oversampling << 5) | (sBME280->param.pressure_oversampling << 2) | (sBME280->param.mode);
	if(BME280_Write8(sBME280, BME280_REG_CTRL_MEAS, ctrl_meas))
	{
		return BME280_CTRL_MEAS_ERROR;
	}

	return BME280_OK;
}

static float Compensate_Temperature(BME280_HandleTypedef * sBME280)
{
	double var1, var2, T;
	int32_t adc_temp;

	BME280_Read_Temperature_Raw(sBME280, &adc_temp);
	var1 = (((double)adc_temp)/16384.0 - ((double)sBME280->dig_T1)/1024.0) * ((double)sBME280->dig_T2);
	var2 = ((((double)adc_temp)/131072.0 - ((double)sBME280->dig_T1)/8192.0) *
	(((double)adc_temp)/131072.0 - ((double)sBME280->dig_T1)/8192.0)) * ((double)sBME280->dig_T3);
	sBME280->t_fine = (int32_t)(var1 + var2);
	T = (var1 + var2) / 5120.0;

	return T;
}

static float Compensate_Pressure(BME280_HandleTypedef * sBME280)
{
	double var1, var2, p;
	int32_t adc_press;
	BME280_Read_Pressure_Raw(sBME280, (uint32_t*)&adc_press);


	var1 = ((double)sBME280->t_fine/2.0) - 64000.0;
	var2 = var1 * var1 * ((double)sBME280->dig_P6) / 32768.0;
	var2 = var2 + var1 * ((double)sBME280->dig_P5) * 2.0;
	var2 = (var2/4.0)+(((double)sBME280->dig_P4) * 65536.0);
	var1 = (((double)sBME280->dig_P3) * var1 * var1 / 524288.0 + ((double)sBME280->dig_P2) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0)*((double)sBME280->dig_P1);
	if (var1 == 0.0)
	{
	return 0; // avoid exception caused by division by zero
	}
	p = 1048576.0 - (double)adc_press;
	p = (p - (var2 / 4096.0)) * 6250.0 / var1;
	var1 = ((double)sBME280->dig_P9) * p * p / 2147483648.0;
	var2 = p * ((double)sBME280->dig_P8) / 32768.0;
	p = p + (var1 + var2 + ((double)sBME280->dig_P7)) / 16.0;
	return p;
}

static float Compensate_Humidity(BME280_HandleTypedef * sBME280)
{
	double var_H;
	int32_t adc_hum;

	BME280_Read_Humidity_Raw(sBME280, (uint16_t*)&adc_hum);

	var_H = (((double)sBME280->t_fine) - 76800.0);
	var_H = (adc_hum - (((double)sBME280->dig_H4) * 64.0 + ((double)sBME280->dig_H5) / 16384.0 * var_H)) *
	(((double)sBME280->dig_H2) / 65536.0 * (1.0 + ((double)sBME280->dig_H6) / 67108864.0 * var_H *
	(1.0 + ((double)sBME280->dig_H3) / 67108864.0 * var_H)));
	var_H = var_H * (1.0 - ((double)sBME280->dig_H1) * var_H / 524288.0);
	if (var_H > 100.0)
	var_H = 100.0;
	else if (var_H < 0.0)
	var_H = 0.0;
	return var_H;
}

static int32_t Compensate_Temperature_Fixed(BME280_HandleTypedef * sBME280)
{
	int32_t var1, var2, T;
	int32_t adc_temp;
	BME280_Read_Temperature_Raw(sBME280, &adc_temp);

	var1 = ((((adc_temp>>3) - ((int32_t)sBME280->dig_T1<<1))) * ((int32_t)sBME280->dig_T2)) >> 11;

	var2 = (((((adc_temp>>4) - ((int32_t)sBME280->dig_T1)) * ((adc_temp>>4) - ((int32_t)sBME280->dig_T1))) >> 12) * ((int32_t)sBME280->dig_T3)) >> 14;

	sBME280->t_fine = var1 + var2;

	T = (sBME280->t_fine * 5 + 128) >> 8;

	return T;
}

static uint32_t Compensate_Pressure_Fixed(BME280_HandleTypedef * sBME280)
{
	int32_t var1, var2;
	uint32_t adc_press, p;

	BME280_Read_Pressure_Raw(sBME280, &adc_press);

	var1 = (((int32_t)sBME280->t_fine)>>1) - (int32_t)64000;

	var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((int32_t)sBME280->dig_P6);

	var2 = var2 + ((var1*((int32_t)sBME280->dig_P5))<<1);

	var2 = (var2>>2)+(((int32_t)sBME280->dig_P4)<<16);

	var1 = (((sBME280->dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((int32_t)sBME280->dig_P2) * var1)>>1))>>18;

	var1 =((((32768+var1))*((int32_t)sBME280->dig_P1))>>15);

	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}

	p = (((uint32_t)(((int32_t)1048576)-adc_press)-(var2>>12)))*3125;

	if (p < 0x80000000)
	{
		p = (p << 1) / ((uint32_t)var1);
	}
	else
	{
		p = (p / (uint32_t)var1) * 2;
	}

	var1 = (((int32_t)sBME280->dig_P9) * ((int32_t)(((p>>3) * (p>>3))>>13)))>>12;

	var2 = (((int32_t)(p>>2)) * ((int32_t)sBME280->dig_P8))>>13;

	p = (uint32_t)((int32_t)p + ((var1 + var2 + sBME280->dig_P7) >> 4));

	return p;

}

BME280_Status BME280_Read_Double(BME280_HandleTypedef *sBME280, float *temp, float *press, float *hum)
{
	*temp = Compensate_Temperature(sBME280);
	*press = Compensate_Pressure(sBME280);
	*hum = Compensate_Humidity(sBME280);

	return BME280_OK;
}

BME280_Status BME280_Read_Fixed(BME280_HandleTypedef *sBME280, int32_t * temperature, uint32_t *pressure)
{
	*temperature = Compensate_Temperature_Fixed(sBME280);
	*pressure = Compensate_Pressure_Fixed(sBME280);

	return BME280_OK;
}

BME280_Status BME280_Read_Temp(BME280_HandleTypedef *sBME280, int32_t * temperature)
{
	*temperature = (Compensate_Temperature_Fixed(sBME280) / 100);

	return BME280_OK;
}
