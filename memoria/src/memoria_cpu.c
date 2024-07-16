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
				int pid, pc;
				char* instruccion = malloc(sizeof(char)); 
				if (instruccion == NULL) {
					perror("Error al asignar memoria");
					enviar_codigo_operacion(fd_cpu, FETCH_ERROR); 
					exit(EXIT_FAILURE);
				} 

				// Desempaqueto el buffer y almaceno info recibida.
				buffer_desempaquetar(buffer, &pid);
				buffer_desempaquetar(buffer, &pc);
				

				// Busco y obtengo la instrucción solicitada
				obtener_instruccion(pid, pc, instruccion);
				printf("La instruccion solicitada fue: %s\n", instruccion); // BORRAR una vez finalizado el testeo

				// Empaqueto la instrucción con el opcode correspondiente y lo envío a CPU.
				t_paquete* paquete = crear_paquete(INSTRUCCION);
				agregar_a_paquete(paquete, instruccion, sizeof(instruccion));
				enviar_paquete(fd_cpu, paquete);

				// Libero memoria.
				eliminar_buffer(buffer);
				free(instruccion);
				eliminar_paquete(paquete);

				break;

            case FRAME: //PENDIENTE DE TESTEAR (hacerlo luego de testear escribir)
			    // TIEMPO DE RETARDO
                usleep(RETARDO_REPUESTA);

				// Creo estructuras necesarias.
				int pid2, num_pag, num_frame;

				// Desempaqueto el buffer y almaceno info recibida.
				buffer_desempaquetar(buffer, &pid2);
				buffer_desempaquetar(buffer, &num_pag);

				// Busco y obtengo el frame.
				obtener_frame(pid2, num_pag, &num_frame);

				// Empaqueto el frame solicitado.
				t_paquete* paquete2 = crear_paquete(FRAME);
				agregar_a_paquete(paquete2, &num_frame, sizeof(num_frame));
				enviar_paquete(fd_cpu, paquete2);

				// Libero memoria.
				eliminar_buffer(buffer);
				eliminar_paquete(paquete2);

				break; 	

			case OP_RESIZE:
			    // TIEMPO DE RETARDO
                usleep(RETARDO_REPUESTA);
				//TODO
				break;

			case SOLICITUD_ESCRITURA:
			    // TIEMPO DE RETARDO
                usleep(RETARDO_REPUESTA);
				//TODO
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

void obtener_frame(int pid, int num_pag, int* num_frame) //PENDIENTE DE TESTEAR (hacerlo luego de testear escribir)
{
	// Obtengo índice del proceso con el pid.
	int index = encontrar_proceso(pid);

	// Si no encuentro, lanzo error.
	if(procesos[index].pid == -1 || procesos[index].tabla_paginas[num_pag].num_frame == -1){
		perror("Error al asignar memoria");
		enviar_codigo_operacion(fd_cpu, FRAME_ERROR); 
		exit(EXIT_FAILURE);
	}

	// Obtengo el frame.
	*num_frame = procesos[index].tabla_paginas[num_pag].num_frame;
}