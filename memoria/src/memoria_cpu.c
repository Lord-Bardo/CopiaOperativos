#include "../include/memoria_cpu.h"
#include <utils/instrucciones.h>
#include <commons/collections/list.h>

t_link_element nro = {10,NULL};
t_link_element ax = {0, nro};
t_list argsSet = {ax,2} //ax 10
t_instruccion hardcode = {0,argsSet};

void atender_memoria_cpu(){
    int continuar = 1;
	while( continuar ){
		int cod_op = recibir_operacion(fd_cpu);
		switch(cod_op){
			case MENSAJE:
				// ...
				break;
			case PAQUETE: //ESTO ES DE PRUEBA Y ESTA ALTAMENTE HARDCODEADO, ES PARA COMPROBAR SI ANDA LO QUE HICIMOS EN CPU
				t_paquete paquete_instrucicon = crear_paquete();
				agregar_a_paquete(paquete_instrucicon,hardcode, sizeof(t_instruccion));//me hace ruido el tamaño
				serializar_paquete_instruccion(paquete_instrucicon, sizeof(t_instruccion));
				// si no hay quilombo con el paquete devolver la instrucicon
				enviar_paquete(paquete_instruccion,fd_cpu);


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