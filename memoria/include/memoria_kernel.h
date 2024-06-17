#ifndef MEMORIA_KERNEL_H_
#define MEMORIA_KERNEL_H_

#include "memoria.h"
#include "memoria_utils.h"

//VARIABLES GLOBALES
void* espacio_usuario; 
void* puntero_espacio_usuario;
t_pcb_memoria procesos[TAM_MEMORIA/TAM_PAGINA]; // En este array voy a ir colocando todos mis procesos.
size_t num_lineas = 0; // Número de líneas leídas de un archivo de pseudocodigo.

//FUNCIONES
void atender_memoria_kernel(void);
void crear_proceso(char* path_proceso);

#endif