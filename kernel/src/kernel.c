#include "kernel.h"

int main(int argc, char* argv[]) {
   

    char *puerto_escucha;
    char *ip_memoria;
    char *puerto_memoria;
    char *ip_cpu;
    char *puerto_cpu_dispatch;
    char *puerto_cpu_interrupt;
    char *algoritmo_planificacion;
    char *quantum;
    char *recursos;
    char *instancias_recursos;
    char *grado_multiprogramacion;

    t_config *config;
    
    // Inicio el log
	logger = iniciar_logger();

	// Inicio el config
	config = iniciar_config();
    //  ARCHIVOS DE CONFIGURACION

    // Inicio el log
	logger = iniciar_logger();

	// Inicio el config
	config = iniciar_config();

    puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    ip_cpu = config_get_string_value(config, "IP_CPU");
    puerto_cpu_dispatch = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    puerto_cpu_interrupt = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
    algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    quantum = config_get_string_value(config, "QUANTUM");
    recursos = config_get_string_value(config, "RECURSOS");
    instancias_recursos = config_get_string_value(config, "INSTANCIAS_RECURSOS");
    grado_multiprogramacion = config_get_string_value(config, "GRADO_MULTIPROGRAMACION");

    log_info(logger, "%s", ip_memoria);

	// Conexion con el modulo memoria
	int conexion_memoria;
    conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
	enviar_mensaje("HOLA", conexion_memoria);
	paquete(conexion_memoria);

    terminar_programa(conexion_memoria, logger , config);

	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
	enviar_mensaje("Hola soy el kernel me estoy comunicando con memoria", conexion_memoria);
	paquete(conexion_memoria);

	terminar_programa(conexion_memoria, logger, config);

	return 0; 
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

t_config *iniciar_config(void)
{
	t_config *nuevo_config;
	nuevo_config = config_create("../kernel.config");
	if (nuevo_config == NULL)
	{
		printf("No se pudo crear el config.");
		exit(2);
	}

	return nuevo_config;
}

<<<<<<< HEAD
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

void terminar_programa(t_config *config)
=======
void leer_consola(t_log *logger)
>>>>>>> 323aa4c0cf2068a82d403de177303843ff3300da
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
<<<<<<< HEAD
}


t_config *iniciar_config(void)
{
	t_config *nuevo_config;
	nuevo_config = config_create("kernel.config"); 
	if (nuevo_config == NULL){
		printf("No se pudo crear el config.");
		exit(2);
	}

	return nuevo_config;
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

=======

>>>>>>> 323aa4c0cf2068a82d403de177303843ff3300da
	liberar_conexion(conexion);
}