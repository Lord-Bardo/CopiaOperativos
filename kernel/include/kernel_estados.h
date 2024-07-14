#ifndef KERNEL_ESTADOS_H_
#define KERNEL_ESTADOS_H_

#include "kernel_gestor.h"
#include "kernel_pcb.h"

// FUNCIONES
t_estado *crear_estado(t_nombre_estado nombre_estado);
void eliminar_estado(t_estado *estado);
void pcb_destroyer(void *pcb);

t_nombre_estado estado_get_nombre_estado(t_estado *estado);
t_list *estado_get_lista_procesos(t_estado *estado);
pthread_mutex_t *estado_get_mutex(t_estado *estado);
sem_t *estado_get_sem(t_estado *estado);

void estado_encolar_pcb(t_estado *estado, t_pcb *pcb);
t_pcb *estado_desencolar_primer_pcb(t_estado *estado);
t_pcb *estado_desencolar_pcb_por_pid(t_estado *estado, int pid);
bool pcb_comparar_pid(void *pcb, void *pid);
t_pcb *estado_rastrear_y_desencolar_pcb_por_pid(int pid);
int list_get_index(t_list *list, bool (*cutting_condition)(void *temp, void *target), void *target);
bool estado_contiene_pcbs(t_estado *estado);
t_estado *estado_rastrear_pcb_por_pid(int pid);

#endif