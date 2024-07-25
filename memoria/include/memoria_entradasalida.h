#ifndef MEMORIA_ENTRADASALIDA_H_
#define MEMORIA_ENTRADASALIDA_H_

#include "memoria_utils.h"

void atender_memoria_entradasalida(void *fd_entradasalida_void);

// Variables globales
extern void* espacio_usuario; // Memoria contigua simulada
extern int fd_entradasalida; // File descriptor de entrada/salida
extern t_log* memoria_logger; // Logger para el módulo de memoria
extern t_log* memoria_logger_min_y_obl; // Logger para el módulo de memoria

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// Funciones
//t_log* memoria_logger = log_create("memoria.log", "MEMORIA", 1, LOG_LEVEL_DEBUG);

t_pcb_memoria* obtener_proceso(int pid);

int escribir_memoria(uint32_t direccion_fisica, void* buffer, int tamanio);
int leer_memoria(uint32_t direccion_fisica, void* buffer, int tamanio);

// Declaración de la función obtener_proceso
t_pcb_memoria* obtener_proceso(int pid);

void enviar_datos(int socket, void* buffer, int size);

#endif