#ifndef KERNEL_H_
#define KERNEL_H_

#include "kernel_gestor.h"
#include "incializar_kernel.h"

// VARIABLES GLOBALES
t_log* kernel_logger;
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

// FUNCIONES
void leer_consola(t_log*);
void paquete(int);
void terminar_programa(int, t_log*, t_config*);

#endif
