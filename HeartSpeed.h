
 /*!
* @file Heartrate.h
* @brief Heartrate.h detailed description for Heartrate.cpp 
*
*  This is written for the heart rate sensor the company library. Mainly used for real 
*  time measurement of blood oxygen saturation, based on measured values calculate heart rate values.
* 
* @author jianghao(460221531@qq.com)
* @version  V1.0
* @date  2016-07-08
*/

#ifndef __HEARSPEED__H
#define __HEARSPEED__H

#include "Arduino.h"
#include "MyTimer2.h"

typedef void (*HEARTCBFUNC)(uint8_t,int);

#define RAW_DATA 1           		///<Serial port mapping switch
#define HEARRATE_VALUE 0           	///<Off by default serial port mapping
	
#define sampleSize 512  			

class HeartSpeed
{
public:
	/*!
   *  @brief Constructor
   */
	HeartSpeed(uint8_t _pin, uint8_t _rawData = HEARRATE_VALUE);
	/*!
   *  @brief Sets the callback function
   */
	void setCB(HEARTCBFUNC cb_);
	/*!
   *  @brief Start test
   */
	void begin(void);
	/*!
   *  @brief Calculate heart rate value
   */
	int calculateSpeed(void);
	/*!
   *  @brief Receive serial data and process
   */
	static void recv( void *);
	/*!
   *  @brief Initializes the heart rate period
   */
	void goInit();
	/*!
   *  @brief Kalman filtering
   */
	float kalmanUpdate(float Accel,float Gyro);
	
private:
	uint8_t rawData;      			 ///<Serial port mapping switch
	uint8_t pin;           
	int Count;			  			 ///<Counts, to run once every minute

	/*The following data is used to calculate the heart rate value*/
	float heart;
	int heartRateNum=0;
	int sum;
	int go1;               			 
	int go2;
	int go3;
	int go4;

	/*Square-wave operation enabling conditions*/
	int high=1;           
	int low=0;

	int sampleNum;					///<Heart rate values

	/*Kalman filter parameters*/
	float  Gyro_y;      
	float  Angle;
	float  Q_bias, Angle_err;
	float  dt=0.05;
	float  Pdot[4] ={0,0,0,0};
	float  Q_angle=0.001;
	float  PP[2][2] = { { 1, 0 },{ 0, 1 } };
	float  Q_gyro=0.003;
	float  PCt_0, PCt_1, E;
	float  K_0, K_1, t_0, t_1;
	char   C_0 = 1;
	float  R_angle=0.5;
};

#endif

/******************************************************************************
  Copyright (C) <2016>  <jianghao>
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  Contact: 460221531@qq.com
 ******************************************************************************/
 
