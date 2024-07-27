#ifndef ENTRADASALIDA_H_
#define ENTRADASALIDA_H_

#include "./entradasalida_gestor.h"
#include "./inicializar_interfaces.h"
#include "./entradasalida_kernel.h"

// VARIABLES GLOBALES
t_log *entradasalida_logger;
t_log *entradasalida_logger_min_y_obl;
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
int RETRASO_COMPACTACION;

int fd_kernel;
int fd_memoria;
int fd_entradasalida;

// Puntero a una estructura de tipo t_bitarray que se usará para gestionar el bitmap
t_bitarray* bitmap; 
// // Puntero que va a apuntar al área de memoria donde el archivo del bitmap va a ser mapeado
void* bitmap_data;
// Lista de archivos metadata
t_dictionary* metadata_dictionary_files;

//FUNCIONES
void conectar_a_kernel(char* nombre_interfaz);
void conectar_a_memoria();
void terminar_programa();

#endif