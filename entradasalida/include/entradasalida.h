#ifndef ENTRADASALIDA_H_
#define ENTRADASALIDA_H_

#include "entradasalida_gestor.h"
#include "inicializar_entradasalida.h"

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

//FUNCIONES
void leer_consola(t_log*);
void paquete(int);
void terminar_programa(int, t_log*, t_config*);

#endif