/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*            ʵ�ֶ�ʱ��4���ܵ�ͷ�ļ�              */
/*                                                 */
/*-------------------------------------------------*/

#ifndef _TIMER4_H
#define _TIMER4_H

extern char timer4_flag;   //�ⲿ����������1:��ʱ��4�жϣ������ݵ�

void TIM4_Init(unsigned short int, unsigned short int);
void TIM4_ENABLE_2S(void);
void TIME4_DISABLE(void);
#endif
