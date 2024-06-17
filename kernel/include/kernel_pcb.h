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
t_registros *crear_registros(void);
int pcb_get_pid(t_pcb *pcb);
int pcb_get_quantum(t_pcb *pcb);
t_nombre_estado pcb_get_estado(t_pcb *pcb);
uint32_t pcb_get_pc(t_pcb *pcb);
t_registros *pcb_get_registros(t_pcb *pcb);
char *pcb_get_path(t_pcb *pcb);
void pcb_cambiar_estado_a(t_pcb *pcb, t_nombre_estado nuevo_estado);

// FORMA 1:
// char *get_registro(char *registro, int bytes_registro);
// void set_registro(char **registro, char* valor, int bytes);
// FORMA 2:
void set_registro(char **registro, char* valor, int bytes_registro);

// AMBAS:
char *pcb_get_registro_ax(t_pcb *pcb);
char *pcb_get_registro_eax(t_pcb *pcb);
void pcb_set_registro_ax(t_pcb *pcb, char *valor);
void pcb_set_registro_eax(t_pcb *pcb, char *valor);

#endif