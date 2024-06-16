#include "../include/inicializar_entradasalida.h"

void inicializar_entradasalida(){
    iniciar_logger();
    iniciar_config();
}

void iniciar_logger(){
    // Creo el entradasalida_logger
    entradasalida_logger = log_create("entradasalida.log", "ENTRADASALIDA", 1, LOG_LEVEL_INFO);
	if(entradasalida_logger == NULL){
		perror("No se pudo crear el logger.");
		exit(1);
	}

    // Creo el entradasalida_logger_min_y_obl
    entradasalida_logger_min_y_obl = log_create("entradasalida_min_y_obl.log", "ENTRADASALIDA", 1, LOG_LEVEL_INFO);
	if(entradasalida_logger_min_y_obl == NULL){
		perror("No se pudo crear el logger.");
		exit(1);
	}
}

void iniciar_config(){
    // Creo el entradasalida_config
    entradasalida_config = config_create("/home/utnso/tp-2024-1c-GSN/entradasalida/entradasalida.config");
	if(entradasalida_config == NULL){
		perror("No se pudo crear el config.");
		exit(2);
	}

    // Obtengo valores del archivo entradasalida.config
    TIPO_INTERFAZ = config_get_string_value(entradasalida_config, "TIPO_INTERFAZ");
	TIEMPO_UNIDAD_TRABAJO = config_get_int_value(entradasalida_config, "TIEMPO_UNIDAD_TRABAJO");
	IP_KERNEL = config_get_string_value(entradasalida_config, "IP_KERNEL");
    PUERTO_KERNEL = config_get_string_value(entradasalida_config, "PUERTO_KERNEL");
    IP_MEMORIA = config_get_string_value(entradasalida_config, "IP_MEMORIA");
    PUERTO_MEMORIA = config_get_string_value (entradasalida_config, "PUERTO_MEMORIA");
    PATH_BASE_DIALFS = config_get_string_value (entradasalida_config, "PATH_BASE_DIALFS");
    BLOCK_SIZE = config_get_int_value(entradasalida_config, "BLOCK_SIZE");
    BLOCK_COUNT = config_get_int_value(entradasalida_config, "BLOCK_COUNT");
    RETRASO_COMPACTACION = config_get_int_value(entradasalida_config, "RETRASO_COMPACTACION");
}

// Logs obligatorios
/* Todos - Operación: "PID: <PID> - Operacion: <OPERACION_A_REALIZAR>"
DialFS - Crear Archivo: "PID: <PID> - Crear Archivo: <NOMBRE_ARCHIVO>"
DialFS - Eliminar Archivo: "PID: <PID> - Eliminar Archivo: <NOMBRE_ARCHIVO>"
DialFS - Truncar Archivo: "PID: <PID> - Truncar Archivo: <NOMBRE_ARCHIVO> - Tamaño: <TAMAÑO>"
DialFS - Leer Archivo: "PID: <PID> - Leer Archivo: <NOMBRE_ARCHIVO> - Tamaño a Leer: <TAMAÑO> - Puntero Archivo: <PUNTERO_ARCHIVO>"
DialFS - Escribir Archivo: "PID: <PID> - Escribir Archivo: <NOMBRE_ARCHIVO> - Tamaño a Escribir: <TAMAÑO> - Puntero Archivo: <PUNTERO_ARCHIVO>"
DialFS - Inicio Compactación: "PID: <PID> - Inicio Compactación."
DialFS - Fin Compactación: "PID: <PID> - Fin Compactación."
 */