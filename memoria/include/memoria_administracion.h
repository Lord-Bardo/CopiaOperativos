#ifndef MEMORIA_ADMINISTRACION_H_
#define MEMORIA_ADMINISTRACION_H_

//copie los include de memoria.h pero no se cuáles va a necesitar en realidad
#include "memoria_gestor.h"
#include "inicializar_memoria.h"
#include "memoria_entradasalida.h"
#include "memoria_kernel.h"
#include "memoria_cpu.h"
#include "instrucciones.c" // no sé si está bien esto

//VARIABLES GLOBALES
void* espacio_usuario; // supongo que con un malloc se puede inicializar y supongo que es donde estarán los frames con las páginas.
void* puntero_espacio_usuario;
typedef struct {
    char* path_pseudocodigo;
    int pid;
} t_pcb_memoria;
typedef struct {
    int frame;
    void *frame_pointer;
    bool presencia;
} t_pagina;
typedef struct { //para las tablas entrar al dictionary.h para ver la estructura de t_config
    int pid;
    pagina_t *paginas;
} t_tabla_paginas;

//FUNCIONES


#endif