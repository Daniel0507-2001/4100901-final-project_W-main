#include "gui.h"

#include "ssd1306.h"        // Librería para controlar la pantalla OLED SSD1306
#include "ssd1306_fonts.h"   // Librería que contiene las fuentes de texto para la pantalla OLED

// Declaraciones externas para las imágenes
extern const uint8_t locked[];    // Imagen de "Locked" (bloqueado)
extern const uint8_t Unlocked[];  // Imagen de "Unlocked" (desbloqueado)
extern const uint8_t Wait[];
// Función para inicializar la interfaz gráfica de usuario (GUI)
void GUI_init(void)
{
    // Inicializa la pantalla OLED SSD1306
    ssd1306_Init();
    // Muestra la pantalla de bloqueo inicialmente
    GUI_locked();
}

//Función para mostrar la pantalla de "Locked" (bloqueado)
void GUI_locked(void)
{
    // Llena la pantalla con color negro
    ssd1306_Fill(Black);
    // Establece la posición del cursor y escribe el texto "Locked" en la pantalla
    ssd1306_SetCursor(33, 5);
    ssd1306_WriteString("CLOSED", Font_11x18, White);
    // Dibuja el icono de "locked" (bloqueado) en la pantalla
    ssd1306_DrawBitmap(50, 35, Wait, 30, 30, White);
    // Actualiza la pantalla OLED para mostrar los cambios
    ssd1306_UpdateScreen();
}

// Función para mostrar la pantalla de "Unlocked" (desbloqueado)
void GUI_unlocked(void)
{
    // Llena la pantalla con color negro
    ssd1306_Fill(Black);
    // Establece la posición del cursor y escribe el texto "Unlocked" en la pantalla
    ssd1306_SetCursor(40, 5);
    ssd1306_WriteString("OPEN", Font_11x18, White);
    // Dibuja el icono de "Unlocked" (desbloqueado) en la pantalla
    ssd1306_DrawBitmap(50, 35, Unlocked, 30, 30, White);
    // Actualiza la pantalla OLED para mostrar los cambios
    ssd1306_UpdateScreen();
}

// Función para mostrar la pantalla de "Fail" (fallo)
void GUI_Fail(void)
{
    // Llena la pantalla con color negro
    ssd1306_Fill(Black);
    // Establece la posición del cursor y escribe el texto "Fail" en la pantalla
    ssd1306_SetCursor(35, 5);
    ssd1306_WriteString("ERROR", Font_11x18, White);
    // Dibuja el icono de "locked" (bloqueado) en la pantalla
    ssd1306_DrawBitmap(50, 35, locked, 30, 30, White);
    // Actualiza la pantalla OLED para mostrar los cambios
    ssd1306_UpdateScreen();
}
void BUZZER_ON(void) {

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); // Enciende el LED en PA5.

}
void BUZZER_OFF(void) {

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // Apaga el LED en PA5.
}


// Función para mostrar la pantalla de "Blocked" (bloqueado temporalmente)
// Función para mostrar la pantalla de "Retry" (reintento en 1 minuto) con cuenta regresiva
void GUI_Retry_Countdown(void)
{
    char buffer[20];  // Buffer para almacenar el texto a mostrar
    uint8_t seconds = 5;  // Número de segundos para la cuenta regresiva

    // Bucle de cuenta regresiva de 5 segundos
    for (uint8_t i = seconds; i > 0; i--)
    {
    	BUZZER_ON();
        // Llena la pantalla con color negro
        ssd1306_Fill(Black);

        // Escribe el texto "Retry in X seconds"
        snprintf(buffer, sizeof(buffer), "Retry  %ds", i);
        ssd1306_SetCursor(7, 5);
        ssd1306_WriteString(buffer, Font_11x18, White);

        // Dibuja el icono de "locked" (bloqueado) en la pantalla
        ssd1306_DrawBitmap(50, 35, Wait, 30, 30, White);

        // Actualiza la pantalla OLED para mostrar los cambios
        ssd1306_UpdateScreen();

        // Espera 1 segundo antes de continuar con la cuenta regresiva
        HAL_Delay(1000);
    }

    // Una vez que la cuenta regresiva termina, se puede mostrar un mensaje de reintento o ejecutar otra acción
    BUZZER_OFF();
    ssd1306_Fill(Black);
    ssd1306_SetCursor(5, 25);
    ssd1306_WriteString("Retry again", Font_11x18, White);
    ssd1306_UpdateScreen();
}

// Función para inicializar la pantalla de actualización de contraseña
void GUI_update_password_init(void)
{
    // Llena la pantalla con color negro
    ssd1306_Fill(Black);
    // Establece la posición del cursor y escribe el texto "New PW:" (Nueva contraseña) en la pantalla
    ssd1306_SetCursor(20, 2);
    ssd1306_WriteString("New ", Font_11x18, White);
    ssd1306_SetCursor(20, 20);
    ssd1306_WriteString("Password:", Font_11x18, White);
    // Actualiza la pantalla OLED para mostrar los cambios
    ssd1306_UpdateScreen();
}

// Función para actualizar y mostrar la contraseña ingresada durante la actualización de contraseña
void GUI_update_password(uint8_t *password)
{
    // Establece la posición del cursor y escribe la contraseña ingresada
    ssd1306_SetCursor(20, 45);
    ssd1306_WriteString((char *)password, Font_7x10, White);
    // Actualiza la pantalla OLED para mostrar los cambios
    ssd1306_UpdateScreen();
}

// Función para mostrar la pantalla de éxito tras actualizar la contraseña
void GUI_update_password_success(void)
{
    // Establece la posición del cursor y escribe el texto "Success!" (¡Éxito!) en la pantalla
    ssd1306_SetCursor(5, 35);
    ssd1306_WriteString("Success!", Font_16x26, White);
    // Actualiza la pantalla OLED para mostrar los cambios
    ssd1306_UpdateScreen();
}
