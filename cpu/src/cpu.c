#include "../include/cpu.h"

int main(int argc, char *argv[]){
	// Inicializar estructuras de CPU (loggers y config)
	inicializar_cpu();

	// Iniciar servidor dispatch de CPU
	fd_cpu_dispatch = iniciar_servidor(PUERTO_ESCUCHA_DISPATCH);
	log_info(cpu_logger, "Servidor CPU DISPATCH iniciado!");

	// Esperar conexion del Kernel
	fd_kernel_dispatch = esperar_cliente(fd_cpu_dispatch);
	log_info(cpu_logger, "Se conecto el cliente KERNEL al servidor CPU DISPATCH!");

	// Iniciar servidor interrupt de CPU
	fd_cpu_interrupt = iniciar_servidor(PUERTO_ESCUCHA_INTERRUPT);
	log_info(cpu_logger, "Servidor CPU INTERRUPT iniciado!");

	// Esperar conexion del Kernel
	fd_kernel_interrupt = esperar_cliente(fd_cpu_interrupt);
	log_info(cpu_logger, "Se conecto el cliente KERNEL al servidor CPU INTERRUPT!");

	// Conexion con memoria
	fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
	log_info(cpu_logger, "Conexion con MEMORIA establecida!");

	// Finalizar CPU (liberar memoria usada por estructuras de CPU)
	terminar_programa(fd_memoria, cpu_logger, cpu_config); // en vez de pasarselas por parametro deberia liberar directo todo

	return 0;
}

void leer_consola(t_log *logger)
{
	char *leido;

	// Leo la primer linea
	leido = readline("> ");

	// El resto, las voy leyendo y logueando hasta recibir un string vacío
	while (leido[0] != '\0')
	{
		log_info(logger, "%s", leido);
		leido = readline("> ");
	}

	// Libero las lineas
	free(leido);
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