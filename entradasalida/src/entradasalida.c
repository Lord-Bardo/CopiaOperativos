#include "../include/entradasalida.h"

int main(int argc, char* argv[]) {
	
    // Inicializar estructuras de entradasalida (loggers y config)
	// inicializar_entradasalida();
	if (argc != 3) {
		perror("Cantidad de argumentos erronea"); /* Ver por qué salta cuando compilo y no cuando ejecuto */
        return 1;
    }

	char* nombre_interfaz = argv[1];
	char* archivo_configuracion = argv[2];

	inicializar_interfaz(nombre_interfaz, archivo_configuracion); // ver si toma bien el archivo

	// Conexion con KERNEL
	conectar_a_kernel(nombre_interfaz); // Le puedo pasar el nombre de interfaz para el logger? VER COMO HACERLO
	//atender_entradasalida_kernel(); // Puesto para probar, en realidad tiene que ejecutarse en el hilo creado más abajo

	// Conexion con MEMORIA
	conectar_a_memoria();

	// Atender los mensajes de Memoria
	pthread_t hilo_memoria;
	pthread_create(&hilo_memoria, NULL, (void*)atender_entradasalida_memoria, NULL);

	// Atender los mensajes de Kernel
	pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_entradasalida_kernel, NULL);

	// Espera a que los hilos finalicen su ejecución
	pthread_join(hilo_kernel, NULL);
	pthread_join(hilo_memoria, NULL);

	// Deberia finalizar filesystem tambien -- ver como implementarlo
	/* if (dialfs_inicializado){
	   terminar_filesystem();
	} */
	
    // Finalizar ENTRADASALIDA (liberar memoria usada)
	terminar_programa();

	return 0;
}

void conectar_a_kernel(char* nombre_interfaz){
	fd_kernel = crear_conexion(IP_KERNEL, PUERTO_KERNEL);
	t_paquete* paquete = crear_paquete(HANDSHAKE_ENTRADASALIDA);
	agregar_string_a_paquete(paquete,nombre_interfaz);
	agregar_string_a_paquete(paquete,TIPO_INTERFAZ);

	// if (strcmp(nombre_interfaz,"generica") == 0){	
	// 	agregar_a_paquete(paquete,GENERICA,sizeof(t_tipo_interfaz));
	// }
	// else if (strcmp(nombre_interfaz,"stdin") == 0){	
	// 	agregar_a_paquete(paquete,STDIN,sizeof(t_tipo_interfaz));
	// }
	// else if (strcmp(nombre_interfaz,"stdout") == 0){	
	// 	agregar_a_paquete(paquete,STDOUT,sizeof(t_tipo_interfaz));
	// }
	// else if (strcmp(nombre_interfaz,"dialfs") == 0){	
	// 	agregar_a_paquete(paquete,DIALFS,sizeof(t_tipo_interfaz));
	// }

	enviar_paquete(fd_kernel,paquete);
	eliminar_paquete(paquete);

	// fd_kernel = crear_conexion(IP_KERNEL, PUERTO_KERNEL);
	// enviar_handshake(fd_kernel, HANDSHAKE_ENTRADASALIDA);

	if( recibir_handshake(fd_kernel) == HANDSHAKE_OK ){
		log_info(entradasalida_logger, "Conexion con KERNEL establecida!");
	}
	else{
		log_info(entradasalida_logger, "No se pudo establecer conexion con KERNEL!");
	}
}

// Recordarle a mati que lo agregue al utils general
// void agregar_string_a_paquete(t_paquete *paquete, char *string){
//     agregar_a_paquete(paquete, string, string_length(string)+1); // +1 para contar el '\0'
// }

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

	/* if(generica_logger != NULL){
		log_destroy(generica_logger);
	}
	if(stdin_logger != NULL){
		log_destroy(stdin_logger);
	}
	if(stdout_logger != NULL){
		log_destroy(stdout_logger);
	}
	if(dialfs_logger != NULL){
		log_destroy(dialfs_logger);
	} */

	liberar_conexion(fd_kernel);
	liberar_conexion(fd_memoria);
}