#include "../include/memoria_cpu.h"

int pid_cpu;

void atender_memoria_cpu(){
	while(1){
		t_codigo_operacion cod_op;
		t_buffer *buffer = crear_buffer();
		recibir_paquete(fd_cpu, &cod_op, buffer);

		switch(cod_op){
			case FETCH:
			    // TIEMPO DE RETARDO
                usleep(RETARDO_RESPUESTA*1000);

				// Creo estructuras necesarias.
				int pc;
				// char* instruccion = malloc(sizeof(char) * 41);
				char* instruccion = NULL;
				
				// if (instruccion == NULL) {
				// 	perror("Error al asignar memoria");
				// 	enviar_codigo_operacion(fd_cpu, FETCH_ERROR); 
				// 	exit(EXIT_FAILURE);
				// } 

				// Desempaqueto el buffer y almaceno info recibida.
				buffer_desempaquetar(buffer, &pid_cpu);
				buffer_desempaquetar(buffer, &pc);
				

				// Busco y obtengo la instrucción solicitada
				obtener_instruccion(pc, &instruccion);
				//printf("La instruccion solicitada fue: %s\n", instruccion); 

				// Empaqueto la instrucción con el opcode correspondiente y lo envío a CPU.
				t_paquete* paquete_fetch = crear_paquete(INSTRUCCION);
				agregar_string_a_paquete(paquete_fetch, instruccion);
				enviar_paquete(fd_cpu, paquete_fetch);

				// Libero memoria.
				eliminar_buffer(buffer);
				free(instruccion);
				eliminar_paquete(paquete_fetch);

				break;

            case FRAME: 
			    // TIEMPO DE RETARDO
                usleep(RETARDO_RESPUESTA*1000);

				// Creo estructuras necesarias.
				int pag, frame;

				// Desempaqueto el buffer y almaceno información recibida.
				buffer_desempaquetar(buffer, &pid_cpu);
				buffer_desempaquetar(buffer, &pag);
				
				// Busco y obtengo el frame.
				obtener_frame(pag, &frame);

				// Empaqueto el frame solicitado.
				t_paquete* paquete_frame = crear_paquete(FRAME);
				agregar_a_paquete(paquete_frame, &frame, sizeof(frame));
				enviar_paquete(fd_cpu, paquete_frame);

				// Libero memoria.
				eliminar_buffer(buffer);
				eliminar_paquete(paquete_frame);

				break; 	

			case COP_RESIZE:
			    // TIEMPO DE RETARDO
                usleep(RETARDO_RESPUESTA*1000);
				
				// Creo estructuras necesarias.
				int size;

				// Desempaqueto el buffer y almaceno información recibida.
				buffer_desempaquetar(buffer, &pid_cpu);
				buffer_desempaquetar(buffer, &size);

				// Cambio el tamaño del proceso.
				resize(size);

				// Libero memoria.
				eliminar_buffer(buffer);

				break;

			case SOLICITUD_ESCRITURA:
			    // TIEMPO DE RETARDO
                usleep(RETARDO_RESPUESTA*1000);
				
				// Creo estructuras necesarias.
				int direc_fisica_write, bytes_write;
				void* dato_write;

				// Desempaqueto y almaceno información recibida.
				buffer_desempaquetar(buffer, &pid_cpu);
				buffer_desempaquetar(buffer, &direc_fisica_write);
				buffer_desempaquetar(buffer, &bytes_write);
				dato_write = malloc(bytes_write);
				buffer_desempaquetar(buffer, dato_write);

				// Escribo en el espacio de usuario de la memoria.
				escribir(direc_fisica_write, bytes_write, dato_write);

				// Log mínimo y obligatorio - Acceso a espacio de usuario.
				printf("Log mínimo y obligatorio - Acceso a espacio de usuario\n");
				log_info(memoria_logger_min_y_obl, "PID: %d - Accion: ESCRIBIR - Direccion fisica: %d - Tamaño: %d", pid_cpu, direc_fisica_write, bytes_write);

				// Envío confirmación de escritura.
				enviar_codigo_operacion(fd_cpu, CONFIRMACION_ESCRITURA);

				// Libero memoria.
				eliminar_buffer(buffer);
				free(dato_write);
				break;

			case SOLICITUD_LECTURA:
			    // TIEMPO DE RETARDO
                usleep(RETARDO_RESPUESTA*1000);
				
				// Creo estructuras necesarias.
				int direc_fisica_read, bytes_read;
				void* dato_read;

				// Desempaqueto y almaceno información recibida.
				buffer_desempaquetar(buffer, &pid_cpu);
				buffer_desempaquetar(buffer, &direc_fisica_read);
				buffer_desempaquetar(buffer, &bytes_read);
				dato_read = malloc(bytes_read);

				// Leo el espacio de usuario de la memoria.
				leer(direc_fisica_read, bytes_read, dato_read);

				// Log mínimo y obligatorio - Acceso a espacio de usuario.
				printf("Log mínimo y obligatorio - Acceso a espacio de usuario\n");
				log_info(memoria_logger_min_y_obl, "PID: %d - Accion: LEER - Direccion fisica: %d - Tamaño: %d", pid_cpu, direc_fisica_read, bytes_read);

				// Envío dato leído.
				t_paquete* paquete_read = crear_paquete(DATO);
				agregar_a_paquete(paquete_read, dato_read, bytes_read);
				enviar_paquete(fd_cpu, paquete_read);

				// Libero memoria.
				eliminar_buffer(buffer);
				eliminar_paquete(paquete_read);
				free(dato_read);
				break;

			default:
				log_warning(memoria_logger, "MEMORIA: Operacion desconocida recibida de CPU");
				break;
		}
	}
}

void obtener_instruccion(int pc, char** instruccion)
{
	// Obtengo el proceso con el pid.
	pthread_mutex_lock(&mutex_procesos);
    t_pcb_memoria* proceso = list_find(procesos, comparar_pid_cpu);
	pthread_mutex_unlock(&mutex_procesos);

	// Obtengo la instrucción.
	char *instruccion_temp = (char *)list_get(proceso->memoria_de_instrucciones, pc);
	
	// Copio la instruccion
	*instruccion = string_duplicate(instruccion_temp);
	log_info(memoria_logger, "La instruccion obtenida es: %s", *instruccion);
	//strcpy(instruccion, (char *)list_get(proceso->memoria_de_instrucciones, pc));
}

void resize(int size)
{
	// Obtengo el proceso con el pid.
	pthread_mutex_lock(&mutex_procesos);
	t_pcb_memoria* proceso = list_find(procesos, comparar_pid_cpu);
	pthread_mutex_unlock(&mutex_procesos);

	int cant_pags = (int)ceil((double)size / (double)TAM_PAGINA);

	// Si el proceso no tiene asignado páginas aún, se las creo por primera vez.
	if(list_size(proceso->tabla_paginas) == 0)
	    asignar_size_proceso(proceso, cant_pags);

	// Si el proceso tiene menos páginas que el size, aumento su tamaño.
	else if(list_size(proceso->tabla_paginas) < cant_pags)
	    aumentar_proceso(proceso, cant_pags);
	
	// Si el proceso tiene más páginas que el size, disminuyo su tamaño.
	else if(list_size(proceso->tabla_paginas) > cant_pags)
	    reducir_proceso(proceso, cant_pags); 	
}