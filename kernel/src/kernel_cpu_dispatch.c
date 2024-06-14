#include "../include/kernel_cpu_dispatch.h"

void atender_kernel_cpu_dispatch(){
    int continuar = 1;
	while( continuar ){
		t_codigo_operacion cod_op;
		recibir_codigo_operacion(fd_cpu_dispatch, &cod_op); // REVISAR SI LO HAGO ASI O NO
		switch(cod_op){
			// case :
			// 	// ..
			// 	break;
			// case -1:
			// 	log_error(kernel_logger, "Se perdio la conexion con CPU DISPATCH!");
			// 	continuar = 0;
			// 	break;
			default:
				log_warning(kernel_logger, "KERNEL: Operacion desconocida recibida de CPU DISPATCH");
				break;
		}
	}
}