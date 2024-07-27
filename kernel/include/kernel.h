#ifndef KERNEL_H_
#define KERNEL_H_

#include "kernel_gestor.h"
#include "inicializar_kernel.h"
#include "kernel_entradasalida.h"
#include "kernel_planificadores.h"
#include "consola.h"

// VARIABLES GLOBALES
t_log* kernel_logger;
t_log* kernel_logger_min_y_obl;
t_config* kernel_config;

char* PUERTO_ESCUCHA;
char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* IP_CPU;
char* PUERTO_CPU_DISPATCH;
char* PUERTO_CPU_INTERRUPT;
char* ALGORITMO_PLANIFICACION;
int QUANTUM;
char** RECURSOS;
char** INSTANCIAS_RECURSOS;
int GRADO_MULTIPROGRAMACION;

int fd_kernel;
int fd_memoria;
int fd_cpu_dispatch;
int fd_cpu_interrupt;

// FUNCIONES
void conectar_a_memoria(void);
void conectar_a_cpu_dispatch(void);
void conectar_a_cpu_interrupt(void);
void aceptar_conexiones_entradasalida(void);
void terminar_programa(void);

#endif