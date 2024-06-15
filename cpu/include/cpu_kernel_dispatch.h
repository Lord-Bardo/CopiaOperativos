#ifndef CPU_KERNEL_DISPATCH_H_
#define CPU_KERNEL_DISPATCH_H_

#include "cpu_gestor.h"
#include "instrucciones.h"
#include "procesos.h"
#include "cpu_utils.h"

void atender_cpu_kernel_dispatch(void);
void iniciar_ciclo_instruccion(t_pcb);
void buffer_desempaquetar_registros(t_buffer *buffer, t_registros *registros);
int pcb_get_pid(t_pcb *pcb);
void buffer_desempaquetar_pcb(t_buffer *buffer, t_pcb* pcb);

#endif