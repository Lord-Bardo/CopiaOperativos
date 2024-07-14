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
#include <commons/collections/dictionary.h>

#include <utils/conexion.h>
#include <utils/registros.h>

// ESTRUCTURAS
// Planificacion
typedef enum{
    PAUSADA = 0,
    ACTIVA = 1
} t_estado_planificacion;

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
    int pid;
    int quantum;
    t_nombre_estado estado;
    uint32_t PC;
    t_registros *registros;
    char *path;
    t_dictionary *diccionario_recursos_usados;
} t_pcb;

// Recurso
typedef struct{
    int instancias;
    t_list *lista_procesos_bloqueados;
    pthread_mutex_t *mutex_lista_procesos_bloqueados;
    sem_t *sem_lista_procesos_bloqueados;
    pthread_mutex_t *mutex_recurso;
} t_recurso;

// Interfaz e/s
typedef struct{
    t_tipo_interfaz tipo_interfaz;
    int fd_interfaz;
    t_list *lista_solicitudes_io;
    pthread_mutex_t *mutex_lista_solicitudes_io;
    sem_t *sem_lista_solicitudes_io;
    sem_t *sem_control_uso_interfaz;
} t_interfaz;

typedef struct{
	t_pcb *pcb;
	t_paquete *paquete_solicitud_io;
} t_solicitud_io;

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
extern int fd_memoria;
extern int fd_cpu_dispatch;
extern int fd_cpu_interrupt;

// Planificacion
extern t_estado_planificacion estado_planificacion;
extern sem_t sem_estado_planificacion_new_to_ready;
extern sem_t sem_estado_planificacion_ready_to_exec;
extern sem_t sem_estado_planificacion_blocked_to_ready;
extern sem_t sem_estado_planificacion_exec_to_exec_or_ready_or_blocked;

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

// Grado multiprogramacion
extern pthread_mutex_t mutex_grado_multiprogramacion;
extern sem_t sem_grado_multiprogramacion;

// Recursos
extern t_dictionary *diccionario_recursos;

// Interfaces
extern t_dictionary *diccionario_interfaces;
extern pthread_mutex_t mutex_diccionario_interfaces;

// Semaforos sockets
extern pthread_mutex_t mutex_socket_memoria;

#endif