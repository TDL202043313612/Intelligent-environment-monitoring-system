/*************************************************************************


 **************************************************************************/

#include "key_r.h"
#include "delay.h"
//#include "include.h"

static KeyAction_Struct_t KeyAction = {KEY_ID_NONE ,KEY_STATE_INIT ,KEY_EVENT_NONE ,0,0,0};
	     
     
u8 GetKeyCode(void);




/****************************************************
  * @brief  �����˿ڳ�ʼ��
  * @param  None
  * @retval None
****************************************************/
void KEY_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
 	
	 RCC_APB2PeriphClockCmd(KEY1_RCC, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
		
	 GPIO_InitStructure.GPIO_Pin = KEY1_PIN;				 //LED0-->PB.5 �˿�����
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	 GPIO_Init(KEY1_PORT, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
	 GPIO_SetBits(KEY1_PORT,KEY1_PIN);						 //PA.5 �����
	
#ifdef KEY2
//����KEY1Ϊ��������
    
#endif

#ifdef KEY3
//����KEY3Ϊ��������
   
#endif

#ifdef KEY4
//����KEY4Ϊ��������
    
#endif

#ifdef KEY5
//����KEY5Ϊ��������
   
#endif
}
/****************************************************
  * @brief  ����ɨ��
  * @param  None
  * @retval ��ֵ����������
****************************************************/
u8 GetKeyCode(void)   
{
	if(0 == KEY1_IO()) return KEY_ID_1;    // �͵�ƽ��Ч
	
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
    static u32 Continuou_i=0;            //������������������
    u32 temp;
	u16 KeyValue = (u16)KEY_EVENT_NONE | (u16)KEY_ID_NONE;    
	KeyID_Enum_t KeyIDTmp = (KeyID_Enum_t)GetKeyCode(); 
	
	switch(KeyAction.KeyState)
	{
		case KEY_STATE_INIT:         //����Ƿ��е�һ�ΰ������� 
		{
			if(KeyIDTmp != KEY_ID_NONE )
			{
				//printf("��������\r\n");
				KeyAction.KeyState = KEY_STATE_JITTER_1 ;
				KeyAction.KeyJitterTime = GetTime();
			}
			break;
		}
		case KEY_STATE_JITTER_1:     //ʶ���һ�ΰ����Ƿ�Ϊ����
		{
			temp = Delta_T(KeyAction.KeyJitterTime);//�������ʱ������
            if(temp > KeyTime_Jitter)
			{
				if(KeyIDTmp != KEY_ID_NONE)  //�Ƕ��� ���밴����������ʶ�� 
				{
					//printf("����ʶ��\r\n");
					KeyAction .KeyState = KEY_STATE_PRESSED_1 ;
					KeyAction .KeyPressedTime = GetTime();
                    KeyAction .KeyID = KeyIDTmp;
                    KeyValue = (u16)KEY_EVENT_PRESSED | (u16)KeyAction .KeyID;  //�ü�ֵ����һ��ֻ����һ��
				}
				else                //�����ź� ֱ�ӷ��س�ʼ״̬
				{
					//printf("����ʶ�����\r\n");
					KeyAction .KeyState = KEY_STATE_INIT ;
                    KeyAction .KeyID = KEY_ID_NONE;
				}
			}
			break;
		}
		case KEY_STATE_PRESSED_1:    //��һ�ΰ������£�ʶ�𳤰����������ɿ�
		{
			temp = Delta_T(KeyAction.KeyPressedTime);//�������ʱ������   ΪʲôҪ��1������
			//printf("temp = %d\r\n",temp);
            if(temp > KeyTime_Long)//������
			{
				
                if(KeyIDTmp  == KEY_ID_NONE )       //�������ͷ�
				{
					//printf("�����ͷ�\r\n");
                    KeyValue = (u16)KEY_EVENT_LONG_RELEASE | (u16)KeyAction.KeyID;  //�ü�ֵ����һ��ֻ����һ��
                    KeyAction.KeyState = KEY_STATE_INIT ; 
                    KeyAction.KeyID = KEY_ID_NONE;
                    Continuou_i = 0;
                }
				else                               //������δ�ͷ�
                {
             		if(temp>(KeyTime_Long + Continuou_i))
                    {
						//printf("����δ�ͷ�\r\n");
                        Continuou_i += KeyTime_Continuou;
                        KeyValue = (u16)KEY_EVENT_LONG_PRESSED | (u16)KeyAction.KeyID;  //�ü�ֵ�����󽫳�������
                    }
                }
			}
            else if(KeyIDTmp == KEY_ID_NONE )       //�̰����ͷ���
			{
				//printf("�̰��ͷ�\r\n");
				KeyAction.KeyState = KEY_STATE_RELEASE_1 ;   //����̰���ʶ��
				KeyAction.KeyReleasedTime = GetTime ();
			}
			break;
		}
		case KEY_STATE_RELEASE_1 :   //��һ�ΰ����ͷ� ��ʶ�𵥻������ڶ��ΰ�������
		{
#if KEY_DOUBLE_EN
            temp = Delta_T(KeyAction.KeyReleasedTime);//�������ʱ������     
			if(KeyIDTmp != KEY_ID_NONE )          //��⵽�ڶ��ΰ�������
			{
				KeyAction.KeyJitterTime = GetTime();
				KeyAction.KeyState = KEY_STATE_JITTER_2;
			}
			else if(temp > KeyTime_Double) //�Ѿ�����˫�������ʱ�䣬�ж�Ϊ������ 
			{
				KeyValue = (u16)KEY_EVENT_SHORT | (u16)KeyAction.KeyID;   //�ü�ֵ����һ��ֻ����һ��
                KeyAction.KeyState = KEY_STATE_INIT;
                KeyAction.KeyID = KEY_ID_NONE;
			}
#else
            KeyValue = (u16)KEY_EVENT_SHORT | (u16)KeyAction.KeyID ;   //�ü�ֵ����һ��ֻ����һ��
            KeyAction.KeyState = KEY_STATE_INIT ;
            KeyAction.KeyID = KEY_ID_NONE;
#endif            
            break;
        }
#if KEY_DOUBLE_EN		
		case KEY_STATE_JITTER_2:    //ʶ��ڶ��ΰ����Ƿ�Ϊ����
		{ 
            temp = Delta_T(KeyAction.KeyJitterTime);//�������ʱ������       
			if(temp > KeyTime_Jitter)
			{
				if(KeyIDTmp != KEY_ID_NONE)  //�Ƕ��� ����ڶ��ΰ�������ʶ�� 
				{
					KeyAction .KeyState = KEY_STATE_PRESSED_2;
					KeyAction .KeyPressedTime = GetTime();
                    KeyAction .KeyID = KeyIDTmp;}
				else                        //�����ź� ���ص�����
				{
					KeyValue = (u16)KEY_EVENT_SHORT | (u16)KeyAction.KeyID;   //�ü�ֵ����һ��ֻ����һ��
                    KeyAction.KeyState = KEY_STATE_INIT;
                    KeyAction.KeyID = KEY_ID_NONE;
				}
			}
            break;
		}
        case KEY_STATE_PRESSED_2:  //�ڶ��ΰ������£�ʶ�𳤰�����˫����
        { 
            temp = Delta_T(KeyAction.KeyPressedTime);//�������ʱ������         
            if(temp > KeyTime_Long)//��_������
				{
				if(KeyIDTmp  == KEY_ID_NONE )       //��_�������ͷ�
				{
                    KeyValue = (u16)KEY_EVENT_SHORT_LONG_RELEASE | (u16)KeyAction.KeyID;  //�ü�ֵ����һ��ֻ����һ��
                    KeyAction.KeyState = KEY_STATE_INIT ; 
                    KeyAction.KeyID = KEY_ID_NONE;
                    Continuou_i = 0;
                }                
				else                               //��_������δ�ͷ�
                {
                    if(temp >(KeyTime_Long + Continuou_i))
                    {
                        Continuou_i += KeyTime_Continuou;
                        KeyValue = (u16)KEY_EVENT_SHORT_LONG_PRESSED | (u16)KeyAction.KeyID;  //�ü�ֵ�����󽫳�������
                    }
                }
                    
			}
            else if(KeyIDTmp == KEY_ID_NONE )       //˫���������ͷ���
			{
				KeyValue = (u16)KEY_EVENT_DOUBLE_SHORT | (u16)KeyAction.KeyID ;   //�ü�ֵ����һ��ֻ����һ��
                KeyAction.KeyState = KEY_STATE_INIT ;
                KeyAction.KeyID = KEY_ID_NONE;
			}
            break;
        }
		case KEY_STATE_RELEASE_2:  // ˫���� �ͷ�(�����������������״̬����Ҫ)
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
  * @brief  �ȴ������ͷ�
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

//*********************����������ģ��**********************************

//
//void DealKeyVal(u16 KeyVal)
//{
//    u16  KeyEvent  = KeyVal&KEYEVENTMASK;   //�����¼�
//    u16  CurKeyVal = KeyVal&KEYIDMASK;      //��ǰ��ֵ
//    switch(KeyEvent)
//    {
//        
//        case KEY_EVENT_PRESSED:  // ��������
//        {
//            Uart2Print("\r\n�������� = 0X%04x\r\n",KeyVal);
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
//        case KEY_EVENT_SHORT:    // ������
//        {
//            Uart2Print("\r\n������ = 0X%04x\r\n",KeyVal);
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
//        case KEY_EVENT_LONG_PRESSED:  //������δ�ͷ�
//        {
//            Uart2Print("\r\n������δ�ͷ� = 0X%04x\r\n",KeyVal);
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
//        case KEY_EVENT_LONG_RELEASE:  //�������ͷ�
//        {
//            Uart2Print("\r\n�������ͷ� = 0X%04x\r\n",KeyVal);
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
//        case KEY_EVENT_DOUBLE_SHORT:  //˫����
//        {
//            Uart2Print("\r\n˫���� = 0X%04x\r\n",KeyVal);
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
//        case KEY_EVENT_SHORT_LONG_PRESSED:  //�̳�����δ�ͷ�
//        {
//            Uart2Print("\r\n�̳�����δ�ͷ� = 0X%04x\r\n",KeyVal);
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
//        case KEY_EVENT_SHORT_LONG_RELEASE:  //�̳������ͷ�
//        {
//            Uart2Print("\r\n�̳������ͷ� = 0X%04x\r\n",KeyVal);
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
