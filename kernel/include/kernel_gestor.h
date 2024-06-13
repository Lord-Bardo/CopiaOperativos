#ifndef KERNEL_GESTOR_H_
#define KERNEL_GESTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <readline/readline.h>
#include <semaphore.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>

#include <utils/conexion.h>

// ESTRUCTURAS
// Estado
typedef enum{
    NEW,
    READY,
    READY_PLUS,
    EXEC,
    BLOCKED,
    EXIT
} t_nombre_estado;

typedef struct{
    t_nombre_estado nombre_estado;
    t_list *lista_procesos;
    pthread_mutex_t *mutex_estado;
    sem_t *sem_estado;
} t_estado;

// PCB
typedef struct{
    __uint32_t PC;
    __uint8_t AX;
    __uint8_t BX;
    __uint8_t CX;
    __uint8_t DX;
    __uint32_t EAX;
    __uint32_t EBX;
    __uint32_t ECX;
    __uint32_t EDX;
    __uint32_t SI; //contiene la direccion logica de memoria de origen desde donde se va a copir un string
    __uint32_t DI; //contiene la direccion logica de memoria de destino desde donde se va a copir un string
} t_registros;

typedef struct{
    int PID;
    int quantum;
    t_nombre_estado estado;
    t_registros registros;
} t_pcb;

// Planificacion
typedef enum{
    PAUSADA = 0,
    ACTIVA = 1
} t_estado_planificacion;

// VARIABLES GLOBALES
// Loggers y Config
extern t_log* kernel_logger;
extern t_log* kernel_logger_min_y_obl;
extern t_config* kernel_config;

// Variables del config
extern char* PUERTO_ESCUCHA;
extern char* IP_MEMORIA;
extern char* PUERTO_MEMORIA;
extern char* IP_CPU;
extern char* PUERTO_CPU_DISPATCH;
extern char* PUERTO_CPU_INTERRUPT;
extern char* ALGORITMO_PLANIFICACION;
extern int QUANTUM;
extern char** RECURSOS;
extern char** INSTANCIAS_RECURSOS;
extern int GRADO_MULTIPROGRAMACION;

// File descriptors (sockets)
extern int fd_kernel;
extern int fd_entradasalida;
extern int fd_memoria;
extern int fd_cpu_dispatch;
extern int fd_cpu_interrupt;

// Planificacion
extern t_estado_planificacion estado_planificacion;

// Estados
extern t_estado *estado_new;
extern t_estado *estado_ready;
extern t_estado *estado_ready_plus;
extern t_estado *estado_exec;
extern t_estado *estado_blocked;
extern t_estado *estado_exit;

// PID
extern int pid_actual;
extern pthread_mutex_t mutex_pid;

// Semaforos
extern pthread_mutex_t mutex_grado_multiprogramacion;
extern sem_t sem_grado_multiprogramacion;
extern sem_t sem_cpu_disponible;
extern pthread_mutex_t mutex_socket_dispatch;
extern pthread_mutex_t mutex_socket_memoria;

#endif