#include "../include/entradasalida_memoria.h"

void atender_entradasalida_memoria(){
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
				log_error(entradasalida_logger, "Se perdio la conexion con MEMORIA!");
				continuar = 0;
				break;
			default:
				log_warning(entradasalida_logger, "Entradasalida: Operacion desconocida recibida de MEMORIA");
				break;
		}
	}
}