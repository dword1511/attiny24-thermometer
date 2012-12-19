/*******************************************
 * Access Dallas 1-Wire Devices with ATMEL AVRs
 * Author of the initial code: Peter Dannegger (danni(at)specs.de)
 * modified by Martin Thomas (mthomas(at)rhrk.uni-kl.de)
 *  9/2004 - use of delay.h, optional bus configuration at runtime
 * 10/2009 - additional delay in ow_bit_io for recovery
 *  5/2010 - timing modifcations, additonal config-values and comments,
 *           use of atomic.h macros, internal pull-up support
 *  7/2010 - added method to skip recovery time after last bit transfered
 *           via ow_command_skip_last_recovery
 *******************************************/

#ifndef __ONEWIRE_H__
#define __ONEWIRE_H__

#include <util/atomic.h>
#include <util/delay.h>
#include <stdint.h>
#include "config.h"

/* Hardware connection */

/* One-wire bus pin configuration */
#define OW_PIN PB0
#define OW_IN PINB
#define OW_OUT PORTB
#define OW_DDR DDRB
#define OW_CONF_DELAYOFFSET 0

/*******************************************
 * Recovery time (T_Rec) minimum 1usec - increase for long lines 
 * 5 usecs is a value give in some Maxim AppNotes
 * 30u secs seem to be reliable for longer lines
 *******************************************/
#ifndef OW_RECOVERY_TIME
#define OW_RECOVERY_TIME 5 /* usec */
#endif /* OW_RECOVERY_TIME */

#define OW_MATCH_ROM 0x55
#define OW_SKIP_ROM 0xCC
#define OW_SEARCH_ROM 0xF0

#define OW_SEARCH_FIRST 0xFF /* start new search */
#define OW_PRESENCE_ERR 0xFF
#define OW_DATA_ERR 0xFE
#define OW_LAST_DEVICE 0x00 /* last device found */

/* rom-code size including CRC */
#define OW_ROMCODE_SIZE 8

uint8_t ow_reset(void);

uint8_t ow_bit_io(uint8_t b);
uint8_t ow_byte_wr(uint8_t b);
uint8_t ow_byte_rd(void);

uint8_t ow_rom_search(uint8_t diff, uint8_t *id);

void ow_command(uint8_t command, uint8_t *id);
void ow_command_with_parasite_enable(uint8_t command, uint8_t *id);

void ow_parasite_enable(void);
void ow_parasite_disable(void);
uint8_t ow_input_pin_state(void);

void ow_set_bus(volatile uint8_t* in, volatile uint8_t* out, volatile uint8_t* ddr, uint8_t pin);

#define OW_GET_IN() (OW_IN & (1 << OW_PIN))
#define OW_OUT_LOW() (OW_OUT &= ( ~ (1 << OW_PIN)))
#define OW_OUT_HIGH() (OW_OUT |= (1 << OW_PIN))
#define OW_DIR_IN() (OW_DDR &= ( ~ (1 << OW_PIN)))
#define OW_DIR_OUT() (OW_DDR |= (1 << OW_PIN))

#endif /* __ONEWIRE_H__ */