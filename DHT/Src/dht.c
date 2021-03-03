#include "dht.h"


__attribute__((weak)) void DHT_PinMode(DHT_Obj *hdht, uint8_t mode)
{
    (void) mode;
}

//------------------------------------
__attribute__((weak)) void DHT_PinWrite(DHT_Obj *hdht, uint8_t state)
{
    (void) state;
}
//-------------------------------------
__attribute__((weak)) uint8_t DHT_PinRead(DHT_Obj *hdht)
{
    (void) NULL;
    return 0;
}
//----------------------------------------------
__attribute__((weak)) void DHT_Delay_us(DHT_Obj *hdht, uint32_t value)
{
    (void) value;
}
//----------------------------------------------
__attribute__((weak)) void DHT_Delay_ms(DHT_Obj *hdht, uint32_t value)
{
    (void) value;
}
//----------------------------------------------
DHT_Result* DHT_Read(DHT_Obj *hdht)
{
    uint8_t j = 0, i;
	uint32_t cnt_delay = 0xFFFFFFFF;

//------------------------------------------

    memset(hdht->Data, 0, sizeof(hdht->Data));

    /* pull pin down for 18 milliseconds */
    DHT_PinMode(hdht, DHT_PIN_OUTPUT);
    DHT_PinWrite(hdht, DHT_PIN_LOW);
    DHT_Delay_ms(hdht, 18);
    /* prepare to read the pin */
    DHT_PinMode(hdht, DHT_PIN_INPUT);
    DHT_Delay_us(hdht, 39);
    if (DHT_PinRead(hdht) == 1)
    {
		return NULL;
    }

    DHT_Delay_us(hdht, 80);

    if (DHT_PinRead(hdht) == 0)
    {
		return NULL;
    }

    DHT_Delay_us(hdht, 80);

    for (j = 0; j < 5; j++)
    {
		hdht->Data[4-j]=0;
		for(i=0; i<8; i++)
		{
			cnt_delay = 0xFFFFFFFF;
			while(DHT_PinRead(hdht) == 0)
			{
				cnt_delay--;
				if (cnt_delay == 0)
				{
					return NULL;
				}
			}
			DHT_Delay_us(hdht, 30);
			if (DHT_PinRead(hdht) == 1)
			{
				hdht->Data[4-j] |= (1<<(7-i));
			}
			cnt_delay = 0xFFFFFFFF;
			while(DHT_PinRead(hdht) == 1)
			{
				cnt_delay--;
				if (cnt_delay == 0)
				{
					return NULL;
				}
			}
		}
    }

    hdht->Result.Temperature = (float)((*(uint16_t*)(hdht->Data + 1)) & 0x3FFF) / 10;
    if((*(uint16_t*)(hdht->Data + 1)) & 0x8000) hdht->Result.Temperature *= -1.0;
    hdht->Result.Humidity = (float)(*(int16_t*)(hdht->Data + 3)) / 10;

    return &hdht->Result;
}