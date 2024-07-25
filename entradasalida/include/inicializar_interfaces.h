#ifndef INICIALIZAR_ENTRADASALIDA_H_
#define INICIALIZAR_ENTRADASALIDA_H_

#include "./entradasalida_gestor.h"

void inicializar_interfaz(char* nombre_interfaz, char* archivo_configuracion);
void inicializar_fs();
void iniciar_logger(char* nombre_interfaz);
void iniciar_config(char* nombre_interfaz, char* archivo_configuracion);
void crear_directorio_fs();
void crear_archivo_bloques();
void crear_bitmap();

#endif