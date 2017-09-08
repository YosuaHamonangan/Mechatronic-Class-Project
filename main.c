#include "conf.h"

#define uPERIOD 50000
#define PERIOD ((double)uPERIOD/1000000.0)
#define PI 3.14159265358
#define DTR PI/180.0
#define RTD 180.0/PI
#define W	0.165
#define R	0.031
#define Kv	4
#define Kp	(Kv/1.414)*(Kv/1.414)
#define Kv_shi	1
#define Kp_shi	(Kv_shi/1.414)*(Kv_shi/1.414)

#define DURASI	2.0
#define STEP	0.6

double temp;
double qr=0.0, qr_prv=0.0, wr;
double ql=0.0, ql_prv=0.0, wl;
double x=0.0,y=0.0, x_cmd=0,y_cmd=0;
double v;
double shi=0.0*DTR;
double ddshi=0.0, shi_old=0,shi_init=0, dshi=0.0, dshi_d=0.0;
double x_init, y_init;
double x_d, y_d;
double dx=0.0,dy=0.0, dx_d,dy_d,shi_d, ddx_ref, ddy_ref, ddshi_ref;
double dqr=0.0,dql=0.0;
double ddqr_ref, ddql_ref;

int t=500;

int dir=1;
uint16_t jarak1,jarak2,sudut = 0;
double kanan=0,kiri=0;
double speed_kanan,speed_kiri;
int a=0,jalan=0,servo;

int main(void)
{
	init();

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_BaseStruct;
	TIM_BaseStruct.TIM_Prescaler = (SystemCoreClock / 1000000) - 1;
	TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_BaseStruct.TIM_Period = uPERIOD-1;
	TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_BaseStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM6, &TIM_BaseStruct);
	TIM_Cmd(TIM6, ENABLE);
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

	NVIC_InitTypeDef nvicStructure;
	nvicStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
	nvicStructure.NVIC_IRQChannelSubPriority = 1;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);

	SetSudut(sudut);
    while(1)
    {
    	/*if(a>0)
    	{
    		ql_prv = ql;
			qr_prv = qr;

			ql = GetMotorL()*DTR;
			qr = GetMotorR()*DTR;

			wl = (ql - ql_prv)*20;
			wr = (qr - qr_prv)*20;
			a=0;
    	}*/
    	if(a==1)
    	{
    		if(t<=DURASI/PERIOD)
    		{
    			int b = t%4;
    			if(b==0) Trigger();
    			else if(b==2 && servo==1)Geser();

    			if(jalan==1) control();

				Kirim();
				t++;
    		}
    		else if(t>DURASI/PERIOD)
    		{
    			SetMotorR(100, STOP);
    			SetMotorL(100, STOP);
			}
    		a=0;
    	}

    	kiri = GetMotorL();
    	kanan = GetMotorR();
    	if(t>DURASI/PERIOD) BlueRead();
    }


}

void TIM6_DAC_IRQHandler()
{
	a=1;
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
}

void control()
{
	x_d = (x_cmd-x_init)*t/DURASI*PERIOD + x_init;
	y_d = (y_cmd-y_init)*t/DURASI*PERIOD + y_init;

	dx_d = (x_cmd - x_init)/DURASI;
	dy_d = (y_cmd - y_init)/DURASI;
	shi_d = atan2(dy_d,dx_d);
	dshi_d = (shi_d - shi_init)/DURASI;

	ddx_ref = (x_d-x)*Kp + (dx_d-dx)*Kv;
	ddy_ref = (y_d-y)*Kp + (dy_d-dy)*Kv;
	ddshi_ref = (shi_d - shi) * Kp_shi + (dshi_d - dshi) * Kv_shi;

	ddqr_ref = (cos(shi)*ddx_ref + sin(shi)*ddy_ref + W*ddshi_ref/2)/R;
	ddql_ref = (cos(shi)*ddx_ref + sin(shi)*ddy_ref - W*ddshi_ref/2)/R;

	dqr=dqr + ddqr_ref*PERIOD;
	dql=dql + ddql_ref*PERIOD;

	if(dqr>0)
	{
		SetMotorR(dqr/20.0,MAJU);
	}
	else
	{
		SetMotorR(dqr/-20.0,MUNDUR);
	}

	if(dql>0)
	{
		SetMotorL(dql/20.0,MAJU);
	}
	else
	{
		SetMotorL(dql/-20.0,MUNDUR);
	}

	ql_prv = ql;
	qr_prv = qr;

	ql = GetMotorL()*DTR;
	qr = GetMotorR()*DTR;

	wl = (ql - ql_prv)/PERIOD;
	wr = (qr - qr_prv)/PERIOD;

	dx = R/2*cos(shi)*(wr + wl);
	dy = R/2*sin(shi)*(wr + wl);
	dshi = R*(wr-wl)/W;

	v= dx*cos(shi) + dy*sin(shi);
	shi_old = shi;
	shi = (R*(qr-ql))/W + 0;
	x = x + v*cos((shi + shi_old)/2)*PERIOD;
	y = y + v*sin((shi + shi_old)/2)*PERIOD;

}

void Geser()
{
	if(dir==1) sudut+=5;
	else if(dir==0) sudut-=5;

	SetSudut(sudut);

	if(sudut==180)dir=0;
	if(sudut==0)dir=1;
}

void Kirim()
{
	uint16_t data;

	BlueSend(0xF5);

	data = t;
	BlueSend(data&0xFF);

	data = shi*RTD*50;
	BlueSend(data&0xFF);
	BlueSend((data>>8));

	data = x*1000;
	BlueSend(data&0xFF);
	BlueSend((data>>8));

	data = x_d*1000;
	BlueSend(data&0xFF);
	BlueSend((data>>8));

	data = y*1000;
	BlueSend(data&0xFF);
	BlueSend((data>>8));

	data = y_d*1000;
	BlueSend(data&0xFF);
	BlueSend((data>>8));

	jarak1 = TIM_GetCapture2(TIM1);
	jarak2 = TIM_GetCapture2(TIM15);
	BlueSend(sudut&0xFF);
	BlueSend(jarak1&0xFF);
	BlueSend((jarak1>>8));
	BlueSend(jarak2&0xFF);
	BlueSend((jarak2>>8));
}

void Trigger()
{
	GPIO_WriteBit(GPIOB , GPIO_Pin_9 , 1);
	int a=TIM_GetCounter(TIM6);
	while(TIM_GetCounter(TIM6)<(a+2500));
	GPIO_WriteBit(GPIOB , GPIO_Pin_9 , 0);
}

void BlueRead()
{
	if(USART_GetFlagStatus(USART3, USART_FLAG_RXNE))
	{
		char input = USART_ReceiveData(USART3);
		if(input==MAJU)
		{
			x_cmd += STEP;
			x_init = x;
			y_init = y;
			shi_init =shi;
			t=0;
			jalan=1;
			servo=1;
		}
		if(input==MUNDUR)
		{
			x_cmd -= STEP;
			x_init = x;
			y_init = y;
			shi_init =shi;
			t=0;
			jalan=1;
			servo=1;
		}
		if(input==KIRI)
		{
			y_cmd += STEP;
			x_init = x;
			y_init = y;
			shi_init =shi;
			t=0;
			jalan=1;
			servo=1;
		}
		if(input==KANAN)
		{
			y_cmd -= STEP;
			x_init = x;
			y_init = y;
			shi_init =shi;
			t=0;
			jalan=1;
			servo=1;
		}
		if(input==SCAN)
		{
			t=0;
			jalan=0;
			servo=1;
		}

		if(input=='a')
		{
			if(sudut<180) sudut+=5;
			SetSudut(sudut);
		}
		if(input=='d')
		{
			if(sudut>0) sudut-=5;
			SetSudut(sudut);
		}
		if(input=='s')
		{
			t=0;
			jalan=0;
			servo=0;
		}
		if(input=='c')
		{
			SetMotorR(100, STOP);
			SetMotorL(100, STOP);
		}
		USART_ClearFlag(USART3, USART_FLAG_RXNE);
	}
}
