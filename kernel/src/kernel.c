#include "../include/kernel.h"

int main(int argc, char* argv[]) {
	// Inicializar estructuras de KERNEL (loggers y config)
	inicializar_kernel();

	// Iniciar servidor de KERNEL
	fd_kernel = iniciar_servidor(PUERTO_ESCUCHA);
	log_info(kernel_logger, "Servidor KERNEL iniciado!");

	// Esperar conexion de ENTRADASALIDA
	fd_entradasalida = esperar_cliente(fd_kernel);
	log_info(kernel_logger, "Se conecto el cliente ENTRADASALIDA al servidor KERNEL!");

	// Conexion con MEMORIA
	fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
	log_info(kernel_logger, "Conexion con MEMORIA establecida!");

	// Conexion con CPU - DISPATCH
	fd_cpu_dispatch = crear_conexion(IP_CPU, PUERTO_CPU_DISPATCH);
	log_info(kernel_logger, "Conexion con CPU establecida!");

	// Conexion con CPU - INTERRUPT
	fd_cpu_interrupt = crear_conexion(IP_CPU, PUERTO_CPU_INTERRUPT);
	log_info(kernel_logger, "Conexion con CPU establecida!");

	// Atender los mensajes de ENTRADASALIDA 
	atender_kernel_entradasalida();

	// Atender los mensajes de MEMORIA
	atender_kernel_memoria();

    // Atender los mensajes de CPU - DISPATCH
	atender_kernel_cpu_dispatch();

	// Atender los mensajes de CPU - INTERRUPR
	atender_kernel_cpu_interrupt();

	// Finalizar KERNEL (liberar memoria usada)
	terminar_programa();

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



void terminar_programa()
{
	if (kernel_logger != NULL)
	{
		log_destroy(kernel_logger);
	}

	if (kernel_config != NULL)
	{
		config_destroy(kernel_config);
	}

	liberar_conexion(fd_cpu_dispatch);
	liberar_conexion(fd_cpu_interrupt);
	liberar_conexion(fd_memoria);
}

