/*
 * button.c
 *
 *  Created on: Jan 7, 2021
 *      Author: Acer
 */

#include "button.h"

// Получение указателя на структуру кнопок
__weak const ButtonObj* Button_GetInstance(void)
{
	UNUSED(NULL);
}

// Получение указателя на структуру кнопок
__weak const ButtonMultiClickObj* ButtonMultickick_GetInstance(void)
{
	UNUSED(NULL);
}

// Функция обработки кнопок
void Button_Handle(const ButtonObj *hbut)
{

	for (uint8_t i = 0; i < hbut->Count; i++)
	{
#if (BUTTON_INVERSE == 1)
		if ((hbut->Desc[i].GPIO_Port->IDR & hbut->Desc[i].Pin) == 0)
#else
		if ((hbut->Desc[i].GPIO_Port->IDR & hbut->Desc[i].Pin))
#endif
		{

			if ((hbut->Prop[i].HoldDelay) < (BUTTON_HOLD_TIME + 1))
			{
				hbut->Prop[i].HoldDelay++;
				if (hbut->Prop[i].HoldDelay == BUTTON_HOLD_TIME)
				{
					if (hbut->Desc[i].HoldCallBack != NULL)
					{
						hbut->Desc[i].HoldCallBack();
					}
				}
			}
		}
		else
		{
			if ((hbut->Prop[i].HoldDelay > 0) && (hbut->Prop[i].HoldDelay < BUTTON_HOLD_TIME))
			{

				if (hbut->Desc[i].PressedCallBack != NULL)
				{
					hbut->Desc[i].PressedCallBack();
				}
			}
			hbut->Prop[i].HoldDelay = 0;
		}
	}

}

// Функция обработки в режиме мультиклик
void ButtonMultiClick_Handle(const ButtonMultiClickObj *hbut)
{

	for (uint8_t i = 0; i < hbut->Count; i++)
	{
#if (BUTTON_INVERSE == 1)
		if ((hbut->Desc[i].GPIO_Port->IDR & hbut->Desc[i].Pin) == 0)
#else
		if ((hbut->Desc[i].GPIO_Port->IDR & hbut->Desc[i].Pin))
#endif
		{

			if ((!hbut->Prop[i].HoldDelay) && (hbut->Prop[i].ClickNum < BUTTON_MULTICLICK_CLICK_NUM))
			{
				hbut->Prop[i].ClickNum++;
				hbut->Prop[i].ClickDelay = BUTTON_MULTICLICK_CLICK_DELAY;
			}

			if ((hbut->Prop[i].HoldDelay) < (BUTTON_HOLD_TIME + 1))
			{
				hbut->Prop[i].HoldDelay++;
				if (hbut->Prop[i].HoldDelay == BUTTON_HOLD_TIME)
				{
					hbut->Prop->ClickNum = 0;
					hbut->Prop->ClickDelay = 0;
					if (hbut->Desc[i].HoldEventCallBack != NULL)
					{
						hbut->Desc[i].HoldEventCallBack();
					}
				}
			}
			else
			{
				hbut->Prop[i].HoldDivider = (hbut->Prop[i].HoldDivider + 1) % BUTTON_MULTICLICK_CLICK_HOLD_DIVIDER;

				if (!hbut->Prop[i].HoldDivider)
				{
					if (hbut->Desc[i].HoldCallBack != NULL)
					{
						hbut->Desc[i].HoldCallBack();
					}
				}
			}
		}
		else
		{
			if ((hbut->Prop[i].HoldDelay < BUTTON_HOLD_TIME))
			{
				if (hbut->Prop[i].ClickDelay)
				{
					if (hbut->Prop[i].ClickDelay == 1)
					{
						switch(hbut->Prop[i].ClickNum)
						{
						case 1 :
							if (hbut->Desc[i].X1ClickCallBack != NULL)
							{
								hbut->Desc[i].X1ClickCallBack();
							}
							break;
						case 2 :
							if (hbut->Desc[i].X2ClickCallBack != NULL)
							{
								hbut->Desc[i].X2ClickCallBack();
							}
							break;

						case 3 :
							if (hbut->Desc[i].X3ClickCallBack != NULL)
							{
								hbut->Desc[i].X3ClickCallBack();
							}
							break;
						}

						hbut->Prop[i].ClickNum = 0;
						hbut->Prop[i].ClickDelay = 0;
					}
					hbut->Prop[i].ClickDelay--;
				}

			}
			hbut->Prop[i].HoldDelay = 0;
		}
	}

}
