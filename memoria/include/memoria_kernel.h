#ifndef MEMORIA_KERNEL_H_
#define MEMORIA_KERNEL_H_

#include "memoria.h"
#include "memoria_utils.h"

//VARIABLES GLOBALES
void* espacio_usuario; 
void* puntero_espacio_usuario;
t_pcb_memoria* procesos; // En esta lista voy a ir colocando todos mis procesos.
size_t num_instruccion; // Número de instrucciones leídas de un archivo de pseudocodigo.

//FUNCIONES
void atender_memoria_kernel(void);
void crear_proceso(t_pcb_memoria *proceso);

#endif