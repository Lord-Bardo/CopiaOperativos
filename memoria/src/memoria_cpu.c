#include "../include/memoria_cpu.h"

void atender_memoria_cpu(){
    int continuar = 1;
	while(continuar){
		t_codigo_operacion cod_op;
		t_buffer *buffer = crear_buffer();
		recibir_paquete(fd_cpu, &cod_op, buffer);
		switch(cod_op){
			case FETCH: 
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

            case FRAME:
				t_paquete *paquete_frame = crear_paquete(FRAME);
				int frame_hardcode = 3; //ACA FRAN DEBERIAS LLAMAR A UNA FUNCION TIPO obtener_frame(nro_pag)

				agregar_a_paquete(paquete_frame,&frame_hardcode,sizeof(int));
			    // lógica para enviar el paquete con la dirección lógica del dato que necesito
				break; 	
			case OP_RESIZE:
				//aca recibirias lo demas querido.
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