#include "conf.h"

uint16_t Kanan_prv = 0, Kiri_prv = 0;
int CounterR = 0, CounterL = 0;

void init()
{
	InitServoMotor();
	InitEncoderInterface();
	InitBluetooth();
	InitSRF();
}

void InitServoMotor()
{
	//PWM
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_2);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_BaseStruct;
	TIM_BaseStruct.TIM_Prescaler = (SystemCoreClock / 100000) - 1;
	TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_BaseStruct.TIM_Period = TMOTOR-1;
	TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_BaseStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_BaseStruct);
	TIM_Cmd(TIM2, ENABLE);

	TIM_OCInitTypeDef TIM_OCStruct;
	TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_Low;

	TIM_OCStruct.TIM_Pulse = 99;
	TIM_OC3Init(TIM2, &TIM_OCStruct);

	TIM_OCStruct.TIM_Pulse = 0;
	TIM_OC2Init(TIM2, &TIM_OCStruct);
	TIM_OC4Init(TIM2, &TIM_OCStruct);

	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);


	//Arah
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz ;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOD, &GPIO_InitStruct);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz ;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void InitEncoderInterface()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_2);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_EncoderInterfaceConfig (TIM4,
								TIM_EncoderMode_TI12,
								TIM_ICPolarity_Rising,
								TIM_ICPolarity_Rising);

	TIM_SetAutoreload (TIM4, 0xffff);
	TIM_Cmd (TIM4, ENABLE);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_2);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_EncoderInterfaceConfig (TIM3,
								TIM_EncoderMode_TI12,
								TIM_ICPolarity_Rising,
								TIM_ICPolarity_Rising);
	TIM_SetAutoreload (TIM3, 0xffff);
	TIM_Cmd (TIM3, ENABLE);
}

void InitBluetooth()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_7);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_7);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	USART_InitTypeDef USART_InitStructure;
	USART_DeInit(USART3);
	USART_InitStructure.USART_BaudRate = 38400;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3,&USART_InitStructure);

	USART_Cmd(USART3, ENABLE);
}

void InitSRF()
{
	//Trigger
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz ;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	GPIO_Init(GPIOF, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_3);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
	TIM_TimeBaseInitTypeDef TIM_BaseStruct;
	TIM_BaseStruct.TIM_Prescaler = (SystemCoreClock / 1000000) - 1;
	TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_BaseStruct.TIM_Period = 0xFFFFFFFF;
	TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_BaseStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_BaseStruct);
	TIM_TimeBaseInit(TIM15, &TIM_BaseStruct);

	TIM_ICInitTypeDef TIM_ICStruct;
	TIM_ICStruct.TIM_Channel = TIM_Channel_1;
	TIM_ICStruct.TIM_ICPolarity =TIM_ICPolarity_Rising;
	TIM_ICStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICStruct.TIM_ICPrescaler = 0;
	TIM_ICStruct.TIM_ICFilter = 0x0;
	TIM_PWMIConfig(TIM1, &TIM_ICStruct);
	TIM_PWMIConfig(TIM15, &TIM_ICStruct);

	TIM_SelectInputTrigger(TIM15, TIM_TS_TI1FP1);
	TIM_SelectSlaveMode(TIM15, TIM_SlaveMode_Reset);
	TIM_SelectMasterSlaveMode(TIM15,TIM_MasterSlaveMode_Enable);
	TIM_Cmd(TIM15, ENABLE);

	TIM_SelectInputTrigger(TIM1, TIM_TS_TI1FP1);
	TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Reset);
	TIM_SelectMasterSlaveMode(TIM1,TIM_MasterSlaveMode_Enable);
	TIM_Cmd(TIM1, ENABLE);
}

void SetSudut(int Sudut)
{
	TIM_SetCompare3(TIM2,59+Sudut*188/180);
}

void SetMotorR(double Duty, int Arah)
{
	if(Arah==MAJU)
	{
		GPIO_WriteBit(GPIOB , GPIO_Pin_5 , 0);
		GPIO_WriteBit(GPIOB , GPIO_Pin_3 , 1);
	}
	else if(Arah==MUNDUR)
	{
		GPIO_WriteBit(GPIOB , GPIO_Pin_3 , 0);
		GPIO_WriteBit(GPIOB , GPIO_Pin_5 , 1);
	}
	else
	{
		GPIO_WriteBit(GPIOB , GPIO_Pin_5 , 0);
		GPIO_WriteBit(GPIOB , GPIO_Pin_3 , 0);
	}
	TIM_SetCompare4(TIM2,Duty*(TMOTOR-1));
}

void SetMotorL(double Duty, int Arah)
{
	if(Arah==MAJU)
	{
		GPIO_WriteBit(GPIOD , GPIO_Pin_0 , 0);
		GPIO_WriteBit(GPIOD , GPIO_Pin_2 , 1);
	}
	else if(Arah==MUNDUR)
	{
		GPIO_WriteBit(GPIOD , GPIO_Pin_2 , 0);
		GPIO_WriteBit(GPIOD , GPIO_Pin_0 , 1);
	}
	else
	{
		GPIO_WriteBit(GPIOD , GPIO_Pin_0 , 0);
		GPIO_WriteBit(GPIOD , GPIO_Pin_2 , 0);
	}
	TIM_SetCompare2(TIM2, Duty*(TMOTOR-1));
}

double GetMotorR()
{
	uint16_t counter = TIM_GetCounter(TIM4);
	if(counter>55000 && Kanan_prv<15000) CounterR--;
	if(Kanan_prv>60000 && counter<15000) CounterR++;
	Kanan_prv = counter;

	if(CounterR<0) return -(65536*(CounterR+1) + counter - 65536)/38.4;//360/128/4/27
	else return -(CounterR*65536+counter)/38.4;

}

double GetMotorL()
{
	uint16_t counter = TIM_GetCounter(TIM3);
	if(counter>55000 && Kiri_prv<15000) CounterL--;
	if(Kiri_prv>60000 && counter<15000) CounterL++;
	Kiri_prv = counter;

	if(CounterL<0) return -(65536*(CounterL+1) + counter - 65536)/38.4;
	else return -(CounterL*65536+counter)/38.4;
}

void BlueSend(int buffer)
{
      USART_SendData(USART3, buffer);
      while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);

}

/*void BlueRead()
{
	if(USART_GetFlagStatus(USART3, USART_FLAG_RXNE))
	{
		char x = USART_ReceiveData(USART3);
		if(x=='a')
		{
			SetMotorR(100, MAJU);
			SetMotorL(100, MAJU);
		}
		if(x=='b')
		{
			SetMotorR(100, MUNDUR);
			SetMotorL(100, MUNDUR);
		}
		if(x=='c')
		{
			SetMotorR(100, STOP);
			SetMotorL(100, STOP);
		}
		USART_ClearFlag(USART3, USART_FLAG_RXNE);
	}
}*/
