#include "timer.h"


volatile int32_t runTime;
#define COMP_CONST 1

volatile bool TIMERflag=false;

void TIMER0_IRQHandler(void){ //A timer IT ezt a f�ggv�nyt h�vja meg
TIMERflag=true;
TIMER_IntClear(TIMER0, TIMER_IF_OF); //a t�bbi interrupt t�rl�se
}

void TimerInit() {
// a perif�ria �rajel oszt�j�nak be�ll�t�sa
CMU_ClockDivSet(cmuClock_HFPER, cmuClkDiv_1);
// *******************************
// * TIMER inicializ�l�sa *
// *******************************
// id�z�t� �rajel�nek enged�lyez�se
CMU_ClockEnable(cmuClock_TIMER0, true);
// l�trehozzuk az inicializ�ci�hoz sz�ks�ges param�terstrukt�r�t
TIMER_Init_TypeDef TIMER0_init = TIMER_INIT_DEFAULT;
// a prescaler-t �t�ll�tjuk
TIMER0_init.prescale = timerPrescale256; // timerPrescale1...timerPrescale1024
// inicializ�l�s a param�terstrukt�r�val
//void TIMER_Init(TIMER_TypeDef *timer, const TIMER_Init_TypeDef *init);
TIMER_Init(TIMER0, &TIMER0_init);
// resetelj�k a sz�ml�l�t
TIMER_CounterSet(TIMER0, 0); //
// be�ll�tjuk a TOP �rt�ket
//__STATIC_INLINE void TIMER_TopSet(TIMER_TypeDef *timer, uint32_t val)
TIMER_TopSet(TIMER0, 27344); // 14MHz/presc/TOP 27344-el 0,5 m�sodpercenk�nt van, 1s=>54687,5
// t�r�lj�k az esetleges f�gg� megszak�t�sokat
//__STATIC_INLINE void TIMER_IntClear(TIMER_TypeDef *timer, uint32_t flags);
TIMER_IntClear(TIMER0, TIMER_IF_OF);
// Timer IT enged�lyez�se
//TIMER_IntEnable(TIMER_TypeDef *timer, uint32_t flags);
TIMER_IntEnable(TIMER0, TIMER_IF_OF);
// Timer IT enged�lyez�se az NVIC-ben
NVIC_EnableIRQ(TIMER0_IRQn);
// *******************************
// * LED inicializ�l�sa *
// *******************************
//BSP_LedsInit();

runTime = DWT->CYCCNT;
//adouble=(double)aint;
runTime = DWT->CYCCNT - runTime - COMP_CONST;
}
