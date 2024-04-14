#ifndef ENTRADASALIDA_H_
#define ENTRADASALIDA_H_

#include "entradasalida_gestor.h"
#include "inicializar_entradasalida.h"


// VARIABLES GLOBALES
t_log *entradasalida_logger;
t_config *entradasalida_config;

char* tipo_interfaz;
int tiempo_unidad_trabajo;
char* ip_kernel;
char* puerto_kernel;
char* ip_memoria;
char* puerto_memoria;
char* path_base_dialfs;
int block_size;
int block_count;

//FUNCIONES
void leer_consola(t_log*);
void paquete(int);
void terminar_programa(int, t_log*, t_config*);

#endif

