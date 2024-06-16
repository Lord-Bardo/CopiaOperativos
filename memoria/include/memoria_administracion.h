#ifndef MEMORIA_ADMINISTRACION_H_
#define MEMORIA_ADMINISTRACION_H_

// Copie los include de memoria.h pero no se cuáles va a necesitar en realidad.
#include "memoria_gestor.h"
#include "inicializar_memoria.h"
#include "memoria_entradasalida.h"
#include "memoria_kernel.h"
#include "memoria_cpu.h"
#include "instrucciones.c" // No sé si está bien esto.

//VARIABLES GLOBALES
void* espacio_usuario; 
void* puntero_espacio_usuario;
size_t num_lineas = 0; // Número de líneas leídas de un archivo de pseudocodigo.
typedef struct {
    int pid;
    char* path_pseudocodigo;
    t_tabla_paginas *tabla_de_pags; //puntero a su tabla de paginas
    char** memoria_de_instrucciones;
} t_pcb_memoria;
typedef struct {
    int frame;
    void *frame_pointer;
    bool presencia;
} t_pagina;
typedef struct { //para las tablas entrar al dictionary.h para ver la estructura de t_config
    int pid;
    pagina_t *paginas;
} t_tabla_paginas tablas[TAM_MEMORIA/TAM_PAGINA]; //defino array de tablas de paginas para persistirlas a todas las que vaya creando.

//FUNCIONES



#endif