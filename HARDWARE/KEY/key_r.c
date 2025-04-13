/*************************************************************************


 **************************************************************************/

#include "key_r.h"
#include "delay.h"
//#include "include.h"

static KeyAction_Struct_t KeyAction = {KEY_ID_NONE ,KEY_STATE_INIT ,KEY_EVENT_NONE ,0,0,0};
	     
     
u8 GetKeyCode(void);




/****************************************************
  * @brief  按键端口初始化
  * @param  None
  * @retval None
****************************************************/
void KEY_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
 	
	 RCC_APB2PeriphClockCmd(KEY1_RCC, ENABLE);	 //使能PB,PE端口时钟
		
	 GPIO_InitStructure.GPIO_Pin = KEY1_PIN;				 //LED0-->PB.5 端口配置
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	 GPIO_Init(KEY1_PORT, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
	 GPIO_SetBits(KEY1_PORT,KEY1_PIN);						 //PA.5 输出高
	
#ifdef KEY2
//配置KEY1为上拉输入
    
#endif

#ifdef KEY3
//配置KEY3为上拉输入
   
#endif

#ifdef KEY4
//配置KEY4为上拉输入
    
#endif

#ifdef KEY5
//配置KEY5为上拉输入
   
#endif
}
/****************************************************
  * @brief  按键扫描
  * @param  None
  * @retval 键值及按键属性
****************************************************/
u8 GetKeyCode(void)   
{
	if(0 == KEY1_IO()) return KEY_ID_1;    // 低电平有效
	
#ifdef KEY2
	
#endif

#ifdef KEY3
	
#endif

#ifdef KEY4
	
#endif

#ifdef KEY5
	
#endif
    return KEY_ID_NONE;
}
/*
*********************************************************************************************************
*                                    Key_GetValue()
*
* Description : Get pressed Key's ID,and the first event key's event and status.
*
* Argument(s) : None.
*
* Return(s)   : @ Description.
*
* Note(s)     : (1) wait to update to support for getting each key's event and status independently.
*				(2) KeyState = keyval & 0xE000,KeyID = keyval & 0x01F8,KeyEvent = keyval & 0x0007.
*				(3) the time for Jitter and others is based on set Timestamp period to 1 ms (@bsp_timer)
*********************************************************************************************************
*/

u16 Key_GetValue(void)
{
    static u32 Continuou_i=0;            //长按键持续触发计数
    u32 temp;
	u16 KeyValue = (u16)KEY_EVENT_NONE | (u16)KEY_ID_NONE;    
	KeyID_Enum_t KeyIDTmp = (KeyID_Enum_t)GetKeyCode(); 
	
	switch(KeyAction.KeyState)
	{
		case KEY_STATE_INIT:         //检测是否有第一次按键按下 
		{
			if(KeyIDTmp != KEY_ID_NONE )
			{
				//printf("按键按下\r\n");
				KeyAction.KeyState = KEY_STATE_JITTER_1 ;
				KeyAction.KeyJitterTime = GetTime();
			}
			break;
		}
		case KEY_STATE_JITTER_1:     //识别第一次按键是否为抖动
		{
			temp = Delta_T(KeyAction.KeyJitterTime);//带溢出的时间差计算
            if(temp > KeyTime_Jitter)
			{
				if(KeyIDTmp != KEY_ID_NONE)  //非抖动 进入按键按键类型识别 
				{
					//printf("按键识别\r\n");
					KeyAction .KeyState = KEY_STATE_PRESSED_1 ;
					KeyAction .KeyPressedTime = GetTime();
                    KeyAction .KeyID = KeyIDTmp;
                    KeyValue = (u16)KEY_EVENT_PRESSED | (u16)KeyAction .KeyID;  //该键值触发一次只返回一次
				}
				else                //错误信号 直接返回初始状态
				{
					//printf("按键识别出错\r\n");
					KeyAction .KeyState = KEY_STATE_INIT ;
                    KeyAction .KeyID = KEY_ID_NONE;
				}
			}
			break;
		}
		case KEY_STATE_PRESSED_1:    //第一次按键按下：识别长按键、按键松开
		{
			temp = Delta_T(KeyAction.KeyPressedTime);//带溢出的时间差计算   为什么要加1？？？
			//printf("temp = %d\r\n",temp);
            if(temp > KeyTime_Long)//长按键
			{
				
                if(KeyIDTmp  == KEY_ID_NONE )       //长按键释放
				{
					//printf("长按释放\r\n");
                    KeyValue = (u16)KEY_EVENT_LONG_RELEASE | (u16)KeyAction.KeyID;  //该键值触发一次只返回一次
                    KeyAction.KeyState = KEY_STATE_INIT ; 
                    KeyAction.KeyID = KEY_ID_NONE;
                    Continuou_i = 0;
                }
				else                               //长按键未释放
                {
             		if(temp>(KeyTime_Long + Continuou_i))
                    {
						//printf("长按未释放\r\n");
                        Continuou_i += KeyTime_Continuou;
                        KeyValue = (u16)KEY_EVENT_LONG_PRESSED | (u16)KeyAction.KeyID;  //该键值触发后将持续返回
                    }
                }
			}
            else if(KeyIDTmp == KEY_ID_NONE )       //短按键释放了
			{
				//printf("短按释放\r\n");
				KeyAction.KeyState = KEY_STATE_RELEASE_1 ;   //进入短按键识别
				KeyAction.KeyReleasedTime = GetTime ();
			}
			break;
		}
		case KEY_STATE_RELEASE_1 :   //第一次按键释放 ：识别单击健、第二次按键触发
		{
#if KEY_DOUBLE_EN
            temp = Delta_T(KeyAction.KeyReleasedTime);//带溢出的时间差计算     
			if(KeyIDTmp != KEY_ID_NONE )          //检测到第二次按键按下
			{
				KeyAction.KeyJitterTime = GetTime();
				KeyAction.KeyState = KEY_STATE_JITTER_2;
			}
			else if(temp > KeyTime_Double) //已经过了双击键检测时间，判断为单击健 
			{
				KeyValue = (u16)KEY_EVENT_SHORT | (u16)KeyAction.KeyID;   //该键值触发一次只返回一次
                KeyAction.KeyState = KEY_STATE_INIT;
                KeyAction.KeyID = KEY_ID_NONE;
			}
#else
            KeyValue = (u16)KEY_EVENT_SHORT | (u16)KeyAction.KeyID ;   //该键值触发一次只返回一次
            KeyAction.KeyState = KEY_STATE_INIT ;
            KeyAction.KeyID = KEY_ID_NONE;
#endif            
            break;
        }
#if KEY_DOUBLE_EN		
		case KEY_STATE_JITTER_2:    //识别第二次按键是否为抖动
		{ 
            temp = Delta_T(KeyAction.KeyJitterTime);//带溢出的时间差计算       
			if(temp > KeyTime_Jitter)
			{
				if(KeyIDTmp != KEY_ID_NONE)  //非抖动 进入第二次按键类型识别 
				{
					KeyAction .KeyState = KEY_STATE_PRESSED_2;
					KeyAction .KeyPressedTime = GetTime();
                    KeyAction .KeyID = KeyIDTmp;}
				else                        //错误信号 返回单击健
				{
					KeyValue = (u16)KEY_EVENT_SHORT | (u16)KeyAction.KeyID;   //该键值触发一次只返回一次
                    KeyAction.KeyState = KEY_STATE_INIT;
                    KeyAction.KeyID = KEY_ID_NONE;
				}
			}
            break;
		}
        case KEY_STATE_PRESSED_2:  //第二次按键按下：识别长按键、双击键
        { 
            temp = Delta_T(KeyAction.KeyPressedTime);//带溢出的时间差计算         
            if(temp > KeyTime_Long)//短_长按键
				{
				if(KeyIDTmp  == KEY_ID_NONE )       //短_长按键释放
				{
                    KeyValue = (u16)KEY_EVENT_SHORT_LONG_RELEASE | (u16)KeyAction.KeyID;  //该键值触发一次只返回一次
                    KeyAction.KeyState = KEY_STATE_INIT ; 
                    KeyAction.KeyID = KEY_ID_NONE;
                    Continuou_i = 0;
                }                
				else                               //短_长按键未释放
                {
                    if(temp >(KeyTime_Long + Continuou_i))
                    {
                        Continuou_i += KeyTime_Continuou;
                        KeyValue = (u16)KEY_EVENT_SHORT_LONG_PRESSED | (u16)KeyAction.KeyID;  //该键值触发后将持续返回
                    }
                }
                    
			}
            else if(KeyIDTmp == KEY_ID_NONE )       //双击键按键释放了
			{
				KeyValue = (u16)KEY_EVENT_DOUBLE_SHORT | (u16)KeyAction.KeyID ;   //该键值触发一次只返回一次
                KeyAction.KeyState = KEY_STATE_INIT ;
                KeyAction.KeyID = KEY_ID_NONE;
			}
            break;
        }
		case KEY_STATE_RELEASE_2:  // 双击键 释放(不检测三击按键，该状态不需要)
		{
            
            break;
		}
#endif 		
		default :
            KeyAction.KeyState = KEY_STATE_INIT;
            KeyAction.KeyID = KEY_ID_NONE;
            KeyAction.KeyJitterTime = 0;
            KeyAction.KeyPressedTime = 0;
            KeyAction.KeyReleasedTime = 0;
            KeyAction.KeyEvent = KEY_EVENT_NONE;
            Continuou_i = 0;
			break;
	}
	return KeyValue ;
}
/****************************************************
  * @brief  等待按键释放
  * @param  None
  * @retval None
****************************************************/
void WaitKyeRelease(void)
{
//    while(!(KeyAction.KeyState == KEY_STATE_INIT))
//    {
//        Key_GetValue();
//    }
	while(KEY1_IO() == 0);
}

//*********************按键处理函数模板**********************************

//
//void DealKeyVal(u16 KeyVal)
//{
//    u16  KeyEvent  = KeyVal&KEYEVENTMASK;   //按键事件
//    u16  CurKeyVal = KeyVal&KEYIDMASK;      //当前键值
//    switch(KeyEvent)
//    {
//        
//        case KEY_EVENT_PRESSED:  // 按键按下
//        {
//            Uart2Print("\r\n按键按下 = 0X%04x\r\n",KeyVal);
//            switch(CurKeyVal) 
//            {
//                case KEY_ID_1:
//                {
//                    
//                    break;
//                }
//                default:
//                    break;
//            }
//            break;
//        }
//        case KEY_EVENT_SHORT:    // 单击健
//        {
//            Uart2Print("\r\n单击健 = 0X%04x\r\n",KeyVal);
//            switch(CurKeyVal) 
//            {
//                case KEY_ID_1:
//                {
//                    
//                    break;
//                }
//                default:
//                    break;
//            }
//            break;
//        }
//        case KEY_EVENT_LONG_PRESSED:  //长按键未释放
//        {
//            Uart2Print("\r\n长按键未释放 = 0X%04x\r\n",KeyVal);
//            switch(CurKeyVal) 
//            {
//                case KEY_ID_1:
//                {
//                    
//                    break;
//                }
//                default:
//                    break;
//            }
//            break;
//        }
//        case KEY_EVENT_LONG_RELEASE:  //长按键释放
//        {
//            Uart2Print("\r\n长按键释放 = 0X%04x\r\n",KeyVal);
//            switch(CurKeyVal) 
//            {
//                case KEY_ID_1:
//                {
//                    
//                    break;
//                }
//                default:
//                    break;
//            }
//            break;
//        }
//        case KEY_EVENT_DOUBLE_SHORT:  //双击键
//        {
//            Uart2Print("\r\n双击键 = 0X%04x\r\n",KeyVal);
//            switch(CurKeyVal) 
//            {
//                case KEY_ID_1:
//                {
//                    
//                    break;
//                }
//                default:
//                    break;
//            }
//            break;
//        }
//        case KEY_EVENT_SHORT_LONG_PRESSED:  //短长按键未释放
//        {
//            Uart2Print("\r\n短长按键未释放 = 0X%04x\r\n",KeyVal);
//            switch(CurKeyVal) 
//            {
//                case KEY_ID_1:
//                {
//                    
//                    break;
//                }
//                default:
//                    break;
//            }
//            break;
//        }
//        case KEY_EVENT_SHORT_LONG_RELEASE:  //短长按键释放
//        {
//            Uart2Print("\r\n短长按键释放 = 0X%04x\r\n",KeyVal);
//            switch(CurKeyVal) 
//            {
//                case KEY_ID_1:
//                {
//                    
//                    break;
//                }
//                default:
//                    break;
//            }
//            break;
//        }
//        default:
//            break;
//    }
//}


//*****************************************************************************/
