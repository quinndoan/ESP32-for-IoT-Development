#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"

#include "DHT22.h"
#include "task_common.h"

static const char *TAG = "DHT";

int DHTgpio = 4;
float temperature = 0.;
float humidity = 0.;

void setDHTgpio(int gpio){
    DHTgpio = gpio;
}

float getHumidity(){
    return humidity;
}
float getTemperature(){
    return temperature;
}

void errorHandler(int response){
    switch (response)
    {
    case DHT_CHECKSUM_ERROR:
        ESP_LOGE(TAG, "CheckSum error");
        break;

    case DHT_TIMEOUT_ERROR:
    ESP_LOGE(TAG, "Timeout Error");
    break;

    case DHT_OK:
    //ESP_LOGE(TAG, "Timeout Error");
    break;
    
    default:
        ESP_LOGE(TAG, "Unknown Error");
    }
}

int getSignalLevel(int usTimeOut, bool state){
    int uSec = 0;
    while (gpio_get_level(DHTgpio)== state){
        if (uSec > usTimeOut){
            return -1;
        }
        ++uSec;
        esp_rom_delay_us(1);
    }
    return uSec;
}

#define MAXdhtData 5	// to complete 40 = 5*8 Bits

int readDHT()
{
int uSec = 0;

uint8_t dhtData[MAXdhtData];
uint8_t byteInx = 0;
uint8_t bitInx = 7;

	for (int k = 0; k<MAXdhtData; k++) 
		dhtData[k] = 0;


	gpio_set_direction( DHTgpio, GPIO_MODE_OUTPUT );

	// pull down for 3 ms for a smooth and nice wake up 
	gpio_set_level( DHTgpio, 0 );
	esp_rom_delay_us( 3000 );

	// pull up for 25 us for a gentile asking for data
	gpio_set_level( DHTgpio, 1 );
	esp_rom_delay_us( 25 );

	gpio_set_direction( DHTgpio, GPIO_MODE_INPUT ); // mode input
  

	uSec = getSignalLevel( 85, 0 );
	if( uSec<0 ) return DHT_TIMEOUT_ERROR; 

	// -- 80us up ------------------------

	uSec = getSignalLevel( 85, 1 );

	if( uSec<0 ) return DHT_TIMEOUT_ERROR;

    // there are no errors, read 40 bits
  
	for( int k = 0; k < 40; k++ ) {

		// -- starts new data transmission with >50us low signal

		uSec = getSignalLevel( 56, 0 );
		if( uSec<0 ) return DHT_TIMEOUT_ERROR;

		// -- check to see if after >70us rx data is a 0 or a 1

		uSec = getSignalLevel( 75, 1 );
		if( uSec<0 ) return DHT_TIMEOUT_ERROR;

		// add the current read to the output data
		// since all dhtData array where set to 0 at the start, 
		// only look for "1" (>28us us)
	
		if (uSec > 40) {
			dhtData[ byteInx ] |= (1 << bitInx);
			}
	
		// index to next byte

		if (bitInx == 0) { bitInx = 7; ++byteInx; }
		else bitInx--;
	}

	// humidity from Data[0] and Data[1]

	humidity = dhtData[0];
	humidity *= 0x100;					// << 8
	humidity += dhtData[1];
	humidity /= 10;						// get the decimal

	// == get temp from Data[2] and Data[3]
	
	temperature = dhtData[2] & 0x7F;	
	temperature *= 0x100;				// move 8 bits to the left, equal *256
	temperature += dhtData[3];
	temperature /= 10;

	if( dhtData[2] & 0x80 ) // negative temperature
		temperature *= -1;


	//checksum
	
	if (dhtData[4] == ((dhtData[0] + dhtData[1] + dhtData[2] + dhtData[3]) & 0xFF)) 
		return DHT_OK;

	else 
		return DHT_CHECKSUM_ERROR;
}

// Task for DHT22 reading

static void DHT22_task(void *pvParameter){
    setDHTgpio(DHTgpio);
    for (;;){
        int res = readDHT();
        errorHandler(res);
        vTaskDelay(4000);
    }
}

// register task
void DHT22_task_start(void){
    xTaskCreatePinnedToCore(&DHT22_task, "DHT22_task", DHT22_TASK_STACK_SIZE, NULL, DHT22_TASK_PRIORITY, NULL, DHT22_TASK_CORE_ID);
}