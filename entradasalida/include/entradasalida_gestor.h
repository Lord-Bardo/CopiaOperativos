#ifndef ENTRADASALIDA_GESTOR_H_
#define ENTRADASALIDA_GESTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <readline/readline.h>
#include <errno.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <dirent.h>
#include <math.h>
#include <sys/stat.h> // Para saber el estado de un archivo
#include <sys/mman.h> // Para mmap
#include <fcntl.h> // Para el open del bitmap
#include <commons/bitarray.h>

#include <utils/conexion.h>

// VARIABLES GLOBALES
extern t_log* entradasalida_logger;
extern t_log* entradasalida_logger_min_y_obl;
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
extern int RETRASO_COMPACTACION;

extern int fd_kernel;
extern int fd_memoria;
extern int fd_entradasalida;

extern t_bitarray* bitmap; 
extern void* bitmap_data;

extern t_dictionary* metadata_dictionary_files;

#endif