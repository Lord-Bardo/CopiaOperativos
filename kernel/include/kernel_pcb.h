#ifndef KERNEL_PCB_H_
#define KERNEL_PCB_H_

#include "kernel_gestor.h"
#include "kernel_utils.h"
#include "kernel_planificadores.h"
#include "kernel_estados.h"

// FUNCIONES
t_pcb *crear_pcb(int pid, char* path);
void eliminar_pcb(t_pcb *pcb);
int generar_pid(void);
int pcb_get_pid(t_pcb *pcb);
int pcb_get_quantum(t_pcb *pcb);
t_nombre_estado pcb_get_estado(t_pcb *pcb);
void pcb_cambiar_estado_a(t_pcb *pcb, t_nombre_estado nuevo_estado);
uint32_t pcb_get_pc(t_pcb *pcb);
char *pcb_get_path(t_pcb *pcb);
t_dictionary *pcb_get_diccionario_recursos_usados(t_pcb *pcb);
bool pcb_usa_recurso(t_pcb *pcb, char *nombre_recurso);

#endif