# button.h

Multi-button driver based on software polling. For use with STM32. Compatibile with RTOS.

## Description

Multiple parallel button driver implemented as state-machine and self-contained data structure - each button instance has all the parameters stored separately for full customisation of the system. This allows for different types of hardware (buttons, switches, contacts) with different transient properties to be used with this driver at the same time in parallel.
The code is using STM32 HAL functions and macros.
Number of button instances is limited only by hardware and memory.
Adjustable timeout interval to keep value in memory while RTOS is busy with higher priority tasks.
Function to return last polled logic state (debounced) regardless of short / long periods.
Short / Long periods are returned only after button release event.

### Customize:
* Pin logic
* Debounce period
* Short-press period
* Long-press period
* Value timeout period

## Authors

Martin Andruska

## Version History

* 1.01
    * Format and comments update    
* 1.00
    * Working version tested

## To do
* Add double-click
* Add second long-press

## License
This project is licensed under the MIT License - see the LICENSE file for details
