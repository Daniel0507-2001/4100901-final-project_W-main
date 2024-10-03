# 4100901-final-project_GUARDIANX


# Proyecto GUARDIANX
 
Este proyecto implementa un sistema de acceso de seguridad, utilizando un microcontrolador STM32L4 como controlador principal. El sistema permite la gestión de la seguridad a través de un teclado matricial para la entrada de secuencias, una pantalla OLED como interfaz gráfica de usuario (GUI), este sistema de seguridad  se utiliza comunicación UART para interactuar con un computador.


## Características

- **Contraseña predeterminada:** La cerradura comienza con la contraseña "0000".
- **Interfaz gráfica (GUI):** Se utiliza una pantalla OLED SSD1306 para mostrar el estado de la cerradura (bloqueado, desbloqueado, error, etc.).
- **Teclado matricial:** El usuario puede ingresar la contraseña mediante un teclado matricial.
- **LED indicador:** Un LED conectado al pin PA5 indica si la cerradura está abierta o cerrada.
- **Buzzer:**  Un buzzer conectado al pin PA0 se activará junto con el LED cuando se abra la cerradura.
- **Bloqueo temporal:** Después de tres intentos fallidos, la cerradura se bloquea temporalmente por 5 segundos, mostrando una cuenta regresiva en la pantalla.
- **Actualización de contraseña:** El usuario puede actualizar la contraseña ingresando una secuencia especial de teclas.

## Instrucciones de Uso

1. **Iniciar el sistema:**
   Al encender el dispositivo, la pantalla OLED mostrará el estado de "bloqueado" ("Locked").

2. **Ingresar la contraseña:**
   - Utiliza el teclado matricial para ingresar la contraseña predeterminada (0000).
   - Si la contraseña es correcta, la pantalla mostrará "desbloqueado" ("Unlocked") y se encenderá el LED.

3. **Cambio de contraseña:**
   - se ingresa la contraseña actual y se presiona la tecla `*` para iniciar el proceso de cambio de contraseña.
   - Ingresa la nueva contraseña usando el teclado matricial y confirma presionando `#`.

4. **Intentos fallidos:**
   - Si ingresas una contraseña incorrecta tres veces consecutivas, la cerradura se bloqueará temporalmente durante 5 segundos. Se mostrará una cuenta regresiva en la pantalla.
   - durante los 5 segundos se activa una alarma buzzer 

### Funciones Clave:
- **`lock_open_lock`**: Abre la cerradura si la contraseña es válida.
- **`lock_validate_password`**: Valida la contraseña ingresada con la almacenada.
- **`lock_get_password`**: Permite al usuario ingresar una nueva contraseña.
- **`LED_SetHigh` y `LED_SetLow`**: Controlan el encendido y apagado del LED.
- **`GUI_locked`, `GUI_unlocked`, `GUI_Fail`, `GUI_Retry_Countdown`**: Funciones para actualizar el estado de la pantalla OLED.


## Requisitos de Hardware

- Microcontrolador STM32L476 
- Pantalla OLED SSD1306 conectada por I2C
- Teclado matricial 4x4
- LED conectado al pin PA5
- Buzzer conectado al pin PA0

## Mejoras Futuras

- implementar comunicacion wifi mediante una esp32 para monitoriar el sistema de manera remota.
  
