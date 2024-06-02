#include "../include/cpu_memoria.h"

void atender_cpu_memoria(){
    int continuar = 1;
	while(continuar){
		int tipo_de_paquete = recibir_operacion(fd_memoria);
		switch(tipo_de_paquete){
			case INSTRUCCION: // ... aca hay que recibir bb recive_buffer size instruccion
				recibir_instruccion(fd_memoria); 
				//contemplar interrupciones?
				break;
			case DATO:
			    recibir_dato(fd_memoria);
			case -1:
				log_error(cpu_logger, "Se perdio la conexion con MEMORIA!");
				continuar = 0;
				break;
			default:
				log_warning(cpu_logger, "CPU: Operacion desconocida recibida de MEMORIA");
				break;
		}
	}
}
/*
SERIALIZED,             // 0
    MESSAGE_OK,             // 1
    MESSAGE_FLAW,           // 2
    MESSAGE_DONE,           // 3
    INSTRUCTIONS,           // 4
    EXECUTION_CONTEXT       // 5
*/