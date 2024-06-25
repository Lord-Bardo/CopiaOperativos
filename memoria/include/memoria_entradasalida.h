#ifndef MEMORIA_ENTRADASALIDA_H_
#define MEMORIA_ENTRADASALIDA_H_

#include "memoria_utils.h"

void atender_memoria_entradasalida(void);

// Variables globales
extern void* espacio_usuario; // Memoria contigua simulada
extern int fd_entradasalida; // File descriptor de entrada/salida
extern t_log* memoria_logger; // Logger para el módulo de memoria

// Funciones
t_log* memoria_logger = log_create("memoria.log", "MEMORIA", 1, LOG_LEVEL_DEBUG);

t_pcb_memoria* obtener_proceso(int pid);

int escribir_memoria(t_pcb_memoria* proceso, int num_pagina, int offset, void* buffer, int tamanio);
int leer_memoria(t_pcb_memoria* proceso, int num_pagina, int offset, void* buffer, int tamanio);

t_pcb_memoria* obtener_proceso(int pid);


#endif