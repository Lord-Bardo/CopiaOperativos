#include "../include/cpu_memoria.h"

void atender_cpu_memoria(){
    int continuar = 1;
	while(continuar){
		t_codigo_operacion cod_op;
		t_buffer * buffer = crear_buffer();
		recibir_paquete(fd_memoria,&cod_op,buffer);
		switch(cod_op){ 
			case FETCH: // ... aca hay que recibir bb recive_buffer size instruccion
				
				//contemplar interrupciones?
				break;
			case DATO:
			    //recibir_dato(fd_memoria);    
			case 8:
				log_error(cpu_logger, "Se perdio la conexion con MEMORIA!");
				continuar = 0;
				break;
			default:
				log_warning(cpu_logger, "CPU: Operacion desconocida recibida de MEMORIA");
				break;
		}
	}
}