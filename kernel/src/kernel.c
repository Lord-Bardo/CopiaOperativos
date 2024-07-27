#include "../include/kernel.h"

int main(int argc, char* argv[]) {		

	if (argc != 2) {
		perror("Cantidad de argumentos para kernel erronea");
        return 1;
    }

	char* archivo_configuracion = argv[1];
	// Inicializar estructuras de KERNEL (loggers y config)
	inicializar_kernel(archivo_configuracion);
	
	// Iniciar planificacion (largo y corto plazo)
	iniciar_planificadores();

	// Conexion con MEMORIA
	conectar_a_memoria();

	// Conexion con CPU - DISPATCH
	conectar_a_cpu_dispatch();

	// Conexion con CPU - INTERRUPT
	conectar_a_cpu_interrupt();
 
	// Iniciar servidor de KERNEL
	fd_kernel = iniciar_servidor(PUERTO_ESCUCHA);
	log_info(kernel_logger, "Servidor KERNEL iniciado!");

	// Esperar conexiones de interfaces ENTRADASALIDA y atender los mensajes de las interfaces ENTRADASALIDA 
	pthread_t hilo_entradasalida;
	pthread_create(&hilo_entradasalida, NULL, (void*)aceptar_conexiones_entradasalida, NULL);

	//Iniciar consola interactiva
	iniciar_consola_interactiva();

    // Esperar a que los hilos finalicen su ejecucion
	pthread_join(hilo_entradasalida, NULL); // en el segundo parametro se guarda el resultado de la funcion q se ejecuto en el hilo, si le pongo NULL basicamente es q no me interesa el resultado, solo me importa esperar a q termine

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

void aceptar_conexiones_entradasalida(){
	int fd_interfaz;
	t_codigo_operacion handshake;
	t_buffer *buffer = crear_buffer();
	while(1){
		fd_interfaz = esperar_cliente(fd_kernel);
		recibir_paquete(fd_interfaz, &handshake, buffer);
		if( handshake == HANDSHAKE_ENTRADASALIDA ){
			enviar_handshake(fd_interfaz, HANDSHAKE_OK);

			// Desempaqueto los datos de la interfaz
			char *nombre_interfaz = buffer_desempaquetar_string(buffer);
			char *tipo_interfaz_string = buffer_desempaquetar_string(buffer);
			t_tipo_interfaz tipo_interfaz;
			if( strcmp(tipo_interfaz_string, "GENERICA") == 0 ){
				tipo_interfaz = GENERICA;
			}
			else if( strcmp(tipo_interfaz_string, "STDIN") == 0 ){
				tipo_interfaz = STDIN;
			}
			else if( strcmp(tipo_interfaz_string, "STDOUT") == 0 ){
				tipo_interfaz = STDOUT;
			}
			else if( strcmp(tipo_interfaz_string, "DIALFS") == 0 ){
				tipo_interfaz = DIALFS;
			}
			else{
				log_error(kernel_logger, "Tipo de interfaz desconocida!");
				break;
			}
			free(tipo_interfaz_string);

			// Agrego la interfaz al diccionario
			pthread_mutex_lock(&mutex_diccionario_interfaces);
			dictionary_put(diccionario_interfaces, nombre_interfaz, crear_interfaz(fd_interfaz, tipo_interfaz));
			pthread_mutex_unlock(&mutex_diccionario_interfaces);

			// Atiendo los mensajes de la interfaz
			pthread_t hilo_interfaz;
			pthread_create(&hilo_interfaz, NULL, (void*)atender_kernel_interfaz, (void *)nombre_interfaz);
			pthread_detach(hilo_interfaz);

			log_info(kernel_logger, "Se conecto el cliente ENTRADASALIDA-%s al servidor KERNEL!", nombre_interfaz);
		}
		else{
			enviar_handshake(fd_interfaz, HANDSHAKE_ERROR);
			liberar_conexion(fd_interfaz);
		}
	}
	eliminar_buffer(buffer);
}

void terminar_programa(){
	if(kernel_logger != NULL){
		log_destroy(kernel_logger);
	}

	if(kernel_logger_min_y_obl != NULL){
		log_destroy(kernel_logger_min_y_obl);
	}

	if(kernel_config != NULL){
		config_destroy(kernel_config);
	}

	liberar_conexion(fd_memoria);
	liberar_conexion(fd_cpu_dispatch);
	liberar_conexion(fd_cpu_interrupt);
	liberar_conexion(fd_kernel);
	// liberar_conexion(fd_entradasalida);

	// deberiamos liberar los recursos usados por los modulos (nose si aca o en otro lado)
}