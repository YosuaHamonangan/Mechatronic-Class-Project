#ifndef CONF_H_INCLUDED
#define CONF_H_INCLUDED

#include "stm32f30x_gpio.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_tim.h"
#include "stm32f30x_misc.h"
#include "stm32f30x_usart.h"
#include <math.h>

#define TMOTOR 2000 //20ms
#define MAJU 38
#define MUNDUR 40
#define KANAN 39
#define KIRI 37
#define STOP 0
#define SCAN 32


void init();
void InitServoMotor();
void InitEncoderInterface();
void InitBluetooth();
void InitSRF();
void SetSudut(int Sudut);
void SetMotorR(double Duty, int Arah);
void SetMotorL(double Duty, int Arah);
double GetMotorR();
double GetMotorL();
void BlueSend(int buffer);
//void BlueRead(void);

#endif
