#ifndef MEMORIA_GESTOR_H_
#define MEMORIA_GESTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <readline/readline.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

#include <utils/conexion.h>
#include <utils/planificadores.h>
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

void* espacio_usuario; 
void* puntero_espacio_usuario;
size_t num_lineas = 0; // Número de líneas leídas de un archivo de pseudocodigo.
typedef struct {
    int pid;
    t_tabla_paginas tabla_de_pags; 
    char** memoria_de_instrucciones;
} t_pcb_memoria procesos[TAM_MEMORIA/TAM_PAGINA]; // En este array voy a ir colocando todos mis procesos.
typedef struct {
    int frame;
    void *frame_pointer;
    bool presencia;
} t_pagina;
typedef struct { //para las tablas entrar al dictionary.h para ver la estructura de t_config
    int pid;
    pagina_t *paginas;
} t_tabla_paginas; 
typedef struct {
    int pid;
    char* path_pseudocodigo;
    t_tabla_paginas *tabla_de_pags; //puntero a su tabla de paginas
    char** memoria_de_instrucciones;
} t_pcb_memoria;


#endif