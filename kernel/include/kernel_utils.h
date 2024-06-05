#ifndef KERNEL_UTILS_H_
#define KERNEL_UTILS_H_

#include "kernel_gestor.h"
#include "kernel_pcb.h"
#include "kernel_estados.h"

// FUNCIONES
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

#endif