#ifndef MEMORIA_UTILS_H_
#define MEMORIA_UTILS_H_

#include "memoria_gestor.h"

int agregar_proceso_a_procesos(t_pcb_memoria proceso);

// MANEJO DE BUFFER.
void buffer_desempaquetar_proceso(t_buffer *buffer, t_pcb_memoria *proceso);
