#include "../include/memoria.h"

t_log* memoria_logger;
t_log* memoria_logger_min_y_obl;
t_config* memoria_config;

char *PUERTO_ESCUCHA;
int TAM_MEMORIA;
int TAM_PAGINA;
char *PATH_INSTRUCCIONES;
int RETARDO_RESPUESTA;

int fd_cpu;
int fd_kernel;
int fd_entradasalida;
int fd_memoria;

int main(int argc, char* argv[]) {

    char* archivo_configuracion = argv[1];

	// Inicializar estructuras de memoria (loggers y config)
	inicializar_memoria(archivo_configuracion);
	
	// Inciar servidor de Memoria
	fd_memoria = iniciar_servidor(PUERTO_ESCUCHA);													
	log_info(memoria_logger, "Servidor MEMORIA iniciado!");

	// Esperar conexion de CPU
	aceptar_conexion_cpu();

	// Esperar conexion de KERNEL
	aceptar_conexion_kernel();

	// Esperar conexiones de interfaces ENTRADASALIDA y atender los mensajes de las interfaces ENTRADASALIDA 
	pthread_t hilo_entradasalida;
	pthread_create(&hilo_entradasalida, NULL, (void*)aceptar_conexiones_entradasalida, NULL);

	// Atender los mensajes de CPU
	pthread_t hilo_cpu;
	pthread_create(&hilo_cpu, NULL, (void*)atender_memoria_cpu, NULL);

	// Atender los mensajes de KERNEL 
	pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_memoria_kernel, NULL);
	
	// Esperar a que los hilos finalicen su ejecucion
	pthread_join(hilo_kernel, NULL); 
    pthread_join(hilo_entradasalida, NULL);
    pthread_join(hilo_cpu, NULL);


	// Finalizar MEMORIA (liberar memoria usada)
	terminar_programa();

	return 0;
}

void terminar_programa(){
    if(memoria_logger != NULL){
        log_destroy(memoria_logger);
    }
	if(memoria_logger_min_y_obl != NULL){
        log_destroy(memoria_logger_min_y_obl);
    }
    
    if(memoria_config != NULL){
		config_destroy(memoria_config);
	}

	// Destruyo mis procesos, el espacio de usuario y los frames libres.
	pthread_mutex_lock(&mutex_procesos);
	list_destroy_and_destroy_elements(procesos, free);
	pthread_mutex_unlock(&mutex_procesos);

	// Destruyo los semáforos
	pthread_mutex_destroy(&mutex_espacio_usuario);
	pthread_mutex_destroy(&mutex_procesos);

	free(espacio_usuario);
	bitarray_destroy(frames_libres); 

    // liberar_conexion(conexion);
	liberar_conexion(fd_cpu);
	liberar_conexion(fd_memoria);
	liberar_conexion(fd_kernel);
	liberar_conexion(fd_entradasalida);
}

void aceptar_conexion_kernel(){
	fd_kernel = esperar_cliente(fd_memoria);
	if( recibir_handshake(fd_kernel) == HANDSHAKE_KERNEL){
		enviar_handshake(fd_kernel, HANDSHAKE_OK);
		log_info(memoria_logger, "¡Se conectó el cliente KERNEL al servidor MEMORIA!");
	}
	else{
		enviar_handshake(fd_kernel, HANDSHAKE_ERROR);
	}
}

void aceptar_conexion_cpu(){
	fd_cpu = esperar_cliente(fd_memoria);
	if(recibir_handshake(fd_cpu) == HANDSHAKE_CPU){
		t_paquete* paquete = crear_paquete(HANDSHAKE_OK);
		agregar_int_a_paquete(paquete, TAM_PAGINA);
		enviar_paquete(fd_cpu, paquete);
		eliminar_paquete(paquete);
		log_info(memoria_logger, "¡Se conectó el cliente CPU al servidor MEMORIA!");
	}
	else{
		liberar_conexion(fd_cpu);
		enviar_handshake(fd_cpu, HANDSHAKE_ERROR);
	}
}

void aceptar_conexiones_entradasalida(){
	int fd_entradasalida;
	while(1){
		fd_entradasalida = esperar_cliente(fd_memoria);
		if( recibir_handshake(fd_entradasalida) == HANDSHAKE_ENTRADASALIDA ){
			enviar_handshake(fd_entradasalida, HANDSHAKE_OK);

			// Atiendo los mensajes de la interfaz
			pthread_t hilo_interfaz;
			pthread_create(&hilo_interfaz, NULL, (void*)atender_memoria_entradasalida, (void *)&fd_entradasalida);
			pthread_detach(hilo_interfaz);

			log_info(memoria_logger, "¡Se conectó el cliente ENTRADASALIDA al servidor MEMORIA!");
		}
		else{
			liberar_conexion(fd_entradasalida);
			enviar_handshake(fd_entradasalida, HANDSHAKE_ERROR);
		}
	}
}