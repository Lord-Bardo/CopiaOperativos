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
    int pid;
    char* path;
    t_pagina tabla_paginas[TAM_MEMORIA/TAM_PAGINA]; 
    char* memoria_de_instrucciones[TAM_MEMORIA]; //en realidad la cant. máx. de instrucciones que un proceso puede tener debe calcularse como el cociente entre el tamaño de memoria y el tamaño máximo de lo que se guardaré en ella (que no será el tamaño del string leido por cada archivo) por el momento uso char considerando 1byte como tamaño minimo.
} t_pcb_memoria;
typedef struct {
    int num_frame;
    void *frame_pointer; //recorre todo un frame.
    bool presencia;
} t_pagina; 

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
extern t_pcb_memoria procesos[TAM_MEMORIA/TAM_PAGINA]; // En este array voy a ir colocando todos mis procesos.
extern size_t num_instruccion = 0; // Número de instrucciones leídas de un archivo de pseudocodigo.

#endif