#ifndef ENTRADASALIDA_H_
#define ENTRADASALIDA_H_

#include "entradasalida_gestor.h"
#include "inicializar_entradasalida.h"
#include "entradasalida_memoria.h"
#include "entradasalida_kernel.h"

// VARIABLES GLOBALES
t_log *entradasalida_logger;
t_config *entradasalida_config;

char* TIPO_INTERFAZ;
int TIEMPO_UNIDAD_TRABAJO;
char* IP_KERNEL;
char* PUERTO_KERNEL;
char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* PATH_BASE_DIALFS;
int BLOCK_SIZE;
int BLOCK_COUNT;

int fd_kernel;
int fd_memoria;

//FUNCIONES
void conectar_a_kernel();
void conectar_a_memoria();
void terminar_programa();

#endif