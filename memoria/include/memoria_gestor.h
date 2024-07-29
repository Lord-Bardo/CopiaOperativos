#ifndef MEMORIA_GESTOR_H_
#define MEMORIA_GESTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <readline/readline.h>
#include <math.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>

#include <utils/conexion.h>
#include <utils/planificadores.h>
#include <utils/instrucciones.h>



// ESTRUCTURAS
typedef struct {
    int num_frame;
} t_pagina; 
typedef struct {
    int pid;
    char* path;
    t_list* tabla_paginas; 
    t_list* memoria_de_instrucciones; 
} t_pcb_memoria;

//pregunta: que onda con el buffer del proceso? La struct del buffer está en conexion.h pero como los conecto?

//VARIABLES GLOBALES
extern t_log* memoria_logger;
extern t_log* memoria_logger_min_y_obl;
extern t_config* memoria_config;

extern char *PUERTO_ESCUCHA;
extern int TAM_MEMORIA;
extern int TAM_PAGINA;
extern char *PATH_INSTRUCCIONES;
extern int RETARDO_RESPUESTA;

extern int fd_cpu;
extern int fd_kernel;
extern int fd_memoria;

extern void* espacio_usuario; 
extern t_bitarray* frames_libres;
extern t_list* procesos; // En esta lista voy a ir colocando todos mis procesos.
extern size_t num_instruccion; // Número de instrucciones leídas de un archivo de pseudocodigo.
extern int pid_es; 
extern int pid_kernel;
extern int pid_cpu;  

extern pthread_mutex_t mutex_espacio_usuario;
extern pthread_mutex_t mutex_procesos;
extern pthread_mutex_t mutex_bitmap;

#endif