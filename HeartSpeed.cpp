#include "HeartSpeed.h"

int heartRate[20]={0};     ///<Heart rate total sample
int sample[sampleSize]={0};

int average_switch=1;
int sum_for_collect;
float average_num;
int sum_high=0;
int sum_low=0;
int num_sum_high=0,num_sum_low=0;///<Two types of situations sum

HEARTCBFUNC cb;
/*!
* @brief The constructor
*
* @brief For get pin and rawDataData
*
* @brief Constructor
*/ 
HeartSpeed::HeartSpeed(uint8_t _pin,uint8_t _rawData)
	:pin(_pin), rawData(_rawData)
{
}
/*!
*  @brief Sets the callback function
*
*  @brief Obtain the callback function name
*
*  @return void
*/
void HeartSpeed::setCB(HEARTCBFUNC cb_)
{
	cb = cb_;
}
/*!
*  @brief Start program
*
*  @brief Set the timer, 5 seconds at a time
*
*  @return void
*/
void HeartSpeed::begin(void)
{
  MsTimer2::set(5,recv,this);
  MsTimer2::start();
}
/*!
*  @brief Receive serial data and process
*
*  @brief Will receive the data of kalman filter, and square wave modulation
* 
*  @return void
*/
void HeartSpeed::recv(void *param)
{
  HeartSpeed *_this = (HeartSpeed *)param;
	int value;
	int data = analogRead(_this->pin);
	if(_this->rawData==1){
    Serial.println(data);
    cb(_this->rawData,data);
    return;
  }

  if(_this->sampleNum<sampleSize){
    sample[_this->sampleNum]=data;
    sample[_this->sampleNum] = _this->kalmanUpdate(0,sample[_this->sampleNum]);///<Square wave using Kalman filter

    if(average_switch){
        if(sample[_this->sampleNum] >= 0){
          sum_high += sample[_this->sampleNum];  
          num_sum_high++;
        }else{
          sum_low += sample[_this->sampleNum];  
          num_sum_low++;
        }
    }
  }else{
    for(int i=0;i<(sampleSize-1);i++)
    {
        sample[i]=sample[i+1];				///<An array of right.
    }
    sample[sampleSize-1]=data;				///<The last array update.
    sample[_this->sampleNum-1] = _this->kalmanUpdate(0,sample[_this->sampleNum-1]);

    average_switch = 0;

    if((sample[_this->sampleNum-1]>=(sum_low*1.1/num_sum_low))&&(_this->high==1)){
          sample[_this->sampleNum-1]=(-15);          
    }else{
        _this->high=0;
        _this->low=1;
    }

    if((sample[_this->sampleNum-1]<=(sum_high*1.5/num_sum_high))&&(_this->low==1)){
          sample[_this->sampleNum-1]=(+25);        
    }
    else{
        _this->low=0;
        _this->high=1;
    }
  }

  if(_this->sampleNum<sampleSize){
    _this->sampleNum++;
  }else{
    _this->sampleNum =sampleSize;
  }

  value = _this->calculateSpeed();
  if(_this->Count==200){		///<5ms once,200 times about 1s
  	if(value > 0){
  		cb(_this->rawData,value);
  	}
    _this->Count=0;
  }else{
    _this->Count++;
  }
}
/*!
*  @brief Calculate heart rate value
*
*  @brief Calculate the number of times per minute and time,Each time to beat
*
*  @return int
*/
int HeartSpeed::calculateSpeed()
{
	int ret = -1;
	if(heartRateNum<20){                     
        for(int i=1;i<sampleSize;i++)
        {
            if(sample[i] != sample[i-1]){	///<Square, adjoining values, represents the waveform changes
                if(go1 == 0){
                    go1 = i;
                }else if(go2 == 0){
                    go2 = i;
                }else if(go3 ==0){
                    go3 = i;
                }else if(go4 ==0){
                    go4 = i;
                    break;
                }
            }
        }
        if((go1==0)||(go2==0)||(go3==0)||(go4==0)){   ///<Becase of the size is 256,A period of more than 256 values,When a group of arrays cannot represent a complete waveform, then the acquisition.
          //nothing to do
        }else{
    		  heartRate[heartRateNum]=(go4-go1);
    		  sum += heartRate[heartRateNum];
    		  heartRateNum++;
		    }
    }
	if(heartRateNum == 20){
        for(int i=1;i<sampleSize;i++)
        {
            if(sample[i] != sample[i-1]){
                if(go1 == 0){
                    go1 = i;
                }else if(go2 == 0){
                    go2 = i;
                }else if(go3 ==0){
                    go3 = i;
                }else if(go4 ==0){
                    go4 = i;
          					if((go4 - go1)<85){
          						go4=0;
          					}else if((go4-go1)>260){
          					  goInit();
          					}else{
          						break;
          					}
                }
            }
        }

        if((go1==0)||(go2==0)||(go3==0)||(go4==0)||((go2-go1)<0)||\
          ((go3-go2)<0)||((go4-go3)<0)){   		///<Filter exceptions beat wave
          goInit();
          goto exit;
        }else{
          sum=0;
          for(int i=0;i<19;i++){		///<After 20 times, nine before migration, last update
              heartRate[i]=heartRate[i+1];
              sum+=heartRate[i];
          }
          heartRate[19]=(go4-go1);
          sum+=heartRate[19];
        }
    }

   if(heartRateNum==20){
     float num_min = 1000,num_max = 0;
     for(int i=0;i<20;i++){
          if(heartRate[i]<num_min){
              num_min = heartRate[i];
          }
          if(heartRate[i]>num_max){
              num_max = heartRate[i];
          }
     }
     
		heart = 60000/(((sum)*5)/(heartRateNum));	
    //heart = 60000/(((sum-num_max-num_min)*5)/(heartRateNum-2)); ///<Minus the maximum and minimum values
		sum=0;
		goInit();
		ret = (int)heart;
		goto exit;
   }
  goInit();
exit:
  return ret;
}
/*!
*  @brief Initializes the heart rate period
*
*  @brief Make go1,go2,go3,go4 equal 0
*
*  @return void
*/
void HeartSpeed::goInit(){
    go1=0;	///<First change
    go2=0;	///<Second change
    go3=0;	///<Third change
    go4=0;	///<Fourth change
}
/*!
*  @brief Kalman filtering
*
*  @brief Remove the varied data, make the waveform more smoothly
*
*  @return void
*/
float HeartSpeed::kalmanUpdate(float Accel,float Gyro){
    Angle+=(Gyro - Q_bias) * dt; ///<Prior estimates

    Pdot[0]=Q_angle - PP[0][1] - PP[1][0]; ///< Pk-A priori estimation error covariance of differential
    Pdot[1]=- PP[1][1];
    Pdot[2]=- PP[1][1];
    Pdot[3]=Q_gyro;
    PP[0][0] += Pdot[0] * dt;   ///< Pk-A priori estimation error covariance of differential-integral
    PP[0][1] += Pdot[1] * dt;   ///< =A priori estimation error covariance
    PP[1][0] += Pdot[2] * dt;
    PP[1][1] += Pdot[3] * dt;

    Angle_err = Accel - Angle;        ///<zk-Prior estimates

    PCt_0 = C_0 * PP[0][0];
    PCt_1 = C_0 * PP[1][0];

    E = R_angle + C_0 * PCt_0;

    K_0 = PCt_0 / E;
    K_1 = PCt_1 / E;

    t_0 = PCt_0;
    t_1 = C_0 * PP[0][1];

    PP[0][0] -= K_0 * t_0;                 ///<Posteriori error covariance
    PP[0][1] -= K_0 * t_1;
    PP[1][0] -= K_1 * t_0;
    PP[1][1] -= K_1 * t_1;

    Angle        += K_0 * Angle_err;         ///<A posteriori estimation
    Q_bias        += K_1 * Angle_err;         ///<A posteriori estimation
    Gyro_y   = Gyro - Q_bias;         ///<Output values (a posteriori) differential = angular velocity

    Angle = Angle + (((Accel-Angle)*0.5 + Gyro_y)*0.001);

    return Angle;
}




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
