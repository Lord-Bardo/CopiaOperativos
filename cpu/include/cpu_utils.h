#ifndef CPU_UTILS_
#define CPU_UTILS_

#include "cpu_gestor.h"
#include "instrucciones.h"

void agregar_instruccion_paquete(t_paquete *paquete, char* instruccion);
void agregar_pcb_paquete(t_paquete *paquete, t_pcb *pcb);
//char* recibir_instruccion(int socket_cliente);





#endif