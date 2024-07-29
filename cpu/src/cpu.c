#include "../include/cpu.h"

int tamanio_pagina;
pthread_mutex_t mutex_diccionario;

int main(int argc, char *argv[]) {
	// Inicializar estructuras de CPU (loggers y config)
	if (argc != 2) {
		perror("Cantidad de argumentos para kernel erronea");
        return 1;
    }

	char* archivo_configuracion = argv[1];

	inicializar_cpu(archivo_configuracion);

	conectar_a_memoria();
	//Conexion con MEMORIA 
	
	//iniciar_ciclo_instruccion(pcb);
	// Iniciar servidor DISPATCH de CPU
	fd_cpu_dispatch = iniciar_servidor(PUERTO_ESCUCHA_DISPATCH);
	log_info(cpu_logger, "Servidor CPU DISPATCH iniciado!");

	// // Iniciar servidor INTERRUPT de CPU
	fd_cpu_interrupt = iniciar_servidor(PUERTO_ESCUCHA_INTERRUPT);
	log_info(cpu_logger, "Servidor CPU INTERRUPT iniciado!");

//----------------------------------test: envío de paquete a memoria-------------------------------------------------------------------------------------------------

//  ENGINEER'S NOTE: Para efectuar este test deben conectarse los módulos MEMORIA-KERNEL-CPU (en este orden) y crear un proceso por
//                   consola desde la terminal de KERNEL ingesando "INICIAR_PROCESO /ArchivoPseudocodigo.txt". Una vez hecho esto, 
//                   desde la terminal de CPU procedemos a ingresar cualquier letra y presionamos la tecla enter para, efectivamente, 
//                   ejecutar el código que aparece a continuación (código que queremos probar y que simula un FETCH):
/*
	readline(">");
	t_paquete* paquete = crear_paquete(FETCH);
	int pid = 0; //KERNEL asigna PIDs incrementalmente comenzando desde el cero.
	int pc = 5;
	agregar_a_paquete(paquete, &pid, sizeof(int));
	agregar_a_paquete(paquete, &pc, sizeof(int));
	enviar_paquete(fd_memoria, paquete);
	eliminar_paquete(paquete);
*/
//-------------------------------fin del test, gracias vuelva pronto!!-------------------------------------------------------------------------------------------
    

	// Esperar conexion del KERNEL
	aceptar_conexion_kernel_dispatch();

	// Esperar conexion del KERNEL
	aceptar_conexion_kernel_interrupt();

	// Atender los mensajes de KERNEL - DISPATCH
	// declaro el hilo
	pthread_t hilo_kernel_dispatch;
	// creo el hilo. Primer parametro: donde guarda el hilo. Segundo: NULL para q se seteen los valores por defecto. Tercero: funcion a ejecutar en el hilo. Cuarto: si la funcion recibe parametros van aca
	pthread_create(&hilo_kernel_dispatch, NULL, (void*)iniciar_ciclo_instruccion, NULL); // tengo duda con el tercer parametro, xq chatgpt recomienda usar un wrapper de la funcion y q tenga la firma q pide pthread_create q es void* (*)(void*) lo cual describe un puntero a una función que toma un puntero void como argumento y devuelve un puntero void. O la otra es modificar directo la firma de la funcion.

	// Atender los mensajes de KERNEL - INTERRUPT
	pthread_t hilo_kernel_interrupt;
	pthread_create(&hilo_kernel_interrupt,NULL, (void*)atender_cpu_kernel_interrupt, NULL);

	//Atender los mensajes de MEMORIA
	//pthread_t hilo_memoria;
	//pthread_create(&hilo_memoria, NULL, (void*)atender_cpu_memoria, NULL); //no hacen falta hilos
	//atender_cpu_memoria();

	// Esperar a que los hilos finalicen su ejecucion
	pthread_join(hilo_kernel_dispatch, NULL); // en el segundo parametro se guarda el resultado de la funcion q se ejecuto en el hilo, si le pongo NULL basicamente es q no me interesa el resultado, solo me importa esperar a q termine
	pthread_join(hilo_kernel_interrupt, NULL);
	//pthread_join(hilo_memoria, NULL);


	// Finalizar CPU (liberar memoria usada)
	terminar_programa();

	return 0;
}


void terminar_programa(){
	if (cpu_logger != NULL){
		log_destroy(cpu_logger);
	}

	if (cpu_config != NULL){
		config_destroy(cpu_config);
	}

	liberar_conexion(fd_cpu_dispatch); 
	liberar_conexion(fd_cpu_interrupt);
	liberar_conexion(fd_kernel_dispatch);
	liberar_conexion(fd_kernel_interrupt);
	liberar_conexion(fd_memoria);
}


void conectar_a_memoria(){ 
	fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
	enviar_handshake(fd_memoria, HANDSHAKE_CPU);
	t_codigo_operacion handshake;
	t_buffer *buffer = crear_buffer();
	recibir_paquete(fd_memoria, &handshake, buffer);
	if( handshake == HANDSHAKE_OK ){
		buffer_desempaquetar(buffer, &tamanio_pagina);
		log_info(cpu_logger, "Conexion con MEMORIA establecida!");
	}
	else{
		log_info(cpu_logger, "No se pudo establcer conexion con MEMORIA!");
	}
	eliminar_buffer(buffer);
}

void aceptar_conexion_kernel_dispatch(){
	fd_kernel_dispatch = esperar_cliente(fd_cpu_dispatch);
	if( recibir_handshake(fd_kernel_dispatch) == HANDSHAKE_KERNEL){
		enviar_handshake(fd_kernel_dispatch, HANDSHAKE_OK);
		log_info(cpu_logger, "¡Se conectó el cliente KERNEL al servidor CPU!");
	}
	else{
		enviar_handshake(fd_kernel_dispatch, HANDSHAKE_ERROR);
	}
}
void aceptar_conexion_kernel_interrupt(){
	fd_kernel_interrupt = esperar_cliente(fd_cpu_interrupt);
	if( recibir_handshake(fd_kernel_interrupt) == HANDSHAKE_KERNEL){
		enviar_handshake(fd_kernel_interrupt, HANDSHAKE_OK);
		log_info(cpu_logger, "¡Se conectó el cliente KERNEL al servidor CPU!");
	}
	else{
		enviar_handshake(fd_kernel_interrupt, HANDSHAKE_ERROR);
	}
}