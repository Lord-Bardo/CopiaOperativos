#include "../include/kernel_cpu_interrupt.h"

void atender_kernel_cpu_dispatch(){
    int continuar = 1;
	while( continuar ){
		int cod_op = recibir_operacion(fd_cpu_interrupt);
		switch(cod_op){
			case MENSAJE:
				// ...
				break;
			case PAQUETE:
				// ...
				break;
			case -1:
				log_error(kernel_logger, "Se perdio la conexion con CPU!");
				continuar = 0;
				break;
			default:
				log_warning(kernel_logger, "KERNEL: Operacion desconocida recibida de CPU");
				break;
		}
	}
}