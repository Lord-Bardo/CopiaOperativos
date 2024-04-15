#include "../include/cpu_memoria.h"

void atender_cpu_memoria(){
    int continuar = 1;
	while( continuar ){
		int cod_op = recibir_operacion(fd_memoria);
		switch(cod_op){
			case MENSAJE:
				// ...
				break;
			case PAQUETE:
				// ...
				break;
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