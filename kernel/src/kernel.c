#include "../include/kernel.h"

int main(int argc, char* argv[]) {
	// Inicializar estructuras de KERNEL (loggers y config)
	inicializar_kernel();

	// Conexion con MEMORIA
	fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
	log_info(kernel_logger, "Conexion con MEMORIA establecida!");

	// Conexion con CPU - DISPATCH
	fd_cpu_dispatch = crear_conexion(IP_CPU, PUERTO_CPU_DISPATCH);
	log_info(kernel_logger, "Conexion con CPU DISPATCH establecida!");

	// Conexion con CPU - INTERRUPT
	fd_cpu_interrupt = crear_conexion(IP_CPU, PUERTO_CPU_INTERRUPT);
	log_info(kernel_logger, "Conexion con CPU INTERRUPT establecida!");

	// Iniciar servidor de KERNEL
	fd_kernel = iniciar_servidor(PUERTO_ESCUCHA);
	log_info(kernel_logger, "Servidor KERNEL iniciado!");

	// Esperar conexion de ENTRADASALIDA
	fd_entradasalida = esperar_cliente(fd_kernel);
	log_info(kernel_logger, "Se conecto el cliente ENTRADASALIDA al servidor KERNEL!");

	// Atender los mensajes de ENTRADASALIDA 
	pthread_t hilo_entradasalida;
	pthread_create(&hilo_entradasalida, NULL, (void*)atender_kernel_entradasalida, NULL);
	atender_kernel_entradasalida();

	// Atender los mensajes de MEMORIA
	pthread_t hilo_memoria;
	pthread_create(&hilo_memoria, NULL, (void*)atender_kernel_memoria, NULL);

    // Atender los mensajes de CPU - DISPATCH
	pthread_t hilo_cpu_dispatch;
	pthread_create(&hilo_cpu_dispatch, NULL, (void*)atender_kernel_cpu_dispatch, NULL);

	// Atender los mensajes de CPU - INTERRUPT
	pthread_t hilo_cpu_interrupt;
	pthread_create(&hilo_cpu_interrupt, NULL, (void*)atender_kernel_cpu_interrupt, NULL);

    // Esperar a que los hilos finalicen su ejecucion
	pthread_join(hilo_entradasalida, NULL); // en el segundo parametro se guarda el resultado de la funcion q se ejecuto en el hilo, si le pongo NULL basicamente es q no me interesa el resultado, solo me importa esperar a q termine
	pthread_join(hilo_memoria, NULL);
	pthread_join(hilo_cpu_dispatch, NULL);
	pthread_join(hilo_cpu_interrupt, NULL);

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

void terminar_programa(){
	if(kernel_logger != NULL){
		log_destroy(kernel_logger);
	}

	if(kernel_config != NULL){
		config_destroy(kernel_config);
	}

	liberar_conexion(fd_cpu_dispatch);
	liberar_conexion(fd_cpu_interrupt);
	liberar_conexion(fd_memoria);
	liberar_conexion(fd_kernel);
	liberar_conexion(fd_entradasalida);
}