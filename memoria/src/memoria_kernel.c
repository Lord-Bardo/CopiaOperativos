#include "../include/memoria_kernel.h"

void atender_memoria_kernel(){
    int continuar = 1;
	while( continuar ){
		int cod_op = recibir_operacion(fd_kernel);
		switch(cod_op){
			
			case -1:
				log_error(memoria_logger, "Se perdio la conexion con KERNEL!");
				continuar = 0;
				break;
			default:
				log_warning(memoria_logger, "MEMORIA: Operacion desconocida recibida de KERNEL");
				break;
		}
	}
}