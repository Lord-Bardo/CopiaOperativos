#include "entradasalida.h"

int main(int argc, char* argv[]) {
    
    int conexion_memoria;


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

	// Obtengo valores del archivo entradasalida.config
    tipo_interfaz = config_get_string_value(config, "TIPO_INTERFAZ");
	tiempo_unidad_trabajo = config_get_string_value(config, "TIEMPO_UNIDAD_TRABAJO");
	ip_kernel = config_get_string_value(config, "IP_KERNEL");
    puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_memoria = config_get_string_value (config, "PUERTO_MEMORIA");
    path_base_dialfs = config_get_string_value (config, "PATH_BASE_DIALFS");
    block_size = config_get_string_value (config, "BLOCK_SIZE");
    block_count = config_get_string_value (config, "BLOCK_COUNT");

	log_info(logger, "%s", ip_kernel); //registrar mensaje info

	//conexion con el modulo de memoria
	conexion_memoria = crear_conexion (ip_memoria, puerto_memoria);
	enviar_mensaje ("Hola hola", conexion_memoria);
	paquete(conexion_memoria);

	/*servidor();*/

	terminar_programa(conexion_memoria, logger, config);

	//conexion con el modulo del kernel
	int conexion_kernel;
	conexion_kernel = crear_conexion (ip_kernel, puerto_kernel);
	enviar_mensaje ("Holi señor Kernel", conexion_kernel);
	paquete(conexion_kernel);

	/*servidor(); //revisar si va*/

	terminar_programa(conexion_kernel, logger, config);
	
	

	return 0;
}

 t_config* iniciar_config(void) 
 {
	t_config *nuevo_config;
	nuevo_config = config_create("../entradasalida.config");
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
