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

// ESTRUCTURAS
typedef struct {
    int num_frame;
    bool bit_presencia;
} t_pagina; 
typedef struct {
    int pid;
    char* path;
    t_pagina* tabla_paginas; 
    char** memoria_de_instrucciones; 
    int cant_paginas; 
} t_pcb_memoria; //FALTARÍA AGREGAR CANTIDAD DE PÁGINAS y offset

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

extern void* espacio_usuario; 
extern void* puntero_espacio_usuario;
extern t_pcb_memoria* procesos; // En esta lista voy a ir colocando todos mis procesos.
extern size_t num_instruccion; // Número de instrucciones leídas de un archivo de pseudocodigo.

#endif