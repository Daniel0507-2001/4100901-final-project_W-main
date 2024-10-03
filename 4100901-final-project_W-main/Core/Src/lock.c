#include "lock.h"
#include "ring_buffer.h"
#include "keypad.h"
#include "main.h"
#include "gui.h"

#include <stdio.h>
#include <string.h>

#define MAX_PASSWORD 12  // Define el tamaño máximo de la contraseña

// Contador de intentos fallidos de contraseña
uint8_t failed_counter = 0;

// Contraseña actual y buffer del teclado
uint8_t password[MAX_PASSWORD] = "2001";  // Contraseña predeterminada
uint8_t keypad_buffer[MAX_PASSWORD];      // Buffer donde se almacenan las teclas presionadas
ring_buffer_t keypad_rb;                  // Ring buffer para gestionar las teclas del teclado

// Variable para almacenar el evento detectado del teclado
extern volatile uint16_t keypad_event;

// Función para obtener una sola tecla presionada del teclado
static uint8_t lock_get_passkey(void)
{
    // Espera hasta que se detecte una tecla presionada
    while (ring_buffer_size(&keypad_rb) == 0) {
        // Ejecuta el teclado para detectar teclas presionadas
        uint8_t key_pressed = keypad_run(&keypad_event);
        if (key_pressed != KEY_PRESSED_NONE) {
            // Si se detecta una tecla, la almacena en el ring buffer
            ring_buffer_put(&keypad_rb, key_pressed);
        }
    }

    // Recupera y devuelve la tecla presionada
    uint8_t key_pressed;
    ring_buffer_get(&keypad_rb, &key_pressed);

    // Si se presiona '*' o '#', devuelve un código especial
    if (key_pressed == '*' || key_pressed == '#') {
        return 0xFF;  // Código especial para las teclas '*' y '#'
    }

    return key_pressed;  // Devuelve la tecla presionada
}

// Función para obtener una nueva contraseña del usuario
static uint8_t lock_get_password(void)
{
    uint8_t idx = 0;               // Índice para el ingreso de la contraseña
    uint8_t passkey = 0;           // Tecla presionada
    uint8_t new_password[MAX_PASSWORD];   // Arreglo para almacenar la nueva contraseña
    memset(new_password, 0, MAX_PASSWORD);  // Inicializa el arreglo de contraseña con ceros
    uint8_t password_shadow[MAX_PASSWORD + 1]  = {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '\0'};  // Sombra para mostrar los caracteres ingresados

    // Continua obteniendo teclas hasta que se presione '#'
    while (passkey != 0xFF) {
        // Muestra la sombra de la contraseña en la interfaz gráfica
        GUI_update_password(password_shadow);

        // Obtiene la tecla presionada y actualiza la pantalla
        passkey = lock_get_passkey();
        password_shadow[idx] = '*';       // Muestra '*' en lugar de la contraseña real
        new_password[idx++] = passkey;    // Almacena la tecla presionada en la nueva contraseña
        GUI_update_password(new_password);

        // Retardo para hacer visible cada tecla en la interfaz gráfica
        HAL_Delay(200);
    }

    // Si se ingresó una contraseña válida, actualiza la contraseña y la GUI
    if (idx > 1) {
        memcpy(password, new_password, MAX_PASSWORD);  // Copia la nueva contraseña al arreglo de contraseña
        GUI_update_password_success();                 // Muestra un mensaje de éxito en la interfaz gráfica
        printf("Password update success\n");           // Imprime un mensaje de éxito en la consola
    } else {
        // Si no se ingresó una contraseña válida, muestra la pantalla de "Locked"
        GUI_locked();
        printf("Password update failed\n");            // Imprime un mensaje de fallo en la consola
        return 0;
    }
    return 1;  // Retorna éxito
}

// Función para validar la contraseña ingresada
static uint8_t lock_validate_password(void)
{
    uint8_t sequence[MAX_PASSWORD];    // Arreglo para almacenar la secuencia de teclas ingresadas
    uint8_t seq_len = ring_buffer_size(&keypad_rb);  // Obtiene la longitud de la secuencia ingresada

    // Recupera la secuencia ingresada desde el buffer
    for (uint8_t idx = 0; idx < seq_len; idx++) {
        ring_buffer_get(&keypad_rb, &sequence[idx]);
    }

    // Compara la secuencia ingresada con la contraseña almacenada
    if (memcmp(sequence, password, 4) == 0) {
        printf("Password validation success\n");  // Si es correcta, imprime éxito
        return 1;  // La contraseña es válida
    }
    printf("Password validation failed\n");  // Si es incorrecta, imprime fallo
    return 0;  // La contraseña es inválida
}

// Función para actualizar la contraseña si la validación es exitosa
static void lock_update_password(void)
{
    if (lock_validate_password() != 0) {
        // Si la contraseña es válida, inicia el proceso de actualización
        GUI_update_password_init();
        lock_get_password();
    } else {
        // Si la contraseña es inválida, muestra la pantalla de "Locked"
        GUI_locked();
    }
}

// Función para encender el LED en el pin PA5
void LED_SetHigh(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);  // Enciende el LED
}

// Función para apagar el LED en el pin PA5
void LED_SetLow(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);  // Apaga el LED
}

// Función para manejar el proceso de abrir la cerradura
static void lock_open_lock(void)
{
    if (lock_validate_password() != 0) {
        // Si la contraseña es válida, muestra "Unlocked" en la interfaz gráfica
        GUI_unlocked();
        printf("Lock opened successfully (OPEN)\n");

        // Enciende el LED en PA5 para indicar que está desbloqueado
        LED_SetHigh();
    } else {
        failed_counter++;  // Incrementa el contador de intentos fallidos

        if (failed_counter < 3) {
            // Si la contraseña es incorrecta, muestra "Failed" y espera 3 segundos
            GUI_Fail();
            printf("Failed attempt: %d\n", failed_counter);
            HAL_Delay(3 * 1000);
        } else {
            // Si la contraseña es incorrecta 3 veces, muestra "Blocked" y espera 10 segundos
            GUI_Retry_Countdown();
            failed_counter = 0;
            printf("Too many failed attempts, blocked for 10 seconds\n");
            HAL_Delay(3 * 1000);  // Espera 3 segundos
        }

        // Apaga el LED en PA5 para indicar que está bloqueado
        LED_SetLow();

        // Después del retardo, muestra la pantalla de "Locked"
        GUI_locked();
        printf("Lock is now in LOCKED state\n");
    }
}

// Función de inicialización del sistema de la cerradura
void lock_init(void)
{
    // Inicializa el buffer de teclado, la interfaz gráfica, etc.
    ring_buffer_init(&keypad_rb, keypad_buffer, 12);
    GUI_init();
}

// Función para manejar la secuencia de teclas presionadas en la cerradura
void lock_sequence_handler(uint8_t key)
{
    if (key == '*') {
        // Si se presiona '*', inicia el proceso de actualización de contraseña
        printf("Key pressed: *\n");
        lock_update_password();
    } else if (key == '#') {
        // Si se presiona '#', intenta abrir la cerradura
        printf("Key pressed: #\n");
        lock_open_lock();
    } else {
        // Si se presiona cualquier otra tecla, la agrega al buffer del teclado
        printf("Key pressed: %c\n", key);
        ring_buffer_put(&keypad_rb, key);
    }
}

/* USER CODE BEGIN 0 */
// Redefinición de la función _write para enviar datos al puerto serie
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
