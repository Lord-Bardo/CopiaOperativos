#include "../include/kernel_cpu_dispatch.h"

void atender_kernel_cpu_dispatch(){
    int continuar = 1;
	while( continuar ){
		int cod_op = recibir_operacion(fd_cpu_dispatch);
		switch(cod_op){
			// case :
			// 	// ..
			// 	break;
			case -1:
				log_error(kernel_logger, "Se perdio la conexion con CPU DISPATCH!");
				continuar = 0;
				break;
			default:
				log_warning(kernel_logger, "KERNEL: Operacion desconocida recibida de CPU DISPATCH");
				break;
		}
	}
}