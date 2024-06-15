
#include "../include/cpu_utils.h"


/*void recibir_pcb(int socket_cliente){
	t_list *pcb_como_lista = recibir_paquete(socket_cliente);
	t_pcb pcb;
	
}*/
void agregar_instruccion_paquete(t_paquete *paquete, char* instruccion){
	agregar_a_paquete(paquete, instruccion, string_length(instruccion));
}

void agregar_contexto_ejecucion_a_paquete(t_paquete *paquete, t_pcb* pcb){
    agregar_pid_a_paquete(paquete, pcb_get_pid(pcb));
    agregar_registros_a_paquete(paquete, pcb_get_registros(pcb));
}
void agregar_uint32_a_paquete(t_paquete *paquete, uint32_t n){
    agregar_a_paquete(paquete, &n, sizeof(uint32_t));
}

void agregar_uint8_a_paquete(t_paquete *paquete, uint8_t n){
    agregar_a_paquete(paquete, &n, sizeof(uint8_t));
}

void agregar_registros_a_paquete(t_paquete *paquete, t_registros registros){
    agregar_uint32_a_paquete(paquete, registros.PC);
    agregar_uint8_a_paquete(paquete, registros.AX);
    agregar_uint8_a_paquete(paquete, registros.BX);
    agregar_uint8_a_paquete(paquete, registros.CX);
    agregar_uint8_a_paquete(paquete, registros.DX);
    agregar_uint32_a_paquete(paquete, registros.EAX);
    agregar_uint32_a_paquete(paquete, registros.EBX);
    agregar_uint32_a_paquete(paquete, registros.ECX);
    agregar_uint32_a_paquete(paquete, registros.EDX);
    agregar_uint32_a_paquete(paquete, registros.SI);
    agregar_uint32_a_paquete(paquete, registros.DI);
}
void agregar_pid_a_paquete(t_paquete *paquete, int pid){
    agregar_a_paquete(paquete, &pid, sizeof(pid));
}

/*char* recibir_instruccion(int socket_cliente) //no se quien hizo esto pero tiene que retornar un char* y no retorna nada
{
	t_list *recibir_instruccion = recibir_paquete(socket_cliente);
	char* instruccion_completa;
	strcpy (instruccion_completa, recibir_instruccion->head->data);
	char **instruccion_parciada = string_split(instruccion_completa, " ");
	ejecutar_instruccion(instruccion_parciada); //no se llama asi me parece
}*/

