/*************************************************************************

 **************************************************************************/

#ifndef __KEY_H__
#define __KEY_H__

#include "SYS.h" 

#define   	KEY_DOUBLE_EN		 1      //1��ʹ��˫��������_��������0����


/*****������������*********************************************/
#define KEY1 
#define	KEY1_RCC		RCC_APB2Periph_GPIOC
#define	KEY1_PORT		GPIOC								
#define KEY1_PIN    GPIO_Pin_13
#define KEY1_IO()		!!(KEY1_PORT->IDR &KEY1_PIN)

		



//����ʱ������ GetTime() ���ص�ʱ��
#define  KeyTime_Jitter		   10		//��������ʱ��
#define  KeyTime_Long		   700	//����������ʱ��
#define  KeyTime_Double		   200 	//˫���� ���ΰ��µ�ʱ���
#define  KeyTime_Continuou     200  //������ ��������ʱ����

/*
*********************************************************************************************************
*                                         	   TYPE DEFINES
*********************************************************************************************************
*/
//��ֵ
typedef enum {
	KEY_ID_NONE	=	0x0000,
	
	KEY_ID_1	=	0x0001, //����������
	KEY_ID_2	=	0x0002, //��Դ����
	KEY_ID_3	=	0x0003,
	KEY_ID_4	=	0x0004,
	KEY_ID_5	=	0x0005,
	KEY_ID_6	=	0x0006,
	KEY_ID_7	=	0x0007,
	KEY_ID_8	=	0x0008,
	KEY_ID_9	=	0x0009,    
	KEY_ID_Multi	,
	
}KeyID_Enum_t;
//����״̬
typedef enum {
	KEY_STATE_INIT					=	0x0000,
	KEY_STATE_JITTER_1				=	0x1000,
	KEY_STATE_PRESSED_1				=	0x2000,
	KEY_STATE_RELEASE_1				=	0x3000,
	KEY_STATE_JITTER_2				=	0x4000,
    KEY_STATE_PRESSED_2				=	0x5000,
	KEY_STATE_RELEASE_2				=	0x6000,	
}KeyState_Enum_t;
//�����¼�
typedef enum {
	KEY_EVENT_NONE					=	0x0000,
    KEY_EVENT_PRESSED               =   0x0100,      //��������
	KEY_EVENT_SHORT					=	0x0200,      //��������(���ͷŰ���)
	KEY_EVENT_LONG_PRESSED          =	0x0300,      //����(δ�ͷŰ��������������ʵ���м�ʱ�����������¼�)
    KEY_EVENT_LONG_RELEASE          =   0x0400,      //�����ͷ�
	KEY_EVENT_DOUBLE_SHORT			=	0x0500,      //˫��(���ͷŰ���)
	KEY_EVENT_SHORT_LONG_PRESSED	=	0x0600,      //�̳���(δ�ͷŰ��������������ʵ���м�ʱ�����������¼�)
    KEY_EVENT_SHORT_LONG_RELEASE    =   0X0700,      //�̳����ͷ�
	
}KeyEvent_Enum_t;

typedef struct {
	KeyID_Enum_t	KeyID;
	KeyState_Enum_t KeyState;
	KeyEvent_Enum_t KeyEvent;
	u32				KeyJitterTime;
	u32				KeyPressedTime;
	u32				KeyReleasedTime;
	
}KeyAction_Struct_t;

#define     KEYSTATEMASK   0XF000
#define     KEYEVENTMASK   0x0F00
#define     KEYIDMASK      0x00FF


void KEY_Init(void);        //KEY��ʼ��
u8 GetKeyCode(void);
u16 Key_GetValue(void);     //����ɨ�輰��ֵ���ж�
void WaitKyeRelease(void);  //�ȴ������ͷ�




#endif /* __KEY_H */

