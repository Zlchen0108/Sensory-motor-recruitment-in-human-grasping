#ifndef __DELAY_H
#define __DELAY_H

void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);
void Delay_s(uint32_t s);
void TIM2_Init(void);
void TIM2_IRQHandler(void);

extern volatile uint32_t System_Tick;

#endif
