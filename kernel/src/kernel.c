#include "../include/kernel.h"

int main(int argc, char* argv[]) {	
	// Inicializar estructuras de KERNEL (loggers y config)
	inicializar_kernel();
	
	// Iniciar planificacion (largo y corto plazo)
	iniciar_planificadores();

	iniciar_consola_interactiva();

	// detener_planificacion();
	// t_pcb *pcb1 = crear_pcb(0, "p");
	// estado_encolar_pcb(estado_new, pcb1);
	// t_pcb *pcb2 = crear_pcb(1, "p");
	// estado_encolar_pcb(estado_new, pcb2);
	// t_pcb *pcb3 = crear_pcb(2, "p");
	// estado_encolar_pcb(estado_new, pcb3);

	// t_pcb *pcb4 = estado_rastrear_y_desencolar_pcb_por_pid(2);
	// log_info(kernel_logger, "%d", pcb_get_pid(pcb4));

	

	// HOLA FRAN, SI QUERES CODEAR ALGO HACELO ACA ENTRE CONECTAR_A_MEMORIA E INICIAR_CONSOLA_INTERACTIVA :)

	// HOLA MATI QUERIDO, GRACIAS POR EL ESPACIO Y LA BUENA ONDA, TE LO DEJO COMENTADO PARA NO VOLVERLO A HACER, 
	// ESPERO NO TE MOLESTE, CUALQUIER COSA CORTÁ Y PEGAMELO EN MEMORIA. TE MANDO UN ABRAZO, CUIDATE <3 
    
//----------------------------------test: envío de paquete a memoria--------------------------------------------------------------------------------------------
	/*
    conectar_a_memoria();

	t_paquete* paquete_proceso = crear_paquete(SOLICITUD_INICIAR_PROCESO);
	char* path = "/ArchivoPseudocodigo.txt";
	int pid = 123;
	agregar_a_paquete(paquete_proceso, &path, sizeof(char));
	agregar_a_paquete(paquete_proceso, &pid, sizeof(int));
	enviar_paquete(fd_memoria, paquete_proceso);
	eliminar_paquete(paquete_proceso);

	iniciar_consola_interactiva();

	//nota: no se por qué kernel no se conecta a memoria :(
	*/
//----------------------------------fin del test, gracias vuelva pronto!!-------------------------------------------------------------------------------------------
    
	// fd_kernel = iniciar_servidor(PUERTO_ESCUCHA);
	// log_info(kernel_logger, "Servidor KERNEL iniciado!");

	// int fd_interfaz = esperar_cliente(fd_kernel);
	// if( recibir_handshake(fd_interfaz) == HANDSHAKE_ENTRADASALIDA ){
	// 	enviar_handshake(fd_interfaz, HANDSHAKE_OK);
	// 	t_codigo_operacion codigo_operacion;
	// 	if( recibir_codigo_operacion(fd_interfaz, &codigo_operacion) <= 0 ){
	// 		log_info(kernel_logger, "SE DESCONECTOOOOOOOOOOOOOOOOOOO");
	// 		// Logica de eliminar la interfaz
	// 	}
	// }

	// t_pcb *pcb = crear_pcb(0, "p");
	// ejecutar_instruccion_wait(pcb,"RB");
	// ejecutar_instruccion_wait(pcb,"RA");
	// ejecutar_instruccion_wait(pcb,"RA");
	// ejecutar_instruccion_signal(pcb, "RA");

	

	// Conexion con MEMORIA
	conectar_a_memoria();

	// Conexion con CPU - DISPATCH
	conectar_a_cpu_dispatch();

	// Conexion con CPU - INTERRUPT
	conectar_a_cpu_interrupt();
 
	// Iniciar servidor de KERNEL
	// fd_kernel = iniciar_servidor(PUERTO_ESCUCHA);
	// log_info(kernel_logger, "Servidor KERNEL iniciado!");

	// Esperar conexiones de interfaces ENTRADASALIDA y atender los mensajes de las interfaces ENTRADASALIDA 
	// pthread_t hilo_entradasalida;
	// pthread_create(&hilo_entradasalida, NULL, (void*)aceptar_conexiones_entradasalida, NULL);

    // Atender los mensajes de CPU - DISPATCH
	// pthread_t hilo_cpu_dispatch;
	// pthread_create(&hilo_cpu_dispatch, NULL, (void*)atender_kernel_cpu_dispatch, NULL);

	// Iniciar consola interactiva
	iniciar_consola_interactiva();

    // Esperar a que los hilos finalicen su ejecucion
	// pthread_join(hilo_entradasalida, NULL); // en el segundo parametro se guarda el resultado de la funcion q se ejecuto en el hilo, si le pongo NULL basicamente es q no me interesa el resultado, solo me importa esperar a q termine
	// pthread_join(hilo_cpu_dispatch, NULL);

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
			t_tipo_interfaz tipo_interfaz;
			buffer_desempaquetar(buffer, &tipo_interfaz);

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

	if(kernel_config != NULL){
		config_destroy(kernel_config);
	}

	liberar_conexion(fd_cpu_dispatch);
	liberar_conexion(fd_cpu_interrupt);
	liberar_conexion(fd_memoria);
	liberar_conexion(fd_kernel);
	// liberar_conexion(fd_entradasalida);

	// deberiamos liberar los recursos usados por los modulos (nose si aca o en otro lado)
}