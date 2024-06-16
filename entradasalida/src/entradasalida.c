#include "../include/entradasalida.h"

int main(int argc, char* argv[]) {
    // Inicializar estructuras de entradasalida (loggers y config)
	inicializar_entradasalida();

	// Conexion con KERNEL
	conectar_a_kernel();

	// Conexion con MEMORIA
	conectar_a_memoria();

	// Atender los mensajes de Memoria
	pthread_t hilo_memoria;
	pthread_create(&hilo_memoria, NULL, (void*)atender_entradasalida_memoria,NULL);

	// Atender los mensajes de Kernel
	pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_entradasalida_kernel, NULL);

	// Espera a que los hilos finalicen su ejecución
	pthread_join(hilo_kernel, NULL);
	pthread_join(hilo_memoria, NULL);

    // Finalizar ENTRADASALIDA (liberar memoria usada)
	terminar_programa();

	return 0;
}

/* void iniciar_interfaz(char* nombre, t_config *archivo_configuracion){

} */

void conectar_a_kernel(){
	fd_kernel = crear_conexion(IP_KERNEL, PUERTO_KERNEL);
	enviar_handshake(fd_kernel, HANDSHAKE_ENTRADASALIDA);
	if( recibir_handshake(fd_kernel) == HANDSHAKE_OK ){
		log_info(entradasalida_logger, "Conexion con KERNEL establecida!");
	}
	else{
		log_info(entradasalida_logger, "No se pudo establecer conexion con KERNEL!");
	}
}

void conectar_a_memoria(){
	fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
	enviar_handshake(fd_memoria, HANDSHAKE_ENTRADASALIDA);
	if( recibir_handshake(fd_memoria) == HANDSHAKE_OK ){
		log_info(entradasalida_logger, "Conexion con MEMORIA establecida!");
	}
	else{
		log_info(entradasalida_logger, "No se pudo establecer conexion con MEMORIA!");
	}
}
    
void terminar_programa(){
	if(entradasalida_logger != NULL){
		log_destroy(entradasalida_logger);
	}

	if(entradasalida_logger_min_y_obl != NULL){
		log_destroy(entradasalida_logger_min_y_obl);
	}

	if(entradasalida_config != NULL){
		config_destroy(entradasalida_config);
	}

	liberar_conexion(fd_kernel);
	liberar_conexion(fd_memoria)
}
