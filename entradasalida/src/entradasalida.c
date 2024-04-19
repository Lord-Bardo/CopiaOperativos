#include "../include/entradasalida.h"

int main(int argc, char* argv[]) {
    // Inicializar estructuras de entradasalida (loggers y config)
	inicializar_entradasalida();

	//Conexion con KERNEL
	fd_kernel = crear_conexion(IP_KERNEL, PUERTO_KERNEL);
	log_info(entradasalida_logger, "Conexion con KERNEL establecida!");

    // Finalizar ENTRADASALIDA (liberar memoria usada por estructuras de ENTRADASALIDA)
	terminar_programa(conexion_kernel, entradasalida_logger, entradasalida_config);

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
	if (entradasalida_logger != NULL)
	{
		log_destroy(entradasalida_logger);
	}

	if (entradasalida_config != NULL)
	{
		config_destroy(entradasalida_config);
	}
	liberar_conexion(fd_kernel);
}
