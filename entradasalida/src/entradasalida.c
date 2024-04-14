#include "entradasalida.h"

int main(int argc, char* argv[]) {
    char *tipo_interfaz;
	char *tiempo_unidad_trabajo;
	char *ip_kernel;
    char *puerto_kernel;
    char *ip_memoria;
    char *puerto_memoria;
    char *path_base_dialfs;
    char *block_size;
    char *block_count;

    t_config *config;
	t_log *logger;

	//inicio del config
    config = iniciar_config();

	//inicio el logger
	logger = iniciar_logger();

	

	log_info(logger, "%s", ip_kernel); //registrar mensaje info

	//conexion con el modulo de memoria
	int conexion_kernel;
	conexion_kernel = crear_conexion (ip_kernel, puerto_kernel);
	enviar_mensaje ("Hola hola", conexion_kernel);
	paquete(conexion_kernel);

	terminar_programa(conexion_kernel, logger, config);

	return 0;
}

 t_config* iniciar_config(void) 
 {
	t_config *nuevo_config;
	nuevo_config = config_create("/home/utnso/tp-2024-1c-GSN/entradasalida/entradasalida.config");
	if (nuevo_config == NULL){
		printf("No se pudo crear el config.");
		exit(2);
	}
	return nuevo_config; 
 }

t_log *iniciar_logger(void)
{
	t_log *nuevo_logger;
	nuevo_logger = log_create("entradasalida.log", "ENTRADAYSALIDA", 1, LOG_LEVEL_INFO);
	if (nuevo_logger == NULL)
	{
		printf("No se pudo crear el logger.");
		exit(1);
	}

	return nuevo_logger;
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
}
