#include "../include/inicializar_interfaces.h"

void inicializar_interfaz(char* nombre_interfaz, char* archivo_configuracion){
    iniciar_logger(nombre_interfaz);
    iniciar_config(nombre_interfaz, archivo_configuracion);
}

/* El de soporte me dijo que conviene tener un solo logger pero algo me dijo de los logs distintos - ver grabacion */
/* Capaz también me conviene poner un parametro como en el config */
void iniciar_logger(char* nombre_interfaz){

   /*  if (strcmp(nombre_interfaz, "generica") == 0) {
        generica_logger = log_create("generica.log", "INTERFAZ GENERICA", 1, LOG_LEVEL_INFO);
        // Ver de validar en cada uno que si no se pudo crear el log que tire un error
    } else if (strcmp(nombre_interfaz, "stdin") == 0) {
        stdin_logger = log_create("stdin.log", "INTERFAZ STDIN", 1, LOG_LEVEL_INFO);
    } else if (strcmp(nombre_interfaz, "stdout") == 0) {
        stdout_logger = log_create("stdout.log", "INTERFAZ STDOUT", 1, LOG_LEVEL_INFO);
    } else if (strcmp(nombre_interfaz, "dialfs") == 0) {
        dialfs_logger = log_create("dialfs.log", "INTERFAZ DIALFS", 1, LOG_LEVEL_INFO);
    } else {
        perror("Tipo de interfaz desconocida: %s\n", nombre_interfaz);
        exit(1);
    } */

    entradasalida_logger = log_create("entradasalida_logger.log", "ENTRADASALIDA", 1, LOG_LEVEL_INFO);
	if(entradasalida_logger == NULL){
		perror("No se pudo crear el logger.");
		exit(1);
	}

    entradasalida_logger_min_y_obl = log_create("entradasalida_min_y_obl.log", "ENTRADASALIDA", 1, LOG_LEVEL_INFO);
	if(entradasalida_logger_min_y_obl == NULL){
		perror("No se pudo crear el logger.");
		exit(1);
	}
}

void iniciar_config(char* nombre_interfaz, char* archivo_configuracion){
    char config_path[256]; // Le pongo ese tamaño para asegurarme que es lo suficientemente grande como para guardar la ruta del archivo
    sprintf(config_path, "/home/utnso/tp-2024-1c-GSN/entradasalida/%s", archivo_configuracion);
    entradasalida_config = config_create(config_path);
	if(entradasalida_config == NULL){
		perror("No se pudo crear el config.");
        perror(config_path);
		exit(2); // El exit(2) se utiliza para indicar que el programa terminó debido a un error relacionado con la creación del archivo de configuración (ver de ponerlo en los otros módulos también)
	}

    TIPO_INTERFAZ = config_get_string_value(entradasalida_config, "TIPO_INTERFAZ");
	TIEMPO_UNIDAD_TRABAJO = config_get_int_value(entradasalida_config, "TIEMPO_UNIDAD_TRABAJO");
	IP_KERNEL = config_get_string_value(entradasalida_config, "IP_KERNEL");
    PUERTO_KERNEL = config_get_string_value(entradasalida_config, "PUERTO_KERNEL");
    IP_MEMORIA = config_get_string_value(entradasalida_config, "IP_MEMORIA");
    PUERTO_MEMORIA = config_get_string_value(entradasalida_config, "PUERTO_MEMORIA");
    PATH_BASE_DIALFS = config_get_string_value(entradasalida_config, "PATH_BASE_DIALFS");
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