#include "stdbool.h"
#include "stdio.h"
#include "stdint.h"
#include "system_stm32f10x.h"
#include "ec_led.h"
#include "ec_motor.h"
#include "ec_timer.h"
#include "ec_key.h"
#include "ec_mpu6050.h"
#include "ec_usart.h"
#include "ec_delay.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "vl53l0x.h"
#include "vl53l0x_gen.h"
#include "stm32f10x_exti.h"
#include <math.h>
#include "stmflash.h"
#include "stack.h"
#include "control.h"


////计算轮子速度所用到的常量
//#define X_PARAMETER          (0.5f)               
//#define Y_PARAMETER           (sqrt(3)/2.f)   

////三个轮子的速度和传感器角度
//int Va,Vb,Vc;
//float Pitch,Roll,Yaw;



//要写入STM32 FLASH的字符串数组
const u8 TEXT_Buffer[]={"STM32 FLASH TEST"};

#define SIZE sizeof(TEXT_Buffer)	 	//数组长度
#define FLASH_SAVE_ADDR  0X08020000 	
//设置FLASH保存地址（必须为偶数，且其值要大与本代码所占用的大小+0X08000000) 0x20000就是128k


/******************************************************************************/
extern VL53L0X_Dev_t vl53l0x_dev,vl53l0x_dev2,vl53l0x_dev3;//设备I2C数据参数
/******************************************************************************/

//int absolute(int a)
//{
//	if (a<0)  return -a;
//	else return a;
//}

//void delay1(u32 time)
//{
//	for(;0<time;time--)
//	{
//	   Delay_ms(10);
//	}
//}

//void go_anywhere(float angel, int speed)
//{
//	Va = -speed*cos(angel/180*3.141592658);
//	Vb = X_PARAMETER*speed*cos(angel/180*3.1415926) - Y_PARAMETER*speed*sin(angel/180*3.1415926);
//	Vc = X_PARAMETER*speed*cos(angel/180*3.1415926) + Y_PARAMETER*speed*sin(angel/180*3.1415926);	
//}


////其他函数算得三个轮子转速，此函数只是驱动轮子转动。
//void set_direction(u16 angel,u16 speed)
//{ 
//	go_anywhere(angel, speed);
//	if (Va>0) Motor_SetCCW(1, true);else  Motor_SetCCW(1, false);
//	if (Vb>0) Motor_SetCCW(2, true);else  Motor_SetCCW(2, false);
//	if (Vc>0) Motor_SetCCW(3, true);else  Motor_SetCCW(3, false);
//	if (Va==0) Motor_Config(1, absolute(Va), false); else Motor_Config(1, absolute(Va), true);
//	if (Vb==0) Motor_Config(2, absolute(Vb), false); else Motor_Config(2, absolute(Vb), true);
//	if (Vc==0) Motor_Config(3, absolute(Vc), false); else Motor_Config(3, absolute(Vc), true);
//}


//void move_time(u16 angel, u16 speed, u16 time)
//{ 
//	Led_On(true);
//  set_direction(angel,speed);  //speed典型值600，1200
//	for(;time>0;time--)
//	{
//		 Delay_ms(10);
//	}
//	Motor_Stop(1);
//	Motor_Stop(2);
//	Motor_Stop(3);
//	Led_On(false);
//}

//void move_distance(u16 angel, u16 speed, u16 distance)
//{ 
//	u32 time;
//	Led_On(true);
//  set_direction(angel,speed);
//	time=100*distance*12000/(78*speed);  //speed典型值600，   speed=1200  速度为8cm/s 左右
//	for(;time>0;time--)
//	{
//		 Delay_ms(10);
//	}
//	Motor_Stop(1);
//	Motor_Stop(2);
//	Motor_Stop(3);
//	Led_On(false);
//}

//void go_left(u16 angel, u32 sudu)
//{ 
//	//设置初始转动方向
//	Motor_SetCCW(1, true);  //初始方向设定为正转
//	Motor_SetCCW(2, true);     
//	Motor_SetCCW(3, true);

//	Motor_Config(1, sudu, true);  //初始speed=600
//	Motor_Config(2, sudu, true);    
//  Motor_Config(3, sudu, true);
//	
//	Led_On(false);
//	delay1(5*angel/(sudu/600));

//	Led_On(true);
//	Motor_Stop(1);
//	Motor_Stop(2);
//	Motor_Stop(3);
//}

//void go_right(u16 angel, u32 speed)
//{ 
//	//设置初始转动方向
//	Motor_SetCCW(1, false);  //初始方向设定为正转
//	Motor_SetCCW(2, false);     
//	Motor_SetCCW(3, false);

//	Motor_Config(1, speed, true);  //初始speed=600
//	Motor_Config(2, speed, true);    
//  Motor_Config(3, speed, true);
//	
//	Led_On(true);
//	delay1(5*angel/(speed/600));

//	Led_On(false);
//	Motor_Stop(1);
//	Motor_Stop(2);
//	Motor_Stop(3);
//}




//u16 test_distance()
//{  
//	static char buf[VL53L0X_MAX_STRING_LENGTH];//测试模式字符串字符缓冲区
//	VL53L0X_Error Status=VL53L0X_ERROR_NONE;//工作状态
//	Status=vl53l0x_start_single_test(&vl53l0x_dev,&vl53l0x_data,buf);//执行一次测量
//	return Distance_data;
//}

//int get_distance(int num)
//{
//	int distance;
//	switch(num)
//   {
//   case 1 :
//	 vl53l0x_set_mode(&vl53l0x_dev,0);
//	VL53L0X_PerformSingleRangingMeasurement(&vl53l0x_dev,&vl53l0x_data);
//    distance = vl53l0x_data.RangeMilliMeter;
//      break;
//   case 2 :
//	vl53l0x_set_mode(&vl53l0x_dev2,0);
//    VL53L0X_PerformSingleRangingMeasurement(&vl53l0x_dev2,&vl53l0x_data2);
//    distance = vl53l0x_data2.RangeMilliMeter;
//      break; 
//    case 3 :
//	vl53l0x_set_mode(&vl53l0x_dev3,0);
//    VL53L0X_PerformSingleRangingMeasurement(&vl53l0x_dev3,&vl53l0x_data3);
//    distance = vl53l0x_data3.RangeMilliMeter;
//      break;
//   default :
//      printf("无效的传感器编号\n" );
//   }
//   return distance;

//}


//int gengsui()
//	{ 
//	int i=0;
//	test_distance();
//	set_direction(0,1200);
//  printf("1");
//	while(1)
//		{
//		if((Distance_data>150)&&(Distance_data<300))
//			{   
//		  test_distance();
//			printf("%4i",Distance_data);
//	    Delay_ms(10);
//			}
//			else
//				{
//				printf("2");
//	      delay1(330);
//	      Led_On(false);
//				Motor_Stop(1);
//	      Motor_Stop(2);
//	      Motor_Stop(3);
//				move_time(90,1200,350);
//	      Led_On(true);
//        return i;
//			  }
//	  }
//  }


//int gengsui1()
//	{ 
//	int i=0;
//	test_distance();
//	set_direction(0,1200);
//	while(1){
//		if((Distance_data>100)&&(Distance_data<300))
//			{   
//			printf("%4i",Distance_data);
//	    Delay_ms(10);
//			i++;
//			test_distance();
//			}
//			else{
//	      delay1(350);
//	      Led_On(false);
//				Motor_Stop(1);
//	      Motor_Stop(2);
//	      Motor_Stop(3);
//				
//				move_time(90,1200,350);
//	      Led_On(true);
//        return i;
//			}
//	}
//}


//void move_ahead(u16 direction ,u8 changedirection,u16 speed,u16 distance)
//{	
//	int time;
//	int t1,t2;
//	if(changedirection==1)
//  {
//		go_right(direction,600);
//		//go_left(direction,600);
//	}
//	else{		
//	//设置初始转动方向
//	set_direction(direction,speed);
//	
//	time=100*distance*12000/(78*speed);
//	//time=time/3;                 //速度固定为1200时进行的简单的校正
//	for(;time>0;time--)
//		{ 
//			Delay_ms(10);
//			test_distance();
//			while(Distance_data<200)
//			{ 
//				Led_On(false);
//				printf("%4i \r \n",Distance_data);
//		    t1=gengsui1();
//				printf("%d \r \n",t1);
//				go_left(90,600);
//				t2=gengsui1();
//				printf("%d \r \n",t2);
//				t1=t1+170;
//				move_time(90,1200,t1);

//				Motor_Stop(1);
//	      Motor_Stop(2);
//	      Motor_Stop(3);
//				
//				go_right(90,600);
//		
//				time=time-t2-700;
//				set_direction(90,1200);

//				for(;time>0;time--)
//        {
//					Delay_ms(10);
//				}
//				Motor_Stop(1);
//	      Motor_Stop(2);
//	      Motor_Stop(3);
//				break;
//			}
//    }
//	Led_On(true);
//	Motor_Stop(1);
//	Motor_Stop(2);
//	Motor_Stop(3);
//	Led_On(false);
//	}
//}
//void move_ahead1(u16 direction ,u8 changedirection,u16 speed,u16 distance)
//{	
//	int time;
//	int t1,t2;
//	if(changedirection==1)
//  {
//		//go_right(direction,600);
//		go_left(direction,600);
//	}
//	else{		
//	//设置初始转动方向
//	set_direction(direction,speed);
//	
//	time=100*distance*12000/(78*speed);
//  time=time/3;                 //速度固定为1200时进行的简单的校正
//	for(;time>0;time--)
//		{ 
//			Delay_ms(10);
//			test_distance();
//			while(Distance_data<200)
//			{ 
//				Led_On(false);
//				printf("%4i \r \n",Distance_data);
//		    t1=gengsui1();
//				printf("%d \r \n",t1);
//				go_left(90,600);
//				t2=gengsui1();
//				printf("%d \r \n",t2);
//				t1=t1+170;
//				move_time(90,1200,t1);

//				Motor_Stop(1);
//	      Motor_Stop(2);
//	      Motor_Stop(3);
//				
//				go_right(90,600);
//		
//				time=time-t2-660;
//				set_direction(90,1200);

//				for(;time>0;time--)
//        {
//					Delay_ms(10);
//				}
//				Motor_Stop(1);
//	      Motor_Stop(2);
//	      Motor_Stop(3);
//				break;
//			}
//    }
//	Led_On(true);
//	Motor_Stop(1);
//	Motor_Stop(2);
//	Motor_Stop(3);
//	Led_On(false);
//	}
//}




int main(void)
{
	u8 mode=0;
	//u8 motor_mode = 0; //电机初始停止
    u8 datatemp[SIZE];
	int d1,d2,d3;
	int t=0;
	
	Stack* directionVec = CreateStack();
  Stack* changedirectionVec=CreateStack();
	Stack* speed=CreateStack();
	Stack* distanceVec=CreateStack();
	
	Uart1_Initialize(9600);  //串口1
	Delay_Initialize();
	
	Led_Initialize();
	printf("1");
	//IIC_Init();
	printf("2");
	//MPU6050_Initialize();
	printf("4");
	Motor_Initialize();
 	printf("3");
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//分组2
	
	Delay_ms(200);
	
	
	if(vl53l0x_init(&vl53l0x_dev))     //vl53l0x初始化
	 {
		printf("VL53L0X_Init Error!!!\r\n");
		Delay_ms(200);
	 }
	 else
		 printf("VL53L0X_Init OK\r\n");
	
	if(vl53l0x_set_mode(&vl53l0x_dev,mode))   //配置测量模式
		{
			printf("Mode Set Error!!!\r\n");
		}
	else
		printf("Mode Set OK!!!\r\n");
	
		One_measurement(mode);
	
	Delay_ms(1000);
	Delay_ms(1000);


	

	if(vl53l0x_init_2(&vl53l0x_dev2))     //vl53l0x初始化
	 {
		printf("VL53L0X_Init Error!!!\r\n");
		Delay_ms(200);
	 }
	 else
		 printf("VL53L0X_Init OK\r\n");
	
	if(vl53l0x_set_mode(&vl53l0x_dev2,mode))   //配置测量模式
		{
			printf("Mode Set Error!!!\r\n");
		}
	else
		printf("Mode Set OK!!!\r\n");
	
		One_measurement2(mode);
	
	Delay_ms(1000);	
	if(vl53l0x_init_3(&vl53l0x_dev3))     //vl53l0x初始化
	 {
		printf("VL53L0X_Init Error!!!\r\n");
		Delay_ms(200);
	 }
	 else
		 printf("VL53L0X_Init OK\r\n");
	
	if(vl53l0x_set_mode(&vl53l0x_dev3,mode))   //配置测量模式
		{
			printf("Mode Set Error!!!\r\n");
		}
	else
		printf("Mode Set OK!!!\r\n");
	
		One_measurement3(mode);
	vl53l0x_info();
	vl53l0x_info2();
	vl53l0x_info3();
	
//	while(1)
//	{
//		One_measurement(mode);
//		One_measurement2(mode);
//		One_measurement3(mode);
//	}
	

	
//	while(1)
//	{
//		d1 = get_distance(1);
//		d2 = get_distance(2);
//		printf("\r\n d1: %4d mm.\r\n",d1);
//		printf("\r\n d2: %4d mm.\r\n",d2);
//		Delay_ms(1000);
//	}
//	
	
	
	
  
	Motor_EnableDriver(true);

  move_ahead(90,0,1200,150);
	move_ahead(90,1,600,110);
	move_ahead(90,0,1200,50);
	
  //move_ahead(90,1,600,210);
	//move_ahead(90,0,1200,220);
	
  PushStack(directionVec,90); 
  PushStack(directionVec,90);
  PushStack(directionVec,90);
	//PushStack(directionVec,90);
  //PushStack(directionVec,90);
	
	PushStack(changedirectionVec,0); 
	PushStack(changedirectionVec,1); 
	PushStack(changedirectionVec,0); 
	//PushStack(changedirectionVec,1);
	//PushStack(changedirectionVec,0);
	
	PushStack(speed,1200); 
	PushStack(speed,600); 
	PushStack(speed,1200);
	//PushStack(speed,600); 
	//PushStack(speed,1200);
	
	PushStack(distanceVec,150); 
	PushStack(distanceVec,110); 
	PushStack(distanceVec,50);
	//PushStack(distanceVec,210); 
	//PushStack(distanceVec,220); 
	
	go_right(180,600);
  
 for(;t<3;t++)
  { 
		printf("33");
		move_ahead1(GetTopElement(directionVec),GetTopElement(changedirectionVec),GetTopElement(speed),GetTopElement(distanceVec));
	  
		printf("%d \r\n",GetTopElement(directionVec));
		printf("%d \r\n",GetTopElement(changedirectionVec));
		printf("%d \r\n",GetTopElement(speed));
		printf("%d \r\n",GetTopElement(distanceVec));
		
		PopStack(directionVec);
		PopStack(changedirectionVec);
		PopStack(speed);
		PopStack(distanceVec);
	}
 
  
	
	DestoryStack(directionVec);
	DestoryStack(changedirectionVec);
	DestoryStack(speed);
	DestoryStack(distanceVec);
	
	//flash测试
	STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)TEXT_Buffer,SIZE);
	Led_On(false);
	STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)datatemp,SIZE);
	printf("%s",datatemp);
 
}




