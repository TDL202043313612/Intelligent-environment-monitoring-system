/*-------------------------------------------------*/
/*            超纬电子STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*         实现DHT11采集温湿度功能的源文件         */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"  //包含需要的头文件
#include "dht11.h"      //包含需要的头文件 
#include "delay.h"      //包含需要的头文件
 
/*-------------------------------------------------*/
/*函数名：复位DHT11                                */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/  
void DHT11_Rst(void)	   
{
    DHT11_IO_OUT(); 	//SET OUTPUT
    DHT11_DQ_OUT=0; 	//??DQ
    Delay_Ms(20);    	//????18ms
    DHT11_DQ_OUT=1; 	//DQ=1 
    Delay_Us(30);     	//????20~40us	
//  DHT11_IO_OUT(); 	//设置IO输出模式
//  DHT11_OUT(0); 	//拉低IO
//  Delay_Ms(30);    	//拉低至少18ms，我们拉低30
//  DHT11_OUT(1); 	//拉高IO
//  Delay_Us(30);     //主机拉高20~40us，我们拉高30us
}

/*-------------------------------------------------*/
/*函数名：等待DHT11的回应                          */
/*参  数：无                                       */
/*返回值：1错误 0正确                              */
/*-------------------------------------------------*/ 
char DHT11_Check(void) 	   
{  

   u8 retry=0;
   DHT11_IO_IN();//SET INPUT	 
   while (DHT11_DQ_IN&&retry<100)//DHT11???40~80us
   {
		retry++;   
		Delay_Us(1);
   };	 
   if(retry>=100)return 1;
   else retry=0;
   while (!DHT11_DQ_IN&&retry<100)//DHT11????????40~80us
   {
       retry++;  
	   Delay_Us(1);
   };
   if(retry>=100)
	 {
		 u1_printf("DHT11初始化错误\r\n");
		 return 1;
	 }	    
   return 0;	
//	char timeout;                          //定义一个变量用于超时判断  
//	
//	timeout = 0;                           //超时变量清零    
//	DHT11_IO_IN();                         //IO设置输入模式
//    while((DHT11_DQ_IN==1)&&(timeout<70)){ //DHT11会拉低40~50us,我们等待70us超时时间	
//		timeout++;                         //超时变量+1
//		Delay_Us(1);                       //延时1us
//	} 
//	if(timeout>=70)return 1;               //如果timeout>=70,说明是因为超时退出的while循环，返回1表示错误
//	else timeout=0;                        //反之，说明是因为等到了DHT11拉低IO，退出的while循环，正确并清零timeout
//    while((DHT11_DQ_IN==0)&&(timeout<70)){ //DHT11拉低后会再次拉高40~50us,,我们等待70us超时时间	
//		timeout++;                         //超时变量+1
//		Delay_Us(1);                       //延时1us
//	}
//	if(timeout>=70)return 2;               //如果timeout>=70,说明是因为超时退出的while循环，返回2表示错误  
//	return 0;                              //反之正确，返回0
}

/*-------------------------------------------------*/
/*函数名：读取一个位                               */
/*参  数：无                                       */
/*返回值：1或0                                     */
/*-------------------------------------------------*/ 
char DHT11_Read_Bit(void) 			 
{
      u8 retry=0;
      while(DHT11_DQ_IN&&retry<100)//???????
      {
             retry++;   
             Delay_Us(1);
       }
      retry=0;
      while(!DHT11_DQ_IN&&retry<100)//??????
       {
              retry++;	
              Delay_Us(1);
         }
      Delay_Us(40);//??40us
      if(DHT11_DQ_IN)
		  return 1;
      else 
		  return 0;	
			
// 	char timeout;                            //定义一个变量用于超时判断  
//	
//	timeout=0;                               //清零timeout	
//	while((DHT11_DQ_IN==1)&&(timeout<40)){   //每一位数据开始，是12~14us的低电平，我们等40us
//		timeout++;                           //超时变量+1
//		Delay_Us(1);                         //延时1us
//	}
//	timeout=0;                               //清零timeout	
//	while((DHT11_DQ_IN==0)&&(timeout<60)){   //接下来，DHT11会拉高IO，根据拉高的时间判断是0或1，我们等60us
//		timeout++;                           //超时变量+1
//		Delay_Us(1);                         //延时1us
//	}
//	Delay_Us(35);                            //延时35us
//	if(DHT11_DQ_IN)return 1;                 //如果延时后，是高电平，那么本位接收的是1，返回1
//	else return 0;		                     //反之延时后，是低电平，那么本位接收的是0，返回0
}

/*-------------------------------------------------*/
/*函数名：读取一个字节                             */
/*参  数：无                                       */
/*返回值：数据                                     */
/*-------------------------------------------------*/ 
char DHT11_Read_Byte(void)    
{ 

    char i,dat;
    dat=0;
   for (i=0;i<8;i++) 
  {
      dat<<=1; 
     dat|=DHT11_Read_Bit();
    }				    
    return dat;	
		
//    char i;                       //定义一个变量用于for循环  
//	char dat;                     //定义一个变量用于保存数据
//   
//	dat=0;	                      //清除保存数据的变量
//	for (i=0;i<8;i++){            //一个字节8位，循环8次	
//   		dat<<=1;                  //左移一位，腾出空位    
//	    dat|=DHT11_Read_Bit();    //读取一位数据
//    }						    
//    return dat;                   //返回一个字节的数据
}

/*-------------------------------------------------*/
/*函数名：读取一次数据温湿度                       */
/*参  数：temp:温度值                              */
/*参  数：humi:湿度值                              */
/*返回值：1错误 0正确                              */
/*-------------------------------------------------*/ 
char DHT11_Read_Data(char *temp, char *humi)    
{ 
    u8 buf[5];
    u8 i;
    DHT11_Rst();
    if(DHT11_Check()==0)
   {
          for(i=0;i<5;i++)
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			*temp=buf[2];
		}
	}else return 1;
	return 0;	

// 	char buf[5];                                       //一次完整的数据有5个字节，定义一个缓冲区
//	char i;                                            //定义一个变量用于for循环  
//	
//	DHT11_Rst();                                       //复位DHT11
//	if(DHT11_Check()==0){                              //判断DHT11回复状态=0的话，表示正确，进入if
//		for(i=0;i<5;i++){                              //一次完整的数据有5个字节，循环5次		
//			buf[i]=DHT11_Read_Byte();                  //每次读取一个字节
//		}
//		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4]){     //判断数据校验，前4个字节相加应该等于第5个字节，正确的话，进入if		
//			*humi=buf[0];                              //湿度数据，保存在humi指针指向的地址变量中
//			*temp=buf[2];                              //温度数据，保存在temp指针指向的地址变量中
//		}else return 1;                                //反之，数据校验错误，直接返回1
//	}else return 2;                                    //反之，如果DHT11回复状态=1的话，表示错误，进入else，直接返回2
//	
//	return 0;	                                       //读取正确返回0    
}

/*-------------------------------------------------*/
/*函数名：初始化DHT11                              */
/*参  数：无                                       */
/*返回值：1错误 0正确                              */
/*-------------------------------------------------*/    	 
char DHT11_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruce;
	RCC_APB2PeriphClockCmd(DHT11_DQ_RCC,ENABLE);		//????PA0
	
	GPIO_InitStruce.GPIO_Pin = DHT11_DQ_PIN;
	GPIO_InitStruce.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruce.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DHT11_DQ_PORT,&GPIO_InitStruce);
 
	DHT11_Rst();
	
	return DHT11_Check();
//	GPIO_InitTypeDef GPIO_InitStructure;                    //定义一个IO端口参数结构体
//	
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE); //使能PA端口时钟
//	
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;              //准备设置PA8
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //速率50Mhz
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		//推免输出方式
//	GPIO_Init(GPIOA, &GPIO_InitStructure);            		//设置PA8	
//	DHT11_Rst();                                            //复位DHT11
//	return DHT11_Check();                                   //返回DHT11的回复状态
}
























