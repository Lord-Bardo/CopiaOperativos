#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "memoria_gestor.h"
#include "inicializar_memoria.h"
#include "memoria_entradasalida.h"
#include "memoria_kernel.h"
#include "memoria_cpu.h"

//VARIABLES GLOBALES
t_log* memoria_logger;
t_config* memoria_config;

char *PUERTO_ESCUCHA;
int TAM_MEMORIA;
int TAM_PAGINA;
char *PATH_INSTRUCCIONES;
int RETARDO_REPUESTA;

int fd_cpu;
int fd_kernel;
int fd_entradasalida;
int fd_memoria;

//FUNCIONES
void leer_consola(t_log*);
void paquete(int);
void terminar_programa();

#endif