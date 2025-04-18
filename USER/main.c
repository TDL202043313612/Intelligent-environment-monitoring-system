/*-----------------------------------------------------*/
/*              超纬电子STM32系列开发板                */
/*-----------------------------------------------------*/
/*                     程序结构                        */
/*-----------------------------------------------------*/
/*USER     ：包含程序的main函数，是整个程序的入口      */
/*HARDWARE ：包含开发板各种功能外设的驱动程序          */
/*CORE     ：包含STM32的核心程序，官方提供，我们不修改 */
/*STLIB    ：官方提供的库文件，我们不修改              */
/*-----------------------------------------------------*/
/*                                                     */
/*           程序main函数，入口函数源文件              */
/*                                                     */
/*-----------------------------------------------------*/

#include "stm32f10x.h"  //包含需要的头文件
#include "main.h"       //包含需要的头文件
#include "delay.h"      //包含需要的头文件
#include "usart1.h"     //包含需要的头文件
#include "usart2.h"     //包含需要的头文件
#include "timer1.h"     //包含需要的头文件
#include "timer2.h"     //包含需要的头文件
#include "timer3.h"     //包含需要的头文件
#include "timer4.h"     //包含需要的头文件
#include "wifi.h"	    //包含需要的头文件
#include "led.h"        //包含需要的头文件
#include "mqtt.h"       //包含需要的头文件
#include "key.h"        //包含需要的头文件
#include "dht11.h"      //包含需要的头文件
#include "adc.h"        //包含需要的头文件
#include "oled.h"        //包含需要的头文件
float temp_al=0;
float hui_al=0;
float smo_al=0;
int auto_flag=0;
int adc;
float volt;
int LED1_Flog=0;
int LED2_Flog=0;

int main(void) 
{
	int i=0;	
	Delay_Init();                   //延时功能初始化              
	Usart1_Init(9600);              //串口1功能初始化，波特率9600
	Usart2_Init(115200);            //串口2功能初始化，波特率115200	
	TIM4_Init(300,7200);            //TIM4初始化，定时时间 300*7200*1000/72000000 = 30ms
	OLED_Init();
	OLED_Clear();
	LED_Init();	                    //LED初始化
	KEY_Init();                     //按键初始化
	DHT11_Init();                   //初始化DHT11
	Adc_Init();
	if(Get_Adc_Average(0,10)>500)
	{
			u1_printf("MQ-135初始化错误\r\n");
			Delay_Ms(400);
	}
	
	WiFi_ResetIO_Init();            //初始化WiFi的复位IO
    MQTT_Buff_Init();               //初始化接收,发送,命令数据的 缓冲区 以及各状态参数
	AliIoT_Parameter_Init();	    //初始化连接阿里云IoT平台MQTT服务器的参数
	OLED_Dis_Menu();	
	while(1)                        //主循环
	{

			
		/*--------------------------------------------------------------------*/
		/*   Connect_flag=1同服务器建立了连接,我们可以发布数据和接收推送了    */
		/*--------------------------------------------------------------------*/
		if(Connect_flag==1){     
			/*-------------------------------------------------------------*/
			/*                     处理发送缓冲区数据                      */
			/*-------------------------------------------------------------*/
				if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr){                //if成立的话，说明发送缓冲区有数据了
				//3种情况可进入if
				//第1种：0x10 连接报文
				//第2种：0x82 订阅报文，且ConnectPack_flag置位，表示连接报文成功
				//第3种：SubcribePack_flag置位，说明连接和订阅均成功，其他报文可发
				if((MQTT_TxDataOutPtr[2]==0x10)||((MQTT_TxDataOutPtr[2]==0x82)&&(ConnectPack_flag==1))||(SubcribePack_flag==1)){    
					u1_printf("发送数据:0x%x\r\n",MQTT_TxDataOutPtr[2]);  //串口提示信息
					MQTT_TxData(MQTT_TxDataOutPtr);                       //发送数据
					MQTT_TxDataOutPtr += BUFF_UNIT;                       //指针下移
					if(MQTT_TxDataOutPtr==MQTT_TxDataEndPtr)              //如果指针到缓冲区尾部了
						MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];            //指针归位到缓冲区开头
				} 				
			}//处理发送缓冲区数据的else if分支结尾
			
			/*-------------------------------------------------------------*/
			/*                     处理接收缓冲区数据                      */
			/*-------------------------------------------------------------*/
			if(MQTT_RxDataOutPtr != MQTT_RxDataInPtr){  //if成立的话，说明接收缓冲区有数据了														
				u1_printf("接收到数据:");
				/*-----------------------------------------------------*/
				/*                    处理CONNACK报文                  */
				/*-----------------------------------------------------*/				
				//if判断，如果第一个字节是0x20，表示收到的是CONNACK报文
				//接着我们要判断第4个字节，看看CONNECT报文是否成功
				if(MQTT_RxDataOutPtr[2]==0x20){             			
				    switch(MQTT_RxDataOutPtr[5]){					
						case 0x00 : u1_printf("CONNECT报文成功\r\n");                            //串口输出信息	
								    ConnectPack_flag = 1;                                        //CONNECT报文成功，订阅报文可发
									break;                                                       //跳出分支case 0x00                                              
						case 0x01 : u1_printf("连接已拒绝，不支持的协议版本，准备重启\r\n");     //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接
									break;                                                       //跳出分支case 0x01   
						case 0x02 : u1_printf("连接已拒绝，不合格的客户端标识符，准备重启\r\n"); //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接
									break;                                                       //跳出分支case 0x02 
						case 0x03 : u1_printf("连接已拒绝，服务端不可用，准备重启\r\n");         //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接
									break;                                                       //跳出分支case 0x03
						case 0x04 : u1_printf("连接已拒绝，无效的用户名或密码，准备重启\r\n");   //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接						
									break;                                                       //跳出分支case 0x04
						case 0x05 : u1_printf("连接已拒绝，未授权，准备重启\r\n");               //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接						
									break;                                                       //跳出分支case 0x05 		
						default   : u1_printf("连接已拒绝，未知状态，准备重启\r\n");             //串口输出信息 
									Connect_flag = 0;                                            //Connect_flag置零，重启连接					
									break;                                                       //跳出分支case default 								
					}				
				}			
				//if判断，第一个字节是0x90，表示收到的是SUBACK报文
				//接着我们要判断订阅回复，看看是不是成功
				else if(MQTT_RxDataOutPtr[2]==0x90){ 
						switch(MQTT_RxDataOutPtr[6]){					
						case 0x00 :
						case 0x01 : u1_printf("订阅成功\r\n");            //串口输出信息
							        SubcribePack_flag = 1;                //SubcribePack_flag置1，表示订阅报文成功，其他报文可发送
									Ping_flag = 0;                        //Ping_flag清零
   								    TIM3_ENABLE_20S();                    //启动30s的PING定时器
									TIM2_ENABLE_10S();                    //启动30s的上传数据的定时器
						u1_printf("1111\r\n"); 
						            TempHumi_State();                     //先发一次数据
									break;                                //跳出分支                                             
						default   : 
											u1_printf("订阅成功\r\n");            //串口输出信息
							        SubcribePack_flag = 1;                //SubcribePack_flag置1，表示订阅报文成功，其他报文可发送
											Ping_flag = 0;                        //Ping_flag清零
											TIM2_ENABLE_10S();                    //启动10s的上传数据的定时器
   								    TIM3_ENABLE_20S();                    //启动20s的PING定时器
											    
											TempHumi_State();
									break;
						                               //跳出分支 								
					}					
				}
				//if判断，第一个字节是0xD0，表示收到的是PINGRESP报文
				else if(MQTT_RxDataOutPtr[2]==0xD0){ 
					u1_printf("PING报文回复\r\n"); 		  //串口输出信息 
					if(Ping_flag==1){                     //如果Ping_flag=1，表示第一次发送
						 Ping_flag = 0;    				  //要清除Ping_flag标志
					}else if(Ping_flag>1){ 				  //如果Ping_flag>1，表示是多次发送了，而且是2s间隔的快速发送
						Ping_flag = 0;     				  //要清除Ping_flag标志
						TIM3_ENABLE_20S(); 				  //PING定时器重回30s的时间
					}				
				}	
				//if判断，如果第一个字节是0x30，表示收到的是服务器发来的推送数据
				//我们要提取控制命令
				else if((MQTT_RxDataOutPtr[2]==0x30)){ 
					u1_printf("服务器等级0推送\r\n"); 		   //串口输出信息 
					MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);  //处理等级0推送数据
				}				
								
				MQTT_RxDataOutPtr += BUFF_UNIT;                     //指针下移
				if(MQTT_RxDataOutPtr==MQTT_RxDataEndPtr)            //如果指针到缓冲区尾部了
					MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];          //指针归位到缓冲区开头                        
			}//处理接收缓冲区数据的else if分支结尾
			
			/*-------------------------------------------------------------*/
			/*                     处理命令缓冲区数据                      */
			/*-------------------------------------------------------------*/
			if(MQTT_CMDOutPtr != MQTT_CMDInPtr){                             //if成立的话，说明命令缓冲区有数据了			       
				u1_printf("命令:%s\r\n",&MQTT_CMDOutPtr[2]);                 //串口输出信息
				auto_flag = (int)find(&MQTT_CMDOutPtr[2],0);
				if(auto_flag){/*0：手动模式 1：自动模式*/
					temp_al=find(&MQTT_CMDOutPtr[2],1);
					hui_al=find(&MQTT_CMDOutPtr[2],2);
					smo_al=find(&MQTT_CMDOutPtr[2],3);
					u1_printf("tem=%0.2f,hui=%0.2f,smo=%0.2f\r\n",temp_al,hui_al,smo_al);
					TIM4_ENABLE_2S();
				}else{
					TIME4_DISABLE();
				}
				MQTT_CMDOutPtr += BUFF_UNIT;                             	 //指针下移
				if(MQTT_CMDOutPtr==MQTT_CMDEndPtr)           	             //如果指针到缓冲区尾部了
					MQTT_CMDOutPtr = MQTT_CMDBuf[0];          	             //指针归位到缓冲区开头				
			}//处理命令缓冲区数据的else if分支结尾	
		}//Connect_flag=1的if分支的结尾
		
		/*--------------------------------------------------------------------*/
		/*      Connect_flag=0同服务器断开了连接,我们要重启连接服务器         */
		/*--------------------------------------------------------------------*/
		else{ 
			u1_printf("需要连接服务器\r\n");                 //串口输出信息
			TIM_Cmd(TIM4,DISABLE);                           //关闭TIM4 
			TIM_Cmd(TIM3,DISABLE);                           //关闭TIM3  
			WiFi_RxCounter=0;                                //WiFi接收数据量变量清零                        
			memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);          //清空WiFi接收缓冲区 
			if(WiFi_Connect_IoTServer()==0){   			     //如果WiFi连接云服务器函数返回0，表示正确，进入if
				u1_printf("建立TCP连接成功\r\n");            //串口输出信息
				Connect_flag = 1;                            //Connect_flag置1，表示连接成功	
				WiFi_RxCounter=0;                            //WiFi接收数据量变量清零                        
				memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);      //清空WiFi接收缓冲区 
				MQTT_Buff_ReInit();                          //重新初始化发送缓冲区                    
			}				
		}
	}
}
/*-------------------------------------------------*/
/*函数名：采集温湿度，并发布给服务器               */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TempHumi_State(void)
{
	char humidity;           //定义一个变量，保存湿度值
	char temperature;        //定义一个变量，保存温度值
	char temp[256];                  
	
	DHT11_Read_Data(&temperature,&humidity);	               //读取温湿度值
	adc =	Get_Adc_Average(ADC_Channel_8,10);
	volt = adc*3.3/4096;
	volt = volt *91;
	u1_printf("温度：%d  湿度：%d  烟雾浓度：%0.3f\r\n",temperature,humidity,volt);  //串口输出信息
	//sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302322\",\"params\":{\"CurrentHumidity\":%d,\"CurrentTemperature\":%d},\"version\":\"1.0.0\"}",humidity,temperature);  //构建回复湿度温度数据
	sprintf(temp,"%d,%d,%0.3f",temperature,humidity,volt);  //构建回复湿度温度数据
	MQTT_PublishQs0(P_TOPIC_NAME,temp,strlen(temp));   //添加数据，发布给服务器	
	
	OLED_Dis_DHT((u8)temperature,(u8)humidity,(u8)volt);
	
	if(auto_flag){/*1：自动 0：手动判断当前是自动模式还是手动模式*/
		if(temperature > temp_al || humidity > hui_al || volt > smo_al){
			if(temperature > temp_al){/*打开风扇*/
					LED1_ON;
				LED1_Flog=1;
			}else{
				LED1_OFF;
				LED1_Flog=0;
			}
			if(volt > smo_al){/*打开排气*/
				LED2_ON;
				LED2_Flog=1;				
			}else{
				LED2_OFF;
				LED2_Flog=0;
			}
			if(humidity > hui_al){/*打开风扇和排气*/
				LED1_ON;
				LED2_ON;
				LED1_Flog=1;
				LED2_Flog=1;
			}

		}else{
			LED1_OFF;
			LED2_OFF;
			LED1_Flog=0;
			LED2_Flog=0;
		}
		
	}else{/*手动模式*/
		
	}
			

}

float find(unsigned char *p,int num)
{
	float number=0,fpoint=0;
	int flag=0,i,num_point=0;
	for(i=0;i<num;i++){
		for(;*p&&*p!=',';p++){}
		if(*p)p++;
	}
	for(;*p&&*p!=',';p++){
		if(*p=='.'){
			flag=1;
			p++;
		}
		if(flag){/*小数*/
			num_point++;
			fpoint= (*p-'0');
			for(i=0;i<num_point;i++){
				fpoint= fpoint/10;
				
			}
			u1_printf("%0.2f\r\n",fpoint);
			number += fpoint;
			continue;
		}
		number = number*10 + (*p-'0');
	}
	return number;
}


