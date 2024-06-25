#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "inicializar_memoria.h"
#include "memoria_entradasalida.h"
#include "memoria_kernel.h"
#include "memoria_cpu.h"

//VARIABLES GLOBALES
extern t_log* memoria_logger;
extern t_config* memoria_config;

extern char *PUERTO_ESCUCHA;
extern int TAM_MEMORIA;
extern int TAM_PAGINA;
extern char *PATH_INSTRUCCIONES;
extern int RETARDO_REPUESTA;

extern int fd_cpu;
extern int fd_kernel;
extern int fd_entradasalida;
extern int fd_memoria;

//FUNCIONES
void leer_consola(t_log*);
void paquete(int);
void terminar_programa();

#endif