#include "../include/kernel_memoria.h"

void atender_kernel_memoria(){
    int continuar = 1;
	while( continuar ){
		int cod_op = recibir_operacion(fd_memoria);
		switch(cod_op){
			case MENSAJE_OK:
				// ...
				break;
			case MENSAJE_FLAW: //NO SE A QUE SE REFIERE CON FLAW
				

				break;
			case MENSAJE_LISTO:
				// ...
				break;
			case INSTRUCCION: // ... aca hay que recibir bb recive_buffer size instruccion
				
				break;
			case PCB:
				// ..
				break;
			case -1:
				log_error(kernel_logger, "Se perdio la conexion con MEMORIA!");
				continuar = 0;
				break;
			default:
				log_warning(kernel_logger, "KERNEL: Operacion desconocida recibida de MEMORIA");
				break;
		}
	}
}