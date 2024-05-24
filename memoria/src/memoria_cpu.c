#include "../include/memoria_cpu.h"
#include <utils/instrucciones.h>
#include <commons/collections/list.h>

t_link_element *head = {0,}
t_list argsSet = {head,2} //ax 10
t_instruccion hardcode = {0,argsSet};
void atender_memoria_cpu(){
    int continuar = 1;
	while( continuar ){
		int cod_op = recibir_operacion(fd_cpu);
		switch(cod_op){
			case MENSAJE:
				// ...
				break;
			case PAQUETE: //este es
				// si no hay quilombo con el paquete devolver la instrucicon

				break;
			case -1:
				log_error(memoria_logger, "Se perdio la conexion con CPU!");
				continuar = 0;
				break;
			default:
				log_warning(memoria_logger, "MEMORIA: Operacion desconocida recibida de CPU");
				break;
		}
	}
}