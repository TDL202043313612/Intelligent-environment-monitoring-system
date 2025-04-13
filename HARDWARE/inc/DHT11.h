/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*         ʵ��DHT11�ɼ���ʪ�ȹ��ܵ�ͷ�ļ�         */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __DHT11_H
#define __DHT11_H 
 
#include "sys.h"
#include "usart1.h"
 
#define DHT11_IO_IN()  {GPIOA->CRL&=0XFF0FFFFF;GPIOA->CRL|=8<<20;}
#define DHT11_IO_OUT() {GPIOA->CRL&=0XFF0FFFFF;GPIOA->CRL|=3<<20;}
 
									   
#define	DHT11_DQ_OUT PAout(5) 
#define	DHT11_DQ_IN  PAin(5)  
 
#define DHT11_DQ_RCC   RCC_APB2Periph_GPIOA
#define DHT11_DQ_PIN   GPIO_Pin_5
#define DHT11_DQ_PORT  GPIOA


//#define DHT11_IO_IN()  {GPIOA->CRH&=0XFFFFFFF0;GPIOA->CRH|=8<<0;}       //PA8����DHT11������Ϊ���
//#define DHT11_IO_OUT() {GPIOA->CRH&=0XFFFFFFF0;GPIOA->CRH|=3<<0;}       //PA8����DHT11������Ϊ����
//										   
//#define DHT11_OUT(x)    GPIO_WriteBit(GPIOA, GPIO_Pin_8, (BitAction)x)  //PA8����DHT11
//#define DHT11_DQ_IN     GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)        //PA8����DHT11	
   
void DHT11_Rst(void);
char DHT11_Check(void);
char DHT11_Read_Bit(void);
char DHT11_Read_Byte(void);
char DHT11_Read_Data(char *temp, char *humi) ;
char DHT11_Init(void);

#endif















