#include "../include/memoria_cpu.h"
#include <utils/instrucciones.h>
#include <commons/collections/list.h>
#include <utils/conexion.h>

 int nro_value = 10;
t_link_element nro = {&nro_value, NULL};

    // Inicialización de t_link_element 'ax' apuntando a 'nro'
int ax_value = 0;
t_link_element ax = {&ax_value, &nro};

    // Inicialización de t_list 'argsSet' con 'ax' como head y '2' como elements_count
t_list argsSet = {&ax, 2};

    // Inicialización de t_instruccion 'hardcode' con '0' como opcode y 'argsSet' como args
t_instruccion hardcode = {0, argsSet};//como el orto

void atender_memoria_cpu(){
    int continuar = 1;
	while( continuar ){
		int cod_op = recibir_operacion(fd_cpu);
		switch(cod_op){
			case FETCH: //no se si va aca?
				recibir_mensaje(fd_cpu)//aca deberia ser elmismo tipo de lo que nos mandaron
				t_paquete paquete_instrucicon = crear_paquete();
				agregar_a_paquete(paquete_instrucicon,hardcode, sizeof(t_instruccion));//me hace ruido el tamaño
				serializar_paquete_instruccion(paquete_instrucicon, sizeof(t_instruccion));
				// si no hay quilombo con el paquete devolver la instrucicon
				enviar_paquete(paquete_instruccion,fd_cpu);
			
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
				log_error(memoria_logger, "Se perdio la conexion con CPU!");
				continuar = 0;
				break;
			default:
				log_warning(memoria_logger, "MEMORIA: Operacion desconocida recibida de CPU");
				break;
		}
	}
}
/*
	
*/