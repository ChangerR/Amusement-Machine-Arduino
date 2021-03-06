////////////////////////////////////////////////////////////////////////////
//
//  This file is part of MPU9150Lib
//
//  Copyright (c) 2013 Pansenti, LLC
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of 
//  this software and associated documentation files (the "Software"), to deal in 
//  the Software without restriction, including without limitation the rights to use, 
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the 
//  Software, and to permit persons to whom the Software is furnished to do so, 
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all 
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

//#include "I2Cdev.h"
#include "MPU9150Lib.h"
#include "CalLib.h"
//#include "dmpKey.h"
//#include "dmpmap.h"
//#include "inv_mpu.h"
//#include "inv_mpu_dmp_motion_driver.h"
#include <EEPROM.h>
#include "misc.h"
#include "slconfig.h"
#include "timer.h"
#include "Wire.h"

MPU9150Lib MPU;                                              // the MPU object
int MPUDeviceId = 1;
boolean DidInit = false;
boolean InCallibrationMode = false;
CALLIB_DATA calData;
int counter = 0;

ElapseTimer MPU9150ReInit;
//  MPU_UPDATE_RATE defines the rate (in Hz) at which the MPU updates the sensor data and DMP output

//  MPU_UPDATE_RATE defines the rate (in Hz) at which the MPU updates the sensor data and DMP output

#define MPU_UPDATE_RATE  (20)

//  MAG_UPDATE_RATE defines the rate (in Hz) at which the MPU updates the magnetometer data
//  MAG_UPDATE_RATE should be less than or equal to the MPU_UPDATE_RATE

#define MAG_UPDATE_RATE  (10)

//  MPU_MAG_MIX defines the influence that the magnetometer has on the yaw output.
//  The magnetometer itself is quite noisy so some mixing with the gyro yaw can help
//  significantly. Some example values are defined below:

#define  MPU_MAG_MIX_GYRO_ONLY          0                   // just use gyro yaw
#define  MPU_MAG_MIX_MAG_ONLY           1                   // just use magnetometer and no gyro yaw
#define  MPU_MAG_MIX_GYRO_AND_MAG       10                  // a good mix value
#define  MPU_MAG_MIX_GYRO_AND_SOME_MAG  50                  // mainly gyros with a bit of mag correction

//  MPU_LPF_RATE is the low pas filter rate and can be between 5 and 188Hz

#define MPU_LPF_RATE   5

void mpu9150_init() {
	Wire.begin();
	MPU.selectDevice(MPUDeviceId);
	if (!MPU.init(MPU_UPDATE_RATE, MPU_MAG_MIX_GYRO_ONLY, MAG_UPDATE_RATE, MPU_LPF_RATE)){
		Serial.println(F("log(\"Trying other MPU9150 address to init\")"));
		Serial.print(F("log(\"IMU Address was :"));
		Serial.print(1);
		MPUDeviceId = !MPUDeviceId;
		Serial.print(F(" but is now:"));
		Serial.print(MPUDeviceId);
		Serial.println("\")");
		MPU.selectDevice(MPUDeviceId);
		if (MPU.init(MPU_UPDATE_RATE, MPU_MAG_MIX_GYRO_ONLY, MAG_UPDATE_RATE, MPU_LPF_RATE)){
			DidInit = true;
			Serial.println(F("log(\"Init worked the second time\")"));
		} else {
			Serial.println(F("log(\"Failed to init on both addresses\")"));
		}
	} else {
		DidInit = true;
		Serial.println(F("log(\"init on primary addresses\")"));
	}                             // start the MPU
	MPU9150ReInit.reset();
}

void mpu9150_update() {
	
	if (!DidInit){
		if( MPU9150ReInit.elapsed(30000)){
			mpu9150_init();
		}else {
			return;
		}
	}	
	
	if (MPU.read()) {

		SLSERIAL.print("MPU9150(");
		SLSERIAL.print(MPU.m_fusedEulerPose[VEC3_X] * RAD_TO_DEGREE);
		SLSERIAL.print(",");
		SLSERIAL.print(MPU.m_fusedEulerPose[VEC3_Y] * RAD_TO_DEGREE);
		SLSERIAL.print(",");
		SLSERIAL.print(MPU.m_fusedEulerPose[VEC3_Z] * RAD_TO_DEGREE);
		SLSERIAL.println(")");
	}
}

void mpu9150_setup()
{	
	mpu9150_init();
	
	timer::getInstance()->add_timer(200,TIMER_REPEAT,mpu9150_update);
}

/*
void mpu9150_ccal(int args,char (*argv)[MAX_ARGUMENT_LENGTH]) {
	
}
*/
