#include "kernel.h"

int main(int argc, char* argv[]) {
    // char *puerto_escucha;
    // char *ip_memoria;
    // char *puerto_memoria;
    // char *ip_cpu;
    // char *puerto_cpu_dispatch;
    // char *puerto_cpu_interrupt;
    // char *algoritmo_planificacion;
    // char *quantum;
    // char *recursos;
    // char *instancias_recursos;
    // char *grado_multiprogramacion;

    // t_config *config;
	// t_log *logger;

	// //  ARCHIVOS DE CONFIGURACION

    // // Inicio el log
	// logger = iniciar_logger();

	// // Inicio el config
	// config = iniciar_config();

    // puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");
    // ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    // puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    // ip_cpu = config_get_string_value(config, "IP_CPU");
    // puerto_cpu_dispatch = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    // puerto_cpu_interrupt = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
    // algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    // quantum = config_get_string_value(config, "QUANTUM");
    // recursos = config_get_string_value(config, "RECURSOS");
    // instancias_recursos = config_get_string_value(config, "INSTANCIAS_RECURSOS");
    // grado_multiprogramacion = config_get_string_value(config, "GRADO_MULTIPROGRAMACION");

    // log_info(logger, "%s", ip_memoria);


	// // Conexion con el modulo memoria
	// int conexion_memoria;
    // conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
	// enviar_mensaje("HOLA", conexion_memoria);
	// paquete(conexion_memoria);

    // terminar_programa(conexion_memoria, logger , config);

	//conexion con entrada salida
	servidor("8003");

	return 0; 
}



t_config *iniciar_config(void)
{
	t_config *nuevo_config;
	nuevo_config = config_create("/home/utnso/tp-2024-1c-GSN/kernel/kernel.config");
	if (nuevo_config == NULL)
	{
		printf("No se pudo crear el config.");
		exit(2);
	}

	return nuevo_config;
}

t_log *iniciar_logger(void)
{
	t_log *nuevo_logger;
	nuevo_logger = log_create("kernel.log", "KERNEL", 1, LOG_LEVEL_INFO);
	if (nuevo_logger == NULL)
	{
		printf("No se pudo crear el logger.");
		exit(1);
	}

	return nuevo_logger;
}


void paquete(int conexion)
{
	char *leido;
	t_paquete *paquete;

	// Creo el paquete
	paquete = crear_paquete();

	// Leo y agrego las lineas al paquete
	leido = readline("> ");
	while (leido[0] != '\0')
	{
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
		leido = readline("> ");
	}

	// Envio el paquete
	enviar_paquete(paquete, conexion);

	// Libero las lineas y el paquete
	free(leido);
	eliminar_paquete(paquete);
}



void terminar_programa(int conexion, t_log *logger, t_config *config)
{
	if (logger != NULL)
	{
		log_destroy(logger);
	}

	if (config != NULL)
	{
		config_destroy(config);
	}

	liberar_conexion(conexion);
}

