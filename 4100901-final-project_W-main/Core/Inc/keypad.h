
#ifndef __KEYPAD_
#define __KEYPAD_

#include <stdint.h>


#define KEYPAD_EVENT_NONE 0xFFFF
#define KEY_PRESSED_NONE 0xFF


void keypad_init(void);
uint8_t keypad_handler(uint16_t column_to_evaluate);

static inline uint8_t keypad_run(volatile uint16_t *keypad_event) {
	uint8_t key_pressed = KEY_PRESSED_NONE;
	if (*keypad_event != KEYPAD_EVENT_NONE){
		key_pressed = keypad_handler(*keypad_event);
		*keypad_event = KEYPAD_EVENT_NONE;
	}
	return key_pressed;
}

#endif /* __KEYPAD_ */
