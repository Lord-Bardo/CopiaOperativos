#include "../include/entradasalida.h"

int main(int argc, char* argv[]) {
	if (argc != 3) {
		perror("Cantidad de argumentos para entradasalida erronea");
        return 1;
    }

	char* nombre_interfaz = argv[1];
	char* archivo_configuracion = argv[2];

    // Inicializar estructuras de entradasalida (loggers y config)
	inicializar_interfaz(nombre_interfaz, archivo_configuracion); // ver si toma bien el archivo

	// Conexion con KERNEL
	conectar_a_kernel(nombre_interfaz); // Le puedo pasar el nombre de interfaz para el logger? VER COMO HACERLO

	// Conexion con MEMORIA
	conectar_a_memoria();


	// Atender los mensajes de Kernel
	pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_entradasalida_kernel, NULL);

	// Espera a que los hilos finalicen su ejecución
	pthread_join(hilo_kernel, NULL);
	
	
    // Finalizar ENTRADASALIDA (liberar memoria usada)
	terminar_programa();

	return 0;
}

void conectar_a_kernel(char* nombre_interfaz){
	fd_kernel = crear_conexion(IP_KERNEL, PUERTO_KERNEL);
	t_paquete* paquete = crear_paquete(HANDSHAKE_ENTRADASALIDA);
	agregar_string_a_paquete(paquete,nombre_interfaz);
	agregar_string_a_paquete(paquete,TIPO_INTERFAZ);

	enviar_paquete(fd_kernel,paquete);
	eliminar_paquete(paquete);

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
	liberar_conexion(fd_memoria);
}