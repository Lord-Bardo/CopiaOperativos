#ifndef MEMORIA_KERNEL_H_
#define MEMORIA_KERNEL_H_

#include "memoria.h"
#include "memoria_utils.h"

//VARIABLES GLOBALES
extern void* espacio_usuario; 
extern void* puntero_espacio_usuario;
extern t_pcb_memoria* procesos; // En esta lista voy a ir colocando todos mis procesos.
extern size_t num_instruccion; // Número de instrucciones leídas de un archivo de pseudocodigo.

//FUNCIONES
void atender_memoria_kernel(void);
void crear_proceso(t_pcb_memoria *proceso);
void finalizar_proceso(int pid);

#endif