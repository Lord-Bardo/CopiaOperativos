#ifndef ENTRADASALIDA_GESTOR_H_
#define ENTRADASALIDA_GESTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

#include <utils/conexion.h>

// VARIABLES GLOBALES
extern t_log* entradasalida_logger;
extern t_config* entradasalida_config;

extern char* TIPO_INTERFAZ;
extern int TIEMPO_UNIDAD_TRABAJO;
extern char* IP_KERNEL;
extern char* PUERTO_ESCUCHA_INTERRUPT;
extern char* PUERTO_KERNEL;
extern char* IP_MEMORIA;
extern char* PUERTO_MEMORIA;
extern char* PATH_BASE_DIALFS;
extern int BLOCK_SIZE;
extern int BLOCK_COUNT;

extern int fd_kernel;
extern int fd_memoria;

#endif