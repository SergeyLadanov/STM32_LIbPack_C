#include "dht_if.h"

#define DHT_PIN 7

void DHT_PinMode(DHT_Obj *hdht, uint8_t mode)
{
    switch (mode)
    {
        case DHT_PIN_OUTPUT : 
            pinMode(DHT_PIN, OUTPUT);
        break;

        case DHT_PIN_INPUT :
            pinMode(DHT_PIN, INPUT);
        break;
    }
}

//------------------------------------
void DHT_PinWrite(DHT_Obj *hdht, uint8_t state)
{
   digitalWrite(DHT_PIN, state);
}
//-------------------------------------
uint8_t DHT_PinRead(DHT_Obj *hdht)
{
    return digitalRead(DHT_PIN);
}
//----------------------------------------------
void DHT_Delay_us(DHT_Obj *hdht, uint32_t value)
{
    delayMicroseconds(value);
}
//----------------------------------------------
void DHT_Delay_ms(DHT_Obj *hdht, uint32_t value)
{
    delay(value);
}