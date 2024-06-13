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


#define TAM_MEMORIA 4096 //habría que ver el tema de la memoria cual tamaño
#define TAM_PAGINA 32 //y esto también

// Estructura para las tablas de páginas
typedef struct {
    int marco;
    int presente;
} pagina_t;

typedef struct { //para las tablas entrar al dictionary.h para ver la estructura de t_config
    int pid;
    pagina_t *paginas;
} tabla_paginas_t;

t_dictionary *diccionario_crear(int tamano_inicial) {
    t_dictionary *diccionario = malloc(sizeof(t_dictionary));
    diccionario->elementos = malloc(tamano_inicial * sizeof(t_hash_element*));
    for (int i = 0; i < tamano_inicial; i++) {
        diccionario->elementos[i] = NULL;
    }
    diccionario->tamano_maximo_tabla = tamano_inicial;
    diccionario->tamano_actual_tabla = 0;
    diccionario->cantidad_elementos = 0;
    return diccionario;
}

void inicializar_memoria(void *memoria, int tam_memoria) {
    memoria = malloc(tam_memoria);
    memset(memoria, 0, tam_memoria);
}

tabla_paginas_t *crear_tabla_paginas(int pid, int cantidad_paginas) {
    tabla_paginas_t *tabla = malloc(sizeof(tabla_paginas_t));
    tabla->pid = pid;
    tabla->paginas = malloc(sizeof(pagina_t) * cantidad_paginas);
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


/*Traduccion para mi para no perderme de las structs
typedef struct {
		t_hash_element **elements; // sería un puntero a un array de punteros a elementos hash. Hash todavía no sé
		int table_max_size; //tamaño max de la tabla (para evitar desbordamiento)
		int table_current_size; //tamaño actual (para controlar tamaño, por posible aumento o reducción)
		int elements_amount; //cant de elementos guardados para usar el rehashing (redimensionar la tabla)
	} t_dictionary;

struct hash_element{
		char *key; //clave del elemento
		unsigned int hashcode; //código hash de la clave
		void *data; //puntero a los datos almacenados en hash
		struct hash_element *next; //puntero al siguiente elemento (para manejo de colisiones) (todavía no lo entiendo el tema de colisiones)
	};
	typedef struct hash_element t_hash_element;