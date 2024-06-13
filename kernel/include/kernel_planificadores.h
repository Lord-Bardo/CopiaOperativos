#ifndef KERNEL_PLANIFICADOR_H_
#define KERNEL_PLANIFICADOR_H_

#include "kernel_gestor.h"
#include "kernel_utils.h"
#include "kernel_pcb.h"
#include "kernel_estados.h"

// VARIABLES GLOBALES
// Planificacion
t_estado_planificacion estado_planificacion;

// Estados
t_estado *estado_new;
t_estado *estado_ready;
t_estado *estado_ready_plus;
t_estado *estado_exec;
t_estado *estado_blocked;
t_estado *estado_exit;

// PID
int pid_actual;
pthread_mutex_t mutex_pid;

// Semaforos
pthread_mutex_t mutex_grado_multiprogramacion;
sem_t sem_grado_multiprogramacion;
sem_t sem_cpu_disponible;
pthread_mutex_t mutex_socket_dispatch;
pthread_mutex_t mutex_socket_memoria;

// FUNCIONES
void iniciar_planificadores(void);
void inicializar_estructuras(void);
void inicializar_estructuras_estados(void);
void inicializar_estructuras_pid(void);
void inicializar_semaforos(void);
void iniciar_planificacion(void);
void detener_planificacion(void);
void cambiar_grado_multiprogramacion_a(int nuevo_grado_multiprogramacion);
void planificador_largo_plazo(void);
void planificador_corto_plazo(void);
void planificador_corto_plazo_fifo(void);
t_pcb *elegir_proceso_segun_fifo(void);
void proceso_a_exec(t_pcb *pcb);
void enviar_contexto_de_ejecucion(void);
void recibir_contexto_de_ejecucion_actualizado(void);
void iniciar_proceso(char *path);
void proceso_a_ready(t_pcb *pcb);
void pedir_a_memoria_iniciar_proceso(int pid, char *path);
void finalizar_proceso(int pid);

#endif