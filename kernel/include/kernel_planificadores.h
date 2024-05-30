#ifndef KERNEL_PLANIFICADOR_H_
#define KERNEL_PLANIFICADOR_H_

#include "kernel_gestor.h"
#include "kernel_pcb.h"
#include "kernel_estados.h"

// VARIABLES GLOBALES
// Estados
t_estado *estado_new;
t_estado *estado_ready;
t_estado *estado_ready_plus;
t_estado *estado_running;
t_estado *estado_blocked;
t_estado *estado_exit;
// PID
int pid_actual;
pthread_mutex_t mutex_pid;
// Semaforos
sem_t sem_grado_multiprogramacion;

// FUNCIONES
void iniciar_proceso(const char *path);
void proceso_a_ready(t_pcb *pcb);

#endif