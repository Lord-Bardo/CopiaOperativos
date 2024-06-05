#ifndef KERNEL_PCB_H_
#define KERNEL_PCB_H_

#include "kernel_gestor.h"
#include "kernel_utils.h"
#include "kernel_planificadores.h"
#include "kernel_estados.h"

// FUNCIONES
t_pcb *crear_pcb(void);
int generar_pid(void);
void eliminar_pcb(t_pcb *pcb);
t_nombre_estado pcb_get_estado(t_pcb *pcb);
int pcb_get_pid(t_pcb *pcb);
void pcb_cambiar_estado_a(t_pcb *pcb, t_nombre_estado nuevo_estado);

#endif