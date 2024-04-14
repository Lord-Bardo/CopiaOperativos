#include "../include/inicializar_cpu.h"

void inicializar_entradasalida(){
    iniciar_logger();
    iniciar_config();
}

void iniciar_logger(){
    // Creo el entradasalida_logger
    entradasalida_logger = log_create("entradasalida.log", "entradasalida", 1, LOG_LEVEL_INFO);
	if (entradasalida_logger == NULL){
		perror("No se pudo crear el logger.");
		exit(1);
	}

    // Se podrian iniciar todos los loggers que vayamos a usar en entradasalida aca
    // Por ejemplo, se podria crear un logger exclusivo para los logs obligatorios (podria ser el entradasalida_logger directamente), otro para debuggear (de LOG_LEVEL_TRACE), etc.
    // ...
}

void iniciar_config(){
    // Creo el entradasalida_config
    entradasalida_config = config_create("/home/utnso/tp-2024-1c-GSN/entradasalida/entradasalida.config");
	if (enctrada_config == NULL){
		perror("No se pudo crear el config.");
		exit(2);
	}

   // Obtengo valores del archivo entradasalida.config
    TIPO_INTERFAZ = config_get_string_value(config, "TIPO_INTERFAZ");
	TIEMPO_UNIDAD_TRABAJO = config_get_string_value(config, "TIEMPO_UNIDAD_TRABAJO");
	IP_KERNEL = config_get_string_value(config, "IP_KERNEL");
    PUERTO_KERNEL = config_get_string_value(config, "PUERTO_KERNEL");
    IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
    PUERTO_MEMORIA = config_get_string_value (config, "PUERTO_MEMORIA");
    PATH_BASE_DIALFS = config_get_string_value (config, "PATH_BASE_DIALFS");
    BLOCK_SIZE = config_get_string_value (config, "BLOCK_SIZE");
    BLOCK_COUNT = config_get_string_value (config, "BLOCK_COUNT");
}