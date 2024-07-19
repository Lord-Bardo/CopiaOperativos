#include "../include/memoria_cpu.h"

void atender_memoria_cpu(){
    int continuar = 1;
	while(continuar){
		t_codigo_operacion cod_op;
		t_buffer *buffer = crear_buffer();
		recibir_paquete(fd_cpu, &cod_op, buffer);
		switch(cod_op){
			case FETCH:
			    // TIEMPO DE RETARDO
                usleep(RETARDO_REPUESTA);

				// Creo estructuras necesarias.
				int pid_fetch, pc;
				char* instruccion = malloc(sizeof(char)); 
				if (instruccion == NULL) {
					perror("Error al asignar memoria");
					enviar_codigo_operacion(fd_cpu, FETCH_ERROR); 
					exit(EXIT_FAILURE);
				} 

				// Desempaqueto el buffer y almaceno info recibida.
				buffer_desempaquetar(buffer, &pid_fetch);
				buffer_desempaquetar(buffer, &pc);
				

				// Busco y obtengo la instrucción solicitada
				obtener_instruccion(pid_fetch, pc, instruccion);
				printf("La instruccion solicitada fue: %s\n", instruccion); // BORRAR una vez finalizado el testeo

				// Empaqueto la instrucción con el opcode correspondiente y lo envío a CPU.
				t_paquete* paquete_fetch = crear_paquete(INSTRUCCION);
				agregar_a_paquete(paquete_fetch, instruccion, sizeof(instruccion));
				enviar_paquete(fd_cpu, paquete_fetch);

				// Libero memoria.
				eliminar_buffer(buffer);
				free(instruccion);
				eliminar_paquete(paquete_fetch);

				break;

            case FRAME: //PENDIENTE DE TESTEAR (hacerlo luego de testear escribir)
			    // TIEMPO DE RETARDO
                usleep(RETARDO_REPUESTA);

				// Creo estructuras necesarias.
				int pid_frame, pag, frame;

				// Desempaqueto el buffer y almaceno información recibida.
				buffer_desempaquetar(buffer, &pid_frame);
				buffer_desempaquetar(buffer, &pag);

				// Busco y obtengo el frame.
				obtener_frame(pid_frame, pag, &frame);

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
                usleep(RETARDO_REPUESTA);
				
				// Creo estructuras necesarias.
				int pid_resize, size;

				// Desempaqueto el buffer y almaceno información recibida.
				buffer_desempaquetar(buffer, &pid_resize);
				buffer_desempaquetar(buffer, &size);

				// Cambio el tamaño del proceso.
				resize(pid_resize, size);

				// Envío confirmación de escritura.
				enviar_codigo_operacion(fd_cpu, CONFIRMACION_RESIZE);

				// Libero memoria.
				eliminar_buffer(buffer);

				break;

			case SOLICITUD_ESCRITURA: // me va a llegar una direc. física que representa la cantidad de bytes que debo desplazarme desde le inicio del espacio de usuario y el dato a escribir.
			    // TIEMPO DE RETARDO
                usleep(RETARDO_REPUESTA);
				
				// Creo estructuras necesarias.
				int frame2, offset, bytes;
				void* dato;

				// Desempaqueto y almaceno información recibida.
				buffer_desempaquetar(buffer, &frame2);
				buffer_desempaquetar(buffer, &offset);
				buffer_desempaquetar(buffer, dato);
				buffer_desempaquetar(buffer, &bytes);

				// Escribo en el espacio de usuario de la memoria.
				escribir(frame, offset, dato, bytes);

				// Envío confirmación de escritura.
				enviar_codigo_operacion(fd_cpu, CONFIRMACION_ESCRITURA);

				// Libero memoria.
				eliminar_buffer(buffer);

				break;

			case SOLICITUD_LECTURA:
			    // TIEMPO DE RETARDO
                usleep(RETARDO_REPUESTA);
				//TODO
				break;

			default:
				log_warning(memoria_logger, "MEMORIA: Operacion desconocida recibida de CPU");
				break;
		}
	}
}

void obtener_instruccion(int pid, int pc, char* instruccion)
{
	// Obtengo índice del proceso con el pid.
	int index = encontrar_proceso(pid);

	// Si no encuentro, lanzo error.
	if(procesos[index].pid == -1){
		perror("Error al asignar memoria");
		enviar_codigo_operacion(fd_cpu, FETCH_ERROR); 
		exit(EXIT_FAILURE);
	}

	// Obtengo la instrucción.
	strcpy(instruccion, procesos[index].memoria_de_instrucciones[pc]);
}

void obtener_frame(int pid, int pag, int* frame) //PENDIENTE DE TESTEAR (hacerlo luego de testear escribir)
{
	// Obtengo índice del proceso con el pid.
	int index = encontrar_proceso(pid);

	// Si no encuentro, lanzo error.
	if(procesos[index].pid == -1 || procesos[index].tabla_paginas[pag].num_frame == -1){
		perror("Error al asignar memoria");
		enviar_codigo_operacion(fd_cpu, FRAME_ERROR); 
		exit(EXIT_FAILURE);
	}

	// Obtengo el frame.
	*frame = procesos[index].tabla_paginas[pag].num_frame;

	// Log obligatorio.
	log_info(memoria_logger, "PID: %d - Pagina: %d - Marco: %d\n", procesos[index].pid, pag, procesos[index].tabla_paginas[pag].num_frame);
}

void escribir(int frame, int offset, void* dato, int bytes)
{
	//TODO - hacer en utils porque es compartida con ENTRADA-SALIDA (en realidad todas deberían estar en utils opino).
}
void resize(int pid, int size)
{
	// Obtengo índice del proceso con el pid.
	int index = encontrar_proceso(pid);

	// Si el proceso no tiene asignado páginas aún, se las creo por primera vez.
	if(procesos[index].tabla_paginas[0].num_frame == -1)
	    asignar_size_proceso(index, size);
	

	// Si el proceso tiene menos páginas que el size, aumento su tamaño.
	if(procesos[index].tabla_paginas[0].num_frame != -1 && sizeof_proceso(index) < size)
	    aumentar_proceso(index, size);
	
	// Si el proceso tiene más páginas que el size, disminuyo su tamaño.
	if(procesos[index].tabla_paginas[0].num_frame != -1 && sizeof_proceso(index) > size)
	    reducir_proceso(index, size);

	// ENGINEER'S NOTE: NO ENTIENDO cómo saber cuáles son los frames libres (para asignar), 
	//                  incluyendo a los frames que se liberan al reducir el tamaño de un 
	//                  proceso. Posible solución: usar una estructura a parte del espacio
	//                  de usuario que contenga a todos los frames y que indiquen si estos 
	//                  estan disponibles o no (se deberá actualizar con cada resize). 	
}