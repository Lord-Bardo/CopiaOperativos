#ifndef KERNEL_ESTADOS_H_
#define KERNEL_ESTADOS_H_

#include "kernel_gestor.h"
#include "kernel_pcb.h"

// ESTRUCTURAS
typedef enum{
    NEW,
    READY,
    READY_PLUS,
    RUNNING,
    BLOCKED,
    EXIT
} t_nombre_estado;

typedef struct{
    t_nombre_estado nombre_estado;
    t_list *lista_procesos;
    pthread_mutex_t *mutex_estado;
    sem_t *sem_estado;
} t_estado;

#endif