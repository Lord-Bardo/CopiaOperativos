#include "../include/memoria.h"

t_log* memoria_logger;
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
	// Inicializar estructuras de memoria (loggers y config)
	inicializar_memoria();
	
	// Inciar servidor de Memoria
	fd_memoria = iniciar_servidor(PUERTO_ESCUCHA);
														
	log_info(memoria_logger, "Servidor MEMORIA iniciado!");
	
//  -----------------------------------TESTS (SIN ENVÍO Y RECIBO DE PAQUETES)---------------------------------------------------------------------
/*
	// TEST - CREAR PROCESO
	t_pcb_memoria *proceso_recibido = inicializar_proceso();
	
	num_instruccion = 0;
	
	proceso_recibido->pid = 123;
	proceso_recibido->path = "/ArchivoPseudocodigo.txt"; 

	crear_proceso(proceso_recibido);
	
	// TEST - RESIZE
	resize(123, 5);
	printf("El tamanio del proceso es: %d\n", sizeof_proceso(*proceso_recibido)); // debería ser 5.
	printf("Ultimo frame asignado: %d\n", procesos[0].tabla_paginas[4].num_frame); // debería ser 4.
	resize(123, 10); // agrando el proceso.
	printf("El tamanio del proceso agrandado es: %d\n", sizeof_proceso(*proceso_recibido)); // debería ser 10
	printf("Ultimo frame asignado: %d\n", procesos[0].tabla_paginas[9].num_frame); // debería ser 9.
	resize(123, 4); // reduzco el proceso.
	printf("El tamanio del proceso reducido es: %d\n", sizeof_proceso(*proceso_recibido)); // debería ser 4
	printf("Ultimo frame liberado: %d\n", procesos[0].tabla_paginas[4].num_frame); // debería ser -1. 

    // TEST OBTENER FRAME - ACCESO A TABLA DE PÁGINAS
	int frame;
	obtener_frame(123, 3, frame);

	// TEST - FETCH
	char* instruccion = malloc(sizeof(char));
	obtener_instruccion(123, 5, instruccion);
	printf("La instruccion solicitada fue: %s\n", instruccion); 
	log_info(memoria_logger, "Se obtuvo instrucción existosamente :)");

	// TEST - ESCRIBIR/LEER
	int dato_a_escribir = 12345;
    int direc_fisica = 100;

	escribir(direc_fisica, sizeof(int), &dato_a_escribir);

	int dato_escrito;
    memcpy(&dato_escrito, (char*)espacio_usuario + direc_fisica, sizeof(int));
    printf("Valor escrito en el espacio de usuario: %d\n", dato_escrito);

	int dato_leido = 0;

    leer(direc_fisica, sizeof(int), &dato_leido);

    
    printf("Valor leído del espacio de usuario: %d\n", dato_leido);

    // TEST - FINALIZAR PROCESO
	finalizar_proceso(123);

	printf("PID del espacio vacío: %d\n", procesos[0].pid);
	log_info(memoria_logger, "Finalizó el proceso existosamente :)");
*/
//  ----------------------------------FIN DE TEST, GRACIAS VUELVA PRONTOS :)----------------------------------------------------------------------------------------


//  Esperar conexion de CPU
	aceptar_conexion_cpu();

//  Esperar conexion de KERNEL
	aceptar_conexion_kernel();
 
//  Esperar conexion de ENTRADASALIDA
 	//aceptar_conexion_entradasalida();

//  Atender los mensajes de CPU
	pthread_t hilo_cpu;
	pthread_create(&hilo_cpu, NULL, (void*)atender_memoria_cpu, NULL);

//  Atender los mensajes de ENTRADASALIDA
    //pthread_t hilo_entradasalida;
    //pthread_create(&hilo_entradasalida, NULL, (void*)atender_memoria_entradasalida, NULL);

//  Atender los mensajes de KERNEL 
	pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_memoria_kernel, NULL);
	
//  Esperar a que los hilos finalicen su ejecucion
	pthread_join(hilo_kernel, NULL); 
    //pthread_join(hilo_entradasalida, NULL);
    pthread_join(hilo_cpu, NULL);


//  Finalizar MEMORIA (liberar memoria usada)
	terminar_programa();

	return 0;
}

void terminar_programa(){
    if(memoria_logger != NULL){
        log_destroy(memoria_logger);
    }
    
    if(memoria_config != NULL){
		config_destroy(memoria_config);
	}

	// Destruyo los semáforos
	pthread_mutex_destroy(&mutex_espacio_usuario);
	pthread_mutex_destroy(&mutex_procesos);

	// Destruyo mis procesos, el espacio de usuario y los frames libres.
	list_destroy_and_destroy_elements(procesos, free);
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
	else
		enviar_handshake(fd_cpu, HANDSHAKE_ERROR);
}

void aceptar_conexion_entradasalida(){
	fd_entradasalida = esperar_cliente(fd_memoria);
	if( recibir_handshake(fd_entradasalida) == HANDSHAKE_ENTRADASALIDA ){
		enviar_handshake(fd_entradasalida, HANDSHAKE_OK);
		log_info(memoria_logger, "¡Se conectó el cliente ENTRADASALIDA al servidor MEMORIA!");
	}
	else{
		enviar_handshake(fd_entradasalida, HANDSHAKE_ERROR);
	}
}
