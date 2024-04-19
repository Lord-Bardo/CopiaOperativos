#include "../include/entradasalida_kernel.h"

void atender_entradasalida_kernel(){
    int continuar = 1;
	while( continuar ){
		int cod_op = recibir_operacion(fd_kernel);
		switch(cod_op){
			case MENSAJE:
				// ...
				break;
			case PAQUETE:
				// ...
				break;
			case -1:
				log_error(entradasalida_logger, "Se perdio la conexion con KERNEL!");
				continuar = 0;
				break;
			default:
				log_warning(entradasalida_logger, "ENTRADASALIDA: Operacion desconocida recibida de KERNEL");
				break;
		}
	}
}