#ifndef KERNEL_GESTOR_H_
#define KERNEL_GESTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <readline/readline.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

#include <utils/conexion.h>
#include <utils/planificadores.h>

// VARIABLES GLOBALES
extern t_log* kernel_logger;
extern t_config* kernel_config;

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

extern int fd_kernel;
extern int fd_entradasalida;
extern int fd_memoria;
extern int fd_cpu_dispatch;
extern int fd_cpu_interrupt;

#endif