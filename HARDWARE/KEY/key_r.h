/*************************************************************************

 **************************************************************************/

#ifndef __KEY_H__
#define __KEY_H__

#include "SYS.h" 

#define   	KEY_DOUBLE_EN		 1      //1：使能双击键、短_长按键，0禁能


/*****按键输入引脚*********************************************/
#define KEY1 
#define	KEY1_RCC		RCC_APB2Periph_GPIOC
#define	KEY1_PORT		GPIOC								
#define KEY1_PIN    GPIO_Pin_13
#define KEY1_IO()		!!(KEY1_PORT->IDR &KEY1_PIN)

		



//以下时基是用 GetTime() 返回的时间
#define  KeyTime_Jitter		   10		//消除抖动时间
#define  KeyTime_Long		   700	//长按键触发时间
#define  KeyTime_Double		   200 	//双击键 两次按下的时间差
#define  KeyTime_Continuou     200  //长按键 连续触发时间间隔

/*
*********************************************************************************************************
*                                         	   TYPE DEFINES
*********************************************************************************************************
*/
//键值
typedef enum {
	KEY_ID_NONE	=	0x0000,
	
	KEY_ID_1	=	0x0001, //编码器按键
	KEY_ID_2	=	0x0002, //电源按键
	KEY_ID_3	=	0x0003,
	KEY_ID_4	=	0x0004,
	KEY_ID_5	=	0x0005,
	KEY_ID_6	=	0x0006,
	KEY_ID_7	=	0x0007,
	KEY_ID_8	=	0x0008,
	KEY_ID_9	=	0x0009,    
	KEY_ID_Multi	,
	
}KeyID_Enum_t;
//按键状态
typedef enum {
	KEY_STATE_INIT					=	0x0000,
	KEY_STATE_JITTER_1				=	0x1000,
	KEY_STATE_PRESSED_1				=	0x2000,
	KEY_STATE_RELEASE_1				=	0x3000,
	KEY_STATE_JITTER_2				=	0x4000,
    KEY_STATE_PRESSED_2				=	0x5000,
	KEY_STATE_RELEASE_2				=	0x6000,	
}KeyState_Enum_t;
//按键事件
typedef enum {
	KEY_EVENT_NONE					=	0x0000,
    KEY_EVENT_PRESSED               =   0x0100,      //按键按下
	KEY_EVENT_SHORT					=	0x0200,      //单击按键(已释放按键)
	KEY_EVENT_LONG_PRESSED          =	0x0300,      //长按(未释放按键，可以在这个实践中计时做连续触发事件)
    KEY_EVENT_LONG_RELEASE          =   0x0400,      //长按释放
	KEY_EVENT_DOUBLE_SHORT			=	0x0500,      //双击(已释放按键)
	KEY_EVENT_SHORT_LONG_PRESSED	=	0x0600,      //短长按(未释放按键，可以在这个实践中计时做连续触发事件)
    KEY_EVENT_SHORT_LONG_RELEASE    =   0X0700,      //短长按释放
	
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


void KEY_Init(void);        //KEY初始化
u8 GetKeyCode(void);
u16 Key_GetValue(void);     //按键扫描及键值的判断
void WaitKyeRelease(void);  //等待按键释放




#endif /* __KEY_H */

