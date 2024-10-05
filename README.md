# Microcontroller Temperature Display and Alarm System

This project is a temperature monitoring system using a microcontroller. It displays the temperature on a 4-digit 7-segment display and features an LED indicator system for different temperature ranges. The system supports both Celsius and Fahrenheit units and includes an alarm function when the temperature exceeds a threshold.

## Features
- **Temperature Display:** Shows the current temperature on a 7-segment display.
- **Celsius/Fahrenheit Toggle:** Allows switching between Celsius and Fahrenheit.
- **Increment/Decrement Controls:** Buttons to adjust the temperature manually for testing.
- **LED Indicator:** Visual indication of temperature ranges using LEDs.
- **Alarm:** LED alarm triggered when the temperature exceeds 40°C (or 104°F).
- **Modes:** 
  - **Seven-Segment Mode:** Displays temperature on a 7-segment display.
  - **LED Indicator Mode:** Uses PWM to control LED brightness for visual temperature indication.

## Hardware Components
- **ATmega microcontroller**
- **4-Digit 7-Segment Display**
- **74HC595 Shift Register**
- **Buttons (for temperature control and mode switching)**
- **LEDs (for temperature indication and alarm)**
- **Resistors, wires, and other peripherals**

## Pin Configuration
- **Shift Register Pins:**
  - `SHIFT_DATA` (PB0) - Serial data input
  - `SHIFT_CLK` (PB3) - Shift register clock
  - `SHIFT_LATCH` (PB4) - Storage register clock
- **Digit Control:**
  - `DIGIT_1` (PC0) - First digit
  - `DIGIT_2` (PC1) - Second digit
  - `DIGIT_3` (PC2) - Third digit
  - `DIGIT_4` (PC3) - Fourth digit (for C/F display)
- **Button Inputs:**
  - `BTN_INC` (PD2) - Increment temperature
  - `BTN_DEC` (PD3) - Decrement temperature
  - `BTN_TOGGLE` (PD4) - Toggle Celsius/Fahrenheit
  - `BTN_ACK` (PD1) - Acknowledge alarm (turn off alarm LED)
  - `BTN_MODE` (PD0) - Switch between Seven-Segment and LED modes
- **Alarm LED:**
  - `ALARM_LED` (PB5) - LED to indicate high temperature (over 40°C)

## Button Functionality
- **Increment Button (BTN_INC):** Increases the displayed temperature.
- **Decrement Button (BTN_DEC):** Decreases the displayed temperature.
- **Toggle Button (BTN_TOGGLE):** Switch between Celsius and Fahrenheit display.
- **Acknowledge Button (BTN_ACK):** Turns off the alarm LED when the temperature exceeds 40°C.
- **Mode Button (BTN_MODE):** Switch between the Seven-Segment display and the LED Indicator mode.

## Temperature Ranges for LED Indicator Mode
- Below 15°C: One LED lights up with brightness proportional to the temperature.
- 15°C - 25°C: Two LEDs light up with increasing brightness.
- 25°C - 35°C: Three LEDs light up, with the third LED getting brighter.
- Above 35°C: All four LEDs light up, indicating very high temperatures.

## Code Explanation
1. **Temperature Control:** 
   - The temperature is manually controlled for testing purposes, but can be adapted to use actual sensor input.
   - The Celsius to Fahrenheit conversion is handled by `celsius_to_fahrenheit()` function.
   
2. **7-Segment Display:**
   - Shift registers (74HC595) are used to control the 7-segment displays.
   - The `display_digit()` function updates each digit using the `shift_out()` function.
   
3. **LED Indicator Mode:**
   - Uses PWM to vary LED brightness depending on the temperature range.
   - The `LED_Indicator()` function manages the LED brightness levels.

4. **Button Handling:**
   - Button states are debounced and processed in the `check_buttons()` function.
   - Various button actions control temperature adjustment, unit toggle, and mode switching.

## Usage Instructions
1. **Connect the microcontroller** and peripherals according to the pin configuration.
2. **Upload the code** to your microcontroller (using AVR or other suitable programming tools).
3. **Use the buttons** to interact with the system:
   - Adjust temperature, toggle units, and switch between display modes.
4. The **LED alarm** will activate if the temperature exceeds 40°C.

## Dependencies
- **AVR Library**: For the ATmega microcontroller.
- **<util/delay.h>**: For delays in the display update.
- **<stdlib.h>**: Used for the `abs()` function.

## License
This project is open-source and can be modified or used for educational purposes.
