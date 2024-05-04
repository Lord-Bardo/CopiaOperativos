#include <assert.h>
#include <string.h>
#include "conexion.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void test_deserializar() {  //no funciona porque el buffer no permite recibir un char
    // Caso de prueba 1: Buffer con un mensaje válido
    {
        char* mensaje_original = "Hola, mundo!";
        int size_mensaje = strlen(mensaje_original) + 1; // +1 para el caracter nulo al final
       // Corregir llamada a deserializar en el primer caso de prueba
        t_buffer* buffer = deserializar(mensaje_original);

        assert(buffer != NULL);
        assert(buffer->size == size_mensaje);
        assert(strcmp(buffer->stream, mensaje_original) == 0);

        free(buffer->stream);
        free(buffer);
    }

    // Caso de prueba 2: Buffer vacío
    {
        t_buffer* buffer = deserializar(NULL);
        assert(buffer != NULL);
        assert(buffer->size == 0);
        assert(buffer->stream == NULL);

        free(buffer);
    }
}

