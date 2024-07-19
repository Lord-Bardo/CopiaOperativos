#ifndef KERNEL_UTILS_H_
#define KERNEL_UTILS_H_

#include "kernel_gestor.h"
#include "kernel_pcb.h"
#include "kernel_estados.h"

// FUNCIONES
// Logs obligatorios
void log_creacion_proceso(t_pcb *pcb);
void log_fin_proceso(t_pcb *pcb, char *motivo_finalizacion);
void log_cambio_estado(t_pcb *pcb, t_nombre_estado estado_anterior, t_nombre_estado nuevo_estado);
char *estado_get_nombre_estado_string(t_nombre_estado nombre_estado);
void log_ingreso_ready(t_estado *estado);
void log_motivo_bloqueo(t_pcb *pcb, char *motivo_bloqueo);
void log_fin_quantum(t_pcb *pcb);
void log_ingreso_ready(t_estado *estado);
char *lista_pids_string(t_estado *estado);
void *pcb_get_pid_transformer(void *pcb);
void pid_destroyer(void *pid);

// Manejo paquete
void agregar_pid_a_paquete(t_paquete *paquete, int pid);
void agregar_estado_a_paquete(t_paquete *paquete, t_nombre_estado estado);
void agregar_pc_a_paquete(t_paquete *paquete, uint32_t pc);
void agregar_registros_a_paquete(t_paquete *paquete, t_registros registros);
void agregar_contexto_ejecucion_a_paquete(t_paquete *paquete, t_pcb* pcb);

// Manejo buffer
void buffer_desempaquetar_registros(t_buffer *buffer, t_registros *registros);
void buffer_desempaquetar_contexto_ejecucion(t_buffer *buffer, t_pcb* pcb);

#endif