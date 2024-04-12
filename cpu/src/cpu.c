#include "cpu.h"

int main(int argc, char *argv[])
{
	int conexion_memoria;
	t_log *logger;
	t_config *config;
	char *ip_memoria;
	char *puerto_memoria;
	char *puerto_escucha_dispatch;
	char *puerto_escucha_interrupt;
	char *cantidad_entradas_tlb;
	char *algoritmo_tlb;

	// Inicio el log
	logger = iniciar_logger();

	// Inicio el config
	config = iniciar_config();

	// Obtengo valores del archivo cpu.config
	ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	puerto_escucha_dispatch = config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH");
	puerto_escucha_interrupt = config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT");
	cantidad_entradas_tlb = config_get_string_value(config, "CANTIDAD_ENTRADAS_TLB");
	algoritmo_tlb = config_get_string_value(config, "ALGORITMO_TLB");

	// Loggeo el valor de la ip
	log_info(logger, "%s", ip_memoria);

	// Conexion con el modulo memoria
	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
	enviar_mensaje("HOLA", conexion_memoria);
	paquete(conexion_memoria);

	servidor();
	
	terminar_programa(conexion_memoria, logger, config);

	return 0;
}

t_log *iniciar_logger(void)
{
	t_log *nuevo_logger;
	nuevo_logger = log_create("cpu.log", "CPU", 1, LOG_LEVEL_INFO);
	if (nuevo_logger == NULL)
	{
		printf("No se pudo crear el logger.");
		exit(1);
	}

	return nuevo_logger;
}

t_config *iniciar_config(void)
{
	t_config *nuevo_config;
	nuevo_config = config_create("../cpu.config");
	if (nuevo_config == NULL)
	{
		printf("No se pudo crear el config.");
		exit(2);
	}

	return nuevo_config;
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