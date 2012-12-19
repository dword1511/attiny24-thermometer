#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include "config.h"
#include "ds18x20.h"

uint8_t sensor_ID[OW_ROMCODE_SIZE];
uint8_t sensor = OW_SEARCH_FIRST;
int16_t disp = ERR_SUCCESS;
uint8_t disp_bit = 0;
volatile uint8_t intflag = 0;

ISR(TIM0_OVF_vect) {
  // TODO: set unused digit (the first digit if being zero)
  // not to be displayed by feeding null value (value larger
  // than 9) to CD4511.
  if(disp < 0) {
    PORTB |= 0b00000010;
    switch(disp_bit) {
      case 0:
        PORTA = (-disp % 10) | 0b10000000;
        disp_bit ++;
        break;
      case 1:
        PORTA = ((-disp % 100) / 10) | 0b01000000;
        disp_bit ++;
        break;
      case 2: PORTA = ((-disp / 100) % 10) | 0b00100000;
      default: disp_bit = 0;
    }
  }

  else {
    PORTB &= 0b11111101;
    switch(disp_bit) {
      case 0:
        PORTA = (disp % 10) | 0b10000000;
        disp_bit ++;
        break;
      case 1:
        PORTA = ((disp % 100) / 10) | 0b01000000;
        disp_bit ++;
        break;
      case 2: PORTA = ((disp / 100) % 10) | 0b00100000;
      default: disp_bit = 0;
    }
  }

  intflag = 0;
}

int main(void) {
  power_adc_disable();
  power_timer1_disable();
  power_usi_disable();

  // setup display
  DDRA = 0b11111111;
  DDRB = 0b00000010;
  // Normal Mode, 1/8/256 MHz
  cli();
  TCCR0A = 0b00000000;
  TCCR0B = 0b00000010;
  TIMSK0 = 0b00000001;
  sei();

  DS18X20_find_sensor(&sensor, sensor_ID);
  if(sensor == OW_PRESENCE_ERR) {
    disp = ERR_SENSOR;
    while(1);
  }
  if(sensor == OW_DATA_ERR) {
    disp = ERR_BUS;
    while(1);
  }

  while(1) {
    intflag = 1;
    while(intflag);
    // Display driver has just finished their routine,
    // good time to communicate with DS18B20 now.

    if(DS18X20_start_meas(DS18X20_POWER_EXTERN, NULL) != DS18X20_OK) {
      disp = ERR_MEASURE;
      break;
    }

    if(DS18X20_read_decicelsius(sensor_ID, &disp) != DS18X20_OK) {
      disp = ERR_READ;
      break;
    }

    // This is a room temperature thermometer, something
    // must be wrong (e.g. instable power supply) if the
    // value goes beyond 80 cel. degree.
    if(disp > 800) disp = ERR_TOOHIGH;
  }
}
