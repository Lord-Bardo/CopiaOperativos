#include "../include/kernel.h"

int main(int argc, char* argv[]) {
	// Inicializar estructuras de KERNEL (loggers y config)
	inicializar_kernel();
	
	// Iniciar planificacion (largo y corto plazo)
	iniciar_planificadores();

	t_pcb *pcb = crear_pcb(0, "p");
	ejecutar_instruccion_signal(pcb, "RA");


	iniciar_consola_interactiva();




	// Conexion con MEMORIA
	conectar_a_memoria();

	// Conexion con CPU - DISPATCH
	conectar_a_cpu_dispatch();

	// Conexion con CPU - INTERRUPT
	conectar_a_cpu_interrupt();

	// Iniciar servidor de KERNEL
	fd_kernel = iniciar_servidor(PUERTO_ESCUCHA);
	log_info(kernel_logger, "Servidor KERNEL iniciado!");

	// Esperar conexion de ENTRADASALIDA
	aceptar_conexion_entradasalida();

	// Atender los mensajes de ENTRADASALIDA 
	pthread_t hilo_entradasalida;
	pthread_create(&hilo_entradasalida, NULL, (void*)atender_kernel_entradasalida, NULL); // Mati: vi en un issue que decian que el main es buen lugar para manejar conexiones de las interfaces e/s

	// Atender los mensajes de MEMORIA
	pthread_t hilo_memoria;
	pthread_create(&hilo_memoria, NULL, (void*)atender_kernel_memoria, NULL);

    // Atender los mensajes de CPU - DISPATCH
	pthread_t hilo_cpu_dispatch;
	pthread_create(&hilo_cpu_dispatch, NULL, (void*)atender_kernel_cpu_dispatch, NULL);

	// Iniciar consola interactiva
	iniciar_consola_interactiva();

    // Esperar a que los hilos finalicen su ejecucion
	pthread_join(hilo_entradasalida, NULL); // en el segundo parametro se guarda el resultado de la funcion q se ejecuto en el hilo, si le pongo NULL basicamente es q no me interesa el resultado, solo me importa esperar a q termine
	pthread_join(hilo_memoria, NULL);
	pthread_join(hilo_cpu_dispatch, NULL);

	// Finalizar KERNEL (liberar memoria usada)
	terminar_programa();

	return 0; 
}

void conectar_a_memoria(){
	fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
	enviar_handshake(fd_memoria, HANDSHAKE_KERNEL);
	if( recibir_handshake(fd_memoria) == HANDSHAKE_OK ){
		log_info(kernel_logger, "Conexion con MEMORIA establecida!");
	}
	else{
		log_info(kernel_logger, "No se pudo establcer conexion con MEMORIA!");
	}
}

void conectar_a_cpu_dispatch(){
	fd_cpu_dispatch = crear_conexion(IP_CPU, PUERTO_CPU_DISPATCH);
	enviar_handshake(fd_cpu_dispatch, HANDSHAKE_KERNEL);
	if( recibir_handshake(fd_cpu_dispatch) == HANDSHAKE_OK ){
		log_info(kernel_logger, "Conexion con CPU DISPATCH establecida!");
	}
	else{
		log_info(kernel_logger, "No se pudo establcer conexion con CPU DISPATCH!");
	}
}

void conectar_a_cpu_interrupt(){
	fd_cpu_interrupt = crear_conexion(IP_CPU, PUERTO_CPU_INTERRUPT);
	enviar_handshake(fd_cpu_interrupt, HANDSHAKE_KERNEL);
	if( recibir_handshake(fd_cpu_interrupt) == HANDSHAKE_OK ){
		log_info(kernel_logger, "Conexion con CPU INTERRUPT establecida!");
	}
	else{
		log_info(kernel_logger, "No se pudo establcer conexion con CPU INTERRUPT!");
	}
}

void aceptar_conexion_entradasalida(){
	fd_entradasalida = esperar_cliente(fd_kernel);
	if( recibir_handshake(fd_entradasalida) == HANDSHAKE_ENTRADASALIDA ){
		enviar_handshake(fd_entradasalida, HANDSHAKE_OK);
		log_info(kernel_logger, "Se conecto el cliente ENTRADASALIDA al servidor KERNEL!");
	}
	else{
		enviar_handshake(fd_entradasalida, HANDSHAKE_ERROR);
	}
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

	// deberiamos liberar los recursos usados por los modulos (nose si aca o en otro lado)
}