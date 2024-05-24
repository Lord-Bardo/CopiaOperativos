#include "../include/memoria_kernel.h"

void atender_memoria_kernel(){
    int continuar = 1;
	while( continuar ){
		int cod_op = recibir_operacion(fd_kernel);
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
				log_error(memoria_logger, "Se perdio la conexion con KERNEL!");
				continuar = 0;
				break;
			default:
				log_warning(memoria_logger, "MEMORIA: Operacion desconocida recibida de KERNEL");
				break;
		}
	}
}