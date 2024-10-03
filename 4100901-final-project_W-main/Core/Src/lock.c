#include "lock.h"
#include "ring_buffer.h"
#include "keypad.h"
#include "main.h"
#include "gui.h"

#include <stdio.h>
#include <string.h>

#define MAX_PASSWORD 12

// Counter for failed password attempts
uint8_t failed_counter = 0;

// Current password and keypad buffer
uint8_t password[MAX_PASSWORD] = "2001";
uint8_t keypad_buffer[MAX_PASSWORD];
ring_buffer_t keypad_rb;

// Variable to store the detected keypad event
extern volatile uint16_t keypad_event;

// Function to get a single key press from the keypad
static uint8_t lock_get_passkey(void)
{
    while (ring_buffer_size(&keypad_rb) == 0) {
        // Wait for key press
        uint8_t key_pressed = keypad_run(&keypad_event);
        if (key_pressed != KEY_PRESSED_NONE) {
            ring_buffer_put(&keypad_rb, key_pressed);
        }
    }

    // Retrieve and return the key pressed
    uint8_t key_pressed;
    ring_buffer_get(&keypad_rb, &key_pressed);
    if (key_pressed == '*' || key_pressed == '#') {
        return 0xFF; // Special code for '*' and '#' keys
    }
    return key_pressed;
}

// Function to get a new password from the user
static uint8_t lock_get_password(void)
{
    uint8_t idx = 0;
    uint8_t passkey = 0;
    uint8_t new_password[MAX_PASSWORD];
    memset(new_password, 0, MAX_PASSWORD);
    uint8_t password_shadow[MAX_PASSWORD + 1]  = {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '\0'};

    // Continue getting keys until '#' is pressed
    while (passkey != 0xFF) {
        // Display the password shadow on the GUI
        GUI_update_password(password_shadow);

        // Get the key pressed and update the display
        passkey = lock_get_passkey();
        password_shadow[idx] = '*';
        new_password[idx++] = passkey;
        GUI_update_password(new_password);

        // Delay to make the key presses visible on the GUI
        HAL_Delay(200);
    }

    // If a valid password is entered, update the password and GUI
    if (idx > 1) {
        memcpy(password, new_password, MAX_PASSWORD);
        GUI_update_password_success();
        printf("Password update success\n");
    } else {
        // If an invalid password is entered, display "Locked" on the GUI
        GUI_locked();
        printf("Password update failed\n");
        return 0;
    }
    return 1; // Return success
}

// Function to validate the entered password
static uint8_t lock_validate_password(void)
{
    uint8_t sequence[MAX_PASSWORD];
    uint8_t seq_len = ring_buffer_size(&keypad_rb);

    // Retrieve the sequence from the keypad buffer
    for (uint8_t idx = 0; idx < seq_len; idx++) {
        ring_buffer_get(&keypad_rb, &sequence[idx]);
    }

    // Compare the entered sequence with the stored password
    if (memcmp(sequence, password, 4) == 0) {
        printf("Password validation success\n");
        return 1; // Password is valid
    }
    printf("Password validation failed\n");
    return 0; // Password is invalid
}

// Function to update the password if the validation is successful
static void lock_update_password(void)
{
    if (lock_validate_password() != 0) {
        // If password is valid, initiate the password update process
        GUI_update_password_init();
        lock_get_password();
    } else {
        // If password is invalid, display "Locked" on the GUI
        GUI_locked();
    }
}



// Función para encender el LED.
void LED_SetHigh(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // Enciende el LED en PA5.

}

// Función para apagar el LED.
void LED_SetLow(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // Apaga el LED en PA5.

}

// Function to handle the process of opening the lock
static void lock_open_lock(void)
{
    if (lock_validate_password() != 0) {
        // If password is valid, display "Unlocked" on the GUI
        GUI_unlocked();
        printf("Lock opened successfully (OPEN)\n");

        // Enciende el LED en PA5 para indicar que está desbloqueado.
        LED_SetHigh();
    } else {
        failed_counter++;

        if (failed_counter < 3) {
            // If the password is incorrect, show "Failed" and wait for 3 seconds
            GUI_Fail();
            printf("Failed attempt: %d\n", failed_counter);
            HAL_Delay(3 * 1000);
        } else {
            // If the password is incorrect 3 times, show "Blocked" and wait for 10 seconds
            GUI_Retry_Countdown();
            failed_counter = 0;
            printf("Too many failed attempts, blocked for 60 seconds\n");
            HAL_Delay(10 * 1000);  // Cambio de 60s a 10s de espera.
        }

        // Apaga el LED en PA5 para indicar que está bloqueado o falló la autenticación.
        LED_SetLow();

        // After the delay, display "Locked" on the GUI
        GUI_locked();
        printf("Lock is now in LOCKED state\n");
    }
}

// Initialization function for the lock system
void lock_init(void)
{
    // Initialize keypad ring buffer, GUI, etc.
    ring_buffer_init(&keypad_rb, keypad_buffer, 12);
    GUI_init();
}

// Function to handle the sequence of keys pressed on the lock
void lock_sequence_handler(uint8_t key)
{
    if (key == '*') {
        // If '*' is pressed, initiate the password update process
        printf("Key pressed: *\n");
        lock_update_password();
    } else if (key == '#') {
        // If '#' is pressed, attempt to open the lock
        printf("Key pressed: #\n");
        lock_open_lock();
    } else {
        // If any other key is pressed, add it to the keypad buffer
        printf("Key pressed: %c\n", key);
        ring_buffer_put(&keypad_rb, key);
    }
}

/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len)
{
    for (int i = 0; i < len; i++) {
        if (ptr[i] == '\n') {
            // Si encuentra un salto de línea, primero envía un retorno de carro
            while (LL_USART_IsActiveFlag_TXE(USART2) == 0);
            LL_USART_TransmitData8(USART2, '\r');
        }
        // Luego envía el carácter original
        while (LL_USART_IsActiveFlag_TXE(USART2) == 0);
        LL_USART_TransmitData8(USART2, ptr[i]);
    }
    return len;
}
/* USER CODE END 0 */

