#include "../include/cpu_memoria.h"

void atender_cpu_memoria(){
    int continuar = 1;
	while( continuar ){
		int tipo_de_paquete = recibir_operacion(fd_memoria);
		switch(tipo_de_paquete ){
			case MENSAJE_OK:
				// ...
				break;
			case MENSAJE_FLAW: //NO SE A QUE SE REFIERE CON FLAW
				

				break;
			case MENSAJE_LISTO:
				// ...
				break;
			case INSTRUCCION: // ... aca hay que recibir bb recive_buffer size instruccion
				t_list * valores = recibir_paquete(fd_memoria); //creo que hay error con el tamaño igual
				t_instruccion *instruccion_a_ejecutar;
				instruccion_a_ejecutar.cod_op = valores->head->data;
				//sacar head de la lista y guardarlo en instruccion.parametros ?
				//tengo que leer el void * del buffer de a poquto con mallocs y crear mi instruccion?


				ejecutar_instruccion(instruccion_a_ejecutar);
				//falta hacer que se elimine la instruccion y liberar esa memoria
				break;
			case PCB:
				// ..
				break;
			case -1:
				log_error(cpu_logger, "Se perdio la conexion con MEMORIA!");
				continuar = 0;
				break;
			default:
				log_warning(cpu_logger, "CPU: Operacion desconocida recibida de MEMORIA");
				break;
		}
	}
}
/*
SERIALIZED,             // 0
    MESSAGE_OK,             // 1
    MESSAGE_FLAW,           // 2
    MESSAGE_DONE,           // 3
    INSTRUCTIONS,           // 4
    EXECUTION_CONTEXT       // 5
*/