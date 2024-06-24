#include "../include/memoria_entradasalida.h"

void atender_memoria_entradasalida(){
    int continuar = 1;
	while( continuar ){
		int cod_op = 1; // hay que hacerlo bien con recibir_codigo_operacion. 
		switch(cod_op){
		
			case -1:
				log_error(memoria_logger, "Se perdio la conexion con ENTRADASALIDA!");
				continuar = 0;
				break;
			default:
				log_warning(memoria_logger, "MEMORIA: Operacion desconocida recibida de ENTRADASALIDA");
				break;
		}
	}
}