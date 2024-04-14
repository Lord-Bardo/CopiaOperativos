#include "../include/cpu.h"

int main(int argc, char *argv[])
{
	// INICIALIZAR CPU
	inicializar_cpu();

	// Loggeo el valor de la ip
	log_info(cpu_logger, "%s", IP_MEMORIA);

	// Conexion con el modulo memoria
	int conexion_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
	enviar_mensaje("HOLA", conexion_memoria);
	paquete(conexion_memoria);
	
	terminar_programa(conexion_memoria, cpu_logger, cpu_config);

	//servidor("8006");

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