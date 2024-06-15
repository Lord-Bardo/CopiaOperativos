
#include "../include/cpu_utils.h"


/*void recibir_pcb(int socket_cliente){
	t_list *pcb_como_lista = recibir_paquete(socket_cliente);
	t_pcb pcb;
	
}*/
void agregar_instruccion_paquete(t_paquete *paquete, char* instruccion)
{
	agregar_a_paquete(paquete, instruccion, string_length(instruccion));
}

void agregar_pcb_paquete(t_paquete *paquete, t_pcb *pcb) {
	agregar_a_paquete(paquete, &(pcb->pid), sizeof(int));
	agregar_a_paquete(paquete, &(pcb->registros.pc), 8);
	agregar_a_paquete(paquete, &(pcb->registros.ax), 2);
	agregar_a_paquete(paquete, &(pcb->registros.bx), 2);
	agregar_a_paquete(paquete, &(pcb->registros.cx), 2);
	agregar_a_paquete(paquete, &(pcb->registros.dx), 2);
	agregar_a_paquete(paquete, &(pcb->registros.eax), 8);
	agregar_a_paquete(paquete, &(pcb->registros.ebx), 8);
	agregar_a_paquete(paquete, &(pcb->registros.ecx), 8);
	agregar_a_paquete(paquete, &(pcb->registros.edx), 8);
	agregar_a_paquete(paquete, &(pcb->registros.si), 8);
	agregar_a_paquete(paquete, &(pcb->registros.di), 8);
}

/*char* recibir_instruccion(int socket_cliente) //no se quien hizo esto pero tiene que retornar un char* y no retorna nada
{
	t_list *recibir_instruccion = recibir_paquete(socket_cliente);
	char* instruccion_completa;
	strcpy (instruccion_completa, recibir_instruccion->head->data);
	char **instruccion_parciada = string_split(instruccion_completa, " ");
	ejecutar_instruccion(instruccion_parciada); //no se llama asi me parece
}*/

