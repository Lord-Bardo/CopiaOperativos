#include "../include/memoria.h"

t_log* memoria_logger;
t_config* memoria_config;

char *PUERTO_ESCUCHA;
int TAM_MEMORIA;
int TAM_PAGINA;
char *PATH_INSTRUCCIONES;
int RETARDO_REPUESTA;

int fd_cpu;
int fd_kernel;
int fd_entradasalida;
int fd_memoria;

int main(int argc, char* argv[]) {
	// Inicializar estructuras de memoria (loggers y config)
	inicializar_memoria();
	//atender_memoria_cpu();
	
	// Inciar servidor de Memoria
	fd_memoria = iniciar_servidor(PUERTO_ESCUCHA);
														
	log_info(memoria_logger, "Servidor MEMORIA iniciado!");
	
//  ------------TEST CREAR PROCESO (SIN ENVÍO Y RECIBO DE PAQUETE)----------------
	t_pcb_memoria *proceso_recibido = malloc(sizeof(t_pcb_memoria));
	proceso_recibido->tabla_paginas = malloc((TAM_MEMORIA / TAM_PAGINA) * sizeof(t_pagina));
	proceso_recibido->memoria_de_instrucciones = malloc(TAM_MEMORIA * sizeof(char*));

	for (int i = 0; i < TAM_MEMORIA; i++) 
		proceso_recibido->memoria_de_instrucciones[i] = malloc(sizeof(char));
	
	num_instruccion = 0;
	if (proceso_recibido->tabla_paginas == NULL || proceso_recibido->memoria_de_instrucciones == NULL) {
		// Manejar el error de asignación de memoria
		fprintf(stderr, "Error al asignar memoria\n");
		exit(EXIT_FAILURE);
	}
	proceso_recibido->pid = 123;
	proceso_recibido->path = "/ArchivoPseudocodigo.txt"; 

	crear_proceso(proceso_recibido);
	printf("Primera instruccion: %s\n", procesos[0].memoria_de_instrucciones[0]);
	printf("Ultima instruccion: %s\n", procesos[0].memoria_de_instrucciones[20]);
	log_info(memoria_logger, "Entré y salí de crear proceso y cree proceso existosamente :)");
	liberar_pcb_memoria(proceso_recibido);
//  ------------------FIN DE TEST, GRACIAS VUELVA PRONTOS :)------------------------
/* 
	// Esperar conexion de CPU
	aceptar_conexion_cpu();

 */	// Esperar conexion de KERNEL
	aceptar_conexion_kernel();
 
	// Esperar conexion de ENTRADASALIDA
	aceptar_conexion_entradasalida();

/*	// Atender los mensajes de CPU
	pthread_t hilo_cpu;
	pthread_create(&hilo_cpu, NULL, (void*)atender_memoria_cpu, NULL);
*/
	// Atender los mensajes de ENTRADASALIDA
	pthread_t hilo_entradasalida;
	pthread_create(&hilo_entradasalida, NULL, (void*)atender_memoria_entradasalida, NULL);

	// Atender los mensajes de KERNEL 
	pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_memoria_kernel, NULL);
	
	// Esperar a que los hilos finalicen su ejecucion
	pthread_join(hilo_kernel, NULL); 
	pthread_join(hilo_entradasalida, NULL);
	//pthread_join(hilo_cpu, NULL);


	// Finalizar MEMORIA (liberar memoria usada)
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
	if( recibir_handshake(fd_cpu) == HANDSHAKE_CPU){
		enviar_handshake(fd_cpu, HANDSHAKE_OK);
		log_info(memoria_logger, "¡Se conectó el cliente CPU al servidor MEMORIA!");
	}
	else{
		enviar_handshake(fd_cpu, HANDSHAKE_ERROR);
	}
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
