#ifndef MEMORIA_GESTOR_H_
#define MEMORIA_GESTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

#include <utils/conexion.h>

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

#endif