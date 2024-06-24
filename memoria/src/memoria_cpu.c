#include "../include/memoria_cpu.h"


    // Inicialización de t_instruccion 'hardcode' con '0' como opcode y 'argsSet' como args
char *hardcode = "SET AX 10";//como el orto

void atender_memoria_cpu(){
    int continuar = 1;
	while(continuar){
		//int cod_op = recibir_operacion(fd_cpu); VA ESTA LINEA, LA DE ABAJO ES SOLO PARA TEST
		t_codigo_operacion cod_op = FETCH;
		switch(cod_op){
			case FETCH: 
				//recibir_mensaje(fd_cpu);//aca deberia ser elmismo tipo de lo que nos mandaron, recibirFetch
				t_paquete *paquete_instruccion = crear_paquete(INSTRUCCION);
				agregar_a_paquete(paquete_instruccion, hardcode, (string_length(hardcode)+1+ sizeof(int)) );//me hace ruido el tamaño
				//void *enviar = serializar_paquete(paquete_instruccion, sizeof(int)+ paquete_instruccion->buffer->size);
				// si no hay quilombo con el paquete devolver la instrucicon
				enviar_paquete(fd_cpu, paquete_instruccion);
			
				break;
            case DATO: 	
			    // lógica para enviar el paquete con la dirección lógica del dato que necesito
				break; 	
			case -1:
				log_error(memoria_logger, "Se perdio la conexion con CPU!");
				continuar = 0;
				break;
			default:
				log_warning(memoria_logger, "MEMORIA: Operacion desconocida recibida de CPU");
				break;
		}
	}
}