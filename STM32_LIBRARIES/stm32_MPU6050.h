/*
 ***************************************************************************************************************
 ***************************************************************************************************************
 ***************************************************************************************************************

  File:		  MPU6050_STM32.h
  Updated:    Dec 19, 2023

 */
 
//includes
 #include "stm32f407xx.h"
 #include "stm32f4xx_hal.h"
 
//defines
 
 #define MPU6050_ADDR 0xD0
 
 #define SMPLRT_DIV_REG 0x19
 #define GYRO_CONFIG_REG 0x1B
 #define ACCEL_CONFIG_REG 0x1C
 #define ACCEL_XOUT_H_REG 0x3B
 #define TEMP_OUT_H_REG 0x41
 #define GYRO_XOUT_H_REG 0x43
 #define PWR_MGMT_1_REG 0x6B
 #define WHO_AM_I_REG 0x75
 
 int16_t Accel_X_RAW = 0;
 int16_t Accel_Y_RAW = 0;
 int16_t Accel_Z_RAW = 0;

 int16_t Gyro_X_RAW = 0;
 int16_t Gyro_Y_RAW = 0;
 int16_t Gyro_Z_RAW = 0;

 extern float Ax, Ay, Az, Gx, Gy, Gz;
 
 
//functions
 
 void MPU6050_init(I2C_HandleTypeDef* hi2c){
	 uint8_t check, data;
	
	 // read who am i register to check if the sensor is ok
	 HAL_I2C_Mem_Read(hi2c, MPU6050_ADDR, WHO_AM_I_REG, 1, &check, 1, 1000);
	 
	 if(check == 104){
		 
		 // wake the sensor by writing 0 to the power management register
	   data = 0;
		 HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &data, 1, 1000);
		 
		 // set the semple frequencyt to 1kHz
		 data=0x07;
		 HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &data, 1, 1000);
		 
		 // Set accelerometer configuration 
		 // XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> ? 2g
		 data = 0x00;
		 HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &data, 1, 1000);

		 // Set Gyroscopic configuration 
		 // XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> ? 250 ?/s
		 data = 0x00;
		 HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &data, 1, 1000);
		 
	 }
 }
 
void MPU6050_Read_Acceleration (I2C_HandleTypeDef* hi2c)
{
	uint8_t Rec_Data[6];

	// Read 6 BYTES of data starting from ACCEL_XOUT_H register

	HAL_I2C_Mem_Read (hi2c, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, 6, 1000);

	Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
	Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
	Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);

	/*** convert the RAW values into acceleration in 'g'
	     we have to divide according to the Full scale value set in FS_SEL
	     I have configured FS_SEL = 0. So I am dividing by 16384.0
	     for more details check ACCEL_CONFIG Register              ****/

	Ax = Accel_X_RAW/16384.0;
	Ay = Accel_Y_RAW/16384.0;
	Az = Accel_Z_RAW/16384.0;
}

void MPU6050_Read_Gyroscope (I2C_HandleTypeDef* hi2c)
{
	uint8_t Rec_Data[6];

	// Read 6 BYTES of data starting from GYRO_XOUT_H register

	HAL_I2C_Mem_Read (hi2c, MPU6050_ADDR, GYRO_XOUT_H_REG, 1, Rec_Data, 6, 1000);

	Gyro_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
	Gyro_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
	Gyro_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);

	/*** convert the RAW values into dps (?/s)
	     we have to divide according to the Full scale value set in FS_SEL
	     I have configured FS_SEL = 0. So I am dividing by 131.0
	     for more details check GYRO_CONFIG Register              ****/

	Gx = Gyro_X_RAW/131.0;
	Gy = Gyro_Y_RAW/131.0;
	Gz = Gyro_Z_RAW/131.0;
}