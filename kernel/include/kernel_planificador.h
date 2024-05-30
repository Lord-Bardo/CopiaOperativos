#ifndef KERNEL_PLANIFICADOR_H_
#define KERNEL_PLANIFICADOR_H_

#include "kernel_gestor.h"
#include "kernel_pcb.h"

// ESTRUCTURAS
typedef enum{
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    EXIT,
} estado;

// VARIABLES GLOBALES
// PID
int pid_actual;
pthread_mutex_t mutex_pid;

// Semaforos
sem_t sem_grado_multiprogramacion;

// Colas
t_queue *ready;
t_queue *ready_plus;

// FUNCIONES
void iniciar_proceso(const char *path);
void proceso_a_ready(t_pcb *pcb);

#endif