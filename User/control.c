#include "vl53l0x.h"
#include "vl53l0x_gen.h"
#include "stack.h"
#include "stm32f10x_exti.h"
#include <math.h>
#include "ec_motor.h"
#include "ec_led.h"
#include "stdint.h"
#include "ec_delay.h"

//���������ٶ����õ��ĳ���
#define X_PARAMETER          (0.5f)               
#define Y_PARAMETER           (sqrt(3)/2.f)   

//�������ӵ��ٶȺʹ������Ƕ�
int Va,Vb,Vc;
float Pitch,Roll,Yaw;



/******************************************************************************/
extern VL53L0X_Dev_t vl53l0x_dev,vl53l0x_dev2,vl53l0x_dev3;//�豸I2C���ݲ���
/******************************************************************************/

int absolute(int a)
{
	if (a<0)  return -a;
	else return a;
}

void delay1(u32 time)
{
	for(;0<time;time--)
	{
	   Delay_ms(10);
	}
}

void go_anywhere(float angel, int speed)
{
	Va = -speed*cos(angel/180*3.141592658);
	Vb = X_PARAMETER*speed*cos(angel/180*3.1415926) - Y_PARAMETER*speed*sin(angel/180*3.1415926);
	Vc = X_PARAMETER*speed*cos(angel/180*3.1415926) + Y_PARAMETER*speed*sin(angel/180*3.1415926);	
}


//�������������������ת�٣��˺���ֻ����������ת����
void set_direction(u16 angel,u16 speed)
{ 
	go_anywhere(angel, speed);
	if (Va>0) Motor_SetCCW(1, true);else  Motor_SetCCW(1, false);
	if (Vb>0) Motor_SetCCW(2, true);else  Motor_SetCCW(2, false);
	if (Vc>0) Motor_SetCCW(3, true);else  Motor_SetCCW(3, false);
	if (Va==0) Motor_Config(1, absolute(Va), false); else Motor_Config(1, absolute(Va), true);
	if (Vb==0) Motor_Config(2, absolute(Vb), false); else Motor_Config(2, absolute(Vb), true);
	if (Vc==0) Motor_Config(3, absolute(Vc), false); else Motor_Config(3, absolute(Vc), true);
}


void move_time(u16 angel, u16 speed, u16 time)
{ 
	Led_On(true);
  set_direction(angel,speed);  //speed����ֵ600��1200
	for(;time>0;time--)
	{
		 Delay_ms(10);
	}
	Motor_Stop(1);
	Motor_Stop(2);
	Motor_Stop(3);
	Led_On(false);
}

void move_distance(u16 angel, u16 speed, u16 distance)
{ 
	u32 time;
	Led_On(true);
  set_direction(angel,speed);
	time=100*distance*12000/(78*speed);  //speed����ֵ600��   speed=1200  �ٶ�Ϊ8cm/s ����
	for(;time>0;time--)
	{
		 Delay_ms(10);
	}
	Motor_Stop(1);
	Motor_Stop(2);
	Motor_Stop(3);
	Led_On(false);
}

void go_left(u16 angel, u32 sudu)
{ 
	//���ó�ʼת������
	Motor_SetCCW(1, true);  //��ʼ�����趨Ϊ��ת
	Motor_SetCCW(2, true);     
	Motor_SetCCW(3, true);

	Motor_Config(1, sudu, true);  //��ʼspeed=600
	Motor_Config(2, sudu, true);    
  Motor_Config(3, sudu, true);
	
	Led_On(false);
	delay1(5*angel/(sudu/600));

	Led_On(true);
	Motor_Stop(1);
	Motor_Stop(2);
	Motor_Stop(3);
}

void go_right(u16 angel, u32 speed)
{ 
	//���ó�ʼת������
	Motor_SetCCW(1, false);  //��ʼ�����趨Ϊ��ת
	Motor_SetCCW(2, false);     
	Motor_SetCCW(3, false);

	Motor_Config(1, speed, true);  //��ʼspeed=600
	Motor_Config(2, speed, true);    
  Motor_Config(3, speed, true);
	
	Led_On(true);
	delay1(5*angel/(speed/600));

	Led_On(false);
	Motor_Stop(1);
	Motor_Stop(2);
	Motor_Stop(3);
}




u16 test_distance()
{  
	static char buf[VL53L0X_MAX_STRING_LENGTH];//����ģʽ�ַ����ַ�������

	vl53l0x_start_single_test(&vl53l0x_dev,&vl53l0x_data,buf);//ִ��һ�β���
	return Distance_data;
}

int get_distance(int num)
{
	int distance;
	switch(num)
   {
   case 1 :
	 vl53l0x_set_mode(&vl53l0x_dev,0);
	VL53L0X_PerformSingleRangingMeasurement(&vl53l0x_dev,&vl53l0x_data);
    distance = vl53l0x_data.RangeMilliMeter;
      break;
   case 2 :
	vl53l0x_set_mode(&vl53l0x_dev2,0);
    VL53L0X_PerformSingleRangingMeasurement(&vl53l0x_dev2,&vl53l0x_data2);
    distance = vl53l0x_data2.RangeMilliMeter;
      break; 
    case 3 :
	vl53l0x_set_mode(&vl53l0x_dev3,0);
    VL53L0X_PerformSingleRangingMeasurement(&vl53l0x_dev3,&vl53l0x_data3);
    distance = vl53l0x_data3.RangeMilliMeter;
      break;
   default :
      printf("��Ч�Ĵ��������\n" );
   }
   return distance;

}


int gengsui()
	{ 
	int i=0;
	test_distance();
	set_direction(0,1200);
  printf("1");
	while(1)
		{
		if((Distance_data>150)&&(Distance_data<300))
			{   
		  test_distance();
			printf("%4i",Distance_data);
	    Delay_ms(10);
			}
			else
				{
				printf("2");
	      delay1(330);
	      Led_On(false);
				Motor_Stop(1);
	      Motor_Stop(2);
	      Motor_Stop(3);
				move_time(90,1200,350);
	      Led_On(true);
        return i;
			  }
	  }
  }


int gengsui1()
	{ 
	int i=0;
	test_distance();
	set_direction(0,1200);
	while(1){
		if((Distance_data>100)&&(Distance_data<300))
			{   
			printf("%4i",Distance_data);
	    Delay_ms(10);
			i++;
			test_distance();
			}
			else{
	      delay1(350);
	      Led_On(false);
				Motor_Stop(1);
	      Motor_Stop(2);
	      Motor_Stop(3);
				
				move_time(90,1200,350);
	      Led_On(true);
        return i;
			}
	}
}


void move_ahead(u16 direction ,u8 changedirection,u16 speed,u16 distance)
{	
	int time;
	int t1,t2;
	if(changedirection==1)
  {
		go_right(direction,600);
		//go_left(direction,600);
	}
	else{		
	//���ó�ʼת������
	set_direction(direction,speed);
	
	time=100*distance*12000/(78*speed);
	//time=time/3;                 //�ٶȹ̶�Ϊ1200ʱ���еļ򵥵�У��
	for(;time>0;time--)
		{ 
			Delay_ms(10);
			test_distance();
			while(Distance_data<200)
			{ 
				Led_On(false);
				printf("%4i \r \n",Distance_data);
		    t1=gengsui1();
				printf("%d \r \n",t1);
				go_left(90,600);
				t2=gengsui1();
				printf("%d \r \n",t2);
				t1=t1+170;
				move_time(90,1200,t1);

				Motor_Stop(1);
	      Motor_Stop(2);
	      Motor_Stop(3);
				
				go_right(90,600);
		
				time=time-t2-700;
				set_direction(90,1200);

				for(;time>0;time--)
        {
					Delay_ms(10);
				}
				Motor_Stop(1);
	      Motor_Stop(2);
	      Motor_Stop(3);
				break;
			}
    }
	Led_On(true);
	Motor_Stop(1);
	Motor_Stop(2);
	Motor_Stop(3);
	Led_On(false);
	}
}
void move_ahead1(u16 direction ,u8 changedirection,u16 speed,u16 distance)
{	
	int time;
	int t1,t2;
	if(changedirection==1)
  {
		//go_right(direction,600);
		go_left(direction,600);
	}
	else{		
	//���ó�ʼת������
	set_direction(direction,speed);
	
	time=100*distance*12000/(78*speed);
  time=time/3;                 //�ٶȹ̶�Ϊ1200ʱ���еļ򵥵�У��
	for(;time>0;time--)
		{ 
			Delay_ms(10);
			test_distance();
			while(Distance_data<200)
			{ 
				Led_On(false);
				printf("%4i \r \n",Distance_data);
		    t1=gengsui1();
				printf("%d \r \n",t1);
				go_left(90,600);
				t2=gengsui1();
				printf("%d \r \n",t2);
				t1=t1+170;
				move_time(90,1200,t1);

				Motor_Stop(1);
	      Motor_Stop(2);
	      Motor_Stop(3);
				
				go_right(90,600);
		
				time=time-t2-660;
				set_direction(90,1200);

				for(;time>0;time--)
        {
					Delay_ms(10);
				}
				Motor_Stop(1);
	      Motor_Stop(2);
	      Motor_Stop(3);
				break;
			}
    }
	Led_On(true);
	Motor_Stop(1);
	Motor_Stop(2);
	Motor_Stop(3);
	Led_On(false);
	}
}