#include "ring_buffer.h"

/**
 * @brief Inicializa la estructura de control del buffer circular
 * @param ring_buffer: Puntero a la estructura de control del buffer circular
 * @param buffer: Puntero al buffer de datos
 * @param capacity: Capacidad del buffer circular
 */
void ring_buffer_init(ring_buffer_t *ring_buffer, uint8_t *buffer, uint16_t capacity)
{
    ring_buffer->buffer = buffer;
    ring_buffer->capacity = capacity;

    ring_buffer->head = 0;
    ring_buffer->tail = 0;
    ring_buffer->is_full = 0;
}

/**
 * @brief Agrega datos al buffer circular
 * @param ring_buffer: Puntero a la estructura de control del buffer circular
 * @param data: Valor a agregar
 * @return 1 si se realizó con éxito, 0 si el buffer está lleno
 */
uint8_t ring_buffer_put(ring_buffer_t *ring_buffer, uint8_t data)
{
    ring_buffer->buffer[ring_buffer->head] = data;
    ring_buffer->head = (ring_buffer->head + 1) % ring_buffer->capacity;

    if (ring_buffer->is_full != 0)
    {
        ring_buffer->tail = (ring_buffer->tail + 1) % ring_buffer->capacity;
    }

    if (ring_buffer->head == ring_buffer->tail)
    {
        ring_buffer->is_full = 1;
        return 0; // El buffer está lleno
    }
    return 1; // Éxito
}

/**
 * @brief Lee datos del buffer circular
 * @param ring_buffer: Puntero a la estructura de control del buffer circular
 * @param data: Puntero a la dirección donde escribir los datos
 * @return 1 si se realizó con éxito, 0 si el buffer está vacío
 */
uint8_t ring_buffer_get(ring_buffer_t *ring_buffer, uint8_t *data)
{
    if (ring_buffer_is_empty(ring_buffer) == 0)
    {
        *data = ring_buffer->buffer[ring_buffer->tail];
        ring_buffer->tail = (ring_buffer->tail + 1) % ring_buffer->capacity;
        ring_buffer->is_full = 0;
        return 1; // Éxito
    }
    return 0; // El buffer está vacío
}

/**
 * @brief Devuelve el tamaño del buffer circular
 * @param ring_buffer: Puntero a la estructura de control del buffer circular
 * @return Tamaño del buffer circular
 */
uint16_t ring_buffer_size(ring_buffer_t *ring_buffer)
{
    uint16_t size = 0;

    if ((ring_buffer->is_full == 0) && (ring_buffer->tail <= ring_buffer->head))
    {
        size = ring_buffer->head - ring_buffer->tail;
    }
    else
    {
        size = ring_buffer->head + (ring_buffer->capacity - ring_buffer->tail);
    }

    return size;
}

/**
 * @brief Verifica si el buffer está vacío
 * @param ring_buffer: Puntero a la estructura de control del buffer circular
 * @return 1 si está vacío, 0 si no está vacío
 */
uint8_t ring_buffer_is_empty(ring_buffer_t *ring_buffer)
{
    return ((ring_buffer->is_full == 0) && (ring_buffer->tail == ring_buffer->head));
}

/**
 * @brief Verifica si el buffer está lleno
 * @param ring_buffer: Puntero a la estructura de control del buffer circular
 * @return 1 si está lleno, 0 si no está lleno
 */
uint8_t ring_buffer_is_full(ring_buffer_t *ring_buffer)
{
    return (ring_buffer->is_full != 0);
}

/**
 * @brief Restablece el buffer para que esté vacío
 * @param ring_buffer: Puntero a la estructura de control del buffer circular
 */
void ring_buffer_reset(ring_buffer_t *ring_buffer)
{
    ring_buffer->tail = 0;
    ring_buffer->head = 0;
    ring_buffer->is_full = 0;
}

