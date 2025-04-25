/*
 * button.h
 *
 *  Created on: Apr 12, 2025
 *      Author: Martin Andruska
 */

/*
MIT License

Copyright (c) 2025 Martin Andruska (https://github.com/masonn7)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
 ******************************************************************************
 * @file    button.h
 * @version	1.01
 * @brief	Each button instance has all the parameters stored separately for full customisation of the system.
 * 			This allows for different types of hardware (buttons, switches, contacts) to be used with this library at the same time.
 *
 ******************************************************************************
 */

#ifndef __BUTTON_H_
#define __BUTTON_H_

/*
 * Include hal lib according to hardware.
 */
#include "stm32f4xx_hal.h"

typedef enum {
	BTN_STATE_DEFAULT = 0,
	BTN_PRESSED_DEBOUNCING = 1,
	BTN_PRESSED_SHORT = 2,
	BTN_PRESSED_LONG = 3,
	  }
Button_State;

typedef enum {
	BTN_INACTIVE = 0,
	BTN_ACTIVE = 1,
	}
Button_LogicState;

typedef struct {
// hardware config
GPIO_TypeDef			*btn_gpio;					// button gpio port (example: GPIOA)
uint16_t				btn_gpio_pin;				// button gpio pin (example: GPIO_PIN_1)
GPIO_PinState			btn_logic_active;			// define if hardware logic active state is 1 or 0 (HAL: SET or RESET )
// timing config
uint32_t				btn_tick_debounce;			// debunce period set by user in ms (default: 1 HAL tick is 1 ms)
uint32_t				btn_tick_long;				// long-press ms set by user
uint32_t				btn_tick_timeout;			// period in ms while detected keystrokes are stored for reading
// process variables
Button_State			btn_state;					// state of the button polling state machine
uint32_t				btn_tick_present;			// tick value buffer for HAL_GetTick() for increased thread-safety
uint32_t				btn_tick_debounce_start;	// hal tick value at the start of debouncing
uint32_t				btn_tick_timeout_start;		// hal tick value when button is released after short/long press is registered
// return variables
Button_LogicState		btn_logic_state;			// software state of the button logic (return active 1 or inactive 0)
uint8_t					btn_value_short;
uint8_t					btn_value_long;
	}Button_t;

void Button_Init(
				Button_t *Button,					// set name of the button instance
				GPIO_TypeDef *set_btn_gpio,			// set button gpio port (use Hal macro)
				uint16_t set_btn_gpio_pin,			// set button IO pin (use Hal macro)
				GPIO_PinState set_btn_logic_active,	// define active if 1 or 0
				uint32_t set_btn_tick_debounce,		// set debounce in tick value (ms)
				uint32_t set_btn_tick_long,			// set long press interval in tick (ms)
				uint32_t set_btn_tick_timeout);		// set button active value read timeout interval in tick (ms)

Button_LogicState Button_Read_Logic(Button_t *Button);
uint8_t Button_Read_Short(Button_t *Button);
uint8_t Button_Read_Long(Button_t *Button);
void Button_Timeout(Button_t *Button);
void Poll_Button(Button_t *Button);

#endif /* __BUTTON_H_ */
