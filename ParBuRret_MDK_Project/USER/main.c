#include "stm32f10x.h"
#include "VL53L1X_api.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "servo.h"

#define KILL PAout(7)

const uint16_t dev = 0x29;
const uint8_t INF = 0x7f;

void Kill_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 //PA �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA
}

int main(void)
{
	VL53L1X_ERROR Status;
	uint8_t state,i,j;
	
	delay_init();	    //��ʱ������ʼ��	  
	Servo_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ�
	VL53L1X_Init();
	Kill_Init();
	
	while(state)
	{
		Status = VL53L1X_BootState(dev, &state);
		delay_ms(2);
	}
	/* Sensor Initialization */
	Status = VL53L1X_SensorInit(dev);
	VL53L1X_SetROI(dev,4,4);      //Set ROI Size = 4*4
	/* enable the ranging*/
	Status = VL53L1X_StartRanging(dev);
	/* ranging loop */
	for(i=1;i<=13;i++)
	{
		for(j=1;j<=13;j++)
		{
			uint8_t st=INF;
			uint8_t dataReady;
			Status = VL53L1X_SetROICenter(dev,((i-1)<<4)+j+1);
			while(!dataReady)
			{
				Status = VL53L1X_CheckForDataReady(dev, &dataReady);
			}
			dataReady = 0;
			Status = VL53L1X_GetRangeStatus(dev,&st);
			if(!st)
			{
				if(i==7&&j==7)
				{
					KILL=1;
				}
				else
				{
					KILL=0;
				}
				if(i<7)
				{
					RotateClockwiseY();
				}
				else
				{
					RotateCounterClockwiseY();
				}
				if(j<7)
				{
					RotateClockwiseX();
				}
				else
				{
					RotateCounterClockwiseX();
				}
			}
			Status = VL53L1X_ClearInterrupt(dev);
		}
	}
}
