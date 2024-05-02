#include <assert.h>
#include <string.h>
#include "conexion.h"


void test_deserializar() {
    // Caso de prueba 1: Buffer con un mensaje válido
    {
        char* mensaje_original = "Hola, mundo!";
        int size_mensaje = strlen(mensaje_original) + 1; // +1 para el caracter nulo al final
        t_buffer* buffer = serializar(mensaje_original, size_mensaje);

        void* mensaje_deserializado = deserializar(buffer);
        assert(strcmp(mensaje_original, mensaje_deserializado) == 0);
        assert(buffer->size == 0);
        assert(buffer->stream == NULL);

        free(mensaje_deserializado);
        free(buffer);
    }

    // Caso de prueba 2: Buffer vacío
    {
        t_buffer* buffer = serializar("", 0);
        if(buffer->size > 0) {
            void* mensaje_deserializado = deserializar(buffer);
            free(mensaje_deserializado); // Por las dudas
        } else {
            printf("\n [ERROR] Al intentar extraer un contenido de un t buffer vacio o con tamaño negativo\n\n");
        }
        free(buffer);
    }
}

int main() {
    test_deserializar();
    printf("Todos los tests pasaron correctamente.\n");
    return 0;
}