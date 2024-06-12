#ifndef KERNEL_UTILS_H_
#define KERNEL_UTILS_H_

#include "kernel_gestor.h"
#include "kernel_pcb.h"
#include "kernel_estados.h"

// FUNCIONES
// Logs obligatorios
void log_creacion_proceso(t_pcb *pcb);
void log_fin_proceso(void);
void log_cambio_estado(t_pcb *pcb, t_nombre_estado estado_anterior, t_nombre_estado nuevo_estado);
char *estado_get_nombre_estado_string(t_nombre_estado nombre_estado);
void log_motivo_bloqueo(void);
void log_fin_quantum(void);
void log_ingreso_ready(void);
char *lista_pids_string(t_estado *estado);
void *pcb_get_pid_transformer(void *pcb);
void pid_destroyer(void *pid);

// Manejo paquete
void agregar_pid_a_paquete(t_paquete *paquete, int pid);
void agregar_quantum_a_paquete(t_paquete *paquete, int quantum);
void agregar_estado_a_paquete(t_paquete *paquete, t_nombre_estado estado);
void agregar_uint32_a_paquete(t_paquete *paquete, uint32_t n);
void agregar_uint8_a_paquete(t_paquete *paquete, uint8_t n);
void agregar_registros_a_paquete(t_paquete *paquete, t_registros registros);
void agregar_pcb_a_paquete(t_paquete *paquete, t_pcb* pcb);
void agregar_string_a_paquete(t_paquete *paquete, char *string);

#endif