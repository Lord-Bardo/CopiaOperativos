#ifndef CPU_MEMORIA_H_
#define CPU_MEMORIA_H_

#include "cpu_gestor.h"
#include "instrucciones.h"
#include "procesos.h"

void atender_cpu_memoria();
void iniciar_ciclo_instruccion(t_pcb);
void buffer_desempaquetar_registros(t_buffer *buffer, t_registros *registros);
int pcb_get_pid(t_pcb *pcb);
void buffer_desempaquetar_pcb(t_buffer *buffer, t_pcb* pcb);


#endif