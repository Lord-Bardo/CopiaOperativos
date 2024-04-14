#include "kernel.h"

int main(int argc, char* argv[]) {
	// Inicializar estructuras de KERNEL (loggers y config)
	inicializar_kernel();

	// Iniciar servidor KERNEL
	// ...

	// Conexion con el modulo memoria
	int conexion_memoria;
    conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
	enviar_mensaje("HOLA", conexion_memoria);
	paquete(conexion_memoria);

    terminar_programa(conexion_memoria, logger , config);

	//conexion con entrada salida
	servidor("8003");

	return 0; 
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

