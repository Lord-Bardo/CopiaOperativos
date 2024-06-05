#ifndef KERNEL_ESTADOS_H_
#define KERNEL_ESTADOS_H_

#include "kernel_gestor.h"
#include "kernel_pcb.h"

// FUNCIONES
t_estado *crear_estado(t_nombre_estado nombre_estado);
void eliminar_estado(t_estado *estado);
void pcb_destroyer(void *pcb);

t_list *estado_get_lista_procesos(t_estado *estado);
pthread_mutex_t *estado_get_mutex(t_estado *estado);
sem_t *estado_get_sem(t_estado *estado);

void estado_encolar_pcb(t_estado *estado, t_pcb *pcb);
t_pcb *estado_desencolar_primer_pcb(t_estado *estado);

#endif