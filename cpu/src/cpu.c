#include "../include/cpu.h"

int main(int argc, char *argv[]){
	// Inicializar estructuras de CPU (loggers y config)
	inicializar_cpu();

	// Iniciar servidor DISPATCH de CPU
	fd_cpu_dispatch = iniciar_servidor(PUERTO_ESCUCHA_DISPATCH);
	log_info(cpu_logger, "Servidor CPU DISPATCH iniciado!");

	// Esperar conexion del KERNEL
	fd_kernel_dispatch = esperar_cliente(fd_cpu_dispatch);
	log_info(cpu_logger, "Se conecto el cliente KERNEL al servidor CPU DISPATCH!");

	// Iniciar servidor INTERRUPT de CPU
	fd_cpu_interrupt = iniciar_servidor(PUERTO_ESCUCHA_INTERRUPT);
	log_info(cpu_logger, "Servidor CPU INTERRUPT iniciado!");

	// Esperar conexion del KERNEL
	fd_kernel_interrupt = esperar_cliente(fd_cpu_interrupt);
	log_info(cpu_logger, "Se conecto el cliente KERNEL al servidor CPU INTERRUPT!");

	// Conexion con MEMORIA
	fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
	log_info(cpu_logger, "Conexion con MEMORIA establecida!");

	// Atender los mensajes de KERNEL - DISPATCH
	// declaro el hilo
	pthread_t hilo_kernel_dispatch;
	// creo el hilo. Primer parametro: donde guarda el hilo. Segundo: NULL para q se seteen los valores por defecto. Tercero: funcion a ejecutar en el hilo. Cuarto: si la funcion recibe parametros van aca
	pthread_create(&hilo_kernel_dispatch, NULL, (void*)atender_cpu_kernel_dispatch, NULL); // tengo duda con el tercer parametro, xq chatgpt recomienda usar un wrapper de la funcion y q tenga la firma q pide pthread_create q es void* (*)(void*) lo cual describe un puntero a una función que toma un puntero void como argumento y devuelve un puntero void. O la otra es modificar directo la firma de la funcion.

	// Atender los mensajes de KERNEL - INTERRUPT
	atender_cpu_kernel_interrupt(); // CAMBIAR POR HILOS

	// Atender los mensajes de MEMORIA
	atender_cpu_memoria(); // CAMBIAR POR HILOS

	// Esperar a que los hilos finalicen su ejecucion
	pthread_join(&hilo_kernel_dispatch, NULL); // en el segundo parametro se guarda el resultado de la funcion q se ejecuto en el hilo, si le pongo NULL basicamente es q no me interesa el resultado, solo me importa esperar a q termine
	// ...

	// Finalizar CPU (liberar memoria usada)
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
	if (cpu_logger != NULL){
		log_destroy(cpu_logger);
	}

	if (cpu_config != NULL){
		config_destroy(cpu_config);
	}

	// Los sockets del servidor creo q no hay q cerrarlos
	// liberar_conexion(fd_cpu_dispatch);
	// liberar_conexion(fd_cpu_interrupt);
	// liberar_conexion(fd_kernel_dispatch);
	// liberar_conexion(fd_kernel_interrupt);
	liberar_conexion(fd_memoria);
}