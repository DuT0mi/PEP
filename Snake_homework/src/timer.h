#pragma once
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "bsp.h"

void TIMER0_IRQHandler(void);

void TimerInit();
