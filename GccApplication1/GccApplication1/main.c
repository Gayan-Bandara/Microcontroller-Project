#define F_CPU 16000000UL  // 16 MHz clock speed
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>  // For abs() function

// Pin definitions
#define SHIFT_DATA  PB0   // 74HC595 SER   (pin 14)
#define SHIFT_CLK   PB3   // 74HC595 SRCLK (pin 11)
#define SHIFT_LATCH PB4   // 74HC595 RCLK  (pin 12)
#define  SHIFT_EN   PD7
#define DIGIT_1     PC0
#define DIGIT_2     PC1
#define DIGIT_3     PC2
#define DIGIT_4     PC3   // New digit for C/F display
#define BTN_INC     PD2   // Increment button on PD2 (INT0)
#define BTN_DEC     PD3   // Decrement button on PD3 (INT1)
#define BTN_TOGGLE  PD4   // Toggle Celsius/Fahrenheit button on PD4
#define ALARM_LED   PB5
#define BTN_ACK     PD1
#define BTN_MODE    PD0

#define DEBOUNCE_TIME 1  // 50ms debounce time

volatile int temperature = 15;
volatile uint8_t is_celsius = 1;  // 1 for Celsius, 0 for Fahrenheit
volatile uint8_t button_state = 0;
volatile uint8_t led_off = 0;  // Flag to indicate if LED should be off
volatile uint8_t display_mode = 0;  // 0 for Seven-segment, 1 for LED_Indicator



// Segment patterns for digits 0-9 and C, F
const uint8_t SEGMENT_MAP[] = {
	0x3f, 0x30, 0x5b, 0x79, 0x74, 0x6d, 0x6f, 0x38, 0x7f, 0x7d,
	0x8f,  // C (index 10)
	0xCe   // F (index 11)
};

// Function prototypes
void shift_out(uint8_t data);
void display_digit(uint8_t digit, uint8_t number);
void check_buttons();
int celsius_to_fahrenheit(int celsius);
void LED_Indicator();

void shift_out(uint8_t data) {
	for (int i = 7; i >= 0; i--) {
		if (data & (1 << i))
		PORTB |= (1 << SHIFT_DATA);
		else
		PORTB &= ~(1 << SHIFT_DATA);
		
		PORTB |= (1 << SHIFT_CLK);
		PORTB &= ~(1 << SHIFT_CLK);
	}
	PORTB |= (1 << SHIFT_LATCH);
	PORTB &= ~(1 << SHIFT_LATCH);
}

void display_digit(uint8_t digit, uint8_t number) {
	// Turn off all digits
	PORTC |= (1 << DIGIT_1) | (1 << DIGIT_2) | (1 << DIGIT_3);
	
	// Output segment data
	shift_out(SEGMENT_MAP[number]);
	
	// Enable the selected digit
	PORTC &= ~(1 << (DIGIT_1 + digit));
}

void check_buttons() {
	static uint8_t last_state = 0;
	static uint8_t debounce_count = 0;
	
	uint8_t current_state = PIND & ((1 << BTN_INC) | (1 << BTN_DEC) | (1 << BTN_TOGGLE) | (1 << BTN_MODE) | (1 << BTN_ACK));
	
	if (current_state != last_state) {
		debounce_count = DEBOUNCE_TIME;
		} else if (debounce_count > 0) {
		debounce_count--;
		if (debounce_count == 0) {
			if ((current_state & (1 << BTN_INC)) == 0) {
				temperature++;
				if (temperature > 100) temperature = 100;
			}
			if ((current_state & (1 << BTN_DEC)) == 0) {
				temperature--;
				if (temperature < 0) temperature = 0;
			}
			if ((current_state & (1 << BTN_TOGGLE)) == 0) {
				is_celsius = !is_celsius;
			}
			if ((current_state & (1 << BTN_ACK)) == 0 && temperature > 40) {
				led_off = 1;  // Set flag to turn off LED
				PORTB &= ~(1 << ALARM_LED);  // Turn off LED immediately
			}
			if ((current_state & (1 << BTN_MODE)) == 0) {
				display_mode = !display_mode;  // Toggle between Seven-segment and LED_Indicator
			}
		}
	}
	
	last_state = current_state;
}

int celsius_to_fahrenheit(int celsius) {
	return (celsius * 9 / 5) + 32;
}

void turn_off_leds(void) {
	DDRD &= ~((1 << DDD5) | (1 << DDD6));
	DDRB &= ~((1 << DDB2) | (1 << DDB1));
	PORTD &= ~((1 << PD5) | (1 << PD6));
	PORTB &= ~((1 << PB2) | (1 << PB1));
}

void LED_Indicator()
{
	turn_off_leds();
	if (temperature < 15) // check variable A below 15
	{
		TCCR0A = (1 << WGM00) | (1 << WGM01) | (1 << COM0B1); // set PWM mode and pin
		TCCR0B = (1 << CS01); // set pre scaler
		DDRD |= (1 << DDD5);  // set PD5 pin output
		
		OCR0B = (temperature * 17); // set OCR value (0-255)
	}
	else if (temperature >= 15 && temperature <= 25) // Check variable A value between 15 and 25
	{
		TCCR0A = (1 << WGM00) | (1 << WGM01) | (1 << COM0A1) | (1 << COM0B1); // set PWM mode and pin
		TCCR0B = (1 << CS01); // set pre scaler
		DDRD |= (1 << DDD5) | (1 << DDD6); // set PD5 and PD6 pin output
		
		OCR0B = 255; // Max brightness for first LED
		OCR0A = ((temperature - 15) * 25); // set OCR value (0-255) for second LED
	}
	else if (temperature > 25 && temperature < 35) // check variable A between 26 and 35
	{
		TCCR0A = (1 << WGM00) | (1 << WGM01) | (1 << COM0A1) | (1 << COM0B1); // set PWM mode and pin
		TCCR0B = (1 << CS01);  // set pre scaler
		DDRD |= (1 << DDD5) | (1 << DDD6); // set PD5 and PD6 pin output
		
		TCCR1A = (1 << WGM10) | (1 << COM1A1);  // set PWM mode and pin
		TCCR1B = (1 << WGM12) | (1 << CS11); // set pre scaler
		DDRB |= (1 << PB1); // set PB1 pin output
		
		OCR0B = 255; // Max brightness for first LED
		OCR0A = 255; // Max brightness for second LED
		OCR1A = ((temperature - 25) * 25); // set OCR value (0-255) for third LED
	}
	else if (temperature >= 35) // check variable A above 35
	{
		TCCR0A = (1 << WGM00) | (1 << WGM01) | (1 << COM0A1) | (1 << COM0B1);  // set PWM mode and pin
		TCCR0B = (1 << CS01);  // set pre scaler
		DDRD |= (1 << DDD5) | (1 << DDD6); // set PD5 and PD6 pin output
		
		TCCR1A = (1 << COM1B1) | (1 << WGM10) | (1 << COM1A1); // set PWM mode and pin
		TCCR1B = (1 << WGM12) | (1 << CS11); // set pre scaler
		DDRB |= (1 << PB1) | (1 << PB2); // set PB1 and PB2 pin output
		
		OCR0B = 255; // Max brightness for first LED
		OCR0A = 255; // Max brightness for second LED
		OCR1A = 255; // Max brightness for third LED
		OCR1B = ((temperature - 35) * 12); // set OCR value (0-255) for fourth LED
	}
}

int main(void) {
	// Set up shift register pins as outputs
	DDRB |= (1 << SHIFT_DATA) | (1 << SHIFT_CLK) | (1 << SHIFT_LATCH);
	
	// Set up digit select pins as outputs
	DDRC |= (1 << DIGIT_1) | (1 << DIGIT_2) | (1 << DIGIT_3) | (1 << DIGIT_4);
	
	// Set up button pins as inputs with pull-ups
	DDRD &= ~((1 << BTN_INC) | (1 << BTN_DEC) | (1 << BTN_TOGGLE) | (1 << BTN_ACK) | (1 << BTN_MODE));
	PORTD |= (1 << BTN_INC) | (1 << BTN_DEC) | (1 << BTN_TOGGLE) | (1 << BTN_ACK) | (1 << BTN_MODE);
	
	DDRB |= (1 << ALARM_LED);
	DDRD |= (1 << SHIFT_EN);
	PORTB &= ~(1 << ALARM_LED);
	
	
	int display_value;
	uint8_t digits[4];
	
	while (1) {
		check_buttons();
		
		if (is_celsius) {
			display_value = temperature;
			digits[3] = 10;  // 'C' for Celsius
			} else {
			display_value = celsius_to_fahrenheit(temperature);
			digits[3] = 11;  // 'F' for Fahrenheit
		}
		
		// Prepare digits
		if (display_value < 0) {
			temperature = 0;
			} else {
			digits[0] = display_value / 100 % 10;
		}
		digits[1] = (display_value / 10) % 10;
		digits[2] = display_value % 10;
		
		if (display_mode == 0) {
			turn_off_leds();
			PORTD &= ~(1 << SHIFT_EN);
			// Seven-segment display mode
			// Display each digit multiple times for smoother appearance
			for (int repeat = 0; repeat < 20; repeat++) {
				for (int i = 0; i < 4; i++) {
					// Turn off all digits
					PORTC |= (1 << DIGIT_1) | (1 << DIGIT_2) | (1 << DIGIT_3) | (1 << DIGIT_4);
					
					// Output segment data
					shift_out(SEGMENT_MAP[digits[i]]);
					
					// Enable the current digit
					PORTC &= ~(1 << (DIGIT_1 + i));
					
					// Wait for a short time
					_delay_us(1);  // Adjust this delay as needed for smooth display
				}
			}
			} else {
			// LED_Indicator mode
			PORTD |= (1 << SHIFT_EN);
			LED_Indicator();
		}
		
		if (!led_off && temperature > 40) {
			PORTB |= (1 << ALARM_LED);  // Turn on LED
			} else {
			PORTB &= ~(1 << ALARM_LED);  // Turn off LED
		}
	}
	
	return 0;
}