/***************************************************************************//**
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "em_device.h"
#include "em_chip.h"

#include "em_cmu.h"
#include <stdint.h>
#include <stdbool.h>

#include "segmentlcd.h"
#include "segmentlcd_individual.h"

#include "stdlib.h" //v�letlen kaja hely gener�l�shoz

#include "uart.h"
#include "snake.h"
#include "timer.h"



/*
 * "sl_udelay.h" is used only by the demo functions to slow things down.
 * Otherwise it is not required to use the SegmentLCD driver extension.
 */
#include <sl_udelay.h>


/***************************************************************************//**
 * Globals
 ******************************************************************************/
extern volatile int UARTvalue;
extern volatile bool UARTflag;
extern volatile bool TIMERflag;
volatile bool UARThappened; //m�shogy kell mozogni
snake mysnake;
uint8_t food;
SegmentLCD_LowerCharSegments_TypeDef mydisplay[7];
bool SymbolFlip;


/***************************************************************************//**
 * FreeRTOS task
 ******************************************************************************/
TaskHandle_t handleFlashing;
TaskHandle_t handleInit;
SemaphoreHandle_t guard;

//Egyszer lefutó inicializáló tasz, lefutás után törli magát
void prvTskINIT(void){
  while(1){
    mysnake=SnakeInit(mysnake);  //pr�ba, hogy megy-e a kirajzol�s stb.
    food=PlaceFood(mysnake);  //random helyre most lerakok egy kaj�t
    SymbolFlip=false;

    vTaskSuspend(NULL);
  }
}

/*Alacsonyabb prioritású taszk, vizsgálja, hogy érkezett-e karakter, lefoglal egy
  szemafort, így az irányváltás nem íródik felül két
  kijelzés között*/
void prvTskDir(void){
      /*Beállítunk egy szemafort, a későbbi UART vétel nem jut érvényre*/
  while(1){
      if(UARTflag) {
        UARTflag=false;
        UARThappened=true;                  //ennek a szemafor beállítás előtt kell lennie, ekkor nem jut érvénye
                                            //egy kijelző periódus alatt érkezett második karakter későbbi periódusban sem
        xSemaphoreTake(guard, portMAX_DELAY);
        UARTvalue=USART_RxDataGet(UART0); //itt NEM szabad kiolvasni
        USART_Tx(UART0, UARTvalue);

      }
  }
}

/*Magasabb prioritású taszk, ha léptettük a kígyót, várakozik*/
/*Elengedjük az UART-os szemafort*/
void prvTskDisp(void){
  while(1){
       if(mysnake.isAlive) {
        if(UARThappened)
          mysnake=NextDirUART(mysnake, UARTvalue);
        else
          mysnake=NextDirNoUART(mysnake);

        UARThappened=false;
          mysnake=MoveSnake(mysnake, &food);
          *mydisplay=SnakeandFoodtoLCD(mysnake, food, mydisplay);
        SegmentLCD_LowerSegments(mydisplay);
        SegmentLCD_Number(mysnake.size);

        vTaskDelay(configTICK_RATE_HZ);
        xSemaphoreGive(guard);

       }
       else {
           vTaskResume(handleFlashing);     //ha vége a játéknak, felébreszti
                                            //a magasabb prioritású villogtató taszkot
       }
  }
}

void prvTskFlashing(void){
  while(1){
         vTaskSuspend(handleFlashing);
         SegmentLCD_AllOff();
         int i = 0;

         while(i < 5){
         if(SymbolFlip) {
           SegmentLCD_Symbol(LCD_SYMBOL_DP2, 1);
           SegmentLCD_Symbol(LCD_SYMBOL_DP3, 1);
           SegmentLCD_Symbol(LCD_SYMBOL_DP4, 1);
           SegmentLCD_Symbol(LCD_SYMBOL_DP5, 1);
           SegmentLCD_Symbol(LCD_SYMBOL_DP6, 1);
           SymbolFlip=false;
         }
         else {
           SegmentLCD_Symbol(LCD_SYMBOL_DP2, 0);
           SegmentLCD_Symbol(LCD_SYMBOL_DP3, 0);
           SegmentLCD_Symbol(LCD_SYMBOL_DP4, 0);
           SegmentLCD_Symbol(LCD_SYMBOL_DP5, 0);
           SegmentLCD_Symbol(LCD_SYMBOL_DP6, 0);
           SymbolFlip=true;
         }
         SegmentLCD_Number(mysnake.size);
         vTaskDelay(configTICK_RATE_HZ);
         i++;
       }
         // Printf the current score with flags
         printf("f"); // start flag
         printf("%d", mysnake.size);
         printf("l"); // end flag

         USART_Tx(UART0, 0x31);
       vTaskResume(handleInit);
  }
}



/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/
void app_init(void)
{
  CHIP_Init();
  uartinit();
  TimerInit();
  /* Enable LCD without voltage boost */
  SegmentLCD_Init(false);
  /* If first word of user data page is non-zero, enable Energy Profiler trace */

  SegmentLCD_AllOff();

  guard = xSemaphoreCreateBinary();

  xTaskCreate(
        prvTskINIT, // function pointer, is just the function's name without brackets and params
        "",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 3, // in freetros the num is bigger than the priority too
        &handleInit);

  xTaskCreate(
         prvTskDir, // function pointer, is just the function's name without brackets and params
         "",
         configMINIMAL_STACK_SIZE,
         NULL,
         tskIDLE_PRIORITY + 1, // in freetros the num is bigger than the priority too
         NULL);

  xTaskCreate(
         prvTskDisp, // function pointer, is just the function's name without brackets and params
         "",
         configMINIMAL_STACK_SIZE,
         NULL,
         tskIDLE_PRIORITY + 2, // in freetros the num is bigger than the priority too
         NULL);

  xTaskCreate(
        prvTskFlashing, // function pointer, is just the function's name without brackets and params
        "",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + 3, // in freetros the num is bigger than the priority too
        &handleFlashing);
}


/***************************************************************************//**
 * App ticking function.
 ******************************************************************************/
void app_process_action(void){

}
