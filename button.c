/*
 * button.c
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
 * @file    button.c
 * @version	1.01
 *
 ******************************************************************************
 */

#include "button.h"

/*
 * Button_Init
 * This function is mandatory and allows user to define parameters of the button.
 */
void Button_Init(
				Button_t *Button,
				GPIO_TypeDef *set_btn_gpio,
				uint16_t set_btn_gpio_pin,
				GPIO_PinState set_btn_logic_active,
				uint32_t set_btn_tick_debounce,
				uint32_t set_btn_tick_long,
				uint32_t set_btn_tick_timeout) {

	Button->btn_gpio = set_btn_gpio;
	Button->btn_gpio_pin = set_btn_gpio_pin;

	if (set_btn_logic_active == GPIO_PIN_RESET) {
		Button->btn_logic_active = GPIO_PIN_RESET;
	}
	else {
		Button->btn_logic_active = GPIO_PIN_SET;
	}
	Button->btn_tick_debounce = set_btn_tick_debounce;
	Button->btn_tick_long = set_btn_tick_long;
	Button->btn_tick_timeout = set_btn_tick_timeout;
}

/*
 * Button_Read_Logic
 *
 * This function returns present state of the software logic value (return 1 if active or 0 if inactive)
 * Serves to determine if the user is holding the button without the release event.
 * Active always returns 1, regardless of physical circuit logic defined in btn_logic_active.
 */
Button_LogicState Button_Read_Logic(Button_t *Button) {
	return(Button->btn_logic_state);
}

/*
 * Button_Read_Short
 *
 * This function returns the value of the button short-press after button release event.
 * Calling the function - reading the value - will reset the value.
 */
uint8_t Button_Read_Short(Button_t *Button) {
	if (HAL_GPIO_ReadPin(Button->btn_gpio, Button->btn_gpio_pin) != Button->btn_logic_active) {
		if (Button->btn_value_short != 0) {
			Button->btn_value_short = 0;
			Button->btn_value_long = 0;
			return 1;
		}
		else {
			return 0;
		}
	}
	else {
		return 0;
	}
}
/*
 * Button_Read_Long
 *
 * This function returns the value of the button long-press after button release event.
 * Calling the function - reading the value - will reset the value.
 */
uint8_t Button_Read_Long(Button_t *Button) {
	if (HAL_GPIO_ReadPin(Button->btn_gpio, Button->btn_gpio_pin) != Button->btn_logic_active) {
		if (Button->btn_value_long != 0) {
			Button->btn_value_short = 0;
			Button->btn_value_long = 0;
			return 1;
		}
		else {
			return 0;
		}
	}
	else {
		return 0;
	}
}

/*
 * Button_Timeout
 *
 * This function resets the active state of button stored in memory after timeout period set by user in Button_Init.
 * It enables tasks to have a period of time for reading the values after button is released - inactive.
 */
void Button_Timeout(Button_t *Button) {
	if ((Button->btn_value_short != 0) || (Button->btn_value_long != 0)) {
		if ( (Button->btn_tick_present - Button->btn_tick_timeout_start) >= Button->btn_tick_timeout) {
			Button->btn_value_short = 0;
			Button->btn_value_long = 0;
		}
	}
}

/*
 * Poll_Button
 *
 * - button state machine
 *
 * This function (button state machine) measures time elapsed from first button press detection in milliseconds
 * Hardware button has 2 physical states - pressed and not pressed - returned by function Button_Read_Logic.
 * With relation of the physical state in time this driver deals with 4 virtual sub-states:
 *
 * ENC_BTN_STATE_DEFAULT (not pressed) - waiting for physical button activation, btn_logic_state defined as 0
 * ENC_BTN_STATE_DEBOUNCING (pressed?) - measure time elapsed since btn is polled as active until "btn_tick_debounce" value is reached,
 *										  btn_state is defined as "BTN_PRESSED_DEBOUNCING" and btn_state still as 0.
 * ENC_BTN_STATE_SHORT (short pressed) - after "btn_tick_short" value is reached, btn_state is defined as "BTN_PRESSED_SHORT" and btn_logic_state as 1
 * ENC_BTN_STATE_LONG (long pressed)   - after "btn_tick_long" value is reached, btn_state stays defined as "BTN_PRESSED_LONG"and btn_logic_state as 1
 *
 * Time-dimension sub-states allows to have 2 different informations by single logical value of a button by measuring time since the activation
 * and dividing it into defined intervals (states).
 *
 */
void Poll_Button(Button_t *Button) {
	Button->btn_tick_present = HAL_GetTick();
	Button_Timeout(Button);
	switch (Button->btn_state) {

		case (BTN_STATE_DEFAULT):
			Button->btn_logic_state = 0;
			if (HAL_GPIO_ReadPin(Button->btn_gpio, Button->btn_gpio_pin) == Button->btn_logic_active) {
				Button->btn_tick_debounce_start = HAL_GetTick();
				Button->btn_state = BTN_PRESSED_DEBOUNCING;
			}
			else {
				Button->btn_state = BTN_STATE_DEFAULT;
			}
			break;

		case (BTN_PRESSED_DEBOUNCING):
			Button->btn_logic_state = 0;
			if (HAL_GPIO_ReadPin(Button->btn_gpio, Button->btn_gpio_pin) == Button->btn_logic_active) {
				if ( (Button->btn_tick_present - Button->btn_tick_debounce_start) >= Button->btn_tick_debounce) {
					Button->btn_state = BTN_PRESSED_SHORT;
				}
				else {
					Button->btn_state = BTN_PRESSED_DEBOUNCING;
				}
			}
			else {
				Button->btn_state = BTN_STATE_DEFAULT;
			}

			break;

		case (BTN_PRESSED_SHORT):
		// after "btn_tick_short" value is reached, button state is defined as "BTN_PRESSED_SHORT" until release event or "btn_tick_long" value is reached
			Button->btn_logic_state = 1;
			if (HAL_GPIO_ReadPin(Button->btn_gpio, Button->btn_gpio_pin) == Button->btn_logic_active) {
				if ( (Button->btn_tick_present - Button->btn_tick_debounce_start) >= Button->btn_tick_long) {
					Button->btn_value_long = 1;
					Button->btn_state = BTN_PRESSED_LONG;
				}
				else {
					Button->btn_state = BTN_PRESSED_SHORT;
				}
			}
			else {
				if (Button->btn_value_short == 0) {
					Button->btn_tick_timeout_start = HAL_GetTick();
					Button->btn_value_short = 1;
				}
				Button->btn_state = BTN_STATE_DEFAULT;
			}
			break;
		// after "btn_tick_long" value is reached, button state is defined as "BTN_PRESSED_LONG" until release event
		case (BTN_PRESSED_LONG):
			Button->btn_logic_state = 1;
			if (HAL_GPIO_ReadPin(Button->btn_gpio, Button->btn_gpio_pin) == Button->btn_logic_active) {
				Button->btn_state = BTN_PRESSED_LONG;
			}
			else {
				if (Button->btn_value_long == 0) {
					Button->btn_tick_timeout_start = HAL_GetTick();
					Button->btn_value_long = 1;
				}
				Button->btn_state = BTN_STATE_DEFAULT;
			}
			break;

		default:
				Button->btn_state = BTN_STATE_DEFAULT;
			break;
	}
}
