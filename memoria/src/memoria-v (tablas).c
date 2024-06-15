//esto es UNA PRUEBA de intento de armar la memoria por la parte de tablas de página

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <commons/config.h>

#include <utils/conexion.h>
#include <utils/planificadores.h>
#include "memoria_gestor.h"
#include "inicializar_memoria.h"
#include "memoria_entradasalida.h"
#include "memoria_kernel.h"
#include "memoria_cpu.h"

t_tabla_paginas *crear_tabla_paginas(int pid, int cantidad_paginas) {
    tabla_paginas_t *tabla = malloc(sizeof(t_tabla_paginas));
    tabla->pid = pid;
    tabla->paginas = malloc(sizeof(t_pagina) * cantidad_paginas);
    for (int i = 0; i < cantidad_paginas; i++) {
        tabla->paginas[i].marco = -1;
        tabla->paginas[i].presente = 0;
    }
    return tabla;
}


void manejar_lectura(int pid, int direccion_fisica) {
    // Implementar la lógica para leer de la memoria
    printf("Leyendo de la dirección física: %d para el PID: %d\n", direccion_fisica, pid);
    // Simular retardo
    usleep(1000);
}

void manejar_escritura(int pid, int direccion_fisica, int valor) {
    // Implementar la lógica para escribir en la memoria
    printf("Escribiendo en la dirección física: %d para el PID: %d\n", direccion_fisica, pid);
    // Simular retardo
    usleep(1000);
}

int main() {
    void *memoria;
    inicializar_memoria(memoria, TAM_MEMORIA);

    int pid = 1;
    int cantidad_paginas = TAM_MEMORIA / TAM_PAGINA;
    tabla_paginas_t *tabla = crear_tabla_paginas(pid, cantidad_paginas);

    // Ejemplo de manejo de lectura/escritura
    manejar_lectura(pid, 100);
    manejar_escritura(pid, 100, 42);

    // Liberar memoria
    free(tabla->paginas);
    free(tabla);
    free(memoria);

    return 0;
}

