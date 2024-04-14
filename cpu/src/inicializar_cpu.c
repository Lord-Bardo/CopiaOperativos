#include "../include/inicializar_cpu.h"

void inicializar_cpu(){
    iniciar_logger();
    iniciar_config();
}

void iniciar_logger(){
    // Creo el cpu_logger
    cpu_logger = log_create("cpu.log", "CPU", 1, LOG_LEVEL_INFO);
	if (cpu_logger == NULL){
		perror("No se pudo crear el logger.");
		exit(1);
	}

    // Se podrian iniciar todos los loggers que vayamos a usar en cpu aca
    // Por ejemplo, se podria crear un logger exclusivo para los logs obligatorios (podria ser el cpu_logger directamente), otro para debuggear (de LOG_LEVEL_TRACE), etc.
    // ...
}

void iniciar_config(){
    // Creo el cpu_config
    cpu_config = config_create("/home/utnso/tp-2024-1c-GSN/cpu/cpu.config");
	if (cpu_config == NULL){
		perror("No se pudo crear el config.");
		exit(2);
	}

    // Obtengo valores del archivo cpu.config
	IP_MEMORIA = config_get_string_value(cpu_config, "IP_MEMORIA");
	PUERTO_MEMORIA = config_get_string_value(cpu_config, "PUERTO_MEMORIA");
	PUERTO_ESCUCHA_DISPATCH = config_get_string_value(cpu_config, "PUERTO_ESCUCHA_DISPATCH");
	PUERTO_ESCUCHA_INTERRUPT = config_get_string_value(cpu_config, "PUERTO_ESCUCHA_INTERRUPT");
	CANTIDAD_ENTRADAS_TLB = config_get_int_value(cpu_config, "CANTIDAD_ENTRADAS_TLB");
	ALGORITMO_TLB = config_get_string_value(cpu_config, "ALGORITMO_TLB");
}